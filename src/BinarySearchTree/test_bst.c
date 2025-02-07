#include "test_bst.h"
#include "bst.h"
#include <assert.h>
#include <stdio.h>

/**
 * A helper function to verify an in-order traversal
 * against an expected array of values.
 */
static void checkInOrderTraversal(BSTNode* root, const int* expected, int size) {
    static int index = 0; 
    index = 0;

    // Local function to do an in-order traversal
    void traverse(BSTNode* node) {
        if (!node) return;
        traverse(node->left);
        assert(node->key == expected[index] && "In-order sequence mismatch");
        index++;
        traverse(node->right);
    }

    traverse(root);
    // After traversing, index should match size
    assert(index == size && "In-order traversal node count mismatch");
}

void testBinarySearchTree(void) {
    printf("Running Binary Search Tree Tests...\n");

    BSTNode* root = NULL;

    /* 
     * Test 1: Insertion & Search 
     */
    root = insertBSTNode(root, 10);
    assert(root != NULL && "Root should not be NULL after first insert");
    assert(root->key == 10 && "Root key should be 10");

    // Insert more
    insertBSTNode(root, 5);
    insertBSTNode(root, 15);
    insertBSTNode(root, 2);
	insertBSTNode(root, 7);
    insertBSTNode(root, 12);
    insertBSTNode(root, 20);

    // Search for existing keys
    BSTNode* found = searchBST(root, 7);
    assert(found != NULL && "Search should find 7 in the tree");
    assert(found->key == 7 && "Found node's key should be 7");

    // Search for non-existing key
    found = searchBST(root, 999);
    assert(found == NULL && "Search for 999 should return NULL");

    /*
     * Test 2: Find Min/Max
     */
    BSTNode* minNode = findMinBSTNode(root);
    assert(minNode != NULL && "Min node should not be NULL in a non-empty tree");
    assert(minNode->key == 2 && "Minimum key should be 2");

    BSTNode* maxNode = findMaxBSTNode(root);
    assert(maxNode != NULL && "Max node should not be NULL in a non-empty tree");
    assert(maxNode->key == 20 && "Maximum key should be 20");

    /*
     * Test 3: In-order traversal check
     * The in-order should result in: 2, 5, 7, 10, 12, 15, 20
     */
    {
        int expectedOrder[] = {2, 5, 7, 10, 12, 15, 20};
        int size = sizeof(expectedOrder)/sizeof(expectedOrder[0]);
        checkInOrderTraversal(root, expectedOrder, size);
    }

    /*
     * Test 4: Delete leaf node
     * Deleting 2 (which is a leaf)
     */
    root = deleteBSTNode(root, 2);
    assert(searchBST(root, 2) == NULL && "2 should no longer be in the tree");

    /*
     * Test 5: Delete node with one child
     * Delete 5 (which now either has left or right child).
     */
    root = deleteBSTNode(root, 5);
    assert(searchBST(root, 5) == NULL && "5 should no longer be in the tree");

    /*
     * Test 6: Delete node with two children
     * Delete 10 (the root), which has two children (7, 15).
     */
    root = deleteBSTNode(root, 10);
    assert(searchBST(root, 10) == NULL && "10 should no longer be in the tree");

    // Check some keys remain
    assert(searchBST(root, 15) != NULL && "15 should still be in the tree");
    assert(searchBST(root, 20) != NULL && "20 should still be in the tree");

    /*
     * Test 7: Free the tree
     */
    freeBST(root);
    root = NULL; // Good practice to nullify after freeing

    printf("All Binary Search Tree tests passed!\n");
}
