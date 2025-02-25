/****************************************************************************
 * File: adj_list.c
 * A full adjacency-list implementation using DynamicArray for both vertices
 * and edges. Conforms to a GraphOps interface.
 ****************************************************************************/

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <string.h>
 
 #include "graph.h"          /* Declares GraphOps, GraphType, createAdjListImpl(...) */
 #include "../DynamicArray/dynamic_array.h"  /* Your generic dynamic array interface */
 #include "../Queue/queue.h"
 #include "../PriorityQueue/pq.h" // or whichever min-heap structure you have

 /*
  * We'll store each edge as a small struct with:
  *   - destIndex: index of the destination vertex
  *   - weight: edge weight (or 1.0 for unweighted)
  */
 typedef struct {
     int    destIndex;
     double weight;
 } Edge;
 
 /*
  * Each vertex has:
  *   - data (void* user data)
  *   - a dynamic array of Edge
  */
 typedef struct {
     void*        data;
     DynamicArray edges;  /* array of Edge */
 } VertexItem;
 
 /*
  * The main AdjacencyList "implementation" struct:
  *   - a DynamicArray of VertexItem
  *   - compare, freeData function pointers
  *   - a GraphType to check directed/weighted
  */
 typedef struct {
     GraphType     type;
     DynamicArray  vertices;    /* each element is a VertexItem */
     int  (*compare)(const void*, const void*);
     void (*freeData)(void*);
 } AdjacencyListImpl;
 
 /* Forward-declare the static ops table we'll define at the bottom */
 static const GraphOps adjListOps;
 
 /***************************************************************************
  *               createAdjListImpl() "factory" function
  ***************************************************************************/
 void* createAdjListImpl(GraphType     type,
                         int           initialCapacity,
                         int         (*compareFunc)(const void*, const void*),
                         void        (*freeFunc)(void*),
                         const GraphOps** opsOut)
 {
     AdjacencyListImpl* impl = (AdjacencyListImpl*)malloc(sizeof(AdjacencyListImpl));
     if (!impl) return NULL;
 
     impl->type = type;
     impl->compare  = (compareFunc ? compareFunc : NULL);
     impl->freeData = (freeFunc ? freeFunc : free);  /* default to free if not provided */
 
     /* Initialize the dynamic array of vertices */
     daInit(&impl->vertices, (initialCapacity > 0 ? initialCapacity : 4));
 
     /* Return our function table pointer */
     *opsOut = &adjListOps;
     return impl;
 }
 
 /***************************************************************************
  *                          Helper Functions
  ***************************************************************************/
 
 /* Check if graph is weighted */
 static bool isWeighted(GraphType t) {
     return (t == GRAPH_UNDIRECTED_WEIGHTED || t == GRAPH_DIRECTED_WEIGHTED);
 }
 
 /* Check if graph is directed */
 static bool isDirected(GraphType t) {
     return (t == GRAPH_DIRECTED_UNWEIGHTED || t == GRAPH_DIRECTED_WEIGHTED);
 }
 
 /* 
  * findVertexIndex:
  *   Iterate over impl->vertices and compare "data" using impl->compare.
  *   Return index if found, else -1.
  */
 static int findVertexIndex(const AdjacencyListImpl* impl, const void* data) {
     size_t n = daSize(&impl->vertices);
     for (size_t i = 0; i < n; i++) {
         const VertexItem* v = (const VertexItem*)daGet(&impl->vertices, i);
         if (impl->compare(v->data, data) == 0) {
             return (int)i;
         }
     }
     return -1;
 }
 
 /* 
  * Free a single vertex's edges dynamic array and optionally the data.
  */
 static void freeVertexItem(VertexItem* vertex, void (*freeData)(void*)) {
     if (!vertex) return;
 
     /* free the edges array */
     daFree(&vertex->edges);
 
     /* free the data */
     if (vertex->data && freeData) {
         freeData(vertex->data);
         vertex->data = NULL;
     }
 }
 
 /* 
  * Remove all edges referencing 'victimIndex' from the adjacency of 'otherIndex'.
  */
 static void removeAllReferences(AdjacencyListImpl* impl, int otherIndex, int victimIndex) {
     VertexItem* v = (VertexItem*)daGetMutable(&impl->vertices, (size_t)otherIndex);
     if (!v) return;
 
     size_t ecount = daSize(&v->edges);
     for (size_t j = 0; j < ecount; ) {
         Edge* e = (Edge*)daGetMutable(&v->edges, j);
         if (e->destIndex == victimIndex) {
             /* remove the edge by swap-with-last and pop */
             size_t last = daSize(&v->edges) - 1;
             if (j != last) {
                 Edge* lastEdge = (Edge*)daGetMutable(&v->edges, last);
                 *e = *lastEdge;  /* overwrite current with last */
             }
             daPopBack(&v->edges, NULL, NULL);
             ecount--;
         } else {
             j++;
         }
     }
 }
 
 /***************************************************************************
  *              OPS Implementation: addVertex, removeVertex
  ***************************************************************************/
 
 /* -------------------- addVertex -------------------- */
 static bool adjListAddVertex(void* _impl, void* data) {
     AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
     if (!impl || !data) return false;
 
     /* Check if it already exists */
     if (findVertexIndex(impl, data) != -1) {
         return false; /* duplicate vertex */
     }
 
     /* Create a VertexItem */
     VertexItem v;
     v.data = data;
     daInit(&v.edges, 2);  /* small initial capacity for the adjacency list */
 
     /* Push it into the dynamic array */
     daPushBack(&impl->vertices, &v, sizeof(VertexItem));
     return true;
 }
 
 /* -------------------- removeVertex -------------------- */
 static bool adjListRemoveVertex(void* _impl, const void* data) {
     AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
     if (!impl || !data) return false;
 
     int idx = findVertexIndex(impl, data);
     if (idx < 0) {
         return false; /* not found */
     }
 
     /* 1) Free edges + data in this vertex */
     VertexItem* victim = (VertexItem*)daGetMutable(&impl->vertices, (size_t)idx);
     freeVertexItem(victim, impl->freeData);
 
     /* 2) Remove references to this vertex from all other vertices */
     size_t n = daSize(&impl->vertices);
     for (size_t i = 0; i < n; i++) {
         if ((int)i == idx) continue;
         removeAllReferences(impl, (int)i, idx);
     }
 
     /* 3) "Swap with last" approach to keep contiguous array */
     size_t lastIndex = daSize(&impl->vertices) - 1;
     if ((size_t)idx != lastIndex) {
         /* copy the last vertex into idx */
         VertexItem* lastV = (VertexItem*)daGetMutable(&impl->vertices, lastIndex);
         VertexItem* oldVictim = (VertexItem*)daGetMutable(&impl->vertices, (size_t)idx);
         *oldVictim = *lastV;  /* struct assignment */
 
         /* fix edges that pointed to lastIndex => now point to idx */
         size_t totalV = daSize(&impl->vertices);
         for (size_t i = 0; i < totalV - 1; i++) {
             VertexItem* v = (VertexItem*)daGetMutable(&impl->vertices, i);
             size_t ecount = daSize(&v->edges);
             for (size_t j = 0; j < ecount; j++) {
                 Edge* e = (Edge*)daGetMutable(&v->edges, j);
                 if (e->destIndex == (int)lastIndex) {
                     e->destIndex = idx;
                 }
             }
         }
     }
 
     /* 4) Pop back the last slot */
     daPopBack(&impl->vertices, NULL, NULL);
 
     return true;
 }
 
 /***************************************************************************
  *              OPS Implementation: addEdge, removeEdge
  ***************************************************************************/
 
 /* -------------------- addEdge -------------------- */
 static bool adjListAddEdge(void* _impl, const void* srcData, const void* dstData, double weight) {
     AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
     if (!impl || !srcData || !dstData) return false;
 
     int srcIdx = findVertexIndex(impl, srcData);
     int dstIdx = findVertexIndex(impl, dstData);
     if (srcIdx < 0 || dstIdx < 0) {
         return false; /* either vertex not found */
     }
 
     double finalW = isWeighted(impl->type) ? weight : 1.0;
 
     /* Insert edge into srcIdx's adjacency if not already present */
     VertexItem* srcV = (VertexItem*)daGetMutable(&impl->vertices, (size_t)srcIdx);
 
     /* Check if edge exists */
     size_t ecount = daSize(&srcV->edges);
     for (size_t i = 0; i < ecount; i++) {
         Edge* e = (Edge*)daGetMutable(&srcV->edges, i);
         if (e->destIndex == dstIdx) {
             /* already exists, update weight if weighted */
             if (isWeighted(impl->type)) {
                 e->weight = finalW;
             }
             return true;
         }
     }
     /* add a new edge */
     Edge newEdge;
     newEdge.destIndex = dstIdx;
     newEdge.weight    = finalW;
     daPushBack(&srcV->edges, &newEdge, sizeof(Edge));
 
     /* If undirected, add reverse edge too */
     if (!isDirected(impl->type)) {
         VertexItem* dstV = (VertexItem*)daGetMutable(&impl->vertices, (size_t)dstIdx);
 
         /* Check if reverse edge exists */
         size_t dcount = daSize(&dstV->edges);
         for (size_t i = 0; i < dcount; i++) {
             Edge* e = (Edge*)daGetMutable(&dstV->edges, i);
             if (e->destIndex == srcIdx) {
                 if (isWeighted(impl->type)) {
                     e->weight = finalW;
                 }
                 return true;
             }
         }
         /* add reverse edge */
         Edge rev;
         rev.destIndex = srcIdx;
         rev.weight    = finalW;
         daPushBack(&dstV->edges, &rev, sizeof(Edge));
     }
 
     return true;
 }
 
 /* -------------------- removeEdge -------------------- */
 static bool adjListRemoveEdge(void* _impl, const void* srcData, const void* dstData) {
     AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
     if (!impl || !srcData || !dstData) return false;
 
     int srcIdx = findVertexIndex(impl, srcData);
     int dstIdx = findVertexIndex(impl, dstData);
     if (srcIdx < 0 || dstIdx < 0) {
         return false;
     }
 
     /* Remove edge from srcIdx->dstIdx */
     /*Looping is for the potential case where we have parallel edges with the same source and destination*/
     {
         VertexItem* srcV = (VertexItem*)daGetMutable(&impl->vertices, (size_t)srcIdx);
         size_t ecount = daSize(&srcV->edges);
         for (size_t i = 0; i < ecount; ) {
             Edge* e = (Edge*)daGetMutable(&srcV->edges, i);
             if (e->destIndex == dstIdx) {
                 /* remove it by swap-with-last */
                 size_t last = daSize(&srcV->edges) - 1;
                 if (i != last) {
                     Edge* lastE = (Edge*)daGetMutable(&srcV->edges, last);
                     *e = *lastE;
                 }
                 daPopBack(&srcV->edges, NULL, NULL);
                 ecount--;
             } else {
                 i++;
             }
         }
     }
     
      /*Looping is for the potential case where we have parallel edges with the same source and destination*/
     /* If undirected, remove reverse edge from dstIdx->srcIdx */
     if (!isDirected(impl->type)) {
         VertexItem* dstV = (VertexItem*)daGetMutable(&impl->vertices, (size_t)dstIdx);
         size_t dcount = daSize(&dstV->edges);
         for (size_t i = 0; i < dcount; ) {
             Edge* e = (Edge*)daGetMutable(&dstV->edges, i);
             if (e->destIndex == srcIdx) {
                 /* remove */
                 size_t last = daSize(&dstV->edges) - 1;
                 if (i != last) {
                     Edge* lastE = (Edge*)daGetMutable(&dstV->edges, last);
                     *e = *lastE;
                 }
                 daPopBack(&dstV->edges, NULL, NULL);
                 dcount--;
             } else {
                 i++;
             }
         }
     }
 
     return true;
 }
 
 /***************************************************************************
  *        OPS Implementation: getNumVertices, getNumEdges, hasEdge
  ***************************************************************************/
 
 /* getNumVertices: simply the size of impl->vertices */
 static int adjListGetNumVertices(const void* _impl) {
     const AdjacencyListImpl* impl = (const AdjacencyListImpl*)_impl;
     if (!impl) return 0;
     return (int)daSize(&impl->vertices);
 }
 
 /* getNumEdges: sum the adjacency sizes. If undirected, each edge is stored twice. */
 static int adjListGetNumEdges(const void* _impl) {
     const AdjacencyListImpl* impl = (const AdjacencyListImpl*)_impl;
     if (!impl) return 0;
 
     bool directed = isDirected(impl->type);
     int count = 0;
 
     size_t vcount = daSize(&impl->vertices);
     for (size_t i = 0; i < vcount; i++) {
         const VertexItem* v = (const VertexItem*)daGet(&impl->vertices, i);
         count += (int)daSize(&v->edges);
     }
 
     if (!directed) {
         count /= 2;  /* each edge appears in two adjacency arrays */
     }
     return count;
 }
 
 /* hasEdge: check if src->dst is present. If outWeight != NULL, store the edge's weight. */
 static bool adjListHasEdge(const void* _impl, const void* srcData, const void* dstData, double* outW) {
     const AdjacencyListImpl* impl = (const AdjacencyListImpl*)_impl;
     if (!impl || !srcData || !dstData) return false;
 
     int srcIdx = findVertexIndex(impl, srcData);
     int dstIdx = findVertexIndex(impl, dstData);
     if (srcIdx < 0 || dstIdx < 0) return false;
 
     const VertexItem* srcV = (const VertexItem*)daGet(&impl->vertices, (size_t)srcIdx);
     size_t ecount = daSize(&srcV->edges);
     for (size_t i = 0; i < ecount; i++) {
         const Edge* e = (const Edge*)daGet(&srcV->edges, i);
         if (e->destIndex == dstIdx) {
             if (outW) {
                 *outW = e->weight;
             }
             return true;
         }
     }
     return false;
 }
 
 /***************************************************************************
  *                OPS Implementation: print, destroy
  ***************************************************************************/
 
 /* print: enumerates all vertices and their adjacency dynamic array */
 static void adjListPrint(const void* _impl, void (*printData)(const void*)) {
     const AdjacencyListImpl* impl = (const AdjacencyListImpl*)_impl;
     if (!impl) return;
 
     printf("AdjList (DynamicArray-based) Graph:\n");
     size_t n = daSize(&impl->vertices);
     for (size_t i = 0; i < n; i++) {
         const VertexItem* v = (const VertexItem*)daGet(&impl->vertices, i);
         printf("Vertex %zu: ", i);
         if (printData) {
             printData(v->data);
         }
         printf(" -> ");
         size_t ecount = daSize(&v->edges);
         for (size_t j = 0; j < ecount; j++) {
             const Edge* e = (const Edge*)daGet(&v->edges, j);
             printf("[dest=%d w=%.2f] ", e->destIndex, e->weight);
         }
         printf("\n");
     }
     printf("\n");
 }
 
 /* destroy: free all vertices' edges + data, then free the dynamic array of vertices, finally free impl */
 static void adjListDestroy(void* _impl) {
     AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
     if (!impl) return;
 
     size_t n = daSize(&impl->vertices);
     for (size_t i = 0; i < n; i++) {
         VertexItem* v = (VertexItem*)daGetMutable(&impl->vertices, i);
         /* free each vertex's edges array, free the data if needed */
         daFree(&v->edges);
         if (v->data && impl->freeData) {
             impl->freeData(v->data);
             v->data = NULL;
         }
     }
     daFree(&impl->vertices);
 
     free(impl);
 }
 
/***************************************************************************
*                OPS Implementation: BFS, DFS, Dijkstra
***************************************************************************/



 static void** adjListBFS(void* _impl, const void* startData, int* outCount) {
    AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
    if (!impl || !startData) {
        if (outCount) *outCount = 0;
        return NULL;
    }

    // 1) find startIndex
    int startIndex = -1;
    size_t n = daSize(&impl->vertices);
    for (size_t i = 0; i < n; i++) {
        const VertexItem* v = (const VertexItem*)daGet(&impl->vertices, i);
        if (impl->compare(v->data, startData) == 0) {
            startIndex = (int)i;
            break;
        }
    }
    if (startIndex < 0) {
        // not found
        if (outCount) *outCount = 0;
        return NULL;
    }

    // 2) typical BFS
    bool* visited = (bool*)calloc(n, sizeof(bool));
    if (!visited) {
        if (outCount) *outCount = 0;
        return NULL;
    }
    void** result = (void**)malloc(sizeof(void*) * n);
    if (!result) {
        free(visited);
        if (outCount) *outCount = 0;
        return NULL;
    }
    int rCount = 0;

    Queue q; // assume you have queueInit, queueEnqueue, etc.
    queueInit(&q);

    // enqueue startIndex
    visited[startIndex] = true;
    queueEnqueue(&q, &startIndex, sizeof(int));

    while (!queueIsEmpty(&q)) {
        int front;
        queueDequeue(&q, &front);

        // add to BFS result
        const VertexItem* v = (const VertexItem*)daGet(&impl->vertices, (size_t)front);
        result[rCount++] = v->data;

        // for each neighbor
        size_t ecount = daSize(&v->edges);
        for (size_t i = 0; i < ecount; i++) {
            const Edge* e = (const Edge*)daGet(&v->edges, i);
            int nbr = e->destIndex;
            if (!visited[nbr]) {
                visited[nbr] = true;
                queueEnqueue(&q, &nbr, sizeof(int));
            }
        }
    }

    queueClear(&q);
    free(visited);

    if (outCount) *outCount = rCount;
    return result;
}

static void dfsHelper(const AdjacencyListImpl* impl,
                      int currentIndex,
                      bool* visited,
                      void** result,
                      int* rCount) 
{
    visited[currentIndex] = true;
    const VertexItem* v = (const VertexItem*)daGet(&impl->vertices, (size_t)currentIndex);
    result[(*rCount)++] = v->data;

    // neighbors
    size_t ecount = daSize(&v->edges);
    for (size_t i = 0; i < ecount; i++) {
        const Edge* e = (const Edge*)daGet(&v->edges, i);
        int nbr = e->destIndex;
        if (!visited[nbr]) {
            dfsHelper(impl, nbr, visited, result, rCount);
        }
    }
}

static void** adjListDFS(void* _impl, const void* startData, int* outCount) {
    AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
    if (!impl || !startData) {
        if (outCount) *outCount = 0;
        return NULL;
    }

    // find startIndex
    int startIndex = -1;
    size_t n = daSize(&impl->vertices);
    for (size_t i = 0; i < n; i++) {
        const VertexItem* v = (const VertexItem*)daGet(&impl->vertices, i);
        if (impl->compare(v->data, startData) == 0) {
            startIndex = (int)i;
            break;
        }
    }
    if (startIndex < 0) {
        if (outCount) *outCount = 0;
        return NULL;
    }

    bool* visited = (bool*)calloc(n, sizeof(bool));
    if (!visited) {
        if (outCount) *outCount = 0;
        return NULL;
    }
    void** result = (void**)malloc(sizeof(void*) * n);
    if (!result) {
        free(visited);
        if (outCount) *outCount = 0;
        return NULL;
    }
    int rCount = 0;

    dfsHelper(impl, startIndex, visited, result, &rCount);

    free(visited);
    if (outCount) *outCount = rCount;
    return result;
}

typedef struct {
    int vertexIndex;
    double distance;
} DijkstraNode;

static int dijkstraNodeCompare(const void* a, const void* b) {
    const DijkstraNode* da = (const DijkstraNode*)a;
    const DijkstraNode* db = (const DijkstraNode*)b;
    if (da->distance < db->distance) return -1;
    else if (da->distance > db->distance) return 1;
    return 0;
}

static double* adjListDijkstra(void* _impl, const void* startData) {
    AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
    if (!impl || !startData) return NULL;

    // 1) find startIndex
    int startIndex = -1;
    size_t n = daSize(&impl->vertices);
    for (size_t i = 0; i < n; i++) {
        const VertexItem* v = (const VertexItem*)daGet(&impl->vertices, i);
        if (impl->compare(v->data, startData) == 0) {
            startIndex = (int)i;
            break;
        }
    }
    if (startIndex < 0) return NULL;

    // 2) dist array
    double* dist = (double*)malloc(sizeof(double)*n);
    if (!dist) return NULL;
    for (size_t i = 0; i < n; i++) {
        dist[i] = 1e15; // large number or DBL_MAX
    }
    dist[startIndex] = 0.0;

    bool* visited = (bool*)calloc(n, sizeof(bool));
    if (!visited) {
        free(dist);
        return NULL;
    }

    PriorityQueue pq;
    pqInit(&pq, dijkstraNodeCompare, true, 16); // min-heap

    DijkstraNode startNode = { startIndex, 0.0 };
    pqPush(&pq, &startNode, sizeof(DijkstraNode));

    while (!pqIsEmpty(&pq)) {
        DijkstraNode current;
        size_t cSize = sizeof(DijkstraNode);
        bool ok = pqPop(&pq, &current, &cSize);
        if (!ok) break;

        int u = current.vertexIndex;
        if (visited[u]) continue;
        visited[u] = true;

        // relax edges from u
        const VertexItem* v = (const VertexItem*)daGet(&impl->vertices, (size_t)u);
        size_t ecount = daSize(&v->edges);
        for (size_t i = 0; i < ecount; i++) {
            const Edge* e = (const Edge*)daGet(&v->edges, i);
            int nbr = e->destIndex;
            double w = e->weight; // assume nonnegative
            if (!visited[nbr]) {
                double alt = dist[u] + w;
                if (alt < dist[nbr]) {
                    dist[nbr] = alt;
                    DijkstraNode nd = { nbr, alt };
                    pqPush(&pq, &nd, sizeof(DijkstraNode));
                }
            }
        }
    }

    pqFree(&pq);
    free(visited);
    return dist;
}



 /***************************************************************************
  *         The function pointer table (GraphOps) for adjacency list
  ***************************************************************************/
 static const GraphOps adjListOps = {
     .addVertex      = adjListAddVertex,
     .removeVertex   = adjListRemoveVertex,
     .addEdge        = adjListAddEdge,
     .removeEdge     = adjListRemoveEdge,
     .getNumVertices = adjListGetNumVertices,
     .getNumEdges    = adjListGetNumEdges,
     .hasEdge        = adjListHasEdge,
     .print          = adjListPrint,
     .destroy        = adjListDestroy,
     .bfs            = adjListBFS,
     .dfs            = adjListDFS,
     .dijkstra       = adjListDijkstra
 };
 