#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "test_graph.h"
#include "graph.h"  

/*
 * We assume "graph.h" declares or includes:
 *   - Enums: GraphType, GraphStorage
 *   - createGraphImplementation(...)
 *   - The BFS, DFS, Dijkstra driver functions:
 *       void**  graphBFS(const Graph* g, const void* startData, int* outCount);
 *       void**  graphDFS(const Graph* g, const void* startData, int* outCount);
 *       double* graphDijkstra(const Graph* g, const void* startData);
 *
 * If they aren't declared in "graph.h", you'd do:
 *   extern void** graphBFS(const Graph*, const void*, int*);
 *   extern void** graphDFS(const Graph*, const void*, int*);
 *   extern double* graphDijkstra(const Graph*, const void*);
 */

/*******************************************************************
 *                   Data Types & Utilities
 *******************************************************************/

/* Quick helper to check if a GraphType is weighted or directed. */
static bool isWeighted(GraphType t) {
    return (t == GRAPH_UNDIRECTED_WEIGHTED || t == GRAPH_DIRECTED_WEIGHTED);
}
static bool isDirected(GraphType t) {
    return (t == GRAPH_DIRECTED_UNWEIGHTED || t == GRAPH_DIRECTED_WEIGHTED);
}

/* 1) Integers */
static int compareInt(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}
static void freeInt(void* data) {
    free(data);
}
static void* createDataInt(int i) {
    int* p = (int*)malloc(sizeof(int));
    *p = i;
    return p;
}

/* 2) Floats */
static int compareFloat(const void* a, const void* b) {
    float fa = *(float*)a;
    float fb = *(float*)b;
    if (fa < fb) return -1;
    if (fa > fb) return 1;
    return 0;
}
static void freeFloat(void* data) {
    free(data);
}
static void* createDataFloat(int i) {
    float* p = (float*)malloc(sizeof(float));
    *p = (float)(i * 1.1f);
    return p;
}

/* 3) Strings */
static int compareString(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}
static void freeString(void* data) {
    free(data);
}
static void* createDataString(int i) {
    char buf[32];
    snprintf(buf, sizeof(buf), "str%d", i);
    return strdup(buf); /* duplicates into heap */
}

/* 4) "Animal" struct */
typedef struct {
    char name[32];
    int  age;
} Animal;

static int compareAnimal(const void* a, const void* b) {
    const Animal* A = (const Animal*)a;
    const Animal* B = (const Animal*)b;
    int c = strcmp(A->name, B->name);
    if (c != 0) return c;
    return (A->age - B->age);
}
static void freeAnimal(void* data) {
    free(data);
}
static void* createDataAnimal(int i) {
    /* We'll create animals named "Animal_i", age i. */
    Animal* a = (Animal*)malloc(sizeof(Animal));
    snprintf(a->name, sizeof(a->name), "Animal_%d", i);
    a->age = i;
    return a;
}

/* We'll define arrays to iterate over all GraphType, GraphStorage combos. */
static GraphType g_types[] = {
    GRAPH_UNDIRECTED_UNWEIGHTED,
    GRAPH_DIRECTED_UNWEIGHTED,
    GRAPH_UNDIRECTED_WEIGHTED,
    GRAPH_DIRECTED_WEIGHTED
};

static GraphStorage g_storages[] = {
    GRAPH_STORAGE_LIST,
    GRAPH_STORAGE_MATRIX
};

/*******************************************************************
 *   BFS / DFS / Dijkstra Test: Small Graph & Verification
 *******************************************************************/

/* We'll define small helper functions that:
 *   - build a small graph,
 *   - call BFS/DFS/Dijkstra,
 *   - check the results with asserts.
 */

/* BFS test: build a small graph, call BFS from a vertex, check order. */
static void testBFSOnSmallGraph(GraphType type, GraphStorage storage) {
    Graph* g = createGraphImplementation(type, storage, 4, compareInt, freeInt);
    assert(g);

    /* Add vertices 0..3 */
    int* v0 = createDataInt(0); addVertex(g, v0);
    int* v1 = createDataInt(1); addVertex(g, v1);
    int* v2 = createDataInt(2); addVertex(g, v2);
    int* v3 = createDataInt(3); addVertex(g, v3);

    /* Connect edges in a small shape, for instance:
         0 -- 1
         |    |
         2 -- 3
     */
    addEdge(g, v0, v1, 1.0);
    addEdge(g, v0, v2, 1.0);
    addEdge(g, v1, v3, 1.0);
    addEdge(g, v2, v3, 1.0);

    int outCount = 0;
    void** bfsOrder = graphBFS(g, v0, &outCount);
    assert(bfsOrder);
    assert(outCount == 4);

    /* Check that we got all {0,1,2,3} exactly once. */
    bool found[4] = {false,false,false,false};
    for (int i = 0; i < outCount; i++) {
        int val = *(int*)bfsOrder[i];
        found[val] = true;
    }
    for (int i = 0; i < 4; i++) {
        assert(found[i] == true);
    }

    free(bfsOrder);
    destroyGraph(g);
    printf("[OK] testBFSOnSmallGraph <%s>\n", (storage == GRAPH_STORAGE_LIST ? "adj_list" : "adj_matrix"));
}
/* DFS test: similar small shape, check we get all vertices. */
static void testDFSOnSmallGraph(GraphType type, GraphStorage storage) {
    Graph* g = createGraphImplementation(type, storage, 4, compareInt, freeInt);
    assert(g);

    int* v0 = createDataInt(0); addVertex(g, v0);
    int* v1 = createDataInt(1); addVertex(g, v1);
    int* v2 = createDataInt(2); addVertex(g, v2);
    int* v3 = createDataInt(3); addVertex(g, v3);

    addEdge(g, v0, v1, 1.0);
    addEdge(g, v0, v2, 1.0);
    addEdge(g, v1, v3, 1.0);
    addEdge(g, v2, v3, 1.0);

    int outCount = 0;
    void** dfsOrder = graphDFS(g, v0, &outCount);
    assert(dfsOrder);
    assert(outCount == 4);

    bool found[4] = {false,false,false,false};
    for (int i = 0; i < outCount; i++) {
        int val = *(int*)dfsOrder[i];
        found[val] = true;
    }
    for (int i = 0; i < 4; i++) {
        assert(found[i] == true);
    }

    free(dfsOrder);
    destroyGraph(g);
    printf("[OK] testDFSOnSmallGraph <%s>\n", (storage == GRAPH_STORAGE_LIST ? "adj_list" : "adj_matrix"));
}

/* Dijkstra test: small weighted shape. We'll confirm distances. */
static void testDijkstraOnSmallGraph(GraphType type, GraphStorage storage) {
    /* Weighted edges or unweighted => each edge=1.0. We'll confirm distances from 0. */
    Graph* g = createGraphImplementation(type, storage, 4, compareInt, freeInt);
    assert(g);

    int* v0 = createDataInt(0); addVertex(g, v0);
    int* v1 = createDataInt(1); addVertex(g, v1);
    int* v2 = createDataInt(2); addVertex(g, v2);
    int* v3 = createDataInt(3); addVertex(g, v3);

    /* Weighted edges, for example:
        0--1=2
        0--2=5
        1--2=1
        1--3=4
        2--3=2
     */
    addEdge(g, v0, v1, 2.0);
    addEdge(g, v0, v2, 5.0);
    addEdge(g, v1, v2, 1.0);
    addEdge(g, v1, v3, 4.0);
    addEdge(g, v2, v3, 2.0);

    double* dist = graphDijkstra(g, v0);
    assert(dist);

    if (isWeighted(type)) {
        /* Weighted => we expect distances:
           0 => 0
           1 => 2
           2 => 3 (0->1->2 =>2+1)
           3 => 5 (0->1->2->3 =>2+1+2=5)
        */
        assert(dist[0] == 0.0);
        assert(dist[1] == 2.0);
        assert(dist[2] == 3.0);
        assert(dist[3] == 5.0);
    } else {
        /* Unweighted => BFS-like distances. We'll do a weaker check:
         * 0 => 0
         * others => not DBL_MAX
         */
        assert(dist[0] == 0.0);
        assert(dist[1] < 1e9);
        assert(dist[2] < 1e9);
        assert(dist[3] < 1e9);
    }

    free(dist);
    destroyGraph(g);
    printf("[OK] testDijkstraOnSmallGraph <%s>\n", (storage == GRAPH_STORAGE_LIST ? "adj_list" : "adj_matrix"));
}

/*******************************************************************
 *           Minimal Basic Test + Stress Test
 *******************************************************************/

/* We'll define createData, compare, free, etc., for each data type. */

static void basicTest(GraphType type,
                      GraphStorage storage,
                      const char* label,
                      void* (*createData)(int),
                      int(*cmpFunc)(const void*, const void*),
                      void(*freeFunc)(void*)) 
{
    /* 1) Create graph */
    Graph* g = createGraphImplementation(type, storage, 4, cmpFunc, freeFunc);
    assert(g != NULL);

    /* 2) Add 3 vertices */
    void* v0 = createData(0);
    void* v1 = createData(1);
    void* v2 = createData(2);

    bool ok = addVertex(g, v0);  assert(ok == true);
    ok = addVertex(g, v1);       assert(ok == true);
    ok = addVertex(g, v2);       assert(ok == true);

    /* check getNumVertices => 3 */
    assert(getNumVertices(g) == 3);

    /* 3) Add edges and check # of edges */
    ok = addEdge(g, v0, v1, 2.5); assert(ok == true);
    ok = addEdge(g, v1, v2, 1.0); assert(ok == true);
    int ecount = getNumEdges(g);
    assert(ecount == 2);

    /* Check hasEdge */
    double w = -999.0;
    bool he = hasEdge(g, v0, v1, &w);
    assert(he == true);
    if (isWeighted(type)) {
        assert(w == 2.5);
    } else {
        assert(w == 1.0); /* unweighted => stored as 1.0 */
    }

    /* 4) remove an edge, check again */
    ok = removeEdge(g, v1, v2);
    assert(ok == true);
    ecount = getNumEdges(g);
    assert(ecount == 1);

    /* 5) remove a vertex, check again */
    ok = removeVertex(g, v0);
    assert(ok == true);
    assert(getNumVertices(g) == 2);

    destroyGraph(g);
    printf("[OK] basicTest <%s>, %s\n", label,
           (storage == GRAPH_STORAGE_LIST ? "adj_list" : "adj_matrix"));
}

/* Larger stress test */
static void stressTest(GraphType type,
                       GraphStorage storage,
                       const char* label,
                       void* (*createData)(int),
                       int(*cmpFunc)(const void*, const void*),
                       void(*freeFunc)(void*)) 
{
    srand((unsigned int)time(NULL)); 

    Graph* g = createGraphImplementation(type, storage, 16, cmpFunc, freeFunc);
    assert(g != NULL);

    /* We'll create up to n vertices */
    int n = 1000; 
    void** arr = (void**)malloc(sizeof(void*) * (size_t)n);
    for (int i = 0; i < n; i++) {
        arr[i] = createData(i);
        bool ok = addVertex(g, arr[i]);
        assert(ok == true);
    }
    assert(getNumVertices(g) == n);

    int edgeAttempts = n * 2;
    for (int i = 0; i < edgeAttempts; i++) {
        int srcIdx = rand() % n;
        int dstIdx = rand() % n;
        if (srcIdx == dstIdx) continue;
        double w = 1.0 + (rand() % 10);
        addEdge(g, arr[srcIdx], arr[dstIdx], w);
    }

    /* random checks */
    for (int i = 0; i < 20; i++) {
        int s = rand() % n;
        int d = rand() % n;
        double w = -1.0;
        bool he = hasEdge(g, arr[s], arr[d], &w);
        if (he) {
            if (!isWeighted(type)) {
                assert(w == 1.0);
            } else {
                assert(w >= 1.0 && w <= 10.0);
            }
        }
    }

    /* remove ~10 vertices randomly */
    int removed = 0;
    for (int i = 0; i < 10; i++) {
        int idx = rand() % n;
        if (arr[idx]) {
            bool ok = removeVertex(g, arr[idx]);
            if (ok) {
                arr[idx] = NULL;
                removed++;
            }
        }
    }
    assert(getNumVertices(g) == (n - removed));

    /* remove some edges randomly */
    for (int i = 0; i < 20; i++) {
        int s = rand() % n;
        int d = rand() % n;
        if (arr[s] && arr[d]) {
            removeEdge(g, arr[s], arr[d]);
        }
    }

    destroyGraph(g);

    /* if the graph "owns" data, it's already freed. If not, we can free leftover. 
       But let's assume the graph owns them. 
       If we want to be sure, we can do:
         for(int i=0;i<n;i++){ if(arr[i]) freeFunc(arr[i]); }
    */
    free(arr);

    printf("[OK] stressTest <%s>, %s\n", label,
           (storage == GRAPH_STORAGE_LIST ? "adj_list" : "adj_matrix"));
}

/*******************************************************************
 *   Combining Basic + BFS/DFS/Dijkstra + Stress
 *******************************************************************/

static void testAllForData(const char* label,
                           GraphType gType,
                           GraphStorage gStore,
                           void* (*createData)(int),
                           int (*cmpFunc)(const void*, const void*),
                           void (*freeFunc)(void*))
{
    /* 1) basicTest (3 vertices, edges, etc.) */
    basicTest(gType, gStore, label, createData, cmpFunc, freeFunc);

    /* 2) BFS small test */
    testBFSOnSmallGraph(gType, gStore);

    /* 3) DFS small test */
    testDFSOnSmallGraph(gType, gStore);

    /* 4) Dijkstra small test */
    testDijkstraOnSmallGraph(gType, gStore);

    /* 5) stressTest (random edges/vertices) */
    stressTest(gType, gStore, label, createData, cmpFunc, freeFunc);
}

/*******************************************************************
 *               Master testGraph() function
 *******************************************************************/
void testGraph(void) {
    printf("=== Starting testGraph() ===\n\n");

    /* If you want to share these arrays: */
    GraphStorage storages[2] = { GRAPH_STORAGE_LIST, GRAPH_STORAGE_MATRIX };
    GraphType types[4] = {
        GRAPH_UNDIRECTED_UNWEIGHTED,
        GRAPH_DIRECTED_UNWEIGHTED,
        GRAPH_UNDIRECTED_WEIGHTED,
        GRAPH_DIRECTED_WEIGHTED
    };

    for (int s = 0; s < 2; s++) {
        for (int t = 0; t < 4; t++) {

            /* int data */
            testAllForData("int", types[t], storages[s],
                           createDataInt, compareInt, freeInt);

            /* float data */
            testAllForData("float", types[t], storages[s],
                           createDataFloat, compareFloat, freeFloat);

            /* string data */
            testAllForData("string", types[t], storages[s],
                           createDataString, compareString, freeString);

            /* animal data */
            testAllForData("animal", types[t], storages[s],
                           createDataAnimal, compareAnimal, freeAnimal);
        }
    }

    printf("\n=== Finished testGraph() with BFS/DFS/Dijkstra + stress tests all passing! ===\n");
}
