#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "pq.h"

/* 
 * =============== HELPER COMPARATORS ===============
 * These comparison functions interpret "a < b" as negative,
 * "a == b" as 0, "a > b" as positive. 
 * Perfect for min-heap usage by default. 
 * We will pass isMinHeap=false if we want a max-heap.
 */
static int compareInt(const void* a, const void* b) {
    int ai = *(const int*)a;
    int bi = *(const int*)b;
    return ai - bi;
}

static int compareFloat(const void* a, const void* b) {
    float af = *(const float*)a;
    float bf = *(const float*)b;
    if (af < bf) return -1;
    if (af > bf) return 1;
    return 0;
}

static int compareString(const void* a, const void* b) {
    // We store pointers to C-strings in the PQ
    const char* sa = *(const char**)a;
    const char* sb = *(const char**)b;
    return strcmp(sa, sb);
}

/* 
 * =============== HEAP INVARIANT CHECK ===============
 * Ensure that for every node i, the parent–child relationship
 * is maintained according to isMinHeap or isMaxHeap.
 */
static void checkHeapInvariant(const PriorityQueue* pq) {
    size_t n = pqSize(pq);
    // For each parent i, check left child (2i+1) and right child (2i+2)
    for (size_t i = 0; i < n; i++) {
        size_t left = 2*i + 1;
        size_t right = 2*i + 2;
        const void* parentData = daGet(&pq->da, i); // direct from underlying array
        if (left < n) {
            const void* leftData = daGet(&pq->da, left);
            // Compare parent and left child
            int cmp = pq->userCompareFunc(parentData, leftData);
            if (pq->isMinHeap) {
                // parent <= child => compare(parent, child) <= 0
                assert(cmp <= 0 && "Min-heap invariant violated (parent > left child)!");
            } else {
                // max-heap => parent >= child => compare(parent, child) >= 0
                assert(cmp >= 0 && "Max-heap invariant violated (parent < left child)!");
            }
        }
        if (right < n) {
            const void* rightData = daGet(&pq->da, right);
            // Compare parent and right child
            int cmp = pq->userCompareFunc(parentData, rightData);
            if (pq->isMinHeap) {
                assert(cmp <= 0 && "Min-heap invariant violated (parent > right child)!");
            } else {
                assert(cmp >= 0 && "Max-heap invariant violated (parent < right child)!");
            }
        }
    }
}

/* 
 * =============== TEST FUNCTIONS ===============
 *
 * We'll create an "internal" version of each test that takes isMinHeap. 
 * Then we'll call these internal functions for both true (min) and false (max).
 */

/* ---------- Test with integers ---------- */

static void testIntegersInternal(bool isMinHeap) {
    printf("=== testIntegers (isMinHeap=%s) ===\n", isMinHeap?"true":"false");

    PriorityQueue pq;
    pqInit(&pq, compareInt, isMinHeap, 0);

    int numbers[] = {10, 4, 15, 2, 8, 20};
    size_t count = sizeof(numbers)/sizeof(numbers[0]);

    // Push them all
    for (size_t i = 0; i < count; i++) {
        pqPush(&pq, &numbers[i], sizeof(int));
        // Check structure after each push
        checkHeapInvariant(&pq);
    }
    assert(pqSize(&pq) == count);

    // If min-heap, top should be the smallest => 2
    // If max-heap, top should be the largest => 20
    int topVal = *(int*)pqTop(&pq);
    int expectedTop = isMinHeap ? 2 : 20;
    assert(topVal == expectedTop);

    // Pop all elements. 
    // For min-heap, they should come out in ascending order.
    // For max-heap, they should come out in descending order.
    int prev = isMinHeap ? -999999 : 999999; 
    for (size_t i = 0; i < count; i++) {
        int poppedVal;
        size_t outSize = sizeof(poppedVal);
        bool popped = pqPop(&pq, &poppedVal, &outSize);
        assert(popped);
        checkHeapInvariant(&pq); // verify structure after each pop
        assert(outSize == sizeof(int));

        if (isMinHeap) {
            assert(poppedVal >= prev && "Ascending order violated for min-heap");
            prev = poppedVal;
        } else {
            assert(poppedVal <= prev && "Descending order violated for max-heap");
            prev = poppedVal;
        }
    }

    assert(pqIsEmpty(&pq));
    pqFree(&pq);

    printf("testIntegers (isMinHeap=%s) passed!\n\n", isMinHeap?"true":"false");
}

/* ---------- Test with floats ---------- */

static void testFloatsInternal(bool isMinHeap) {
    printf("=== testFloats (isMinHeap=%s) ===\n", isMinHeap?"true":"false");

    PriorityQueue pq;
    pqInit(&pq, compareFloat, isMinHeap, 0);

    float values[] = {3.14f, 1.0f, 2.72f, -1.5f, 10.01f};
    size_t count = sizeof(values) / sizeof(values[0]);

    for (size_t i = 0; i < count; i++) {
        pqPush(&pq, &values[i], sizeof(float));
        checkHeapInvariant(&pq);
    }
    assert(pqSize(&pq) == count);

    // Top check
    // For min-heap => -1.5
    // For max-heap => 10.01
    float topVal = *(float*)pqTop(&pq);
    float expected = isMinHeap ? -1.5f : 10.01f;
    assert(topVal == expected);

    // Pop in ascending (min) or descending (max) order
    float prev = isMinHeap ? -999999.0f : 9999999.0f;
    for (size_t i = 0; i < count; i++) {
        float poppedVal;
        size_t outSize = sizeof(poppedVal);
        bool popped = pqPop(&pq, &poppedVal, &outSize);
        assert(popped);
        checkHeapInvariant(&pq);
        assert(outSize == sizeof(float));

        if (isMinHeap) {
            assert(poppedVal >= prev && "Ascending order violated for min-heap");
            prev = poppedVal;
        } else {
            assert(poppedVal <= prev && "Descending order violated for max-heap");
            prev = poppedVal;
        }
    }

    assert(pqIsEmpty(&pq));
    pqFree(&pq);

    printf("testFloats (isMinHeap=%s) passed!\n\n", isMinHeap?"true":"false");
}

/* ---------- Test with strings ---------- */

static void testStringsInternal(bool isMinHeap) {
    printf("=== testStrings (isMinHeap=%s) ===\n", isMinHeap?"true":"false");

    PriorityQueue pq;
    pqInit(&pq, compareString, isMinHeap, 0);

    const char* words[] = {"banana", "apple", "orange", "zzz", "aaa"};
    size_t count = sizeof(words)/sizeof(words[0]);

    // Push each pointer
    // In the PQ we are storing (char*) copies
    for (size_t i = 0; i < count; i++) {
        const char* tmp = words[i];
        pqPush(&pq, &tmp, sizeof(char*));
        checkHeapInvariant(&pq);
    }
    assert(pqSize(&pq) == count);

    // For min-heap, top is lexicographically smallest => "aaa"
    // For max-heap, top is lexicographically largest => "zzz"
    const char** topStrPtr = (const char**)pqTop(&pq);
    assert(topStrPtr);
    const char* expected = isMinHeap ? "aaa" : "zzz";
    assert(strcmp(*topStrPtr, expected) == 0);

    // Pop all
    // For min-heap => ascending lexicographic
    // For max-heap => descending lexicographic
    const char* prev = isMinHeap ? NULL : "\x7F"; // something "bigger than zzz" for descending
    // (We can do "~", or any high ASCII sentinel. Using \x7F or similar to be safe)
    
    for (size_t i = 0; i < count; i++) {
        const char* poppedWord;
        size_t outSize = sizeof(poppedWord);
        bool popped = pqPop(&pq, &poppedWord, &outSize);
        assert(popped);
        checkHeapInvariant(&pq);

        if (isMinHeap) {
            // Must be >= the previous word lexicographically
            if (prev) {
                assert(strcmp(poppedWord, prev) >= 0);
            }
            prev = poppedWord;
        } else {
            // Must be <= the previous word lexicographically
            // For the first iteration, prev is \x7F
            assert(strcmp(poppedWord, prev) <= 0);
            prev = poppedWord;
        }
    }

    assert(pqIsEmpty(&pq));
    pqFree(&pq);

    printf("testStrings (isMinHeap=%s) passed!\n\n", isMinHeap?"true":"false");
}

/* ---------- Stress test with random integers ---------- */

static void testStressInternal(bool isMinHeap) {
    printf("=== testStress (isMinHeap=%s) ===\n", isMinHeap?"true":"false");

    PriorityQueue pq;
    pqInit(&pq, compareInt, isMinHeap, 0);

    // Let's use a smaller test size if we are verifying after every operation
    // because doing 1 million push/pops + invariants is expensive.
    const int TEST_SIZE = 30000;
    srand((unsigned)time(NULL));

    // Insert random ints, checking structure each time
    for (int i = 0; i < TEST_SIZE; i++) {
        int r = rand();
        pqPush(&pq, &r, sizeof(int));
        checkHeapInvariant(&pq);
    }

    // Pop them all, checking the order and structure
    // For min-heap => ascending
    // For max-heap => descending
    int prev = isMinHeap ? -2147483647 : 2147483647; // extremes
    for (int i = 0; i < TEST_SIZE; i++) {
        int poppedVal;
        size_t outSize = sizeof(poppedVal);
        bool popped = pqPop(&pq, &poppedVal, &outSize);
        assert(popped);
        assert(outSize == sizeof(int));
        checkHeapInvariant(&pq);

        if (isMinHeap) {
            // ascending
            assert(poppedVal >= prev);
            prev = poppedVal;
        } else {
            // descending
            assert(poppedVal <= prev);
            prev = poppedVal;
        }
    }

    assert(pqIsEmpty(&pq));
    pqFree(&pq);
    printf("testStress (isMinHeap=%s) passed!\n\n", isMinHeap?"true":"false");
}

/* 
 * =============== MASTER TEST ===============
 * Call each test in both min-heap and max-heap modes.
 */
void testPriorityQueue(void) {
    printf("=== Running Priority Queue Tests ===\n\n");
    
    // 1) Integers
    testIntegersInternal(true);   // min-heap
    testIntegersInternal(false);  // max-heap

    // 2) Floats
    testFloatsInternal(true);
    testFloatsInternal(false);

    // 3) Strings
    testStringsInternal(true);
    testStringsInternal(false);

    // 4) Stress Test
    testStressInternal(true);
    testStressInternal(false);

    printf("=== All tests passed! ===\n");
}
