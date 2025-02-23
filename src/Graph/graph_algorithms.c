#include <stdio.h>
#include <stdlib.h>
#include <float.h>   /* for DBL_MAX to represent "infinity" */
#include <stdbool.h>
#include <string.h>

#include "graph_algorithms.h"

/**********************************************************************
 *                        Helper Functions
 **********************************************************************/

/*
 * Utility to find the index of a vertex (same as in graph.c, 
 * but we keep a local copy if needed).
 * Or we can make graph.c's "findVertexIndex" a public function if desired.
 */
static int findVertexIndex(const Graph* graph, const void* data) {
    if (!graph || !data) return -1;
    for (int i = 0; i < graph->size; i++) {
        if (graph->compare(graph->vertices[i].data, data) == 0) {
            return i;
        }
    }
    return -1;
}

/**********************************************************************
 *                                BFS
 **********************************************************************/

void** graphBFS(const Graph* graph, const void* startData, int* outCount) {
    if (!graph || !startData) {
        if (outCount) *outCount = 0;
        return NULL;
    }

    int startIndex = findVertexIndex(graph, startData);
    if (startIndex < 0) {
        /* startData not in graph */
        if (outCount) *outCount = 0;
        return NULL;
    }

    /* We'll maintain a visited array (size = graph->size) */
    bool* visited = (bool*)calloc(graph->size, sizeof(bool));
    if (!visited) {
        if (outCount) *outCount = 0;
        return NULL;
    }

    /* We'll store the BFS order in a temporary array of void* */
    void** result = (void**)malloc(sizeof(void*) * graph->size);
    if (!result) {
        free(visited);
        if (outCount) *outCount = 0;
        return NULL;
    }
    int  resultCount = 0;

    /* Initialize queue */
    Queue q;
    queueInit(&q);

    /* Enqueue the start index */
    queueEnqueue(&q, &startIndex, sizeof(int));
    visited[startIndex] = true;

    while (!queueIsEmpty(&q)) {
        int frontIndex;
        queueDequeue(&q, &frontIndex);

        /* Add to BFS result */
        result[resultCount++] = graph->vertices[frontIndex].data;

        /* Explore neighbors */
        Edge* e = graph->vertices[frontIndex].edges;
        while (e) {
            int neighborIndex = e->destIndex;
            if (!visited[neighborIndex]) {
                visited[neighborIndex] = true;
                queueEnqueue(&q, &neighborIndex, sizeof(int));
            }
            e = e->next;
        }
    }

    /* Cleanup */
    free(visited);
    queueClear(&q);

    if (outCount) *outCount = resultCount;
    return result;
}

/**********************************************************************
 *                                DFS
 **********************************************************************/

/* 
 * We can implement DFS recursively or using our own stack. 
 * Below is a recursive approach.
 */
static void dfsHelper(const Graph* graph, 
                      int currentIndex,
                      bool* visited,
                      void** result,
                      int* resultCount) 
{
    visited[currentIndex] = true;
    /* Add to DFS result */
    result[(*resultCount)++] = graph->vertices[currentIndex].data;

    /* Explore neighbors */
    Edge* e = graph->vertices[currentIndex].edges;
    while (e) {
        int neighborIndex = e->destIndex;
        if (!visited[neighborIndex]) {
            dfsHelper(graph, neighborIndex, visited, result, resultCount);
        }
        e = e->next;
    }
}

void** graphDFS(const Graph* graph, const void* startData, int* outCount) {
    if (!graph || !startData) {
        if (outCount) *outCount = 0;
        return NULL;
    }

    int startIndex = findVertexIndex(graph, startData);
    if (startIndex < 0) {
        /* startData not found */
        if (outCount) *outCount = 0;
        return NULL;
    }

    bool* visited = (bool*)calloc(graph->size, sizeof(bool));
    if (!visited) {
        if (outCount) *outCount = 0;
        return NULL;
    }

    /* We'll store DFS order in a void* array */
    void** result = (void**)malloc(sizeof(void*) * graph->size);
    if (!result) {
        free(visited);
        if (outCount) *outCount = 0;
        return NULL;
    }
    int resultCount = 0;

    /* Recursive DFS from startIndex */
    dfsHelper(graph, startIndex, visited, result, &resultCount);

    free(visited);
    if (outCount) *outCount = resultCount;
    return result;
}

/**********************************************************************
 *                             Dijkstra
 **********************************************************************/

/*
 * We’ll implement a standard single-source Dijkstra for graphs with non-negative weights.
 * We'll store distances in an array "dist".
 * We'll use a PriorityQueue that returns the minimum distance vertex first (min-heap).
 *
 * The graph can be directed or undirected. If undirected, the adjacency lists store edges in both directions.
 * The procedure is the same either way, though, as long as edge weights are non-negative.
 */
typedef struct {
    int vertexIndex;
    double distance;
} DijkstraNode;

/* 
 * Compare function for the DijkstraNode in the priority queue. 
 * This will compare by distance field.
 */
static int dijkstraNodeCompare(const void* a, const void* b) {
    const DijkstraNode* da = (const DijkstraNode*)a;
    const DijkstraNode* db = (const DijkstraNode*)b;
    /* If da->distance < db->distance => negative => da has higher priority in a min-heap. */
    if (da->distance < db->distance) return -1;
    else if (da->distance > db->distance) return 1;
    return 0;
}

double* graphDijkstra(const Graph* graph, const void* startData) {
    if (!graph || !startData) return NULL;

    int startIndex = findVertexIndex(graph, startData);
    if (startIndex < 0) return NULL;  /* Not found */

    /* Allocate array of distances, initialize to "infinity" */
    double* dist = (double*)malloc(sizeof(double) * graph->size);
    if (!dist) return NULL;
    for (int i = 0; i < graph->size; i++) {
        dist[i] = DBL_MAX; /* a stand-in for infinity */
    }
    dist[startIndex] = 0.0; /* distance to itself is 0 */

    /* Visited array is optional in Dijkstra, 
     * we can store "visited" or rely on dist array. 
     * We'll keep a visited to stop reprocessing a node after we pop it from PQ with final distance.
     */
    bool* visited = (bool*)calloc(graph->size, sizeof(bool));
    if (!visited) {
        free(dist);
        return NULL;
    }

    /* Initialize priority queue (min-heap). We'll push a DijkstraNode with distance=0 for startIndex. */
    PriorityQueue pq;
    pqInit(&pq, dijkstraNodeCompare, true /* isMinHeap */, 16 /* initialCapacity */);

    DijkstraNode startNode = {startIndex, 0.0};
    pqPush(&pq, &startNode, sizeof(DijkstraNode));

    while (!pqIsEmpty(&pq)) {
        /* pop the node with smallest distance */
        DijkstraNode current;
        size_t dataSize = sizeof(DijkstraNode);
        bool ok = pqPop(&pq, &current, &dataSize);
        if (!ok) break; /* Shouldn't happen unless empty */

        int u = current.vertexIndex;
        if (visited[u]) {
            /* We already processed this vertex as final. Skip */
            continue;
        }
        visited[u] = true;

        /* Relax edges from u */
        Edge* e = graph->vertices[u].edges;
        while (e) {
            int v = e->destIndex;
            double w = e->weight;
            /* Even if the graph is unweighted, weight=1.0 in our code. 
             * For negative weights, Dijkstra wouldn't be correct, but we assume non-negative. 
             */
            if (!visited[v] && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                /* push the updated node in the PQ */
                DijkstraNode nd = { v, dist[v] };
                pqPush(&pq, &nd, sizeof(DijkstraNode));
            }
            e = e->next;
        }
    }

    /* cleanup */
    pqFree(&pq);
    free(visited);

    return dist;
}
