#include <stdio.h>
#include <stdlib.h>
#include "test_linkedlist.h"
#include "linkedlist.h"  // The linked list implementation you're testing

// A helper print function for integers
static void printInt(const void* data) {
    printf("%d -> ", *(const int*)data);
}

// A helper compare function for integers
static int intCompare(const void* a, const void* b) {
    int intA = *(const int*)a;
    int intB = *(const int*)b;
    return (intA - intB);
}

void testLinkedList(void) {
    printf("=== Testing Linked List (Extended) ===\n\n");

    /* ----------------------------------------------------------------------
     * 1) Basic insert at beginning and end (Existing Tests)
     * --------------------------------------------------------------------*/
    Node* head = NULL;

    // Insert some data at beginning
    int a = 10, b = 20, c = 30;
    insertAtBeginning(&head, &a, sizeof(int));  // List: 10
    insertAtBeginning(&head, &b, sizeof(int));  // List: 20 -> 10
    insertAtEnd(&head, &c, sizeof(int));        // List: 20 -> 10 -> 30

    printf("[After initial inserts] List: ");
    printList(head, printInt);

    // Remove at beginning
    int removedValue;
    if (removeAtBeginning(&head, &removedValue)) {
        printf("Removed (beginning): %d\n", removedValue); // Should remove 20
    }
    printf("[After removeAtBeginning] List: ");
    printList(head, printInt);

    /* ----------------------------------------------------------------------
     * 2) Insert At Position
     * --------------------------------------------------------------------*/
    printf("\n--- Testing insertAtPosition ---\n");
    // Current list should be: 10 -> 30 -> NULL

    // Insert at position 0 (same as insertAtBeginning)
    int d = 40;
    int result = insertAtPosition(&head, &d, sizeof(int), 0); // List: 40 -> 10 -> 30
    printf("Insert 40 at position 0: %s\n", result ? "Success" : "Fail");
    printList(head, printInt);

    // Insert at position 2 (middle)
    int e = 50;
    result = insertAtPosition(&head, &e, sizeof(int), 2); // List: 40 -> 10 -> 50 -> 30
    printf("Insert 50 at position 2: %s\n", result ? "Success" : "Fail");
    printList(head, printInt);

    // Insert at position 4 (end)
    int f = 60;
    result = insertAtPosition(&head, &f, sizeof(int), 4); // List: 40 -> 10 -> 50 -> 30 -> 60
    printf("Insert 60 at position 4: %s\n", result ? "Success" : "Fail");
    printList(head, printInt);

    // Try to insert at invalid position (e.g., position 10)
    int g = 70;
    result = insertAtPosition(&head, &g, sizeof(int), 10); // Should fail
    printf("Insert 70 at invalid position 10: %s\n", result ? "Success" : "Fail");
    printList(head, printInt);

    /* ----------------------------------------------------------------------
     * 3) Remove At Position
     * --------------------------------------------------------------------*/
    printf("\n--- Testing removeAtPosition ---\n");
    // Current list: 40 -> 10 -> 50 -> 30 -> 60

    // Remove at position 0
    if (removeAtPosition(&head, 0, &removedValue)) { // Should remove 40
        printf("Removed at position 0: %d\n", removedValue);
    } else {
        printf("Failed to remove at position 0\n");
    }
    printList(head, printInt);

    // Remove at position 2 (current list: 10 -> 50 -> 30 -> 60)
    if (removeAtPosition(&head, 2, &removedValue)) { // Should remove 30
        printf("Removed at position 2: %d\n", removedValue);
    } else {
        printf("Failed to remove at position 2\n");
    }
    printList(head, printInt);

    // Remove at last position (which should now be position 2, containing 60)
    if (removeAtPosition(&head, 2, &removedValue)) { // Should remove 60
        printf("Removed at last position: %d\n", removedValue);
    } else {
        printf("Failed to remove at last position\n");
    }
    printList(head, printInt);

    // Try to remove at an invalid position (e.g., 5)
    if (removeAtPosition(&head, 5, &removedValue)) {
        printf("Removed at position 5: %d\n", removedValue);
    } else {
        printf("Failed to remove at position 5 (invalid)\n");
    }
    printList(head, printInt);

    /* ----------------------------------------------------------------------
     * 4) Search
     * --------------------------------------------------------------------*/
    printf("\n--- Testing search ---\n");
    // Current list: 10 -> 50  (positions: 0 -> 1)
    Node* found = NULL;
    int searchVal = 50;
    found = search(head, &searchVal, intCompare);
    if (found) {
        printf("Search for %d: Found node with data %d\n", searchVal, *(int*)found->data);
    } else {
        printf("Search for %d: Not found\n", searchVal);
    }

    searchVal = 999;
    found = search(head, &searchVal, intCompare);
    if (found) {
        printf("Search for %d: Found node with data %d\n", searchVal, *(int*)found->data);
    } else {
        printf("Search for %d: Not found\n", searchVal);
    }

    /* ----------------------------------------------------------------------
     * 5) Sort
     * --------------------------------------------------------------------*/
    printf("\n--- Testing sortList ---\n");
    // Let's insert more elements first to get a good test list
    int arr[] = {90, 10, 30, 50, 20, 40};
    // Current list: 10 -> 50
    for (int i = 0; i < 6; i++) {
        insertAtEnd(&head, &arr[i], sizeof(int));
    }
    // Now list: 10 -> 50 -> 90 -> 10 -> 30 -> 50 -> 20 -> 40

    printf("List before sort: ");
    printList(head, printInt);

    sortList(&head, intCompare);
    printf("List after sort (ascending): ");
    printList(head, printInt);

    // Test sorting a single-element list
    Node* single = NULL;
    int singleVal = 100;
    insertAtEnd(&single, &singleVal, sizeof(int));
    sortList(&single, intCompare);
    printf("Single-element list after sort: ");
    printList(single, printInt);
    freeList(&single);  // Clean up

    // Test sorting an empty list
    Node* empty = NULL;
    sortList(&empty, intCompare);
    printf("Empty list after sort: ");
    printList(empty, printInt);
    // (Nothing to free here)

    /* ----------------------------------------------------------------------
     * Clean Up
     * --------------------------------------------------------------------*/
    printf("\n--- Freeing the main test list ---\n");
    freeList(&head);
    printf("List after freeList: ");
    printList(head, printInt);

    printf("\n=== Linked List Tests Complete ===\n\n");
}
