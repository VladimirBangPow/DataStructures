#include "bp_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* ------------------------ B+ TREE INTERNAL STRUCTURES ---------------------- */

typedef struct BPNode {
    int isLeaf;             /* 1 if leaf node, 0 if internal node */
    int numKeys;            /* number of keys stored in this node */
    void *keys[BP_TREE_ORDER];  /* array of keys (void*) */
    
    /* Children for internal nodes, data pointers for leaves */
    union {
        struct BPNode *children[BP_TREE_ORDER + 1]; /* internal children */
        void *values[BP_TREE_ORDER + 1];            /* leaf data pointers */
    };
    
    struct BPNode *nextLeaf; /* leaf-link for range scans (optional) */
    struct BPNode *prevLeaf; /* for doubly-linked leaves (optional) */
    
} BPNode;

struct BPTree {
    BPNode      *root;
    CompareFunc  compareFunc;
    CopyFunc     copyFunc;
    FreeFunc     freeFunc;
    size_t       size; /* total number of data entries in all leaves */
};

/* ------------------------- FORWARD DECLARATIONS --------------------------- */
static BPNode* createNode(int isLeaf);
static void    freeNode(BPTree *tree, BPNode *node);
static void*   searchNode(const BPTree *tree, const BPNode *node, const void* data);
static BPNode* insertNode(BPTree *tree, BPNode *node, void *data, void **promoKey, BPNode **promoChild);
static void    splitLeaf(BPTree *tree, BPNode *leaf, void *data, int insertPos,
                         void **promoKey, BPNode **promoChild);
static void    splitInternal(BPTree *tree, BPNode *node, void *promoKey, BPNode *promoChild,
                             int insertPos, void **newKey, BPNode **newChild);
static int     findLeafInsertPos(const BPTree *tree, const BPNode *leaf, const void* data);
static int     findInternalInsertPos(const BPTree *tree, const BPNode *node, const void* data);

static int     deleteNode(BPTree *tree, BPNode *node, const void* data);
static void    rebalanceAfterDelete(BPTree *tree, BPNode *node, int childIndex);
static void    mergeLeafNodes(BPTree *tree, BPNode *left, BPNode *right, int parentIndex, BPNode *parent);
static void    mergeInternalNodes(BPTree *tree, BPNode *left, BPNode *right, int parentIndex, BPNode *parent);

/* --------------------------- UTILITY FUNCTIONS ---------------------------- */

static BPNode* createNode(int isLeaf)
{
    BPNode *node = (BPNode*) calloc(1, sizeof(BPNode));
    if (!node) return NULL;
    node->isLeaf = isLeaf;
    node->numKeys = 0;
    node->nextLeaf = NULL;
    node->prevLeaf = NULL;
    return node;
}

static void freeNode(BPTree *tree, BPNode *node)
{
    if (!node) return;
    if (node->isLeaf) {
        /* Free the values stored here (if we own them) */
        for (int i = 0; i < node->numKeys; i++) {
            if (node->values[i]) {
                tree->freeFunc(node->values[i]);
                node->values[i] = NULL;
            }
        }
    } else {
        /* Recursively free children */
        for (int i = 0; i <= node->numKeys; i++) {
            if (node->children[i]) {
                freeNode(tree, node->children[i]);
                node->children[i] = NULL;
            }
        }
    }
    /* Free the keys */
    for (int i = 0; i < node->numKeys; i++) {
        if (node->keys[i]) {
            tree->freeFunc(node->keys[i]);
            node->keys[i] = NULL;
        }
    }
    free(node);
}

/* ------------------------- B+ TREE CREATE/DESTROY ------------------------- */

BPTree* createBPTree(CompareFunc compareFunc, CopyFunc copyFunc, FreeFunc freeFunc)
{
    BPTree *tree = (BPTree*) calloc(1, sizeof(BPTree));
    if (!tree) return NULL;
    tree->root = createNode(/* isLeaf = */ 1); /* start as a leaf */
    if (!tree->root) {
        free(tree);
        return NULL;
    }
    tree->compareFunc = compareFunc;
    tree->copyFunc    = copyFunc;
    tree->freeFunc    = freeFunc;
    tree->size = 0;
    return tree;
}

void destroyBPTree(BPTree *tree)
{
    if (!tree) return;
    freeNode(tree, tree->root);
    free(tree);
}

/* ------------------------- B+ TREE SEARCH ------------------------- */

static void* searchNode(const BPTree *tree, const BPNode *node, const void* data)
{
    if (!node) return NULL;
    if (node->isLeaf) {
        /* Linear search in leaf node */
        for (int i = 0; i < node->numKeys; i++) {
            int cmp = tree->compareFunc(data, node->keys[i]);
            if (cmp == 0) {
                return node->values[i]; /* or node->keys[i] if same pointer */
            } else if (cmp < 0) {
                break; /* not found */
            }
        }
        return NULL;
    } else {
        /* Internal node: find the branch to follow */
        int i;
        for (i = 0; i < node->numKeys; i++) {
            int cmp = tree->compareFunc(data, node->keys[i]);
            if (cmp < 0) {
                return searchNode(tree, node->children[i], data);
            }
        }
        /* If not found in any key comparison, go rightmost child */
        return searchNode(tree, node->children[node->numKeys], data);
    }
}

void* bpTreeSearch(const BPTree *tree, const void *data)
{
    if (!tree || !tree->root || tree->root->numKeys == 0) return NULL;
    return searchNode(tree, tree->root, data);
}

/* ------------------------- B+ TREE INSERT ------------------------- */

void bpTreeInsert(BPTree *tree, const void *data)
{
    if (!tree || !data) return;

    void *promoKey = NULL;
    BPNode *promoChild = NULL;

    /* Insert recursively */
    BPNode *newRoot = insertNode(tree, tree->root, (void*)data, &promoKey, &promoChild);

    if (promoKey) {
        /* Root split occurred: create a new root */
        BPNode *root = createNode(/* isLeaf= */ 0);
        root->numKeys = 1;
        /* The new root's key[0] = promoKey */
        root->keys[0] = promoKey;
        /* left child = old root, right child = promoChild */
        root->children[0] = tree->root;
        root->children[1] = promoChild;
        tree->root = root;
    }
}

static BPNode* insertNode(BPTree *tree, BPNode *node, void *data,
                          void **promoKey, BPNode **promoChild)
{
    if (node->isLeaf) {
        /* Find position to insert in leaf */
        int pos = findLeafInsertPos(tree, node, data);

        /* If duplicates not desired, check for existing key, etc. (omitted) */

        /* If node not full, just insert */
        if (node->numKeys < BP_TREE_ORDER - 1) {
            /* Shift to make space */
            for (int i = node->numKeys; i > pos; i--) {
                node->keys[i] = node->keys[i-1];
                node->values[i] = node->values[i-1];
            }
            node->keys[pos]   = tree->copyFunc(data);
            node->values[pos] = node->keys[pos]; /* or store separate if needed */
            node->numKeys++;
            tree->size++;
            *promoKey = NULL;
            *promoChild = NULL;
            return node;
        } else {
            /* Node is full -> split */
            splitLeaf(tree, node, data, pos, promoKey, promoChild);
            return node; /* returning original node pointer */
        }
    } else {
        /* Internal node */
        int pos = findInternalInsertPos(tree, node, data);

        /* Insert into child recursively */
        void *newKey = NULL;
        BPNode *newChild = NULL;
        insertNode(tree, node->children[pos], data, &newKey, &newChild);

        if (newKey) {
            /* Child was split, we must insert newKey into 'node' at 'pos' */
            if (node->numKeys < BP_TREE_ORDER - 1) {
                /* No split at this level */
                for (int i = node->numKeys; i > pos; i--) {
                    node->keys[i] = node->keys[i-1];
                    node->children[i+1] = node->children[i];
                }
                node->keys[pos] = newKey;
                node->children[pos+1] = newChild;
                node->numKeys++;
                *promoKey = NULL;
                *promoChild = NULL;
                return node;
            } else {
                /* Need to split this internal node */
                splitInternal(tree, node, newKey, newChild, pos, promoKey, promoChild);
                return node;
            }
        } else {
            /* No split from child */
            *promoKey = NULL;
            *promoChild = NULL;
            return node;
        }
    }
}

static int findLeafInsertPos(const BPTree *tree, const BPNode *leaf, const void* data)
{
    int i = 0;
    while (i < leaf->numKeys && tree->compareFunc(data, leaf->keys[i]) > 0) {
        i++;
    }
    return i;
}

static int findInternalInsertPos(const BPTree *tree, const BPNode *node, const void* data)
{
    int i = 0;
    while (i < node->numKeys && tree->compareFunc(data, node->keys[i]) > 0) {
        i++;
    }
    return i;
}

static void splitLeaf(BPTree *tree, BPNode *leaf, void *data, int insertPos,
                      void **promoKey, BPNode **promoChild)
{
    /* Create a new leaf */
    BPNode *newLeaf = createNode(/* isLeaf= */ 1);
    if (!newLeaf) return;

    /* Temp array to hold all keys/values before we split */
    void *tempKeys[BP_TREE_ORDER];
    void *tempValues[BP_TREE_ORDER];
    memset(tempKeys, 0, sizeof(tempKeys));
    memset(tempValues, 0, sizeof(tempValues));

    for (int i = 0; i < leaf->numKeys; i++) {
        tempKeys[i] = leaf->keys[i];
        tempValues[i] = leaf->values[i];
    }

    /* Insert new data into temp arrays */
    for (int i = leaf->numKeys; i > insertPos; i--) {
        tempKeys[i] = tempKeys[i-1];
        tempValues[i] = tempValues[i-1];
    }
    tempKeys[insertPos]   = tree->copyFunc(data);
    tempValues[insertPos] = tempKeys[insertPos];
    int total = leaf->numKeys + 1;

    /* Decide the split point (half) */
    int splitPoint = total / 2;
    leaf->numKeys = splitPoint;
    newLeaf->numKeys = total - splitPoint;

    for (int i = 0; i < leaf->numKeys; i++) {
        leaf->keys[i]   = tempKeys[i];
        leaf->values[i] = tempValues[i];
    }
    for (int i = leaf->numKeys; i < BP_TREE_ORDER - 1; i++) {
        leaf->keys[i] = NULL;
        leaf->values[i] = NULL;
    }

    for (int i = 0; i < newLeaf->numKeys; i++) {
        newLeaf->keys[i]   = tempKeys[i + splitPoint];
        newLeaf->values[i] = tempValues[i + splitPoint];
    }
    for (int i = newLeaf->numKeys; i < BP_TREE_ORDER - 1; i++) {
        newLeaf->keys[i] = NULL;
        newLeaf->values[i] = NULL;
    }

    /* Link leaves */
    newLeaf->nextLeaf = leaf->nextLeaf;
    if (newLeaf->nextLeaf) {
        newLeaf->nextLeaf->prevLeaf = newLeaf;
    }
    leaf->nextLeaf = newLeaf;
    newLeaf->prevLeaf = leaf;

    /* The promoKey is the first key in the new leaf, to be inserted in parent */
    *promoKey = tree->copyFunc(newLeaf->keys[0]);
    *promoChild = newLeaf;

    tree->size++; /* because we inserted a new key overall */
}

static void splitInternal(BPTree *tree, BPNode *node, void *promoKey, BPNode *promoChild,
                          int insertPos, void **newKey, BPNode **newChild)
{
    /* We temporarily hold all keys/children before re-splitting */
    void *tempKeys[BP_TREE_ORDER + 1];
    BPNode *tempChildren[BP_TREE_ORDER + 2];
    memset(tempKeys, 0, sizeof(tempKeys));
    memset(tempChildren, 0, sizeof(tempChildren));

    for (int i = 0; i < node->numKeys; i++) {
        tempKeys[i] = node->keys[i];
    }
    for (int i = 0; i <= node->numKeys; i++) {
        tempChildren[i] = node->children[i];
    }

    /* Insert the new key/child in the temp arrays */
    for (int i = node->numKeys; i > insertPos; i--) {
        tempKeys[i] = tempKeys[i-1];
        tempChildren[i+1] = tempChildren[i];
    }
    tempKeys[insertPos] = promoKey;
    tempChildren[insertPos+1] = promoChild;

    int total = node->numKeys + 1;
    int splitPoint = total / 2; /* median index for the new root key */

    /* Create a new internal node */
    BPNode *rightNode = createNode(/* isLeaf= */ 0);

    /* We copy left side back to original node */
    node->numKeys = splitPoint;
    for (int i = 0; i < splitPoint; i++) {
        node->keys[i] = tempKeys[i];
        node->children[i] = tempChildren[i];
    }
    node->children[splitPoint] = tempChildren[splitPoint];

    for (int i = splitPoint; i < BP_TREE_ORDER - 1; i++) {
        node->keys[i] = NULL;
        node->children[i+1] = NULL;
    }

    /* The middle key will be promoted to parent */
    *newKey = tempKeys[splitPoint];

    /* Right node gets the right side of the split */
    rightNode->numKeys = total - splitPoint - 1;
    for (int i = 0; i < rightNode->numKeys; i++) {
        rightNode->keys[i] = tempKeys[i + splitPoint + 1];
        rightNode->children[i] = tempChildren[i + splitPoint + 1];
    }
    rightNode->children[rightNode->numKeys] = tempChildren[total];

    /* The newChild is the newly created internal node */
    *newChild = rightNode;
}

/* ------------------------- B+ TREE DELETE ------------------------- */

void bpTreeDelete(BPTree *tree, const void *data)
{
    if (!tree || !tree->root || tree->root->numKeys == 0) return;
    int changed = deleteNode(tree, tree->root, data);
    if (changed) {
        tree->size--;
    }
    /* If root is an internal node with no keys, reduce height */
    if (!tree->root->isLeaf && tree->root->numKeys == 0) {
        BPNode *toFree = tree->root;
        /* The new root is its only child */
        tree->root = tree->root->children[0];
        free(toFree);
    }
}

static int deleteNode(BPTree *tree, BPNode *node, const void* data)
{
    if (node->isLeaf) {
        /* Find the key to remove */
        int idx = -1;
        for (int i = 0; i < node->numKeys; i++) {
            int cmp = tree->compareFunc(data, node->keys[i]);
            if (cmp == 0) {
                idx = i;
                break;
            } else if (cmp < 0) {
                break;
            }
        }
        if (idx == -1) {
            return 0; /* not found */
        }
        /* found, remove */
        tree->freeFunc(node->keys[idx]);
        node->keys[idx] = NULL;
        node->values[idx] = NULL;
        for (int i = idx; i < node->numKeys - 1; i++) {
            node->keys[i] = node->keys[i+1];
            node->values[i] = node->values[i+1];
        }
        node->keys[node->numKeys - 1] = NULL;
        node->values[node->numKeys - 1] = NULL;
        node->numKeys--;

        return 1; /* removed one item */
    } else {
        /* Internal node search */
        int i = 0;
        while (i < node->numKeys && tree->compareFunc(data, node->keys[i]) >= 0) {
            i++;
        }
        int changed = deleteNode(tree, node->children[i], data);
        if (changed) {
            /* Check if child underflowed */
            rebalanceAfterDelete(tree, node, i);
        }
        return changed;
    }
}

/* Rebalance after a deletion if the child at childIndex has underflowed. */
static void rebalanceAfterDelete(BPTree *tree, BPNode *node, int childIndex)
{
    BPNode *child = node->children[childIndex];
    if (!child) return;

    int minKeys = (BP_TREE_ORDER - 1) / 2; /* Rough min for this toy example */

    if (child->numKeys >= minKeys) {
        return; /* no underflow */
    }

    /* Try borrow from sibling or merge */
    BPNode *leftSibling  = (childIndex - 1 >= 0) ? node->children[childIndex - 1] : NULL;
    BPNode *rightSibling = (childIndex + 1 <= node->numKeys) ? node->children[childIndex + 1] : NULL;

    /* For simplicity, we demonstrate merge if cannot borrow from left sibling. 
       A robust implementation would also attempt borrowing from left or right if possible. */

    if (leftSibling && leftSibling->numKeys > minKeys) {
        /* Borrow from left sibling (not fully implemented). 
           For brevity, let's skip and go straight to merge for illustration. */
    } else if (rightSibling && rightSibling->numKeys > minKeys) {
        /* Borrow from right sibling (also omitted for brevity). */
    } else {
        /* Merge with sibling. Prefer left sibling if exists, else right sibling. */
        if (leftSibling) {
            /* Merge child into leftSibling */
            if (child->isLeaf) {
                mergeLeafNodes(tree, leftSibling, child, childIndex - 1, node);
            } else {
                mergeInternalNodes(tree, leftSibling, child, childIndex - 1, node);
            }
        } else if (rightSibling) {
            /* Merge rightSibling into child */
            if (child->isLeaf) {
                mergeLeafNodes(tree, child, rightSibling, childIndex, node);
            } else {
                mergeInternalNodes(tree, child, rightSibling, childIndex, node);
            }
        }
    }
}

static void mergeLeafNodes(BPTree *tree, BPNode *left, BPNode *right,
                           int parentIndex, BPNode *parent)
{
    /* Move all keys from right to left */
    int start = left->numKeys;
    for (int i = 0; i < right->numKeys; i++) {
        left->keys[start + i]   = right->keys[i];
        left->values[start + i] = right->values[i];
    }
    left->numKeys += right->numKeys;

    /* Fix leaf links */
    left->nextLeaf = right->nextLeaf;
    if (right->nextLeaf) {
        right->nextLeaf->prevLeaf = left;
    }

    /* Remove right pointer from parent */
    for (int i = parentIndex; i < parent->numKeys - 1; i++) {
        parent->keys[i] = parent->keys[i+1];
        parent->children[i+1] = parent->children[i+2];
    }
    parent->keys[parent->numKeys - 1] = NULL;
    parent->children[parent->numKeys] = NULL;
    parent->numKeys--;

    /* Free the 'right' node structure */
    free(right);
}

static void mergeInternalNodes(BPTree *tree, BPNode *left, BPNode *right,
                               int parentIndex, BPNode *parent)
{
    /* Promote parent's key[parentIndex] into left? 
       (In some B+ tree variants, the separator is "lost"; 
        in others it is re-inserted. Implementation-specific.)
       For simplicity, let's just treat it like a typical B-Tree merge: */

    int leftStart = left->numKeys;
    /* Insert parent's key at left->keys[leftStart] */
    left->keys[leftStart] = parent->keys[parentIndex];
    left->numKeys++;

    /* Move all keys/children from right into left */
    for (int i = 0; i < right->numKeys; i++) {
        left->keys[leftStart + 1 + i] = right->keys[i];
    }
    for (int i = 0; i <= right->numKeys; i++) {
        left->children[leftStart + 1 + i] = right->children[i];
    }
    left->numKeys += right->numKeys;

    /* Remove right pointer from parent */
    for (int i = parentIndex; i < parent->numKeys - 1; i++) {
        parent->keys[i] = parent->keys[i+1];
        parent->children[i+1] = parent->children[i+2];
    }
    parent->keys[parent->numKeys - 1] = NULL;
    parent->children[parent->numKeys] = NULL;
    parent->numKeys--;

    free(right);
}

/* --------------- OPTIONAL: B+ TREE SIZE (NUMBER OF ITEMS) --------------- */
size_t bpTreeSize(const BPTree *tree)
{
    if (!tree) return 0;
    return tree->size;
}
