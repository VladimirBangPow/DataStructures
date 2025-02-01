#ifndef DEQUE_H
#define DEQUE_H

#include <stddef.h>     // for size_t
#include <stdbool.h>    // for bool
#include "../DoubleLinkedList/double_linkedlist.h"

// For simplicity, we can typedef Deque as a DoubleLinkedList
typedef DoubleLinkedList Deque;

// Initialize the deque
void dequeInit(Deque* dq);

// Destroy the deque completely
void dequeDestroy(Deque* dq);

// Check if the deque is empty
bool dequeIsEmpty(const Deque* dq);

// Push an element to the front
void dequePushFront(Deque* dq, const void* data, size_t data_size);

// Push an element to the back
void dequePushBack(Deque* dq, const void* data, size_t data_size);

// Pop an element from the front
// Returns true if an element was removed, false if empty
bool dequePopFront(Deque* dq, void* outData);

// Pop an element from the back
// Returns true if an element was removed, false if empty
bool dequePopBack(Deque* dq, void* outData);

// Utility function to print the content of deque (given a user-defined print function)
void dequePrint(const Deque* dq, void (*printFunc)(const void*));

#endif // DEQUE_H
