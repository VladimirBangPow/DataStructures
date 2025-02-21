#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "test_segtree.h"
#include "segtree.h"
#include "../DynamicArray/dynamic_array.h"

#include <time.h>



// -------------------- Data Structures for Testing -------------------- //

// 1) Integers: We'll do a sum merge
static void intMerge(const void* left, const void* right, void* out) {
    int l = *(const int*)left;
    int r = *(const int*)right;
    *(int*)out = l + r;
}

// 2) Strings: We'll do concatenation (very basic). We assume each node is a
//    fixed-size buffer of length 64 for demonstration. So elementSize=64.
static void stringMerge(const void* left, const void* right, void* out) {
    const char* strLeft  = (const char*)left;
    const char* strRight = (const char*)right;
    char*       strOut   = (char*)out;

    // We do a naive strncat with a maximum length
    // Clear out first
    strOut[0] = '\0';
    // In real usage, you'd do more robust checks (overflow, etc.)
    strncat(strOut, strLeft, 63);
    size_t leftLen = strlen(strOut);
    strncat(strOut + leftLen, strRight, 63 - leftLen);
}

// 3) A struct example: We'll define a simple struct with two ints
//    We'll merge by summing the fields
typedef struct {
    int x;
    int y;
} MyPair;

static void pairMerge(const void* left, const void* right, void* out) {
    const MyPair* l = (const MyPair*)left;
    const MyPair* r = (const MyPair*)right;
    MyPair* o       = (MyPair*)out;

    o->x = l->x + r->x;
    o->y = l->y + r->y;
}

// 4) A "set" example: We'll define a Set as a dynamic array of unique ints.
//    We'll store that in the segment tree. Merging is set union.
typedef struct {
    // For simplicity, store up to 16 ints in an array (no dynamic inside).
    // Alternatively, you could store a dynamic array for arbitrary size.
    int data[16];
    int size; // how many elements are used
} IntSet;

// Merge: set union
static void setUnion(const void* left, const void* right, void* out) {
    const IntSet* ls = (const IntSet*)left;
    const IntSet* rs = (const IntSet*)right;
    IntSet* os       = (IntSet*)out;

    // Start from left
    os->size = 0;
    for(int i=0; i<ls->size; i++){
        os->data[os->size++] = ls->data[i];
    }
    // Add elements from right if not already in os
    for(int j=0; j<rs->size; j++){
        int val = rs->data[j];
        // Check if val is in os
        bool found = false;
        for(int k=0; k<os->size; k++){
            if(os->data[k] == val){
                found = true;
                break;
            }
        }
        if(!found && os->size < 16){
            os->data[os->size++] = val;
        }
    }
}

// Helper to create an IntSet with a single element
static IntSet makeSet1(int val) {
    IntSet s;
    s.size = 1;
    s.data[0] = val;
    for(int i=1; i<16; i++){
        s.data[i] = 0;
    }
    return s;
}

// Helper to check if an IntSet contains an element
static bool setContains(const IntSet* s, int val) {
    for(int i=0; i<s->size; i++){
        if(s->data[i] == val){
            return true;
        }
    }
    return false;
}




// -------------------- Tests -------------------- //

// Test integers
static void testInts(void) {
    printf("Running testInts...\n");

    // Create an array of ints: [1, 2, 3, 4, 5]
    DynamicArray intData;
    daInit(&intData, 5);

    int arr[] = {1, 2, 3, 4, 5};
    for(size_t i=0; i<5; i++){
        daPushBack(&intData, &arr[i], sizeof(int));
    }

    // Build segment tree
    SegmentTree st;
    segtreeInit(&st, 5, sizeof(int), intMerge);
    segtreeBuild(&st, &intData);

    // Query sum of [0..4] -> should be 15
    int result = 0;
    bool ok = segtreeQuery(&st, 0, 4, &result);
    assert(ok);
    assert(result == 15);

    // Query sum of [1..3] -> 2+3+4 = 9
    result = 0;
    ok = segtreeQuery(&st, 1, 3, &result);
    assert(ok);
    assert(result == 9);

    // Update index 2 (the element '3') to '10'
    int newVal = 10;
    segtreeUpdate(&st, 2, &newVal, sizeof(int));

    // Query sum of [0..4] -> 1+2+10+4+5 = 22
    result = 0;
    ok = segtreeQuery(&st, 0, 4, &result);
    assert(ok);
    assert(result == 22);

    segtreeFree(&st);
    daFree(&intData);

    printf("testInts passed.\n");
}

// Test strings
static void testStrings(void) {
    printf("Running testStrings...\n");

    // We'll store each string in a fixed 64-byte buffer.
    // For demonstration, let's have 4 strings.

    DynamicArray strData;
    daInit(&strData, 4);

    char buf[64];

    memset(buf, 0, 64);
    strncpy(buf, "Hello", 63);
    daPushBack(&strData, buf, 64);

    memset(buf, 0, 64);
    strncpy(buf, " ", 63);
    daPushBack(&strData, buf, 64);

    memset(buf, 0, 64);
    strncpy(buf, "World", 63);
    daPushBack(&strData, buf, 64);

    memset(buf, 0, 64);
    strncpy(buf, "!!!", 63);
    daPushBack(&strData, buf, 64);

    SegmentTree st;
    segtreeInit(&st, 4, 64, stringMerge);
    segtreeBuild(&st, &strData);

    // Query [0..3] => "Hello World!!!"
    char result[64];
    memset(result, 0, 64);
    bool ok = segtreeQuery(&st, 0, 3, &result);
    assert(ok);
    assert(strcmp(result, "Hello World!!!") == 0);

    // Update index 3 => "???" instead of "!!!"
    memset(buf, 0, 64);
    strncpy(buf, "???", 63);
    segtreeUpdate(&st, 3, buf, 64);

    // Query [2..3] => "World???"
    memset(result, 0, 64);
    ok = segtreeQuery(&st, 2, 3, &result);
    assert(ok);
    assert(strcmp(result, "World???") == 0);

    segtreeFree(&st);
    daFree(&strData);

    printf("testStrings passed.\n");
}

// Test struct (MyPair)
static void testStruct(void) {
    printf("Running testStruct...\n");

    DynamicArray pairData;
    daInit(&pairData, 4);

    MyPair p1 = {1, 2};
    MyPair p2 = {3, 4};
    MyPair p3 = {5, 6};
    MyPair p4 = {7, 8};
    daPushBack(&pairData, &p1, sizeof(MyPair));
    daPushBack(&pairData, &p2, sizeof(MyPair));
    daPushBack(&pairData, &p3, sizeof(MyPair));
    daPushBack(&pairData, &p4, sizeof(MyPair));

    SegmentTree st;
    segtreeInit(&st, 4, sizeof(MyPair), pairMerge);
    segtreeBuild(&st, &pairData);

    // Query [0..3] -> (1+3+5+7, 2+4+6+8) = (16, 20)
    MyPair result;
    bool ok = segtreeQuery(&st, 0, 3, &result);
    assert(ok);
    assert(result.x == 16 && result.y == 20);

    // Update index 2 => {10, 10}
    MyPair newVal = {10, 10};
    segtreeUpdate(&st, 2, &newVal, sizeof(MyPair));

    // Query [0..3] -> (1+3+10+7, 2+4+10+8) = (21, 24)
    ok = segtreeQuery(&st, 0, 3, &result);
    assert(ok);
    assert(result.x == 21 && result.y == 24);

    segtreeFree(&st);
    daFree(&pairData);

    printf("testStruct passed.\n");
}

// Print a basic ASCII progress bar on one line
static void printProgressBar(size_t current, size_t total) {
    // For a nicer display, define a bar width
    const int barWidth = 50;

    // Calculate progress fraction
    double fraction = (double)current / (double)total;
    if (fraction > 1.0) fraction = 1.0;

    // Build bar string
    int filled = (int)(fraction * barWidth);

    printf("\r[");
    for (int i = 0; i < barWidth; i++) {
        if (i < filled) printf("=");
        else printf(" ");
    }
    printf("] %3d%%", (int)(fraction * 100));

    fflush(stdout);
}

static void testStressInts(void) {
    printf("Running stress test with integers...\n");

    srand((unsigned int)time(NULL)); // seed random

    // 1. Create a random array of integers
    const size_t NUM_ELEMS = 20000;  // Feel free to change
    const size_t NUM_OPS   = 100000; // Number of random ops

    DynamicArray data;
    daInit(&data, NUM_ELEMS);

    // Fill with random ints in range [0..99]
    for (size_t i = 0; i < NUM_ELEMS; i++) {
        int val = rand() % 100;
        daPushBack(&data, &val, sizeof(int));
    }

    // 2. Build segment tree
    SegmentTree st;
    segtreeInit(&st, NUM_ELEMS, sizeof(int), intMerge);
    segtreeBuild(&st, &data);

    // Immediately check validity after build
    assert(segtreeIsValidFull(&st) && "Segment Tree invalid right after build!");

    // 3. Perform NUM_OPS random updates or queries
    for (size_t i = 0; i < NUM_OPS; i++) {
        // Print progress bar
        printProgressBar(i, NUM_OPS);

        // 70% queries, 30% updates
        int op = rand() % 10;
        if (op < 7) {
            // Query operation
            size_t l = rand() % NUM_ELEMS;
            size_t r = rand() % NUM_ELEMS;
            if (l > r) {
                size_t temp = l; l = r; r = temp;
            }
            int queryResult = 0;
            bool ok = segtreeQuery(&st, l, r, &queryResult);
            assert(ok); 
            // We won't do anything with queryResult except verify it doesn't crash
        } else {
            // Update operation
            size_t pos = rand() % NUM_ELEMS;
            int newVal = rand() % 100;
            segtreeUpdate(&st, pos, &newVal, sizeof(int));
            // Reflect that update in data array as well, for consistency
            int* dataSlot = (int*)daGetMutable(&data, pos);
            *dataSlot = newVal;
        }

        // Check validity after every operation
        assert(segtreeIsValidFull(&st) && "Segment Tree invalid after an operation!");
    }

    // Final progress to 100%
    printProgressBar(NUM_OPS, NUM_OPS);
    printf("\nAll operations done.\n");

    // 4. Clean up
    segtreeFree(&st);
    daFree(&data);

    printf("Stress test passed.\n");
}


// Test sets
static void testSets(void) {
    printf("Running testSets...\n");

    DynamicArray setData;
    daInit(&setData, 4);

    // We'll store 4 sets
    IntSet s1 = makeSet1(1);
    IntSet s2 = makeSet1(2);
    IntSet s3 = makeSet1(3);
    IntSet s4 = makeSet1(4);

    daPushBack(&setData, &s1, sizeof(IntSet));
    daPushBack(&setData, &s2, sizeof(IntSet));
    daPushBack(&setData, &s3, sizeof(IntSet));
    daPushBack(&setData, &s4, sizeof(IntSet));

    SegmentTree st;
    segtreeInit(&st, 4, sizeof(IntSet), setUnion);
    segtreeBuild(&st, &setData);

    // Query [0..3] => union of {1}, {2}, {3}, {4} => {1,2,3,4}
    IntSet result;
    bool ok = segtreeQuery(&st, 0, 3, &result);
    assert(ok);
    assert(result.size == 4);
    assert(setContains(&result, 1));
    assert(setContains(&result, 2));
    assert(setContains(&result, 3));
    assert(setContains(&result, 4));

    // Update index 1 => {10}
    IntSet newSet = makeSet1(10);
    segtreeUpdate(&st, 1, &newSet, sizeof(IntSet));

    // Query [0..3] => union of {1}, {10}, {3}, {4} => size=4, elements=1,3,4,10
    ok = segtreeQuery(&st, 0, 3, &result);
    assert(ok);
    assert(result.size == 4);
    assert(setContains(&result, 1));
    assert(setContains(&result, 10));
    assert(setContains(&result, 3));
    assert(setContains(&result, 4));

    segtreeFree(&st);
    daFree(&setData);

    printf("testSets passed.\n");
}

// -------------------- Test Driver -------------------- //
void testSegTree(void)
{
    printf("=== Starting Segment Tree Tests ===\n");
    testInts();
    testStrings();
    testStruct();
    testSets();
    testStressInts();
    printf("=== All Segment Tree Tests Passed ===\n");
}
