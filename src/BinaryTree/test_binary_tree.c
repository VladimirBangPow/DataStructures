#include "test_binary_tree.h"
#include "binary_tree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Test basic creation, root-setting, and insertion to left/right */
static void testBasicInsertion(void) {
    BinaryTree* tree = btCreate();
    assert(tree != NULL);
    assert(tree->root == NULL);

    // Set the root (should succeed)
    bool result = btSetRoot(tree, 10);
    assert(result == true);
    assert(tree->root != NULL);
    assert(tree->root->data == 10);
    assert(btCountNodes(tree) == 1);

    // Insert left child
    result = btInsertLeft(tree->root, 5);
    assert(result == true);
    assert(tree->root->left != NULL);
    assert(tree->root->left->data == 5);
    assert(btCountNodes(tree) == 2);

    // Insert right child
    result = btInsertRight(tree->root, 15);
    assert(result == true);
    assert(tree->root->right != NULL);
    assert(tree->root->right->data == 15);
    assert(btCountNodes(tree) == 3);

    // Attempt to reset root -> should fail because root already exists
    result = btSetRoot(tree, 999);
    assert(result == false);

    // Attempt to insert left child where one already exists -> should fail
    result = btInsertLeft(tree->root, 999);
    assert(result == false);

    // Attempt to insert right child where one already exists -> should fail
    result = btInsertRight(tree->root, 999);
    assert(result == false);

    btDestroy(tree);
}

/* Test searching for values in a small tree */
static void testFind(void) {
    BinaryTree* tree = btCreate();
    btSetRoot(tree, 10);

    // Insert children around the root
    btInsertLeft(tree->root, 5);
    btInsertRight(tree->root, 15);

    // Insert more nodes
    btInsertLeft(tree->root->left, 2);
    btInsertRight(tree->root->left, 7);
    btInsertLeft(tree->root->right, 12);
    btInsertRight(tree->root->right, 20);

    // We should now have 7 total nodes
    assert(btCountNodes(tree) == 7);

    // Find existing values
    assert(btFind(tree, 10) == tree->root);
    assert(btFind(tree, 5) == tree->root->left);
    assert(btFind(tree, 2) == tree->root->left->left);
    assert(btFind(tree, 7) == tree->root->left->right);
    assert(btFind(tree, 12) == tree->root->right->left);
    assert(btFind(tree, 20) == tree->root->right->right);

    // Find a non-existent value
    assert(btFind(tree, 999) == NULL);

    btDestroy(tree);
}

/* A simple "stress" test that creates a large chain to the left */
static void testStress(void) {
    const int N = 10000;

    BinaryTree* tree = btCreate();
    bool result = btSetRoot(tree, 0);
    assert(result == true);

    int nodeCount = 1;  // root is 1 node
    BinaryTreeNode* current = tree->root;

    // Create a long chain of left children
    for (int i = 1; i < N; i++) {
        bool inserted = btInsertLeft(current, i);
        assert(inserted == true);
        current = current->left;
        nodeCount++;
    }

    // Verify node count
    assert(btCountNodes(tree) == (size_t)nodeCount);

    // Spot-check a few finds
    BinaryTreeNode* found = btFind(tree, N - 1);
    assert(found != NULL);
    assert(found->data == (N - 1));

    // Value outside the range should not be found
    found = btFind(tree, N);
    assert(found == NULL);

    btDestroy(tree);
}

/*
 * This is the "master" function that runs all sub-tests.
 * You can call this from your main or elsewhere.
 */
void testBinaryTree(void) {
    printf("Beginning BinaryTree tests...\n");

    testBasicInsertion();
    printf(" - Basic insertion tests passed.\n");

    testFind();
    printf(" - Find tests passed.\n");

    testStress();
    printf(" - Stress test passed.\n");

    printf("All BinaryTree tests PASSED successfully.\n");
}
