#ifndef QUEUE_H
#define QUEUE_H

#include "../DoubleLinkedList/double_linkedlist.h"

typedef struct {
    DoubleLinkedList list;
} Queue;

void queueInit(Queue* queue);
void queueEnqueue(Queue* queue, const void* data, size_t data_size);
int queueDequeue(Queue* queue, void* outData);
int queueIsEmpty(const Queue* queue);
void queueClear(Queue* queue);

#endif // QUEUE_H
