#include <stdio.h>
#include "deque.h"

// A sample print function for integers
void printInt(const void* data) {
    printf("%d -> ", *(const int*)data);
}

void testDeque(void) {
    printf("=== Testing Deque ===\n");

    Deque dq;
    dequeInit(&dq);

    int a = 10, b = 20, c = 30;
    dequePushFront(&dq, &a, sizeof(int)); // front=10
    dequePushFront(&dq, &b, sizeof(int)); // front=20 -> 10
    dequePushBack(&dq, &c, sizeof(int));  // front=20 -> 10 -> 30

    printf("Deque content:\n");
    dequePrint(&dq, printInt); // Should print: 20 -> 10 -> 30 -> NULL

    // Pop from front
    int val;
    if (dequePopFront(&dq, &val)) {
        printf("Popped front: %d\n", val);
    }
    dequePrint(&dq, printInt);

    // Cleanup
    dequeClear(&dq);
    printf("Deque after clearing:\n");
    dequePrint(&dq, printInt); // Should print: NULL
    printf("=== Deque Tests Complete ===\n\n");

    return;
}





