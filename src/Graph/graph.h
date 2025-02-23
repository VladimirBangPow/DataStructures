#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <stddef.h>

/* 
 * Enum or flags for graph properties.
 * You can alternatively store booleans (isDirected, isWeighted).
 */
typedef enum {
    GRAPH_UNDIRECTED_UNWEIGHTED,
    GRAPH_DIRECTED_UNWEIGHTED,
    GRAPH_UNDIRECTED_WEIGHTED,
    GRAPH_DIRECTED_WEIGHTED
} GraphType;

/*
 * A structure to represent an adjacency list node (edge).
 * 'weight' is meaningful only for weighted graphs. For an unweighted graph,
 * you can ignore or set this to a default value (like 1.0 or 0.0).
 */
typedef struct Edge {
    int destIndex;         /* Index in the graph->vertices array this edge goes to */
    double weight;         /* Edge weight if graph is weighted; else 1.0 or unused */
    struct Edge* next;     /* Pointer to the next edge in the adjacency list */
} Edge;

/*
 * A structure to store each vertex in the graph.
 * 'data' is a generic pointer to user-supplied vertex data.
 * 'edges' is the adjacency list head for this vertex.
 */
typedef struct Vertex {
    void* data;
    Edge* edges;
} Vertex;

/*
 * Graph structure.
 * - 'type' indicates directed/undirected/weighted/unweighted.
 * - 'capacity' is how many vertices we can hold (initially allocated).
 * - 'size' is the current number of vertices.
 * - 'vertices' is a dynamic array of Vertex.
 * - 'compare' is used to compare vertex data (return 0 if equal, <0 or >0 otherwise).
 * - 'freeData' is used to free the vertex data if needed (user-supplied).
 */
typedef struct Graph {
    GraphType type;
    int capacity;
    int size;
    Vertex* vertices;

    int  (*compare)(const void*, const void*);
    void (*freeData)(void*);
} Graph;

/* Function prototypes */
Graph* createGraph(GraphType type, 
                   int initialCapacity,
                   int (*compareFunc)(const void*, const void*),
                   void (*freeFunc)(void*));

void destroyGraph(Graph* graph);

/* Vertex operations */
bool addVertex(Graph* graph, void* data);
bool removeVertex(Graph* graph, const void* data);

/* Edge operations */
bool addEdge(Graph* graph, const void* sourceData, const void* destData, double weight);
bool removeEdge(Graph* graph, const void* sourceData, const void* destData);

/* Utility & Query */
int  getNumVertices(const Graph* graph);
int  getNumEdges(const Graph* graph);
bool hasEdge(const Graph* graph, const void* sourceData, const void* destData, double* outWeight);

/* Debug printing (optional) */
void printGraph(const Graph* graph, void (*printData)(const void*));

#endif /* GRAPH_H */
