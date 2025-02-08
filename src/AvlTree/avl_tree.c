#include "avl_tree.h"
#include <stdlib.h>
#include <stdio.h>

/* --------------------------------------------------
 *  Utility Functions: height, balance, max, etc.
 * -------------------------------------------------- */

/* We define the "height of an empty node" as -1, and a leaf has height 0 */
static int get_height(AvlNode *node) {
    return (node == NULL) ? -1 : node->height;
}

static int max(int a, int b) {
    return (a > b) ? a : b;
}

/* Recomputes the node->height from its children. */
static void update_height(AvlNode *node) {
    if (node) {
        int left_h = get_height(node->left);
        int right_h = get_height(node->right);
        node->height = 1 + max(left_h, right_h);
    }
}

/* Balance factor = height(left subtree) - height(right subtree). */
static int get_balance(AvlNode *node) {
    if (!node) return 0;
    return get_height(node->left) - get_height(node->right);
}

/* --------------------------------------------------
 *  Rotations
 * -------------------------------------------------- */

/* Right rotation to fix LEFT-HEAVY subtree (LL case) */
static AvlNode *rotate_right(AvlNode *y) {
    AvlNode *x = y->left;
    AvlNode *T2 = x->right;

    /* Perform rotation */
    x->right = y;
    y->left = T2;

    /* Update heights */
    update_height(y);
    update_height(x);

    /* x becomes the new root of this subtree */
    return x;
}

/* Left rotation to fix RIGHT-HEAVY subtree (RR case) */
static AvlNode *rotate_left(AvlNode *x) {
    AvlNode *y = x->right;
    AvlNode *T2 = y->left;

    /* Perform rotation */
    y->left = x;
    x->right = T2;

    /* Update heights */
    update_height(x);
    update_height(y);

    /* y becomes the new root of this subtree */
    return y;
}

/* --------------------------------------------------
 *  AVL Insertion (recursive)
 * -------------------------------------------------- */
static AvlNode *avl_insert_node(AvlNode *node, void *data,
                                int (*compare)(const void*, const void*)) {
    /* Normal BST insertion */
    if (node == NULL) {
        AvlNode *new_node = (AvlNode *)malloc(sizeof(AvlNode));
        new_node->data = data;
        new_node->left = new_node->right = NULL;
        new_node->height = 0;
        return new_node;
    }

    /* Compare to decide which side to go */
    int cmp = compare(data, node->data);
    if (cmp < 0) {
        node->left = avl_insert_node(node->left, data, compare);
    } else if (cmp > 0) {
        node->right = avl_insert_node(node->right, data, compare);
    } else {
        /* Duplicate data? For simplicity, we do nothing. */
        return node;
    }

    /* Update height of this ancestor node */
    update_height(node);

    /* Check the balance factor to see if rebalancing is needed */
    int balance = get_balance(node);

    /* LL Case: left-heavy, and data < node->left->data */
    if (balance > 1 && compare(data, node->left->data) < 0) {
        return rotate_right(node);
    }
    /* RR Case: right-heavy, and data > node->right->data */
    if (balance < -1 && compare(data, node->right->data) > 0) {
        return rotate_left(node);
    }
    /* LR Case: left-heavy, but data > node->left->data */
    if (balance > 1 && compare(data, node->left->data) > 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    /* RL Case: right-heavy, but data < node->right->data */
    if (balance < -1 && compare(data, node->right->data) < 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    /* If balanced, just return the node */
    return node;
}

/* --------------------------------------------------
 *  AVL Removal (recursive)
 * -------------------------------------------------- */

/* Helper to find the minimum node in the right subtree (in-order successor) */
static AvlNode *find_min_node(AvlNode *node) {
    AvlNode *current = node;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

static AvlNode *avl_remove_node(AvlNode *node, void *data,
                                int (*compare)(const void*, const void*),
                                void (*free_data)(void *)) {
    if (node == NULL) {
        /* Data not found; do nothing */
        return NULL;
    }

    int cmp = compare(data, node->data);
    if (cmp < 0) {
        node->left = avl_remove_node(node->left, data, compare, free_data);
    } else if (cmp > 0) {
        node->right = avl_remove_node(node->right, data, compare, free_data);
    } else {
        /* We found the node to be deleted */
        if (node->left == NULL || node->right == NULL) {
            /* One child or no child */
            AvlNode *temp = node->left ? node->left : node->right;

            if (free_data) {
                free_data(node->data);
            }
            free(node);

            return temp;  /* could be NULL if no children */
        } else {
            /* Node with two children:
             * get the in-order successor (smallest in right subtree) */
            AvlNode *minRight = find_min_node(node->right);
            /* Copy its data to this node */
            node->data = minRight->data;
            /*
             * Remove the in-order successor from the right subtree.
             * Note: We do NOT free `minRight->data` here because
             * we *moved* that pointer up into node->data.
             */
            node->right = avl_remove_node(node->right, minRight->data,
                                          compare, NULL);
        }
    }

    /* If the tree had only one node, just return */
    if (node == NULL) {
        return NULL;
    }

    /* Update height of the current node */
    update_height(node);

    /* Rebalance if needed */
    int balance = get_balance(node);

    /* LL Case */
    if (balance > 1 && get_balance(node->left) >= 0) {
        return rotate_right(node);
    }
    /* LR Case */
    if (balance > 1 && get_balance(node->left) < 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    /* RR Case */
    if (balance < -1 && get_balance(node->right) <= 0) {
        return rotate_left(node);
    }
    /* RL Case */
    if (balance < -1 && get_balance(node->right) > 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

/* --------------------------------------------------
 *  AVL Search (recursive)
 * -------------------------------------------------- */
static void *avl_search_node(AvlNode *node, void *data,
                             int (*compare)(const void*, const void*)) {
    if (node == NULL) {
        return NULL;
    }
    int cmp = compare(data, node->data);
    if (cmp == 0) {
        return node->data;
    } else if (cmp < 0) {
        return avl_search_node(node->left, data, compare);
    } else {
        return avl_search_node(node->right, data, compare);
    }
}

/* --------------------------------------------------
 *  Public API
 * -------------------------------------------------- */
AvlTree *avl_create(int (*compare)(const void *, const void *)) {
    AvlTree *tree = (AvlTree *)malloc(sizeof(AvlTree));
    tree->root = NULL;
    tree->compare = compare;
    return tree;
}

/* Post-order traversal to free nodes */
static void avl_destroy_node(AvlNode *node, void (*free_data)(void *)) {
    if (!node) return;
    avl_destroy_node(node->left, free_data);
    avl_destroy_node(node->right, free_data);
    if (free_data) {
        free_data(node->data);
    }
    free(node);
}

void avl_destroy(AvlTree *tree, void (*free_data)(void *)) {
    if (!tree) return;
    avl_destroy_node(tree->root, free_data);
    free(tree);
}

void avl_insert(AvlTree *tree, void *data) {
    if (!tree) return;
    tree->root = avl_insert_node(tree->root, data, tree->compare);
}

void avl_remove(AvlTree *tree, void *data, void (*free_data)(void *)) {
    if (!tree) return;
    tree->root = avl_remove_node(tree->root, data, tree->compare, free_data);
}

void *avl_search(AvlTree *tree, void *data) {
    if (!tree) return NULL;
    return avl_search_node(tree->root, data, tree->compare);
}
