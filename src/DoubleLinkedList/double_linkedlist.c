#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "double_linkedlist.h"

void dllInit(DoubleLinkedList* list) {
    list->head = list->tail = NULL;
}

static DNode* createDNode(const void* data, size_t data_size) {
    // 1. Allocate memory for the new node
    DNode* newNode = (DNode*)malloc(sizeof(DNode));
    if (!newNode) {
        fprintf(stderr, "Failed to allocate memory for new DNode.\n");
        exit(EXIT_FAILURE);
    }

    // 2. Allocate memory for the data and copy it
    newNode->data = malloc(data_size);
    if (!newNode->data) {
        fprintf(stderr, "Failed to allocate memory for node->data.\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }
    memcpy(newNode->data, data, data_size);

    // 3. Store the size so we know how big the data is (for remove operations)
    newNode->data_size = data_size;

    // 4. Initialize prev and next
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}

void dllInsertFront(DoubleLinkedList* list, const void* data, size_t data_size) {
    // Step 1: Create a new node
    DNode* newNode = createDNode(data, data_size);
    // createDNode(...) should:
    //  - malloc the DNode itself
    //  - malloc for newNode->data
    //  - memcpy the user's data into newNode->data
    //  - set newNode->prev = newNode->next = NULL

    // Step 2: If the list is empty, head and tail both become newNode
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        // Step 3: The new node’s 'next' becomes the old head
        newNode->next = list->head;

        // Step 4: The old head’s 'prev' becomes the new node
        list->head->prev = newNode;

        // Step 5: Finally, update the list’s head pointer
        list->head = newNode;
    }
}


void dllInsertBack(DoubleLinkedList* list, const void* data, size_t data_size) {
    DNode* newNode = createDNode(data, data_size);
    if (list->tail == NULL) {
        list->head = list->tail = newNode;
    } else {
        newNode->prev = list->tail;
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

int dllRemoveFront(DoubleLinkedList* list, void* outData) {
if (!list->head) {
        // Empty list
        return 0;
    }

    DNode* temp = list->head;

    // Copy data if requested
    if (outData) {
        memcpy(outData, temp->data, temp->data_size);
    }

    if (list->head == list->tail) {
        // Single node case
        list->head = NULL;
        list->tail = NULL;
    } else {
        // Multiple nodes
        list->head = temp->next;
        list->head->prev = NULL;
    }

    free(temp->data);
    free(temp);

    return 1;
}

int dllRemoveBack(DoubleLinkedList* list, void* outData) {

	if (!list->tail) return 0; // empty

    DNode* temp = list->tail;
    if (outData) {
        memcpy(outData, temp->data, temp->data_size);
    }

    if (list->head == list->tail) {
        // Only one node
        list->head = NULL;
        list->tail = NULL;
    } else {
        // More than one
        list->tail = temp->prev;
        list->tail->next = NULL;
    }
    free(temp->data);
    free(temp);
    return 1;
}

void dllPrint(const DoubleLinkedList* list, void (*printFunc)(const void*)) {
    for (DNode* temp = list->head; temp; temp = temp->next) {
        printFunc(temp->data);
    }
    printf("NULL\n");
}

void dllFree(DoubleLinkedList* list) {
    while (dllRemoveFront(list, NULL)) {
        // keep removing from the front until empty
    }
}
