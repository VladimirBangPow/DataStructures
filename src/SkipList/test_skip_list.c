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

void printInt(const void *data)
{
    // Cast the void * back to an int *
    printf("%d", *(int *)data);
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



// Simple shuffle function
static void shuffleArray(int *array, int n)
{
    for (int i = 0; i < n - 1; i++) {
        int j = i + rand() / (RAND_MAX / (n - i) + 1);
        int temp = array[j];
        array[j] = array[i];
        array[i] = temp;
    }
}

// A stress test that inserts n integers (0..n-1) in random order, 
// then searches them in random order, and finally removes them in random order.
void stressTestSkipListInt(int n)
{
    // Seed the RNG for reproducibility; you can also do srand(time(NULL)) globally
    srand((unsigned)time(NULL));

    // 1. Allocate an array of n integers
    int *values = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        values[i] = i;
    }

    // 2. Create and initialize a skip list
    SkipList sl;
    // maxLevel = 12, probability = 0.5 for typical usage
    // freeInt is the free function for integer pointers
    slInit(&sl, 12, 0.5f, intComparator, freeInt);

    // 3. Shuffle and insert all values in random order
    shuffleArray(values, n);
    for (int i = 0; i < n; i++) {
        int *valPtr = (int *)malloc(sizeof(int));
        *valPtr = values[i];
        bool inserted = slInsert(&sl, valPtr);
        assert(inserted && "Insertion failed for a unique integer.");
    }

    // 4. Shuffle and search all values in random order
    shuffleArray(values, n);
    for (int i = 0; i < n; i++) {
        bool found = slSearch(&sl, &values[i]);
        assert(found && "Value should be found after insertion.");
    }

    // 5. Shuffle and remove all values in random order
    shuffleArray(values, n);
    for (int i = 0; i < n; i++) {
        bool removed = slRemove(&sl, &values[i]);
        assert(removed && "Value should be removable since it exists in the skip list.");
    }

    // 6. Verify that none of them remain
    //    (Optional shuffle again, but not strictly needed to check emptiness)
    shuffleArray(values, n);
    for (int i = 0; i < n; i++) {
        bool found = slSearch(&sl, &values[i]);
        assert(!found && "Value should not be found after removal.");
    }

    // 7. Cleanup
    slFree(&sl);
    free(values);

    printf("Stress test with %d items PASSED.\n", n);
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

	stressTestSkipListInt(1000000);
    printf("All SkipList tests passed!\n");
}
