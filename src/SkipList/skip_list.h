#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <stdbool.h> // for bool
#include <stddef.h>  // for size_t

/** 
 * Comparison function for skip list:
 *   - returns < 0 if (a < b)
 *   - returns = 0 if (a == b)
 *   - returns > 0 if (a > b)
 */
typedef int (*SkipListComparator)(const void *a, const void *b);

/**
 * Optional free function for skip list:
 *   - if not NULL, this is called to free each node's data 
 *     on removal or when slFree() is called.
 */
typedef void (*SkipListFreeFunc)(void *data);

/**
 * A single node in the skip list.
 *  - data: generic pointer to user data
 *  - forward: dynamic array of pointers to nodes (one for each level)
 */
typedef struct SkipListNode {
    void *data;
    struct SkipListNode **forward; 
} SkipListNode;

/**
 * The skip list structure itself:
 *  - header: special header node that holds no real data
 *  - maxLevel: max possible levels any node can have
 *  - probability: chance of "promoting" a node to higher level
 *  - level: current highest level in the list (ranges 1..maxLevel)
 *  - cmp: comparator for node data
 *  - freeFunc: optional function to free node data
 */
typedef struct SkipList {
    SkipListNode *header;
    int maxLevel;
    float probability;
    int level;

    SkipListComparator cmp;
    SkipListFreeFunc freeFunc;
} SkipList;

/**
 * Initializes a skip list.
 * @param sl          The SkipList pointer to initialize
 * @param maxLevel    Maximum level (height) any node might have
 * @param probability Promotion probability (often 0.5f)
 * @param cmp         Comparison function for data ordering
 * @param freeFunc    Optional function to free node data (NULL if not needed)
 */
void slInit(SkipList *sl, int maxLevel, float probability,
            SkipListComparator cmp, SkipListFreeFunc freeFunc);

/**
 * Frees all skip list nodes and, if freeFunc != NULL, frees the node data.
 * After this, the skip list is no longer usable unless reinitialized.
 */
void slFree(SkipList *sl);

/**
 * Inserts a new data pointer into the skip list if it is not already present.
 * @param sl    The SkipList
 * @param data  The pointer to data to store
 * @return true if insertion occurred, false if an identical element already exists
 */
bool slInsert(SkipList *sl, void *data);

/**
 * Searches the skip list for a given data pointer (by comparison).
 * @param sl    The SkipList
 * @param data  Pointer to data to find
 * @return true if found, false otherwise
 */
bool slSearch(const SkipList *sl, const void *data);

/**
 * Removes a data pointer from the skip list if it exists.
 * If sl->freeFunc != NULL, that function is called to free the data.
 * @param sl    The SkipList
 * @param data  The data pointer to remove
 * @return true if removed, false if not found
 */
bool slRemove(SkipList *sl, const void *data);

#endif // SKIP_LIST_H
