#ifndef BTREE_H
#define BTREE_H

#include <stdbool.h>
#include <stddef.h>

/* Comparison function pointer */
typedef int (*btree_cmp_func)(const void *a, const void *b);

/* Destructor function pointer */
typedef void (*btree_destroy_func)(void *data);

/* Forward declarations */
typedef struct BTreeNode BTreeNode; /* Incomplete type */

typedef struct BTree {
    int t;
    btree_cmp_func cmp;
    btree_destroy_func destroy;
    BTreeNode *root;
} BTree;

/* Public API */
BTree *btree_create(int t, btree_cmp_func cmp, btree_destroy_func destroy);
void btree_destroy(BTree *tree);
void *btree_search(BTree *tree, const void *key);
void btree_insert(BTree *tree, void *key);
bool btree_remove(BTree *tree, const void *key);

#endif /* BTREE_H */
