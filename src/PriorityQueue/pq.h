#ifndef PQ_H
#define PQ_H

#include <stdbool.h>
#include <stddef.h>

// Forward declare the DynamicArray so we can include dynamic_array.h in the .c
// to avoid circular dependencies if necessary.
#include "../DynamicArray/dynamic_array.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A generic priority queue that uses a DynamicArray for storage.
 * The compareFunc determines the ordering. 
 * If compareFunc(a, b) < 0, then a is considered "higher priority" than b.
 */
typedef struct {
    DynamicArray da;  // The underlying dynamic array (heap)
    int (*compareFunc)(const void*, const void*);
} PriorityQueue;

/**
 * Initialize a priority queue.
 * 
 * \param pq            A pointer to a PriorityQueue instance.
 * \param compareFunc   A comparator function: 
 *                      returns negative if a < b, 
 *                      0 if a == b, 
 *                      positive if a > b.
 *                      The logic here can define min-heap or max-heap behavior.
 * \param initialCapacity The initial capacity for the underlying dynamic array.
 */
void pqInit(PriorityQueue* pq, 
            int (*compareFunc)(const void*, const void*), 
            size_t initialCapacity);

/**
 * Free resources used by the priority queue.
 */
void pqFree(PriorityQueue* pq);

/**
 * Insert a new element into the priority queue.
 * 
 * \param pq        The PriorityQueue.
 * \param data      Pointer to the data to insert.
 * \param dataSize  Size of the data.
 */
void pqPush(PriorityQueue* pq, const void* data, size_t dataSize);

/**
 * Remove the top (highest priority) element from the queue.
 * 
 * \param pq            The PriorityQueue.
 * \param outData       Buffer to store the popped data (can be NULL if not needed).
 * \param outDataSize   Pointer to the size of outData (will be set to the size of
 *                      the popped data on success). Can be NULL if not needed.
 * \return              true if an element was popped, false if queue is empty.
 */
bool pqPop(PriorityQueue* pq, void* outData, size_t* outDataSize);

/**
 * Return (but do not remove) the top (highest priority) element.
 * 
 * \param pq    The PriorityQueue.
 * \return      A pointer to the top element or NULL if empty.
 */
const void* pqTop(const PriorityQueue* pq);

/**
 * Checks if the priority queue is empty.
 */
bool pqIsEmpty(const PriorityQueue* pq);

/**
 * Returns the number of elements in the priority queue.
 */
size_t pqSize(const PriorityQueue* pq);

#ifdef __cplusplus
}
#endif

#endif // PQ_H
