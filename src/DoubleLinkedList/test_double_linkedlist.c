#include <stdio.h>
#include <stdlib.h>
#include "test_double_linkedlist.h"
#include "double_linkedlist.h"

/**
 * Helper function to print an integer node's data.
 */
static void printInt(const void* data) {
    printf("%d -> ", *(const int*)data);
}

void testDoubleLinkedList(void) {
    printf("=== Testing Doubly Linked List ===\n");

    // 1) Initialize the list
    DoubleLinkedList list;
    dllInit(&list);

    // 2) Insert some integers at front
    int a = 10, b = 20, c = 30;
    dllInsertFront(&list, &a, sizeof(int)); // front: 10
    printf("List after front inserts: ");
    dllPrint(&list, printInt);
    dllInsertFront(&list, &b, sizeof(int)); // front: 20 -> 10
    dllInsertFront(&list, &c, sizeof(int)); // front: 30 -> 20 -> 10

    printf("List after front inserts: ");
    dllPrint(&list, printInt);

    // 3) Insert some integers at back
    int d = 40, e = 50;
    dllInsertBack(&list, &d, sizeof(int));  // 30 -> 20 -> 10 -> 40
    dllInsertBack(&list, &e, sizeof(int));  // 30 -> 20 -> 10 -> 40 -> 50

    printf("List after back inserts:  ");
    dllPrint(&list, printInt);

    // 4) Remove from front
    int removedFront;
    if (dllRemoveFront(&list, &removedFront)) {
        printf("Removed front value: %d\n", removedFront);
    }
    printf("List now: ");
    dllPrint(&list, printInt);

    // 5) Remove from back
    int removedBack;
    if (dllRemoveBack(&list, &removedBack)) {
        printf("Removed back value: %d\n", removedBack);
    }
    printf("List now: ");
    dllPrint(&list, printInt);

    // 6) Free the list
    dllFree(&list);
    printf("List after dllFree: ");
    dllPrint(&list, printInt);

    printf("=== Doubly Linked List Tests Complete ===\n\n");
}
