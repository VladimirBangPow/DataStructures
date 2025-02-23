#ifndef GRAPH_ALGORITHMS_H
#define GRAPH_ALGORITHMS_H

#include "graph.h"
#include "../Queue/queue.h"
#include "../PriorityQueue/pq.h"

/*
 * Performs a BFS starting from the vertex containing 'startData'.
 * Returns a newly allocated array of void* in BFS order.
 * The number of visited vertices is returned via outCount (if non-NULL).
 * Returns NULL if startData is not in the graph or if allocation fails.
 *
 * Note: The BFS ignores edge weights (treating them like an unweighted graph).
 */
void** graphBFS(const Graph* graph, const void* startData, int* outCount);

/*
 * Performs a DFS (recursive or stack-based) starting from 'startData'.
 * Returns a newly allocated array of void* in DFS order.
 * The number of visited vertices is returned via outCount (if non-NULL).
 * Returns NULL if startData is not in the graph or if allocation fails.
 */
void** graphDFS(const Graph* graph, const void* startData, int* outCount);

/*
 * Runs Dijkstra's algorithm for single-source shortest path on a (weighted) graph.
 *
 * If the graph is unweighted, it still works, but BFS might be simpler. 
 *
 * Returns a newly allocated array of distances, where distance[i] is the cost 
 * of the shortest path from 'startData' to the vertex at index i.
 *
 * - If a vertex is unreachable, distance[i] will be set to some "infinity" (e.g. very large double).
 * - If 'startData' is not found, returns NULL.
 *
 * The caller is responsible for freeing the returned array.
 */
double* graphDijkstra(const Graph* graph, const void* startData);

#endif /* GRAPH_ALGORITHMS_H */
