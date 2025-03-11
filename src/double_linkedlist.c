#include <stdio.h>      // for printf, fprintf
#include <stdlib.h>     // for malloc, free, exit, EXIT_FAILURE
#include <string.h>     // for memcpy
#include "double_linkedlist.h"

/**
 * Initializes a DoubleLinkedList structure.
 * Sets both head and tail pointers to NULL (an empty list).
 */
void dllInit(DoubleLinkedList* list) {
    // When the list is empty, both head and tail are NULL
    list->head = list->tail = NULL;
}

/**
 * Helper function to create a new doubly-linked-list node (DNode).
 * 1. Allocates memory for the node.
 * 2. Allocates memory for (and copies) the data.
 * 3. Stores the data size.
 * 4. Initializes prev and next pointers to NULL.
 *
 * @param data       A pointer to the data to store in this node.
 * @param data_size  Size (in bytes) of the data to copy.
 * @return           A pointer to the newly created DNode.
 */
static DNode* createDNode(const void* data, size_t data_size) {
    // 1. Allocate memory for the new node
    DNode* newNode = (DNode*)malloc(sizeof(DNode));
    if (!newNode) {
        fprintf(stderr, "Failed to allocate memory for new DNode.\n");
        exit(EXIT_FAILURE);
    }

    // 2. Allocate memory for the data and copy it into the newly allocated space
    newNode->data = malloc(data_size);
    if (!newNode->data) {
        fprintf(stderr, "Failed to allocate memory for node->data.\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }
    memcpy(newNode->data, data, data_size);

    // 3. Store the size of the data (useful for copying out or freeing)
    newNode->data_size = data_size;

    // 4. Initialize 'prev' and 'next' pointers to NULL
    newNode->prev = NULL;
    newNode->next = NULL;

    // Return a pointer to the newly created node
    return newNode;
}

/**
 * Inserts a new node at the front (head) of the doubly-linked list.
 *
 * @param list       The list where the node will be inserted.
 * @param data       Pointer to the data to store in the new node.
 * @param data_size  The number of bytes of data to copy into the new node.
 */
void dllInsertFront(DoubleLinkedList* list, const void* data, size_t data_size) {
    // Step 1: Create a new node using our helper function
    DNode* newNode = createDNode(data, data_size);

    // Step 2: If the list is empty, both head and tail become the new node
    if (list->head == NULL) {
        // The list was empty, so the new node is now the only node
        list->head = newNode;
        list->tail = newNode;
    } else {
        // The list is not empty

        // Step 3: Set the new node's next pointer to the current head
        newNode->next = list->head;

        // Step 4: The old head's prev pointer now should point back to the new node
        list->head->prev = newNode;

        // Step 5: Finally, update the list's head pointer to the new node
        list->head = newNode;
    }
}

/**
 * Inserts a new node at the back (tail) of the doubly-linked list.
 *
 * @param list       The list where the node will be inserted.
 * @param data       Pointer to the data to store in the new node.
 * @param data_size  The number of bytes of data to copy into the new node.
 */
void dllInsertBack(DoubleLinkedList* list, const void* data, size_t data_size) {
    // Create the new node using the helper function
    DNode* newNode = createDNode(data, data_size);

    // If the list is empty (tail == NULL), new node becomes head and tail
    if (list->tail == NULL) {
        list->head = list->tail = newNode;
    } else {
        // Otherwise, link the new node in at the back
        newNode->prev = list->tail;    // newNode's 'prev' points to the old tail
        list->tail->next = newNode;    // the old tail's 'next' now points to the new node
        list->tail = newNode;          // update the 'tail' pointer to the new node
    }
}

/**
 * Removes a node from the front (head) of the list.
 * 
 * @param list    The list from which to remove the node.
 * @param outData If non-NULL, the data from the removed node will be copied here.
 * @return        1 if a node was removed, or 0 if the list was empty.
 */
int dllRemoveFront(DoubleLinkedList* list, void* outData) {
    // If the list is empty (no head), we cannot remove anything
    if (!list->head) {
        return 0; // indicate failure/no removal
    }

    // Temporary pointer to the node being removed
    DNode* temp = list->head;

    // Copy out the node's data if the user requested it
    if (outData) {
        memcpy(outData, temp->data, temp->data_size);
    }

    // If head == tail, it means there's only one node in the list
    if (list->head == list->tail) {
        list->head = NULL; // list becomes empty
        list->tail = NULL;
    } else {
        // More than one node in the list:
        list->head = temp->next;     // move the head pointer to the next node
        list->head->prev = NULL;     // new head's prev must be NULL
    }

    // Free the node's data buffer, then free the node itself
    free(temp->data);
    free(temp);

    return 1; // success
}

/**
 * Removes a node from the back (tail) of the list.
 *
 * @param list    The list from which to remove the node.
 * @param outData If non-NULL, the data from the removed node will be copied here.
 * @return        1 if a node was removed, or 0 if the list was empty.
 */
int dllRemoveBack(DoubleLinkedList* list, void* outData) {
    // If the list is empty (tail == NULL), no removal is possible
    if (!list->tail) {
        return 0;
    }

    // Temporary pointer to the node being removed
    DNode* temp = list->tail;

    // Copy out data if requested
    if (outData) {
        memcpy(outData, temp->data, temp->data_size);
    }

    // If head == tail, there's only one node
    if (list->head == list->tail) {
        list->head = NULL;
        list->tail = NULL;
    } else {
        // Otherwise, we have multiple nodes
        list->tail = temp->prev;    // move tail pointer to the previous node
        list->tail->next = NULL;    // new tail's next must be NULL
    }

    // Free the data buffer, then free the node
    free(temp->data);
    free(temp);

    return 1; // success
}

/**
 * Prints the contents of the list by traversing from head to tail.
 * The user supplies a function pointer (printFunc) to print each node's data.
 *
 * @param list      The list to print.
 * @param printFunc A function pointer that knows how to print the node's data.
 */
void dllPrint(const DoubleLinkedList* list, void (*printFunc)(const void*)) {
    // Start from the head, move through each node
    for (DNode* temp = list->head; temp; temp = temp->next) {
        // Call the user-provided function to print this node's data
        printFunc(temp->data);
    }
    // Print a visual terminator or indicator
    printf("NULL\n");
}

/**
 * Frees the entire list. Continuously removes nodes from the front until
 * the list is empty.
 *
 * @param list The list to free.
 */
void dllFree(DoubleLinkedList* list) {
    // dllRemoveFront(...) removes a node each time until the list is empty.
    // We keep removing until dllRemoveFront returns 0 (empty list).
    while (dllRemoveFront(list, NULL)) {
        // No additional work needed here, loop does the cleanup
    }
}
