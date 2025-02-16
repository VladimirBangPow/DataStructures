#include "test_bp_tree.h"
#include "bp_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/* ---- Example comparison/copy/free for int ---- */
static int compareInt(const void *a, const void *b) {
    int x = *(const int*)a;
    int y = *(const int*)b;
    return (x > y) - (x < y); /* 1 if x>y, -1 if x<y, 0 if equal */
}
static void* copyInt(const void *data) {
    int *p = malloc(sizeof(int));
    *p = *(const int*)data;
    return p;
}
static void freeInt(void *data) {
    free(data);
}

/* ---- Example comparison/copy/free for double ---- */
static int compareDouble(const void *a, const void *b) {
    double x = *(const double*)a;
    double y = *(const double*)b;
    return (x > y) - (x < y);
}
static void* copyDouble(const void *data) {
    double *p = malloc(sizeof(double));
    *p = *(const double*)data;
    return p;
}
static void freeDouble(void *data) {
    free(data);
}

/* ---- Example comparison/copy/free for C-string ---- */
static int compareString(const void *a, const void *b) {
    return strcmp((const char*)a, (const char*)b);
}
static void* copyString(const void *data) {
    const char *src = (const char*)data;
    char *dst = malloc(strlen(src) + 1);
    strcpy(dst, src);
    return dst;
}
static void freeString(void *data) {
    free(data);
}

/* ---- Helper: Verify B+ Tree properties (very basic checks) ----
   For a real test, you'd check:
    1) All leaves are at the same depth
    2) Node key counts are within [min, max]
    3) The keys in each node are sorted
    4) The overall number of items is correct
   etc.
   Here we do a minimal example: just a structural DFS for leaves at same level.
*/
static int checkLeafLevels(const BPNode *node, int level, int *leafLevelFound) {
    if (!node) return 1; 
    if (node->isLeaf) {
        if (*leafLevelFound == -1) {
            *leafLevelFound = level;
        } else {
            /* If we find another leaf at a different level, fail */
            if (*leafLevelFound != level) return 0;
        }
        return 1;
    }
    for (int i = 0; i <= node->numKeys; i++) {
        const BPNode *child = node->children[i];
        if (child) {
            if (!checkLeafLevels(child, level + 1, leafLevelFound)) {
                return 0;
            }
        }
    }
    return 1;
}

/* A quick function to get the root node from BPTree, declared 'extern' in .c only. */
extern const void* getBPTreeRoot(const BPTree *tree); /* Not in .h - for test only */
const void* getBPTreeRoot(const BPTree *tree) {
    return (const void*)tree->root;
}

static void verifyBPTreeProperties(const BPTree *tree) {
    /* 1) All leaves at same level? */
    int leafLevelFound = -1;
    if (tree->root) {
        int ok = checkLeafLevels(tree->root, 0, &leafLevelFound);
        assert(ok && "B+ tree leaves are not at the same level!");
    }
    /* Additional checks could be done here. */
}

static void testIntOperations(void) {
    BPTree *tree = createBPTree(compareInt, copyInt, freeInt);
    assert(tree);

    /* Insert some integers */
    int values[] = {10, 20, 5, 15, 25, 8, 12, 3};
    for (int i = 0; i < 8; i++) {
        bpTreeInsert(tree, &values[i]);
        verifyBPTreeProperties(tree);
    }
    assert(bpTreeSize(tree) == 8);

    /* Search for them */
    int x = 10;
    void *found = bpTreeSearch(tree, &x);
    assert(found && "Should have found 10 in B+ tree");
    int y = *(int*)found;
    assert(y == 10);

    /* Delete a couple */
    x = 5;
    bpTreeDelete(tree, &x);
    verifyBPTreeProperties(tree);
    assert(bpTreeSize(tree) == 7);

    x = 25;
    bpTreeDelete(tree, &x);
    verifyBPTreeProperties(tree);
    assert(bpTreeSize(tree) == 6);

    /* Search for something not in the tree */
    x = 99;
    found = bpTreeSearch(tree, &x);
    assert(!found && "99 should not be found");

    destroyBPTree(tree);
}

static void testDoubleOperations(void) {
    BPTree *tree = createBPTree(compareDouble, copyDouble, freeDouble);
    assert(tree);

    double values[] = {1.1, 2.2, 3.3, 2.5, 1.05};
    for (int i = 0; i < 5; i++) {
        bpTreeInsert(tree, &values[i]);
        verifyBPTreeProperties(tree);
    }
    assert(bpTreeSize(tree) == 5);

    double key = 2.2;
    void *found = bpTreeSearch(tree, &key);
    assert(found);
    assert(*(double*)found == 2.2);

    /* Remove 3.3 */
    double k = 3.3;
    bpTreeDelete(tree, &k);
    verifyBPTreeProperties(tree);
    assert(bpTreeSize(tree) == 4);

    destroyBPTree(tree);
}

static void testStringOperations(void) {
    BPTree *tree = createBPTree(compareString, copyString, freeString);
    assert(tree);

    const char *words[] = {"apple", "banana", "cherry", "date", "banana"};
    for (int i = 0; i < 5; i++) {
        bpTreeInsert(tree, words[i]);
        verifyBPTreeProperties(tree);
    }
    /* If duplicates are stored, size=5, else size=4 if ignoring duplicates. */
    size_t s = bpTreeSize(tree);
    assert(s >= 4 && s <= 5);

    const char *query = "banana";
    void *found = bpTreeSearch(tree, query);
    assert(found && strcmp((char*)found, "banana") == 0);

    /* Remove 'banana' once (if duplicates exist, remove one instance) */
    bpTreeDelete(tree, query);
    verifyBPTreeProperties(tree);

    destroyBPTree(tree);
}

/* --------------- A STRESS TEST --------------- */

static void stressTestBPTree(void) {
    /* 
     * 1) Insert a large number of random integers 
     * 2) After each insert, verify structure 
     * 3) Then remove them in random order, verifying structure
     */
    const int N = 5000; /* scale up or down as needed */
    BPTree *tree = createBPTree(compareInt, copyInt, freeInt);
    assert(tree);

    /* We'll keep them in an array to randomize insertion and deletion order */
    int *arr = (int*)malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        arr[i] = rand() % (N * 10);
    }

    /* Insert in random order */
    for (int i = 0; i < N; i++) {
        bpTreeInsert(tree, &arr[i]);
        verifyBPTreeProperties(tree);
        assert(bpTreeSize(tree) == (size_t)(i+1));
    }

    /* Shuffle arr for random deletion order */
    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }

    /* Delete them one by one */
    for (int i = 0; i < N; i++) {
        bpTreeDelete(tree, &arr[i]);
        verifyBPTreeProperties(tree);
        assert(bpTreeSize(tree) == (size_t)(N - 1 - i));
    }

    free(arr);
    destroyBPTree(tree);
}

void testBPTree(void)
{
    srand((unsigned int)time(NULL));

    /* Basic typed tests with assertions */
    testIntOperations();
    testDoubleOperations();
    testStringOperations();

    /* Stress test for scale and correctness after each operation */
    stressTestBPTree();

    printf("All B+ tree tests PASSED.\n");
}
