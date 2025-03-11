#include "treap.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

/* 
 * Creates a new TreapNode with a given key and a random priority.
 */
static TreapNode* createNode(void *key)
{
    TreapNode *node = (TreapNode*)malloc(sizeof(TreapNode));
    if (!node) {
        perror("Failed to allocate TreapNode");
        return NULL;
    }
    node->key = key;
    node->priority = rand();  /* random priority for treap balancing */
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* 
 * Right rotation (for fixing heap property).
 *
 *      y                x
 *     /  \     --->    /  \
 *    x    T3          T1   y
 *   / \                   / \
 *  T1  T2                T2  T3
 */
static TreapNode* rotateRight(TreapNode *y)
{
    TreapNode *x = y->left;
    TreapNode *T2 = x->right;
    x->right = y;
    y->left = T2;
    return x;
}

/* 
 * Left rotation (for fixing heap property).
 *
 *     x                  y
 *    /  \      --->     /  \
 *   T1   y             x    T3
 *      /  \           / \
 *     T2  T3         T1  T2
 */
static TreapNode* rotateLeft(TreapNode *x)
{
    TreapNode *y = x->right;
    TreapNode *T2 = y->left;
    y->left = x;
    x->right = T2;
    return y;
}

/* 
 * Recursively inserts a key into the Treap, returns the new root of the subtree.
 * 'success' indicates whether the insertion actually happened (key was unique).
 */
static TreapNode* insertRec(TreapNode *root, void *key,
                            int (*compare)(const void*, const void*),
                            bool *success)
{
    if (!root) {
        TreapNode *newNode = createNode(key);
        if (!newNode) {
            *success = false;
        }
        return newNode;
    }

    int cmp = compare(key, root->key);

    /* Insert using BST property */
    if (cmp < 0) {
        root->left = insertRec(root->left, key, compare, success);
        /* Fix heap property if violated */
        if (root->left && root->left->priority > root->priority) {
            root = rotateRight(root);
        }
    }
    else if (cmp > 0) {
        root->right = insertRec(root->right, key, compare, success);
        /* Fix heap property if violated */
        if (root->right && root->right->priority > root->priority) {
            root = rotateLeft(root);
        }
    }
    else {
        /* Key already exists: do not insert again. */
        *success = false;
    }

    return root;
}

/* 
 * Recursively searches for a key in the Treap.
 */
static bool searchRec(TreapNode *root, void *key,
                      int (*compare)(const void*, const void*))
{
    if (!root) return false;

    int cmp = compare(key, root->key);

    if (cmp == 0) {
        return true;
    } else if (cmp < 0) {
        return searchRec(root->left, key, compare);
    } else {
        return searchRec(root->right, key, compare);
    }
}

/* 
 * Recursively removes a key from the Treap, returns the new root of the subtree.
 * 'success' indicates whether removal actually found the key.
 */
static TreapNode* deleteRec(TreapNode *root, void *key,
                            int (*compare)(const void*, const void*),
                            void (*freeKey)(void*),
                            bool *success)
{
    if (!root) {
        *success = false;
        return NULL;
    }

    int cmp = compare(key, root->key);

    if (cmp < 0) {
        root->left = deleteRec(root->left, key, compare, freeKey, success);
    }
    else if (cmp > 0) {
        root->right = deleteRec(root->right, key, compare, freeKey, success);
    }
    else {
        /* Key found. This is the node to be deleted. */
        *success = true;

        /* If left child is NULL */
        if (!root->left) {
            TreapNode *temp = root->right;
            if (freeKey) {
                freeKey(root->key);
            }
            free(root);
            root = temp;
        }
        /* If right child is NULL */
        else if (!root->right) {
            TreapNode *temp = root->left;
            if (freeKey) {
                freeKey(root->key);
            }
            free(root);
            root = temp;
        } else {
            /* Both children exist: rotate based on priority to "push down" the node */
            if (root->left->priority > root->right->priority) {
                root = rotateRight(root);
                /* After rotation, continue deleting the same key */
                root->right = deleteRec(root->right, key, compare, freeKey, success);
            } else {
                root = rotateLeft(root);
                root->left = deleteRec(root->left, key, compare, freeKey, success);
            }
        }
    }
    return root;
}

/* 
 * Recursively frees all nodes in the Treap.
 */
static void clearRec(TreapNode *root, void (*freeKey)(void*))
{
    if (!root) return;
    clearRec(root->left, freeKey);
    clearRec(root->right, freeKey);
    if (freeKey) {
        freeKey(root->key);
    }
    free(root);
}

/* 
 * ======================
 * Public API Functions
 * ======================
 */

Treap* treapCreate(int (*compare)(const void*, const void*),
                   void (*freeKey)(void*))
{
    Treap *treap = (Treap*)malloc(sizeof(Treap));
    if (!treap) {
        perror("Failed to create Treap");
        return NULL;
    }
    treap->root = NULL;
    treap->compare = compare;
    treap->freeKey = freeKey;

    /* One-time seed for random priorities (you can seed elsewhere if preferred) */
    srand((unsigned int)time(NULL));

    return treap;
}

void treapDestroy(Treap *treap)
{
    if (!treap) return;
    treapClear(treap);
    free(treap);
}

bool treapInsert(Treap *treap, void *key)
{
    if (!treap || !key) return false;
    bool success = true;
    treap->root = insertRec(treap->root, key, treap->compare, &success);
    return success;
}

bool treapSearch(Treap *treap, void *key)
{
    if (!treap || !key) return false;
    return searchRec(treap->root, key, treap->compare);
}

bool treapRemove(Treap *treap, void *key)
{
    if (!treap || !key) return false;
    bool success = false;
    treap->root = deleteRec(treap->root, key, treap->compare, treap->freeKey, &success);
    return success;
}

void treapClear(Treap *treap)
{
    if (!treap) return;
    clearRec(treap->root, treap->freeKey);
    treap->root = NULL;
}
