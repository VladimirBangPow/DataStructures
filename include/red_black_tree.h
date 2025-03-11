/* red_black_tree.h */

#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <stddef.h>  /* for size_t */


/* Define colors publicly */
typedef enum { RED, BLACK } RBColor;

/* Define the node structure publicly */
typedef struct RBTNode {
    void* data;
    RBColor color;
    struct RBTNode* left;
    struct RBTNode* right;
    struct RBTNode* parent;
} RBTNode;

/* Define the main tree structure publicly */
typedef struct RBTREE {
    RBTNode* root;
    RBTNode* nil;  /* sentinel */
    int (*cmp)(const void*, const void*);
    void (*destroy)(void*);
    size_t size;
} RBTREE;

/* Public function prototypes: */

RBTREE* rb_create(int (*cmp)(const void*, const void*), void (*destroy)(void*));
void rb_destroy(RBTREE* tree);
int rb_insert(RBTREE* tree, void* data);
void* rb_search(RBTREE* tree, const void* data);
int rb_delete(RBTREE* tree, const void* data);
void rb_print_inorder(const RBTREE* tree, void (*print_func)(const void*));
size_t rb_size(const RBTREE* tree);

/* 
 * (Optional) Provide a verify function that can now be inline or external, 
 * but *any* code including this header can directly do their own checks 
 * if they want.
 */
int rb_verify_properties(const RBTREE* tree);

#endif /* RED_BLACK_TREE_H */
