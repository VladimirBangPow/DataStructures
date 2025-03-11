#include "red_black_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* --- Helper function prototypes --- */
static RBTNode* create_node(RBTREE* tree, void* data);
static void destroy_subtree(RBTREE* tree, RBTNode* node);
static void left_rotate(RBTREE* tree, RBTNode* x);
static void right_rotate(RBTREE* tree, RBTNode* y);
static void insert_fixup(RBTREE* tree, RBTNode* z);
static void delete_fixup(RBTREE* tree, RBTNode* x);
static RBTNode* tree_minimum(RBTREE* tree, RBTNode* node);
static void transplant(RBTREE* tree, RBTNode* u, RBTNode* v);
static void inorder_helper(const RBTREE* tree, const RBTNode* node, 
                           void (*print_func)(const void*));
static RBTNode* search_node(const RBTREE* tree, RBTNode* node, 
                            const void* data);

/* --- Public functions --- */

RBTREE* rb_create(int (*cmp)(const void*, const void*), void (*destroy)(void*)) {
    RBTREE* tree = (RBTREE*)malloc(sizeof(RBTREE));
    if (!tree) {
        return NULL;
    }

    /* Create the sentinel nil node */
    tree->nil = (RBTNode*)malloc(sizeof(RBTNode));
    if (!tree->nil) {
        free(tree);
        return NULL;
    }
    tree->nil->color = BLACK;
    tree->nil->left = tree->nil->right = tree->nil->parent = NULL;
    tree->nil->data = NULL; /* No data in sentinel */

    tree->root = tree->nil;
    tree->cmp = cmp;
    tree->destroy = destroy;
    tree->size = 0;

    return tree;
}

void rb_destroy(RBTREE* tree) {
    if (!tree) return;

    destroy_subtree(tree, tree->root);

    /* free sentinel */
    free(tree->nil);
    free(tree);
}

int rb_insert(RBTREE* tree, void* data) {
    if (!tree) return -1;

    RBTNode* y = tree->nil;
    RBTNode* x = tree->root;

    /* Standard BST insert to find position of new node */
    while (x != tree->nil) {
        y = x;
        int cmp_result = tree->cmp(data, x->data);
        if (cmp_result == 0) {
            /* Duplicate data not allowed (or handle duplicates differently) */
            return -1;
        } else if (cmp_result < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    /* Create new node z */
    RBTNode* z = create_node(tree, data);
    if (!z) {
        return -1;
    }
    z->parent = y;

    if (y == tree->nil) {
        /* Tree was empty */
        tree->root = z;
    } else {
        if (tree->cmp(z->data, y->data) < 0) {
            y->left = z;
        } else {
            y->right = z;
        }
    }

    /* Fix Red-Black properties */
    insert_fixup(tree, z);

    tree->size++;
    return 0;
}

void* rb_search(RBTREE* tree, const void* data) {
    if (!tree || !data) return NULL;

    RBTNode* node = search_node(tree, tree->root, data);
    return (node == tree->nil) ? NULL : node->data;
}

int rb_delete(RBTREE* tree, const void* data) {
    if (!tree || !data) return -1;

    RBTNode* z = search_node(tree, tree->root, data);
    if (z == tree->nil) {
        /* Not found */
        return -1;
    }

    RBTNode* y = z;
    RBColor y_original_color = y->color;
    RBTNode* x = NULL;

    if (z->left == tree->nil) {
        x = z->right;
        transplant(tree, z, z->right);
    } else if (z->right == tree->nil) {
        x = z->left;
        transplant(tree, z, z->left);
    } else {
        /* find minimum in z->right subtree */
        y = tree_minimum(tree, z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    /* If we had a destructor, call it on z->data */
    if (tree->destroy) {
        tree->destroy(z->data);
    }
    free(z);

    if (y_original_color == BLACK) {
        delete_fixup(tree, x);
    }

    tree->size--;
    return 0;
}

void rb_print_inorder(const RBTREE* tree, void (*print_func)(const void*)) {
    if (!tree || tree->root == tree->nil || !print_func) {
        return;
    }
    inorder_helper(tree, tree->root, print_func);
}

size_t rb_size(const RBTREE* tree) {
    if (!tree) return 0;
    return tree->size;
}

/* --- Internal / Helper function definitions --- */

static void destroy_subtree(RBTREE* tree, RBTNode* node) {
    if (node == tree->nil) return;
    destroy_subtree(tree, node->left);
    destroy_subtree(tree, node->right);

    if (tree->destroy) {
        tree->destroy(node->data);
    }
    free(node);
}

static RBTNode* create_node(RBTREE* tree, void* data) {
    RBTNode* node = (RBTNode*)malloc(sizeof(RBTNode));
    if (!node) return NULL;
    node->data = data;
    node->color = RED; /* New node is always red initially */
    node->left = tree->nil;
    node->right = tree->nil;
    node->parent = tree->nil;
    return node;
}

/* Left rotate around x */
static void left_rotate(RBTREE* tree, RBTNode* x) {
    RBTNode* y = x->right;
    x->right = y->left;
    if (y->left != tree->nil) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

/* Right rotate around y */
static void right_rotate(RBTREE* tree, RBTNode* y) {
    RBTNode* x = y->left;
    y->left = x->right;
    if (x->right != tree->nil) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == tree->nil) {
        tree->root = x;
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

/* Insert fixup to maintain RB properties */
static void insert_fixup(RBTREE* tree, RBTNode* z) {
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBTNode* y = z->parent->parent->right; /* uncle */
            if (y->color == RED) {
                /* Case 1: uncle is red (We color flip)*/
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                /* Uncle is black  (We Rotate)*/
                if (z == z->parent->right) {
                    /* Case 2: rotate left */
                    z = z->parent;
                    left_rotate(tree, z);
                }
                /* Case 3 */
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(tree, z->parent->parent);
            }
        } else {
            /* symmetric case */
            RBTNode* y = z->parent->parent->left; /* uncle */
            if (y->color == RED) {
				/* Uncle is red (We color flip)*/
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
				/* Uncle is black (We rotate)*/
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(tree, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

/* Replace subtree u with subtree v */
static void transplant(RBTREE* tree, RBTNode* u, RBTNode* v) {
    if (u->parent == tree->nil) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

static RBTNode* tree_minimum(RBTREE* tree, RBTNode* node) {
    while (node->left != tree->nil) {
        node = node->left;
    }
    return node;
}

/* Delete fixup to maintain RB properties */
static void delete_fixup(RBTREE* tree, RBTNode* x) {
    while (x != tree->root && x->color == BLACK) {
        if (x == x->parent->left) {
            RBTNode* w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate(tree, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(tree, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate(tree, x->parent);
                x = tree->root;
            }
        } else {
            RBTNode* w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate(tree, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(tree, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = BLACK;
}

static void inorder_helper(const RBTREE* tree, const RBTNode* node, 
                           void (*print_func)(const void*)) {
    if (node == tree->nil) return;
    inorder_helper(tree, node->left, print_func);
    print_func(node->data);
    inorder_helper(tree, node->right, print_func);
}

static RBTNode* search_node(const RBTREE* tree, RBTNode* node, 
                            const void* data) {
    if (node == tree->nil) {
        return tree->nil;
    }
    int cmp_result = tree->cmp(data, node->data);
    if (cmp_result == 0) {
        return node;
    } else if (cmp_result < 0) {
        return search_node(tree, node->left, data);
    } else {
        return search_node(tree, node->right, data);
    }
}
