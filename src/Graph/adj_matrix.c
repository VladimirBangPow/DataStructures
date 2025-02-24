#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "graph.h"        /* or your main header that references createAdjMatrixImpl */

/* 
 * A simple adjacency matrix implementation:
 *  - 'matrix[i][j] = -1.0' => no edge
 *  - 'matrix[i][j] >= 0.0' => edge of that weight
 *  - vertexData[i] holds the pointer to user data
 *  - 'size' is how many vertices are actually in use
 *  - 'capacity' is the allocated dimension for vertexData[] and matrix[][]
 */
typedef struct {
    GraphType   type;
    int         capacity;
    int         size;

    void**      vertexData; /* array of length 'capacity', each is user data for that vertex */
    double**    matrix;     /* 2D array [capacity][capacity], -1.0 => no edge */
    
    int  (*compare)(const void*, const void*);
    void (*freeData)(void*);
} AdjacencyMatrixImpl;


/* ----------------------------------------------------------------
   Helpers to check if the graph is weighted or directed
   (you can also do these checks inline if you prefer).
   ---------------------------------------------------------------- */
static bool isWeighted(GraphType t) {
    return (t == GRAPH_UNDIRECTED_WEIGHTED || t == GRAPH_DIRECTED_WEIGHTED);
}
static bool isDirected(GraphType t) {
    return (t == GRAPH_DIRECTED_UNWEIGHTED || t == GRAPH_DIRECTED_WEIGHTED);
}

/* ----------------------------------------------------------------
   Local forward declarations of our function pointers 
   that match the GraphOps structure.
   ---------------------------------------------------------------- */
static bool   adjMatrixAddVertex    (void* impl, void* data);
static bool   adjMatrixRemoveVertex (void* impl, const void* data);
static bool   adjMatrixAddEdge      (void* impl, const void* srcData, const void* dstData, double weight);
static bool   adjMatrixRemoveEdge   (void* impl, const void* srcData, const void* dstData);
static int    adjMatrixGetNumVertices (const void* impl);
static int    adjMatrixGetNumEdges    (const void* impl);
static bool   adjMatrixHasEdge      (const void* impl, const void* srcData, const void* dstData, double* outWeight);
static void   adjMatrixPrint        (const void* impl, void (*printData)(const void*));
static void   adjMatrixDestroy      (void* impl);

/* The function-pointer table for adjacency matrix */
static const GraphOps adjMatrixOps = {
    .addVertex      = adjMatrixAddVertex,
    .removeVertex   = adjMatrixRemoveVertex,
    .addEdge        = adjMatrixAddEdge,
    .removeEdge     = adjMatrixRemoveEdge,
    .getNumVertices = adjMatrixGetNumVertices,
    .getNumEdges    = adjMatrixGetNumEdges,
    .hasEdge        = adjMatrixHasEdge,
    .print          = adjMatrixPrint,
    .destroy        = adjMatrixDestroy
};


/* ----------------------------------------------------------------
   This is the "factory" function that graph.c (or your main code)
   calls to create the adjacency-matrix-based internal structure.
   'opsOut' is a pointer to a const GraphOps* that we fill with adjMatrixOps
   ---------------------------------------------------------------- */
void* createAdjMatrixImpl(GraphType    type,
                          int          initialCapacity,
                          int        (*compareFunc)(const void*, const void*),
                          void       (*freeFunc)(void*),
                          const GraphOps** opsOut)
{
    AdjacencyMatrixImpl* impl = (AdjacencyMatrixImpl*)malloc(sizeof(AdjacencyMatrixImpl));
    if (!impl) return NULL;

    impl->type     = type;
    impl->capacity = (initialCapacity > 0 ? initialCapacity : 4);
    impl->size     = 0;
    impl->compare  = compareFunc;
    impl->freeData = (freeFunc ? freeFunc : free);

    /* Allocate the vertexData array */
    impl->vertexData = (void**)calloc((size_t)impl->capacity, sizeof(void*));
    if (!impl->vertexData) {
        free(impl);
        return NULL;
    }

    /* Allocate the matrix (capacity x capacity) */
    impl->matrix = (double**)malloc(sizeof(double*) * (size_t)impl->capacity);
    if (!impl->matrix) {
        free(impl->vertexData);
        free(impl);
        return NULL;
    }

    for (int i = 0; i < impl->capacity; i++) {
        impl->matrix[i] = (double*)malloc(sizeof(double) * (size_t)impl->capacity);
        if (!impl->matrix[i]) {
            /* free partial allocations */
            for (int k = 0; k < i; k++) {
                free(impl->matrix[k]);
            }
            free(impl->matrix);
            free(impl->vertexData);
            free(impl);
            return NULL;
        }
        /* Initialize row i with -1.0 => no edge */
        for (int j = 0; j < impl->capacity; j++) {
            impl->matrix[i][j] = -1.0;
        }
    }

    /* Return our function table */
    *opsOut = &adjMatrixOps;
    return impl;
}

/* ----------------------------------------------------------------
    Helper: find the index of a vertex by user data 
    returns -1 if not found
   ---------------------------------------------------------------- */
static int findVertexIndex(const AdjacencyMatrixImpl* impl, const void* data) {
    if (!impl || !data) return -1;
    for (int i = 0; i < impl->size; i++) {
        if (impl->compare(impl->vertexData[i], data) == 0) {
            return i;
        }
    }
    return -1;
}

/* ----------------------------------------------------------------
   Helper: expand capacity if needed
   We'll double the capacity, then reallocate:
     - vertexData
     - matrix (each row, plus new rows)
   ---------------------------------------------------------------- */
static bool resizeMatrix(AdjacencyMatrixImpl* impl) {
    if (impl->size < impl->capacity) {
        return true;  /* no need to resize yet */
    }
    int oldCap = impl->capacity;
    int newCap = oldCap * 2;

    /* Resize vertexData array */
    void** newData = (void**)realloc(impl->vertexData, sizeof(void*) * (size_t)newCap);
    if (!newData) return false;
    impl->vertexData = newData;
    /* initialize the new portion to NULL */
    for (int i = oldCap; i < newCap; i++) {
        impl->vertexData[i] = NULL;
    }

    /* Expand matrix pointer array */
    double** newMatrix = (double**)realloc(impl->matrix, sizeof(double*) * (size_t)newCap);
    if (!newMatrix) return false;
    impl->matrix = newMatrix;

    /* For newly added rows, allocate them and init to -1 */
    for (int i = oldCap; i < newCap; i++) {
        impl->matrix[i] = (double*)malloc(sizeof(double) * (size_t)newCap);
        if (!impl->matrix[i]) {
            // roll back partial
            for (int k = oldCap; k < i; k++) {
                free(impl->matrix[k]);
                impl->matrix[k] = NULL;
            }
            return false;
        }
        /* init row i to -1.0 */
        for (int j = 0; j < newCap; j++) {
            impl->matrix[i][j] = -1.0;
        }
    }

    /* Now we must expand each existing row from oldCap to newCap columns */
    for (int i = 0; i < oldCap; i++) {
        double* newRow = (double*)realloc(impl->matrix[i], sizeof(double) * (size_t)newCap);
        if (!newRow) {
            // rollback would be tricky; consider it an error 
            // or you'd do a 2-phase approach
            return false;
        }
        impl->matrix[i] = newRow;
        /* init new columns to -1 */
        for (int j = oldCap; j < newCap; j++) {
            impl->matrix[i][j] = -1.0;
        }
    }

    impl->capacity = newCap;
    return true;
}

/* ----------------------------------------------------------------
   freeMatrix: used in destroy
   ---------------------------------------------------------------- */
static void freeMatrix(AdjacencyMatrixImpl* impl) {
    if (!impl->matrix) return;
    for (int i = 0; i < impl->capacity; i++) {
        free(impl->matrix[i]);
    }
    free(impl->matrix);
    impl->matrix = NULL;
}

/* ----------------------------------------------------------------
   Implementation of addVertex
   1) check if data already present
   2) resize if needed
   3) store data in vertexData[size], size++
   ---------------------------------------------------------------- */
static bool adjMatrixAddVertex(void* _impl, void* data) {
    AdjacencyMatrixImpl* impl = (AdjacencyMatrixImpl*)_impl;
    if (!impl || !data) return false;

    /* Check if it already exists */
    if (findVertexIndex(impl, data) != -1) {
        return false; /* already in the graph */
    }

    /* Possibly resize matrix if size >= capacity */
    if (!resizeMatrix(impl)) return false;

    /* Insert at index = impl->size */
    impl->vertexData[impl->size] = data;
    impl->size++;

    return true;
}

/* ----------------------------------------------------------------
   removeVertex
   We'll:
   1) find the index
   2) free the data if needed
   3) shift the last vertex into this slot
   4) fix the matrix rows/cols
   5) reduce impl->size
   ---------------------------------------------------------------- */
static bool adjMatrixRemoveVertex(void* _impl, const void* data) {
    AdjacencyMatrixImpl* impl = (AdjacencyMatrixImpl*)_impl;
    if (!impl || !data) return false;
    if (impl->size == 0) return false;

    int idx = findVertexIndex(impl, data);
    if (idx < 0) {
        return false; /* not found */
    }

    /* free user data if needed */
    if (impl->vertexData[idx] && impl->freeData) {
        impl->freeData(impl->vertexData[idx]);
    }
    impl->vertexData[idx] = NULL;

    int lastIndex = impl->size - 1;
    if (idx != lastIndex) {
        /* Move last vertex's data into idx */
        impl->vertexData[idx] = impl->vertexData[lastIndex];
        impl->vertexData[lastIndex] = NULL;

        /* Now we must fix the matrix for row/col idx and lastIndex. 
         * We'll copy row lastIndex -> row idx
         * We'll copy col lastIndex -> col idx
         */
        for (int j = 0; j < impl->size; j++) {
            // row idx = row last
            impl->matrix[idx][j] = impl->matrix[lastIndex][j];
            // col idx = col last
            impl->matrix[j][idx] = impl->matrix[j][lastIndex];
        }
    }

    /* set the last row/column to -1 (or zero out) for cleanliness */
    for (int j = 0; j < impl->size; j++) {
        impl->matrix[lastIndex][j] = -1.0;
        impl->matrix[j][lastIndex] = -1.0;
    }

    impl->size--;
    return true;
}

/* ----------------------------------------------------------------
   addEdge
   1) find srcIndex, dstIndex
   2) set matrix[src][dst] = weight
   3) if undirected => matrix[dst][src] = weight
   For unweighted => store 1.0
   ---------------------------------------------------------------- */
static bool adjMatrixAddEdge(void* _impl, const void* srcData, const void* dstData, double weight) {
    AdjacencyMatrixImpl* impl = (AdjacencyMatrixImpl*)_impl;
    if (!impl || !srcData || !dstData) return false;

    int srcIdx = findVertexIndex(impl, srcData);
    int dstIdx = findVertexIndex(impl, dstData);
    if (srcIdx < 0 || dstIdx < 0) {
        return false; /* vertices not found */
    }

    double finalWeight = (isWeighted(impl->type) ? weight : 1.0);

    impl->matrix[srcIdx][dstIdx] = finalWeight;
    if (!isDirected(impl->type)) {
        impl->matrix[dstIdx][srcIdx] = finalWeight;
    }

    return true;
}

/* ----------------------------------------------------------------
   removeEdge
   1) find srcIndex, dstIndex
   2) set matrix[src][dst] = -1
   3) if undirected => matrix[dst][src] = -1
   ---------------------------------------------------------------- */
static bool adjMatrixRemoveEdge(void* _impl, const void* srcData, const void* dstData) {
    AdjacencyMatrixImpl* impl = (AdjacencyMatrixImpl*)_impl;
    if (!impl || !srcData || !dstData) return false;

    int srcIdx = findVertexIndex(impl, srcData);
    int dstIdx = findVertexIndex(impl, dstData);
    if (srcIdx < 0 || dstIdx < 0) {
        return false; 
    }

    impl->matrix[srcIdx][dstIdx] = -1.0;
    if (!isDirected(impl->type)) {
        impl->matrix[dstIdx][srcIdx] = -1.0;
    }
    return true;
}

/* ----------------------------------------------------------------
   getNumVertices
   simply return impl->size
   ---------------------------------------------------------------- */
static int adjMatrixGetNumVertices(const void* _impl) {
    const AdjacencyMatrixImpl* impl = (const AdjacencyMatrixImpl*)_impl;
    if (!impl) return 0;
    return impl->size;
}

/* ----------------------------------------------------------------
   getNumEdges
   We'll scan the upper or entire matrix. 
   For directed => we count each cell != -1. 
   For undirected => each edge is stored twice, so we can either count half or only do upper triangular.
   ---------------------------------------------------------------- */
static int adjMatrixGetNumEdges(const void* _impl) {
    const AdjacencyMatrixImpl* impl = (const AdjacencyMatrixImpl*)_impl;
    if (!impl) return 0;

    bool directed = isDirected(impl->type);
    int count = 0;

    if (directed) {
        /* Count all cells that != -1.0 */
        for (int i = 0; i < impl->size; i++) {
            for (int j = 0; j < impl->size; j++) {
                if (impl->matrix[i][j] >= 0.0) {
                    count++;
                }
            }
        }
    } else {
        /* Undirected => each edge appears in matrix[i][j] and matrix[j][i].
         * We only count i < j or i > j to avoid double counting
         */
        for (int i = 0; i < impl->size; i++) {
            for (int j = i+1; j < impl->size; j++) {
                if (impl->matrix[i][j] >= 0.0) {
                    count++;
                }
            }
        }
    }
    return count;
}

/* ----------------------------------------------------------------
   hasEdge
   1) find srcIndex, dstIndex
   2) if matrix[srcIdx][dstIdx] != -1 => edge
   3) if outWeight != NULL, store the weight
   ---------------------------------------------------------------- */
static bool adjMatrixHasEdge(const void* _impl, const void* srcData, const void* dstData, double* outWeight) {
    const AdjacencyMatrixImpl* impl = (const AdjacencyMatrixImpl*)_impl;
    if (!impl || !srcData || !dstData) return false;

    int srcIdx = findVertexIndex(impl, srcData);
    int dstIdx = findVertexIndex(impl, dstData);
    if (srcIdx < 0 || dstIdx < 0) return false;

    double val = impl->matrix[srcIdx][dstIdx];
    if (val >= 0.0) {
        if (outWeight) {
            *outWeight = val;
        }
        return true;
    }
    return false;
}

/* ----------------------------------------------------------------
   print
   We'll print a row for each vertex. 
   Also print edges for debugging.
   ---------------------------------------------------------------- */
static void adjMatrixPrint(const void* _impl, void (*printData)(const void*)) {
    const AdjacencyMatrixImpl* impl = (const AdjacencyMatrixImpl*)_impl;
    if (!impl) return;

    printf("AdjMatrix Graph:\n");
    for (int i = 0; i < impl->size; i++) {
        printf("Vertex %d: ", i);
        if (printData) {
            printData(impl->vertexData[i]);
        }
        printf(" -> ");
        for (int j = 0; j < impl->size; j++) {
            double w = impl->matrix[i][j];
            if (w >= 0.0) {
                printf("[dest=%d w=%.2f] ", j, w);
            }
        }
        printf("\n");
    }
    printf("\n");
}

/* ----------------------------------------------------------------
   destroy
   free all user data, free matrix, free everything
   note that we do not individually free user data for all 
   vertices if removeVertex already freed them. But typically 
   we do it once if the user hasn't removed them all.
   ---------------------------------------------------------------- */
static void adjMatrixDestroy(void* _impl) {
    AdjacencyMatrixImpl* impl = (AdjacencyMatrixImpl*)_impl;
    if (!impl) return;

    /* free all user data */
    for (int i = 0; i < impl->size; i++) {
        if (impl->vertexData[i] && impl->freeData) {
            impl->freeData(impl->vertexData[i]);
        }
        impl->vertexData[i] = NULL;
    }
    free(impl->vertexData);

    /* free matrix rows & the array of pointers */
    freeMatrix(impl);

    free(impl);
}

