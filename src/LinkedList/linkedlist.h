#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h> // for size_t

// Node structure for a singly linked list storing generic data
typedef struct Node {
    void* data;
    size_t data_size;  // store how many bytes 'data' has
    struct Node* next;
} Node;

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
 * Remove the node at the beginning of the list and optionally copy out its data.
 *  - head: Address of the pointer to the head node
 *  - outData: Pointer to a buffer where data will be copied (can be NULL)
 *    NOTE: The size of this buffer should be at least as large as data_size in the node.
 * Returns: 1 if a node was removed, 0 if the list was empty
 */
int removeAtBeginning(Node** head, void* outData);

/**
 * Print all elements in the linked list.
 *  - head: Pointer to the head of the list
 *  - printFunc: Function pointer for printing a single node's data
 *               typedef:  void (*PrintFunc)(const void* data);
 */
typedef void (*PrintFunc)(const void* data);
void printList(const Node* head, PrintFunc printFunc);

/**
 * Free all nodes in the linked list, including their data.
 *  - head: Address of the pointer to the head node
 */
void freeList(Node** head);

#endif // LINKEDLIST_H
