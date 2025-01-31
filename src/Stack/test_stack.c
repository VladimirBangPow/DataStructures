#include <stdio.h>
#include <stdlib.h>
#include "test_stack.h"
#include "stack.h"       // The stack implementation
#include "../LinkedList/linkedlist.h"  // Because stack uses the linked list internally

static void printInt(const void* data) {
    printf("%d -> ", *(const int*)data);
}

// A helper function to print the stack
static void printIntStack(const Stack* stack) {
    printf("Stack top: ");
    printList(stack->top, printInt);
}

void testStack(void) {
    printf("=== Testing Stack ===\n");

    // 1) Initialize the stack
    Stack stack;
    stackInit(&stack);

    // 2) Push some integers
    int x = 10, y = 20, z = 30;
    stackPush(&stack, &x, sizeof(int)); // top is 10
    stackPush(&stack, &y, sizeof(int)); // top is 20
    stackPush(&stack, &z, sizeof(int)); // top is 30

    printf("Stack after pushing 3 integers:\n");
    printIntStack(&stack);

    // 3) Peek the top
    int topValue;
    if (stackPeek(&stack, &topValue)) {
        printf("Peeked top value: %d\n", topValue);
    } else {
        printf("Stack is empty!\n");
    }

    // 4) Pop one
    int poppedValue;
    if (stackPop(&stack, &poppedValue)) {
        printf("Popped value: %d\n", poppedValue);
    } else {
        printf("Stack is empty, cannot pop!\n");
    }

    printf("Stack after popping once:\n");
    printIntStack(&stack);

    // 5) Clear
    stackClear(&stack);
    printf("Stack after clearing:\n");
    printIntStack(&stack);

    printf("=== Stack Tests Complete ===\n\n");
}
