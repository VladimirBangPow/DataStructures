#ifndef CIRCULAR_LIST_H
#define CIRCULAR_LIST_H

#include <stddef.h> // for size_t

// A singly circular linked list node
typedef struct CNode {
    void* data;
    size_t data_size;
    struct CNode* next;
} CNode;

typedef struct {
    CNode* tail; // We'll keep a tail pointer, so tail->next = head
} CircularList;

/**
 * Initialize an empty circular list.
 */
void clistInit(CircularList* list);

/**
 * Insert a new node at the front (head) of the list.
 * (O(1) if we keep a tail pointer)
 */
void clistInsertFront(CircularList* list, const void* data, size_t data_size);

/**
 * Insert a new node at the back (tail) of the list.
 */
void clistInsertBack(CircularList* list, const void* data, size_t data_size);

/**
 * Remove the front node from the list.
 *  - If outData is non-null, copies the removed node's data to outData.
 *  - Returns 1 if successful, 0 if empty list.
 */
int clistRemoveFront(CircularList* list, void* outData);

/**
 * Remove the back node from the list.
 *  - If outData is non-null, copies the removed node's data to outData.
 *  - Returns 1 if successful, 0 if empty list.
 */
int clistRemoveBack(CircularList* list, void* outData);

/**
 * Print all elements in the circular list once.
 *  - printFunc is a function pointer to print a single node's data.
 */
typedef void (*CPrintFunc)(const void*);
void clistPrint(const CircularList* list, CPrintFunc printFunc);

/**
 * Free all nodes in the circular list.
 */
void clistFree(CircularList* list);

#endif // CIRCULAR_LIST_H
