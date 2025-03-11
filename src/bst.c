#include <stdio.h>
#include <stdlib.h>
#include "bst.h"

/**
 * Creates a new BST node with the given data.
 */
BSTNode *createBSTNode(void *data) {
    BSTNode *newNode = (BSTNode *)malloc(sizeof(BSTNode));
    if (!newNode) {
        perror("Failed to allocate memory for a new BST node");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

/**
 * Inserts a data item into the BST.
 */
BSTNode *insertBSTNode(BSTNode *root, void *data, CompareFunc compare) {
    if (root == NULL) {
        return createBSTNode(data);
    }

    int cmp = compare(data, root->data);
    if (cmp < 0) {
        root->left = insertBSTNode(root->left, data, compare);
    } else if (cmp > 0) {
        root->right = insertBSTNode(root->right, data, compare);
    }
    /* if cmp == 0, data is considered equal; 
       you can decide whether to allow duplicates or not.
       Here, we do nothing if the data is already present.
    */
    return root;
}

/**
 * Searches for a data item in the BST.
 */
BSTNode *searchBST(BSTNode *root, void *data, CompareFunc compare) {
    if (root == NULL) {
        return NULL;
    }
    int cmp = compare(data, root->data);
    if (cmp == 0) {
        return root;
    } else if (cmp < 0) {
        return searchBST(root->left, data, compare);
    } else {
        return searchBST(root->right, data, compare);
    }
}

/**
 * Finds the node with the minimum data in the BST.
 */
BSTNode *findMinBSTNode(BSTNode *root) {
    if (root == NULL) {
        return NULL;
    }
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

/**
 * Finds the node with the maximum data in the BST.
 */
BSTNode *findMaxBSTNode(BSTNode *root) {
    if (root == NULL) {
        return NULL;
    }
    while (root->right != NULL) {
        root = root->right;
    }
    return root;
}

/**
 * Deletes a data item from the BST if it exists.
 */
BSTNode *deleteBSTNode(BSTNode *root, void *data, CompareFunc compare) {
    if (root == NULL) {
        return NULL;
    }

    int cmp = compare(data, root->data);
    if (cmp < 0) {
        root->left = deleteBSTNode(root->left, data, compare);
    } else if (cmp > 0) {
        root->right = deleteBSTNode(root->right, data, compare);
    } else {
        // This is the node we want to delete
        if (root->left == NULL) {
            BSTNode *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            BSTNode *temp = root->left;
            free(root);
            return temp;
        } else {
            // Node with two children
            BSTNode *temp = findMinBSTNode(root->right);
            root->data = temp->data; // Copy the data
            root->right = deleteBSTNode(root->right, temp->data, compare);
        }
    }
    return root;
}

/**
 * In-order traversal
 */
void inorderTraversal(const BSTNode *root, PrintFunc printFunc) {
    if (root == NULL) {
        return;
    }
    inorderTraversal(root->left, printFunc);
    printFunc(root->data);
    inorderTraversal(root->right, printFunc);
}

/**
 * Pre-order traversal
 */
void preorderTraversal(const BSTNode *root, PrintFunc printFunc) {
    if (root == NULL) {
        return;
    }
    printFunc(root->data);
    preorderTraversal(root->left, printFunc);
    preorderTraversal(root->right, printFunc);
}

/**
 * Post-order traversal
 */
void postorderTraversal(const BSTNode *root, PrintFunc printFunc) {
    if (root == NULL) {
        return;
    }
    postorderTraversal(root->left, printFunc);
    postorderTraversal(root->right, printFunc);
    printFunc(root->data);
}

/**
 * Free all nodes in the BST. This does NOT free the data pointed to
 * by each node. If you need to free the data, you can modify this
 * function to accept a destructor function pointer for user data.
 */
void freeBST(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}
