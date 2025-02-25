#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <stddef.h>

/* Graph type: is it directed or undirected, weighted or unweighted? */
typedef enum {
    GRAPH_UNDIRECTED_UNWEIGHTED,
    GRAPH_DIRECTED_UNWEIGHTED,
    GRAPH_UNDIRECTED_WEIGHTED,
    GRAPH_DIRECTED_WEIGHTED
} GraphType;

/* Implementation storage choice: adjacency list or adjacency matrix */
typedef enum {
    GRAPH_STORAGE_LIST,
    GRAPH_STORAGE_MATRIX
} GraphStorage;


/* We will declare a struct GraphOps with function pointers, 
 * then we define adjacency-list ops in adj_list.c, adjacency-matrix ops in adj_matrix.c.
 */
typedef struct GraphOps {
    // existing function pointers
    bool   (*addVertex)(void* impl, void* data);
    bool   (*removeVertex)(void* impl, const void* data);
    bool   (*addEdge)(void* impl, const void* srcData, const void* dstData, double weight);
    bool   (*removeEdge)(void* impl, const void* srcData, const void* dstData);
    int    (*getNumVertices)(const void* impl);
    int    (*getNumEdges)(const void* impl);
    bool   (*hasEdge)(const void* impl, const void* srcData, const void* dstData, double* outWeight);
    void   (*print)(const void* impl, void (*printData)(const void*));
    void   (*destroy)(void* impl);
    void**  (*bfs)(void* impl, const void* startData, int* outCount);
    void**  (*dfs)(void* impl, const void* startData, int* outCount);
    double* (*dijkstra)(void* impl, const void* startData);
} GraphOps;

/* Our public Graph struct definition (hidden from user) */
typedef struct Graph {
    GraphType     type;
    GraphStorage  storage;
    int  (*compare)(const void*, const void*);
    void (*freeData)(void*);

    const GraphOps* ops;   /* function pointer table */
    void*           impl;  /* actual adjacency-list or matrix structure */
}Graph;



/* Create a graph. 
 * storageMode: GRAPH_STORAGE_LIST or GRAPH_STORAGE_MATRIX
 */
Graph* createGraphImplementation(GraphType     type,
                                 GraphStorage  storageMode,
                                 int           initialCapacity,
                                 int  (*compareFunc)(const void*, const void*),
                                 void (*freeFunc)(void*));

/* Public graph operations (delegated internally) */
bool addVertex   (Graph* graph, void* data);
bool removeVertex(Graph* graph, const void* data);
bool addEdge     (Graph* graph, const void* srcData, const void* dstData, double weight);
bool removeEdge  (Graph* graph, const void* srcData, const void* dstData);
int  getNumVertices(const Graph* graph);
int  getNumEdges   (const Graph* graph);
bool hasEdge        (const Graph* graph, const void* srcData, const void* dstData, double* outWeight);
void printGraph     (const Graph* graph, void (*printData)(const void*));
void destroyGraph   (Graph* graph);
void** graphBFS(const Graph* g, const void* startData, int* outCount);
void** graphDFS(const Graph* g, const void* startData, int* outCount);
double* graphDijkstra(const Graph* g, const void* startData);

/* Possibly BFS/DFS, etc. 
 * Or we provide an interface to iterate neighbors, so BFS is done externally. */

#endif /* GRAPH_H */
