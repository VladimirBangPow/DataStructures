#ifndef SEGTREE_H
#define SEGTREE_H

#include <stddef.h> // for size_t
#include <stdbool.h>
#include "../DynamicArray/dynamic_array.h"

#ifdef __cplusplus
extern "C" {
#endif

// Function pointer type for merging two segment tree nodes.
// leftData  - pointer to left child's data
// rightData - pointer to right child's data
// outData   - pointer to memory where the merged result should be stored
typedef void (*SegTreeMergeFunc)(const void* leftData, const void* rightData, void* outData);

// A generic segment tree that stores arbitrary data in its nodes.
typedef struct {
    DynamicArray nodes;    // The underlying storage for all nodes in the segment tree
    size_t       n;        // Number of leaves (size of the input array)
    size_t       elementSize;  // Size of each data element in bytes
    SegTreeMergeFunc mergeFunc; // Merging function provided by the user
} SegmentTree;

/**
 * Initialize a SegmentTree.
 *
 * @param st           - Pointer to a SegmentTree to initialize
 * @param n            - Number of elements (size of the original array)
 * @param elementSize  - Size in bytes of each data element
 * @param mergeFunc    - Pointer to function that merges two child nodes
 */
void segtreeInit(SegmentTree* st, size_t n, size_t elementSize, SegTreeMergeFunc mergeFunc);

/**
 * Build the segment tree from the given array of data.
 *
 * @param st   - Pointer to an already-initialized SegmentTree
 * @param data - The data array (DynamicArray) from which the tree is built
 *               Must have at least n elements, each of size elementSize
 */
void segtreeBuild(SegmentTree* st, const DynamicArray* data);

/**
 * Query the segment tree for the range [left, right].
 * Merges all segments in the specified range and stores the result in outData.
 *
 * @param st      - Pointer to the SegmentTree
 * @param left    - Left boundary of query range (0-based)
 * @param right   - Right boundary of query range (inclusive, 0-based)
 * @param outData - Pointer to a buffer of size elementSize to store the result
 * @return true if the query range is valid and the tree can produce a result,
 *         false if the range is out of bounds or invalid (in which case outData is unmodified).
 */
bool segtreeQuery(const SegmentTree* st, size_t left, size_t right, void* outData);

/**
 * Update a single element at the given index in the segment tree.
 *
 * @param st         - Pointer to the SegmentTree
 * @param index      - Index (0-based) of the element to update
 * @param newValue   - Pointer to the new value
 * @param valueSize  - Size of the new value (must match elementSize)
 */
void segtreeUpdate(SegmentTree* st, size_t index, const void* newValue, size_t valueSize);

/**
 * Free the resources used by the SegmentTree.
 * This calls daFree on the internal DynamicArray.
 *
 * @param st - Pointer to the SegmentTree
 */
void segtreeFree(SegmentTree* st);


// bool segtreeIsValidFull(const SegmentTree* st);

#ifdef __cplusplus
}
#endif

#endif // SEGTREE_H

