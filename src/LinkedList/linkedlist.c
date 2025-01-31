#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // for memcpy
#include "linkedlist.h"

Node* createNode(const void* data, size_t data_size) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed for new node.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for data and copy it
    newNode->data = malloc(data_size);
    if (!newNode->data) {
        fprintf(stderr, "Memory allocation failed for node data.\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }
    memcpy(newNode->data, data, data_size);

    newNode->data_size = data_size;
    newNode->next = NULL;

    return newNode;
}

void insertAtBeginning(Node** head, const void* data, size_t data_size) {
    Node* newNode = createNode(data, data_size);
    newNode->next = *head;  // new node points to the old head
    *head = newNode;        // new node becomes the head
}

void insertAtEnd(Node** head, const void* data, size_t data_size) {
    Node* newNode = createNode(data, data_size);

    if (*head == NULL) {
        // if the list is empty, newNode becomes the head
        *head = newNode;
        return;
    }

    // otherwise, traverse to the end
    Node* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

int removeAtBeginning(Node** head, void* outData) {
    if (*head == NULL) {
        return 0; // List is empty, nothing to remove
    }

    Node* toRemove = *head;
    // If outData is not NULL, copy the data out before freeing
    if (outData) {
        memcpy(outData, toRemove->data, toRemove->data_size);
    }

    // Advance the head pointer
    *head = toRemove->next;

    // Free node and its internal data
    free(toRemove->data);
    free(toRemove);

    return 1; // Successfully removed
}

void printList(const Node* head, PrintFunc printFunc) {
    const Node* current = head;
    while (current != NULL) {
        printFunc(current->data);
        current = current->next;
    }
    printf("NULL\n");
}

void freeList(Node** head) {
    Node* current = *head;
    while (current != NULL) {
        Node* nextNode = current->next;
        // free the data and the node itself
        free(current->data);
        free(current);
        current = nextNode;
    }
    *head = NULL;
}


/****************************/
/* 1. Search Function       */
/****************************/
Node* search(Node* head, const void* key, CompareFunc compare) {
    Node* current = head;
    while (current != NULL) {
        if (compare(current->data, key) == 0) {
            return current;  // Found a match
        }
        current = current->next;
    }
    return NULL; // No match
}

/*********************************/
/* 2. Insert at Position         */
/*********************************/
int insertAtPosition(Node** head, const void* data, size_t data_size, int position) {
    if (position < 0) {
        return 0;
    }

    // Insert at beginning
    if (position == 0) {
        insertAtBeginning(head, data, data_size);
        return 1;
    }

    Node* current = *head;
    for (int i = 0; i < position - 1; i++) {
        if (current == NULL) {
            return 0;
        }
        current = current->next;
    }

    if (current == NULL) {
        return 0;
    }

    Node* newNode = createNode(data, data_size);
    newNode->next = current->next;
    current->next = newNode;
    return 1;
}

/*********************************/
/* 3. Remove at Position         */
/*********************************/
int removeAtPosition(Node** head, int position, void* outData) {
    if (*head == NULL || position < 0) {
        return 0;
    }

    // Remove at beginning
    if (position == 0) {
        return removeAtBeginning(head, outData);
    }

    Node* current = *head;
    for (int i = 0; i < position - 1; i++) {
        if (current->next == NULL) {
            return 0;
        }
        current = current->next;
    }

    Node* toRemove = current->next;
    if (!toRemove) {
        return 0;
    }

    if (outData != NULL) {
        memcpy(outData, toRemove->data, toRemove->data_size);
    }

    current->next = toRemove->next;
    free(toRemove->data);
    free(toRemove);
    return 1;
}

/*********************************/
/* 4. Sort (Merge Sort)          */
/*********************************/
static void splitList(Node* source, Node** frontRef, Node** backRef) {
    Node* slow = source;
    Node* fast = source->next;

    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

static Node* sortedMerge(Node* a, Node* b, CompareFunc compare) {
    if (!a) return b;
    if (!b) return a;

    Node* result = NULL;
    if (compare(a->data, b->data) <= 0) {
        result = a;
        result->next = sortedMerge(a->next, b, compare);
    } else {
        result = b;
        result->next = sortedMerge(a, b->next, compare);
    }
    return result;
}

static Node* mergeSort(Node* head, CompareFunc compare) {
    if (!head || !head->next) {
        return head;
    }

    Node* a;
    Node* b;
    splitList(head, &a, &b);

    a = mergeSort(a, compare);
    b = mergeSort(b, compare);

    return sortedMerge(a, b, compare);
}

void sortList(Node** head, CompareFunc compare) {
    if (*head == NULL || (*head)->next == NULL) {
        return; // Nothing to sort
    }
    *head = mergeSort(*head, compare);
}
