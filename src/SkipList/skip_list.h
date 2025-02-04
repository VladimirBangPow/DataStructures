#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <stddef.h>  // for size_t
#include <stdbool.h> // for bool

/** 
 * Comparison function for skip list:
 *   - returns < 0 if (a < b)
 *   - returns = 0 if (a == b)
 *   - returns > 0 if (a > b)
 */
typedef int (*SkipListComparator)(const void *a, const void *b);

/**
 * Optional free function for skip list:
 *   - if not NULL, this is called to free each node's data 
 *     on removal or when slFree() is called.
 */
typedef void (*SkipListFreeFunc)(void *data);


typedef struct SkipListNode {
    void *data;
    struct SkipListNode *above;
	struct SkipListNode *below;
	struct SkipListNode *next;
	struct SkipListNode *prev; 
} SkipListNode;



typedef struct SkipList {
	SkipListNode *head;
    SkipListComparator cmp;
    SkipListFreeFunc freeFunc;
} SkipList;



void slInit(SkipList *sl,SkipListComparator cmp, SkipListFreeFunc freeFunc);

void slFree(SkipList *sl);

bool slInsert(SkipList *sl, void *data);

bool slSearch(const SkipList *sl, const void *data);

bool slRemove(SkipList *sl, const void *data);



#endif // SKIP_LIST_H
