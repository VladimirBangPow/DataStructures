#ifndef DEQUE_H
#define DEQUE_H

#include <stddef.h>  // for size_t

// A doubly linked list node
typedef struct DequeNode {
    void* data;
    size_t data_size;
    struct DequeNode* prev;
    struct DequeNode* next;
} DequeNode;

// The Deque structure
typedef struct {
    DequeNode* front;
    DequeNode* back;
} Deque;


void dequePrint(const Deque* dq, void (*printFunc)(const void*));


/**
 * Initialize an empty deque.
 */
void dequeInit(Deque* dq);

/**
 * Insert data at the front of the deque.
 */
void dequePushFront(Deque* dq, const void* data, size_t data_size);

/**
 * Insert data at the back of the deque.
 */
void dequePushBack(Deque* dq, const void* data, size_t data_size);

/**
 * Remove an element from the front of the deque.
 * - If `outData` is non-null, copies the removed data into `outData`.
 * - Returns 1 if successful, 0 if deque is empty.
 */
int dequePopFront(Deque* dq, void* outData);

/**
 * Remove an element from the back of the deque.
 * - If `outData` is non-null, copies the removed data into `outData`.
 * - Returns 1 if successful, 0 if deque is empty.
 */
int dequePopBack(Deque* dq, void* outData);

/**
 * Check if the deque is empty.
 * - Returns 1 if empty, 0 otherwise.
 */
int dequeIsEmpty(const Deque* dq);

/**
 * Clear all elements from the deque, freeing memory.
 */
void dequeClear(Deque* dq);

#endif // DEQUE_H
