#include "dynamic_array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// A helper function to resize the capacity if needed.
static void daResizeIfNeeded(DynamicArray* da) {
    if (da->size >= da->capacity) {
        // Increase capacity
        size_t newCapacity = (da->capacity == 0) ? 4 : da->capacity * 2;
        da->elements = realloc(da->elements, newCapacity * sizeof(void*));
        da->elementSizes = realloc(da->elementSizes, newCapacity * sizeof(size_t));
        if (!da->elements || !da->elementSizes) {
            fprintf(stderr, "Failed to reallocate memory in daResizeIfNeeded.\n");
            exit(EXIT_FAILURE);
        }
        da->capacity = newCapacity;
    }
}

void daInit(DynamicArray* da, size_t initialCapacity) {
    da->size = 0;
    da->capacity = (initialCapacity == 0) ? 4 : initialCapacity;
    da->elements = (void**)malloc(da->capacity * sizeof(void*));
    da->elementSizes = (size_t*)malloc(da->capacity * sizeof(size_t));
    if (!da->elements || !da->elementSizes) {
        fprintf(stderr, "Failed to allocate memory in daInit.\n");
        exit(EXIT_FAILURE);
    }
}

void daFree(DynamicArray* da) {
    // Free each element's data
    for (size_t i = 0; i < da->size; i++) {
        free(da->elements[i]);
    }
    // Free the arrays themselves
    free(da->elements);
    free(da->elementSizes);

    // Reset the struct to a safe state
    da->elements = NULL;
    da->elementSizes = NULL;
    da->size = 0;
    da->capacity = 0;
}

size_t daSize(const DynamicArray* da) {
    return da->size;
}

bool daIsEmpty(const DynamicArray* da) {
    return (da->size == 0);
}

void daPushBack(DynamicArray* da, const void* data, size_t dataSize) {
    // Resize if needed
    daResizeIfNeeded(da);

    // Allocate memory for this new element
    da->elements[da->size] = malloc(dataSize);
    if (!da->elements[da->size]) {
        fprintf(stderr, "Failed to allocate memory for new element in daPushBack.\n");
        exit(EXIT_FAILURE);
    }

    // Copy data in
    memcpy(da->elements[da->size], data, dataSize);
    da->elementSizes[da->size] = dataSize;
    da->size++;
}

bool daPopBack(DynamicArray* da, void* outData, size_t* outDataSize) {
    if (daIsEmpty(da)) {
        return false;
    }
    // The last element is at index da->size - 1
    size_t lastIndex = da->size - 1;

    // Copy data out if requested
    if (outData && outDataSize) {
        // Make sure the user-provided buffer is large enough or they can handle it
        // We'll just do a safe memcpy up to the smaller of the two sizes
        size_t copySize = (da->elementSizes[lastIndex] < *outDataSize)
                          ? da->elementSizes[lastIndex]
                          : *outDataSize;
        memcpy(outData, da->elements[lastIndex], copySize);
        // Also return the actual size stored
        *outDataSize = da->elementSizes[lastIndex];
    }

    // Free the last element
    free(da->elements[lastIndex]);

    da->size--;

    return true;
}

const void* daGet(const DynamicArray* da, size_t index) {
    if (index >= da->size) {
        return NULL; // Out of range
    }
    return da->elements[index];
}

void* daGetMutable(DynamicArray* da, size_t index) {
    if (index >= da->size) {
        return NULL;
    }
    return da->elements[index];
}

size_t daGetElementSize(const DynamicArray* da, size_t index) {
    if (index >= da->size) {
        return 0;
    }
    return da->elementSizes[index];
}

void daPrint(const DynamicArray* da, void (*printFunc)(const void*, size_t)) {
    printf("[ ");
    for (size_t i = 0; i < da->size; i++) {
        printFunc(da->elements[i], da->elementSizes[i]);
        if (i < da->size - 1) {
            printf(", ");
        }
    }
    printf(" ]\n");
}
