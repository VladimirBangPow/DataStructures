#include "splay_tree.h"
#include <stdlib.h>
#include <assert.h>

/* 
 * Internal node structure for the splay tree.
 */
typedef struct SplayTreeNode {
    void* data;
    struct SplayTreeNode* left;
    struct SplayTreeNode* right;
} SplayTreeNode;

/* 
 * The splay tree handle holds a pointer to the root node,
 * a comparator function, a destructor function, and node count.
 */
struct SplayTree {
    SplayTreeNode* root;
    CompareFunc compare;
    DestroyFunc destroy;
    size_t size;
};

/* 
 * Utility: Create a new node.
 */
static SplayTreeNode* splayNodeCreate(void* data) {
    SplayTreeNode* node = (SplayTreeNode*)malloc(sizeof(SplayTreeNode));
    if (!node) return NULL;
    node->data = data;
    node->left = node->right = NULL;
    return node;
}

/* 
 * Utility: Destroy a node (calls user-provided destructor if any).
 */
static void splayNodeDestroy(SplayTreeNode* node, DestroyFunc destroy) {
    if (destroy && node->data) {
        destroy(node->data);
    }
    free(node);
}

/* 
 * Right rotation:
 * 
 *    p           q
 *   / \         / \
 *  q   C  -->  A   p
 * / \             / \
 *A   B           B   C
 */
static SplayTreeNode* rotateRight(SplayTreeNode* p) {
    SplayTreeNode* q = p->left;
    if (!q) return p;
    p->left = q->right;
    q->right = p;
    return q;
}

/* 
 * Left rotation:
 *
 *   p              q
 *  / \            / \
 * A   q   -->    p   C
 *    / \        / \
 *   B   C      A   B
 */
static SplayTreeNode* rotateLeft(SplayTreeNode* p) {
    SplayTreeNode* q = p->right;
    if (!q) return p;
    p->right = q->left;
    q->left = p;
    return q;
}

/*
 * Splay operation: moves the node with data == 'key' (if it exists)
 * or the last accessed node on the search path to the root.
 *
 * This implementation uses the "bottom-up" splaying approach 
 * (Zig, Zig-Zig, Zig-Zag).
 *
 * Returns the new root after the splay operation.
 */
SplayTreeNode* splay(SplayTreeNode* root, void* key, CompareFunc cmp) {
    if (root == NULL) { return NULL; }
    int comp = cmp(key, root->data);
    if (comp < 0) {
        if (root->left == NULL) { return root; }
        int compLeft = cmp(key, root->left->data);
        if (compLeft < 0) {
            root->left->left = splay(root->left->left, key, cmp);
            root = rotateRight(root);
        } else if (compLeft > 0) {
            root->left->right = splay(root->left->right, key, cmp);
            if (root->left->right) {
                root->left = rotateLeft(root->left);
            }
        }
        return (root->left == NULL) ? root : rotateRight(root);
    }
    else if (comp > 0) {
        if (root->right == NULL) { return root; }
        int compRight = cmp(key, root->right->data);
        if (compRight > 0) {
            root->right->right = splay(root->right->right, key, cmp);
            root = rotateLeft(root);
        } else if (compRight < 0) {
            root->right->left = splay(root->right->left, key, cmp);
            if (root->right->left) {
                root->right = rotateRight(root->right);
            }
        }
        return (root->right == NULL) ? root : rotateLeft(root);
    }
    else {
        return root;
    }
}

/*
 * Recursive helper to destroy all nodes in the subtree.
 */
static void destroySubtree(SplayTreeNode* root, DestroyFunc destroy) {
    if (!root) return;
    destroySubtree(root->left, destroy);
    destroySubtree(root->right, destroy);
    splayNodeDestroy(root, destroy);
}

/*
 * Recursive function to check BST property:
 *  - All values in left subtree < root->data
 *  - All values in right subtree > root->data
 */
static int isBSTUtil(SplayTreeNode* root, CompareFunc cmp, void* minKey, void* maxKey) {
    if (!root) return 1;
    /* Check current node vs. minKey/maxKey */
    if (minKey && cmp(root->data, minKey) <= 0) return 0;
    if (maxKey && cmp(root->data, maxKey) >= 0) return 0;
    /* Recursively check subtrees */
    return isBSTUtil(root->left, cmp, minKey, root->data) 
        && isBSTUtil(root->right, cmp, root->data, maxKey);
}

/* 
 * =======================
 *  PUBLIC API FUNCTIONS
 * =======================
 */

SplayTree* splayTreeCreate(CompareFunc compare, DestroyFunc destroy) {
    SplayTree* tree = (SplayTree*)malloc(sizeof(SplayTree));
    if (!tree) return NULL;
    tree->root = NULL;
    tree->compare = compare;
    tree->destroy = destroy;
    tree->size = 0;
    return tree;
}

void splayTreeDestroy(SplayTree* tree) {
    if (!tree) return;
    destroySubtree(tree->root, tree->destroy);
    free(tree);
}

int splayTreeInsert(SplayTree* tree, void* data) {
    if (!tree) return 0;
    if (!tree->root) {
        // Empty tree, create root
        tree->root = splayNodeCreate(data);
        if (!tree->root) return 0;
        tree->size = 1;
        return 1;
    }

    // Splay the key to the root (or last accessed)
    tree->root = splay(tree->root, data, tree->compare);

    int comp = tree->compare(data, tree->root->data);
    if (comp == 0) {
        // Key already exists, do not insert a duplicate
        // (Alternatively, you could decide to allow duplicates)
        // For now, we just replace data or ignore.
        // We'll ignore here, but you might want to free `data` or handle it differently.
        return 1;
    }

    // Insert new node
    SplayTreeNode* node = splayNodeCreate(data);
    if (!node) return 0;

    // If new data < root->data, attach root->left
    // else attach root->right
    if (comp < 0) {
        node->left = tree->root->left;
        node->right = tree->root;
        tree->root->left = NULL;
    } else {
        node->right = tree->root->right;
        node->left = tree->root;
        tree->root->right = NULL;
    }
    tree->root = node;
    tree->size++;

    return 1;
}

void* splayTreeSearch(SplayTree* tree, void* key) {
    if (!tree || !tree->root) return NULL;
    // Splay the key (or last accessed node)
    tree->root = splay(tree->root, key, tree->compare);

    // Check if the root is now the key
    if (tree->compare(key, tree->root->data) == 0) {
        return tree->root->data;
    }
    return NULL;
}

int splayTreeDelete(SplayTree* tree, void* key) {
    if (!tree || !tree->root) return 0;

    // Splay the key to the root
    tree->root = splay(tree->root, key, tree->compare);

    // Check if root->data == key
    if (tree->compare(key, tree->root->data) != 0) {
        // key not found
        return 0;
    }

    // Now root is the node to delete
    SplayTreeNode* toDelete = tree->root;

    // If no left subtree, replace root with right subtree
    if (!tree->root->left) {
        tree->root = tree->root->right;
    } else {
        // Splay the maximum in the left subtree (by splaying 'key' bigger than any in left)
        SplayTreeNode* leftSubtree = tree->root->left;
        // Disconnect
        toDelete->left = NULL;

        // Splay with a key guaranteed to be larger than anything in leftSubtree
        leftSubtree = splay(leftSubtree, key, tree->compare);
        // Now leftSubtree's root has no right child
        leftSubtree->right = toDelete->right;
        tree->root = leftSubtree;
    }

    // Destroy the deleted node
    splayNodeDestroy(toDelete, tree->destroy);
    tree->size--;

    return 1;
}

size_t splayTreeSize(const SplayTree* tree) {
    if (!tree) return 0;
    return tree->size;
}

int splayTreeIsValidBST(const SplayTree* tree) {
    if (!tree) return 1; // empty tree is trivially valid
    return isBSTUtil(tree->root, tree->compare, NULL, NULL);
}
