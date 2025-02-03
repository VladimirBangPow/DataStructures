#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <stddef.h>  // for size_t
#include <stdbool.h> // for bool

typedef struct SkipListNode {
    void *data;
    struct SkipListNode *above;
	struct SkipListNode *below;
	struct SkipListNode *next;
	struct SkipListNode *prev; 
} SkipListNode;



typedef struct SkipList {
	SkipListNode *head;
} SkipList;



void slInit(SkipList *sl);

void slFree(SkipList *sl);

bool slInsert(SkipList *sl, void *data);

bool slSearch(const SkipList *sl, const void *data);

bool slRemove(SkipList *sl, const void *data);



#endif // SKIP_LIST_H
