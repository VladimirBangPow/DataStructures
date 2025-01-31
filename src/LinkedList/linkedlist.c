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
