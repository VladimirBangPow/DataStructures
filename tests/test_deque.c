#include <stdio.h>
#include <assert.h>
#include "deque.h"
#include "test_deque.h"
// A small helper to print integer data
static void printInt(const void* data) {
    printf("%d -> ", *(const int*)data);
}

/**
 * Test: Creating an empty deque and ensuring it behaves correctly (edge case tests).
 */
static void testEmptyDeque(void) {
    printf("\n--- testEmptyDeque ---\n");
    Deque dq;
    dequeInit(&dq);

    // The deque should be empty
    assert(dequeIsEmpty(&dq) && "Deque should be empty initially.");

    // Attempt to pop from front on empty
    int value = 0;
    bool popped = dequePopFront(&dq, &value);
    assert(!popped && "Popping front from empty deque should return false (0).");

    // Attempt to pop from back on empty
    popped = dequePopBack(&dq, &value);
    assert(!popped && "Popping back from empty deque should return false (0).");

    // Clean up
    dequeDestroy(&dq);
    printf("testEmptyDeque passed!\n");
}

/**
 * Test basic push-front/pop-front logic
 */
static void testPushFrontPopFront(void) {
    printf("\n--- testPushFrontPopFront ---\n");
    Deque dq;
    dequeInit(&dq);

    int nums[] = {10, 20, 30, 40};
    // Insert front in the order 10, 20, 30, 40
    // Final structure (front -> back): 40, 30, 20, 10
    for (int i = 0; i < 4; i++) {
        dequePushFront(&dq, &nums[i], sizeof(nums[i]));
    }
    dequePrint(&dq, printInt);  // Show the structure

    // Pop from front (should get 40, then 30, then 20, then 10)
    int outValue;
    bool popped;

    popped = dequePopFront(&dq, &outValue);
    assert(popped && outValue == 40);

    popped = dequePopFront(&dq, &outValue);
    assert(popped && outValue == 30);

    popped = dequePopFront(&dq, &outValue);
    assert(popped && outValue == 20);

    popped = dequePopFront(&dq, &outValue);
    assert(popped && outValue == 10);

    // Now empty again
    assert(dequeIsEmpty(&dq));

    // Clean up
    dequeDestroy(&dq);
    printf("testPushFrontPopFront passed!\n");
}

/**
 * Test basic push-back/pop-back logic
 */
static void testPushBackPopBack(void) {
    printf("\n--- testPushBackPopBack ---\n");
    Deque dq;
    dequeInit(&dq);

    int nums[] = {100, 200, 300};
    // Insert back in order: 100, 200, 300
    // Final structure: front-> 100 -> 200 -> 300 -> back
    for (int i = 0; i < 3; i++) {
        dequePushBack(&dq, &nums[i], sizeof(nums[i]));
    }
    dequePrint(&dq, printInt);

    // Pop from back (should get 300, then 200, then 100)
    int outValue;
    bool popped;

    popped = dequePopBack(&dq, &outValue);
    assert(popped && outValue == 300);

    popped = dequePopBack(&dq, &outValue);
    assert(popped && outValue == 200);

    popped = dequePopBack(&dq, &outValue);
    assert(popped && outValue == 100);

    // Now should be empty
    assert(dequeIsEmpty(&dq));

    // Clean up
    dequeDestroy(&dq);
    printf("testPushBackPopBack passed!\n");
}

/**
 * Test a mixed sequence of pushes and pops, front and back
 */
static void testMixedOperations(void) {
    printf("\n--- testMixedOperations ---\n");
    Deque dq;
    dequeInit(&dq);

    int x1 = 11, x2 = 22, x3 = 33, x4 = 44, x5 = 55;
    // Push front x1=11  -> front: 11
    dequePushFront(&dq, &x1, sizeof(x1));
    // Push front x2=22  -> front: 22, 11
    dequePushFront(&dq, &x2, sizeof(x2));
    // Push back x3=33   -> front: 22, 11, 33
    dequePushBack(&dq, &x3, sizeof(x3));
    // Push back x4=44   -> front: 22, 11, 33, 44
    dequePushBack(&dq, &x4, sizeof(x4));

    // Print current
    printf("Deque after some pushes: ");
    dequePrint(&dq, printInt);

    // Pop front (should get 22)
    int outValue;
    bool popped = dequePopFront(&dq, &outValue);
    assert(popped && outValue == 22);

    // Pop back (should get 44)
    popped = dequePopBack(&dq, &outValue);
    assert(popped && outValue == 44);

    // Now we have front: 11, 33
    // Push back x5=55 -> front: 11, 33, 55
    dequePushBack(&dq, &x5, sizeof(x5));

    printf("Deque after pops and another push: ");
    dequePrint(&dq, printInt);

    // Pop front (11), then front again (33), then front again (55)
    popped = dequePopFront(&dq, &outValue);
    assert(popped && outValue == 11);

    popped = dequePopFront(&dq, &outValue);
    assert(popped && outValue == 33);

    popped = dequePopFront(&dq, &outValue);
    assert(popped && outValue == 55);

    // Should be empty
    assert(dequeIsEmpty(&dq));

    // Clean up
    dequeDestroy(&dq);
    printf("testMixedOperations passed!\n");
}

/**
 * Test boundary conditions or unusual sequences (e.g., push/pop with a single item).
 */
static void testEdgeCases(void) {
    printf("\n--- testEdgeCases ---\n");
    Deque dq;
    dequeInit(&dq);

    int val = 999;
    // Immediately pop from empty
    int outValue;
    assert(!dequePopFront(&dq, &outValue));
    assert(!dequePopBack(&dq, &outValue));

    // Push one item front
    dequePushFront(&dq, &val, sizeof(val));
    dequePrint(&dq, printInt);

    // Pop it from the back (same item)
    assert(dequePopBack(&dq, &outValue));
    assert(outValue == val);

    // Should be empty now
    assert(dequeIsEmpty(&dq));

    // Push one item back
    dequePushBack(&dq, &val, sizeof(val));
    // Pop it from the front
    assert(dequePopFront(&dq, &outValue));
    assert(outValue == val);

    // Confirm empty
    assert(dequeIsEmpty(&dq));

    dequeDestroy(&dq);
    printf("testEdgeCases passed!\n");
}

/**
 * "Driver" function that calls all of the above test functions. 
 * This is *not* a main(), so you can call it from anywhere else as needed.
 */
void testDeque(void) {
    testEmptyDeque();
    testPushFrontPopFront();
    testPushBackPopBack();
    testMixedOperations();
    testEdgeCases();

    printf("\nAll Deque tests passed successfully!\n");
}






