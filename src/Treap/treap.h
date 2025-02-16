#ifndef TREAP_H
#define TREAP_H

#include <stdbool.h>

/* 
 * A node in the Treap. 
 * key is stored as void* to allow for generic usage with any data type.
 */
typedef struct TreapNode {
    void *key;
    int priority;
    struct TreapNode *left;
    struct TreapNode *right;
} TreapNode;

/* 
 * The Treap structure, which includes:
 *   - A pointer to the root node.
 *   - A comparison function pointer, used to maintain BST property.
 *   - An optional freeKey function pointer, to free keys if needed.
 */
typedef struct {
    TreapNode *root;
    int (*compare)(const void *a, const void *b);
    void (*freeKey)(void *key);
} Treap;

/**
 * Creates and returns a new Treap.
 *
 * @param compare A function pointer used to compare two keys.
 *                It should return a negative integer if (a < b), 0 if (a == b), 
 *                and a positive integer if (a > b).
 * @param freeKey A function pointer used to free a key. May be NULL 
 *                if your keys do not require special deallocation.
 * @return        A pointer to the newly created Treap, or NULL on failure.
 */
Treap* treapCreate(int (*compare)(const void *, const void *),
                   void (*freeKey)(void *));

/**
 * Destroys the Treap and frees all associated memory.
 *
 * @param treap A pointer to the Treap.
 */
void treapDestroy(Treap *treap);

/**
 * Inserts a key into the Treap.
 *
 * @param treap A pointer to the Treap.
 * @param key   A pointer to the key to be inserted.
 * @return      true if insertion succeeded; false if the key already exists or on error.
 */
bool treapInsert(Treap *treap, void *key);

/**
 * Searches for a key in the Treap.
 *
 * @param treap A pointer to the Treap.
 * @param key   A pointer to the key to be searched.
 * @return      true if the key is found, false otherwise.
 */
bool treapSearch(Treap *treap, void *key);

/**
 * Removes a key from the Treap.
 *
 * @param treap A pointer to the Treap.
 * @param key   A pointer to the key to be removed.
 * @return      true if removal was successful (key found), false otherwise.
 */
bool treapRemove(Treap *treap, void *key);

/**
 * Clears the Treap, removing all nodes, but does not free the Treap struct itself.
 *
 * @param treap A pointer to the Treap.
 */
void treapClear(Treap *treap);

#endif /* TREAP_H */
