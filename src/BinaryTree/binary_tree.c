#include "binary_tree.h"
#include <stdlib.h>

/* Forward declarations of static (private) helper functions */
static BinaryTreeNode* createNode(int value);
static void freeNodes(BinaryTreeNode* node);
static size_t countNodesHelper(const BinaryTreeNode* node);
static BinaryTreeNode* findNodeHelper(BinaryTreeNode* node, int value);

BinaryTree* btCreate(void) {
    BinaryTree* tree = (BinaryTree*)malloc(sizeof(BinaryTree));
    if (!tree) {
        return NULL;
    }
    tree->root = NULL;
    return tree;
}

void btDestroy(BinaryTree* tree) {
    if (!tree) {
        return;
    }
    freeNodes(tree->root);
    free(tree);
}

bool btSetRoot(BinaryTree* tree, int value) {
    if (!tree) {
        return false;
    }
    if (tree->root != NULL) {
        // Root already exists, do not overwrite
        return false;
    }
    BinaryTreeNode* node = createNode(value);
    if (!node) {
        return false;
    }
    tree->root = node;
    return true;
}

bool btInsertLeft(BinaryTreeNode* parent, int value) {
    if (!parent) {
        return false;
    }
    if (parent->left != NULL) {
        // Already has a left child, do not overwrite
        return false;
    }
    BinaryTreeNode* node = createNode(value);
    if (!node) {
        return false;
    }
    parent->left = node;
    return true;
}

bool btInsertRight(BinaryTreeNode* parent, int value) {
    if (!parent) {
        return false;
    }
    if (parent->right != NULL) {
        // Already has a right child
        return false;
    }
    BinaryTreeNode* node = createNode(value);
    if (!node) {
        return false;
    }
    parent->right = node;
    return true;
}

size_t btCountNodes(const BinaryTree* tree) {
    if (!tree || !tree->root) {
        return 0;
    }
    return countNodesHelper(tree->root);
}

BinaryTreeNode* btFind(const BinaryTree* tree, int value) {
    if (!tree) {
        return NULL;
    }
    return findNodeHelper(tree->root, value);
}

/* --------------------
 * Static Helper Functions
 * --------------------
 */

/* Create a single new node with the given value. */
static BinaryTreeNode* createNode(int value) {
    BinaryTreeNode* node = (BinaryTreeNode*)malloc(sizeof(BinaryTreeNode));
    if (!node) {
        return NULL;
    }
    node->data = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* Free all nodes in the subtree rooted at 'node'. */
static void freeNodes(BinaryTreeNode* node) {
    if (!node) {
        return;
    }
    freeNodes(node->left);
    freeNodes(node->right);
    free(node);
}

/* Returns the count of nodes in the subtree rooted at 'node'. */
static size_t countNodesHelper(const BinaryTreeNode* node) {
    if (!node) {
        return 0;
    }
    // 1 for this node + left subtree count + right subtree count
    return 1 + countNodesHelper(node->left) + countNodesHelper(node->right);
}

/* Searches the subtree rooted at 'node' for 'value'. */
static BinaryTreeNode* findNodeHelper(BinaryTreeNode* node, int value) {
    if (!node) {
        return NULL;
    }
    if (node->data == value) {
        return node;
    }
    // Search left subtree
    BinaryTreeNode* found = findNodeHelper(node->left, value);
    if (found) {
        return found;
    }
    // Otherwise search right subtree
    return findNodeHelper(node->right, value);
}
