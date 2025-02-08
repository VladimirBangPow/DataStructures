#include "test_avl.h"
#include "avl_tree.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* 
 * Comparison functions for different data types:
 * int, double, and C-string (char *).
 */
static int compare_int(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

static int compare_double(const void *a, const void *b) {
    double x = *(const double *)a;
    double y = *(const double *)b;
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}

static int compare_str(const void *a, const void *b) {
    /* Compare C-strings */
    return strcmp((const char *)a, (const char *)b);
}

/* 
 * Free functions: 
 * If we allocated memory for int/double/string, 
 * we need to free it here. If they are stack allocated, do not free.
 */

/* For heap-allocated integers */
static void free_int(void *data) {
    free(data);
}

/* For heap-allocated doubles */
static void free_double(void *data) {
    free(data);
}

/* For heap-allocated strings */
static void free_str(void *data) {
    free(data);
}

/* --------------------------------------------------
 *  Test: Basic insertion/search/removal with int
 * -------------------------------------------------- */
static void test_integers_basic(void) {
    AvlTree *tree = avl_create(compare_int);

    /* Insert some integers */
    int *val1 = malloc(sizeof(int)); *val1 = 10;
    int *val2 = malloc(sizeof(int)); *val2 = 5;
    int *val3 = malloc(sizeof(int)); *val3 = 20;
    int *val4 = malloc(sizeof(int)); *val4 = 15;

    avl_insert(tree, val1);
    avl_insert(tree, val2);
    avl_insert(tree, val3);
    avl_insert(tree, val4);

    /* Search */
    int key = 5;
    int *found = (int *)avl_search(tree, &key);
    assert(found != NULL && *found == 5);

    /* Remove */
    int key_remove = 10;
    avl_remove(tree, &key_remove, free_int);
    /* Searching for 10 should now fail */
    int *found2 = (int *)avl_search(tree, &key_remove);
    assert(found2 == NULL);

    /* Cleanup the rest */
    key = 5;   avl_remove(tree, &key, free_int); 
    key = 20;  avl_remove(tree, &key, free_int);
    key = 15;  avl_remove(tree, &key, free_int);

    avl_destroy(tree, NULL); 
    /* We pass NULL here because we've already freed
       the data for every node we removed. */
    printf("[test_integers_basic] Passed.\n");
}

/* --------------------------------------------------
 *  Test: Basic insertion with double
 * -------------------------------------------------- */
static void test_doubles_basic(void) {
    AvlTree *tree = avl_create(compare_double);

    double *v1 = malloc(sizeof(double)); *v1 = 3.14;
    double *v2 = malloc(sizeof(double)); *v2 = 2.71;
    double *v3 = malloc(sizeof(double)); *v3 = 1.41;

    avl_insert(tree, v1);
    avl_insert(tree, v2);
    avl_insert(tree, v3);

    double key = 2.71;
    double *found = (double *)avl_search(tree, &key);
    assert(found && *found == 2.71);

    /* Cleanup everything */
    avl_destroy(tree, free_double);

    printf("[test_doubles_basic] Passed.\n");
}

/* --------------------------------------------------
 *  Test: Basic insertion with string
 * -------------------------------------------------- */
static void test_strings_basic(void) {
    AvlTree *tree = avl_create(compare_str);

    /* Insert heap-allocated strings */
    char *s1 = strdup("apple");
    char *s2 = strdup("banana");
    char *s3 = strdup("cherry");

    avl_insert(tree, s1);
    avl_insert(tree, s2);
    avl_insert(tree, s3);

    /* Search */
    char *key = "banana";
    char *found = (char *)avl_search(tree, key);
    assert(found != NULL && strcmp(found, "banana") == 0);

    /* Remove "banana" */
    avl_remove(tree, key, free_str);
    found = (char *)avl_search(tree, key);
    assert(found == NULL);

    avl_destroy(tree, free_str);

    printf("[test_strings_basic] Passed.\n");
}

/* --------------------------------------------------
 *  Stress Test
 *  - Insert a large number of random integers
 *  - Search for them
 *  - Remove them
 * -------------------------------------------------- */
static void test_stress_int(void) {
    AvlTree *tree = avl_create(compare_int);

    const int N = 10000;
    int *values = (int *)malloc(sizeof(int) * N);

    srand((unsigned int)time(NULL));

    /* Insert random integers into the tree */
    for (int i = 0; i < N; i++) {
        values[i] = rand() % (N * 10);
        int *heap_val = malloc(sizeof(int));
        *heap_val = values[i];
        avl_insert(tree, heap_val);
    }

    /* Search for first 100 in the array */
    for (int i = 0; i < 100; i++) {
        int key = values[i];
        int *found = (int *)avl_search(tree, &key);
        assert(found != NULL && *found == key);
    }

    /* Remove all values */
    for (int i = 0; i < N; i++) {
        avl_remove(tree, &values[i], free_int);
    }

    free(values);
    avl_destroy(tree, NULL);

    printf("[test_stress_int] Passed (N = %d).\n", N);
}

/* 
 * Run all the tests 
 */
void testAvlTree(void) {
    printf("Running AVL Tree tests...\n");
    /* Basic tests */
    test_integers_basic();
    test_doubles_basic();
    test_strings_basic();
    test_stress_int();
    printf("All AVL Tree tests passed.\n");
}
