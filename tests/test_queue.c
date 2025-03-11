#include <stdio.h>
#include <stdlib.h>
#include "test_queue.h"
#include "queue.h"
#include "double_linkedlist.h" // because our queue uses the doubly linked list internally

/**
 * Helper function to print an integer in the queue nodes.
 */
static void printInt(const void* data) {
    printf("%d -> ", *(const int*)data);
}

/**
 * Helper function to print the entire queue (front to back).
 */
static void printQueue(const Queue* queue) {
    // We'll reuse dllPrint from the doubly linked list, since the queue
    // is just a doubly linked list behind the scenes.
    printf("Queue (front -> back): ");
    dllPrint(&queue->list, printInt);
}

void testQueue(void) {
    printf("=== Testing Queue ===\n");

    // 1) Initialize the queue
    Queue q;
    queueInit(&q);

    // 2) Enqueue some integers
    int x = 10, y = 20, z = 30;
    queueEnqueue(&q, &x, sizeof(int)); // front=10
    queueEnqueue(&q, &y, sizeof(int)); // front=10, back=20
    queueEnqueue(&q, &z, sizeof(int)); // front=10, back=30

    printf("Queue after enqueuing 3 integers:\n");
    printQueue(&q);

    // 3) Dequeue one
    int dequeuedVal;
    if (queueDequeue(&q, &dequeuedVal)) {
        printf("Dequeued value: %d\n", dequeuedVal);
    } else {
        printf("Queue was empty, couldn't dequeue!\n");
    }
    printQueue(&q);

    // 4) Check if empty
    printf("Is queue empty? %s\n", queueIsEmpty(&q) ? "Yes" : "No");

    // 5) Clear the queue
    queueClear(&q);
    printf("Queue after clearing:\n");
    printQueue(&q);

    printf("Is queue empty? %s\n", queueIsEmpty(&q) ? "Yes" : "No");

    printf("=== Queue Tests Complete ===\n\n");
}
