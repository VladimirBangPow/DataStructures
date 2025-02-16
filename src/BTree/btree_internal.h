#ifndef BTREE_INTERNAL_H
#define BTREE_INTERNAL_H

#include <stdbool.h>
#include "btree.h"

/* Full definition of BTreeNode for internal use */
struct BTreeNode {
    bool leaf;
    int nkeys;
    void **keys;
    struct BTreeNode **children;
};

#endif /* BTREE_INTERNAL_H */
