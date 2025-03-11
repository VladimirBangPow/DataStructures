#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stddef.h>

/* --------------------------
 *    AVL Node Definition
 * -------------------------- */
typedef struct AvlNode {
    void *data;
    struct AvlNode *left;
    struct AvlNode *right;
    int height; 
} AvlNode;

/* --------------------------
 *    AVL Tree Definition
 * -------------------------- */
typedef struct AvlTree {
    AvlNode *root;

    /*
     * A comparison function:
     *   compare(a, b) <  0  if a < b
     *   compare(a, b) == 0  if a == b
     *   compare(a, b) >  0  if a > b
     */
    int (*compare)(const void *a, const void *b);
} AvlTree;

/* --------------------------
 *    AVL Tree API
 * -------------------------- */

/**
 * Creates a new AvlTree with the given compare function.
 * Returns a pointer to the newly allocated AvlTree.
 */
AvlTree *avl_create(int (*compare)(const void *, const void *));

/**
 * Destroys an entire AVL tree, calling free_data() on each node's data
 * (if free_data is not NULL), and then freeing the nodes themselves.
 */
void avl_destroy(AvlTree *tree, void (*free_data)(void *));

/**
 * Inserts the given data into the AVL tree, rebalancing as necessary.
 */
void avl_insert(AvlTree *tree, void *data);

/**
 * Removes the given data from the AVL tree, if it exists, using the
 * free_data() function to release node data (if free_data is not NULL).
 */
void avl_remove(AvlTree *tree, void *data, void (*free_data)(void *));

/**
 * Searches for 'data' in the tree. Returns a pointer to the stored data
 * if found, or NULL otherwise.
 */
void *avl_search(AvlTree *tree, void *data);

#endif /* AVL_TREE_H */
