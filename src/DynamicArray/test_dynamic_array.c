#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "test_dynamic_array.h"
#include "dynamic_array.h"

// A helper print function for generic data. We'll try to interpret
// the data as a string if it appears to have a null terminator
// (just as an example). Otherwise, we'll print hex bytes.
static void genericPrint(const void* data, size_t dataSize) {
    // Check if it's a string (ends with '\0')
    // We'll do a quick check: if the last byte is 0, assume string
    if (dataSize > 0) {
        const unsigned char* bytes = (const unsigned char*)data;
        if (bytes[dataSize - 1] == 0) {
            // Probably a string
            printf("\"%s\"", (const char*)data);
            return;
        }
    }
    // Otherwise, just print as hex
    printf("0x");
    const unsigned char* bytes = (const unsigned char*)data;
    for (size_t i = 0; i < dataSize; i++) {
        printf("%02X", bytes[i]);
    }
}

// A specialized print function for integer data (for demonstration)
static void intPrint(const void* data, size_t dataSize) {
    if (dataSize == sizeof(int)) {
        printf("%d", *(const int*)data);
    } else {
        // fallback
        genericPrint(data, dataSize);
    }
}

/***************************************
 * Individual Test Functions
 ***************************************/

// 1. Test with integers
static void testDynamicArrayWithInts(void) {
    printf("\n-- testDynamicArrayWithInts --\n");
    DynamicArray da;
    daInit(&da, 2);

    // Push a few integers
    int a = 10, b = 20, c = 30;
    daPushBack(&da, &a, sizeof(a));
    daPushBack(&da, &b, sizeof(b));
    daPushBack(&da, &c, sizeof(c));

    // Check size
    assert(daSize(&da) == 3);

    // Print them
    printf("Array contents (intPrint): ");
    daPrint(&da, intPrint);

    // Pop the last one
    int outVal = 0;
    size_t outSize = sizeof(outVal);
    bool popped = daPopBack(&da, &outVal, &outSize);
    assert(popped == true && outVal == 30 && outSize == sizeof(int));

    // Check size again
    assert(daSize(&da) == 2);

    daFree(&da);
    printf("testDynamicArrayWithInts passed!\n");
}

// 2. Test with strings
static void testDynamicArrayWithStrings(void) {
    printf("\n-- testDynamicArrayWithStrings --\n");
    DynamicArray da;
    daInit(&da, 2);

    const char* s1 = "Hello";
    const char* s2 = "World!";
    const char* s3 = "A long string maybe?";
    daPushBack(&da, s1, strlen(s1) + 1); // +1 for '\0'
    daPushBack(&da, s2, strlen(s2) + 1);
    daPushBack(&da, s3, strlen(s3) + 1);

    // Print using our generic function
    printf("Array contents (genericPrint): ");
    daPrint(&da, genericPrint);

    // Check size
    assert(daSize(&da) == 3);

    // Pop the last string
    char buffer[50];
    size_t outSize = sizeof(buffer);
    bool popped = daPopBack(&da, buffer, &outSize);
    assert(popped == true);
    // buffer should now contain "A long string maybe?"
    assert(strcmp(buffer, "A long string maybe?") == 0);

    // Now size is 2
    assert(daSize(&da) == 2);

    daFree(&da);
    printf("testDynamicArrayWithStrings passed!\n");
}

// 3. Test with a user-defined struct
typedef struct {
    int id;
    float x, y;
} Point;

static void testDynamicArrayWithStructs(void) {
    printf("\n-- testDynamicArrayWithStructs --\n");
    DynamicArray da;
    daInit(&da, 1);

    Point p1 = { .id = 1, .x = 10.0f, .y = 20.0f };
    Point p2 = { .id = 2, .x = -5.5f, .y = 3.14f };
    daPushBack(&da, &p1, sizeof(p1));
    daPushBack(&da, &p2, sizeof(p2));

    assert(daSize(&da) == 2);

    // Let's retrieve them
    const Point* r1 = (const Point*)daGet(&da, 0);
    const Point* r2 = (const Point*)daGet(&da, 1);
    assert(r1 && r1->id == 1 && r1->x == 10.0f && r1->y == 20.0f);
    assert(r2 && r2->id == 2 && r2->x == -5.5f && r2->y == 3.14f);

    // Print with genericPrint (it'll just show raw bytes, but that's fine)
    printf("Array contents (genericPrint): ");
    daPrint(&da, genericPrint);

    // Pop last
    Point outP = {0};
    size_t outSize = sizeof(outP);
    bool popped = daPopBack(&da, &outP, &outSize);
    assert(popped);
    assert(outP.id == 2);

    // Now size = 1
    assert(daSize(&da) == 1);

    daFree(&da);
    printf("testDynamicArrayWithStructs passed!\n");
}

static void testDynamicArrayResizing(void) {
    printf("\n-- testDynamicArrayResizing --\n");
    // Start with a very small initial capacity to ensure resizing happens quickly
    DynamicArray da;
    daInit(&da, 1);

    // We'll insert 20 integers, which definitely exceeds the initial capacity of 1
    int count = 20;
    for (int i = 0; i < count; i++) {
        daPushBack(&da, &i, sizeof(i));
        // Optionally, check that the size is correctly updated after each push
        assert(daSize(&da) == (size_t)(i + 1));
    }

    // Verify the contents are correct
    for (int i = 0; i < count; i++) {
        const int* valPtr = (const int*)daGet(&da, (size_t)i);
        assert(valPtr != NULL);
        assert(*valPtr == i && "Element mismatch after dynamic resizing");
    }

    // Now let's pop some elements to confirm they come out in the correct order
    for (int i = count - 1; i >= 10; i--) { 
        int outVal = 0;
        size_t outSize = sizeof(outVal);
        bool popped = daPopBack(&da, &outVal, &outSize);
        assert(popped == true);
        assert(outVal == i && "Popped value does not match expected");
        // size should decrement properly
        assert(daSize(&da) == (size_t)i);
    }

    // We should have 10 elements left (indices 0..9)
    assert(daSize(&da) == 10);

    // Double-check those remaining elements
    for (int i = 0; i < 10; i++) {
        const int* valPtr = (const int*)daGet(&da, (size_t)i);
        assert(valPtr != NULL);
        assert(*valPtr == i && "Element mismatch after popping some elements");
    }

    daFree(&da);
    printf("testDynamicArrayResizing passed!\n");
}


/***************************************
 * Driver to run all tests
 ***************************************/
void testDynamicArray(void) {
    testDynamicArrayWithInts();
    testDynamicArrayWithStrings();
    testDynamicArrayWithStructs();
    testDynamicArrayResizing();

    printf("\nAll DynamicArray tests passed successfully!\n");
}
