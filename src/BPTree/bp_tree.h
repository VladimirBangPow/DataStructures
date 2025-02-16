#ifndef BP_TREE_H
#define BP_TREE_H

#include <stddef.h>  // for size_t


/*
 * Forward declarations for a generic B+ Tree in C
 * The B+ tree is parameterized by user-provided function pointers:
 *   - compareFunc: compares two data items
 *   - copyFunc:    creates a copy of a data item
 *   - freeFunc:    frees a data item
 *
 * This allows the B+ tree to store any type of data.
 */

/* Function pointer types for user-defined operations on data */
typedef int   (*CompareFunc)(const void *a, const void *b);
typedef void* (*CopyFunc)(const void *data);
typedef void  (*FreeFunc)(void *data);

/* Define an order for the B+ tree (max children in internal nodes).
 * For a real dynamic library, you might pass 'order' to createBPTree.
 * Here, for simplicity, we fix an example order = 4.
 */
#ifndef BP_TREE_ORDER
#define BP_TREE_ORDER 4
#endif

/* Forward struct references */
typedef struct BPTree BPTree;

/* ---- Core API ---- */

/*
 * createBPTree:
 *   Creates and initializes a new B+ tree with the given function pointers.
 *   Returns a pointer to a BPTree, or NULL on failure.
 */
BPTree* createBPTree(CompareFunc compareFunc, CopyFunc copyFunc, FreeFunc freeFunc);

/*
 * destroyBPTree:
 *   Destroys the entire B+ tree, freeing all nodes and data using the freeFunc.
 *   The pointer to the tree becomes invalid after this call.
 */
void destroyBPTree(BPTree *tree);

/*
 * bpTreeInsert:
 *   Inserts a (key) data item into the B+ tree.
 *   If duplicates are allowed, you can store them; if not, skip or update.
 *   Implementation is up to your design.
 */
void bpTreeInsert(BPTree *tree, const void *data);

/*
 * bpTreeDelete:
 *   Removes a (key) data item from the B+ tree.
 *   If the item is not found, no change. 
 */
void bpTreeDelete(BPTree *tree, const void *data);

/*
 * bpTreeSearch:
 *   Searches for 'data' in the B+ tree.
 *   Returns a pointer to the stored data item if found, or NULL if not found.
 *   For a real system, you might return a list of matching items (for duplicates).
 */
void* bpTreeSearch(const BPTree *tree, const void *data);

/*
 * bpTreeSize:
 *   Returns the total number of data items stored in the B+ tree.
 *   (An optional helper function, if desired.)
 */
size_t bpTreeSize(const BPTree *tree);



#endif /* BP_TREE_H */
