/****************************************************************************
 * File: adj_list.c
 * A full adjacency-list implementation of GraphOps for an abstract Graph.
 ****************************************************************************/

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <string.h>
 
 /* 
  * Include your main graph header, which must declare:
  *   typedef struct GraphOps { ... } GraphOps;
  *   The signature of createAdjListImpl(...).
  *   The enum GraphType { ... } with DIRECTED/UNDIRECTED, WEIGHTED/UNWEIGHTED.
  */
 #include "graph.h"       // or your main "graph.h" that references "GraphOps"
 
 /*
  * Each adjacency node:
  *  - destIndex: index of the destination vertex in the vertices[] array
  *  - weight:    edge weight (or 1.0 if unweighted)
  *  - next:      pointer to the next EdgeNode in the list
  */
 typedef struct EdgeNode {
     int destIndex;
     double weight;
     struct EdgeNode* next;
 } EdgeNode;
 
 /*
  * Each vertex in the adjacency-list representation has:
  *  - data: user-supplied pointer (e.g., int*, struct*, etc.)
  *  - edges: a linked list of EdgeNodes
  */
 typedef struct VertexItem {
     void* data;
     EdgeNode* edges;
 } VertexItem;
 
 /*
  * Our "implementation" struct that stores an array of VertexItem plus metadata.
  */
 typedef struct {
     GraphType type;
     int capacity;       /* allocated size of the vertices array */
     int size;           /* current number of vertices in use    */
     VertexItem* vertices;
 
     int  (*compare)(const void*, const void*);
     void (*freeData)(void*);
 } AdjacencyListImpl;
 
 /* Forward-declare the ops table that we'll define at the bottom */
 static const GraphOps adjListOps;
 
 /***************************************************************************
  *                      Factory: createAdjListImpl()
  ***************************************************************************/
 void* createAdjListImpl(GraphType type,
                         int initialCapacity,
                         int  (*compareFunc)(const void*, const void*),
                         void (*freeFunc)(void*),
                         const GraphOps** opsOut)
 {
     AdjacencyListImpl* impl = (AdjacencyListImpl*)malloc(sizeof(AdjacencyListImpl));
     if (!impl) return NULL;
 
     impl->type     = type;
     impl->capacity = (initialCapacity > 0 ? initialCapacity : 4);
     impl->size     = 0;
     impl->compare  = (compareFunc ? compareFunc : NULL);
     impl->freeData = (freeFunc ? freeFunc : free); /* default to free if not provided */
 
     impl->vertices = (VertexItem*)calloc((size_t)impl->capacity, sizeof(VertexItem));
     if (!impl->vertices) {
         free(impl);
         return NULL;
     }
 
     /* Return the function-pointer table to the caller */
     *opsOut = &adjListOps;
     return impl;
 }
 
 /***************************************************************************
  *                          Helper Functions
  ***************************************************************************/
 
 /* 
  * isWeighted / isDirected: 
  * to check if the graph is weighted/directed based on the GraphType enum.
  */
 static bool isWeighted(GraphType t) {
     return (t == GRAPH_UNDIRECTED_WEIGHTED || t == GRAPH_DIRECTED_WEIGHTED);
 }
 static bool isDirected(GraphType t) {
     return (t == GRAPH_DIRECTED_UNWEIGHTED || t == GRAPH_DIRECTED_WEIGHTED);
 }
 
 /*
  * findVertexIndex:
  *   Looks through impl->vertices[0..size-1] and compares data with impl->compare.
  *   Returns the index if found, else -1.
  */
 static int findVertexIndex(const AdjacencyListImpl* impl, const void* data) {
     for (int i = 0; i < impl->size; i++) {
         if (impl->compare(impl->vertices[i].data, data) == 0) {
             return i;
         }
     }
     return -1;
 }
 
 /*
  * resizeIfNeeded:
  *   If size == capacity, we double capacity and reallocate impl->vertices.
  *   We set newly allocated spots to {NULL, NULL}.
  */
 static bool resizeIfNeeded(AdjacencyListImpl* impl) {
     if (impl->size < impl->capacity) {
         return true; /* no need to resize */
     }
     int newCap = impl->capacity * 2;
     VertexItem* newArr = (VertexItem*)realloc(impl->vertices, sizeof(VertexItem) * (size_t)newCap);
     if (!newArr) {
         return false;
     }
     impl->vertices = newArr;
     /* initialize the new portion */
     for (int i = impl->capacity; i < newCap; i++) {
         impl->vertices[i].data  = NULL;
         impl->vertices[i].edges = NULL;
     }
     impl->capacity = newCap;
     return true;
 }
 
 /*
  * freeEdgeList:
  *   Frees all EdgeNode items in a singly linked list.
  */
 static void freeEdgeList(EdgeNode* edge) {
     while (edge) {
         EdgeNode* temp = edge;
         edge = edge->next;
         free(temp);
     }
 }
 
 /*
  * createEdgeNode:
  *   helper to allocate a new EdgeNode with given destIndex, weight, next=NULL.
  */
 static EdgeNode* createEdgeNode(int destIndex, double weight) {
     EdgeNode* e = (EdgeNode*)malloc(sizeof(EdgeNode));
     if (!e) return NULL;
     e->destIndex = destIndex;
     e->weight    = weight;
     e->next      = NULL;
     return e;
 }
 
 /***************************************************************************
  *                  OPS Implementation: addVertex, removeVertex
  ***************************************************************************/
 
 /* -------------------- addVertex -------------------- */
 static bool adjListAddVertex(void* _impl, void* data) {
     AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
     if (!impl || !data) return false;
 
     /* Check if already exists */
     if (findVertexIndex(impl, data) != -1) {
         return false; /* already in the graph */
     }
 
     /* Possibly resize */
     if (!resizeIfNeeded(impl)) {
         return false;
     }
 
     /* Place new vertex at index = impl->size */
     impl->vertices[impl->size].data  = data;
     impl->vertices[impl->size].edges = NULL;
     impl->size++;
     return true;
 }
 
 /* -------------------- removeVertex -------------------- */
 static bool adjListRemoveVertex(void* _impl, const void* data) {
     AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
     if (!impl || !data) return false;
     if (impl->size == 0) return false;
 
     /* 1) Find index */
     int idx = findVertexIndex(impl, data);
     if (idx < 0) {
         return false; /* not found */
     }
 
     /* 2) free this vertex's edges */
     freeEdgeList(impl->vertices[idx].edges);
     impl->vertices[idx].edges = NULL;
 
     /* 3) free the vertex data */
     if (impl->vertices[idx].data && impl->freeData) {
         impl->freeData(impl->vertices[idx].data);
     }
     impl->vertices[idx].data = NULL;
 
     /* 4) Remove references to this vertex from other adjacency lists */
     for (int i = 0; i < impl->size; i++) {
         if (i == idx) continue;
         EdgeNode* prev = NULL;
         EdgeNode* cur  = impl->vertices[i].edges;
         while (cur) {
             if (cur->destIndex == idx) {
                 /* Remove this edgeNode */
                 if (prev) {
                     prev->next = cur->next;
                 } else {
                     impl->vertices[i].edges = cur->next;
                 }
                 free(cur);
                 break; /* remove once if we assume no duplicates */
             }
             prev = cur;
             cur  = cur->next;
         }
     }
 
     /* 5) swap with last vertex if not the last one */
     int lastIndex = impl->size - 1;
     if (idx != lastIndex) {
         /* Move the last vertex into idx */
         impl->vertices[idx].data  = impl->vertices[lastIndex].data;
         impl->vertices[idx].edges = impl->vertices[lastIndex].edges;
 
         /* Fix edges that pointed to lastIndex so they now point to idx */
         for (int i = 0; i < impl->size - 1; i++) {
             EdgeNode* e = impl->vertices[i].edges;
             while (e) {
                 if (e->destIndex == lastIndex) {
                     e->destIndex = idx;
                 }
                 e = e->next;
             }
         }
     }
 
     /* Pop back the last slot */
     impl->vertices[lastIndex].data  = NULL;
     impl->vertices[lastIndex].edges = NULL;
     impl->size--;
 
     return true;
 }
 
 /***************************************************************************
  *                  OPS Implementation: addEdge, removeEdge
  ***************************************************************************/
 
 /* -------------------- addEdge -------------------- */
 static bool adjListAddEdge(void* _impl, const void* srcData, const void* dstData, double weight) {
     AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
     if (!impl || !srcData || !dstData) return false;
 
     int srcIdx = findVertexIndex(impl, srcData);
     int dstIdx = findVertexIndex(impl, dstData);
     if (srcIdx < 0 || dstIdx < 0) {
         return false; /* vertices not found */
     }
 
     /* Weighted or unweighted? */
     double finalWeight = (isWeighted(impl->type) ? weight : 1.0);
 
     /* Check if edge already exists in srcIdx's list. If so, update weight */
     EdgeNode* e = impl->vertices[srcIdx].edges;
     while (e) {
         if (e->destIndex == dstIdx) {
             /* Edge already exists, update weight if needed */
             if (isWeighted(impl->type)) {
                 e->weight = finalWeight;
             }
             /* done */
             return true;
         }
         e = e->next;
     }
 
     /* Otherwise, create a new EdgeNode and insert it at the front (or end) */
     EdgeNode* newE = createEdgeNode(dstIdx, finalWeight);
     if (!newE) return false;
     newE->next = impl->vertices[srcIdx].edges;
     impl->vertices[srcIdx].edges = newE;
 
     /* If undirected, also add the reverse edge */
     if (!isDirected(impl->type)) {
         /* check if reverse edge exists */
         e = impl->vertices[dstIdx].edges;
         while (e) {
             if (e->destIndex == srcIdx) {
                 if (isWeighted(impl->type)) {
                     e->weight = finalWeight;
                 }
                 return true;
             }
             e = e->next;
         }
         /* create reverse */
         EdgeNode* rev = createEdgeNode(srcIdx, finalWeight);
         if (!rev) return false;
         rev->next = impl->vertices[dstIdx].edges;
         impl->vertices[dstIdx].edges = rev;
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
         return false; /* not found */
     }
 
     /* Remove edge from src->dst */
     EdgeNode* prev = NULL;
     EdgeNode* cur  = impl->vertices[srcIdx].edges;
     while (cur) {
         if (cur->destIndex == dstIdx) {
             /* remove this node */
             if (prev) {
                 prev->next = cur->next;
             } else {
                 impl->vertices[srcIdx].edges = cur->next;
             }
             free(cur);
             break;
         }
         prev = cur;
         cur  = cur->next;
     }
 
     /* If undirected, remove reverse edge from dst->src */
     if (!isDirected(impl->type)) {
         prev = NULL;
         cur  = impl->vertices[dstIdx].edges;
         while (cur) {
             if (cur->destIndex == srcIdx) {
                 if (prev) {
                     prev->next = cur->next;
                 } else {
                     impl->vertices[dstIdx].edges = cur->next;
                 }
                 free(cur);
                 break;
             }
             prev = cur;
             cur  = cur->next;
         }
     }
 
     return true;
 }
 
 /***************************************************************************
  *           OPS Implementation: getNumVertices, getNumEdges, hasEdge
  ***************************************************************************/
 
 /* getNumVertices: just return impl->size */
 static int adjListGetNumVertices(const void* _impl) {
     const AdjacencyListImpl* impl = (const AdjacencyListImpl*)_impl;
     if (!impl) return 0;
     return impl->size;
 }
 
 /* getNumEdges: count edges in adjacency. If undirected, each edge is stored twice. */
 static int adjListGetNumEdges(const void* _impl) {
     const AdjacencyListImpl* impl = (const AdjacencyListImpl*)_impl;
     if (!impl) return 0;
 
     bool directed = isDirected(impl->type);
     int count = 0;
     for (int i = 0; i < impl->size; i++) {
         EdgeNode* e = impl->vertices[i].edges;
         while (e) {
             count++;
             e = e->next;
         }
     }
     /* if undirected, each edge appears in adjacency lists of both endpoints */
     if (!directed) {
         count /= 2;
     }
     return count;
 }
 
 /* hasEdge: check adjacency list of srcIdx for an edgeNode with destIndex=dstIdx */
 static bool adjListHasEdge(const void* _impl, const void* srcData, const void* dstData, double* outW) {
     const AdjacencyListImpl* impl = (const AdjacencyListImpl*)_impl;
     if (!impl || !srcData || !dstData) return false;
 
     int srcIdx = findVertexIndex(impl, srcData);
     int dstIdx = findVertexIndex(impl, dstData);
     if (srcIdx < 0 || dstIdx < 0) return false;
 
     EdgeNode* e = impl->vertices[srcIdx].edges;
     while (e) {
         if (e->destIndex == dstIdx) {
             if (outW) {
                 *outW = e->weight;
             }
             return true;
         }
         e = e->next;
     }
     return false;
 }
 
 /***************************************************************************
  *             OPS Implementation: print, destroy
  ***************************************************************************/
 
 /* Print out each vertex and its adjacency list */
 static void adjListPrint(const void* _impl, void (*printData)(const void*)) {
     const AdjacencyListImpl* impl = (const AdjacencyListImpl*)_impl;
     if (!impl) return;
 
     printf("AdjList Graph:\n");
     for (int i = 0; i < impl->size; i++) {
         printf("Vertex %d: ", i);
         if (printData) {
             printData(impl->vertices[i].data);
         }
         printf(" -> ");
         EdgeNode* e = impl->vertices[i].edges;
         while (e) {
             printf("[dest=%d w=%.2f] ", e->destIndex, e->weight);
             e = e->next;
         }
         printf("\n");
     }
     printf("\n");
 }
 
 /* Destroy the entire adjacency-list implementation */
 static void adjListDestroy(void* _impl) {
     AdjacencyListImpl* impl = (AdjacencyListImpl*)_impl;
     if (!impl) return;
 
     /* For each vertex, free all edges, then free the vertex data */
     for (int i = 0; i < impl->size; i++) {
         freeEdgeList(impl->vertices[i].edges);
         impl->vertices[i].edges = NULL;
 
         if (impl->vertices[i].data && impl->freeData) {
             impl->freeData(impl->vertices[i].data);
         }
         impl->vertices[i].data = NULL;
     }
 
     /* free the array of vertices */
     free(impl->vertices);
     impl->vertices = NULL;
 
     /* finally, free the impl struct */
     free(impl);
 }
 
 /***************************************************************************
  *           The function pointer table (GraphOps) for adjacency list
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
     .destroy        = adjListDestroy
 };
 