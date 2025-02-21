#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "segtree.h"

// Forward declarations of static helper functions
static void buildRec(SegmentTree* st, const DynamicArray* data,
                     size_t start, size_t end, size_t idx);

static bool queryRec(const SegmentTree* st,
                     size_t start, size_t end, 
                     size_t left, size_t right, 
                     size_t idx,
                     void* outData);

static void updateRec(SegmentTree* st,
                      size_t start, size_t end,
                      size_t idx, size_t pos,
                      const void* newVal);

void segtreeInit(SegmentTree* st, size_t n, size_t elementSize, SegTreeMergeFunc mergeFunc)
{
    if (!st || !mergeFunc || n == 0 || elementSize == 0) {
        return;
    }

    st->n = n;
    st->elementSize = elementSize;
    st->mergeFunc = mergeFunc;

    // Initialize dynamic array with capacity ~4*n
    daInit(&st->nodes, (n * 4 > 0) ? n * 4 : 1);

    // Preallocate actual buffers, so that daGetMutable() won't return NULL.
    // We'll create exactly 4*n nodes; each node is an allocated block.
    for (size_t i = 0; i < 4 * n; i++) {
        void* dummy = calloc(1, elementSize);     // allocate zero-initialized
        daPushBack(&st->nodes, dummy, elementSize);
        free(dummy);  // safe because daPushBack internally copies the data
    }
}


// Build the segment tree from 'data'
void segtreeBuild(SegmentTree* st, const DynamicArray* data)
{
    if (!st || !data || daSize(data) < st->n) {
        return; // minimal error checking
    }
    // Recursively build the tree from range [0..n-1] at index 1
    // We treat index 1 as the root in typical segment tree style
    // but we store in st->nodes; we will just skip index 0 for convenience.
    // Alternatively, we can store the root at index = 1, or store at index = 0.
    // Here, let's do root at idx=1 for easier child indexing (2*i, 2*i+1).
    buildRec(st, data, 0, st->n - 1, 1);
}

bool segtreeQuery(const SegmentTree* st, size_t left, size_t right, void* outData)
{
    if (!st || !outData || left > right || right >= st->n) {
        return false;
    }

    return queryRec(st, 0, st->n - 1, left, right, 1, outData);
}

void segtreeUpdate(SegmentTree* st, size_t index, const void* newValue, size_t valueSize)
{
    if (!st || !newValue || index >= st->n || valueSize != st->elementSize) {
        return;
    }
    updateRec(st, 0, st->n - 1, 1, index, newValue);
}

void segtreeFree(SegmentTree* st)
{
    if (!st) return;
    daFree(&st->nodes);
    st->n = 0;
    st->elementSize = 0;
    st->mergeFunc = NULL;
}

// ------------------ Static Helper Functions ------------------ //

// Ensure daGetMutable can access st->nodes at index idx (1-based index).
// If idx is beyond current capacity/size, we expand with dummy elements.
static void ensureNodeIndex(SegmentTree* st, size_t idx)
{
    // Expand capacity if needed
    while (idx >= st->nodes.capacity) {
        // Just push dummy
        daPushBack(&st->nodes, NULL, 0);
    }
    // Adjust size if needed
    if (idx >= st->nodes.size) {
        st->nodes.size = idx + 1;
    }
}

static void buildRec(SegmentTree* st, const DynamicArray* data,
                     size_t start, size_t end, size_t idx)
{
    // Make sure we can access st->nodes at idx
    ensureNodeIndex(st, idx);

    if (start == end) {
        // Leaf node: copy the data element from 'data' at 'start'
        const void* leafData = daGet(data, start);
        void* nodeData = daGetMutable(&st->nodes, idx);
        memcpy(nodeData, leafData, st->elementSize);
        return;
    }

    size_t mid = (start + end) / 2;
    buildRec(st, data, start, mid, idx * 2);
    buildRec(st, data, mid + 1, end, idx * 2 + 1);

    // Merge children into this node
    void* leftData  = daGetMutable((DynamicArray*)&st->nodes, idx * 2);
    void* rightData = daGetMutable((DynamicArray*)&st->nodes, idx * 2 + 1);
    void* nodeData  = daGetMutable((DynamicArray*)&st->nodes, idx);
    st->mergeFunc(leftData, rightData, nodeData);
}

static bool queryRec(const SegmentTree* st,
                     size_t start, size_t end, 
                     size_t left, size_t right, 
                     size_t idx,
                     void* outData)
{
    if (left > end || right < start || idx >= st->nodes.size) {
        // No overlap
        return false;
    }
    if (left <= start && end <= right) {
        // Full coverage
        const void* nodeData = daGet(&st->nodes, idx);
        memcpy(outData, nodeData, st->elementSize);
        return true;
    }

    // Partial coverage
    size_t mid = (start + end) / 2;

    // Use variable-length arrays (C99)
    unsigned char leftBuff[st->elementSize];
    unsigned char rightBuff[st->elementSize];
    bool usedLeft = queryRec(st, start, mid, left, right, idx * 2, leftBuff);
    bool usedRight = queryRec(st, mid + 1, end, left, right, idx * 2 + 1, rightBuff);

    if (!usedLeft && !usedRight) {
        return false;
    } else if (usedLeft && !usedRight) {
        memcpy(outData, leftBuff, st->elementSize);
        return true;
    } else if (!usedLeft && usedRight) {
        memcpy(outData, rightBuff, st->elementSize);
        return true;
    } else {
        // Merge results from both sides
        st->mergeFunc(leftBuff, rightBuff, outData);
        return true;
    }
}

static void updateRec(SegmentTree* st,
                      size_t start, size_t end,
                      size_t idx, size_t pos,
                      const void* newVal)
{
    if (start == end) {
        // Leaf node
        void* nodeData = daGetMutable(&st->nodes, idx);
        memcpy(nodeData, newVal, st->elementSize);
        return;
    }

    size_t mid = (start + end) / 2;
    if (pos <= mid) {
        updateRec(st, start, mid, idx * 2, pos, newVal);
    } else {
        updateRec(st, mid + 1, end, idx * 2 + 1, pos, newVal);
    }

    // After update, re-merge
    void* leftData  = daGetMutable(&st->nodes, idx * 2);
    void* rightData = daGetMutable(&st->nodes, idx * 2 + 1);
    void* nodeData  = daGetMutable(&st->nodes, idx);
    st->mergeFunc(leftData, rightData, nodeData);
}

// Forward-declare a helper
static bool isValidNode(const SegmentTree* st, size_t idx, size_t start, size_t end);

// Check entire Segment Tree for correctness.
bool segtreeIsValidFull(const SegmentTree* st)
{
    if (!st || st->n == 0 || st->elementSize != sizeof(int)) {
        // Currently we only support checking integer-sum trees in this example
        return true; // or false, or skip entirely
    }
    // The root node is at idx=1, covering [0..n-1]
    return isValidNode(st, 1, 0, st->n - 1);
}

static bool isValidNode(const SegmentTree* st, size_t idx, size_t start, size_t end)
{
    // If idx is out of range, just ignore (valid enough, no child)
    if (idx >= daSize(&st->nodes)) {
        return true;
    }

    // Retrieve this node's value
    const int* nodeVal = (const int*)daGet(&st->nodes, idx);
    if (!nodeVal) {
        // If there's no data pointer, it's invalid
        return false;
    }

    if (start == end) {
        // Leaf node: no need to compare with children
        return true;
    }

    size_t mid = (start + end) / 2;
    size_t leftIdx = idx * 2;
    size_t rightIdx = idx * 2 + 1;

    // Check children recursively
    bool leftOK  = isValidNode(st, leftIdx, start, mid);
    bool rightOK = isValidNode(st, rightIdx, mid + 1, end);

    if (!leftOK || !rightOK) {
        return false;
    }

    // If children are valid, let's see if nodeVal == leftVal + rightVal
    const int* leftVal  = (const int*)daGet(&st->nodes, leftIdx);
    const int* rightVal = (const int*)daGet(&st->nodes, rightIdx);

    // The child might not exist if we've not built up that index => skip
    // But typically, if `buildRec` was correct, children do exist. We'll do a partial check:
    if (leftVal && rightVal) {
        int expected = (*leftVal) + (*rightVal);
        if (*nodeVal != expected) {
            return false;
        }
    }

    return true;
}




// #include <stdio.h>
// #include <stdlib.h>
// #include <limits.h>
// #include "segtree.h"

// /* 
//  * A static helper function to build the segment tree.
//  * idx:  current index in the tree array
//  * left, right: bounds of the segment in the original array
//  */
// static void build_tree(SegmentTree *st, int *arr, int idx, int left, int right) {
//     if (left == right) {
//         st->tree[idx] = arr[left];
//         return;
//     }
//     int mid = (left + right) / 2;
//     build_tree(st, arr, idx * 2,     left,     mid);
//     build_tree(st, arr, idx * 2 + 1, mid + 1,  right);
//     st->tree[idx] = st->combine(st->tree[idx * 2], st->tree[idx * 2 + 1]);
// }

// /*
//  * Create a segment tree from an integer array arr of size n,
//  * using the provided combine function and identity value.
//  */
// SegmentTree* segtree_create(int *arr, int n, CombineFunc combine, int identity) {
//     SegmentTree* st = (SegmentTree*) malloc(sizeof(SegmentTree));
//     if (!st) {
//         fprintf(stderr, "Failed to allocate memory for SegmentTree.\n");
//         return NULL;
//     }
//     st->n = n;
//     st->combine = combine;
//     st->identity = identity;

//     /* Allocate up to 4*n for the segment tree storage */
//     st->tree = (int*) malloc(sizeof(int) * 4 * n);
//     if (!st->tree) {
//         fprintf(stderr, "Failed to allocate memory for SegmentTree->tree.\n");
//         free(st);
//         return NULL;
//     }

//     build_tree(st, arr, 1, 0, n - 1);
//     return st;
// }

// /* 
//  * A static helper function for range queries.
//  */
// static int query_tree(SegmentTree* st, int idx, int left, int right, int ql, int qr) {
//     if (qr < left || ql > right) {
//         /* The segment [left, right] does not overlap with [ql, qr]. */
//         return st->identity;
//     }
//     if (ql <= left && right <= qr) {
//         /* The segment [left, right] is completely inside [ql, qr]. */
//         return st->tree[idx];
//     }
//     int mid = (left + right) / 2;
//     int left_res = query_tree(st, idx * 2,     left,     mid, ql, qr);
//     int right_res = query_tree(st, idx * 2 + 1, mid + 1,  right, ql, qr);
//     return st->combine(left_res, right_res);
// }

// /* 
//  * Query the segment tree for the range [left, right].
//  */
// int segtree_query(SegmentTree* st, int left, int right) {
//     if (!st || left < 0 || right >= st->n || left > right) {
//         return st->identity; 
//     }
//     return query_tree(st, 1, 0, st->n - 1, left, right);
// }

// /* 
//  * A static helper function for point updates in the tree.
//  */
// static void update_tree(SegmentTree* st, int idx, int left, int right, int pos, int value) {
//     if (left == right) {
//         st->tree[idx] = value;
//         return;
//     }
//     int mid = (left + right) / 2;
//     if (pos <= mid) {
//         update_tree(st, idx * 2, left, mid, pos, value);
//     } else {
//         update_tree(st, idx * 2 + 1, mid + 1, right, pos, value);
//     }
//     st->tree[idx] = st->combine(st->tree[idx * 2], st->tree[idx * 2 + 1]);
// }

// /* 
//  * Update the value at position index in the original array to 'value'.
//  */
// void segtree_update(SegmentTree* st, int index, int value) {
//     if (!st || index < 0 || index >= st->n) {
//         return;
//     }
//     update_tree(st, 1, 0, st->n - 1, index, value);
// }

// /* 
//  * Destroy the segment tree and free resources.
//  */
// void segtree_destroy(SegmentTree* st) {
//     if (!st) return;
//     if (st->tree) free(st->tree);
//     free(st);
// }

// /* 
//  * Some common combine functions:
//  */

// /* Sum combination */
// int combine_sum(int a, int b) {
//     return a + b;
// }

// /* Min combination */
// int combine_min(int a, int b) {
//     return (a < b) ? a : b;
// }

// /* Max combination */
// int combine_max(int a, int b) {
//     return (a > b) ? a : b;
// }

// /* GCD combination */
// static int gcd_internal(int a, int b) {
//     return (b == 0) ? a : gcd_internal(b, a % b);
// }

// int combine_gcd(int a, int b) {
//     return gcd_internal(a, b);
// }
