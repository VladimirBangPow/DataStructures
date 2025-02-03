#include "test_skip_list.h"
#include "skip_list.h"
#include "../LinkedList/linkedlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <string.h>

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


// Here is the "deep" bench function showing ratio analysis
void deepBench(void) {
    // We'll test at consecutive sizes that double each step
    int sizes[] = { 1000, 2000, 4000, 8000, 16000, 32000 };
    int numSizes = sizeof(sizes)/sizeof(sizes[0]);

    double skipTimes[6], listTimes[6];

    for(int i=0; i<numSizes; i++) {
        int n = sizes[i];

        // 1. Prepare data
        int *values = (int*) malloc(n * sizeof(int));
        for(int j=0; j<n; j++) values[j] = j;
        shuffle(values, n);

        // 2. Time SkipList insertion
        SkipList sl;
        slInit(&sl, 12, 0.5f, intComparator, free);
        double start = get_time_seconds();
        for(int j=0; j<n; j++) {
            int *valPtr = (int*)malloc(sizeof(int));
            *valPtr = values[j];
            slInsert(&sl, valPtr);
        }
        double end = get_time_seconds();
        skipTimes[i] = end - start;
        slFree(&sl);

        // 3. Time LinkedList insertion (sorted)
        // reshuffle or rebuild
        for(int j=0; j<n; j++) values[j] = j;
        shuffle(values, n);
        Node* head = NULL;
        start = get_time_seconds();
        for(int j=0; j<n; j++) {
            insertInSortedOrder(&head, &values[j], sizeof(int), intComparator);
        }
        end = get_time_seconds();
        listTimes[i] = end - start;
        freeList(&head);

        free(values);
    }

    // Print the results
    printf("\n-----------------------------------------------\n");
    printf("   n    SkipT(s)  ListT(s)  (List/Skip) Ratio\n");
    printf("-----------------------------------------------\n");
    for(int i=0; i<numSizes; i++) {
        double ratio = listTimes[i] / skipTimes[i];
        printf("%5d  %8.4f  %8.4f   %10.2f\n",
            sizes[i], skipTimes[i], listTimes[i], ratio);
    }

    // Now do the doubling ratio analysis:
    // T_skip(2n)/T_skip(n) vs. (2n * log(2n)) / (n * log(n)) => ~2 for large n
    // T_list(2n)/T_list(n) vs. 4
    printf("\nDouble-Ratio Analysis:\n");
    printf("Compare T(2n)/T(n) to theoretical ratio.\n\n");

    printf("Index |   n->2n    | skip(2n)/skip(n) | skip_theory | ratio_of_ratios(skip)\n");
    printf("--------------------------------------------------------------------------\n");
    for(int i=0; i<numSizes-1; i++) {
        double n1 = (double)sizes[i];
        double n2 = (double)sizes[i+1];
        double empirical = skipTimes[i+1] / skipTimes[i];
        double theory = (n2 * log(n2)) / (n1 * log(n1)); // n log n model
        double ratioOfRatios = empirical / theory;

        printf("  %2d   %5.0f->%5.0f    %10.3f     %10.3f      %10.3f\n",
               i, n1, n2, empirical, theory, ratioOfRatios);
    }

    printf("\nIndex |   n->2n    | list(2n)/list(n) | list_theory=4 | ratio_of_ratios(list)\n");
    printf("----------------------------------------------------------------------\n");
    for(int i=0; i<numSizes-1; i++) {
        double empirical = listTimes[i+1] / listTimes[i];
        double ratioOfRatios = empirical / 4.0; // n^2 => factor of 4 if we double n
        printf("  %2d   %5d->%5d    %10.3f       %10.3f\n",
               i, sizes[i], sizes[i+1], empirical, ratioOfRatios);
    }

    // Meaningful assertions:
    // 1) For large n, skipTimes must be < listTimes (skip is faster).
    // 2) The ratio skip(2n)/skip(n) should approach ~2, the ratio list(2n)/list(n) ~4
    //    or at least skip < 3 while list > 3 for large n. You can be creative:
    int last = numSizes - 1;
    assert(skipTimes[last] < listTimes[last] && "SkipList must be faster at largest n!");

    // We can also check ratio of doubling:
    for(int i=1; i<numSizes; i++){
        double skipDoubling = skipTimes[i] / skipTimes[i-1];
        double listDoubling = listTimes[i] / listTimes[i-1];
        // We expect skipDoubling < listDoubling for large i:
        if(sizes[i] >= 8000) {
            assert(skipDoubling < listDoubling &&
                   "SkipList's doubling ratio should be smaller than the linked list's at large n!");
        }
    }
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

	deepBench();

    printf("All SkipList tests passed!\n");
}
