#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "test_graph.h"
#include "graph.h"   /* or whichever header declares createGraphImplementation(...) */

/* 
 * We assume you have definitions like:
 *   GraphType { GRAPH_UNDIRECTED_UNWEIGHTED, GRAPH_DIRECTED_UNWEIGHTED, ... }
 *   GraphStorage { GRAPH_STORAGE_LIST, GRAPH_STORAGE_MATRIX }
 * plus the standard Graph* createGraphImplementation(...).
 */

/*******************************************************************
 *                   Data Types & Utilities
 *******************************************************************/


 /* ----------------------------------------------------------------
   Helpers to check if the graph is weighted or directed
   (you can also do these checks inline if you prefer).
   ---------------------------------------------------------------- */
static bool isWeighted(GraphType t) {
    return (t == GRAPH_UNDIRECTED_WEIGHTED || t == GRAPH_DIRECTED_WEIGHTED);
}
static bool isDirected(GraphType t) {
    return (t == GRAPH_DIRECTED_UNWEIGHTED || t == GRAPH_DIRECTED_WEIGHTED);
}

/* 1) Integers: we allocate an int* so we can store distinct pointers. */
static int compareInt(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}
static void freeInt(void* data) {
    free(data);
}

/* 2) Floats: we store float* similarly. */
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

/* 3) Strings: we store char* (dynamic). We'll compare by strcmp. */
static int compareString(const void* a, const void* b) {
    const char* sa = (const char*)a;
    const char* sb = (const char*)b;
    return strcmp(sa, sb);
}
static void freeString(void* data) {
    free(data);
}

/* 4) Animal struct: a simple example. */
typedef struct {
    char name[32];
    int  age;
} Animal;

static int compareAnimal(const void* a, const void* b) {
    const Animal* A = (const Animal*)a;
    const Animal* B = (const Animal*)b;
    /* compare by name first, then age if tie */
    int c = strcmp(A->name, B->name);
    if (c != 0) return c;
    return (A->age - B->age);
}
static void freeAnimal(void* data) {
    /* data was malloc'ed so just free */
    free(data);
}

/* Helper to create a new Animal* */
static Animal* makeAnimal(const char* name, int age) {
    Animal* a = (Animal*)malloc(sizeof(Animal));
    strcpy(a->name, name);
    a->age = age;
    return a;
}

/* 
 * We'll define macros to easily iterate over all GraphType/GraphStorage combos.
 */
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
 *           Test Routines for each Data Type
 *******************************************************************/

/*
 * We'll define a small function that:
 *   1. Creates a graph with a given GraphType, GraphStorage.
 *   2. Adds 3 vertices (v0, v1, v2) => check getNumVertices == 3
 *   3. Adds edges between them => check getNumEdges etc.
 *   4. Removes some edges => check again
 *   5. Removes a vertex => check again
 *
 * This is a minimal test to ensure basic correctness. 
 * We'll do a bigger "stressTest" afterwards.
 *
 * The 'createData' parameter is a function pointer that we call 
 *   to get a "new instance" of the data type for each vertex.
 * The 'cmpFunc' and 'freeFunc' are the compare/free for that data type.
 * 
 * We'll keep printing to a minimum, mostly rely on assert().
 */
typedef void* (*CreateDataFn)(int index);

static void basicTest(GraphType type,
                      GraphStorage storage,
                      const char* label, /* e.g. "int", "float", "string", "Animal" */
                      CreateDataFn createData,
                      int  (*cmpFunc)(const void*, const void*),
                      void (*freeFunc)(void*)) 
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
    /* If unweighted or weighted, same # of edges. For directed => edges = 2 so far. 
     * For undirected => edges = 2 as well, but each edge stored in adjacency. 
     * So # edges => 2 total. 
     */
    assert(ecount == 2);

    /* Check hasEdge */
    double w = -999.0;
    bool he = hasEdge(g, v0, v1, &w);
    assert(he == true);
    if (type == GRAPH_UNDIRECTED_WEIGHTED || type == GRAPH_DIRECTED_WEIGHTED) {
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

    /* Minimal print to confirm success */
    /* (You could remove this for minimal output; but let's keep a short line) */
    printf("[OK] basicTest <%s>, %s\n", label, 
           (storage == GRAPH_STORAGE_LIST ? "adj_list" : "adj_matrix"));
}

/*******************************************************************
 *     Stress Test: many inserts, edges, random checks, etc.
 *******************************************************************/

/*
 * This function performs a bigger stress test. 
 * We'll create (n) vertices, do random edges, check # of edges, 
 * do random removeEdge, removeVertex, etc.
 * We'll do some random "hasEdge" checks too.
 *
 * We'll keep the test somewhat short, but enough to show we can scale 
 * and detect issues. We'll also use assert for meaningful checks. 
 */
#include <time.h>

static void stressTest(GraphType type,
                       GraphStorage storage,
                       const char* label,
                       CreateDataFn createData,
                       int  (*cmpFunc)(const void*, const void*),
                       void (*freeFunc)(void*)) 
{
    srand((unsigned int)time(NULL)); /* simple seed */

    Graph* g = createGraphImplementation(type, storage, 16, cmpFunc, freeFunc);
    assert(g != NULL);

    /* We'll create up to n vertices */
    int n = 50;
    /* store pointers in an array so we can remove them easily */
    void** arr = (void**)malloc(sizeof(void*) * (size_t)n);
    for (int i = 0; i < n; i++) {
        arr[i] = createData(i);
        bool ok = addVertex(g, arr[i]);
        assert(ok == true);
    }
    assert(getNumVertices(g) == n);

    /* We'll add some random edges. 
     * We won't go too big, just a few times n, to keep it from exploding.
     */
    int edgeAttempts = n * 3;
    for (int i = 0; i < edgeAttempts; i++) {
        int srcIdx = rand() % n;
        int dstIdx = rand() % n;
        if (srcIdx == dstIdx) continue; /* skip self loop for simplicity */
        double w = 1.0 + (rand() % 10); /* random weight 1..10 */
        bool ok = addEdge(g, arr[srcIdx], arr[dstIdx], w);
        /* even if it returns false for duplicates, that's fine */
        (void)ok;
    }

    /* Now we do some random checks */
    for (int i = 0; i < 20; i++) {
        int srcIdx = rand() % n;
        int dstIdx = rand() % n;
        double w = -1.0;
        bool he = hasEdge(g, arr[srcIdx], arr[dstIdx], &w);
        if (he) {
            /* if edge is found, then w >= 0.0 (or 1.0 for unweighted). 
             * Weighted => possibly the actual random weight. 
             */
            if (!isWeighted(type)) {
                assert(w == 1.0);
            } else {
                assert(w >= 1.0 && w <= 10.0);
            }
        } else {
            /* no edge => w remains -1.0 or so, no big check needed */
        }
    }

    /* Remove ~10 vertices randomly */
    int removeCount = 10;
    int removed = 0;
    for (int i = 0; i < removeCount; i++) {
        int idx = rand() % n;
        if (arr[idx] != NULL) {
            bool ok = removeVertex(g, arr[idx]);
            if (ok) {
                arr[idx] = NULL; /* mark we removed it */
                removed++;
            }
        }
    }
    assert(getNumVertices(g) == (n - removed));

    /* remove some edges randomly */
    int edgeRemoveAttempts = 20;
    for (int i = 0; i < edgeRemoveAttempts; i++) {
        int srcIdx = rand() % n;
        int dstIdx = rand() % n;
        if (arr[srcIdx] && arr[dstIdx]) {
            bool ok = removeEdge(g, arr[srcIdx], arr[dstIdx]);
            /* might be false if edge didn't exist, that's fine */
            (void)ok;
        }
    }

    /* Clean up */
    destroyGraph(g);

    // /* free array of pointers. We allocated them with createData. */
    // for (int i = 0; i < n; i++) {
    //     if (arr[i]) {
    //         freeFunc(arr[i]);
    //     }
    // }
    free(arr);

    printf("[OK] stressTest <%s>, %s\n", label,
           (storage == GRAPH_STORAGE_LIST ? "adj_list" : "adj_matrix"));
}

/*******************************************************************
 *               Master testGraph() function
 *******************************************************************/

/* 
 * We'll define "createData" functions for each of the data types. 
 * They produce a new pointer for a given index (just for demonstration).
 */

static void* createDataInt(int i) {
    int* p = (int*)malloc(sizeof(int));
    *p = i;
    return p;
}

static void* createDataFloat(int i) {
    float* p = (float*)malloc(sizeof(float));
    *p = (float)(i * 1.1); /* example: 0.0, 1.1, 2.2, etc. */
    return p;
}

static void* createDataString(int i) {
    /* We'll create dynamic strings like "str0", "str1", etc. */
    char buf[32];
    snprintf(buf, sizeof(buf), "str%d", i);
    char* s = strdup(buf); /* duplicates into heap */
    return s;
}

static void* createDataAnimal(int i) {
    /* We'll create animals named "Animal_i", age i. */
    char name[32];
    snprintf(name, sizeof(name), "Animal_%d", i);
    Animal* a = makeAnimal(name, i);
    return a;
}

/* 
 * Now we define a helper that runs the basicTest + stressTest 
 * for a given (type, storage) and data config. 
 */
static void runAllTestsForData(const char* dataLabel,
                               GraphType gType,
                               GraphStorage gStore,
                               CreateDataFn createData,
                               int  (*cmpFunc)(const void*, const void*),
                               void (*freeFunc)(void*))
{
    /* Basic test */
    basicTest(gType, gStore, dataLabel, createData, cmpFunc, freeFunc);

    /* Stress test */
    stressTest(gType, gStore, dataLabel, createData, cmpFunc, freeFunc);
}

/* 
 * The master testGraph() which enumerates over:
 *   - each GraphType
 *   - each GraphStorage
 *   - each Data Type
 */
void testGraph(void) {
    printf("=== Starting testGraph() ===\n");

    for (int st = 0; st < 2; st++) {
        GraphStorage store = g_storages[st];
        for (int gt = 0; gt < 4; gt++) {
            GraphType gtype = g_types[gt];

            /* Test int */
            runAllTestsForData("int", gtype, store, createDataInt, compareInt, freeInt);

            /* Test float */
            runAllTestsForData("float", gtype, store, createDataFloat, compareFloat, freeFloat);

            /* Test string */
            runAllTestsForData("string", gtype, store, createDataString, compareString, freeString);

            /* Test Animal */
            runAllTestsForData("animal", gtype, store, createDataAnimal, compareAnimal, freeAnimal);
        }
    }

    printf("=== Finished testGraph() with all tests passing! ===\n");
}
