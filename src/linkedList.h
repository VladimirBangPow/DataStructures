#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h> // for size_t

// Node structure for a singly linked list storing generic data
typedef struct Node {
    void* data;
    struct Node* next;
} Node;

/**
 * Function pointer type for printing node data.
 * The user-provided function should cast data to the correct type and print it.
 */
typedef void (*PrintFunc)(const void* data);

/**
 * Create a new node which stores a copy of the data.
 *  - data: Pointer to the data to be stored
 *  - data_size: Number of bytes of the data
 * Returns: Pointer to the newly created node
 */
Node* createNode(const void* data, size_t data_size);

/**
 * Insert a new node at the beginning of the linked list.
 *  - head: Address of the pointer to the head node
 *  - data: Pointer to the data to be stored
 *  - data_size: Number of bytes of the data
 */
void insertAtBeginning(Node** head, const void* data, size_t data_size);

/**
 * Insert a new node at the end of the linked list.
 *  - head: Address of the pointer to the head node
 *  - data: Pointer to the data to be stored
 *  - data_size: Number of bytes of the data
 */
void insertAtEnd(Node** head, const void* data, size_t data_size);

/**
 * Print all elements in the linked list.
 *  - head: Pointer to the head of the list
 *  - printFunc: Function pointer for printing a single node's data
 */
void printList(const Node* head, PrintFunc printFunc);

/**
 * Free all nodes in the linked list, including their data.
 *  - head: Address of the pointer to the head node
 *
 * Note: Since createNode() allocates memory for the data,
 *       freeList() will automatically free it as well.
 */
void freeList(Node** head);

#endif // LINKEDLIST_H
