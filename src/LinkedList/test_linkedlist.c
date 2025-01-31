#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
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

/* ========================================================================= */
/* Individual Tests (static)                                                */
/* ========================================================================= */

static void test_createNode_impl(void) {
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
}

static void test_insertAtBeginning_impl(void) {
    Node* head = NULL;

    int val1 = 10;
    insertAtBeginning(&head, &val1, sizeof(int));
    // List: [10]
    assert(head != NULL);
    assert(nodeDataAsInt(head) == 10);
    assert(head->next == NULL);

    int val2 = 20;
    insertAtBeginning(&head, &val2, sizeof(int));
    // List: [20 -> 10]
    assert(head != NULL);
    assert(nodeDataAsInt(head) == 20);
    assert(head->next != NULL);
    assert(nodeDataAsInt(head->next) == 10);

    freeList(&head);
    assert(head == NULL);
}

static void test_insertAtEnd_impl(void) {
    Node* head = NULL;

    int val1 = 10;
    insertAtEnd(&head, &val1, sizeof(int));
    // List: [10]
    assert(head != NULL);
    assert(nodeDataAsInt(head) == 10);
    assert(head->next == NULL);

    int val2 = 20;
    insertAtEnd(&head, &val2, sizeof(int));
    // List: [10 -> 20]
    assert(head != NULL);
    assert(nodeDataAsInt(head) == 10);
    assert(head->next != NULL);
    assert(nodeDataAsInt(head->next) == 20);
    assert(head->next->next == NULL);

    freeList(&head);
    assert(head == NULL);
}

static void test_removeAtBeginning_impl(void) {
    Node* head = NULL;
    int val1 = 10, val2 = 20;
    insertAtBeginning(&head, &val1, sizeof(int)); // List: [10]
    insertAtBeginning(&head, &val2, sizeof(int)); // List: [20 -> 10]

    int removedValue;
    // Remove first element (20)
    int res = removeAtBeginning(&head, &removedValue);
    assert(res == 1);
    assert(removedValue == 20);
    // List is now [10]
    assert(head != NULL);
    assert(nodeDataAsInt(head) == 10);

    // Remove next element (10)
    res = removeAtBeginning(&head, &removedValue);
    assert(res == 1);
    assert(removedValue == 10);
    // List is now empty
    assert(head == NULL);

    // Attempt to remove from empty list
    res = removeAtBeginning(&head, &removedValue);
    assert(res == 0);

    freeList(&head);
    assert(head == NULL);
}

static void test_insertAtPosition_impl(void) {
    Node* head = NULL;
    int res;

    // Insert at pos 0 => same as insertAtBeginning
    int val1 = 10;
    res = insertAtPosition(&head, &val1, sizeof(int), 0); 
    assert(res == 1);
    assert(head != NULL);
    assert(nodeDataAsInt(head) == 10);

    // Insert 20 at pos 0 => [20 -> 10]
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
    Node* tmp = head;
    assert(nodeDataAsInt(tmp) == 20); tmp = tmp->next;
    assert(nodeDataAsInt(tmp) == 30); tmp = tmp->next;
    assert(nodeDataAsInt(tmp) == 10); tmp = tmp->next;
    assert(nodeDataAsInt(tmp) == 40); tmp = tmp->next;
    assert(tmp == NULL);

    // Try invalid position
    int val5 = 50;
    res = insertAtPosition(&head, &val5, sizeof(int), 10); 
    assert(res == 0);

    freeList(&head);
    assert(head == NULL);
}

static void test_removeAtPosition_impl(void) {
    Node* head = NULL;
    int removedValue;
    int res;

    // Create: [10 -> 20 -> 30 -> 40]
    int vals[] = {10, 20, 30, 40};
    for (int i = 0; i < 4; i++) {
        insertAtEnd(&head, &vals[i], sizeof(int));
    }

    // Remove at pos 0 => remove 10 => [20 -> 30 -> 40]
    res = removeAtPosition(&head, 0, &removedValue);
    assert(res == 1);
    assert(removedValue == 10);

    // Remove at pos 1 => remove 30 => [20 -> 40]
    res = removeAtPosition(&head, 1, &removedValue);
    assert(res == 1);
    assert(removedValue == 30);

    // Remove at pos 1 => remove 40 => [20]
    res = removeAtPosition(&head, 1, &removedValue);
    assert(res == 1);
    assert(removedValue == 40);

    // Invalid remove => pos 5
    res = removeAtPosition(&head, 5, &removedValue);
    assert(res == 0);
    // List is [20]

    // Remove last item => pos 0 => remove 20 => []
    res = removeAtPosition(&head, 0, &removedValue);
    assert(res == 1);
    assert(removedValue == 20);

    freeList(&head);
    assert(head == NULL);
}

static void test_search_impl(void) {
    Node* head = NULL;

    // Create: [10 -> 20 -> 30]
    int vals[] = {10, 20, 30};
    for (int i = 0; i < 3; i++) {
        insertAtEnd(&head, &vals[i], sizeof(int));
    }

    // Search for 20
    int key = 20;
    Node* found = search(head, &key, intCompare);
    assert(found != NULL);
    assert(nodeDataAsInt(found) == 20);

    // Search for non-existing 999
    key = 999;
    found = search(head, &key, intCompare);
    assert(found == NULL);

    freeList(&head);
    assert(head == NULL);
}

static void test_sortList_impl(void) {
    Node* head = NULL;
    // Insert random order: [50 -> 10 -> 40 -> 30 -> 20]
    int vals[] = {50, 10, 40, 30, 20};
    for (int i = 0; i < 5; i++) {
        insertAtEnd(&head, &vals[i], sizeof(int));
    }

    sortList(&head, intCompare);
    // Should now be [10 -> 20 -> 30 -> 40 -> 50]
    Node* temp = head;
    for (int i = 1; i <= 5; i++) {
        assert(nodeDataAsInt(temp) == i * 10);
        temp = temp->next;
    }
    assert(temp == NULL);

    // Single-element list
    Node* single = NULL;
    int singleVal = 100;
    insertAtEnd(&single, &singleVal, sizeof(int));
    sortList(&single, intCompare);
    assert(nodeDataAsInt(single) == 100);
    assert(single->next == NULL);
    freeList(&single);

    // Empty list
    Node* empty = NULL;
    sortList(&empty, intCompare);
    assert(empty == NULL);

    freeList(&head);
    assert(head == NULL);
}

static void test_freeList_impl(void) {
    // Build [123 -> 456], then free
    Node* head = NULL;
    int val1 = 123, val2 = 456;
    insertAtEnd(&head, &val1, sizeof(int));
    insertAtEnd(&head, &val2, sizeof(int));

    freeList(&head);
    assert(head == NULL);

    // Safe to call again
    freeList(&head);
    assert(head == NULL);
}

/* ========================================================================= */
/* Test Runner Implementation (Renamed from 'main' to 'testLinkedList')      */
/* ========================================================================= */

void testLinkedList(void) {
    printf("=== BEGIN LINKED LIST TESTS (ASSERTION-BASED) ===\n\n");

    printf("Running test_createNode_impl...\n");
    test_createNode_impl();
    printf("-> PASSED.\n\n");

    printf("Running test_insertAtBeginning_impl...\n");
    test_insertAtBeginning_impl();
    printf("-> PASSED.\n\n");

    printf("Running test_insertAtEnd_impl...\n");
    test_insertAtEnd_impl();
    printf("-> PASSED.\n\n");

    printf("Running test_removeAtBeginning_impl...\n");
    test_removeAtBeginning_impl();
    printf("-> PASSED.\n\n");

    printf("Running test_insertAtPosition_impl...\n");
    test_insertAtPosition_impl();
    printf("-> PASSED.\n\n");

    printf("Running test_removeAtPosition_impl...\n");
    test_removeAtPosition_impl();
    printf("-> PASSED.\n\n");

    printf("Running test_search_impl...\n");
    test_search_impl();
    printf("-> PASSED.\n\n");

    printf("Running test_sortList_impl...\n");
    test_sortList_impl();
    printf("-> PASSED.\n\n");

    printf("Running test_freeList_impl...\n");
    test_freeList_impl();
    printf("-> PASSED.\n\n");

    printf("=== ALL TESTS PASSED SUCCESSFULLY ===\n");
}
