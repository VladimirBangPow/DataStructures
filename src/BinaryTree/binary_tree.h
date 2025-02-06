#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <stdbool.h>
#include <stddef.h>

/*
 * A node in a plain binary tree storing an integer value.
 * left and right point to subtrees (or NULL).
 */
typedef struct BinaryTreeNode {
    int data;
    struct BinaryTreeNode* left;
    struct BinaryTreeNode* right;
} BinaryTreeNode;

/*
 * A binary tree object that holds a pointer to the root node.
 */
typedef struct BinaryTree {
    BinaryTreeNode* root;
} BinaryTree;

/*
 * Creates a new, empty BinaryTree structure.
 * Returns a pointer to a newly allocated BinaryTree,
 * or NULL if allocation fails.
 */
BinaryTree* btCreate(void);

/*
 * Destroys a BinaryTree and frees all nodes.
 * The 'tree' pointer itself is then invalid.
 */
void btDestroy(BinaryTree* tree);

/*
 * Sets the root of the tree to a new node with 'value'
 * ONLY if the current root is NULL (to avoid overwriting).
 * Returns 'true' on success, 'false' otherwise.
 */
bool btSetRoot(BinaryTree* tree, int value);

/*
 * Inserts a new node with 'value' as the LEFT child of 'parent',
 * provided that 'parent->left' is currently NULL.
 * Returns true on success, false if 'parent' is NULL, or 'parent->left' is not NULL,
 * or if allocation fails.
 */
bool btInsertLeft(BinaryTreeNode* parent, int value);

/*
 * Inserts a new node with 'value' as the RIGHT child of 'parent',
 * provided that 'parent->right' is currently NULL.
 * Returns true on success, false on error (same reasons as btInsertLeft).
 */
bool btInsertRight(BinaryTreeNode* parent, int value);

/*
 * Counts the total number of nodes in the tree.
 * Returns 0 if 'tree' is NULL or empty.
 */
size_t btCountNodes(const BinaryTree* tree);

/*
 * Searches for a node with the given 'value' anywhere in the tree.
 * Returns a pointer to the node if found, or NULL if not found or if 'tree' is NULL.
 */
BinaryTreeNode* btFind(const BinaryTree* tree, int value);

#endif // BINARY_TREE_H
