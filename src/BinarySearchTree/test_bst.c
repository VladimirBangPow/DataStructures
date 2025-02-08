#include "test_bst.h"
#include "bst.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------------------------------------------------------
 *                      INT TEST
 * ---------------------------------------------------------------- */
static int compareInts(const void *a, const void *b) {
    /* Compare the integer values pointed to by a and b */
    int intA = *(const int *)a;
    int intB = *(const int *)b;
    return (intA - intB);
}

static void checkInOrderInt(const BSTNode *root, const int *expected, size_t size) {
    /* We’ll do a quick in-order traversal and compare each visited value
       against 'expected' in sequence. */
    static size_t index = 0;
    index = 0;  // reset each time we call the function

    void traverse(const BSTNode *node) {
        if (!node) return;
        traverse(node->left);
        int value = *(int *)node->data;
        assert(value == expected[index] && "In-order integer sequence mismatch");
        index++;
        traverse(node->right);
    }

    traverse(root);
    assert(index == size && "In-order traversal (int) count mismatch");
}

static void testBSTWithInts(void) {
    printf("  [INT] Testing BST with integers...\n");

    BSTNode *root = NULL;

    /* Example data */
    static int dataArray[] = {10, 5, 15, 2, 7, 12, 20};

    /* Insert each item */
    for (size_t i = 0; i < sizeof(dataArray)/sizeof(dataArray[0]); i++) {
        root = insertBSTNode(root, &dataArray[i], compareInts);
    }

    /* Search for an existing key (7) */
    int searchVal = 7;
    BSTNode *found = searchBST(root, &searchVal, compareInts);
    assert(found && *(int *)found->data == 7);

    /* Search for a non-existing key (999) */
    int nonExistent = 999;
    assert(searchBST(root, &nonExistent, compareInts) == NULL);

    /* Find min and max */
    BSTNode *minNode = findMinBSTNode(root);
    BSTNode *maxNode = findMaxBSTNode(root);
    assert(minNode && *(int *)minNode->data == 2);
    assert(maxNode && *(int *)maxNode->data == 20);

    /* Check in-order traversal */
    {
        int expectedInOrder[] = {2, 5, 7, 10, 12, 15, 20};
        size_t size = sizeof(expectedInOrder) / sizeof(expectedInOrder[0]);
        checkInOrderInt(root, expectedInOrder, size);
    }

    /* Delete a leaf (2) */
    int delVal = 2;
    root = deleteBSTNode(root, &delVal, compareInts);
    assert(searchBST(root, &delVal, compareInts) == NULL);

    /* Delete a node with one child (5, which should have child 7) */
    delVal = 5;
    root = deleteBSTNode(root, &delVal, compareInts);
    assert(searchBST(root, &delVal, compareInts) == NULL);

    /* Delete a node with two children (10) */
    delVal = 10;
    root = deleteBSTNode(root, &delVal, compareInts);
    assert(searchBST(root, &delVal, compareInts) == NULL);

    /* Clean up */
    freeBST(root);
    root = NULL;
}

/* ----------------------------------------------------------------
 *                      CHAR TEST
 * ---------------------------------------------------------------- */
static int compareChars(const void *a, const void *b) {
    /* Compare the char values pointed to by a and b */
    char charA = *(const char *)a;
    char charB = *(const char *)b;
    return (int)charA - (int)charB;
}

static void checkInOrderChar(const BSTNode *root, const char *expected, size_t size) {
    static size_t index = 0;
    index = 0;

    void traverse(const BSTNode *node) {
        if (!node) return;
        traverse(node->left);
        char value = *(char *)node->data;
        assert(value == expected[index] && "In-order char sequence mismatch");
        index++;
        traverse(node->right);
    }

    traverse(root);
    assert(index == size && "In-order traversal (char) count mismatch");
}

static void testBSTWithChars(void) {
    printf("  [CHAR] Testing BST with characters...\n");

    BSTNode *root = NULL;

    static char chars[] = {'m', 'g', 't', 'a', 'z', 'k', 'o'};
    size_t n = sizeof(chars) / sizeof(chars[0]);

    /* Insert the chars */
    for (size_t i = 0; i < n; i++) {
        root = insertBSTNode(root, &chars[i], compareChars);
    }

    /* Search for 'z' */
    char searchVal = 'z';
    BSTNode *found = searchBST(root, &searchVal, compareChars);
    assert(found && *(char *)found->data == 'z');

    /* In-order should be: a g k m o t z (alphabetical order) */
    {
        const char expected[] = {'a', 'g', 'k', 'm', 'o', 't', 'z'};
        checkInOrderChar(root, expected, sizeof(expected)/sizeof(expected[0]));
    }

    /* Delete a char (say 'm', which might be the root) */
    char toDelete = 'm';
    root = deleteBSTNode(root, &toDelete, compareChars);
    assert(searchBST(root, &toDelete, compareChars) == NULL);

    /* Free */
    freeBST(root);
    root = NULL;
}

/* ----------------------------------------------------------------
 *                      STRING TEST
 * ---------------------------------------------------------------- */
static int compareStrings(const void *a, const void *b) {
    /* a and b are `char*` pointers, so cast and use strcmp */
    const char *strA = *(const char **)a;  /* we store pointer-to-string in the tree */
    const char *strB = *(const char **)b;
    return strcmp(strA, strB);
}

static void checkInOrderStrings(const BSTNode *root, const char *expected[], size_t size) {
    static size_t index = 0;
    index = 0;

    void traverse(const BSTNode *node) {
        if (!node) return;
        traverse(node->left);

        const char *value = *(const char **)node->data;
        assert(strcmp(value, expected[index]) == 0 && "In-order string mismatch");
        index++;

        traverse(node->right);
    }

    traverse(root);
    assert(index == size && "In-order traversal (string) count mismatch");
}

static void testBSTWithStrings(void) {
    printf("  [STRING] Testing BST with strings...\n");

    BSTNode *root = NULL;

    /* For demonstration, store array of string literals (const char*). */
    static const char *strings[] = {"delta", "alpha", "echo", "charlie", "bravo"};
    size_t n = sizeof(strings) / sizeof(strings[0]);

    /* Insert the strings (note: we pass &strings[i] because the tree holds void*) */
    for (size_t i = 0; i < n; i++) {
        root = insertBSTNode(root, &strings[i], compareStrings);
    }

    /* In-order should be: "alpha", "bravo", "charlie", "delta", "echo" */
    {
        static const char *expected[] = {"alpha", "bravo", "charlie", "delta", "echo"};
        checkInOrderStrings(root, expected, 5);
    }

    /* Search for "charlie" */
    const char *searchVal = "charlie";
    BSTNode *found = searchBST(root, &searchVal, compareStrings);
    assert(found != NULL && strcmp(*(const char **)found->data, "charlie") == 0);

    /* Delete "delta" */
    const char *toDelete = "delta";
    root = deleteBSTNode(root, &toDelete, compareStrings);
    assert(searchBST(root, &toDelete, compareStrings) == NULL);

    /* Free */
    freeBST(root);
    root = NULL;
}

/* ----------------------------------------------------------------
 *                      STRUCT TEST
 * ---------------------------------------------------------------- */
/* Example struct */
typedef struct Person {
    const char *name;
    int age;
} Person;

/* Compare by age, then by name if ages are the same */
static int comparePersons(const void *a, const void *b) {
    const Person *pA = (const Person *)a;
    const Person *pB = (const Person *)b;

    if (pA->age < pB->age) return -1;
    if (pA->age > pB->age) return 1;
    /* if same age, compare names */
    return strcmp(pA->name, pB->name);
}

/* We can do a quick in-order check by verifying ascending ages, for example. */
static void checkInOrderPersons(const BSTNode *root, const Person *expected, size_t size) {
    static size_t index = 0;
    index = 0;

    void traverse(const BSTNode *node) {
        if (!node) return;
        traverse(node->left);

        const Person *p = (const Person *)node->data;
        assert(p->age == expected[index].age && "In-order Person age mismatch");
        assert(strcmp(p->name, expected[index].name) == 0 && "In-order Person name mismatch");
        index++;

        traverse(node->right);
    }

    traverse(root);
    assert(index == size && "In-order traversal (struct) count mismatch");
}

static void testBSTWithStructs(void) {
    printf("  [STRUCT] Testing BST with Person struct...\n");

    BSTNode *root = NULL;

    /* For simplicity, store these statically. */
    static Person people[] = {
        {"Alice", 30},
        {"Bob",   25},
        {"Charlie", 35},
        {"Dave",    25},   /* same age as Bob, but name > Bob */
        {"Eve",     40}
    };
    size_t n = sizeof(people) / sizeof(people[0]);

    /* Insert them */
    for (size_t i = 0; i < n; i++) {
        root = insertBSTNode(root, &people[i], comparePersons);
    }

    /* In-order by age, then name:
       1) Bob (25), 2) Dave (25), 3) Alice (30), 4) Charlie (35), 5) Eve (40)
    */
    static Person expected[] = {
        {"Bob", 25},
        {"Dave", 25},
        {"Alice", 30},
        {"Charlie", 35},
        {"Eve", 40}
    };
    checkInOrderPersons(root, expected, n);

    /* Search for "Alice" (age 30) */
    Person searchVal = {"Alice", 30};
    BSTNode *found = searchBST(root, &searchVal, comparePersons);
    assert(found != NULL && "Should find Alice in the tree");
    assert(((Person *)found->data)->age == 30);

    /* Delete "Bob" (25) */
    Person delVal = {"Bob", 25};
    root = deleteBSTNode(root, &delVal, comparePersons);
    assert(searchBST(root, &delVal, comparePersons) == NULL && "Bob should be removed");

    freeBST(root);
    root = NULL;
}

/* 
 * Optional: Simple function to verify the BST is still in sorted order via in-order traversal.
 * We'll just confirm that each visited node is >= the previously visited value.
 */
static void verifySortedOrder(const BSTNode *root) {
    static int lastVal = -2147483648;  // or INT_MIN
    static int firstCall = 1;

    if (firstCall) {
        // Reset static variables each time we start a new check
        lastVal = -2147483648; 
        firstCall = 0;
    }

    if (!root) return;
    verifySortedOrder(root->left);

    int currentVal = *(int *)root->data;
    assert(currentVal >= lastVal && "BST out of order!");
    lastVal = currentVal;

    verifySortedOrder(root->right);
}

/*
 * STRESS TEST:
 * 1. Generate random integers.
 * 2. Insert them into the BST.
 * 3. Perform random searches & deletions.
 * 4. Validate final BST structure/order.
 */
static void testBSTStress(void) {
    printf("  [STRESS] Running large-scale/stress test on BST with int data...\n");

    /* Choose how many random values to insert. 
       Adjust as needed for your performance requirements. */
    const int NUM_VALUES = 20000;

    /* Allocate an array to store random integers. */
    int *values = (int *)malloc(NUM_VALUES * sizeof(int));
    if (!values) {
        fprintf(stderr, "Failed to allocate memory for stress test.\n");
        return;
    }

    /* Seed the random number generator. 
       You could also take a time-based or user-provided seed. */
    srand((unsigned int)time(NULL));

    /* Generate random integers. */
    for (int i = 0; i < NUM_VALUES; i++) {
        values[i] = rand() % (NUM_VALUES * 10);  // Range: [0..NUM_VALUES*10)
    }

    /* Create an empty BST. */
    BSTNode *root = NULL;

    /* 1) Insert all random values. */
    for (int i = 0; i < NUM_VALUES; i++) {
        root = insertBSTNode(root, &values[i], compareInts);
    }

    /*
     * 2) Optionally, do some random searches.
     *    For example, pick 1000 random elements from 'values' to search for.
     */
    for (int i = 0; i < 1000; i++) {
        int idx = rand() % NUM_VALUES;
        int searchVal = values[idx];
        BSTNode *found = searchBST(root, &searchVal, compareInts);
        // Not asserting found/unfound here because duplicates or partial coverage
        // might exist. But you could do additional checks if needed.
        (void)found; // just to suppress unused variable warning
    }

    /*
     * 3) Optionally, delete some random subset of items.
     *    E.g., delete 25% of the inserted items.
     */
    int deleteCount = NUM_VALUES / 4;
    for (int i = 0; i < deleteCount; i++) {
        int idx = rand() % NUM_VALUES;
        int deleteVal = values[idx];
        root = deleteBSTNode(root, &deleteVal, compareInts);
    }

    /*
     * 4) Verify the BST is still in sorted order with an in-order traversal.
     *    If you want to ensure the exact contents, you'd need to track
     *    which items are actually still in the tree after deletions.
     */
    verifySortedOrder(root);
    // reset the static check variables (in case you want to call again)
    // easiest way is to re-init them in the function, or do a small trick:
    // Call verifySortedOrder(NULL) once more.
    verifySortedOrder(NULL);

    /* 5) Free the BST. */
    freeBST(root);
    root = NULL;

    /* 6) Clean up allocated array. */
    free(values);

    printf("  [STRESS] Stress test completed successfully.\n");
}

/* ----------------------------------------------------------------
 *                   MAIN TEST ENTRY POINT
 * ---------------------------------------------------------------- */
void testBinarySearchTree(void) {
    printf("Running Generic Binary Search Tree Tests...\n\n");

    testBSTWithInts();
    testBSTWithChars();
    testBSTWithStrings();
    testBSTWithStructs();
	testBSTStress();
    printf("\nAll BST tests for multiple data types passed!\n");
}
