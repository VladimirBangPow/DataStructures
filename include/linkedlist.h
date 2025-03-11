#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>

// Forward declaration of Node
typedef struct Node {
    void* data;
    size_t data_size;
    struct Node* next;
} Node;

// Function pointer type for comparing two data items.
// Should return:
//   < 0 if a < b
//     0 if a == b
//   > 0 if a > b
typedef int (*CompareFunc)(const void* a, const void* b);

// Function pointer type for printing a single data item
typedef void (*PrintFunc)(const void* data);

/**
 * @brief Creates a new node with the given data.
 *
 * @param data Pointer to data to be stored in the new node.
 * @param data_size Size (in bytes) of the data to be stored.
 * @return Pointer to the newly created Node.
 */
Node* createNode(const void* data, size_t data_size);

/**
 * @brief Inserts a new node at the beginning of the list.
 *
 * @param head Pointer to the head pointer of the list.
 * @param data Pointer to the data to insert.
 * @param data_size Size (in bytes) of the data to be stored.
 */
void insertAtBeginning(Node** head, const void* data, size_t data_size);

/**
 * @brief Inserts a new node at the end of the list.
 *
 * @param head Pointer to the head pointer of the list.
 * @param data Pointer to the data to insert.
 * @param data_size Size (in bytes) of the data to be stored.
 */
void insertAtEnd(Node** head, const void* data, size_t data_size);

/**
 * @brief Removes a node from the beginning of the list.
 *
 * @param head Pointer to the head pointer of the list.
 * @param outData If not NULL, the removed node's data is copied to this pointer.
 * @return 1 if removal was successful, 0 if the list is empty.
 */
int removeAtBeginning(Node** head, void* outData);

/**
 * @brief Prints the entire list using the provided print function.
 *
 * @param head Pointer to the head of the list.
 * @param printFunc Function to print a single node's data.
 */
void printList(const Node* head, PrintFunc printFunc);

/**
 * @brief Frees the entire list, including all node data.
 *
 * @param head Pointer to the head pointer of the list.
 */
void freeList(Node** head);

/* ========================================================================= */
/* New functions for searching, inserting, removing at a position, and sorting */
/* ========================================================================= */

/**
 * @brief Searches the list for the first node matching `key` using `compare`.
 *
 * @param head Pointer to the head of the list.
 * @param key Pointer to the data to search for.
 * @param compare Comparison function to determine equality.
 * @return Pointer to the first node that matches `key`, or NULL if not found.
 */
Node* search(Node* head, const void* key, CompareFunc compare);

/**
 * @brief Inserts a new node at the specified 0-based position in the list.
 *
 * @param head Pointer to the head pointer of the list.
 * @param data Pointer to the data to insert.
 * @param data_size Size (in bytes) of the data to store in the new node.
 * @param position The 0-based index where the node should be inserted.
 * @return 1 if insertion was successful, 0 if position was invalid.
 */
int insertAtPosition(Node** head, const void* data, size_t data_size, int position);

/**
 * @brief Removes a node at the specified 0-based position in the list.
 *
 * @param head Pointer to the head pointer of the list.
 * @param position The 0-based index of the node to remove.
 * @param outData If not NULL, the removed node's data is copied to this pointer.
 * @return 1 if removal was successful, 0 if position was invalid or list is empty.
 */
int removeAtPosition(Node** head, int position, void* outData);

/**
 * @brief Sorts the linked list in ascending order according to `compare`.
 *
 * @param head Pointer to the head pointer of the list.
 * @param compare Comparison function that defines the sorting order.
 */
void sortList(Node** head, CompareFunc compare);


void insertInSortedOrder(Node** head, const void* data, size_t data_size, CompareFunc cmp);

int removeValue(Node** head, const void* data, CompareFunc cmp, void* outData);

#endif // LINKEDLIST_H
