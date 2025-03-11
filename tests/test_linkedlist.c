#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"
#include "test_linkedlist.h"

/* ========================================================================= */
/* Helpers                                                                   */
/* ========================================================================= */

/**
 * @brief A comparison function for integers (ascending order).
 */
static int intCompare(const void* a, const void* b) {
    int intA = *(const int*)a;
    int intB = *(const int*)b;
    return (intA - intB);
}

/**
 * @brief Helper to convert Node->data to int for easy checks.
 */
static int nodeDataAsInt(const Node* node) {
    return *(int*)(node->data);
}


// A helper print function for integers
static void printInt(const void* data) {
    printf("%d -> ", *(const int*)data);
}

/* ========================================================================= */
/* Individual Tests                                                          */
/* ========================================================================= */

static void test_createNode(void) {
    printf("Running %s...\n", __func__);

    int value = 42;
    Node* n = createNode(&value, sizeof(int));
    assert(n != NULL);
    assert(n->data != NULL);
    assert(n->data_size == sizeof(int));
    assert(*(int*)(n->data) == 42);
    assert(n->next == NULL);

    // Clean up
    free(n->data);
    free(n);

    printf("%s PASSED.\n", __func__);
}

static void test_insertAtBeginning(void) {
    printf("Running %s...\n", __func__);

    Node* head = NULL;

    int val1 = 10;
    insertAtBeginning(&head, &val1, sizeof(int));
    // List should be: [10]
    assert(head != NULL);
    assert(nodeDataAsInt(head) == 10);
    assert(head->next == NULL);

    int val2 = 20;
    insertAtBeginning(&head, &val2, sizeof(int));
    // List should be: [20 -> 10]
	/*
		PRINT LIST DEMO IS HERE ALONG WITH PRINT INT
	*/
	printList(head, printInt);

    assert(head != NULL);
    assert(nodeDataAsInt(head) == 20);
    assert(head->next != NULL);
    assert(nodeDataAsInt(head->next) == 10);

    freeList(&head);
    assert(head == NULL);

    printf("%s PASSED.\n", __func__);
}

static void test_insertAtEnd(void) {
    printf("Running %s...\n", __func__);

    Node* head = NULL;

    int val1 = 10;
    insertAtEnd(&head, &val1, sizeof(int));
    // List should be: [10]
    assert(head != NULL);
    assert(nodeDataAsInt(head) == 10);
    assert(head->next == NULL);

    int val2 = 20;
    insertAtEnd(&head, &val2, sizeof(int));
    // List should be: [10 -> 20]
    assert(head != NULL);
    assert(nodeDataAsInt(head) == 10);
    assert(head->next != NULL);
    assert(nodeDataAsInt(head->next) == 20);
    assert(head->next->next == NULL);

    freeList(&head);
    assert(head == NULL);

    printf("%s PASSED.\n", __func__);
}

static void test_removeAtBeginning(void) {
    printf("Running %s...\n", __func__);

    Node* head = NULL;
    int val1 = 10, val2 = 20;
    insertAtBeginning(&head, &val1, sizeof(int)); // List: [10]
    insertAtBeginning(&head, &val2, sizeof(int)); // List: [20 -> 10]

    int removedValue;
    // Remove first element (20)
    int res = removeAtBeginning(&head, &removedValue);
    assert(res == 1);
    assert(removedValue == 20);
    // List should now be [10]
    assert(head != NULL);
    assert(nodeDataAsInt(head) == 10);

    // Remove next element (10)
    res = removeAtBeginning(&head, &removedValue);
    assert(res == 1);
    assert(removedValue == 10);
    // List should now be []
    assert(head == NULL);

    // Attempt to remove from empty list
    res = removeAtBeginning(&head, &removedValue);
    assert(res == 0);

    freeList(&head); // safe to call on empty
    assert(head == NULL);

    printf("%s PASSED.\n", __func__);
}

static void test_insertAtPosition(void) {
    printf("Running %s...\n", __func__);

    Node* head = NULL;
    int res;

    // Start with an empty list

    // Insert at pos 0 => same as insertAtBeginning
    int val1 = 10;
    res = insertAtPosition(&head, &val1, sizeof(int), 0); 
    assert(res == 1);
    assert(head != NULL);
    assert(nodeDataAsInt(head) == 10);

    // Insert 20 at pos 0 => now [20 -> 10]
    int val2 = 20;
    res = insertAtPosition(&head, &val2, sizeof(int), 0);
    assert(res == 1);
    assert(nodeDataAsInt(head) == 20);
    assert(nodeDataAsInt(head->next) == 10);

    // Insert 30 at pos 1 => [20 -> 30 -> 10]
    int val3 = 30;
    res = insertAtPosition(&head, &val3, sizeof(int), 1);
    assert(res == 1);
    assert(nodeDataAsInt(head) == 20);
    assert(nodeDataAsInt(head->next) == 30);
    assert(nodeDataAsInt(head->next->next) == 10);

    // Insert 40 at pos 3 => [20 -> 30 -> 10 -> 40]
    int val4 = 40;
    res = insertAtPosition(&head, &val4, sizeof(int), 3);
    assert(res == 1);
    // Verify final arrangement
    Node* tmp = head;
    assert(nodeDataAsInt(tmp) == 20); tmp = tmp->next;
    assert(nodeDataAsInt(tmp) == 30); tmp = tmp->next;
    assert(nodeDataAsInt(tmp) == 10); tmp = tmp->next;
    assert(nodeDataAsInt(tmp) == 40); tmp = tmp->next;
    assert(tmp == NULL);

    // Try invalid position
    int val5 = 50;
    res = insertAtPosition(&head, &val5, sizeof(int), 10); 
    assert(res == 0); // Should fail

    // Clean up
    freeList(&head);
    assert(head == NULL);

    printf("%s PASSED.\n", __func__);
}

static void test_removeAtPosition(void) {
    printf("Running %s...\n", __func__);

    Node* head = NULL;
    int removedValue;
    int res;

    // Create a small list: [10 -> 20 -> 30 -> 40]
    int vals[] = {10, 20, 30, 40};
    for (int i = 0; i < 4; i++) {
        insertAtEnd(&head, &vals[i], sizeof(int));
    }

    // Remove at position 0 => remove 10
    res = removeAtPosition(&head, 0, &removedValue);
    assert(res == 1);
    assert(removedValue == 10);
    // List is now [20 -> 30 -> 40]

    // Remove at position 1 => remove 30
    res = removeAtPosition(&head, 1, &removedValue);
    assert(res == 1);
    assert(removedValue == 30);
    // List is now [20 -> 40]

    // Remove at last position => position 1 => remove 40
    res = removeAtPosition(&head, 1, &removedValue);
    assert(res == 1);
    assert(removedValue == 40);
    // List is now [20]

    // Remove at invalid position => position 5
    res = removeAtPosition(&head, 5, &removedValue);
    assert(res == 0);
    // List remains [20]

    // Remove at position 0 => remove 20
    res = removeAtPosition(&head, 0, &removedValue);
    assert(res == 1);
    assert(removedValue == 20);
    // List is now empty

    freeList(&head); // no effect on empty
    assert(head == NULL);

    printf("%s PASSED.\n", __func__);
}

static void test_search(void) {
    printf("Running %s...\n", __func__);

    Node* head = NULL;
    // Populate: [10 -> 20 -> 30]
    int vals[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        insertAtEnd(&head, &vals[i], sizeof(int));
    }

    // Search for 20
    int key = 20;
    Node* found = search(head, &key, intCompare);
    assert(found != NULL);
    assert(*(int*)found->data == 20);

    // Search for non-existing
    key = 999;
    found = search(head, &key, intCompare);
    assert(found == NULL);

    // Cleanup
    freeList(&head);
    assert(head == NULL);

    printf("%s PASSED.\n", __func__);
}

static void test_sortList(void) {
    printf("Running %s...\n", __func__);

    Node* head = NULL;
    // Let's insert some values in random order
    int vals[] = {50, 10, 40, 30, 20};
    for (int i = 0; i < 5; i++) {
        insertAtEnd(&head, &vals[i], sizeof(int));
    }
    // Now the list is [50 -> 10 -> 40 -> 30 -> 20]
    sortList(&head, intCompare);
    // The list should be [10 -> 20 -> 30 -> 40 -> 50]
    Node* temp = head;
    for (int i = 1; i <= 5; i++) {
        assert(nodeDataAsInt(temp) == i*10);
        temp = temp->next;
    }
    assert(temp == NULL);

    // Test single-element list
    Node* single = NULL;
    int singleVal = 100;
    insertAtEnd(&single, &singleVal, sizeof(int));
    sortList(&single, intCompare);
    assert(single != NULL);
    assert(nodeDataAsInt(single) == 100);
    assert(single->next == NULL);
    freeList(&single);
    assert(single == NULL);

    // Test empty list
    Node* empty = NULL;
    sortList(&empty, intCompare);
    assert(empty == NULL);

    // Cleanup main list
    freeList(&head);
    assert(head == NULL);

    printf("%s PASSED.\n", __func__);
}

static void test_freeList(void) {
    printf("Running %s...\n", __func__);

    // Create a list, then free it
    Node* head = NULL;
    int val1 = 123, val2 = 456;
    insertAtEnd(&head, &val1, sizeof(int));
    insertAtEnd(&head, &val2, sizeof(int));

    freeList(&head);
    assert(head == NULL);

    // Safe to call freeList again
    freeList(&head);
    assert(head == NULL);

    printf("%s PASSED.\n", __func__);
}

/* ========================================================================= */
/* Main Test Runner                                                          */
/* ========================================================================= */

void testLinkedList(void) {
    printf("=== BEGIN LINKED LIST TESTS (ASSERTION-BASED) ===\n\n");

    test_createNode();
    test_insertAtBeginning();
    test_insertAtEnd();
    test_removeAtBeginning();
    test_insertAtPosition();
    test_removeAtPosition();
    test_search();
    test_sortList();
    test_freeList();

    printf("\n=== ALL TESTS PASSED SUCCESSFULLY ===\n");
}
