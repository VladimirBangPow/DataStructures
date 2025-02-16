#include "bp_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* -------------------- Utility: a default int comparator -------------------- */
int bptree_int_cmp(const void* a, const void* b) {
    // a and b are expected to be (const int*) addresses
    int A = *(const int*)a;
    int B = *(const int*)b;
    return (A > B) - (A < B);  //  1 if A>B, 0 if A==B, -1 if A<B
}

/* ------------------------- Internal Helper Functions ----------------------- */

static BPTreeNode* create_node(int order, bool is_leaf);
static void        free_node(BPTreeNode* node);
static void        set_parent(BPTreeNode* child, BPTreeNode* parent);
static void        split_child(BPTree* tree, BPTreeNode* parent, int child_index);
static void        insert_non_full(BPTree* tree, BPTreeNode* node, void* key, void* value);
static BPTreeNode* find_leaf(BPTree* tree, BPTreeNode* node, void* key);
static void        print_recursive(BPTreeNode* node, int level);

static bool        delete_entry(BPTree* tree, BPTreeNode* node, void* key, void* pointer);
static void        remove_key_from_node(BPTreeNode* node, void* key, BPTree* tree);
static int         get_key_index(BPTreeNode* node, void* key, bptree_cmp cmp);
static int         get_child_index(BPTreeNode* parent, BPTreeNode* child);
static void        adjust_root(BPTree* tree);
static void        merge_nodes(BPTree* tree, BPTreeNode* left, BPTreeNode* right, int index_in_parent);
static void        redistribute_nodes(BPTreeNode* left, BPTreeNode* right, int index_in_parent, BPTree* tree);


/* ------------------------ Public Interface Functions ----------------------- */

/* Create a new B+ tree with a specified order and comparator. */
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

/* Destroy the entire B+ tree. */
void bptree_destroy(BPTree* tree) {
    if (!tree) return;

    // Recursively free all nodes
    if (tree->root) {
        // We can free the subtree from the root
        // A simple post-order traversal approach:
        // But let's do a function that does it recursively.
        // We'll reuse free_node (which recurses).
        free_node(tree->root);
    }
    free(tree);
}

/* Insert a (key, value) pair into the B+ tree. */
void bptree_insert(BPTree* tree, void* key, void* value) {
    BPTreeNode* root = tree->root;
    int order = tree->order;

    // If root is full, we must split it before descending
    if (root->num_keys == order - 1) {
        // Create a new root
        BPTreeNode* new_root = create_node(order, false);
        new_root->children[0] = root;
        set_parent(root, new_root);

        // Split the old root
        split_child(tree, new_root, 0);

        // Determine which child to descend into
        if (tree->cmp(key, new_root->keys[0]) > 0) {
            insert_non_full(tree, new_root->children[1], key, value);
        } else {
            insert_non_full(tree, new_root->children[0], key, value);
        }

        tree->root = new_root;
    }
    else {
        // Root not full, just insert
        insert_non_full(tree, root, key, value);
    }
}

/* Search for a key in the B+ tree. Returns the associated value pointer or NULL. */
void* bptree_search(BPTree* tree, void* key) {
    if (!tree || !tree->root) return NULL;
    BPTreeNode* leaf = find_leaf(tree, tree->root, key);
    if (!leaf) return NULL;

    // Perform a linear search in the leaf
    for (int i = 0; i < leaf->num_keys; i++) {
        if (tree->cmp(key, leaf->keys[i]) == 0) {
            return leaf->values[i];
        }
    }
    return NULL; // Not found
}

/* Delete the entry with the given key from the B+ tree. Returns true if found. */
bool bptree_delete(BPTree* tree, void* key) {
    if (!tree || !tree->root) return false;

    // 1. Find the leaf containing the key
    BPTreeNode* leaf = find_leaf(tree, tree->root, key);
    if (!leaf) return false;

    // 2. Check if the key actually exists in the leaf
    int index = get_key_index(leaf, key, tree->cmp);
    if (index == -1) {
        // key not found
        return false;
    }

    // 3. Remove the key/value from this leaf node
    remove_key_from_node(leaf, key, tree);
    
    // 4. If this removal causes the leaf to have too few keys, rebalance
    return delete_entry(tree, leaf, key, NULL);
}

/* Print the B+ Tree level-by-level. */
void bptree_print(BPTree* tree) {
    if (!tree || !tree->root) {
        printf("(empty B+ tree)\n");
        return;
    }
    printf("B+ Tree (order %d):\n", tree->order);
    print_recursive(tree->root, 0);
    printf("\n");
}

/* ------------------------- Node Creation/Deletion ------------------------- */

static BPTreeNode* create_node(int order, bool is_leaf) {
    BPTreeNode* node = (BPTreeNode*)calloc(1, sizeof(BPTreeNode));
    if (!node) {
        fprintf(stderr, "Error: Unable to allocate BPTreeNode.\n");
        return NULL;
    }
    node->is_leaf   = is_leaf;
    node->num_keys  = 0;
    node->keys      = (void**)calloc(order - 1, sizeof(void*));
    node->children  = (BPTreeNode**)calloc(order, sizeof(BPTreeNode*));
    node->parent    = NULL;
    node->next      = NULL;

    // If it's a leaf, allocate space for values as well
    if (is_leaf) {
        node->values = (void**)calloc(order - 1, sizeof(void*));
    }

    return node;
}

/* Frees all nodes in the subtree rooted at 'node' recursively. */
static void free_node(BPTreeNode* node) {
    if (!node) return;

    // If internal node, free children subtrees
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            free_node(node->children[i]);
        }
    }
    // Free arrays
    free(node->keys);
    if (node->values) {
        free(node->values);
    }
    free(node->children);

    // Finally, free the node itself
    free(node);
}

static void set_parent(BPTreeNode* child, BPTreeNode* parent) {
    if (child) {
        child->parent = parent;
    }
}

/* ---------------------- Insertion Helpers ---------------------- */

/**
 * Split a full child of 'parent' at index 'child_index'.
 * Standard B+ tree split: half keys go to a new node, and
 * one key is promoted to the parent (for an internal node).
 */
static void split_child(BPTree* tree, BPTreeNode* parent, int child_index) {
    BPTreeNode* full_child = parent->children[child_index];
    int order = tree->order;

    // Create a new node to hold the right half of 'full_child'
    BPTreeNode* new_node = create_node(order, full_child->is_leaf);
    set_parent(new_node, parent);

    int mid = (order - 1) / 2;

    // If it's a leaf node, move the upper half of the keys/values
    if (full_child->is_leaf) {
        // new_node gets the right half of full_child
        new_node->num_keys = full_child->num_keys - mid;
        for (int i = 0; i < new_node->num_keys; i++) {
            new_node->keys[i] = full_child->keys[mid + i];
            new_node->values[i] = full_child->values[mid + i];
        }

        full_child->num_keys = mid;

        // Link the sibling
        new_node->next = full_child->next;
        full_child->next = new_node;

        // Insert the new_node into the parent
        // The "promoted" key is new_node->keys[0]
        for (int i = parent->num_keys; i > child_index; i--) {
            parent->children[i + 1] = parent->children[i];
            parent->keys[i] = parent->keys[i - 1];
        }
        parent->children[child_index + 1] = new_node;
        parent->keys[child_index] = new_node->keys[0];
        parent->num_keys++;
    }
    else {
        // Internal node splitting
        // new_node gets the right half of the keys (minus the "middle" key)
        new_node->num_keys = full_child->num_keys - mid - 1;
        for (int i = 0; i < new_node->num_keys; i++) {
            new_node->keys[i] = full_child->keys[mid + 1 + i];
        }
        // Copy children
        for (int i = 0; i <= new_node->num_keys; i++) {
            new_node->children[i] = full_child->children[mid + 1 + i];
            set_parent(new_node->children[i], new_node);
        }

        // The middle key is promoted to the parent
        void* promoted_key = full_child->keys[mid];

        full_child->num_keys = mid;

        // Insert new_node into parent
        for (int i = parent->num_keys; i > child_index; i--) {
            parent->children[i + 1] = parent->children[i];
            parent->keys[i] = parent->keys[i - 1];
        }
        parent->children[child_index + 1] = new_node;
        parent->keys[child_index] = promoted_key;
        parent->num_keys++;
    }
}

static void insert_non_full(BPTree* tree, BPTreeNode* node, void* key, void* value) {
    int i = node->num_keys - 1;
    bptree_cmp cmp = tree->cmp;

    if (node->is_leaf) {
        // Move keys/values that are greater than 'key' to the right
        while (i >= 0 && cmp(key, node->keys[i]) < 0) {
            node->keys[i + 1]   = node->keys[i];
            node->values[i + 1] = node->values[i];
            i--;
        }
        // Insert the new key
        node->keys[i + 1]   = key;
        node->values[i + 1] = value;
        node->num_keys++;
    } else {
        // Find child to descend into
        while (i >= 0 && cmp(key, node->keys[i]) < 0) {
            i--;
        }
        i++;

        // If that child is full, split it first
        if (node->children[i]->num_keys == tree->order - 1) {
            split_child(tree, node, i);

            // After splitting, the key to go down might change
            if (cmp(key, node->keys[i]) > 0) {
                i++;
            }
        }
        insert_non_full(tree, node->children[i], key, value);
    }
}

/* Find the leaf node that would contain a given key. */
static BPTreeNode* find_leaf(BPTree* tree, BPTreeNode* node, void* key) {
    bptree_cmp cmp = tree->cmp;
    while (!node->is_leaf) {
        int i = 0;
        // find the child to follow
        while (i < node->num_keys && cmp(key, node->keys[i]) > 0) {
            i++;
        }
        node = node->children[i];
    }
    return node;
}

/* ---------------------- Printing (Debug) ---------------------- */

static void print_recursive(BPTreeNode* node, int level) {
    if (!node) return;

    // Print indentation
    for (int i = 0; i < level; i++) {
        printf("   ");
    }
    printf("|");

    // Print keys
    for (int i = 0; i < node->num_keys; i++) {
        printf(" *");
    }
    printf("*\n");

    // Another line with actual key values:
    for (int i = 0; i < level; i++) {
        printf("   ");
    }
    printf(" [");
    for (int i = 0; i < node->num_keys; i++) {
        // If you know they're integers, cast and print:
        // (For truly generic data, you'd need a user callback to print.)
        printf("%d", *(int*)node->keys[i]);
        if (i < node->num_keys - 1) printf(", ");
    }
    printf("] %s\n", node->is_leaf ? "(leaf)" : "(internal)");

    // Recurse on children if internal
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            print_recursive(node->children[i], level + 1);
        }
    }
}

/* ---------------------- Deletion Helpers ---------------------- */

/**
 * delete_entry:
 * After removing (key, pointer) from a node, fix any underflow by
 * rebalancing or merging nodes, and possibly adjust the root.
 */
static bool delete_entry(BPTree* tree, BPTreeNode* node, void* key, void* pointer) {
    // If node is root, special handling
    if (node == tree->root) {
        adjust_root(tree);
        return true;
    }

    // Check minimum occupancy
    int min_keys = (tree->order - 1) / 2; // a simple rule; real B+ trees may differ
    if (node->is_leaf) {
        // For a leaf, we can allow fewer keys than for internal nodes in some definitions
        // But let's keep it the same for simplicity
    }

    if (node->num_keys >= min_keys) {
        // Node has not underflowed, so no further adjustments needed
        return true;
    }

    // Node underflow: we must either borrow from a sibling or merge
    BPTreeNode* parent = node->parent;
    if (!parent) {
        // No parent => node is root => handled by adjust_root earlier
        return true;
    }

    // Find the index of 'node' in its parent's children array
    int index_in_parent = get_child_index(parent, node);

    // Identify left or right sibling
    BPTreeNode* left_sibling = (index_in_parent - 1 >= 0) 
                               ? parent->children[index_in_parent - 1] 
                               : NULL;
    BPTreeNode* right_sibling = (index_in_parent + 1 <= parent->num_keys)
                                ? parent->children[index_in_parent + 1]
                                : NULL;

    // Try to borrow from left sibling
    if (left_sibling && left_sibling->num_keys > min_keys) {
        redistribute_nodes(left_sibling, node, index_in_parent - 1, tree);
        return true;
    }
    // Try to borrow from right sibling
    if (right_sibling && right_sibling->num_keys > min_keys) {
        redistribute_nodes(node, right_sibling, index_in_parent, tree);
        return true;
    }

    // Otherwise, we must merge
    if (left_sibling) {
        merge_nodes(tree, left_sibling, node, index_in_parent - 1);
    } else {
        merge_nodes(tree, node, right_sibling, index_in_parent);
    }
    return true;
}

/* Remove key (and associated value/pointer) from the given node. */
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
        // Key not found
        return;
    }
    // Shift keys (and values if leaf) left
    for (int i = idx; i < node->num_keys - 1; i++) {
        node->keys[i] = node->keys[i + 1];
        if (node->is_leaf) {
            node->values[i] = node->values[i + 1];
        }
    }
    node->num_keys--;
    // Clear the last slot
    node->keys[node->num_keys] = NULL;
    if (node->is_leaf) {
        node->values[node->num_keys] = NULL;
    }
}

/* Return index of key in node->keys[] if present, otherwise -1. */
static int get_key_index(BPTreeNode* node, void* key, bptree_cmp cmp) {
    for (int i = 0; i < node->num_keys; i++) {
        if (cmp(key, node->keys[i]) == 0) return i;
    }
    return -1;
}

/* Return the index of 'child' in 'parent->children'. */
static int get_child_index(BPTreeNode* parent, BPTreeNode* child) {
    for (int i = 0; i <= parent->num_keys; i++) {
        if (parent->children[i] == child) return i;
    }
    return -1; // Not found (shouldn't happen if usage is correct)
}

/* If root has 0 keys (which can happen after a merge), make child the new root. */
static void adjust_root(BPTree* tree) {
    BPTreeNode* root = tree->root;
    if (root->num_keys > 0) {
        return;  // Nothing to do
    }
    // If it's not a leaf, promote its single child
    if (!root->is_leaf) {
        BPTreeNode* new_root = root->children[0];
        new_root->parent = NULL;
        tree->root = new_root;
        free(root->keys);
        free(root->values);
        free(root->children);
        free(root);
    }
    // If it's a leaf and no keys, then the tree is empty
    else {
        // This means the tree is now completely empty
        // We'll keep root as a leaf with zero keys
        // (Some definitions might free it and set tree->root=NULL.)
    }
}

/* Merge node 'right' into node 'left'. index_in_parent is the parent's key index
 * that separates left and right. The parent's key will be removed or adjusted. */
static void merge_nodes(BPTree* tree, BPTreeNode* left, BPTreeNode* right, int index_in_parent) {
    BPTreeNode* parent = left->parent;
    int start = left->num_keys;

    // If leaf, copy all (key,value) pairs from right
    if (left->is_leaf) {
        for (int i = 0; i < right->num_keys; i++) {
            left->keys[start + i]   = right->keys[i];
            left->values[start + i] = right->values[i];
        }
        left->num_keys += right->num_keys;
        // Link the leaf chain
        left->next = right->next;
    }
    else {
        // Internal node: "pull down" the parent's key at index_in_parent, then copy right's keys/children
        // Actually in a B+ tree, the key to move down is parent's key[index_in_parent], but let's keep it simple.
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

    // Remove the key from the parent
    for (int i = index_in_parent; i < parent->num_keys - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->keys[parent->num_keys - 1] = NULL;
    parent->children[parent->num_keys] = NULL;
    parent->num_keys--;

    // Free the right node
    free(right->keys);
    if (right->values) free(right->values);
    free(right->children);
    free(right);

    // Re-balance the parent if needed
    delete_entry(tree, parent, NULL, NULL);
}

/* Redistribute one key from 'right' to 'left' or vice versa. */
static void redistribute_nodes(BPTreeNode* left, BPTreeNode* right,
                               int index_in_parent, BPTree* tree)
{
    BPTreeNode* parent = left->parent;
    // If left is a leaf, we move one (key,value) from right to the end of left
    if (left->is_leaf) {
        if (left->num_keys < right->num_keys) {
            // Move the first (key,value) of 'right' to the end of 'left'
            left->keys[left->num_keys] = right->keys[0];
            left->values[left->num_keys] = right->values[0];
            left->num_keys++;

            // Shift 'right' keys left
            for (int i = 0; i < right->num_keys - 1; i++) {
                right->keys[i] = right->keys[i + 1];
                right->values[i] = right->values[i + 1];
            }
            right->num_keys--;
            // Update parent's key
            parent->keys[index_in_parent] = right->keys[0];
        }
        else {
            // Move the last (key,value) of 'left' to the front of 'right'
            for (int i = right->num_keys; i > 0; i--) {
                right->keys[i] = right->keys[i - 1];
                right->values[i] = right->values[i - 1];
            }
            right->keys[0] = left->keys[left->num_keys - 1];
            right->values[0] = left->values[left->num_keys - 1];
            right->num_keys++;

            // Update parent
            parent->keys[index_in_parent] = right->keys[0];
            left->num_keys--;
            left->keys[left->num_keys] = NULL;
            left->values[left->num_keys] = NULL;
        }
    }
    else {
        // Internal node redistribution
        // Borrow the leftmost key of 'right' or the rightmost key of 'left'
        if (left->num_keys < right->num_keys) {
            // Move parent's key[index_in_parent] down into left
            left->keys[left->num_keys] = parent->keys[index_in_parent];
            left->children[left->num_keys + 1] = right->children[0];
            if (right->children[0]) {
                right->children[0]->parent = left;
            }
            left->num_keys++;

            // The first key of 'right' goes up to the parent
            parent->keys[index_in_parent] = right->keys[0];

            // Shift 'right' keys left
            for (int i = 0; i < right->num_keys - 1; i++) {
                right->keys[i] = right->keys[i + 1];
                right->children[i] = right->children[i + 1];
            }
            right->children[right->num_keys - 1] = right->children[right->num_keys];
            
            right->num_keys--;
        }
        else {
            // Move parent's key[index_in_parent] down into right
            for (int i = right->num_keys; i > 0; i--) {
                right->keys[i] = right->keys[i - 1];
                right->children[i + 1] = right->children[i];
            }
            right->children[1] = right->children[0];

            // parent's key is appended to 'right'
            right->keys[0] = parent->keys[index_in_parent];
            right->children[0] = left->children[left->num_keys];
            if (right->children[0]) {
                right->children[0]->parent = right;
            }
            right->num_keys++;

            // The last key of 'left' moves up to the parent
            parent->keys[index_in_parent] = left->keys[left->num_keys - 1];
            left->keys[left->num_keys - 1] = NULL;
            left->children[left->num_keys] = NULL;
            left->num_keys--;
        }
    }
}
