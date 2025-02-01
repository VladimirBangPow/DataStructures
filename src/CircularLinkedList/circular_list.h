#ifndef CIRCULAR_LIST_H
#define CIRCULAR_LIST_H

#include "../DoubleLinkedList/double_linkedlist.h"

/*
 * We'll simply alias DoubleLinkedList to "CircularList."
 * Internally, it's still the same struct with head/tail pointers;
 * we just promise to maintain circular links using wrapper functions.
 */
typedef DoubleLinkedList CircularList;

/* Initialize */
void clistInit(CircularList* list);

/* Insert at front (maintaining circularity) */
void clistInsertFront(CircularList* list, const void* data, size_t data_size);

/* Insert at back (maintaining circularity) */
void clistInsertBack(CircularList* list, const void* data, size_t data_size);

/* Remove at front (maintaining circularity) */
int clistRemoveFront(CircularList* list, void* outData);

/* Remove at back (maintaining circularity) */
int clistRemoveBack(CircularList* list, void* outData);

/* Print in a circular manner (we'll do a do-while loop) */
void clistPrint(const CircularList* list, void (*printFunc)(const void*));

/* Free all nodes */
void clistFree(CircularList* list);

#endif
