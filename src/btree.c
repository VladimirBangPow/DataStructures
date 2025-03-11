#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "btree_internal.h"  /* We need the full BTreeNode definition */


/* ------------------------------------------------------------------
 * 0) Static function prototypes or define them before usage
 * ------------------------------------------------------------------ */
static BTreeNode *btree_node_create(int t, bool leaf);
static void btree_node_destroy(BTreeNode *node, btree_destroy_func destroy);

static void btree_split_child(BTree *tree, BTreeNode *parent, int index);
static void btree_insert_nonfull(BTree *tree, BTreeNode *node, void *key);

static bool btree_remove_internal(BTree *tree, BTreeNode *node, const void *key);
static void btree_merge_children(BTree *tree, BTreeNode *node, int idx);

/* ------------------------------------------------------------------
 * 1) Implementation: Create/Destroy
 * ------------------------------------------------------------------ */
BTree *btree_create(int t, btree_cmp_func cmp, btree_destroy_func destroy) {
    if (t < 2 || !cmp) {
        /* invalid args */
        return NULL;
    }
    BTree *tree = (BTree *)malloc(sizeof(BTree));
    if (!tree) {
        fprintf(stderr, "Out of memory!\n");
        exit(EXIT_FAILURE);
    }
    tree->t = t;
    tree->cmp = cmp;
    tree->destroy = destroy;
    tree->root = btree_node_create(t, true); /* initially a leaf */
    return tree;
}


void btree_destroy(BTree *tree) {
    if (!tree) return;
    btree_node_destroy(tree->root, tree->destroy);
    free(tree);
}

static void btree_node_destroy(BTreeNode *node, btree_destroy_func destroy) {
    if (!node) return;
    if (destroy) {
        for (int i = 0; i < node->nkeys; i++) {
            destroy(node->keys[i]);
        }
    }
    if (!node->leaf) {
        for (int i = 0; i <= node->nkeys; i++) {
            btree_node_destroy(node->children[i], destroy);
        }
    }
    free(node->keys);
    free(node->children);
    free(node);
}

/* ------------------------------------------------------------------
 * 2) Static Node Create/Destroy
 * ------------------------------------------------------------------ */
static BTreeNode *btree_node_create(int t, bool leaf) {
    BTreeNode *node = (BTreeNode *)malloc(sizeof(BTreeNode));
    if (!node) {
        fprintf(stderr, "Out of memory!\n");
        exit(EXIT_FAILURE);
    }
    node->leaf = leaf;
    node->nkeys = 0;
    node->keys = (void **)calloc(2 * t - 1, sizeof(void *));
    node->children = (BTreeNode **)calloc(2 * t, sizeof(BTreeNode *));
    if (!node->keys || !node->children) {
        fprintf(stderr, "Out of memory!\n");
        exit(EXIT_FAILURE);
    }
    return node;
}





/* ------------------------------------------------------------------
 * 3) Search
 * ------------------------------------------------------------------ */

void *btree_search(BTree *tree, const void *key) {
    if (!tree || !tree->root || !key) return NULL;

    BTreeNode *cur = tree->root;
    while (cur) {
        int i;
        for (i=0; i < cur->nkeys && tree->cmp(key, cur->keys[i]) > 0; i++)
			;
		
        if (i < cur->nkeys && tree->cmp(key, cur->keys[i]) == 0) {
            return cur->keys[i];
        }else if (cur->leaf) {
            return NULL;
        }else {
        	cur = cur->children[i];
		}
    }
    return NULL;
}

/* ------------------------------------------------------------------
 * 4) Insert
 * ------------------------------------------------------------------ */
void btree_insert(BTree *tree, void *key) {
    if (!tree || !key) return;

    BTreeNode *root = tree->root;
    int maxKeys = 2 * tree->t - 1;
    if (root->nkeys == maxKeys) {
        /* If root is full, split first */
        BTreeNode *newRoot = btree_node_create(tree->t, false);
        newRoot->children[0] = root;
        tree->root = newRoot;
        btree_split_child(tree, newRoot, 0);
        btree_insert_nonfull(tree, newRoot, key);
    } else {
        btree_insert_nonfull(tree, root, key);
    }
}

/* Insert key into non-full node */
static void btree_insert_nonfull(BTree *tree, BTreeNode *node, void *key) {
    int i = node->nkeys - 1;
    if (node->leaf) {
        while (i >= 0 && tree->cmp(key, node->keys[i]) < 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->nkeys++;
    } else {
        while (i >= 0 && tree->cmp(key, node->keys[i]) < 0) {
            i--;
        }
        i++;
        if (node->children[i]->nkeys == 2 * tree->t - 1) {
            btree_split_child(tree, node, i);
            if (tree->cmp(key, node->keys[i]) > 0) {
                i++;
            }
        }
        btree_insert_nonfull(tree, node->children[i], key);
    }
}

/* Split a full child of parent->children[index] */
static void btree_split_child(BTree *tree, BTreeNode *parent, int index) {
    int t = tree->t;
    BTreeNode *fullChild = parent->children[index];
    BTreeNode *newChild = btree_node_create(t, fullChild->leaf);

    newChild->nkeys = t - 1;
    for (int j = 0; j < t - 1; j++) {
        newChild->keys[j] = fullChild->keys[j + t];
    }
    if (!fullChild->leaf) {
        for (int j = 0; j < t; j++) {
            newChild->children[j] = fullChild->children[j + t];
        }
    }
    fullChild->nkeys = t - 1;

    for (int j = parent->nkeys; j >= index + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[index + 1] = newChild;

    for (int j = parent->nkeys - 1; j >= index; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[index] = fullChild->keys[t - 1];
    parent->nkeys++;
}

/* ------------------------------------------------------------------
 * 5) Remove
 * ------------------------------------------------------------------ */
bool btree_remove(BTree *tree, const void *key) {
    if (!tree || !tree->root || !key) return false;
    bool removed = btree_remove_internal(tree, tree->root, key);

    if (tree->root->nkeys == 0 && !tree->root->leaf) {
        /* Shrink tree if root is empty but has a child */
        BTreeNode *oldRoot = tree->root;
        tree->root = tree->root->children[0];
        free(oldRoot->keys);
        free(oldRoot->children);
        free(oldRoot);
    }
    return removed;
}

static bool btree_remove_internal(BTree *tree, BTreeNode *node, const void *key) {
    int t = tree->t;
    int idx = 0;

    /* Find the key in node, if present */
    while (idx < node->nkeys && tree->cmp(key, node->keys[idx]) > 0) {
        idx++;
    }

    if (idx < node->nkeys && tree->cmp(key, node->keys[idx]) == 0) {
        /* Key found at index idx in node->keys */
        if (node->leaf) {
            /* Remove from leaf directly */
            for (int i = idx; i < node->nkeys - 1; i++) {
                node->keys[i] = node->keys[i + 1];
            }
            node->nkeys--;
            return true;
        } else {
            /* Internal node remove */
            BTreeNode *leftChild = node->children[idx];
            BTreeNode *rightChild = node->children[idx + 1];
            /* If leftChild has >= t keys, replace with predecessor */
            if (leftChild->nkeys >= t) {
                BTreeNode *cur = leftChild;
                while (!cur->leaf) {
                    cur = cur->children[cur->nkeys];
                }
                void *predKey = cur->keys[cur->nkeys - 1];
                node->keys[idx] = predKey;
                return btree_remove_internal(tree, leftChild, predKey);
            }
            /* Else if rightChild has >= t keys, replace with successor */
            else if (rightChild->nkeys >= t) {
                BTreeNode *cur = rightChild;
                while (!cur->leaf) {
                    cur = cur->children[0];
                }
                void *succKey = cur->keys[0];
                node->keys[idx] = succKey;
                return btree_remove_internal(tree, rightChild, succKey);
            }
            /* Otherwise, merge leftChild, key, rightChild and remove from the merged node */
            else {
                btree_merge_children(tree, node, idx);
                return btree_remove_internal(tree, leftChild, key);
            }
        }
    } else {
        /* Key is not in this node */
        if (node->leaf) {
            /* Not found at all */
            return false;
        }
        BTreeNode *child = node->children[idx];
        /* Ensure child has at least t keys */
        if (child->nkeys < t) {
            BTreeNode *leftSibling  = (idx > 0) ? node->children[idx - 1] : NULL;
            BTreeNode *rightSibling = (idx < node->nkeys) ? node->children[idx + 1] : NULL;

            /* Borrow from left sibling? */
            if (leftSibling && leftSibling->nkeys >= t) {
                /* Shift child’s keys right */
                for (int i = child->nkeys - 1; i >= 0; i--) {
                    child->keys[i + 1] = child->keys[i];
                }
                if (!child->leaf) {
                    for (int i = child->nkeys; i >= 0; i--) {
                        child->children[i + 1] = child->children[i];
                    }
                }
                child->keys[0] = node->keys[idx - 1];
                if (!child->leaf) {
                    child->children[0] = leftSibling->children[leftSibling->nkeys];
                }
                child->nkeys++;
                node->keys[idx - 1] = leftSibling->keys[leftSibling->nkeys - 1];
                leftSibling->nkeys--;
            }
            /* Borrow from right sibling? */
            else if (rightSibling && rightSibling->nkeys >= t) {
                child->keys[child->nkeys] = node->keys[idx];
                if (!child->leaf) {
                    child->children[child->nkeys + 1] = rightSibling->children[0];
                }
                child->nkeys++;
                node->keys[idx] = rightSibling->keys[0];
                for (int i = 0; i < rightSibling->nkeys - 1; i++) {
                    rightSibling->keys[i] = rightSibling->keys[i + 1];
                }
                if (!rightSibling->leaf) {
                    for (int i = 0; i < rightSibling->nkeys; i++) {
                        rightSibling->children[i] = rightSibling->children[i + 1];
                    }
                }
                rightSibling->nkeys--;
            }
            /* Merge with a sibling */
            else {
                int mergeIdx = (leftSibling ? idx - 1 : idx);
                btree_merge_children(tree, node, mergeIdx);
                if (mergeIdx == idx - 1) {
                    child = node->children[mergeIdx];
                }
            }
        }
        /* Recursively remove from child */
        return btree_remove_internal(tree, child, key);
    }
}

/* Merge children[idx] and children[idx+1] with keys[idx] as the median */
static void btree_merge_children(BTree *tree, BTreeNode *node, int idx) {
    int t = tree->t;
    BTreeNode *leftChild = node->children[idx];
    BTreeNode *rightChild = node->children[idx + 1];

    leftChild->keys[t - 1] = node->keys[idx];
    for (int i = 0; i < rightChild->nkeys; i++) {
        leftChild->keys[i + t] = rightChild->keys[i];
    }
    if (!leftChild->leaf) {
        for (int i = 0; i <= rightChild->nkeys; i++) {
            leftChild->children[i + t] = rightChild->children[i];
        }
    }
    leftChild->nkeys += rightChild->nkeys + 1;

    /* Shift keys in parent to remove key idx */
    for (int i = idx; i < node->nkeys - 1; i++) {
        node->keys[i] = node->keys[i + 1];
        node->children[i + 1] = node->children[i + 2];
    }
    node->nkeys--;

    free(rightChild->keys);
    free(rightChild->children);
    free(rightChild);
}

