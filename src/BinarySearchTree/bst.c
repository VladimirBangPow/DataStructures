#include <stdio.h>
#include <stdlib.h>
#include "bst.h"

/* Create a new BST node */
BSTNode* createBSTNode(int key) {
    BSTNode* newNode = (BSTNode*)malloc(sizeof(BSTNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    newNode->key = key;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

/* Insert a key into the BST */
BSTNode* insertBSTNode(BSTNode* root, int key) {
    if (root == NULL) {
        // Tree is empty, create a new node
        return createBSTNode(key);
    }

    if (key < root->key) {
        root->left = insertBSTNode(root->left, key);
    } else if (key > root->key) {
        root->right = insertBSTNode(root->right, key);
    }
    // If key == root->key, we can decide to do nothing or handle duplicates
    // For this example, do nothing if key already exists.

    return root;
}

/* Search for a key in the BST */
BSTNode* searchBST(BSTNode* root, int key) {
    if (root == NULL || root->key == key) {
        return root;
    }

    if (key < root->key) {
        return searchBST(root->left, key);
    } else {
        return searchBST(root->right, key);
    }
}

/* Find the node with the minimum key in the BST */
BSTNode* findMinBSTNode(BSTNode* root) {
    if (root == NULL) {
        return NULL;
    }

    BSTNode* current = root;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

/* Find the node with the maximum key in the BST */
BSTNode* findMaxBSTNode(BSTNode* root) {
    if (root == NULL) {
        return NULL;
    }

    BSTNode* current = root;
    while (current->right != NULL) {
        current = current->right;
    }
    return current;
}

/* Delete a key from the BST */
BSTNode* deleteBSTNode(BSTNode* root, int key) {
    if (root == NULL) {
        // Key not found
        return NULL;
    }

    if (key < root->key) {
        root->left = deleteBSTNode(root->left, key);
    } else if (key > root->key) {
        root->right = deleteBSTNode(root->right, key);
    } else {
        // root->key == key, this is the node to delete
        if (root->left == NULL && root->right == NULL) {
            // Case 1: No children
            free(root);
            root = NULL;
        } else if (root->left == NULL) {
            // Case 2: One child (right)
            BSTNode* temp = root->right;
            free(root);
            root = temp;
        } else if (root->right == NULL) {
            // Case 2: One child (left)
            BSTNode* temp = root->left;
            free(root);
            root = temp;
        } else {
            // Case 3: Two children
            // Find the minimum node in the right subtree
            BSTNode* temp = findMinBSTNode(root->right);
            // Copy the key to the root
            root->key = temp->key;
            // Delete the minimum node in the right subtree
            root->right = deleteBSTNode(root->right, temp->key);
        }
    }
    return root;
}

/* In-order traversal */
void inorderTraversal(BSTNode* root) {
    if (root == NULL) {
        return;
    }
    inorderTraversal(root->left);
    printf("%d ", root->key);
    inorderTraversal(root->right);
}

/* Pre-order traversal */
void preorderTraversal(BSTNode* root) {
    if (root == NULL) {
        return;
    }
    printf("%d ", root->key);
    preorderTraversal(root->left);
    preorderTraversal(root->right);
}

/* Post-order traversal */
void postorderTraversal(BSTNode* root) {
    if (root == NULL) {
        return;
    }
    postorderTraversal(root->left);
    postorderTraversal(root->right);
    printf("%d ", root->key);
}

/* Free the entire BST */
void freeBST(BSTNode* root) {
    if (root == NULL) {
        return;
    }
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}
