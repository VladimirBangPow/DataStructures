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


bool segtreeIsValidFull(const SegmentTree* st);

#ifdef __cplusplus
}
#endif

#endif // SEGTREE_H


// #ifndef SEG_TREE_H
// #define SEG_TREE_H

// #ifdef __cplusplus
// extern "C" {
// #endif

// /*
//  * Define a function pointer type for combining two integers.
//  */
// typedef int (*CombineFunc)(int, int);

// /*
//  * SegmentTree structure:
//  * - n:         the size of the original array
//  * - tree:      the underlying array storing segment tree nodes
//  * - combine:   function pointer for combining two segment values
//  * - identity:  identity element for the combine function 
//  *              (e.g. 0 for sum, INT_MAX for min, etc.)
//  */
// typedef struct {
//     int n;
//     int *tree;
//     CombineFunc combine;
//     int identity;
// } SegmentTree;

// /* Create a segment tree for the given array (arr) of length n, with the
//  * specified combine function and identity element. */
// SegmentTree* segtree_create(int *arr, int n, CombineFunc combine, int identity);

// /* Free all memory used by the segment tree. */
// void segtree_destroy(SegmentTree* st);

// /* Query the range [left, right] in the segment tree. */
// int segtree_query(SegmentTree* st, int left, int right);

// /* Update the value at position index in the array (and reflect in tree). */
// void segtree_update(SegmentTree* st, int index, int value);

// /*
//  * Some common combine functions. 
//  * For example, sum, min, max, gcd, etc.
//  */
// int combine_sum(int a, int b);
// int combine_min(int a, int b);
// int combine_max(int a, int b);
// int combine_gcd(int a, int b);

// #ifdef __cplusplus
// }
// #endif

// #endif
