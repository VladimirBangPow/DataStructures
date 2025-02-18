/******************************************************************************
 * bptree.c
 *
 * A B+ tree implementation conforming to bptree.h
 * Now with a full deletion implementation.
 *
 * Key points:
 *  - We store 'void *' in keys and values. The user supplies a comparator (cmp).
 *  - Insertion & search are O(log N). Deletion is also O(log N).
 *  - We label the insertion scenarios (1-6). Deletion uses standard B+ logic:
 *    * Find leaf, remove key
 *    * If node underflows, borrow or merge
 *    * Possibly merge upward
 *    * Adjust root if empty or single child
 *
 ******************************************************************************/

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <string.h>
 #include "bp_tree.h"
 
 /* --------------------------------------------------------------------------
  * A simple integer comparator for convenience
  * -------------------------------------------------------------------------- */
 int bptree_int_cmp(const void* a, const void* b) {
     /* 'a' and 'b' are pointers to int. */
     int x = *(const int*)a;
     int y = *(const int*)b;
     if (x < y) return -1;
     if (x > y) return  1;
     return 0;
 }
 
 /* --------------------------------------------------------------------------
  * Internal helper: create a BPTreeNode
  * -------------------------------------------------------------------------- */
 static BPTreeNode* bptree_node_create(int order, bool is_leaf) {
     BPTreeNode* node = (BPTreeNode*)calloc(1, sizeof(BPTreeNode));
     if (!node) {
         fprintf(stderr, "Error: Could not allocate BPTreeNode.\n");
         return NULL;
     }
     node->is_leaf  = is_leaf;
     node->num_keys = 0;
     node->keys     = (void**)calloc(order - 1, sizeof(void*));
     node->children = (BPTreeNode**)calloc(order, sizeof(BPTreeNode*));
     node->values   = NULL;  /* only used if leaf */
 
     if (is_leaf) {
         /* leaf has up to (order - 1) key/value pairs */
         node->values = (void**)calloc(order - 1, sizeof(void*));
     }
     node->parent = NULL;
     node->next   = NULL; /* used in leaf chaining */
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
         fprintf(stderr, "Comparator function is NULL.\n");
         return NULL;
     }
 
     BPTree* tree = (BPTree*)calloc(1, sizeof(BPTree));
     if (!tree) {
         fprintf(stderr, "Error: Could not allocate BPTree.\n");
         return NULL;
     }
     tree->order = order;
     tree->root  = NULL;
     tree->cmp   = cmp;
     return tree;
 }
 
 /* --------------------------------------------------------------------------
  * Free an entire subtree (post-order traversal).
  * -------------------------------------------------------------------------- */
 static void bptree_node_free_subtree(BPTreeNode* node, int order) {
     if (!node) return;
     if (!node->is_leaf) {
         /* internal node: free children first */
         for (int i = 0; i <= node->num_keys; i++) {
             bptree_node_free_subtree(node->children[i], order);
         }
     }
     free(node->keys);
     free(node->children);
     if (node->is_leaf) {
         free(node->values);
     }
     free(node);
 }
 
 /* --------------------------------------------------------------------------
  * bptree_destroy
  * -------------------------------------------------------------------------- */
 void bptree_destroy(BPTree* tree) {
     if (!tree) return;
     if (tree->root) {
         bptree_node_free_subtree(tree->root, tree->order);
     }
     free(tree);
 }
 
 /* --------------------------------------------------------------------------
  * find_leaf: descend the tree until a leaf node is found.
  * -------------------------------------------------------------------------- */
 static BPTreeNode* bptree_find_leaf(BPTree* tree, void* key) {
     BPTreeNode* current = tree->root;
     while (current && !current->is_leaf) {
         int i = 0;
         while (i < current->num_keys) {
             if (tree->cmp(key, current->keys[i]) < 0) {
                 break;
             }
             i++;
         }
         current = current->children[i];
     }
     return current;
 }
 
 /* search_in_leaf: find the matching key. Return corresponding value pointer or NULL. */
 static void* bptree_search_in_leaf(BPTreeNode* leaf, bptree_cmp cmp, void* key) {
     for (int i = 0; i < leaf->num_keys; i++) {
         if (cmp(key, leaf->keys[i]) == 0) {
             return leaf->values[i];
         }
     }
     return NULL;
 }
 
 /* --------------------------------------------------------------------------
  * bptree_search
  * -------------------------------------------------------------------------- */
 void* bptree_search(BPTree* tree, void* key) {
     if (!tree || !tree->root) {
         return NULL;
     }
     BPTreeNode* leaf = bptree_find_leaf(tree, key);
     if (!leaf) return NULL;
     return bptree_search_in_leaf(leaf, tree->cmp, key);
 }
 
 /* --------------------------------------------------------------------------
  * Insertion logic (scenarios 1-6)
  * -------------------------------------------------------------------------- */
 static void bptree_insert_leaf_no_split(BPTreeNode* leaf, bptree_cmp cmp, void* key, void* value);
 static void bptree_insert_leaf_split(BPTree* tree, BPTreeNode* leaf, void* key, void* value);
 static void bptree_insert_into_parent(BPTree* tree, BPTreeNode* left, void* up_key, BPTreeNode* right);
 static void insert_into_node(BPTreeNode* node, int idx, void* key, BPTreeNode* right);
 
 void bptree_insert(BPTree* tree, void* key, void* value) {
     if (!tree->root) {
         /* SCENARIO 1: Empty Tree */
         BPTreeNode* leaf = bptree_node_create(tree->order, true);
         leaf->keys[0]   = key;
         leaf->values[0] = value;
         leaf->num_keys  = 1;
         tree->root      = leaf;
         return;
     }
 
     /* find leaf */
     BPTreeNode* leaf = bptree_find_leaf(tree, key);
 
     if (leaf->num_keys < (tree->order - 1)) {
         /* SCENARIO 2: Leaf has space */
         bptree_insert_leaf_no_split(leaf, tree->cmp, key, value);
     } else {
         /* SCENARIO 3: Leaf is full -> split */
         bptree_insert_leaf_split(tree, leaf, key, value);
     }
 }
 
 /* Leaf insertion without split */
 static void bptree_insert_leaf_no_split(BPTreeNode* leaf, bptree_cmp cmp, void* key, void* value) {
     int i = leaf->num_keys - 1;
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
 
 /* Leaf insertion with split => new leaf and promote up_key */
 static void bptree_insert_leaf_split(BPTree* tree, BPTreeNode* leaf, void* key, void* value) {
     int order = tree->order;
     void** temp_keys   = (void**)calloc(order, sizeof(void*));
     void** temp_vals   = (void**)calloc(order, sizeof(void*));
     if (!temp_keys || !temp_vals) {
         free(temp_keys); free(temp_vals);
         return;
     }
 
     /* merge old + new in temp */
     int insert_idx = 0;
     while (insert_idx < leaf->num_keys && tree->cmp(key, leaf->keys[insert_idx]) > 0) {
         insert_idx++;
     }
     int i, j;
     for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
         if (j == insert_idx) {
             j++;
         }
         temp_keys[j] = leaf->keys[i];
         temp_vals[j] = leaf->values[i];
     }
     /* place new key/value */
     temp_keys[insert_idx] = key;
     temp_vals[insert_idx] = value;
 
     /* split point */
     int split = (order - 1) / 2;
 
     /* left leaf */
     leaf->num_keys = 0;
     for (i = 0; i < split; i++) {
         leaf->keys[i]   = temp_keys[i];
         leaf->values[i] = temp_vals[i];
         leaf->num_keys++;
     }
     BPTreeNode* new_leaf = bptree_node_create(order, true);
     int k;
     for (k = 0; i < order; i++, k++) {
         new_leaf->keys[k]   = temp_keys[i];
         new_leaf->values[k] = temp_vals[i];
         new_leaf->num_keys++;
     }
     new_leaf->next = leaf->next;
     leaf->next     = new_leaf;
     new_leaf->parent = leaf->parent;
 
     void* up_key = new_leaf->keys[0];
 
     free(temp_keys);
     free(temp_vals);
 
     /* Insert up_key into parent */
     bptree_insert_into_parent(tree, leaf, up_key, new_leaf);
 }
 
 /* Insert up_key from child split into the parent node. */
 static void bptree_insert_into_parent(BPTree* tree, BPTreeNode* left, void* up_key, BPTreeNode* right) {
     BPTreeNode* parent = left->parent;
     if (!parent) {
         /* SCENARIO 6: create new root */
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
 
     /* find where 'left' is in parent's children */
     int idx = 0;
     while (idx <= parent->num_keys && parent->children[idx] != left) {
         idx++;
     }
 
     if (parent->num_keys < (tree->order - 1)) {
         /* SCENARIO 4: parent not full */
         insert_into_node(parent, idx, up_key, right);
         right->parent = parent;
     } else {
         /* SCENARIO 5: parent is full => split internal node */
         int order = tree->order;
         void** temp_keys = (void**)calloc(order, sizeof(void*));
         BPTreeNode** temp_ch = (BPTreeNode**)calloc(order+1, sizeof(BPTreeNode*));
         if (!temp_keys || !temp_ch) {
             free(temp_keys); free(temp_ch);
             return;
         }
         int i, j=0;
         for (i=0; i <= parent->num_keys; i++, j++) {
             if (j == idx+1) j++;
             temp_ch[j] = parent->children[i];
         }
         j=0;
         for (i=0; i < parent->num_keys; i++, j++) {
             if (j == idx) j++;
             temp_keys[j] = parent->keys[i];
         }
         /* add up_key, right */
         temp_keys[idx]    = up_key;
         temp_ch[idx+1]    = right;
 
         int mid = (order - 1)/2; /* fix to ensure left side >= 1 key for order=3 */
         parent->num_keys = 0;
         for (i=0; i < mid; i++) {
             parent->children[i] = temp_ch[i];
             parent->keys[i]     = temp_keys[i];
             parent->num_keys++;
         }
         parent->children[i] = temp_ch[i];
         void* promoted_key = temp_keys[mid];
 
         BPTreeNode* new_node = bptree_node_create(order, false);
         int k = 0;
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
         new_node->parent   = parent;
 
         for (int c=0; c <= parent->num_keys; c++) {
             if (parent->children[c]) {
                 parent->children[c]->parent = parent;
             }
         }
 
         free(temp_keys);
         free(temp_ch);
 
         bptree_insert_into_parent(tree, parent, promoted_key, new_node);
     }
 }
 
 /* Insert key+right into a non-full parent at index 'idx'. */
 static void insert_into_node(BPTreeNode* node, int idx, void* key, BPTreeNode* right) {
     for (int i=node->num_keys; i>idx; i--) {
         node->keys[i]       = node->keys[i-1];
         node->children[i+1] = node->children[i];
     }
     node->keys[idx]         = key;
     node->children[idx+1]   = right;
     node->num_keys++;
 }
 
 /* --------------------------------------------------------------------------
  * bptree_delete
  * -------------------------------------------------------------------------- */
 
 /* --- Deletion additions --- */
 static bool bptree_delete_entry(BPTree* tree, BPTreeNode* node, void* key, void* value_or_child);
 static void bptree_remove_entry_from_node(BPTreeNode* node, bptree_cmp cmp,
                                           void* key, void* value_or_child);
 static void bptree_adjust_root(BPTree* tree);
 static BPTreeNode* bptree_get_left_sibling(BPTreeNode* node, int* index_in_parent);
 static BPTreeNode* bptree_get_right_sibling(BPTreeNode* node, int* index_in_parent);
 static void bptree_coalesce_nodes(BPTree* tree, BPTreeNode* left, BPTreeNode* right,
                                   int parent_key_index);
 static void bptree_redistribute_nodes(BPTree* tree, BPTreeNode* left, BPTreeNode* right,
                                       int parent_key_index, bool left_is_left);
 
 /**
  * bptree_delete:
  *   1) find the leaf containing 'key'
  *   2) remove the key
  *   3) if leaf underflows, borrow or merge
  *   4) possibly merge upward
  */
 bool bptree_delete(BPTree* tree, void* key) {
     if (!tree || !tree->root) return false;
     BPTreeNode* leaf = bptree_find_leaf(tree, key);
     if (!leaf) return false;
 
     /* Find position of key in leaf */
     int idx = -1;
     for (int i=0; i<leaf->num_keys; i++) {
         if (tree->cmp(key, leaf->keys[i]) == 0) {
             idx = i;
             break;
         }
     }
     if (idx < 0) {
         /* not found */
         return false;
     }
 
     /* Remove key from leaf */
     return bptree_delete_entry(tree, leaf, key, leaf->values[idx]);
 }
 
 /* bptree_delete_entry:
  *    1) remove key and pointer from node
  *    2) if node is root, call adjust_root if empty
  *    3) if node underflows, borrow or merge
  */
 static bool bptree_delete_entry(BPTree* tree, BPTreeNode* node, void* key, void* value_or_child) {
     /* remove key and pointer from node */
     bptree_remove_entry_from_node(node, tree->cmp, key, value_or_child);
 
     /* if node == root, check if root is empty or needs adjusting */
     if (node == tree->root) {
         bptree_adjust_root(tree);
         return true;
     }
 
     /* check min # of keys required */
     int order = tree->order;
     int min_keys = (order - 1) / 2;  /* for leaves: (m-1)/2. For internal: ~ (m/2)-1. */
     if (!node->is_leaf) {
         /* internal node typically requires at least (order/2) children => (order/2)-1 keys */
         /* e.g. for order=3 => at least 1 key. So min_keys is the same formula, but 
            you might do an extra check if you want a distinct formula for internal. */
         min_keys = (order/2) - 1;
         if (min_keys < 1) min_keys=1; /* in case order=3 => (3/2)-1=0.5 => 0 */
     }
 
     /* if node not underflow, done */
     if (node->num_keys >= min_keys) {
         return true;
     }
 
     /* otherwise, fix underflow by borrow or merge. */
     BPTreeNode* parent = node->parent;
     if (!parent) {
         /* if parent is NULL but node != root => edge case, typically doesn't happen */
         return true;
     }
 
     /* find index_in_parent */
     int index_in_parent = -1;
     for (int i=0; i <= parent->num_keys; i++) {
         if (parent->children[i] == node) {
             index_in_parent = i;
             break;
         }
     }
 
     /* siblings */
     BPTreeNode* left_sib  = (index_in_parent>0) ? parent->children[index_in_parent - 1] : NULL;
     BPTreeNode* right_sib = (index_in_parent<parent->num_keys) ? parent->children[index_in_parent + 1] : NULL;
 
     /* Try borrow from left or right sibling if possible */
     if (left_sib && left_sib->num_keys > min_keys) {
         bptree_redistribute_nodes(tree, left_sib, node, index_in_parent-1, true);
     }
     else if (right_sib && right_sib->num_keys > min_keys) {
         bptree_redistribute_nodes(tree, node, right_sib, index_in_parent, false);
     }
     else {
         /* merge with sibling */
         if (left_sib) {
             bptree_coalesce_nodes(tree, left_sib, node, index_in_parent-1);
         } else if (right_sib) {
             bptree_coalesce_nodes(tree, node, right_sib, index_in_parent);
         }
     }
     return true;
 }
 
 /* Remove key & pointer from node. Does not fix underflow. */
 static void bptree_remove_entry_from_node(BPTreeNode* node, bptree_cmp cmp,
                                           void* key, void* value_or_child) {
     int i, found_key=-1;
     /* find key in node->keys */
     for (i=0; i<node->num_keys; i++) {
         if (cmp(key, node->keys[i]) == 0) {
             found_key = i;
             break;
         }
     }
     if (found_key>=0) {
         /* shift keys left from found_key */
         for (int j=found_key; j<node->num_keys-1; j++) {
             node->keys[j] = node->keys[j+1];
             if (node->is_leaf) {
                 node->values[j] = node->values[j+1];
             }
         }
         /* clear last slot */
         node->keys[node->num_keys - 1] = NULL;
         if (node->is_leaf) {
             node->values[node->num_keys - 1] = NULL;
         }
         node->num_keys--;
     }
 
     /* if internal node, remove child pointer if matches 'value_or_child' */
     if (!node->is_leaf) {
         int c_idx=-1;
         for (i=0; i<=node->num_keys; i++) {
             if (node->children[i] == (BPTreeNode*)value_or_child) {
                 c_idx = i; break;
             }
         }
         if (c_idx>=0) {
             for (int j=c_idx; j<node->num_keys+1; j++) {
                 node->children[j] = node->children[j+1];
             }
             node->children[node->num_keys+1] = NULL;
         }
     }
 }
 
 /* adjust_root: if root has 0 keys, remove it or replace with child. */
 static void bptree_adjust_root(BPTree* tree) {
     BPTreeNode* root = tree->root;
     if (!root) return;
     if (root->num_keys>0) return; /* no adjustment needed */
 
     /* If it's a leaf & 0 keys => entire tree is empty */
     if (root->is_leaf) {
         tree->root = NULL;
         free(root->keys);
         free(root->values);
         free(root->children);
         free(root);
     } else {
         /* otherwise, root is internal but has no keys => make its only child new root */
         BPTreeNode* child = root->children[0];
         child->parent = NULL;
         tree->root = child;
         /* free old root */
         free(root->keys);
         free(root->children);
         free(root);
     }
 }
 
 /* coalesce_nodes: merges 'right' into 'left' if left is the preceding sibling. */
 static void bptree_coalesce_nodes(BPTree* tree, BPTreeNode* left, BPTreeNode* right,
                                   int parent_key_index) {
     BPTreeNode* parent = left->parent;
     /* For B+ tree:
      * If internal, we bring down parent->keys[parent_key_index].
      * Merge right's keys+children into left.
      * Then remove parent->keys[parent_key_index].
      * Then fix parent->children, etc.
      */
     if (!left->is_leaf) {
         /* Internal node merge:
          * left->keys[left->num_keys] = parent->keys[parent_key_index]
          * then copy right->keys into left
          */
         // bring down the parent's separator
         left->keys[left->num_keys] = parent->keys[parent_key_index];
         left->num_keys++;
         // copy right's keys & children
         for (int i=0; i<right->num_keys; i++) {
             left->keys[left->num_keys] = right->keys[i];
             left->children[left->num_keys] = right->children[i];
             if (left->children[left->num_keys]) {
                 left->children[left->num_keys]->parent = left;
             }
             left->num_keys++;
         }
         // last child
         left->children[left->num_keys] = right->children[right->num_keys];
         if (left->children[left->num_keys]) {
             left->children[left->num_keys]->parent = left;
         }
     } else {
         /* Leaf merge: copy right's key/values to the end of left */
         for (int i=0; i<right->num_keys; i++) {
             left->keys[left->num_keys]   = right->keys[i];
             left->values[left->num_keys] = right->values[i];
             left->num_keys++;
         }
         left->next = right->next;
     }
 
     /* now remove parent's key at parent_key_index, and child's pointer */
     void* key_to_remove = parent->keys[parent_key_index];
     bptree_delete_entry(tree, parent, key_to_remove, right);
     /* free the right node */
     free(right->keys);
     if (right->is_leaf) free(right->values);
     free(right->children);
     free(right);
 }
 
 /* redistribute_nodes: move 1 key from left sibling to right or vice versa */
 static void bptree_redistribute_nodes(BPTree* tree, BPTreeNode* left, BPTreeNode* right,
                                       int parent_key_index, bool left_is_left) {
     BPTreeNode* parent = left->parent;
     /* For internal node:
      *  - if we move from left->right, parent's key in [parent_key_index] moves down to 'right',
      *    left->last key moves up to parent.
      *  - if leaf, we just shift key/value; parent's key is updated to reflect new boundary.
      */
     if (!left->is_leaf) {
         if (left_is_left) {
             /* shift right node right by 1 */
             for (int i=right->num_keys; i>0; i--) {
                 right->keys[i]     = right->keys[i-1];
                 right->children[i+1] = right->children[i];
             }
             right->children[1] = right->children[0];
             /* parent's separator goes down to right->keys[0] */
             right->keys[0] = parent->keys[parent_key_index];
             right->children[0] = left->children[left->num_keys];
             if (right->children[0]) {
                 right->children[0]->parent = right;
             }
             right->num_keys++;
 
             /* left->last key moves up to parent->keys[parent_key_index] */
             parent->keys[parent_key_index] = left->keys[left->num_keys -1];
             left->keys[left->num_keys -1] = NULL;
             left->children[left->num_keys] = NULL;
             left->num_keys--;
         } else {
             /* right->left shift */
             left->keys[left->num_keys] = parent->keys[parent_key_index];
             left->children[left->num_keys+1] = right->children[0];
             if (left->children[left->num_keys+1]) {
                 left->children[left->num_keys+1]->parent = left;
             }
             left->num_keys++;
 
             parent->keys[parent_key_index] = right->keys[0];
             /* shift right->keys left */
             for (int i=0; i<right->num_keys-1; i++) {
                 right->keys[i]       = right->keys[i+1];
                 right->children[i]   = right->children[i+1];
             }
             right->children[right->num_keys-1] = right->children[right->num_keys];
             right->keys[right->num_keys-1]     = NULL;
             right->children[right->num_keys]   = NULL;
             right->num_keys--;
         }
     } else {
         /* leaf nodes redistribution */
         if (left_is_left) {
             /* move first key of right -> end of left */
             left->keys[left->num_keys]   = right->keys[0];
             left->values[left->num_keys] = right->values[0];
             left->num_keys++;
             /* parent's key is updated to right->keys[1] */
             parent->keys[parent_key_index] = right->keys[1];
             /* shift right left by 1 */
             for (int i=0; i<right->num_keys-1; i++) {
                 right->keys[i]   = right->keys[i+1];
                 right->values[i] = right->values[i+1];
             }
             right->keys[right->num_keys-1]   = NULL;
             right->values[right->num_keys-1] = NULL;
             right->num_keys--;
         } else {
             /* move last key of left to front of right */
             for (int i=right->num_keys; i>0; i--) {
                 right->keys[i]   = right->keys[i-1];
                 right->values[i] = right->values[i-1];
             }
             right->keys[0]   = left->keys[left->num_keys-1];
             right->values[0] = left->values[left->num_keys-1];
             parent->keys[parent_key_index] = right->keys[0];
 
             left->keys[left->num_keys-1]   = NULL;
             left->values[left->num_keys-1] = NULL;
             left->num_keys++;
             right->num_keys++;
             left->num_keys--;
         }
     }
 }
 
 /* --------------------------------------------------------------------------
  * bptree_print: simple BFS for debugging
  * -------------------------------------------------------------------------- */
 typedef struct QNode {
     BPTreeNode* node;
     struct QNode* next;
 } QNode;
 
 static void enqueue(QNode** head, BPTreeNode* node) {
     QNode* qn = (QNode*)malloc(sizeof(QNode));
     qn->node = node;
     qn->next = NULL;
     if (!(*head)) {
         *head = qn;
         return;
     }
     QNode* tmp = *head;
     while (tmp->next) {
         tmp = tmp->next;
     }
     tmp->next = qn;
 }
 
 static BPTreeNode* dequeue(QNode** head) {
     if (!(*head)) return NULL;
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
     QNode* queue = NULL;
     enqueue(&queue, tree->root);
 
     BPTreeNode* last_in_level = tree->root;
     BPTreeNode* next_last_in_level = NULL;
     printf("B+ Tree (order=%d):\n", tree->order);
 
     while (queue) {
         BPTreeNode* node = dequeue(&queue);
         /* Print node's keys as pointer addresses or interpret them if you know they're int* */
         printf("[");
         for (int i = 0; i < node->num_keys; i++) {
             if (i > 0) printf(" ");
             printf("%p", node->keys[i]);
         }
         printf("] ");
 
         if (!node->is_leaf) {
             for (int i = 0; i <= node->num_keys; i++) {
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
 