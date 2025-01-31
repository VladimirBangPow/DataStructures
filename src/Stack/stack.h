#ifndef STACK_H
#define STACK_H

#include <stddef.h>  // for size_t
#include "../LinkedList/linkedlist.h"

// A simple Stack structure that uses a linked list internally
typedef struct {
    Node* top; // The top of the stack
} Stack;

/**
 * Initializes the stack (empty).
 */
void stackInit(Stack* stack);

/**
 * Pushes a new element on top of the stack.
 *  - stack: pointer to the Stack
 *  - data: pointer to the element to be stored
 *  - data_size: size in bytes of the element
 */
void stackPush(Stack* stack, const void* data, size_t data_size);

/**
 * Pops the top element from the stack.
 *  - stack: pointer to the Stack
 *  - outData: pointer to a buffer where the popped data will be copied
 * Returns: 1 if pop succeeded, 0 if the stack was empty
 */
int stackPop(Stack* stack, void* outData);

/**
 * Peeks at the top element without removing it.
 *  - stack: pointer to the Stack
 *  - outData: pointer to a buffer where the top data will be copied
 * Returns: 1 if the stack is not empty, 0 otherwise
 */
int stackPeek(const Stack* stack, void* outData);

/**
 * Checks if the stack is empty.
 * Returns: 1 if empty, 0 if not empty
 */
int stackIsEmpty(const Stack* stack);

/**
 * Clears the entire stack (frees memory).
 * The stack is empty afterward.
 */
void stackClear(Stack* stack);

#endif // STACK_H
