#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <stddef.h>

/* 
 * The Red-Black Tree is generic and stores void* data. 
 * A user-provided comparison function (cmp) and an optional 
 * destructor (destroy) can be supplied.
 */

/* Opaque handle for the RB Tree */
typedef struct RBTREE RBTREE;

/* 
 * Create a new RBTREE.
 * cmp     = Comparison function returning:
 *           < 0 if (a < b),
 *           0 if (a == b),
 *           > 0 if (a > b).
 * destroy = Optional function for destroying node data (can be NULL).
 */
RBTREE* rb_create(int (*cmp)(const void*, const void*), 
                  void (*destroy)(void*));

/* 
 * Destroy the tree. 
 * Frees all nodes. If destroy != NULL, it will be called 
 * on each node's data before freeing.
 */
void rb_destroy(RBTREE* tree);

/*
 * Insert data into the tree. 
 * Returns 0 if successful, non-zero on error or if data is a duplicate.
 */
int rb_insert(RBTREE* tree, void* data);

/*
 * Search for data in the tree. 
 * Returns a pointer to the stored data if found, or NULL otherwise.
 */
void* rb_search(RBTREE* tree, const void* data);

/* 
 * Delete data from the tree. 
 * Returns 0 if found and deleted, non-zero otherwise.
 */
int rb_delete(RBTREE* tree, const void* data);

/*
 * Optional: Print the tree in-order by calling print_func on each data element.
 * (Used mainly for debugging or demonstration.)
 */
void rb_print_inorder(const RBTREE* tree, void (*print_func)(const void*));

/* 
 * Get the number of elements in the tree.
 */
size_t rb_size(const RBTREE* tree);

#endif /* RED_BLACK_TREE_H */
