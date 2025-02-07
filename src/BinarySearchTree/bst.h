#ifndef BST_H
#define BST_H

/**
 * A node in the Binary Search Tree
 */
typedef struct BSTNode {
    int key;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

/**
 * Creates a new BST node with the given key.
 * @param key The key to store in the node.
 * @return A pointer to the newly created BSTNode.
 */
BSTNode* createBSTNode(int key);

/**
 * Inserts a key into the BST. If the root is NULL, 
 * a new node is created.
 * @param root The current root of the BST.
 * @param key The key to insert.
 * @return The new root of the BST (may be the same if root is non-NULL).
 */
BSTNode* insertBSTNode(BSTNode* root, int key);

/**
 * Searches for a key in the BST.
 * @param root The root of the BST.
 * @param key The key to search for.
 * @return Pointer to the BSTNode containing the key, or NULL if not found.
 */
BSTNode* searchBST(BSTNode* root, int key);

/**
 * Finds the node with the minimum key in the BST.
 * @param root The root of the BST.
 * @return The node with the smallest key, or NULL if tree is empty.
 */
BSTNode* findMinBSTNode(BSTNode* root);

/**
 * Finds the node with the maximum key in the BST.
 * @param root The root of the BST.
 * @return The node with the largest key, or NULL if tree is empty.
 */
BSTNode* findMaxBSTNode(BSTNode* root);

/**
 * Deletes a key from the BST if it exists.
 * @param root The current root of the BST.
 * @param key The key to delete.
 * @return The new root after deletion.
 */
BSTNode* deleteBSTNode(BSTNode* root, int key);

/**
 * Performs an in-order traversal of the BST and prints the keys.
 * @param root The current root of the BST.
 */
void inorderTraversal(BSTNode* root);

/**
 * Performs a pre-order traversal of the BST and prints the keys.
 * @param root The current root of the BST.
 */
void preorderTraversal(BSTNode* root);

/**
 * Performs a post-order traversal of the BST and prints the keys.
 * @param root The current root of the BST.
 */
void postorderTraversal(BSTNode* root);

/**
 * Frees all nodes in the BST.
 * @param root The current root of the BST.
 */
void freeBST(BSTNode* root);

#endif // BST_H
