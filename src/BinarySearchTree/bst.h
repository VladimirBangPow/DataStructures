#ifndef BST_H
#define BST_H


#include <stddef.h>

/**
 * A function pointer type used for comparing two data items.
 * The function should return:
 *  - a negative value if a < b
 *  - zero if a == b
 *  - a positive value if a > b
 */
typedef int (*CompareFunc)(const void *a, const void *b);

/**
 * A function pointer type used for printing or processing a data item.
 */
typedef void (*PrintFunc)(const void *data);

/**
 * A node in the generic Binary Search Tree
 */
typedef struct BSTNode {
    void *data;                 /**< Pointer to the data */
    struct BSTNode *left;       /**< Pointer to the left child */
    struct BSTNode *right;      /**< Pointer to the right child */
} BSTNode;

/**
 * Creates a new BST node with the given data.
 * @param data Pointer to the data to store in the node.
 * @return A pointer to the newly created BSTNode.
 */
BSTNode *createBSTNode(void *data);

/**
 * Inserts a data item into the BST. If the root is NULL, 
 * a new node is created and returned as the new root.
 * @param root The current root of the BST.
 * @param data The data item to insert.
 * @param compare A function pointer to compare two data items.
 * @return The (possibly new) root of the BST.
 */
BSTNode *insertBSTNode(BSTNode *root, void *data, CompareFunc compare);

/**
 * Searches for a data item in the BST.
 * @param root The root of the BST.
 * @param data The data item to search for.
 * @param compare A function pointer to compare two data items.
 * @return Pointer to the BSTNode containing the data, or NULL if not found.
 */
BSTNode *searchBST(BSTNode *root, void *data, CompareFunc compare);

/**
 * Finds the node with the minimum data in the BST.
 * (According to the compare function provided.)
 * @param root The root of the BST.
 * @return The node with the smallest data, or NULL if the tree is empty.
 */
BSTNode *findMinBSTNode(BSTNode *root);

/**
 * Finds the node with the maximum data in the BST.
 * (According to the compare function provided.)
 * @param root The root of the BST.
 * @return The node with the largest data, or NULL if the tree is empty.
 */
BSTNode *findMaxBSTNode(BSTNode *root);

/**
 * Deletes a data item from the BST if it exists.
 * @param root The current root of the BST.
 * @param data The data item to delete.
 * @param compare A function pointer to compare two data items.
 * @return The new root after deletion.
 */
BSTNode *deleteBSTNode(BSTNode *root, void *data, CompareFunc compare);

/**
 * Performs an in-order traversal of the BST and processes (or prints) the data.
 * @param root The current root of the BST.
 * @param printFunc A function pointer to process or print the data item.
 */
void inorderTraversal(const BSTNode *root, PrintFunc printFunc);

/**
 * Performs a pre-order traversal of the BST and processes (or prints) the data.
 * @param root The current root of the BST.
 * @param printFunc A function pointer to process or print the data item.
 */
void preorderTraversal(const BSTNode *root, PrintFunc printFunc);

/**
 * Performs a post-order traversal of the BST and processes (or prints) the data.
 * @param root The current root of the BST.
 * @param printFunc A function pointer to process or print the data item.
 */
void postorderTraversal(const BSTNode *root, PrintFunc printFunc);

/**
 * Frees all nodes in the BST. Note that this does NOT free the data
 * pointed to by each node, since the BST is generic and does not
 * assume ownership of the data. If you need to free the data,
 * you must do so before or during this operation (e.g. by modifying
 * this function to accept a function pointer for destroying data).
 * @param root The current root of the BST.
 */
void freeBST(BSTNode *root);


#endif // BST_H
