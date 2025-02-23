#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "graph.h"
#include "test_graph.h"

#include "graph_algorithms.h"  // <-- Include our BFS/DFS/Dijkstra




/* A simple struct to use as vertex data in one of the tests */
typedef struct {
    int id;
    char name[32];
} MyStruct;

/* A compare function for integer pointers */
static int compareInt(const void* a, const void* b) {
    /* cast to pointer-to-int */
    int intA = *(int*)a;
    int intB = *(int*)b;
    return (intA - intB);
}

/* A compare function for MyStruct pointers */
static int compareMyStruct(const void* a, const void* b) {
    /* cast to pointer-to-MyStruct */
    const MyStruct* sA = (const MyStruct*)a;
    const MyStruct* sB = (const MyStruct*)b;
    /* You can compare by id, or compare by name, or a combination. */
    if (sA->id != sB->id) {
        return sA->id - sB->id;
    }
    return strcmp(sA->name, sB->name);
}

/* A print function for integer pointers */
static void printInt(const void* data) {
    printf("%d", *(int*)data);
}

/* A print function for MyStruct pointers */
static void printMyStruct(const void* data) {
    const MyStruct* s = (const MyStruct*)data;
    printf("{id=%d, name=%s}", s->id, s->name);
}

/* A free function for integer pointers */
static void freeInt(void* data) {
    free(data); 
}

/* A free function for MyStruct pointers */
static void freeMyStruct(void* data) {
    free(data);
}

/* 
 * Test a particular type of graph with integer vertex data.
 * This function will do some basic add/remove vertex/edge tests,
 * then check invariants with assert.
 */
static void testIntGraph(GraphType type) {
    printf("Testing int graph of type %d...\n", (int)type);

    /* Create a graph with small initial capacity */
    Graph* g = createGraph(type, 4, compareInt, freeInt);
    assert(g != NULL);

    /* Add some vertices */
    for (int i = 0; i < 5; i++) {
        int* val = (int*)malloc(sizeof(int));
        *val = i;
        bool added = addVertex(g, val);
        assert(added == true);
    }
    assert(getNumVertices(g) == 5);

    /* Add edges among them */
    bool ok = addEdge(g, &((int){0}), &((int){1}), 2.5);
    assert(ok == true);
    ok = addEdge(g, &((int){0}), &((int){2}), 3.0);
    assert(ok == true);

    /* check presence */
    double w = 0.0;
    bool hasE = hasEdge(g, &((int){0}), &((int){1}), &w);
    assert(hasE == true);
    /* Weighted or not, if unweighted, w=1.0, if weighted, w=2.5 (due to above call) */
    if (type == GRAPH_UNDIRECTED_WEIGHTED || type == GRAPH_DIRECTED_WEIGHTED) {
        assert(w == 2.5);
    } else {
        assert(w == 1.0);
    }

    /* remove an edge */
    ok = removeEdge(g, &((int){0}), &((int){1}));
    assert(ok == true);

    hasE = hasEdge(g, &((int){0}), &((int){1}), NULL);
    assert(hasE == false);

    /* remove a vertex */
    ok = removeVertex(g, &((int){2}));
    assert(ok == true);
    assert(getNumVertices(g) == 4);

    /* Clean up */
    destroyGraph(g);

    printf("Int graph of type %d passed.\n", (int)type);
}

/*
 * Test a particular type of graph with MyStruct vertex data.
 * We do similar operations but test the struct-based compare and free.
 */
static void testStructGraph(GraphType type) {
    printf("Testing struct graph of type %d...\n", (int)type);

    Graph* g = createGraph(type, 2, compareMyStruct, freeMyStruct);
    assert(g != NULL);

    /* Add some struct vertices */
    MyStruct* s1 = (MyStruct*)malloc(sizeof(MyStruct));
    s1->id = 100; strcpy(s1->name, "Alpha");
    addVertex(g, s1);

    MyStruct* s2 = (MyStruct*)malloc(sizeof(MyStruct));
    s2->id = 200; strcpy(s2->name, "Beta");
    addVertex(g, s2);

    assert(getNumVertices(g) == 2);

    /* Add edge */
    bool ok = addEdge(g, s1, s2, 10.0);
    assert(ok == true);

    double w = 0.0;
    bool hasE = hasEdge(g, s1, s2, &w);
    assert(hasE == true);

    if (type == GRAPH_UNDIRECTED_WEIGHTED || type == GRAPH_DIRECTED_WEIGHTED) {
        assert(w == 10.0);
    } else {
        assert(w == 1.0);
    }

    /* remove the edge */
    removeEdge(g, s1, s2);
    hasE = hasEdge(g, s1, s2, NULL);
    assert(hasE == false);

    destroyGraph(g);

    printf("Struct graph of type %d passed.\n", (int)type);
}

static void testBFSAndDFS(GraphType type) {
    printf("Testing BFS/DFS on graph type %d...\n", (int)type);

    Graph* g = createGraph(type, 5, compareInt, freeInt);
    assert(g);

    /* Add vertices 0,1,2,3,4 */
    for (int i = 0; i < 5; i++) {
        int* val = (int*)malloc(sizeof(int));
        *val = i;
        addVertex(g, val);
    }

    /* Add some edges */
    addEdge(g, &((int){0}), &((int){1}), 1.0);
    addEdge(g, &((int){0}), &((int){2}), 1.0);
    addEdge(g, &((int){1}), &((int){3}), 1.0);
    addEdge(g, &((int){2}), &((int){4}), 1.0);

    /* BFS from 0 */
    int bfsCount;
    void** bfsOrder = graphBFS(g, &((int){0}), &bfsCount);
    printf("BFS order: ");
    for (int i = 0; i < bfsCount; i++) {
        int* v = (int*)bfsOrder[i];
        printf("%d ", *v);
    }
    printf("\n");
    free(bfsOrder);  /* we allocated it in BFS */

    /* DFS from 0 */
    int dfsCount;
    void** dfsOrder = graphDFS(g, &((int){0}), &dfsCount);
    printf("DFS order: ");
    for (int i = 0; i < dfsCount; i++) {
        int* v = (int*)dfsOrder[i];
        printf("%d ", *v);
    }
    printf("\n");
    free(dfsOrder);

    destroyGraph(g);
    printf("BFS/DFS test done for graph type %d.\n\n", (int)type);
}

static void testDijkstra(GraphType type) {
    printf("Testing Dijkstra on graph type %d...\n", (int)type);

    Graph* g = createGraph(type, 5, compareInt, freeInt);
    assert(g);

    /* Add vertices 0..4 */
    for (int i = 0; i < 5; i++) {
        int* val = (int*)malloc(sizeof(int));
        *val = i;
        addVertex(g, val);
    }

    /* We'll attach some weights. 
     * If the graph is unweighted, edges are effectively 1.0 anyway.
     * For weighted, we can test the actual distances.
     */
    addEdge(g, &((int){0}), &((int){1}), 2.0);
    addEdge(g, &((int){0}), &((int){2}), 5.0);
    addEdge(g, &((int){1}), &((int){2}), 1.0);
    addEdge(g, &((int){1}), &((int){3}), 4.0);
    addEdge(g, &((int){2}), &((int){3}), 2.0);
    addEdge(g, &((int){2}), &((int){4}), 10.0);
    addEdge(g, &((int){3}), &((int){4}), 3.0);

    /* Dijkstra from 0 */
    double* dist = graphDijkstra(g, &((int){0}));
    if (dist) {
        printf("Distances from vertex 0:\n");
        for (int i = 0; i < getNumVertices(g); i++) {
            printf("  to vertex %d: ", i);
            if (dist[i] >= 1e9) { /* or check if dist[i] == DBL_MAX */
                printf("unreachable\n");
            } else {
                printf("%.2f\n", dist[i]);
            }
        }
        free(dist);
    }

    destroyGraph(g);
    printf("Dijkstra test done for graph type %d.\n\n", (int)type);
}

/*
 * Main test driver that runs all tests. 
 * We test each of the 4 graph types with both int and struct data.
 */
void testGraph(void) {
    printf("=== Starting testGraph() ===\n\n");

    /* Test each GraphType with integer data */
    testIntGraph(GRAPH_UNDIRECTED_UNWEIGHTED);
    testIntGraph(GRAPH_DIRECTED_UNWEIGHTED);
    testIntGraph(GRAPH_UNDIRECTED_WEIGHTED);
    testIntGraph(GRAPH_DIRECTED_WEIGHTED);

    /* Test each GraphType with struct data */
    testStructGraph(GRAPH_UNDIRECTED_UNWEIGHTED);
    testStructGraph(GRAPH_DIRECTED_UNWEIGHTED);
    testStructGraph(GRAPH_UNDIRECTED_WEIGHTED);
    testStructGraph(GRAPH_DIRECTED_WEIGHTED);


    /* BFS/DFS tests */
    testBFSAndDFS(GRAPH_UNDIRECTED_UNWEIGHTED);
    testBFSAndDFS(GRAPH_DIRECTED_UNWEIGHTED);
    testBFSAndDFS(GRAPH_UNDIRECTED_WEIGHTED);
    testBFSAndDFS(GRAPH_DIRECTED_WEIGHTED);

    /* Dijkstra tests */
    testDijkstra(GRAPH_UNDIRECTED_UNWEIGHTED);
    testDijkstra(GRAPH_DIRECTED_UNWEIGHTED);
    testDijkstra(GRAPH_UNDIRECTED_WEIGHTED);
    testDijkstra(GRAPH_DIRECTED_WEIGHTED);

    printf("\n=== All tests passed! ===\n");
}
