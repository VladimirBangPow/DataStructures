#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stddef.h>  // for size_t
#include <stdbool.h> // for bool

// A generic dynamic array which can store elements of arbitrary type.
// Internally, it stores an array of pointers to element data, plus each element's size.
typedef struct {
    void**  elements;    // Array of pointers to each element's data
    size_t* elementSizes; // Array of sizes (in bytes) for each element
    size_t  size;        // Number of elements currently stored
    size_t  capacity;    // Allocated capacity (in number of elements)
} DynamicArray;

// Initialize a dynamic array, setting initial capacity to something like 4 or 8.
void daInit(DynamicArray* da, size_t initialCapacity);

// Free all resources used by the dynamic array.
void daFree(DynamicArray* da);

// Get the current number of elements stored in the array.
size_t daSize(const DynamicArray* da);

// Check if the dynamic array is empty.
bool daIsEmpty(const DynamicArray* da);

// Add an element to the end (push back). The data is copied internally.
void daPushBack(DynamicArray* da, const void* data, size_t dataSize);

// Remove the last element (pop back). Returns true on success, false if empty.
// The removed data is copied into outData if outData is non-NULL.
bool daPopBack(DynamicArray* da, void* outData, size_t* outDataSize);

// Retrieve a pointer to the element at a given index (for read-only).
// Returns NULL if index is out of range.
const void* daGet(const DynamicArray* da, size_t index);

// Retrieve a pointer to the element at a given index (for read-write).
// Returns NULL if index is out of range.
void* daGetMutable(DynamicArray* da, size_t index);

// Get the size (in bytes) of the element at a given index.
// Returns 0 if index is out of range.
size_t daGetElementSize(const DynamicArray* da, size_t index);

// Print the contents of the array using a user-provided print function that knows how to handle one element of data.
void daPrint(const DynamicArray* da, void (*printFunc)(const void*, size_t));

#endif // DYNAMIC_ARRAY_H
