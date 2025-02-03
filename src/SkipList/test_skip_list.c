#include "test_skip_list.h"
#include "skip_list.h"
#include "../LinkedList/linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

/************************************************
 * COMPARATOR FUNCTIONS
 ***********************************************/

// Compare two integers pointed to by 'a' and 'b'
static int intComparator(const void *a, const void *b)
{
    int aa = *(const int *)a;
    int bb = *(const int *)b;
    return (aa < bb) ? -1 : (aa > bb) ? 1 : 0;
}

// Compare two C strings (null-terminated char*) 
static int strComparator(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

/************************************************
 * FREE FUNCTIONS
 ***********************************************/

// Free an int pointer (just a wrapper for free)
static void freeInt(void *data)
{
    free(data); 
}

// Free a string pointer (allocated by strdup)
static void freeString(void *data)
{
    free(data);
}

/************************************************
 * STRUCT & COMPARATORS FOR TEST 3
 ***********************************************/

// A simple 2D point
typedef struct MyPoint {
    int x;
    int y;
} MyPoint;

// Compare by x first, then y if x is the same
static int pointComparator(const void *a, const void *b)
{
    const MyPoint *p1 = (const MyPoint *)a;
    const MyPoint *p2 = (const MyPoint *)b;

    if (p1->x < p2->x) return -1;
    if (p1->x > p2->x) return 1;
    // If x is same, compare y
    if (p1->y < p2->y) return -1;
    if (p1->y > p2->y) return 1;
    return 0;
}

// Free function for MyPoint, if we dynamically allocate them
static void freePoint(void *data)
{
    free(data);
}

/************************************************
 * TEST HELPERS FOR INTS
 ***********************************************/

// Insert a range of integers [start, end) by dynamically allocating each int
static void insertIntRange(SkipList *sl, int start, int end)
{
    for (int i = start; i < end; i++) {
        int *valPtr = (int *)malloc(sizeof(int));
        *valPtr = i;
        bool ok = slInsert(sl, valPtr);
        assert(ok && "Failed to insert integer in range");
    }
}

// Check that integers [start, end) all exist in the skip list
static void checkIntRangeExists(const SkipList *sl, int start, int end)
{
    for (int i = start; i < end; i++) {
        bool found = slSearch(sl, &i);
        assert(found && "Integer not found in skip list when expected");
    }
}

// Remove a range of integers [start, end), verifying they were present
static void removeIntRange(SkipList *sl, int start, int end)
{
    for (int i = start; i < end; i++) {
        bool removed = slRemove(sl, &i);
        assert(removed && "Integer not removed from skip list when expected");
    }
}

/************************************************
 * TEST HELPERS FOR STRINGS
 ***********************************************/

// Insert a list of strings (by strdup) into the skip list
static void insertStrings(SkipList *sl, const char *strings[], int count)
{
    for (int i = 0; i < count; i++) {
        char *copy = strdup(strings[i]);
        bool ok = slInsert(sl, copy);
        assert(ok && "Failed to insert string");
    }
}

// Check that certain strings exist in the skip list
static void checkStringsExist(const SkipList *sl, const char *strings[], int count)
{
    for (int i = 0; i < count; i++) {
        bool found = slSearch(sl, strings[i]);
        assert(found && "String not found in skip list");
    }
}

// Remove strings from skip list
static void removeStrings(SkipList *sl, const char *strings[], int count)
{
    for (int i = 0; i < count; i++) {
        bool removed = slRemove(sl, strings[i]);
        assert(removed && "String not removed from skip list when expected");
    }
}

/************************************************
 * TEST HELPERS FOR POINT STRUCTS
 ***********************************************/

// Insert an array of MyPoint values. We'll dynamically allocate each point.
static void insertPoints(SkipList *sl, const MyPoint *points, int count)
{
    for (int i = 0; i < count; i++) {
        MyPoint *p = (MyPoint *)malloc(sizeof(MyPoint));
        *p = points[i]; // copy struct
        bool ok = slInsert(sl, p);
        assert(ok && "Failed to insert a point struct");
    }
}

// Check that certain points exist in the skip list
static void checkPointsExist(const SkipList *sl, const MyPoint *points, int count)
{
    for (int i = 0; i < count; i++) {
        bool found = slSearch(sl, &points[i]);
        assert(found && "Point struct not found in skip list when expected");
    }
}

// Remove an array of points from skip list
static void removePoints(SkipList *sl, const MyPoint *points, int count)
{
    for (int i = 0; i < count; i++) {
        bool removed = slRemove(sl, &points[i]);
        assert(removed && "Point struct not removed from skip list when expected");
    }
}

// For the singly linked list, we only need to store the integer value
// We won't do a separate free because we'll handle it inside Node itself.

// Timer function
static double get_time_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

// Shuffle an array of int in-place
static void shuffle(int *array, int n) {
    if (n > 1) {
        for (int i = 0; i < n - 1; i++) {
            int j = i + rand() / (RAND_MAX / (n - i) + 1);
            int temp = array[j];
            array[j] = array[i];
            array[i] = temp;
        }
    }
}

// Compare performance of SkipList vs. Sorted Linked List on `n` elements
static void benchmarkComparison(int n)
{
    // Prepare data
    int *values = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        values[i] = i;
    }
    shuffle(values, n);

    /*************************
     * 1) SkipList benchmark *
     *************************/
    SkipList sl;
    slInit(&sl, 12, 0.5f, intComparator, free); // We'll just free() int pointers

    // a) Insert
    double start = get_time_seconds();
    for (int i = 0; i < n; i++) {
        int *valPtr = malloc(sizeof(int));
        *valPtr = values[i];
        bool ok = slInsert(&sl, valPtr);
        (void)ok; // If you want to assert, you can: assert(ok);
    }
    double end = get_time_seconds();
    double slInsertTime = end - start;

    // b) Search
    // Re-shuffle for random search order
    shuffle(values, n);
    start = get_time_seconds();
    for (int i = 0; i < n; i++) {
        bool found = slSearch(&sl, &values[i]);
        (void)found; // or assert(found)
    }
    end = get_time_seconds();
    double slSearchTime = end - start;

    // c) Remove
    // Re-shuffle for random removal order
    shuffle(values, n);
    start = get_time_seconds();
    for (int i = 0; i < n; i++) {
        bool removed = slRemove(&sl, &values[i]);
        (void)removed; // or assert(removed)
    }
    end = get_time_seconds();
    double slRemoveTime = end - start;

    slFree(&sl);

    /******************************************
     * 2) Sorted Linked List (naïve) benchmark *
     ******************************************/
    // Rebuild the same data
    for (int i = 0; i < n; i++) {
        values[i] = i;
    }
    shuffle(values, n);

    Node* head = NULL; // singly linked list head

    // a) Insert (in sorted order)
    start = get_time_seconds();
    for (int i = 0; i < n; i++) {
        // Because our Node stores a copy, we can just pass &values[i]
        insertInSortedOrder(&head, &values[i], sizeof(int), intComparator);
    }
    end = get_time_seconds();
    double llInsertTime = end - start;

    // b) Search
    shuffle(values, n);
    start = get_time_seconds();
    for (int i = 0; i < n; i++) {
        Node* found = search(head, &values[i], intComparator);
        (void)found; // or assert(found != NULL)
    }
    end = get_time_seconds();
    double llSearchTime = end - start;

    // c) Remove
    shuffle(values, n);
    start = get_time_seconds();
    for (int i = 0; i < n; i++) {
        // removeValue will do a linear search and remove the node
        int success = removeValue(&head, &values[i], intComparator, NULL);
        (void)success; // or assert(success == 1)
    }
    end = get_time_seconds();
    double llRemoveTime = end - start;

    freeList(&head); // Clean up any leftover nodes if needed

    // Print final results
    printf("\n\n=== Benchmark n=%d ===\n", n);
    printf("SkipList:\n");
    printf("  Insert  : %.6f sec  (%.2f us/op)\n",
           slInsertTime, (slInsertTime / n) * 1e6);
    printf("  Search  : %.6f sec  (%.2f us/op)\n",
           slSearchTime, (slSearchTime / n) * 1e6);
    printf("  Remove  : %.6f sec  (%.2f us/op)\n",
           slRemoveTime, (slRemoveTime / n) * 1e6);

    printf("Linked List (Sorted Insert):\n");
    printf("  Insert  : %.6f sec  (%.2f us/op)\n",
           llInsertTime, (llInsertTime / n) * 1e6);
    printf("  Search  : %.6f sec  (%.2f us/op)\n",
           llSearchTime, (llSearchTime / n) * 1e6);
    printf("  Remove  : %.6f sec  (%.2f us/op)\n",
           llRemoveTime, (llRemoveTime / n) * 1e6);

    free(values);
}

// You can call this in your main() or inside testSkipList()
static void runBenchmarks(void)
{
    srand((unsigned)time(NULL));

    // Warm-up / smaller tests
    benchmarkComparison(10000);
    benchmarkComparison(50000);

    // Larger tests
    benchmarkComparison(100000);
    // benchmarkComparison(500000);
    // // If your hardware allows, you can go even bigger:
    // benchmarkComparison(1000000);
}



/************************************************
 * The main test function
 ***********************************************/
void testSkipList(void)
{
    printf("Starting SkipList tests...\n");

    /* ----------------
       Test 1: Integers
       ----------------*/
    {
        printf("Test 1: Integer skip list...\n");
        // Create skip list for integers
        SkipList sl;
        slInit(&sl, 12, 0.5f, intComparator, freeInt);

        // Insert 0..99
        insertIntRange(&sl, 0, 100);
        // Check they exist
        checkIntRangeExists(&sl, 0, 100);

        // Remove 0..50
        removeIntRange(&sl, 0, 50);
        // Ensure 0..49 are gone
        for (int i = 0; i < 50; i++) {
            bool found = slSearch(&sl, &i);
            assert(!found && "Integer should have been removed");
        }
        // Ensure 50..99 still exist
        checkIntRangeExists(&sl, 50, 100);

        slFree(&sl);
        printf("Integer skip list test passed!\n");
    }

    /* ---------------
       Test 2: Strings
       ---------------*/
    {
        printf("Test 2: String skip list...\n");
        // Create skip list for C strings
        SkipList sl;
        slInit(&sl, 12, 0.5f, strComparator, freeString);

        const char *words[] = {
            "apple", "banana", "orange", "grape", "lemon"
        };
        const int wordCount = sizeof(words)/sizeof(words[0]);

        // Insert strings
        insertStrings(&sl, words, wordCount);
        checkStringsExist(&sl, words, wordCount);

        // Try inserting a duplicate
        char *dupTest = strdup("banana");
        bool inserted = slInsert(&sl, dupTest);
        assert(!inserted && "Duplicate insertion should fail");
        // We must free it ourselves because skip list won't store it
        free(dupTest);

        // Remove some of them
        removeStrings(&sl, words, 2); // remove "apple", "banana"
        bool found = slSearch(&sl, "apple");
        assert(!found && "apple should have been removed");
        found = slSearch(&sl, "banana");
        assert(!found && "banana should have been removed");

        // The others remain
        bool stillThere = slSearch(&sl, "orange");
        assert(stillThere);

        slFree(&sl);
        printf("String skip list test passed!\n");
    }

    /* ----------------
       Test 3: Structs
       ----------------*/
    {
        printf("Test 3: Struct skip list (MyPoint)...\n");
        // Create skip list for MyPoint structs
        SkipList sl;
        slInit(&sl, 12, 0.5f, pointComparator, freePoint);

        // We'll insert these points in random order
        MyPoint pointsToInsert[] = {
            {3, 4},
            {1, 2},
            {7, 1},
            {1, 1},
            {3, 6},
            {1, 2} // duplicate of an existing point
        };
        const int pointCount = sizeof(pointsToInsert) / sizeof(pointsToInsert[0]);

        // We'll insert the first 5 unique ones
        insertPoints(&sl, pointsToInsert, 5);

        // Check they exist
        checkPointsExist(&sl, pointsToInsert, 5);

        // Try to insert the 6th point, which duplicates {1, 2}
        bool inserted = slInsert(&sl, &pointsToInsert[5]);
        assert(!inserted && "Duplicate struct insertion should fail");

        // Remove a couple of points
        // We'll remove {1,1} and {3,6}
        MyPoint removeList[] = {
            {1, 1},
            {3, 6}
        };
        removePoints(&sl, removeList, 2);

        // Ensure they're gone
        bool found = slSearch(&sl, &removeList[0]);
        assert(!found && "{1,1} should have been removed");
        found = slSearch(&sl, &removeList[1]);
        assert(!found && "{3,6} should have been removed");

        // The others remain
        // That means {3,4}, {1,2}, and {7,1} still exist
        MyPoint stillThereList[] = {
            {3, 4},
            {1, 2},
            {7, 1}
        };
        checkPointsExist(&sl, stillThereList, 3);

        slFree(&sl);
        printf("Struct skip list test passed!\n");
    }

	runBenchmarks();
    printf("All SkipList tests passed!\n");
}
