#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"



/* We'll forward-declare the adjacency-list and adjacency-matrix "create" 
 * functions that return (impl*, GraphOps*).
 */
void* createAdjListImpl(GraphType type,
                        int initialCapacity,
                        int  (*compareFunc)(const void*, const void*),
                        void (*freeFunc)(void*),
                        const GraphOps** opsOut);

void* createAdjMatrixImpl(GraphType type,
                          int initialCapacity,
                          int  (*compareFunc)(const void*, const void*),
                          void (*freeFunc)(void*),
                          const GraphOps** opsOut);




/* Factory function: create a Graph with the chosen storage mode */
Graph* createGraphImplementation(GraphType     type,
                                 GraphStorage  storageMode,
                                 int           initialCapacity,
                                 int  (*compareFunc)(const void*, const void*),
                                 void (*freeFunc)(void*)) 
{
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) return NULL;

    g->type     = type;
    g->storage  = storageMode;
    g->compare  = compareFunc ? compareFunc : NULL;
    g->freeData = freeFunc ? freeFunc : free; /* default to free if not provided */

    /* We call either createAdjListImpl or createAdjMatrixImpl, 
     * which returns (impl pointer) + a pointer to the ops table.
     */
    if (storageMode == GRAPH_STORAGE_LIST) {
        g->impl = createAdjListImpl(type, initialCapacity, g->compare, g->freeData, &g->ops);
    } else {
        g->impl = createAdjMatrixImpl(type, initialCapacity, g->compare, g->freeData, &g->ops);
    }

    if (!g->impl) {
        free(g);
        return NULL;
    }

    return g;
}

/* Now define the delegating functions: they just call g->ops->theFunction(g->impl, ...) */

bool addVertex(Graph* graph, void* data) {
    if (!graph) return false;
    return graph->ops->addVertex(graph->impl, data);
}

bool removeVertex(Graph* graph, const void* data) {
    if (!graph) return false;
    return graph->ops->removeVertex(graph->impl, data);
}

bool addEdge(Graph* graph, const void* srcData, const void* dstData, double weight) {
    if (!graph) return false;
    return graph->ops->addEdge(graph->impl, srcData, dstData, weight);
}

bool removeEdge(Graph* graph, const void* srcData, const void* dstData) {
    if (!graph) return false;
    return graph->ops->removeEdge(graph->impl, srcData, dstData);
}

int getNumVertices(const Graph* graph) {
    if (!graph) return 0;
    return graph->ops->getNumVertices(graph->impl);
}

int getNumEdges(const Graph* graph) {
    if (!graph) return 0;
    return graph->ops->getNumEdges(graph->impl);
}

bool hasEdge(const Graph* graph, const void* srcData, const void* dstData, double* outWeight) {
    if (!graph) return false;
    return graph->ops->hasEdge(graph->impl, srcData, dstData, outWeight);
}

void printGraph(const Graph* graph, void (*printData)(const void*)) {
    if (!graph) return;
    graph->ops->print(graph->impl, printData);
}

void destroyGraph(Graph* graph) {
    if (!graph) return;
    /* call the impl's destroy function, then free the Graph struct */
    graph->ops->destroy(graph->impl);
    graph->impl = NULL;
    free(graph);
}
