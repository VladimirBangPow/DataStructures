#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pq.h"
#include "../DynamicArray/dynamic_array.h"

/**
 * Swap two elements in the DynamicArray.
 */
static void swapElements(DynamicArray* da, size_t indexA, size_t indexB) {
    if (indexA == indexB) return;

    void* tempData = da->elements[indexA];
    size_t tempSize = da->elementSizes[indexA];

    da->elements[indexA] = da->elements[indexB];
    da->elementSizes[indexA] = da->elementSizes[indexB];

    da->elements[indexB] = tempData;
    da->elementSizes[indexB] = tempSize;
}

/**
 * Bubble up the element at 'index' to maintain the heap property.
 */
static void bubbleUp(PriorityQueue* pq, size_t index) {
    // child index: index
    // parent index: (index - 1) / 2
    while (index > 0) {
        size_t parentIndex = (index - 1) / 2;
        const void* childData = daGet(&pq->da, index);
        const void* parentData = daGet(&pq->da, parentIndex);

        // If child < parent based on compareFunc, swap
        // i.e., child is higher priority if compareFunc < 0
        if (pq->compareFunc(childData, parentData) < 0) {
            swapElements(&pq->da, index, parentIndex);
            index = parentIndex;
        } else {
            // Heap property satisfied
            break;
        }
    }
}

/**
 * Bubble down the element at 'index' to maintain the heap property.
 */
static void bubbleDown(PriorityQueue* pq, size_t index) {
    size_t size = daSize(&pq->da);

    while (true) {
        size_t leftChild = 2 * index + 1;
        size_t rightChild = 2 * index + 2;
        size_t smallest = index; // assume current index is the "smallest"/highest priority

        const void* currentData = daGet(&pq->da, index);

        // Check left child
        if (leftChild < size) {
            const void* leftData = daGet(&pq->da, leftChild);
            if (pq->compareFunc(leftData, currentData) < 0) {
                smallest = leftChild;
            }
        }

        // Check right child
        if (rightChild < size) {
            const void* smallestData = daGet(&pq->da, smallest);
            const void* rightData = daGet(&pq->da, rightChild);
            if (pq->compareFunc(rightData, smallestData) < 0) {
                smallest = rightChild;
            }
        }

        if (smallest != index) {
            // swap with smallest child
            swapElements(&pq->da, index, smallest);
            index = smallest;
        } else {
            break; // heap property is restored
        }
    }
}

/********************************************************
 * Public interface
 ********************************************************/

void pqInit(PriorityQueue* pq, 
            int (*compareFunc)(const void*, const void*), 
            size_t initialCapacity) {
    daInit(&pq->da, initialCapacity);
    pq->compareFunc = compareFunc;
}

void pqFree(PriorityQueue* pq) {
    daFree(&pq->da);
    pq->compareFunc = NULL;
}

void pqPush(PriorityQueue* pq, const void* data, size_t dataSize) {
    // 1. Push the new element to the end of the array.
    daPushBack(&pq->da, data, dataSize);

    // 2. Bubble up the new element.
    size_t newIndex = daSize(&pq->da) - 1;
    bubbleUp(pq, newIndex);
}

bool pqPop(PriorityQueue* pq, void* outData, size_t* outDataSize) {
    if (pqIsEmpty(pq)) {
        return false;
    }

    // 1. Swap top (index 0) with the last element (index size-1).
    size_t lastIndex = daSize(&pq->da) - 1;
    swapElements(&pq->da, 0, lastIndex);

    // 2. Pop the last element from the dynamic array to outData/outDataSize if needed.
    bool popped = daPopBack(&pq->da, outData, outDataSize);
    if (!popped) {
        // Shouldn't happen if we checked !empty, but just in case
        return false;
    }

    // 3. Bubble down the new top at index 0 (if not empty).
    if (!pqIsEmpty(pq)) {
        bubbleDown(pq, 0);
    }

    return true;
}

const void* pqTop(const PriorityQueue* pq) {
    if (pqIsEmpty(pq)) {
        return NULL;
    }
    return daGet(&pq->da, 0);
}

bool pqIsEmpty(const PriorityQueue* pq) {
    return daIsEmpty(&pq->da);
}

size_t pqSize(const PriorityQueue* pq) {
    return daSize(&pq->da);
}
