#include "deque.h"

void dequeInit(Deque* dq) {
    // Under the hood, dq is just a DoubleLinkedList
    dllInit(dq);
}

void dequeDestroy(Deque* dq) {
    // Use our DLL free method
    dllFree(dq);
}

bool dequeIsEmpty(const Deque* dq) {
    // If head == NULL, it means the list is empty
    return (dq->head == NULL);
}

void dequePushFront(Deque* dq, const void* data, size_t data_size) {
    dllInsertFront(dq, data, data_size);
}

void dequePushBack(Deque* dq, const void* data, size_t data_size) {
    dllInsertBack(dq, data, data_size);
}

bool dequePopFront(Deque* dq, void* outData) {
    // dllRemoveFront returns 1 if successful, 0 if empty
    return (dllRemoveFront(dq, outData) != 0);
}

bool dequePopBack(Deque* dq, void* outData) {
    // dllRemoveBack returns 1 if successful, 0 if empty
    return (dllRemoveBack(dq, outData) != 0);
}

void dequePrint(const Deque* dq, void (*printFunc)(const void*)) {
    // Just call dllPrint
    dllPrint(dq, printFunc);
}
