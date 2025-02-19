#ifndef PQ_H
#define PQ_H

#include <stdbool.h>
#include <stddef.h>
#include "../DynamicArray/dynamic_array.h"

typedef struct PriorityQueue {
    DynamicArray da;                   // our underlying dynamic array
    int (*userCompareFunc)(const void*, const void*);
    bool isMinHeap;                    // if true => min-heap, else max-heap
} PriorityQueue;

/**
 * Initialize the priority queue.
 * 
 * @param pq                 Pointer to a PriorityQueue struct
 * @param userCompareFunc    A base comparison function that returns:
 *                             negative if (a < b),
 *                             zero if (a == b),
 *                             positive if (a > b).
 * @param isMinHeap          true => treat userCompareFunc as ascending for a min-heap
 *                           false => invert userCompareFunc for a max-heap
 * @param initialCapacity    Initial capacity for the underlying dynamic array
 */
void pqInit(PriorityQueue* pq,
            int (*userCompareFunc)(const void*, const void*),
            bool isMinHeap,
            size_t initialCapacity);

void pqFree(PriorityQueue* pq);

void pqPush(PriorityQueue* pq, const void* data, size_t dataSize);

/**
 * Pop the top (lowest in min-heap / highest in max-heap) element from the queue.
 * Returns false if empty.
 */
bool pqPop(PriorityQueue* pq, void* outData, size_t* outDataSize);

/** 
 * Return a pointer to the top element (or NULL if empty). 
 * Does not remove it.
 */
const void* pqTop(const PriorityQueue* pq);

/** 
 * Returns true if the queue is empty. 
 */
bool pqIsEmpty(const PriorityQueue* pq);

size_t pqSize(const PriorityQueue* pq);

#endif
