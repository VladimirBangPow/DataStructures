/******************************************************************************
 * bptree.c
 *
 * B+ Tree Implementation with both Insertion & Deletion.
 * 
 * Key Fixes:
 *  - free_node(...) now frees keys, children, values
 *  - After merges, set parent->keys[] / parent->children[] out-of-range slots to NULL
 *  - Ensures empty internal/leaf nodes are fully removed.
 ******************************************************************************/

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <string.h>
 #include "bp_tree.h"
 
 /* For simplicity, we define a small ORDER = 4. */
 #define ORDER 4
 #define MIN_CHILDREN ((ORDER + 1) / 2)  // 2 for ORDER=4
 #define MAX_KEYS (ORDER - 1)           // 3 if ORDER=4
 #define MIN_KEYS (MIN_CHILDREN - 1)    // 1 if ORDER=4
 
 /* --------------------------------------------------------------------------
  * A simple integer comparator for convenience
  * -------------------------------------------------------------------------- */
 int bptree_int_cmp(const void* a, const void* b) {
     int x = *(const int*)a;
     int y = *(const int*)b;
     if (x < y) return -1;
     if (x > y) return  1;
     return 0;
 }
 
 /* --------------------------------------------------------------------------
  * BPTreeNode creation
  * -------------------------------------------------------------------------- */
 static BPTreeNode* bptree_node_create(int order, bool is_leaf) {
     BPTreeNode* node = (BPTreeNode*)calloc(1, sizeof(BPTreeNode));
     if (!node) {
         fprintf(stderr, "Error: node allocation failed.\n");
         return NULL;
     }
     node->is_leaf  = is_leaf;
     node->num_keys = 0;
     node->keys     = (void**)calloc(order - 1, sizeof(void*));
     node->children = (BPTreeNode**)calloc(order,   sizeof(BPTreeNode*));
     node->values   = NULL;
 
     if (is_leaf) {
         node->values = (void**)calloc(order - 1, sizeof(void*));
     }
 
     node->parent = NULL;
     node->next   = NULL;
     return node;
 }
 
 /* --------------------------------------------------------------------------
  * bptree_create
  * -------------------------------------------------------------------------- */
 BPTree* bptree_create(int order, bptree_cmp cmp) {
     if (order < 3) {
         fprintf(stderr, "B+ tree order must be >= 3.\n");
         return NULL;
     }
     if (!cmp) {
         fprintf(stderr, "Comparator function cannot be NULL.\n");
         return NULL;
     }
     BPTree* tree = (BPTree*)calloc(1, sizeof(BPTree));
     tree->order = order;
     tree->root  = NULL;
     tree->cmp   = cmp;
     return tree;
 }
 
 /* --------------------------------------------------------------------------
  * Recursive free of all nodes in a subtree
  * -------------------------------------------------------------------------- */
 static void bptree_node_free_subtree(BPTreeNode* node, int order) {
     if (!node) return;
     if (!node->is_leaf) {
         for (int i = 0; i <= node->num_keys; i++) {
             bptree_node_free_subtree(node->children[i], order);
         }
     }
     /* Properly free everything in node. */
     free(node->keys);
     free(node->children);
     if (node->is_leaf) {
         free(node->values);
     }
     free(node);
 }
 
 /* bptree_destroy */
 void bptree_destroy(BPTree* tree) {
     if (!tree) return;
     if (tree->root) {
         bptree_node_free_subtree(tree->root, tree->order);
     }
     free(tree);
 }
 
 /* --------------------------------------------------------------------------
  * bptree_search
  * -------------------------------------------------------------------------- */
 static BPTreeNode* bptree_find_leaf(BPTree* tree, void* key);
 static void* bptree_search_in_leaf(BPTreeNode* leaf, bptree_cmp cmp, void* key);
 
 void* bptree_search(BPTree* tree, void* key) {
     if (!tree->root) return NULL;
     BPTreeNode* leaf = bptree_find_leaf(tree, key);
     if (!leaf) return NULL;
     return bptree_search_in_leaf(leaf, tree->cmp, key);
 }
 
 static BPTreeNode* bptree_find_leaf(BPTree* tree, void* key) {
     BPTreeNode* current = tree->root;
     while (current && !current->is_leaf) {
         int i = 0;
         while (i < current->num_keys && tree->cmp(key, current->keys[i]) >= 0) {
             i++;
         }
         current = current->children[i];
     }
     return current;
 }
 
 static void* bptree_search_in_leaf(BPTreeNode* leaf, bptree_cmp cmp, void* key) {
     for (int i=0; i<leaf->num_keys; i++) {
         if (cmp(key, leaf->keys[i])==0) {
             return leaf->values[i];
         }
     }
     return NULL;
 }
 
 /* --------------------------------------------------------------------------
  * bptree_insert: (Scenarios 1–6)
  * -------------------------------------------------------------------------- */
 static void bptree_insert_leaf_no_split(BPTreeNode* leaf, bptree_cmp cmp, void* key, void* value);
 static void bptree_insert_leaf_split(BPTree* tree, BPTreeNode* leaf, void* key, void* value);
 static void bptree_insert_into_parent(BPTree* tree, BPTreeNode* left, void* up_key, BPTreeNode* right);
 static void insert_into_node(BPTreeNode* node, int index, void* key, BPTreeNode* right);
 
 void bptree_insert(BPTree* tree, void* key, void* value) {
     if (!tree->root) {
         /* SCENARIO 1: Empty tree => new leaf root */
         BPTreeNode* leaf = bptree_node_create(tree->order, true);
         leaf->keys[0]   = key;
         leaf->values[0] = value;
         leaf->num_keys  = 1;
         tree->root      = leaf;
         return;
     }
 
     /* find leaf for 'key' */
     BPTreeNode* leaf = bptree_find_leaf(tree, key);
 
     if (leaf->num_keys < (tree->order - 1)) {
         /* SCENARIO 2: Leaf has space */
         bptree_insert_leaf_no_split(leaf, tree->cmp, key, value);
     } else {
         /* SCENARIO 3: Leaf is full => split */
         bptree_insert_leaf_split(tree, leaf, key, value);
     }
 }
 
 static void bptree_insert_leaf_no_split(BPTreeNode* leaf, bptree_cmp cmp, void* key, void* value) {
     int i = leaf->num_keys -1;
     while (i >= 0 && cmp(key, leaf->keys[i]) < 0) {
         leaf->keys[i+1]   = leaf->keys[i];
         leaf->values[i+1] = leaf->values[i];
         i--;
     }
     i++;
     leaf->keys[i]   = key;
     leaf->values[i] = value;
     leaf->num_keys++;
 }
 
 /* Scenario 3: Leaf full => split */
 static void bptree_insert_leaf_split(BPTree* tree, BPTreeNode* leaf, void* key, void* value) {
     int order = tree->order;
     void** temp_keys   = (void**)calloc(order, sizeof(void*));
     void** temp_values = (void**)calloc(order, sizeof(void*));
 
     /* figure out insertion index */
     int insert_idx = 0;
     while (insert_idx < leaf->num_keys && tree->cmp(key, leaf->keys[insert_idx]) > 0) {
         insert_idx++;
     }
 
     /* merge existing keys + new key into temp arrays */
     int i, j;
     for (i=0, j=0; i<leaf->num_keys; i++, j++) {
         if (j == insert_idx) j++;
         temp_keys[j]   = leaf->keys[i];
         temp_values[j] = leaf->values[i];
     }
     temp_keys[insert_idx]   = key;
     temp_values[insert_idx] = value;
 
     int split = (order-1)/2;  /* for ORDER=4, split=1 */
 
     /* Refill the original leaf (left) */
     leaf->num_keys = 0;
     for (i=0; i<split; i++) {
         leaf->keys[i]   = temp_keys[i];
         leaf->values[i] = temp_values[i];
         leaf->num_keys++;
     }
 
     /* New right leaf */
     BPTreeNode* new_leaf = bptree_node_create(order, true);
     new_leaf->parent = leaf->parent;
     int k=0;
     for (; i<order; i++, k++) {
         new_leaf->keys[k]   = temp_keys[i];
         new_leaf->values[k] = temp_values[i];
         new_leaf->num_keys++;
     }
     new_leaf->next = leaf->next;
     leaf->next     = new_leaf;
 
     /* up_key = first key of new_leaf */
     void* up_key = new_leaf->keys[0];
 
     free(temp_keys);
     free(temp_values);
 
     /* Insert up_key into parent => scenario 4/5/6 */
     bptree_insert_into_parent(tree, leaf, up_key, new_leaf);
 }
 
 static void bptree_insert_into_parent(BPTree* tree, BPTreeNode* left, void* up_key, BPTreeNode* right) {
     BPTreeNode* parent = left->parent;
     if (!parent) {
         /* SCENARIO 6: new root */
         BPTreeNode* new_root = bptree_node_create(tree->order, false);
         new_root->keys[0]     = up_key;
         new_root->children[0] = left;
         new_root->children[1] = right;
         new_root->num_keys    = 1;
         left->parent  = new_root;
         right->parent = new_root;
         tree->root    = new_root;
         return;
     }
 
     /* find position of 'left' in parent->children */
     int idx = 0;
     while (idx <= parent->num_keys && parent->children[idx] != left) {
         idx++;
     }
 
     if (parent->num_keys < (tree->order -1)) {
         /* SCENARIO 4: parent has space */
         insert_into_node(parent, idx, up_key, right);
         right->parent = parent;
     } else {
         /* SCENARIO 5: parent is full => split internal node */
         int order = tree->order;
         void** temp_keys = (void**)calloc(order, sizeof(void*));
         BPTreeNode** temp_ch = (BPTreeNode**)calloc(order+1, sizeof(BPTreeNode*));
 
         int i, j=0;
         /* copy children with gap for new child */
         for (i=0; i<=parent->num_keys; i++, j++) {
             if (j == idx+1) j++;
             temp_ch[j] = parent->children[i];
         }
 
         /* copy keys with gap for new up_key */
         j=0;
         for (i=0; i<parent->num_keys; i++, j++) {
             if (j == idx) j++;
             temp_keys[j] = parent->keys[i];
         }
         temp_keys[idx]   = up_key;
         temp_ch[idx+1]   = right;
 
         /* split point => mid = (order-1)/2 for internal node */
         int mid = (order -1)/2;
 
         parent->num_keys = 0;
         for (i=0; i<mid; i++) {
             parent->children[i] = temp_ch[i];
             parent->keys[i]     = temp_keys[i];
             parent->num_keys++;
         }
         parent->children[i] = temp_ch[i];
 
         void* promoted_key = temp_keys[mid];
 
         /* new sibling node for parent */
         BPTreeNode* new_node = bptree_node_create(order, false);
         new_node->parent = parent;
 
         int k=0;
         for (i = mid+1; i < order; i++, k++) {
             new_node->children[k] = temp_ch[i];
             new_node->keys[k]     = temp_keys[i];
             if (new_node->children[k]) {
                 new_node->children[k]->parent = new_node;
             }
         }
         new_node->children[k] = temp_ch[i];
         if (new_node->children[k]) {
             new_node->children[k]->parent = new_node;
         }
         new_node->num_keys = k;
 
         /* fix parent's child->parent pointers */
         for (int c=0; c <= parent->num_keys; c++) {
             if (parent->children[c]) {
                 parent->children[c]->parent = parent;
             }
         }
 
         free(temp_keys);
         free(temp_ch);
 
         /* recursively insert promoted_key up */
         bptree_insert_into_parent(tree, parent, promoted_key, new_node);
     }
 }
 
 static void insert_into_node(BPTreeNode* node, int index, void* key, BPTreeNode* right) {
     for (int i=node->num_keys; i>index; i--) {
         node->keys[i]       = node->keys[i-1];
         node->children[i+1] = node->children[i];
     }
     node->keys[index]       = key;
     node->children[index+1] = right;
     node->num_keys++;
 }
 
 /* ------------------------- DELETE LOGIC BELOW -------------------------- */
 static BPTreeNode* find_leaf(BPTree* tree, void* key);
 static bool key_exists_in_leaf(BPTreeNode* leaf, void* key, int (*cmp)(const void*, const void*));
 static void remove_key_from_leaf(BPTreeNode* leaf, void* key, int (*cmp)(const void*, const void*));
 
 static void handle_underflow(BPTree* tree, BPTreeNode* node);
 static bool try_redistribute(BPTree* tree, BPTreeNode* node);
 static void borrow_from_left_leaf(BPTreeNode* node, BPTreeNode* left, int parentIndex, int (*cmp)(const void*, const void*));
 static void borrow_from_right_leaf(BPTreeNode* node, BPTreeNode* right, int parentIndex, int (*cmp)(const void*, const void*));
 static void borrow_from_left_internal(BPTreeNode* node, BPTreeNode* left, int parentIndex);
 static void borrow_from_right_internal(BPTreeNode* node, BPTreeNode* right, int parentIndex);
 
 static void merge_nodes(BPTree* tree, BPTreeNode* left, BPTreeNode* right, int sepIndex);
 static void merge_leaves(BPTree* tree, BPTreeNode* leftLeaf, BPTreeNode* rightLeaf, int sepIndex);
 static void merge_internals(BPTree* tree, BPTreeNode* leftNode, BPTreeNode* rightNode, int sepIndex);
 
 static int find_child_index(BPTreeNode* parent, BPTreeNode* child);
 static int find_key_index_in_node(BPTreeNode* node, void* key, int (*cmp)(const void*, const void*));
 static bool underflows(BPTree* tree, BPTreeNode* node);
 
 /* -- Corrected free_node: free arrays THEN free(node). -- */
 static void free_node(BPTreeNode* node) {
     if (!node) return;
     if (node->keys)     free(node->keys);
     if (node->children) free(node->children);
     if (node->values)   free(node->values);
     free(node);
 }
 
 /*
  * bptree_delete: returns true if 'key' found & removed, else false
  */
 bool bptree_delete(BPTree* tree, void* key) {
     if (!tree || !tree->root) {
         return false;
     }
     /* 1. Find leaf node where 'key' would reside. */
     BPTreeNode* leaf = find_leaf(tree, key);
     if (!leaf) {
         return false;
     }
     /* 2. Check if key actually exists in leaf. */
     if (!key_exists_in_leaf(leaf, key, tree->cmp)) {
         return false;
     }
     /* 3. Remove key from leaf. */
     remove_key_from_leaf(leaf, key, tree->cmp);
 
     /* 4. Check underflow. */
     if (underflows(tree, leaf)) {
         handle_underflow(tree, leaf);
     }
 
     /* 5. Special root case. */
     BPTreeNode* root = tree->root;
     if (!root->is_leaf && root->num_keys == 0) {
         BPTreeNode* newRoot = root->children[0];
         if (newRoot) {
             newRoot->parent = NULL;
         }
         tree->root = newRoot;
         free_node(root);
     }
     return true;
 }
 
 /* -- find_leaf for deletion -- */
 static BPTreeNode* find_leaf(BPTree* tree, void* key) {
     if (!tree || !tree->root) {
         return NULL;
     }
     BPTreeNode* current = tree->root;
     while (!current->is_leaf) {
         int i = 0;
         while (i < current->num_keys && tree->cmp(key, current->keys[i]) >= 0) {
             i++;
         }
         current = current->children[i];
     }
     return current;
 }
 
 static bool key_exists_in_leaf(BPTreeNode* leaf, void* key, int (*cmp)(const void*, const void*)) {
     for (int i = 0; i < leaf->num_keys; i++) {
         if (cmp(key, leaf->keys[i]) == 0) {
             return true;
         }
     }
     return false;
 }
 
 static void remove_key_from_leaf(BPTreeNode* leaf, void* key, int (*cmp)(const void*, const void*)) {
     int pos = -1;
     for (int i = 0; i < leaf->num_keys; i++) {
         if (cmp(key, leaf->keys[i]) == 0) {
             pos = i;
             break;
         }
     }
     if (pos == -1) return; // shouldn't happen
     /* Shift left to fill gap */
     for (int i = pos; i < leaf->num_keys - 1; i++) {
         leaf->keys[i]   = leaf->keys[i + 1];
         leaf->values[i] = leaf->values[i + 1];
     }
     leaf->num_keys--;
 }
 
 /* -------------------------- Handle Underflow ------------------------------ */
 static void handle_underflow(BPTree* tree, BPTreeNode* node) {
     if (!node->parent) {
         // If node is root & alone, allow underflow
         return;
     }
     // Try to borrow
     if (try_redistribute(tree, node)) {
         return;
     }
     // Else merge
     int parentIndex = find_child_index(node->parent, node);
     BPTreeNode* leftSibling  = (parentIndex > 0) ? node->parent->children[parentIndex - 1] : NULL;
     BPTreeNode* rightSibling = (parentIndex < node->parent->num_keys)
                                ? node->parent->children[parentIndex + 1]
                                : NULL;
 
     if (leftSibling) {
         merge_nodes(tree, leftSibling, node, parentIndex - 1);
     } else if (rightSibling) {
         merge_nodes(tree, node, rightSibling, parentIndex);
     }
 
     // After merging, check parent underflow
     if (node->parent && underflows(tree, node->parent)) {
         handle_underflow(tree, node->parent);
     }
 }
 
 /* Attempt to borrow from left/right sibling */
 static bool try_redistribute(BPTree* tree, BPTreeNode* node) {
     int min_keys = ((tree->order + 1) / 2) - 1; // for order=4 => 1
     BPTreeNode* parent = node->parent;
     int parentIndex = find_child_index(parent, node);
 
     BPTreeNode* leftSibling  = (parentIndex > 0) ? parent->children[parentIndex - 1] : NULL;
     BPTreeNode* rightSibling = (parentIndex < parent->num_keys)
                                ? parent->children[parentIndex + 1]
                                : NULL;
 
     // Borrow from left
     if (leftSibling && leftSibling->num_keys > min_keys) {
         if (node->is_leaf) {
             borrow_from_left_leaf(node, leftSibling, parentIndex, tree->cmp);
         } else {
             borrow_from_left_internal(node, leftSibling, parentIndex);
         }
         return true;
     }
     // Borrow from right
     if (rightSibling && rightSibling->num_keys > min_keys) {
         if (node->is_leaf) {
             borrow_from_right_leaf(node, rightSibling, parentIndex + 1, tree->cmp);
         } else {
             borrow_from_right_internal(node, rightSibling, parentIndex + 1);
         }
         return true;
     }
     return false;
 }
 
 /* ------------------------- Borrowing Functions -------------------------- */
 static void borrow_from_left_leaf(
     BPTreeNode* node, BPTreeNode* left, int parentIndex,
     int (*cmp)(const void*, const void*)
 ) {
     printf("Borrowing from left leaf\n");
     // Move last key of left -> front of node
     int last = left->num_keys - 1;
     void* borrowedKey = left->keys[last];
     void* borrowedVal = left->values[last];
 
     // Shift node's keys right
     for (int i = node->num_keys; i > 0; i--) {
         node->keys[i]   = node->keys[i - 1];
         node->values[i] = node->values[i - 1];
     }
     node->keys[0]   = borrowedKey;
     node->values[0] = borrowedVal;
 
     left->num_keys--;
     node->num_keys++;
 
     // Update parent's separator
     node->parent->keys[parentIndex - 1] = node->keys[0];
 }
 
 static void borrow_from_right_leaf(
     BPTreeNode* node, BPTreeNode* right, int parentIndex,
     int (*cmp)(const void*, const void*)
 ) {
     printf("Borrowing from right leaf\n");
     // Move first key of right -> end of node
     void* borrowedKey = right->keys[0];
     void* borrowedVal = right->values[0];
 
     node->keys[node->num_keys]   = borrowedKey;
     node->values[node->num_keys] = borrowedVal;
     node->num_keys++;
 
     // Shift right->keys left
     for (int i=0; i< right->num_keys -1; i++) {
         right->keys[i]   = right->keys[i+1];
         right->values[i] = right->values[i+1];
     }
     right->num_keys--;
 
     // Update parent's separator
     node->parent->keys[parentIndex - 1] = right->keys[0];
 }
 
 /* Borrow from left internal node */
 static void borrow_from_left_internal(BPTreeNode* node, BPTreeNode* left, int parentIndex) {
     BPTreeNode* parent = node->parent;
     // Shift node keys/children right
     for (int i=node->num_keys; i>0; i--) {
         node->keys[i] = node->keys[i-1];
         node->children[i+1] = node->children[i];
     }
     node->children[1] = node->children[0];
 
     // parent's separator goes to node->keys[0]
     node->keys[0] = parent->keys[parentIndex - 1];
     node->children[0] = left->children[left->num_keys];
     if (node->children[0]) {
         node->children[0]->parent = node;
     }
     node->num_keys++;
 
     // parent's separator replaced by left->keys[last]
     parent->keys[parentIndex - 1] = left->keys[left->num_keys -1];
 
     // left loses one key/child
     left->num_keys--;
     left->children[left->num_keys+1] = NULL;
 }
 
 static void borrow_from_right_internal(BPTreeNode* node, BPTreeNode* right, int parentIndex) {
     BPTreeNode* parent = node->parent;
     // node gets parent's separator
     node->keys[node->num_keys] = parent->keys[parentIndex - 1];
     node->children[node->num_keys + 1] = right->children[0];
     if (node->children[node->num_keys + 1]) {
         node->children[node->num_keys + 1]->parent = node;
     }
     node->num_keys++;
 
     // parent's separator becomes right->keys[0]
     parent->keys[parentIndex -1] = right->keys[0];
 
     // shift right->keys/children left
     for (int i=0; i< right->num_keys -1; i++) {
         right->keys[i] = right->keys[i+1];
         right->children[i] = right->children[i+1];
     }
     right->children[right->num_keys -1] = right->children[right->num_keys];
     right->num_keys--;
 }
 
 /* --------------------------- Merging Functions --------------------------- */
 static void merge_nodes(BPTree* tree, BPTreeNode* left, BPTreeNode* right, int sepIndex) {
    if (left->is_leaf && right->is_leaf) {
         merge_leaves(tree, left, right, sepIndex);
     } else {
         merge_internals(tree, left, right, sepIndex);
     }
 }
 
static void merge_leaves(BPTree* tree, BPTreeNode* leftLeaf, BPTreeNode* rightLeaf, int sepIndex)
{
    printf("Merging leaves\n");
    BPTreeNode* parent = leftLeaf->parent;

    // 1) Append all keys/values from rightLeaf into leftLeaf
    for (int i = 0; i < rightLeaf->num_keys; i++) {
        leftLeaf->keys[leftLeaf->num_keys + i]   = rightLeaf->keys[i];
        leftLeaf->values[leftLeaf->num_keys + i] = rightLeaf->values[i];
    }
    leftLeaf->num_keys += rightLeaf->num_keys;

    // 2) Update leaf-link pointer
    leftLeaf->next = rightLeaf->next;

    // 3) Remove the separator key in the parent
    //    and the rightLeaf pointer in 'children[]'.
    //    This is done by shifting everything to the left.
    for (int i = sepIndex; i < parent->num_keys - 1; i++) {
        parent->keys[i]       = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }

    // 4) Clear out the old last slot (since we shifted left).
    parent->keys[parent->num_keys - 1]     = NULL;
    parent->children[parent->num_keys]     = NULL;

    // 5) Parent now has one fewer key
    parent->num_keys--;

    // 6) Free the entire rightLeaf node
    free_node(rightLeaf);
}
 
 /* Merge internal nodes */
 static void merge_internals(BPTree* tree, BPTreeNode* leftNode, BPTreeNode* rightNode, int sepIndex) {
     printf("Merging internal nodes\n"); 

     BPTreeNode* parent = leftNode->parent;
     void* sepKey = parent->keys[sepIndex];
 
     // sepKey goes into leftNode
     leftNode->keys[leftNode->num_keys] = sepKey;
     leftNode->num_keys++;
 
     // Copy rightNode's keys/children
     int offset = leftNode->num_keys;
     for (int i=0; i<rightNode->num_keys; i++) {
         leftNode->keys[offset + i] = rightNode->keys[i];
     }
     for (int i=0; i <= rightNode->num_keys; i++) {
         leftNode->children[offset + i] = rightNode->children[i];
         if (rightNode->children[i]) {
             rightNode->children[i]->parent = leftNode;
         }
     }
     leftNode->num_keys += rightNode->num_keys;
 
     // Remove sepKey & child pointer from parent
     for (int i=sepIndex; i<parent->num_keys-1; i++) {
         parent->keys[i]       = parent->keys[i+1];
         parent->children[i+1] = parent->children[i+2];
     }
     // Clear the now-unused slot
     parent->keys[parent->num_keys -1] = NULL;
     parent->children[parent->num_keys] = NULL;
 
     parent->num_keys--;
 
     free_node(rightNode);
 }
 
 /* -------------------------- Utility Functions --------------------------- */
 static int find_child_index(BPTreeNode* parent, BPTreeNode* child) {
     for (int i=0; i <= parent->num_keys; i++) {
         if (parent->children[i] == child) {
             return i;
         }
     }
     return -1; 
 }
 
 static int find_key_index_in_node(BPTreeNode* node, void* key, int (*cmp)(const void*, const void*)) {
     for (int i=0; i<node->num_keys; i++) {
         int c = cmp(key, node->keys[i]);
         if (c == 0) return i;
         if (c < 0) return i; 
     }
     return node->num_keys;
 }
 
 /* Underflows if node->num_keys < MIN_KEYS (except root) */
 static bool underflows(BPTree* tree, BPTreeNode* node) {
     if (!node->parent) {
         // Root may have fewer
         return false;
     }
     int min_keys = ((tree->order + 1)/2) -1; // For ORDER=4 => 1
     return (node->num_keys < min_keys);
 }
 
 /* --------------------------------------------------------------------------
  * bptree_print: BFS-level debug
  * -------------------------------------------------------------------------- */
 typedef struct QNode {
     BPTreeNode* node;
     struct QNode* next;
 } QNode;
 
 static void enqueue(QNode** head, BPTreeNode* node) {
     QNode* nq = (QNode*)malloc(sizeof(QNode));
     nq->node = node;
     nq->next = NULL;
     if (!(*head)) {
         *head = nq;
         return;
     }
     QNode* tmp = *head;
     while (tmp->next) tmp = tmp->next;
     tmp->next = nq;
 }
 
 static BPTreeNode* dequeue(QNode** head) {
     if(!(*head)) return NULL;
     QNode* front = *head;
     BPTreeNode* node = front->node;
     *head = front->next;
     free(front);
     return node;
 }
 
 void bptree_print(BPTree* tree) {
     if (!tree || !tree->root) {
         printf("(Empty B+ Tree)\n");
         return;
     }
     printf("B+ Tree (order=%d):\n", tree->order);
     QNode* queue = NULL;
     enqueue(&queue, tree->root);
 
     BPTreeNode* last_in_level = tree->root;
     BPTreeNode* next_last_in_level = NULL;
 
     while (queue) {
         BPTreeNode* node = dequeue(&queue);
         /* Print node's keys (assuming they're int*). */
         printf("[");
         for (int i=0; i<node->num_keys; i++){
             if (i>0) printf(" ");
             int keyval = *(int*)node->keys[i];
             printf("%d", keyval);
         }
         printf("] ");
 
         if (!node->is_leaf) {
             for (int i=0; i<=node->num_keys; i++){
                 if (node->children[i]) {
                     enqueue(&queue, node->children[i]);
                     next_last_in_level = node->children[i];
                 }
             }
         }
 
         if (node == last_in_level) {
             printf("\n");
             last_in_level = next_last_in_level;
             next_last_in_level = NULL;
         }
     }
     printf("\n");
 }
 