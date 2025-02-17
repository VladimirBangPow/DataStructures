#include "bp_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define MAX_QUEUE 1024

/* --------------------------------------------------------------------------
 * A simple integer comparator for convenience
 * -------------------------------------------------------------------------- */
int bptree_int_cmp(const void* a, const void* b) {
    int A = *(const int*)a;
    int B = *(const int*)b;
    return (A > B) - (A < B);
}

/* --------------------------------------------------------------------------
 * Data structure creation & destruction
 * -------------------------------------------------------------------------- */
static BPTreeNode* create_node(int order, bool is_leaf);
static void        free_node(BPTreeNode* node);
static void        set_parent(BPTreeNode* child, BPTreeNode* parent);

static void        split_child(BPTree* tree, BPTreeNode* parent, int child_index);
static void        insert_non_full(BPTree* tree, BPTreeNode* node, void* key, void* value);
static BPTreeNode* find_leaf(BPTree* tree, BPTreeNode* node, void* key);

/* Deletion helpers (unchanged except for references to fix_overflow removed) */
static bool        delete_entry(BPTree* tree, BPTreeNode* node, void* key, void* pointer);
static void        remove_key_from_node(BPTreeNode* node, void* key, BPTree* tree);
static int         get_key_index(BPTreeNode* node, void* key, bptree_cmp cmp);
static int         get_child_index(BPTreeNode* parent, BPTreeNode* child);
static void        adjust_root(BPTree* tree);
static void        merge_nodes(BPTree* tree, BPTreeNode* left, BPTreeNode* right, int index_in_parent);
static void        redistribute_nodes(BPTreeNode* left, BPTreeNode* right, int index_in_parent, BPTree* tree);

/* --------------------------------------------------------------------------
 * BPTree: creation, destruction, and printing
 * -------------------------------------------------------------------------- */
BPTree* bptree_create(int order, bptree_cmp cmp) {
    if (order < 3) {
        fprintf(stderr, "B+ Tree order must be >= 3. Using fallback=3.\n");
        order = 3;
    }
    if (!cmp) {
        fprintf(stderr, "Comparator function cannot be NULL.\n");
        return NULL;
    }

    BPTree* tree = (BPTree*)malloc(sizeof(BPTree));
    if (!tree) {
        fprintf(stderr, "Error: unable to allocate BPTree.\n");
        return NULL;
    }
    tree->order = order;
    tree->cmp   = cmp;

    // Create an empty leaf node as the root
    BPTreeNode* root = create_node(order, true);
    if (!root) {
        free(tree);
        return NULL;
    }
    tree->root = root;

    return tree;
}

void bptree_destroy(BPTree* tree) {
    if (!tree) return;
    if (tree->root) {
        free_node(tree->root);
    }
    free(tree);
}

/**
 * bptree_print: a BFS-level print function (unchanged)
 */
void bptree_print(BPTree* tree) {
    if (!tree || !tree->root) {
        printf("(empty B+ tree)\n");
        return;
    }

    BPTreeNode* queue[MAX_QUEUE];
    int level_of[MAX_QUEUE];
    int front = 0, back = 0;

    queue[back] = tree->root;
    level_of[back] = 0;
    back++;

    int current_level = 0;
    bool first_in_level = true;

    printf("Level 0: ");
    while (front < back) {
        BPTreeNode* node = queue[front];
        int lvl = level_of[front];
        front++;

        if (lvl > current_level) {
            printf("\n");
            current_level = lvl;
            printf("Level %d: ", current_level);
            first_in_level = true;
        }

        if (!first_in_level) {
            printf(" ");
        }
        first_in_level = false;

        printf("[");
        for (int i = 0; i < node->num_keys; i++) {
            int val = *(int*)node->keys[i];
            printf("%d", val);
            if (i < node->num_keys - 1) {
                printf(", ");
            }
        }
        printf("]");

        if (!node->is_leaf) {
            for (int i = 0; i <= node->num_keys; i++) {
                if (node->children[i]) {
                    queue[back] = node->children[i];
                    level_of[back] = lvl + 1;
                    back++;
                }
            }
        }
    }
    printf("\n\n");
}

/* --------------------------------------------------------------------------
 * Insert: pure top-down approach
 * -------------------------------------------------------------------------- */

/**
 * bptree_insert:
 *  1) If root is full, split root first (creating a new root).
 *  2) insert_non_full into the appropriate child.
 */
void bptree_insert(BPTree* tree, void* key, void* value) {
    BPTreeNode* root = tree->root;
    int order = tree->order;

    // If root is full, split it before descending
    if (root->num_keys == order - 1) {
        // Create a new root
        BPTreeNode* new_root = create_node(order, false);
        new_root->children[0] = root;
        set_parent(root, new_root);

        // Split old root
        split_child(tree, new_root, 0);

        // Now decide which child to descend into
        if (tree->cmp(key, new_root->keys[0]) > 0) {
            insert_non_full(tree, new_root->children[1], key, value);
        } else {
            insert_non_full(tree, new_root->children[0], key, value);
        }
        tree->root = new_root;
    } else {
        // Otherwise root not full, just insert
        insert_non_full(tree, root, key, value);
    }
}

/**
 * insert_non_full:
 *  If 'node' is not full, we can safely insert a key into it (or descend).
 *  If we descend into a child that is full, we first split that child
 *  so it has room.
 */
static void insert_non_full(BPTree* tree, BPTreeNode* node, void* key, void* value) {
    bptree_cmp cmp = tree->cmp;
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        // Shift bigger keys right
        while (i >= 0 && cmp(key, node->keys[i]) < 0) {
            node->keys[i + 1]   = node->keys[i];
            node->values[i + 1] = node->values[i];
            i--;
        }
        // Insert here
        node->keys[i + 1]   = key;
        node->values[i + 1] = value;
        node->num_keys++;
    } else {
        // Find the child to go into
        while (i >= 0 && cmp(key, node->keys[i]) < 0) {
            i--;
        }
        i++;

        // If that child is full, split it first
        if (node->children[i]->num_keys == tree->order - 1) {
            split_child(tree, node, i);

            // After splitting, we may need to adjust i
            if (cmp(key, node->keys[i]) > 0) {
                i++;
            }
        }
        insert_non_full(tree, node->children[i], key, value);
    }
}

/**
 * split_child:
 *  Splits node->children[child_index], which is full, into two nodes,
 *  promoting the middle key up to 'node'.
 *  Because we only call this if child is full (and 'node' is not full),
 *  there's guaranteed space in 'node->keys[]' to shift/promote.
 */
static void split_child(BPTree* tree, BPTreeNode* parent, int child_index) {
    BPTreeNode* full_child = parent->children[child_index];
    int order = tree->order;

    // Create new (right) node
    BPTreeNode* new_node = create_node(order, full_child->is_leaf);
    new_node->parent = parent;

    int total = full_child->num_keys;  // # of keys in the full child
    int mid = (total) / 2;         // ~ half

    if (full_child->is_leaf) {
        // Leaf: new_node gets the top half of keys
        new_node->num_keys = full_child->num_keys - mid;

        for (int i = 0; i < new_node->num_keys; i++) {
            new_node->keys[i]   = full_child->keys[mid + i];
            new_node->values[i] = full_child->values[mid + i];
        }
        full_child->num_keys = mid;

        // Link siblings
        new_node->next = full_child->next;
        full_child->next = new_node;

        // Move parent's children to make space
        for (int i = parent->num_keys; i > child_index; i--) {
            parent->keys[i]         = parent->keys[i - 1];
            parent->children[i + 1] = parent->children[i];
            if (parent->children[i + 1]) {
                parent->children[i + 1]->parent = parent;
            }
        }
        // Promote new_node->keys[0] into the parent
        parent->keys[child_index]         = new_node->keys[0];
        parent->children[child_index + 1] = new_node;
        parent->num_keys++;
    }
    else {
        // Internal node
        new_node->num_keys = full_child->num_keys - mid - 1;

        // The key at 'mid' is "promoted" to the parent
        void* promote_key = full_child->keys[mid];

        // Right half of keys go to new_node
        for (int i = 0; i < new_node->num_keys; i++) {
            new_node->keys[i] = full_child->keys[mid + 1 + i];
        }
        // Move children
        for (int i = 0; i <= new_node->num_keys; i++) {
            new_node->children[i] = full_child->children[mid + 1 + i];
            if (new_node->children[i]) {
                new_node->children[i]->parent = new_node;
            }
        }
        full_child->num_keys = mid;

        // Shift parent's keys[] and children[] right
        for (int i = parent->num_keys; i > child_index; i--) {
            parent->keys[i]         = parent->keys[i - 1];
            parent->children[i + 1] = parent->children[i];
            if (parent->children[i + 1]) {
                parent->children[i + 1]->parent = parent;
            }
        }
        parent->keys[child_index]         = promote_key;
        parent->children[child_index + 1] = new_node;
        parent->num_keys++;
    }
}

/* --------------------------------------------------------------------------
 * Searching
 * -------------------------------------------------------------------------- */
void* bptree_search(BPTree* tree, void* key) {
    if (!tree || !tree->root) return NULL;
    BPTreeNode* leaf = find_leaf(tree, tree->root, key);
    if (!leaf) return NULL;

    bptree_cmp cmp = tree->cmp;
    for (int i = 0; i < leaf->num_keys; i++) {
        if (cmp(key, leaf->keys[i]) == 0) {
            return leaf->values[i];
        }
    }
    return NULL;
}

static BPTreeNode* find_leaf(BPTree* tree, BPTreeNode* node, void* key) {
    bptree_cmp cmp = tree->cmp;
    while (!node->is_leaf) {
        int i = 0;
        while (i < node->num_keys && cmp(key, node->keys[i]) > 0) {
            i++;
        }
        node = node->children[i];
    }
    return node;
}

/* --------------------------------------------------------------------------
 * Deletion logic remains similar (unchanged).
 * -------------------------------------------------------------------------- */
bool bptree_delete(BPTree* tree, void* key) {
    if (!tree || !tree->root) return false;

    BPTreeNode* leaf = find_leaf(tree, tree->root, key);
    if (!leaf) return false;

    int index = get_key_index(leaf, key, tree->cmp);
    if (index == -1) {
        return false;
    }

    remove_key_from_node(leaf, key, tree);
    return delete_entry(tree, leaf, key, NULL);
}

static bool delete_entry(BPTree* tree, BPTreeNode* node, void* key, void* pointer) {
    if (node == tree->root) {
        adjust_root(tree);
        return true;
    }

    int min_keys = (tree->order - 1) / 2;
    if (node->num_keys >= min_keys) {
        return true;
    }

    BPTreeNode* parent = node->parent;
    int index_in_parent = get_child_index(parent, node);

    BPTreeNode* left_sibling = (index_in_parent - 1 >= 0) 
                               ? parent->children[index_in_parent - 1] 
                               : NULL;
    BPTreeNode* right_sibling = (index_in_parent + 1 <= parent->num_keys)
                                ? parent->children[index_in_parent + 1]
                                : NULL;

    if (left_sibling && left_sibling->num_keys > min_keys) {
        redistribute_nodes(left_sibling, node, index_in_parent - 1, tree);
        return true;
    }
    if (right_sibling && right_sibling->num_keys > min_keys) {
        redistribute_nodes(node, right_sibling, index_in_parent, tree);
        return true;
    }

    if (left_sibling) {
        merge_nodes(tree, left_sibling, node, index_in_parent - 1);
    } else {
        merge_nodes(tree, node, right_sibling, index_in_parent);
    }
    return true;
}

static void remove_key_from_node(BPTreeNode* node, void* key, BPTree* tree) {
    bptree_cmp cmp = tree->cmp;
    int idx = -1;
    for (int i = 0; i < node->num_keys; i++) {
        if (cmp(key, node->keys[i]) == 0) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        return;
    }
    for (int i = idx; i < node->num_keys - 1; i++) {
        node->keys[i] = node->keys[i + 1];
        if (node->is_leaf) {
            node->values[i] = node->values[i + 1];
        }
    }
    node->num_keys--;
    node->keys[node->num_keys] = NULL;
    if (node->is_leaf) {
        node->values[node->num_keys] = NULL;
    }
}

static int get_key_index(BPTreeNode* node, void* key, bptree_cmp cmp) {
    for (int i = 0; i < node->num_keys; i++) {
        if (cmp(key, node->keys[i]) == 0) {
            return i;
        }
    }
    return -1;
}

static int get_child_index(BPTreeNode* parent, BPTreeNode* child) {
    for (int i = 0; i <= parent->num_keys; i++) {
        if (parent->children[i] == child) {
            return i;
        }
    }
    return -1;
}

static void adjust_root(BPTree* tree) {
    BPTreeNode* root = tree->root;
    if (root->num_keys > 0) {
        return;
    }
    if (!root->is_leaf) {
        BPTreeNode* new_root = root->children[0];
        new_root->parent = NULL;
        tree->root = new_root;
        free(root->keys);
        free(root->values);
        free(root->children);
        free(root);
    } else {
        // The tree is now empty
        // keep root as empty leaf
    }
}

static void merge_nodes(BPTree* tree, BPTreeNode* left, BPTreeNode* right, int index_in_parent) {
    BPTreeNode* parent = left->parent;
    int start = left->num_keys;

    if (left->is_leaf) {
        for (int i = 0; i < right->num_keys; i++) {
            left->keys[start + i]   = right->keys[i];
            left->values[start + i] = right->values[i];
        }
        left->num_keys += right->num_keys;
        left->next = right->next;
    }
    else {
        void* parent_key = parent->keys[index_in_parent];
        left->keys[start] = parent_key;
        left->num_keys++;

        for (int i = 0; i < right->num_keys; i++) {
            left->keys[left->num_keys + i] = right->keys[i];
        }
        for (int i = 0; i <= right->num_keys; i++) {
            left->children[left->num_keys + i] = right->children[i];
            if (right->children[i]) {
                right->children[i]->parent = left;
            }
        }
        left->num_keys += right->num_keys;
    }

    for (int i = index_in_parent; i < parent->num_keys - 1; i++) {
        parent->keys[i]         = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->keys[parent->num_keys - 1] = NULL;
    parent->children[parent->num_keys] = NULL;
    parent->num_keys--;

    free(right->keys);
    if (right->values) free(right->values);
    free(right->children);
    free(right);

    delete_entry(tree, parent, NULL, NULL);
}

static void redistribute_nodes(BPTreeNode* left, BPTreeNode* right,
                               int index_in_parent, BPTree* tree)
{
    BPTreeNode* parent = left->parent;
    int order = tree->order;

    if (left->is_leaf) {
        // ...
        // unchanged from your code
        if (left->num_keys < right->num_keys) {
            left->keys[left->num_keys] = right->keys[0];
            left->values[left->num_keys] = right->values[0];
            left->num_keys++;
            for (int i = 0; i < right->num_keys - 1; i++) {
                right->keys[i] = right->keys[i + 1];
                right->values[i] = right->values[i + 1];
            }
            right->num_keys--;
            parent->keys[index_in_parent] = right->keys[0];
        } else {
            for (int i = right->num_keys; i > 0; i--) {
                right->keys[i]   = right->keys[i - 1];
                right->values[i] = right->values[i - 1];
            }
            right->keys[0]   = left->keys[left->num_keys - 1];
            right->values[0] = left->values[left->num_keys - 1];
            right->num_keys++;
            parent->keys[index_in_parent] = right->keys[0];
            left->num_keys--;
            left->keys[left->num_keys] = NULL;
            left->values[left->num_keys] = NULL;
        }
    }
    else {
        // ...
        // unchanged from your code
        if (left->num_keys < right->num_keys) {
            left->keys[left->num_keys] = parent->keys[index_in_parent];
            left->children[left->num_keys + 1] = right->children[0];
            if (right->children[0]) {
                right->children[0]->parent = left;
            }
            left->num_keys++;
            parent->keys[index_in_parent] = right->keys[0];
            for (int i = 0; i < right->num_keys - 1; i++) {
                right->keys[i]     = right->keys[i + 1];
                right->children[i] = right->children[i + 1];
            }
            right->children[right->num_keys - 1] = right->children[right->num_keys];
            right->num_keys--;
        } else {
            for (int i = right->num_keys; i > 0; i--) {
                right->keys[i]     = right->keys[i - 1];
                right->children[i+1] = right->children[i];
            }
            right->children[1] = right->children[0];
            right->keys[0]     = parent->keys[index_in_parent];
            right->children[0] = left->children[left->num_keys];
            if (right->children[0]) {
                right->children[0]->parent = right;
            }
            right->num_keys++;
            parent->keys[index_in_parent] = left->keys[left->num_keys - 1];
            left->keys[left->num_keys - 1] = NULL;
            left->children[left->num_keys] = NULL;
            left->num_keys--;
        }
    }
}

/* --------------------------------------------------------------------------
 * Implementation details for node creation, freeing, parent assignment
 * -------------------------------------------------------------------------- */
static BPTreeNode* create_node(int order, bool is_leaf) {
    BPTreeNode* node = (BPTreeNode*)calloc(1, sizeof(BPTreeNode));
    if (!node) {
        fprintf(stderr, "Error: Unable to allocate BPTreeNode.\n");
        return NULL;
    }
    node->is_leaf   = is_leaf;
    node->num_keys  = 0;
    node->keys      = (void**)calloc(order - 1, sizeof(void*));
    node->children  = (BPTreeNode**)calloc(order,   sizeof(BPTreeNode*));
    node->parent    = NULL;
    node->next      = NULL;

    if (is_leaf) {
        node->values = (void**)calloc(order - 1, sizeof(void*));
    }
    return node;
}

static void free_node(BPTreeNode* node) {
    if (!node) return;
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            free_node(node->children[i]);
        }
    }
    free(node->keys);
    if (node->values) {
        free(node->values);
    }
    free(node->children);
    free(node);
}

static void set_parent(BPTreeNode* child, BPTreeNode* parent) {
    if (child) {
        child->parent = parent;
    }
}
