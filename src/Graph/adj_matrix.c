#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "graph.h"        /* or your main header that references createAdjMatrixImpl */
#include "../Queue/queue.h"
#include "../PriorityQueue/pq.h"
#include <float.h> // for DBL_MAX
/* 
 * A simple adjacency matrix implementation:
 *  - 'matrix[i][j] = -1.0' => no edge
 *  - 'matrix[i][j] >= 0.0' => edge of that weight
 *  - vertexData[i] holds the pointer to user data
 *  - 'size' is how many vertices are actually in use
 *  - 'capacity' is the allocated dimension for vertexData[] and matrix[][]
 *  - vertexData[i] is a void* that references the actual data for vertex i.
 *  - matrix[i][j] says whether vertex i is connected to vertex j (and with what weight).
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
static bool   adjMatrixAddVertex      (void* impl, void* data);
static bool   adjMatrixRemoveVertex   (void* impl, const void* data);
static bool   adjMatrixAddEdge        (void* impl, const void* srcData, const void* dstData, double weight);
static bool   adjMatrixRemoveEdge     (void* impl, const void* srcData, const void* dstData);
static int    adjMatrixGetNumVertices (const void* impl);
static int    adjMatrixGetNumEdges    (const void* impl);
static bool   adjMatrixHasEdge        (const void* impl, const void* srcData, const void* dstData, double* outWeight);
static void   adjMatrixPrint          (const void* impl, void (*printData)(const void*));
static void   adjMatrixDestroy        (void* impl);
static void** adjMatrixBFS            (void* impl, const void* startData, int* outCount);
static void** adjMatrixDFS            (void* impl, const void* startData, int* outCount);
static double* adjMatrixDijkstra      (void* impl, const void* startData);

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
    .destroy        = adjMatrixDestroy,
    .bfs            = adjMatrixBFS,
    .dfs            = adjMatrixDFS,
    .dijkstra       = adjMatrixDijkstra
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
    /* Only resize if we've actually reached current capacity. */
    if (impl->size < impl->capacity) {
        return true;  /* no need to resize. */
    }

    int oldCap = impl->capacity;
    int newCap = oldCap * 2;

    /* 1) Expand vertexData to newCap */
    void** newData = (void**)realloc(impl->vertexData, sizeof(void*) * (size_t)newCap);
    if (!newData) {
        return false; /* out of memory, rollback not needed as we haven't modified anything yet */
    }
    impl->vertexData = newData;
    /* Initialize the newly added slots in vertexData to NULL */
    for (int i = oldCap; i < newCap; i++) {
        impl->vertexData[i] = NULL;
    }

    /* 2) Expand the array-of-row-pointers: impl->matrix => newCap */
    double** newMatrix = (double**)realloc(impl->matrix, sizeof(double*) * (size_t)newCap);
    if (!newMatrix) {
        /* We must rollback the vertexData expansion we just did. */
        /* In theory, we could do a shrink of vertexData. 
           Or at least set them back: but we’ll do an even simpler approach 
           since we haven't changed impl->matrix yet. */

        /* Let's do a shrink attempt (not always guaranteed to succeed). */
        void** shrinkData = (void**)realloc(impl->vertexData, sizeof(void*) * (size_t)oldCap);
        if (shrinkData) {
            impl->vertexData = shrinkData;
        }
        /* or set them to the old pointer if we tracked it. For brevity, we do a best effort. */

        return false;
    }
    impl->matrix = newMatrix;

    /* 3) Allocate brand-new rows for indices [oldCap..newCap-1] */
    for (int i = oldCap; i < newCap; i++) {
        impl->matrix[i] = (double*)malloc(sizeof(double) * (size_t)newCap);
        if (!impl->matrix[i]) {
            /* partial rollback: free newly allocated rows so far [oldCap..i-1], 
               then restore old pointer array if possible */
            for (int k = oldCap; k < i; k++) {
                free(impl->matrix[k]);
                impl->matrix[k] = NULL;
            }

            /* Attempt to shrink matrix pointer array back to oldCap (best effort). */
            double** shrinkRows = (double**)realloc(impl->matrix, sizeof(double*) * (size_t)oldCap);
            if (shrinkRows) {
                impl->matrix = shrinkRows;
            }
            /* Also shrink vertexData if possible (again best effort). */
            void** shrinkData = (void**)realloc(impl->vertexData, sizeof(void*) * (size_t)oldCap);
            if (shrinkData) {
                impl->vertexData = shrinkData;
            }

            return false;
        }
        /* Initialize the new row i to -1.0 => no edge. */
        for (int j = 0; j < newCap; j++) {
            impl->matrix[i][j] = -1.0;
        }
    }

    /* 4) Expand existing rows [0..oldCap-1] from oldCap to newCap columns. */
    for (int i = 0; i < oldCap; i++) {
        double* newRow = (double*)realloc(impl->matrix[i], sizeof(double) * (size_t)newCap);
        if (!newRow) {

            /* 4b) free the brand-new rows [oldCap..newCap-1] */
            for (int nr = oldCap; nr < newCap; nr++) {
                free(impl->matrix[nr]);
                impl->matrix[nr] = NULL;
            }
            /* 4c) Attempt to shrink matrix pointer array back to oldCap. 
                   This means we "undo" the step of expanding to newCap pointer slots. */
            double** shrinkRows = (double**)realloc(impl->matrix, sizeof(double*) * (size_t)oldCap);
            if (shrinkRows) {
                impl->matrix = shrinkRows;
            }
            /* 4d) Attempt to revert vertexData to oldCap as well. */
            void** shrinkData = (void**)realloc(impl->vertexData, sizeof(void*) * (size_t)oldCap);
            if (shrinkData) {
                impl->vertexData = shrinkData;
            }

            return false;
        }

        /* If realloc succeeded, store new pointer and init new columns. */
        impl->matrix[i] = newRow;
        for (int j = oldCap; j < newCap; j++) {
            impl->matrix[i][j] = -1.0;
        }
    }

    /* 5) Update capacity and return success. */
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

/* ----------------------------------------------------------------
   BFS and DFS
   We'll use a queue for BFS and a recursive DFS helper.
   ---------------------------------------------------------------- */

static void** adjMatrixBFS(void* _impl, const void* startData, int* outCount) {
    AdjacencyMatrixImpl* impl = (AdjacencyMatrixImpl*)_impl;
    if (!impl || !startData) {
        if (outCount) *outCount = 0;
        return NULL;
    }

    // 1) find startIndex
    int startIndex = -1;
    for (int i = 0; i < impl->size; i++) {
        if (impl->compare(impl->vertexData[i], startData) == 0) {
            startIndex = i;
            break;
        }
    }
    if (startIndex < 0) {
        if (outCount) *outCount = 0;
        return NULL;
    }

    bool* visited = (bool*)calloc((size_t)impl->size, sizeof(bool));
    void** result = (void**)malloc(sizeof(void*) * (size_t)impl->size);
    if (!visited || !result) {
        free(visited); free(result);
        if (outCount) *outCount = 0;
        return NULL;
    }
    int rCount = 0;

    Queue q; 
    queueInit(&q);

    visited[startIndex] = true;
    queueEnqueue(&q, &startIndex, sizeof(int));

    while (!queueIsEmpty(&q)) {
        int front;
        queueDequeue(&q, &front);
        // add to BFS result
        result[rCount++] = impl->vertexData[front];

        // check neighbors by scanning row 'front'
        for (int j = 0; j < impl->size; j++) {
            if (impl->matrix[front][j] >= 0.0 && !visited[j]) {
                visited[j] = true;
                queueEnqueue(&q, &j, sizeof(int));
            }
        }
    }

    queueClear(&q);
    free(visited);

    if (outCount) *outCount = rCount;
    return result;
}

static void adjMatrixDFSHelper(const AdjacencyMatrixImpl* impl,
                               int current,
                               bool* visited,
                               void** result,
                               int* rCount)
{
    visited[current] = true;
    result[(*rCount)++] = impl->vertexData[current];

    // check row 'current' for neighbors
    for (int j = 0; j < impl->size; j++) {
        if (impl->matrix[current][j] >= 0.0 && !visited[j]) {
            adjMatrixDFSHelper(impl, j, visited, result, rCount);
        }
    }
}

static void** adjMatrixDFS(void* _impl, const void* startData, int* outCount) {
    AdjacencyMatrixImpl* impl = (AdjacencyMatrixImpl*)_impl;
    if (!impl || !startData) {
        if (outCount) *outCount = 0;
        return NULL;
    }

    int startIndex = -1;
    for (int i = 0; i < impl->size; i++) {
        if (impl->compare(impl->vertexData[i], startData) == 0) {
            startIndex = i;
            break;
        }
    }
    if (startIndex < 0) {
        if (outCount) *outCount = 0;
        return NULL;
    }

    bool* visited = (bool*)calloc((size_t)impl->size, sizeof(bool));
    void** result = (void**)malloc(sizeof(void*) * (size_t)impl->size);
    if (!visited || !result) {
        free(visited); free(result);
        if (outCount) *outCount = 0;
        return NULL;
    }
    int rCount = 0;

    adjMatrixDFSHelper(impl, startIndex, visited, result, &rCount);

    free(visited);
    if (outCount) *outCount = rCount;
    return result;
}

/* ----------------------------------------------------------------
   Dijkstra's algorithm
   We'll use a priority queue to store vertices to visit.
   We'll store the distance to each vertex in a double array.
   ---------------------------------------------------------------- */
typedef struct {
    int vertex;
    double dist;
} MatDijkstraNode;

static int matDijkstraCompare(const void* a, const void* b) {
    const MatDijkstraNode* da = (const MatDijkstraNode*)a;
    const MatDijkstraNode* db = (const MatDijkstraNode*)b;
    if (da->dist < db->dist) return -1;
    if (da->dist > db->dist) return 1;
    return 0;
}

static double* adjMatrixDijkstra(void* _impl, const void* startData) {
    AdjacencyMatrixImpl* impl = (AdjacencyMatrixImpl*)_impl;
    if (!impl || !startData) return NULL;

    // find startIndex
    int startIndex = -1;
    for (int i = 0; i < impl->size; i++) {
        if (impl->compare(impl->vertexData[i], startData) == 0) {
            startIndex = i;
            break;
        }
    }
    if (startIndex < 0) return NULL;

    double* dist = (double*)malloc(sizeof(double)*impl->size);
    if (!dist) return NULL;
    for (int i = 0; i < impl->size; i++) {
        dist[i] = DBL_MAX;
    }
    dist[startIndex] = 0.0;

    bool* visited = (bool*)calloc((size_t)impl->size, sizeof(bool));
    if (!visited) {
        free(dist);
        return NULL;
    }

    PriorityQueue pq;
    pqInit(&pq, matDijkstraCompare, true, 16);

    MatDijkstraNode startN = { startIndex, 0.0 };
    pqPush(&pq, &startN, sizeof(MatDijkstraNode));

    while (!pqIsEmpty(&pq)) {
        MatDijkstraNode curr;
        size_t cSize = sizeof(MatDijkstraNode);
        if (!pqPop(&pq, &curr, &cSize)) break;

        int u = curr.vertex;
        if (visited[u]) continue;
        visited[u] = true;

        // relax edges from u by scanning row u
        for (int v = 0; v < impl->size; v++) {
            double w = impl->matrix[u][v];
            if (w >= 0.0 && !visited[v]) {  // there's an edge u->v
                double alt = dist[u] + w;
                if (alt < dist[v]) {
                    dist[v] = alt;
                    MatDijkstraNode nd = { v, alt };
                    pqPush(&pq, &nd, sizeof(MatDijkstraNode));
                }
            }
        }
    }

    pqFree(&pq);
    free(visited);
    return dist;
}