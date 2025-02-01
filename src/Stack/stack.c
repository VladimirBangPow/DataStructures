#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "../LinkedList/linkedlist.h"

/* ----------------------------------------------------------------------------
   Basic Stack Operations
   ----------------------------------------------------------------------------*/

void stackInit(Stack* stack) {
    stack->top = NULL;
}

void stackPush(Stack* stack, const void* data, size_t data_size) {
    insertAtBeginning(&stack->top, data, data_size);
}

int stackPop(Stack* stack, void* outData) {
    return removeAtBeginning(&stack->top, outData);
}

int stackPeek(const Stack* stack, void* outData) {
    if (!stack->top) {
        return 0; // Stack empty
    }
    // Copy the top node's data
    memcpy(outData, stack->top->data, stack->top->data_size);
    return 1;
}

int stackIsEmpty(const Stack* stack) {
    return (stack->top == NULL);
}

void stackClear(Stack* stack) {
    freeList(&stack->top);
}

/* ----------------------------------------------------------------------------
   Extended Printing Functionality
   ----------------------------------------------------------------------------*/

/**
 * @brief Utility to compute the size (number of nodes) in the stack
 */
static int stackSize(const Stack* stack) {
    int count = 0;
    for (Node* cur = stack->top; cur; cur = cur->next) {
        count++;
    }
    return count;
}

/**
 * @brief Prints the stack items from [startIndex..endIndex]. We do a two-pass approach:
 *        - Pass 1: Build an array of formatted strings & find the maximum length.
 *        - Pass 2: Print them with ASCII-art borders, clamped by maxWidth if > 0.
 */
void printStackRange(const Stack* stack,
                            int startIndex,
                            int endIndex,
                            int maxWidth,
                            StackFormatFunc formatter)
{
    if (!stack || !stack->top) {
        printf("(Stack is empty)\n");
        return;
    }
    if (!formatter) {
        printf("(No formatter function provided)\n");
        return;
    }
    // Clamp startIndex >= 0
    if (startIndex < 0) {
        startIndex = 0;
    }

    // Find total size
    int size = stackSize(stack);
    // Adjust endIndex if out of range
    if (endIndex >= size) {
        endIndex = size - 1;
    }
    // Check if valid range
    if (endIndex < 0 || startIndex > endIndex) {
        printf("(Invalid range or nothing to print)\n");
        return;
    }

    // -----------------------------------------------------
    // 1) FIRST PASS: find the maxLen
    // -----------------------------------------------------
    size_t maxLen = 0;
    {
        Node* cur = stack->top;
        int index = 0;
        const size_t LOCAL_BUF_SIZE = 512;
        char buffer[LOCAL_BUF_SIZE];

        while (cur && index <= endIndex) {
            if (index >= startIndex) {
                // Format the current node's data
                memset(buffer, 0, LOCAL_BUF_SIZE);
                formatter(cur->data, buffer, LOCAL_BUF_SIZE);

                size_t length = strlen(buffer);
                if (length > maxLen) {
                    maxLen = length;
                }
            }
            cur = cur->next;
            index++;
        }
    }

    // Clamp width if maxWidth > 0
    if (maxWidth > 0 && (int)maxLen > maxWidth) {
        maxLen = maxWidth;
    }
    // Ensure at least width=1
    if (maxLen == 0) {
        maxLen = 1;
    }

    // -----------------------------------------------------
    // 2) SECOND PASS: print each item with known maxLen
    // -----------------------------------------------------
    {
        Node* cur = stack->top;
        int index = 0;
        const size_t LOCAL_BUF_SIZE = 512;
        char buffer[LOCAL_BUF_SIZE];

        while (cur && index <= endIndex) {
            if (index >= startIndex) {
                // Format again
                memset(buffer, 0, LOCAL_BUF_SIZE);
                formatter(cur->data, buffer, LOCAL_BUF_SIZE);

                // Possibly embed ellipses if longer than maxLen
                size_t length = strlen(buffer);
                if (length > maxLen) {
                    buffer[maxLen - 3] = '\0'; // cut at maxLen-3
                    strcat(buffer, "...");
                    length = maxLen;
                }

                // Print top border
                printf("|");
                for (size_t w = 0; w < maxLen; w++) {
                    printf("=");
                }
                printf("|\n");

                // Print content
                printf("|%s", buffer);
                // Pad if shorter
                if (length < maxLen) {
                    for (size_t pad = length; pad < maxLen; pad++) {
                        printf(" ");
                    }
                }
                printf("|\n");

                // Bottom border
                printf("|");
                for (size_t w = 0; w < maxLen; w++) {
                    printf("=");
                }
                printf("|\n");
            }
            cur = cur->next;
            index++;
        }
    }
}
/**
 * @brief Prints the top `n` items (indexes [0..n-1]) using `printStackRange`.
 */
void printHead(const Stack* stack, int n, int maxWidth, StackFormatFunc formatter) {
    if (n <= 0) {
        printf("(Nothing to print, n <= 0)\n");
        return;
    }
    printStackRange(stack, 0, n - 1, maxWidth, formatter);
}

/**
 * @brief Prints the bottom `n` items (the last n items in the stack).
 *        If the stack size is s, we want [s-n .. s-1].
 */
void printTail(const Stack* stack, int n, int maxWidth, StackFormatFunc formatter) {
    if (n <= 0) {
        printf("(Nothing to print, n <= 0)\n");
        return;
    }
    int size = stackSize(stack);
    if (size == 0) {
        printf("(Stack is empty)\n");
        return;
    }
    if (n >= size) {
        // Print the entire stack
        printStackRange(stack, 0, size - 1, maxWidth, formatter);
    } else {
        // Print the last n items
        printStackRange(stack, size - n, size - 1, maxWidth, formatter);
    }
}
