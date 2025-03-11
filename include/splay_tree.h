#ifndef SPLAY_TREE_H
#define SPLAY_TREE_H

#include <stddef.h> // for size_t



/* 
 * Function pointer type for comparing two data items.
 * Should return:
 *   negative if (a < b)
 *   0 if (a == b)
 *   positive if (a > b)
 */
typedef int (*CompareFunc)(const void* a, const void* b);

/*
 * Function pointer type for destroying a data item.
 * If you store dynamically allocated data, this function
 * should free it. Otherwise, it can be NULL or a no-op.
 */
typedef void (*DestroyFunc)(void* data);

/* Forward declaration of the splay tree handle */
typedef struct SplayTree SplayTree;

/* 
 * Creates a new, empty splay tree. 
 *  - compare: comparator function for data
 *  - destroy: destructor function for data (may be NULL if not needed)
 */
SplayTree* splayTreeCreate(CompareFunc compare, DestroyFunc destroy);

/*
 * Destroys the splay tree and frees all associated memory.
 * If a destroy function was provided, it is called on each data item.
 */
void splayTreeDestroy(SplayTree* tree);

/*
 * Inserts 'data' into the splay tree. 
 * Returns 1 on success, 0 on failure (e.g., if out of memory).
 * After insertion, the newly inserted node is splayed to the root.
 */
int splayTreeInsert(SplayTree* tree, void* data);

/*
 * Searches for 'key' in the splay tree.
 * Returns pointer to the data if found, or NULL if not found.
 * In either case, the last accessed node (found or not) is splayed to the root.
 */
void* splayTreeSearch(SplayTree* tree, void* key);

/*
 * Deletes the node with matching 'key' from the splay tree.
 * Returns 1 if deletion succeeded (key found), 0 otherwise (key not found).
 * If deleted, the node containing 'key' is first splayed to root, then removed.
 */
int splayTreeDelete(SplayTree* tree, void* key);

/*
 * Returns the current number of nodes in the splay tree.
 */
size_t splayTreeSize(const SplayTree* tree);

/*
 * (Optional) A utility function that checks if the splay tree
 * is internally valid as a BST. Returns 1 if valid, 0 otherwise.
 * This is useful for testing but might be omitted in production builds.
 */
int splayTreeIsValidBST(const SplayTree* tree);


#endif /* SPLAY_TREE_H */
