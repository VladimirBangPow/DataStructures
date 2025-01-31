#include <stdio.h>
#include "test_circular_list.h"
#include "circular_list.h"

static void printInt(const void* data) {
    printf("%d -> ", *(const int*)data);
}

void testCircularList(void) {
    CircularList list;
    clistInit(&list);

    int a=10, b=20, c=30;
    clistInsertFront(&list, &a, sizeof(int)); // front=10
    clistInsertFront(&list, &b, sizeof(int)); // front=20 -> 10
    clistInsertBack(&list, &c, sizeof(int));  // front=20 -> 10 -> 30

    printf("Circular list content: ");
    clistPrint(&list, printInt);

    int removed;
    if (clistRemoveFront(&list, &removed)) {
        printf("Removed front: %d\n", removed);
    }
    clistPrint(&list, printInt);

    clistFree(&list);
}
