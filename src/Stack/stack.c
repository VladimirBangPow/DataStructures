#include <string.h>  // for memcpy
#include "stack.h"
#include "../LinkedList/linkedlist.h"

// Initializes the stack to be empty
void stackInit(Stack* stack) {
    stack->top = NULL;
}

// Push: insert at the beginning of the list
void stackPush(Stack* stack, const void* data, size_t data_size) {
    insertAtBeginning(&stack->top, data, data_size);
}

// Pop: remove at the beginning of the list
int stackPop(Stack* stack, void* outData) {
    return removeAtBeginning(&stack->top, outData);
}

// Peek: just copy from the top node without removing
int stackPeek(const Stack* stack, void* outData) {
    if (!stack->top) {
        return 0; // Stack empty
    }
    // Copy the top node's data
    memcpy(outData, stack->top->data, stack->top->data_size);
    return 1;
}

// Check if empty
int stackIsEmpty(const Stack* stack) {
    return (stack->top == NULL);
}

// Clear the entire stack
void stackClear(Stack* stack) {
    freeList(&stack->top);
}
