#include <stdio.h>
#include <stdlib.h>
#include "test_linkedlist.h"
#include "linkedlist.h"  // The linked list implementation you're testing

// A helper print function for integers
static void printInt(const void* data) {
    printf("%d -> ", *(const int*)data);
}

void testLinkedList(void) {
    printf("=== Testing Linked List ===\n");
    
    // 1) Create a list
    Node* head = NULL;

    // 2) Insert some data at beginning
    int a = 10, b = 20;
    insertAtBeginning(&head, &a, sizeof(int));
    insertAtBeginning(&head, &b, sizeof(int));

    // 3) Insert some data at end
    int c = 30;
    insertAtEnd(&head, &c, sizeof(int));

    // 4) Print the list
    printf("List after inserts: ");
    printList(head, printInt);

    // 5) Remove at beginning
    int removedValue;
    if (removeAtBeginning(&head, &removedValue)) {
        printf("Removed value (beginning): %d\n", removedValue);
    }

    // 6) Print the list again
    printf("List after removal: ");
    printList(head, printInt);

    // 7) Free the list
    freeList(&head);
    printf("List after freeList: ");
    printList(head, printInt);

    printf("=== Linked List Tests Complete ===\n\n");
}
