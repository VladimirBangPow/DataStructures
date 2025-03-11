#include <stdio.h>
#include <stdlib.h>
#include "circular_list.h"

/*
 * A small helper that "repairs" next/prev pointers
 * so the list is truly circular:
 *
 *  head->prev = tail
 *  tail->next = head
 *
 *  ... but only if the list is not empty.
 */
static void enforceCircular(CircularList* list) {
	if (!list->head) return; // empty => no fix
    if (list->head == list->tail) {
        // Single node
        list->head->prev = list->head;
        list->head->next = list->head;
    } else {
        // 2+ nodes
        list->head->prev = list->tail;
        list->tail->next = list->head;
    }
}

void clistInit(CircularList* list) {
    dllInit(list);  // Initialize via the original DLL function
}

/* Insert front, then enforce circularity */
void clistInsertFront(CircularList* list, const void* data, size_t data_size) {
    dllInsertFront(list, data, data_size); 
    enforceCircular(list);
}

/* Insert back, then enforce circularity */
void clistInsertBack(CircularList* list, const void* data, size_t data_size) {
    dllInsertBack(list, data, data_size);
    enforceCircular(list);
}

/* Remove front, then enforce circularity */
int clistRemoveFront(CircularList* list, void* outData) {
    int result = dllRemoveFront(list, outData);
    // If successful, patch circular links
    if (result) {
        enforceCircular(list);
    }
    return result;
}

/* Remove back, then enforce circularity */
int clistRemoveBack(CircularList* list, void* outData) {
    int result = dllRemoveBack(list, outData);
    if (result) {
        enforceCircular(list);
    }
    return result;
}

/* Print in a circular manner, but stop once we get back to head */
void clistPrint(const CircularList* list, void (*printFunc)(const void*)) {
    if (!list->head) {
        printf("NULL\n");
        return;
    }

    /* We'll do a do-while loop from head to tail->next == head */
    DNode* current = list->head;
    do {
        printFunc(current->data);
        current = current->next;
    } while (current && current != list->head);

    printf(" (circular back to head)\n");
}

/* Free everything. You can just call dllFree, which removes all nodes.
   The 'circular' aspect doesn't matter if we're destroying the entire list. */
void clistFree(CircularList* list) {
    dllFree(list);
}
