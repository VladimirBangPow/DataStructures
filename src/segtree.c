#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "segtree.h"

// Forward declarations of static helper functions
static void _buildSegTree(SegmentTree* st, const DynamicArray* data,
                          size_t start, size_t end, size_t idx);

static bool _querySegTree(const SegmentTree* st,
                          size_t start, size_t end, 
                          size_t left, size_t right, 
                          size_t idx,
                          void* outData);

static void _updateSegTree(SegmentTree* st,
                           size_t start, size_t end,
                           size_t idx, size_t pos,
                           const void* newVal);

/**
 * segtreeInit:
 * ------------
 * Initializes a SegmentTree structure.
 *
 * Parameters:
 *  - st:           Pointer to a SegmentTree object to initialize.
 *  - n:            Number of elements in the original array the segment tree will represent.
 *  - elementSize:  The size (in bytes) of each element that the segment tree will store.
 *  - mergeFunc:    A user-supplied function that merges the data of two children nodes.
 *
 * Behavior:
 *  - Checks basic parameter validity (e.g., st is non-null, n > 0, etc.).
 *  - Stores n, elementSize, and mergeFunc in the SegmentTree.
 *  - Initializes the underlying DynamicArray (st->nodes) with a starting capacity of 'n'.
 *    (This capacity can be expanded later as needed.)
 *
 * Note:
 *  - This function does NOT build the tree itself. Call segtreeBuild() afterwards to populate it.
 */
void segtreeInit(SegmentTree* st, size_t n, size_t elementSize, SegTreeMergeFunc mergeFunc)
{
    if (!st || !mergeFunc || n == 0 || elementSize == 0) {
        return;
    }

    st->n = n;
    st->elementSize = elementSize;
    st->mergeFunc = mergeFunc;

    // Initialize dynamic array with capacity = n
    daInit(&st->nodes, n);
}

/**
 * segtreeBuild:
 * -------------
 * Builds the segment tree from the contents of a DynamicArray.
 *
 * Parameters:
 *  - st:   Pointer to an initialized SegmentTree (via segtreeInit).
 *  - data: Pointer to a DynamicArray that holds at least 'st->n' elements.
 *
 * Behavior:
 *  - Recursively constructs the tree nodes for the range [0..(n-1)] at segment-tree index 1.
 *  - Uses a helper function _buildSegTree() to handle the recursion.
 *  - Each leaf node will store exactly one element from 'data', and each internal node
 *    will store the merged result (using st->mergeFunc) of its two children.
 *
 * Note:
 *  - If 'data' has fewer than 'st->n' elements, this function returns immediately (invalid input).
 *  - The root of the tree is stored at index 1 in 'st->nodes' (index 0 is unused).
 */
void segtreeBuild(SegmentTree* st, const DynamicArray* data)
{
    if (!st || !data || daSize(data) < st->n) {
        return; // minimal error checking
    }

    // Build the tree covering [0..n-1] with root at idx=1
    _buildSegTree(st, data, 0, st->n - 1, 1);
}

/**
 * segtreeQuery:
 * -------------
 * Performs a range query on the segment tree for the interval [left..right].
 *
 * Parameters:
 *  - st:      Pointer to a SegmentTree.
 *  - left:    The left boundary (inclusive) of the query range (0-based index).
 *  - right:   The right boundary (inclusive) of the query range (0-based index).
 *  - outData: A pointer to a buffer where the result of the query will be stored.
 *             This buffer must be at least 'st->elementSize' bytes.
 *
 * Returns:
 *  - true if the query was successful and some overlap existed.
 *  - false if the input was invalid or no coverage of [left..right] was found.
 *
 * Behavior:
 *  - Uses the internal _querySegTree function to traverse the segment tree nodes
 *    that overlap with [left..right].
 *  - Merges data from overlapping segments using st->mergeFunc.
 *  - Copies the final result into 'outData' if there is any overlap.
 */
bool segtreeQuery(const SegmentTree* st, size_t left, size_t right, void* outData)
{
    if (!st || !outData || left > right || right >= st->n) {
        return false;
    }

    return _querySegTree(st, 0, st->n - 1, left, right, 1, outData);
}

/**
 * segtreeUpdate:
 * --------------
 * Updates a single element in the segment tree at the given index.
 *
 * Parameters:
 *  - st:        Pointer to a SegmentTree.
 *  - index:     The index of the leaf element to update (0-based).
 *  - newValue:  Pointer to the new data to store at 'index'.
 *  - valueSize: The size in bytes of 'newValue' (must match st->elementSize).
 *
 * Behavior:
 *  - If 'index' is within range [0..n-1] and valueSize matches st->elementSize,
 *    the leaf node corresponding to 'index' is updated to 'newValue'.
 *  - Recursively re-merges internal nodes up the tree so that parents reflect
 *    the updated data.
 *  - Uses the helper function _updateSegTree to perform the recursive update.
 */
void segtreeUpdate(SegmentTree* st, size_t index, const void* newValue, size_t valueSize)
{
    if (!st || !newValue || index >= st->n || valueSize != st->elementSize) {
        return;
    }
    _updateSegTree(st, 0, st->n - 1, 1, index, newValue);
}

/**
 * segtreeFree:
 * ------------
 * Frees all internal resources used by the segment tree.
 *
 * Parameters:
 *  - st: Pointer to a SegmentTree.
 *
 * Behavior:
 *  - Calls daFree on the st->nodes dynamic array, freeing all memory for tree nodes.
 *  - Resets st->n, st->elementSize, and st->mergeFunc to 0 or NULL.
 */
void segtreeFree(SegmentTree* st)
{
    if (!st) return;
    daFree(&st->nodes);
    st->n = 0;
    st->elementSize = 0;
    st->mergeFunc = NULL;
}

// ------------------ Static Helper Functions ------------------ //

/**
 * ensureNodeIndex:
 * ----------------
 * Ensures that the dynamic array 'st->nodes' has a valid allocated block at the
 * specified 'idx' (1-based index). If 'idx' is beyond the current size, we
 * push newly allocated blocks until the size is large enough.
 *
 * Parameters:
 *  - st:  Pointer to a SegmentTree.
 *  - idx: The index in 'st->nodes' we need to ensure is allocated.
 *
 * Behavior:
 *  - While idx >= daSize(&st->nodes), allocates a block of 'st->elementSize' bytes
 *    and daPushBack()s it.
 *  - This guarantees that daGetMutable(&st->nodes, idx) will return a non-NULL pointer.
 */
static void ensureNodeIndex(SegmentTree* st, size_t idx)
{
    // While 'idx' is beyond the current size, push a properly allocated block
    while (idx >= daSize(&st->nodes)) {
        void* block = calloc(1, st->elementSize); // allocate real storage
        daPushBack(&st->nodes, block, st->elementSize);
        free(block); // we can free because daPushBack copies the contents
    }
}

/**
 * _buildSegTree:
 * --------------
 * Recursively builds the tree nodes in the range [start..end], storing the result
 * in 'st->nodes' at index 'idx'. Each node is allocated on demand using ensureNodeIndex.
 *
 * Parameters:
 *  - st:    Pointer to a SegmentTree.
 *  - data:  The original data array (DynamicArray) with at least st->n elements.
 *  - start: Left boundary (inclusive) of the range we're building.
 *  - end:   Right boundary (inclusive).
 *  - idx:   Current node index in the segment tree (1-based).
 *
 * Behavior:
 *  - Base case: if (start == end), this is a leaf node. We copy data[start] into the tree node.
 *  - Otherwise, recursively build the left child for [start..mid] and the right child for
 *    [mid+1..end], then merge them into the node at 'idx'.
 */
static void _buildSegTree(SegmentTree* st, const DynamicArray* data,
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
    _buildSegTree(st, data, start, mid, idx * 2);
    _buildSegTree(st, data, mid + 1, end, idx * 2 + 1);

    // Merge children into this node
    void* leftData  = daGetMutable((DynamicArray*)&st->nodes, idx * 2);
    void* rightData = daGetMutable((DynamicArray*)&st->nodes, idx * 2 + 1);
    void* nodeData  = daGetMutable((DynamicArray*)&st->nodes, idx);
    st->mergeFunc(leftData, rightData, nodeData);
}

/**
 * _querySegTree:
 * --------------
 * Recursively queries the segment tree for the range [left..right] within the
 * node that covers [start..end], located at index 'idx' in st->nodes. 
 *
 * Parameters:
 *  - st:      Pointer to a SegmentTree.
 *  - start:   The start of the current node's range.
 *  - end:     The end of the current node's range.
 *  - left:    The left boundary (inclusive) of the query.
 *  - right:   The right boundary (inclusive) of the query.
 *  - idx:     Current node index in the segment tree (1-based).
 *  - outData: A buffer to store the merged result of this node's coverage.
 *
 * Returns:
 *  - true if there was at least some overlap (meaning outData is valid).
 *  - false if there was no overlap at all.
 *
 * Behavior:
 *  - If [start..end] does not overlap [left..right], returns false immediately.
 *  - If [start..end] is fully inside [left..right], copies this node's data into outData.
 *  - Otherwise, we split into left child [start..mid] (idx*2) and right child
 *    [mid+1..end] (idx*2+1). We recursively gather partial results from both children,
 *    merge them (if both sides were used), and store the final result in outData.
 */
static bool _querySegTree(const SegmentTree* st,
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
        // Full coverage by [start..end]
        const void* nodeData = daGet(&st->nodes, idx);
        memcpy(outData, nodeData, st->elementSize);
        return true;
    }

    // Partial coverage, so we query children
    size_t mid = (start + end) / 2;

    // Temporary buffers for left and right
    unsigned char leftBuff[st->elementSize];
    unsigned char rightBuff[st->elementSize];

    bool usedLeft  = _querySegTree(st, start, mid, left, right, idx * 2, leftBuff);
    bool usedRight = _querySegTree(st, mid + 1, end, left, right, idx * 2 + 1, rightBuff);

    if (!usedLeft && !usedRight) {
        // No coverage from children
        return false;
    } else if (usedLeft && !usedRight) {
        // Only left child used
        memcpy(outData, leftBuff, st->elementSize);
        return true;
    } else if (!usedLeft && usedRight) {
        // Only right child used
        memcpy(outData, rightBuff, st->elementSize);
        return true;
    } else {
        // Both sides used => merge
        st->mergeFunc(leftBuff, rightBuff, outData);
        return true;
    }
}

/**
 * _updateSegTree:
 * ---------------
 * Recursively updates the value of one element at position 'pos' within the segment tree.
 *
 * Parameters:
 *  - st:      Pointer to a SegmentTree.
 *  - start:   The start of the current node's range.
 *  - end:     The end of the current node's range.
 *  - idx:     Current node index in the segment tree (1-based).
 *  - pos:     The index (0-based) within the original array that we're updating.
 *  - newVal:  Pointer to the new value (size must match st->elementSize).
 *
 * Behavior:
 *  - If [start..end] is just 'pos' (i.e., start==end==pos), we copy 'newVal' into this leaf node.
 *  - Otherwise, we recurse into the left or right child depending on whether pos <= mid or pos > mid.
 *  - After updating the child, we re-merge left and right child data into the current node to keep
 *    the segment tree consistent.
 */
static void _updateSegTree(SegmentTree* st,
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
        _updateSegTree(st, start, mid, idx * 2, pos, newVal);
    } else {
        _updateSegTree(st, mid + 1, end, idx * 2 + 1, pos, newVal);
    }

    // After update, re-merge
    void* leftData  = daGetMutable(&st->nodes, idx * 2);
    void* rightData = daGetMutable(&st->nodes, idx * 2 + 1);
    void* nodeData  = daGetMutable(&st->nodes, idx);
    st->mergeFunc(leftData, rightData, nodeData);
}
