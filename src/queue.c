#include "queue.h"

void queueInit(Queue* queue) {
    dllInit(&queue->list);
}

void queueEnqueue(Queue* queue, const void* data, size_t data_size) {
    dllInsertBack(&queue->list, data, data_size);
}

int queueDequeue(Queue* queue, void* outData) {
    return dllRemoveFront(&queue->list, outData);
}

int queueIsEmpty(const Queue* queue) {
    return queue->list.head == NULL;
}

void queueClear(Queue* queue) {
    dllFree(&queue->list);
}
