#ifndef BPTREE_H
#define BPTREE_H

#include <stdbool.h>
#include <stddef.h>

/* Comparator function type:
 *   Returns:
 *     < 0 if (a < b)
 *     = 0 if (a == b)
 *     > 0 if (a > b)
 */
typedef int (*bptree_cmp)(const void* a, const void* b);

/* Forward declaration of BPTreeNode. */
typedef struct BPTreeNode BPTreeNode;

/* --------------------------------------------------------------------------
 * B+ Tree structure
 *   - order = maximum number of children a node can have
 *             (so max keys in a node is order - 1)
 *   - root  = pointer to the root node
 *   - cmp   = comparison function for keys
 * -------------------------------------------------------------------------- */
typedef struct BPTree {
    int        order;
    BPTreeNode* root;
    bptree_cmp cmp;
} BPTree;

/* --------------------------------------------------------------------------
 * B+ Tree Node structure
 *   - is_leaf = true if node is a leaf
 *   - num_keys = current number of keys stored
 *   - keys     = array of key pointers
 *   - values   = array of value pointers (only used if leaf)
 *   - children = array of child pointers (B+ internal or leaf links)
 *   - next     = pointer to next leaf (if leaf)
 *   - parent   = pointer to parent node
 * -------------------------------------------------------------------------- */
typedef struct BPTreeNode {
    bool            is_leaf;
    int             num_keys;
    void**          keys;
    void**          values;   /* only used if is_leaf == true */
    struct BPTreeNode** children;
    struct BPTreeNode*  parent;
    struct BPTreeNode*  next;
} BPTreeNode;

/**
 * Create an empty B+ tree with the specified order and comparator.
 * 'order' should be >= 3 for meaningful usage.
 * 'cmp' must be a function that orders keys (like strcmp or integer-compare).
 *
 * Returns a pointer to a newly allocated BPTree.
 */
BPTree* bptree_create(int order, bptree_cmp cmp);

/**
 * Destroy the entire B+ tree, freeing all allocated memory.
 */
void bptree_destroy(BPTree* tree);

/**
 * Insert a (key, value) pair into the B+ tree.
 * 
 * The tree only stores the pointer references to key/value (no deep copy).
 * The user is responsible for ensuring key/value remain valid for
 * the lifetime of the tree, or that keys/values are dynamically allocated.
 */
void bptree_insert(BPTree* tree, void* key, void* value);

/**
 * Search for a key in the B+ tree.
 * Returns the associated value pointer if found, or NULL if not found.
 */
void* bptree_search(BPTree* tree, void* key);

/**
 * Delete the entry with the given key from the B+ tree.
 * Returns true if the key was found and deleted, false otherwise.
 */
bool bptree_delete(BPTree* tree, void* key);

/**
 * Print the B+ tree contents to stdout (for debugging).
 */
void bptree_print(BPTree* tree);

/* A simple integer comparator for convenience:
 * Usage: pass bptree_int_cmp as the 'cmp' argument to bptree_create
 * if your keys are addresses to integer variables.
 */
int bptree_int_cmp(const void* a, const void* b);

#endif // BPTREE_H
