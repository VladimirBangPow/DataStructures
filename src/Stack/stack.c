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
    if (startIndex < 0) startIndex = 0;

    int size = stackSize(stack);
    if (endIndex >= size) {
        endIndex = size - 1;
    }
    if (endIndex < 0 || startIndex > endIndex) {
        printf("(Invalid range or nothing to print)\n");
        return;
    }

    int rangeCount = endIndex - startIndex + 1;
    // We'll store each formatted item in a temporary array of char*
    char** items = (char**)malloc(rangeCount * sizeof(char*));
    if (!items) {
        printf("(Memory allocation failed)\n");
        return;
    }

    const size_t ITEM_BUFFER_SIZE = 512;
    size_t maxLen = 0;  // track longest string

    // -- PASS 1: Gather formatted strings & measure --
    Node* cur = stack->top;
    int index = 0;
    int arrayPos = 0;

    while (cur && index <= endIndex) {
        if (index >= startIndex && index <= endIndex) {
            items[arrayPos] = (char*)malloc(ITEM_BUFFER_SIZE);
            if (!items[arrayPos]) {
                printf("(Allocation failed for item)\n");
                // free partial
                for (int j = 0; j < arrayPos; j++) {
                    free(items[j]);
                }
                free(items);
                return;
            }
            memset(items[arrayPos], 0, ITEM_BUFFER_SIZE);

            // Format the data
            formatter(cur->data, items[arrayPos], ITEM_BUFFER_SIZE);

            size_t length = strlen(items[arrayPos]);
            if (length > maxLen) {
                maxLen = length;
            }
            arrayPos++;
        }
        cur = cur->next;
        index++;
    }

    // If user set a clamp width
    if (maxWidth > 0 && (int)maxLen > maxWidth) {
        maxLen = maxWidth;
    }
    if (maxLen == 0) {
        maxLen = 1; // avoid zero width
    }

    // -- PASS 2: Print each item aligned to maxLen --
    for (int i = 0; i < rangeCount; i++) {
        char* text = items[i];
        size_t textLen = strlen(text);

        // If too long, embed ellipsis so total length doesn't exceed maxLen
        if (textLen > maxLen) {
            text[maxLen - 3] = '\0'; // cut at maxLen-3
            strcat(text, "...");     // now exactly maxLen chars
            textLen = maxLen;
        }

        // Print top border
        printf("|");
        for (size_t w = 0; w < maxLen; w++) {
            printf("=");
        }
        printf("|\n");

        // Print content
        printf("|%s", text);

        // If still shorter, pad with spaces
        textLen = strlen(text);
        if (textLen < maxLen) {
            for (size_t pad = textLen; pad < maxLen; pad++) {
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

        free(text); // typical cleanup
    }

    free(items); // free the array of char* pointers
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
