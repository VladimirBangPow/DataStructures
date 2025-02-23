#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Internal function to resize the vertices array if needed */
static bool resizeGraph(Graph* graph, int newCapacity) {
    Vertex* newVertices = (Vertex*)realloc(graph->vertices, sizeof(Vertex) * newCapacity);
    if (!newVertices) {
        return false;
    }
    graph->vertices = newVertices;
    /* Initialize newly allocated spots */
    for (int i = graph->capacity; i < newCapacity; i++) {
        graph->vertices[i].data  = NULL;
        graph->vertices[i].edges = NULL;
    }
    graph->capacity = newCapacity;
    return true;
}

/* 
 * Helper function to find the index of a vertex in the graph->vertices array.
 * Returns index if found, else returns -1.
 */
static int findVertexIndex(const Graph* graph, const void* data) {
    for (int i = 0; i < graph->size; i++) {
        if (graph->compare(graph->vertices[i].data, data) == 0) {
            return i;
        }
    }
    return -1;
}

/*
 * Creates a new graph with the specified type and initial capacity.
 * compareFunc: function to compare two vertex data items (must not be NULL).
 * freeFunc: function to free vertex data (if you store dynamically allocated data). 
 *           If no special freeing is needed, you can pass NULL or a no-op function.
 */
Graph* createGraph(GraphType type, 
                   int initialCapacity,
                   int (*compareFunc)(const void*, const void*),
                   void (*freeFunc)(void*)) 
{
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) return NULL;

    graph->type = type;
    graph->capacity = initialCapacity;
    graph->size = 0;
    graph->compare = compareFunc;
    graph->freeData = freeFunc ? freeFunc : free;  /* default to free if not provided */

    graph->vertices = (Vertex*)calloc(initialCapacity, sizeof(Vertex));
    if (!graph->vertices) {
        free(graph);
        return NULL;
    }
    return graph;
}

/* Helper function to free an adjacency list */
static void freeEdges(Edge* edge) {
    while (edge) {
        Edge* temp = edge;
        edge = edge->next;
        free(temp);
    }
}

/*
 * Destroys the graph by freeing all edges and all vertex data, then the graph itself.
 */
void destroyGraph(Graph* graph) {
    if (!graph) return;

    /* Free adjacency lists and vertex data */
    for (int i = 0; i < graph->size; i++) {
        /* free adjacency list for each vertex */
        freeEdges(graph->vertices[i].edges);
        graph->vertices[i].edges = NULL;

        /* free vertex data if needed */
        if (graph->vertices[i].data && graph->freeData) {
            graph->freeData(graph->vertices[i].data);
            graph->vertices[i].data = NULL;
        }
    }

    /* free the vertex array */
    free(graph->vertices);

    /* finally free the graph struct */
    free(graph);
}

/*
 * Adds a vertex to the graph. 
 * Returns false if the vertex already exists or if reallocation fails.
 */
bool addVertex(Graph* graph, void* data) {
    if (!graph || !data) return false;

    /* Check if vertex already exists */
    if (findVertexIndex(graph, data) != -1) {
        return false;  /* Vertex already in graph */
    }

    /* Resize if necessary */
    if (graph->size >= graph->capacity) {
        if (!resizeGraph(graph, graph->capacity * 2)) {
            return false;
        }
    }

    /* Insert new vertex at index = graph->size */
    graph->vertices[graph->size].data = data;
    graph->vertices[graph->size].edges = NULL;
    graph->size++;

    return true;
}

/*
 * Removes a vertex from the graph, along with all edges referencing it.
 */
bool removeVertex(Graph* graph, const void* data) {
    if (!graph || !data || graph->size == 0) return false;

    int idx = findVertexIndex(graph, data);
    if (idx == -1) {
        return false; /* vertex not found */
    }

    /* Free adjacency list of this vertex */
    freeEdges(graph->vertices[idx].edges);
    graph->vertices[idx].edges = NULL;

    /* Free the vertex data */
    if (graph->vertices[idx].data && graph->freeData) {
        graph->freeData(graph->vertices[idx].data);
    }
    graph->vertices[idx].data = NULL;

    /* We also need to remove edges from all other vertices that point to this vertex. */
    for (int i = 0; i < graph->size; i++) {
        if (i == idx) continue;
        Edge* prev = NULL;
        Edge* current = graph->vertices[i].edges;
        while (current) {
            if (current->destIndex == idx) {
                /* remove this edge */
                if (prev) {
                    prev->next = current->next;
                } else {
                    graph->vertices[i].edges = current->next;
                }
                free(current);
                break;  /* remove only once if there's a single edge referencing? or all? */
            }
            prev = current;
            current = current->next;
        }
    }

    /* Now "shift" the last vertex in the array to fill this slot, to keep it contiguous. */
    int lastIndex = graph->size - 1;
    if (idx != lastIndex) {
        /* Move the last vertex data into idx slot */
        graph->vertices[idx].data  = graph->vertices[lastIndex].data;
        graph->vertices[idx].edges = graph->vertices[lastIndex].edges;
        graph->vertices[lastIndex].data  = NULL;
        graph->vertices[lastIndex].edges = NULL;

        /* All edges in the graph that pointed to lastIndex should now point to idx. */
        for (int i = 0; i < graph->size - 1; i++) {
            Edge* e = graph->vertices[i].edges;
            while (e) {
                if (e->destIndex == lastIndex) {
                    e->destIndex = idx;
                }
                e = e->next;
            }
        }
    }

    /* decrement size */
    graph->size--;

    return true;
}

/*
 * Adds an edge between two vertices (sourceData -> destData).
 * If the graph is undirected, also adds the reverse edge (destData -> sourceData).
 * If the graph is unweighted, 'weight' can be ignored or set to a default (like 1.0).
 */
bool addEdge(Graph* graph, const void* sourceData, const void* destData, double weight) {
    if (!graph || !sourceData || !destData) return false;

    int srcIndex = findVertexIndex(graph, sourceData);
    int dstIndex = findVertexIndex(graph, destData);

    if (srcIndex == -1 || dstIndex == -1) {
        return false; /* One of the vertices doesn't exist */
    }

    /* For unweighted graphs, you might ignore or override 'weight' with 1.0. */
    bool weighted = false;
    switch (graph->type) {
        case GRAPH_UNDIRECTED_WEIGHTED:
        case GRAPH_DIRECTED_WEIGHTED:
            weighted = true;
            break;
        default:
            weighted = false;
            break;
    }
    double finalWeight = weighted ? weight : 1.0;

    /* Check if edge already exists from src -> dst */
    Edge* e = graph->vertices[srcIndex].edges;
    while (e) {
        if (e->destIndex == dstIndex) {
            /* Edge already exists, update weight if needed */
            if (weighted) {
                e->weight = finalWeight;
            }
            return true;
        }
        e = e->next;
    }

    /* Otherwise, create a new edge node at the front of adjacency list (for simplicity) */
    Edge* newEdge = (Edge*)malloc(sizeof(Edge));
    if (!newEdge) return false;
    newEdge->destIndex = dstIndex;
    newEdge->weight = finalWeight;
    newEdge->next = graph->vertices[srcIndex].edges;
    graph->vertices[srcIndex].edges = newEdge;

    /* If undirected, add the reverse edge as well */
    bool directed = false;
    switch (graph->type) {
        case GRAPH_DIRECTED_UNWEIGHTED:
        case GRAPH_DIRECTED_WEIGHTED:
            directed = true;
            break;
        default:
            directed = false;
            break;
    }
    if (!directed) {
        /* Check if reverse edge exists from dst -> src */
        e = graph->vertices[dstIndex].edges;
        while (e) {
            if (e->destIndex == srcIndex) {
                /* Edge already exists, update weight if needed */
                if (weighted) {
                    e->weight = finalWeight;
                }
                return true;
            }
            e = e->next;
        }
        /* Create reverse edge */
        Edge* revEdge = (Edge*)malloc(sizeof(Edge));
        if (!revEdge) return false;
        revEdge->destIndex = srcIndex;
        revEdge->weight = finalWeight;
        revEdge->next = graph->vertices[dstIndex].edges;
        graph->vertices[dstIndex].edges = revEdge;
    }

    return true;
}

/*
 * Removes an edge from sourceData to destData. For undirected graphs,
 * also remove the reverse edge from destData to sourceData.
 */
bool removeEdge(Graph* graph, const void* sourceData, const void* destData) {
    if (!graph || !sourceData || !destData) return false;

    int srcIndex = findVertexIndex(graph, sourceData);
    int dstIndex = findVertexIndex(graph, destData);

    if (srcIndex == -1 || dstIndex == -1) {
        return false; /* vertices not found */
    }

    bool directed = false;
    switch (graph->type) {
        case GRAPH_DIRECTED_UNWEIGHTED:
        case GRAPH_DIRECTED_WEIGHTED:
            directed = true;
            break;
        default:
            directed = false;
            break;
    }

    /* remove edge from srcIndex -> dstIndex */
    Edge* prev = NULL;
    Edge* current = graph->vertices[srcIndex].edges;
    while (current) {
        if (current->destIndex == dstIndex) {
            /* remove this edge */
            if (prev) {
                prev->next = current->next;
            } else {
                graph->vertices[srcIndex].edges = current->next;
            }
            free(current);
            current = NULL;
            break;
        }
        prev = current;
        current = current->next;
    }

    if (!directed) {
        /* also remove reverse edge from dstIndex -> srcIndex */
        prev = NULL;
        current = graph->vertices[dstIndex].edges;
        while (current) {
            if (current->destIndex == srcIndex) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    graph->vertices[dstIndex].edges = current->next;
                }
                free(current);
                current = NULL;
                break;
            }
            prev = current;
            current = current->next;
        }
    }

    return true;
}

/*
 * Returns the number of vertices in the graph.
 */
int getNumVertices(const Graph* graph) {
    if (!graph) return 0;
    return graph->size;
}

/*
 * Returns the total number of edges in the graph.
 * For undirected graphs, each edge is counted once.
 */
int getNumEdges(const Graph* graph) {
    if (!graph) return 0;

    bool directed = false;
    switch (graph->type) {
        case GRAPH_DIRECTED_UNWEIGHTED:
        case GRAPH_DIRECTED_WEIGHTED:
            directed = true;
            break;
        default:
            directed = false;
            break;
    }

    int count = 0;
    for (int i = 0; i < graph->size; i++) {
        Edge* e = graph->vertices[i].edges;
        while (e) {
            count++;
            e = e->next;
        }
    }
    /* For undirected graphs, each edge appears twice in adjacency lists. */
    if (!directed) {
        count /= 2;
    }
    return count;
}

/*
 * Checks if an edge from sourceData to destData exists.
 * If outWeight is not NULL and the edge is found (weighted or not),
 * this function sets *outWeight to the edge's weight.
 */
bool hasEdge(const Graph* graph, const void* sourceData, const void* destData, double* outWeight) {
    if (!graph || !sourceData || !destData) return false;

    int srcIndex = findVertexIndex(graph, sourceData);
    int dstIndex = findVertexIndex(graph, destData);

    if (srcIndex == -1 || dstIndex == -1) {
        return false;
    }

    Edge* e = graph->vertices[srcIndex].edges;
    while (e) {
        if (e->destIndex == dstIndex) {
            if (outWeight) {
                *outWeight = e->weight;
            }
            return true;
        }
        e = e->next;
    }

    return false;
}

/*
 * Optional debug function to print the graph.
 * 'printData' is a user-supplied function that prints vertex data.
 */
void printGraph(const Graph* graph, void (*printData)(const void*)) {
    if (!graph) return;

    printf("Graph:\n");
    for (int i = 0; i < graph->size; i++) {
        printf("Vertex %d: ", i);
        if (printData) {
            printData(graph->vertices[i].data);
        }
        printf(" -> ");
        Edge* e = graph->vertices[i].edges;
        while (e) {
            printf("[");
            if (printData) {
                printData(graph->vertices[e->destIndex].data);
            }
            printf(" (w=%.2f)] ", e->weight);
            e = e->next;
        }
        printf("\n");
    }
    printf("\n");
}
