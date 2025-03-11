#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include "linkedlist.h"

/**
 * @brief A simple stack implemented on top of a singly linked list.
 *        The top of the stack corresponds to the head of the linked list.
 */
typedef struct {
    Node* top;  // points to the top of the stack
} Stack;

/**
 * @brief Initializes the stack to be empty.
 */
void stackInit(Stack* stack);

/**
 * @brief Pushes a copy of the given data onto the stack.
 *        Internally inserts at the beginning of the list.
 *
 * @param stack Pointer to the stack
 * @param data Pointer to the data to be pushed
 * @param data_size Size of the data in bytes
 */
void stackPush(Stack* stack, const void* data, size_t data_size);

/**
 * @brief Pops the top item off the stack (removes from the beginning).
 *
 * @param stack Pointer to the stack
 * @param outData If not NULL, the removed item's data is copied here.
 * @return 1 if pop was successful, 0 if the stack is empty
 */
int stackPop(Stack* stack, void* outData);

/**
 * @brief Peeks at the top item without removing it.
 *
 * @param stack Pointer to the stack
 * @param outData If not NULL, the top item's data is copied here.
 * @return 1 if peek was successful, 0 if the stack is empty
 */
int stackPeek(const Stack* stack, void* outData);

/**
 * @brief Checks if the stack is empty.
 *
 * @param stack Pointer to the stack
 * @return 1 if empty, 0 otherwise
 */
int stackIsEmpty(const Stack* stack);

/**
 * @brief Clears the entire stack by freeing all elements.
 *
 * @param stack Pointer to the stack
 */
void stackClear(Stack* stack);

/* ----------------------------------------------------------------------------
   Printing Enhancements
   ----------------------------------------------------------------------------*/

/**
 * @brief A user-supplied function that converts `data` into a text string.
 *        The result must fit within outBufferSize, including the null terminator.
 */
typedef void (*StackFormatFunc)(const void* data, char* outBuffer, size_t outBufferSize);

/**
 * @brief Prints items in the stack from [startIndex..endIndex], top = index 0,
 *        in a vertical ASCII-art style. The column width is the length of the
 *        longest formatted item, but clamped by maxWidth if maxWidth > 0.
 *
 * @param stack     The stack to print
 * @param startIndex 0-based index of the top item to start printing from
 * @param endIndex   0-based index of the last item to print (inclusive)
 * @param maxWidth   If <= 0, ignore width clamping; otherwise, clamp to this width
 * @param formatter  Function pointer for formatting the data
 */
void printStackRange(const Stack* stack,
                     int startIndex,
                     int endIndex,
                     int maxWidth,
                     StackFormatFunc formatter);

/**
 * @brief Prints the top n items of the stack using `printStackRange`.
 *
 * @param stack    The stack to print
 * @param n        Number of items from the top
 * @param maxWidth Clamps item width if > 0
 * @param formatter Function pointer for formatting the data
 */
void printHead(const Stack* stack, int n, int maxWidth, StackFormatFunc formatter);

/**
 * @brief Prints the bottom n items of the stack using `printStackRange`.
 *
 * @param stack    The stack to print
 * @param n        Number of items from the bottom
 * @param maxWidth Clamps item width if > 0
 * @param formatter Function pointer for formatting the data
 */
void printTail(const Stack* stack, int n, int maxWidth, StackFormatFunc formatter);

#endif // STACK_H

