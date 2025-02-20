#include "test_splay_tree.h"
#include "splay_tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* 
 * ============================
 *  COMPARATOR IMPLEMENTATIONS
 * ============================
 */
static int compareInt(const void* a, const void* b) {
    int aa = *(int*)a;
    int bb = *(int*)b;
    return (aa > bb) - (aa < bb); // returns 1, 0, or -1
}

static int compareFloat(const void* a, const void* b) {
    float fa = *(float*)a;
    float fb = *(float*)b;
    if (fa < fb) return -1;
    if (fa > fb) return 1;
    return 0;
}

static int compareString(const void* a, const void* b) {
    const char* sa = *(const char**)a;
    const char* sb = *(const char**)b;
    return strcmp(sa, sb);
}

/*
 * Example struct to test storing user-defined data.
 */
typedef struct {
    int id;
    float score;
} MyRecord;

static int compareMyRecord(const void* a, const void* b) {
    const MyRecord* ra = (const MyRecord*)a;
    const MyRecord* rb = (const MyRecord*)b;
    // Compare primarily by id, then by score if IDs are equal
    if (ra->id < rb->id) return -1;
    if (ra->id > rb->id) return 1;
    if (ra->score < rb->score) return -1;
    if (ra->score > rb->score) return 1;
    return 0;
}

/*
 * ===========================
 *  DESTRUCTOR IMPLEMENTATION
 * ===========================
 */
static void destroyString(void* data) {
    /* Each node stores a pointer to a dynamically allocated char* */
    char* str = *(char**)data;
    free(str); 
    /* Also free the pointer to char* itself if we allocated it. 
       But typically, you'd store strings differently. */
    free(data);
}

static void destroyMyRecord(void* data) {
    /* 
     * If we had dynamically allocated MyRecord, we'd free it.
     * For demonstration, let's assume it was malloc'ed.
     */
    free((MyRecord*)data);
}

/*
 * Helper to create a heap-allocated string object 
 * so we can store (char**) in the splay tree generically.
 */
static void* allocateString(const char* s) {
    char** ptr = (char**)malloc(sizeof(char*));
    *ptr = strdup(s);  // allocate copy of string
    return ptr;
}

/*
 * ===========================
 *        TEST FUNCTIONS
 * ===========================
 */

/* Test splay tree with integers */
static void testIntSplayTree(void) {
    SplayTree* tree = splayTreeCreate(compareInt, NULL);
    assert(tree != NULL);

    /* Insert some values */
    int values[] = {10, 5, 20, 15, 30, 25, 1};
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        int* valPtr = (int*)malloc(sizeof(int));
        *valPtr = values[i];
        int rc = splayTreeInsert(tree, valPtr);
        assert(rc == 1);
        /* After insert, size should be i+1 */
        assert(splayTreeSize(tree) == i + 1);
        /* Check BST property is maintained */
        assert(splayTreeIsValidBST(tree) == 1);
    }

    /* Search for existing value */
    int key = 15;
    int* result = (int*)splayTreeSearch(tree, &key);
    assert(result != NULL);
    assert(*result == 15);

    /* Search for missing value */
    key = 999;
    result = (int*)splayTreeSearch(tree, &key);
    assert(result == NULL);

    /* Delete a value */
    key = 20;
    int delRC = splayTreeDelete(tree, &key);
    assert(delRC == 1);
    assert(splayTreeSize(tree) == 6);

    /* Delete a non-existent value */
    key = 999;
    delRC = splayTreeDelete(tree, &key);
    assert(delRC == 0);
    assert(splayTreeSize(tree) == 6);

    /* Clean up */
    splayTreeDestroy(tree);
}

/* Test splay tree with floats */
static void testFloatSplayTree(void) {
    SplayTree* tree = splayTreeCreate(compareFloat, NULL);
    assert(tree != NULL);

    float values[] = {3.14f, 2.71f, 1.41f, 1.73f, 2.23f};
    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        float* valPtr = (float*)malloc(sizeof(float));
        *valPtr = values[i];
        int rc = splayTreeInsert(tree, valPtr);
        assert(rc == 1);
        assert(splayTreeIsValidBST(tree) == 1);
    }
    assert(splayTreeSize(tree) == 5);

    float key = 1.41f;
    float* result = (float*)splayTreeSearch(tree, &key);
    assert(result != NULL && *result == 1.41f);

    /* Clean up */
    splayTreeDestroy(tree);
}

/* Test splay tree with strings */
static void testStringSplayTree(void) {
    SplayTree* tree = splayTreeCreate(compareString, destroyString);
    assert(tree != NULL);

    /* Insert strings */
    const char* words[] = {"banana", "apple", "cherry", "date", "elderberry"};
    for (size_t i = 0; i < 5; i++) {
        void* strData = allocateString(words[i]);
        int rc = splayTreeInsert(tree, strData);
        assert(rc == 1);
        assert(splayTreeIsValidBST(tree) == 1);
    }
    assert(splayTreeSize(tree) == 5);

    /* Search for one string */
    const char* searchKey = "cherry";
    void* keyPtr = (void*)&searchKey;
    char** found = (char**)splayTreeSearch(tree, keyPtr);
    assert(found != NULL && strcmp(*found, "cherry") == 0);

    /* Delete one string */
    const char* deleteKey = "apple";
    void* delKeyPtr = (void*)&deleteKey;
    int delRC = splayTreeDelete(tree, delKeyPtr);
    assert(delRC == 1);
    assert(splayTreeSize(tree) == 4);

    /* Clean up */
    splayTreeDestroy(tree);
}

/* Test splay tree with a custom struct */
static void testStructSplayTree(void) {
    SplayTree* tree = splayTreeCreate(compareMyRecord, destroyMyRecord);
    assert(tree != NULL);

    /* Insert a few MyRecord objects */
    MyRecord recs[] = {
        { .id = 10, .score = 90.0f },
        { .id = 5,  .score = 72.5f },
        { .id = 7,  .score = 88.2f },
        { .id = 5,  .score = 99.9f }, /* same id as second, different score */
    };
    for (size_t i = 0; i < 4; i++) {
        MyRecord* r = (MyRecord*)malloc(sizeof(MyRecord));
        *r = recs[i];
        splayTreeInsert(tree, r);
        assert(splayTreeIsValidBST(tree) == 1);
    }

    /* Check size is 4 (assuming no deduping on same IDs with different scores) */
    assert(splayTreeSize(tree) == 4);

    /* Search for rec with (id=7, score=88.2f) */
    MyRecord query = { .id = 7, .score = 88.2f };
    MyRecord* found = (MyRecord*)splayTreeSearch(tree, &query);
    assert(found != NULL && found->id == 7 && found->score == 88.2f);

    /* Clean up */
    splayTreeDestroy(tree);
}

/*
 * A simple stress test that inserts a large number of random integers,
 * searches them, and then deletes them. We assert the tree always remains valid.
 */
static void stressTest(void) {
    const size_t N = 10000;

    // 1) Allocate and fill an array with unique values 0..N-1
    int* arr = (int*)malloc(N * sizeof(int));
    for (size_t i = 0; i < N; i++) {
        arr[i] = (int)i;
    }

    // 2) Shuffle them (Fisher-Yates shuffle)
    for (size_t i = 0; i < N - 1; i++) {
        size_t j = i + rand() % (N - i); 
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }

    // 3) Create your splay tree
    SplayTree* tree = splayTreeCreate(compareInt, free);
    assert(tree != NULL);

    // 4) Insert N unique elements
    for (size_t i = 0; i < N; i++) {
        int* valPtr = (int*)malloc(sizeof(int));
        *valPtr = arr[i];
        int rc = splayTreeInsert(tree, valPtr);
        assert(rc == 1);

        // Optionally check BST property periodically
        if (i % 1000 == 0) {
            assert(splayTreeIsValidBST(tree) == 1);
        }
    }

    // Now, since we inserted N unique elements, size should be exactly N
    assert(splayTreeSize(tree) == N);

    // 5) Search for random keys
    for (size_t i = 0; i < 1000; i++) {
        int key = rand() % (N * 2); // may or may not exist
        splayTreeSearch(tree, &key);
        // Optionally check BST property
        if (i % 100 == 0) {
            assert(splayTreeIsValidBST(tree) == 1);
        }
    }

    // 6) Delete half of them (randomly chosen)
    // size_t countDeleted = 0;
    for (size_t i = 0; i < N / 2; i++) {
        int key = rand() % N; // some key in [0, N-1]
        splayTreeDelete(tree, &key);
        // if (delRC == 1) {
        //     countDeleted++;
        // }
        if (i % 100 == 0) {
            assert(splayTreeIsValidBST(tree) == 1);
        }
    }

    // 7) Clean up
    free(arr);
    splayTreeDestroy(tree);
}

/*
 * =========================
 *   MAIN TEST ENTRY POINT
 * =========================
 */

void testSplayTree(void) {
    printf("Running int splay tree tests...\n");
    testIntSplayTree();
    printf("  [OK] int tests passed.\n\n");

    printf("Running float splay tree tests...\n");
    testFloatSplayTree();
    printf("  [OK] float tests passed.\n\n");

    printf("Running string splay tree tests...\n");
    testStringSplayTree();
    printf("  [OK] string tests passed.\n\n");

    printf("Running struct splay tree tests...\n");
    testStructSplayTree();
    printf("  [OK] struct tests passed.\n\n");

    printf("Running stress test...\n");
    stressTest();
    printf("  [OK] stress test passed.\n\n");

    printf("All tests passed!\n");
}
