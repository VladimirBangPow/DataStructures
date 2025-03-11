#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pq.h"
#include "dynamic_array.h"

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
 * A helper that uses the base compare function but flips it if we're in max-heap mode.
 *
 * In a min-heap, we do: userCompareFunc(a, b).
 * In a max-heap, we effectively want "a is higher priority if a > b", so we invert
 * the comparison by swapping arguments in the call.
 */
static int priorityCompare(const PriorityQueue* pq, const void* a, const void* b) {
    if (pq->isMinHeap) {
        // Normal “ascending” compare
        return pq->userCompareFunc(a, b);
    } else {
        // Max-heap => invert the result
        // (i.e., if userCompareFunc returns negative for (a < b),
        //  we want negative if (a > b), so we swap a and b.)
        return pq->userCompareFunc(b, a);
    }
}

/**
 * Bubble up the element at 'index' to maintain the heap property.
 */
static void bubbleUp(PriorityQueue* pq, size_t index) {
    while (index > 0) {
        size_t parentIndex = (index - 1) / 2;
        const void* childData = daGet(&pq->da, index);
        const void* parentData = daGet(&pq->da, parentIndex);

        // If child is "higher priority" than parent, swap
        // "Higher priority" means priorityCompare(...) < 0
        if (priorityCompare(pq, childData, parentData) < 0) {
            swapElements(&pq->da, index, parentIndex);
            index = parentIndex;
        } else {
            break; // no swap needed, heap property satisfied
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
        size_t highestPriorityIndex = index; // assume current index has highest priority
        const void* currentData = daGet(&pq->da, index);

        // Check left child
        if (leftChild < size) {
            const void* leftData = daGet(&pq->da, leftChild);
            if (priorityCompare(pq, leftData, currentData) < 0) {
                highestPriorityIndex = leftChild;
            }
        }

        // Check right child
        if (rightChild < size) {
            const void* bestDataSoFar = daGet(&pq->da, highestPriorityIndex);
            const void* rightData = daGet(&pq->da, rightChild);
            if (priorityCompare(pq, rightData, bestDataSoFar) < 0) {
                highestPriorityIndex = rightChild;
            }
        }

        if (highestPriorityIndex != index) {
            // Swap with the child that has higher priority
            swapElements(&pq->da, index, highestPriorityIndex);
            index = highestPriorityIndex;
        } else {
            break; // heap property is restored
        }
    }
}

/********************************************************
 * Public interface
 ********************************************************/
void pqInit(PriorityQueue* pq,
            int (*userCompareFunc)(const void*, const void*),
            bool isMinHeap,
            size_t initialCapacity) 
{
    daInit(&pq->da, initialCapacity);
    pq->userCompareFunc = userCompareFunc;  // user-provided base compare
    pq->isMinHeap = isMinHeap;              // min-heap or max-heap mode?
}

void pqFree(PriorityQueue* pq) {
    daFree(&pq->da);
    pq->userCompareFunc = NULL;
    pq->isMinHeap = true; // or false, doesn’t really matter now
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
