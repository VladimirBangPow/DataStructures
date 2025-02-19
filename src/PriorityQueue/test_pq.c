#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "test_pq.h"
#include "pq.h"

/**
 * Comparator for integers (min-heap).
 * Return < 0 if int a < int b,
 *         0 if a == b,
 *         > 0 if a > b.
 */
static int compareInt(const void* a, const void* b) {
    int ai = *(const int*)a;
    int bi = *(const int*)b;
    return (ai - bi);
}

/**
 * Comparator for floats (min-heap).
 */
static int compareFloat(const void* a, const void* b) {
    float af = *(const float*)a;
    float bf = *(const float*)b;
    if (af < bf) return -1;
    if (af > bf) return 1;
    return 0;
}

/**
 * Comparator for C-strings (lexicographical min-heap).
 */
static int compareString(const void* a, const void* b) {
    const char* sa = *(const char**)a; // Because we'll store a pointer to the string
    const char* sb = *(const char**)b;
    return strcmp(sa, sb);
}

/**
 * A helper to print an int array. 
 * Not strictly required, but can help in debugging.
 */
static void printInt(const void* data, size_t dataSize) {
    (void)dataSize; // not used
    printf("%d", *(const int*)data);
}

/**
 * Test the priority queue with integers.
 */
static void testIntegers(void) {
    printf("=== testIntegers ===\n");
    PriorityQueue pq;
    pqInit(&pq, compareInt, 0);

    // Insert a few integers
    int numbers[] = {10, 4, 15, 2, 8, 20};
    const size_t count = sizeof(numbers) / sizeof(numbers[0]);
    for (size_t i = 0; i < count; i++) {
        pqPush(&pq, &numbers[i], sizeof(int));
    }

    // The queue should be in min-heap order (2 is smallest)
    assert(pqSize(&pq) == count);
    int top = *(int*)pqTop(&pq);
    assert(top == 2);

    // Pop all elements and check ascending order
    int prev = -999999;
    size_t outSize;
    for (size_t i = 0; i < count; i++) {
        int poppedVal;
        outSize = sizeof(poppedVal);
        bool popped = pqPop(&pq, &poppedVal, &outSize);
        assert(popped);
        assert((int)outSize == (int)sizeof(int));

        // Make sure it's in ascending order
        assert(poppedVal >= prev);
        prev = poppedVal;
    }
    assert(pqIsEmpty(&pq));

    pqFree(&pq);
    printf("testIntegers passed!\n\n");
}

/**
 * Test the priority queue with floats.
 */
static void testFloats(void) {
    printf("=== testFloats ===\n");
    PriorityQueue pq;
    pqInit(&pq, compareFloat, 0);

    float values[] = {3.14f, 1.0f, 2.72f, -1.5f, 10.01f};
    const size_t count = sizeof(values) / sizeof(values[0]);

    for (size_t i = 0; i < count; i++) {
        pqPush(&pq, &values[i], sizeof(float));
    }
    assert(pqSize(&pq) == count);

    // Check top
    float topVal = *(float*)pqTop(&pq);
    assert(topVal == -1.5f);

    // Pop in ascending order
    float prev = -999999.0f;
    for (size_t i = 0; i < count; i++) {
        float poppedVal;
        size_t outSize = sizeof(poppedVal);
        bool popped = pqPop(&pq, &poppedVal, &outSize);
        assert(popped);
        assert(outSize == sizeof(float));
        assert(poppedVal >= prev);
        prev = poppedVal;
    }
    assert(pqIsEmpty(&pq));

    pqFree(&pq);
    printf("testFloats passed!\n\n");
}

/**
 * Test the priority queue with strings.
 * We'll store pointers to strings, so we must be careful about
 * how we push. One common approach is to store a (char *) in the PQ.
 * 
 * Because our dynamic_array is storing data by copy, we can't just do
 * `const char* txt = "hello"; pqPush(...)` with `&txt`. We actually
 * want to store `char *` in the array. 
 * 
 * Or we can store the entire string if we want. 
 * For the sake of demonstrating the comparator, let's store pointers.
 * We'll have a comparator that does strcmp on the pointed-to strings.
 */
static void testStrings(void) {
    printf("=== testStrings ===\n");
    PriorityQueue pq;
    pqInit(&pq, compareString, 0);

    const char* words[] = {"banana", "apple", "orange", "zzz", "aaa"};
    size_t count = sizeof(words)/sizeof(words[0]);

    // We will push pointers (char*) into the PQ. 
    // Because daPushBack copies the pointer, we can do this safely.
    for (size_t i = 0; i < count; i++) {
        // We'll push the pointer itself: 
        //   The data is &words[i], the size is sizeof(char*)
        // Then the PQ dynamic array will store one pointer (4 or 8 bytes).
        const char* tmp = words[i];
        pqPush(&pq, &tmp, sizeof(char*));
    }

    assert(pqSize(&pq) == count);
    // In lexicographical order, "aaa" is smallest, so top should be "aaa"
    const char** topStrPtr = (const char**)pqTop(&pq);
    assert(topStrPtr);
    assert(strcmp(*topStrPtr, "aaa") == 0);

    // Pop all, check ascending lexicographic order
    char* prev = NULL;
    for (size_t i = 0; i < count; i++) {
        const char* poppedWord;
        size_t outSize = sizeof(poppedWord);
        bool popped = pqPop(&pq, &poppedWord, &outSize);
        assert(popped);
        // If we want to ensure ascending order, we can do:
        if (prev) {
            assert(strcmp(poppedWord, prev) >= 0);
        }
        prev = (char*)poppedWord;
    }

    assert(pqIsEmpty(&pq));
    pqFree(&pq);
    printf("testStrings passed!\n\n");
}

/**
 * Stress test with random integers.
 * Insert a large number of random integers, then pop them all to verify
 * ascending order (if using a min-heap).
 */
static void testStress(void) {
    printf("=== testStress ===\n");
    PriorityQueue pq;
    pqInit(&pq, compareInt, 0);

    // Let's do 10000 random integers, for example
    const int TEST_SIZE = 10000;
    srand((unsigned)time(NULL));

    // Insert random ints
    for (int i = 0; i < TEST_SIZE; i++) {
        int r = rand();
        pqPush(&pq, &r, sizeof(int));
    }

    // Pop them all, ensure ascending order
    int prev = -1;
    size_t outSize = 0;
    for (int i = 0; i < TEST_SIZE; i++) {
        int poppedVal;
        outSize = sizeof(poppedVal);
        bool popped = pqPop(&pq, &poppedVal, &outSize);
        assert(popped);
        assert(outSize == sizeof(int));
        // The first pop we can't compare with prev if we want strict ascending,
        // but for subsequent ones we ensure poppedVal >= prev.
        assert(poppedVal >= prev);
        prev = poppedVal;
    }

    assert(pqIsEmpty(&pq));
    pqFree(&pq);

    printf("testStress passed!\n\n");
}

/**
 * Run all tests.
 */
void testPriorityQueue(void) {
    printf("=== Running Priority Queue Tests ===\n");
    testIntegers();
    testFloats();
    testStrings();
    testStress();
    printf("=== All tests passed! ===\n");  
}


