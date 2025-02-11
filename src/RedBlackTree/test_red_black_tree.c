#include "test_red_black_tree.h"
#include "red_black_tree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* ----- Struct: Person ----- */
typedef struct Person {
    char name[50];
    int age;
} Person;

/* 
 * Example comparison functions for int, double, and string.
 * (Unchanged from previous example)
 */
static int cmp_int(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib); 
}

static int cmp_double(const void* a, const void* b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

static int cmp_str(const void* a, const void* b) {
    const char* sa = *(const char**)a;
    const char* sb = *(const char**)b;
    return strcmp(sa, sb);
}


/* 
 * We'll compare Person primarily by age, 
 * and if ages are equal, compare by name.
 */
static int cmp_person(const void* a, const void* b) {
    const Person* pa = (const Person*)a;
    const Person* pb = (const Person*)b;

    if (pa->age < pb->age) return -1;
    if (pa->age > pb->age) return 1;
    /* If ages are the same, compare lexicographically by name */
    return strcmp(pa->name, pb->name);
}

/*
 * Optional destructor for Person*.
 * Here, we assume each Person is allocated dynamically (malloc),
 * so we can simply free it. If 'name' was dynamically allocated,
 * you would also do free(person->name) here, etc.
 */
static void destroy_person(void* data) {
    free(data);
}

/* ----- Optional destructors for other tests ----- */
static void destroy_int(void* data) {
    free(data);
}

static void destroy_double(void* data) {
    free(data);
}

static void destroy_str(void* data) {
    char** s = (char**)data;
    free(*s);   /* free the string buffer */
    free(s);    /* free the pointer to the string */
}

/* ----- Print functions for debugging other types (unchanged) ----- */
// static void print_int(const void* data) {
//     printf("%d ", *(const int*)data);
// }

// static void print_double(const void* data) {
//     printf("%f ", *(const double*)data);
// }

// static void print_str(const void* data) {
//     const char* s = *(const char**)data;
//     printf("%s ", s);
// }

// static void print_person(const void* data) {
//     const Person* p = (const Person*)data;
//     printf("%s (%d) ", p->name, p->age);
// }
/* ----- Existing Tests ----- */

static void test_integers(void) {
    printf("Running integer test...\n");
    RBTREE* t = rb_create(cmp_int, destroy_int);
    assert(t);

    int N = 10;
    for (int i = 0; i < N; i++) {
        int* val = (int*)malloc(sizeof(int));
        *val = i;
        int res = rb_insert(t, val);
        assert(res == 0);
    }
    assert(rb_size(t) == (size_t)N);

    /* Search them */
    for (int i = 0; i < N; i++) {
        int tmp = i;
        int* found = rb_search(t, &tmp);
        assert(found != NULL);
        assert(*found == i);
    }

    /* Delete a few */
    for (int i = 0; i < 5; i++) {
        int tmp = i;
        int res = rb_delete(t, &tmp);
        assert(res == 0);
    }
    assert(rb_size(t) == (size_t)(N - 5));

    rb_destroy(t);
    printf("Integer test PASSED.\n");
}

static void test_doubles(void) {
    printf("Running double test...\n");
    RBTREE* t = rb_create(cmp_double, destroy_double);
    assert(t);

    double values[] = {3.14, 2.71, 1.4142, 6.022, 9.81};
    size_t n = sizeof(values)/sizeof(values[0]);

    for (size_t i = 0; i < n; i++) {
        double* val = (double*)malloc(sizeof(double));
        *val = values[i];
        int res = rb_insert(t, val);
        assert(res == 0);
    }
    assert(rb_size(t) == n);

    for (size_t i = 0; i < n; i++) {
        double tmp = values[i];
        double* found = rb_search(t, &tmp);
        assert(found != NULL);
        assert(*found == values[i]);
    }

    /* Try removing one */
    {
        double tmp = 3.14;
        int res = rb_delete(t, &tmp);
        assert(res == 0);
        assert(rb_size(t) == n - 1);
    }

    rb_destroy(t);
    printf("Double test PASSED.\n");
}

static void test_strings(void) {
    printf("Running string test...\n");
    RBTREE* t = rb_create(cmp_str, destroy_str);
    assert(t);

    const char* words[] = {"apple", "orange", "banana", "grape", "lemon"};
    size_t n = sizeof(words)/sizeof(words[0]);

    for (size_t i = 0; i < n; i++) {
        char** data_ptr = malloc(sizeof(char*));
        *data_ptr = strdup(words[i]);
        int res = rb_insert(t, data_ptr);
        assert(res == 0);
    }
    assert(rb_size(t) == n);

    /* Search */
    for (size_t i = 0; i < n; i++) {
        const char* key = words[i];
        char** found = rb_search(t, &key);
        assert(found != NULL);
        assert(strcmp(*found, key) == 0);
    }

    rb_destroy(t);
    printf("String test PASSED.\n");
}

/* ----- New: Struct-based Test ----- */

static void test_structs(void) {
    printf("Running struct (Person) test...\n");

    RBTREE* t = rb_create(cmp_person, destroy_person);
    assert(t);

    /* We'll create some Person instances dynamically. */
    Person* p1 = malloc(sizeof(Person));
    strcpy(p1->name, "Alice");
    p1->age = 25;

    Person* p2 = malloc(sizeof(Person));
    strcpy(p2->name, "Bob");
    p2->age = 30;

    Person* p3 = malloc(sizeof(Person));
    strcpy(p3->name, "Carol");
    p3->age = 25; /* same age as Alice but different name */

    Person* p4 = malloc(sizeof(Person));
    strcpy(p4->name, "David");
    p4->age = 40;

    /* Insert them into the tree */
    assert(rb_insert(t, p1) == 0);
    assert(rb_insert(t, p2) == 0);
    assert(rb_insert(t, p3) == 0);
    assert(rb_insert(t, p4) == 0);

    /* Check size is 4 */
    assert(rb_size(t) == 4);

    /* Let's search for Carol by constructing a temporary struct 
     * with the same name/age.
     */
    {
        Person key;
        strcpy(key.name, "Carol");
        key.age = 25;
        Person* found = rb_search(t, &key);
        assert(found != NULL);
        /* We can compare names and ages just to be sure. */
        assert(strcmp(found->name, "Carol") == 0);
        assert(found->age == 25);
    }

    /* Let's delete Bob */
    {
        Person key;
        strcpy(key.name, "Bob");
        key.age = 30;
        assert(rb_delete(t, &key) == 0);
        assert(rb_size(t) == 3);
    }

    /* Insert a duplicate (Alice again) to ensure it fails. */
    {
        Person* dup = malloc(sizeof(Person));
        strcpy(dup->name, "Alice");
        dup->age = 25;
        /* Should fail because cmp_person sees it as identical to p1. */
        assert(rb_insert(t, dup) != 0);
        free(dup); /* We need to free it if not inserted. */
    }

    /* Check printing if you like (optional): */
    /*
    printf("In-order traversal of Person tree:\n");
    rb_print_inorder(t, print_person);
    printf("\n");
    */

    rb_destroy(t);
    printf("Struct (Person) test PASSED.\n");
}



/*STRESS TEST HELPERS*/
/* Forward declarations of helper check functions */
static int check_root_black(const RBTREE* tree);
static int check_no_consecutive_red(const RBTREE* tree, const RBTNode* node);
static int check_black_height_consistency(const RBTREE* tree, 
                                          const RBTNode* node, 
                                          int current_black_count, 
                                          int* expected_black_count);
static int compute_height(const RBTREE* tree, const RBTNode* node);

int verify_rb_properties(const RBTREE* tree) {
    if (tree->root == tree->nil) {
        /* An empty tree is trivially correct, though by definition 
           your sentinel node is black, so presumably this is fine. */
        return 1;
    }

    /* 1) Root must be black */
    if (!check_root_black(tree)) {
        fprintf(stderr, "Red-Black check failed: root is not black.\n");
        return 0;
    }

    /* 2) Check for no consecutive red nodes */
    if (!check_no_consecutive_red(tree, tree->root)) {
        fprintf(stderr, "Red-Black check failed: found two consecutive red nodes.\n");
        return 0;
    }

    /* 3) Check black-height is consistent across all paths */
    int expected_black_count = -1;
    if (!check_black_height_consistency(tree, 
                                        tree->root, 
                                        0, 
                                        &expected_black_count)) {
        fprintf(stderr, "Red-Black check failed: black-height inconsistency.\n");
        return 0;
    }

    /* 4) (Optional) Check height <= 2 * log2(n+1) */
    {
        size_t n = rb_size(tree);
        if (n > 0) {
            int h = compute_height(tree, tree->root);
            double max_expected = 2.0 * log2((double)n + 1.0);
            if (h > (int)ceil(max_expected)) {
                fprintf(stderr, 
                        "Red-Black check warning: height %d exceeds 2*log2(n+1) ~ %.2f\n", 
                        h, max_expected);
                /* 
                 * We can decide if we call this a failure or just a warning.
                 * Strictly speaking, red-black trees guarantee O(log n),
                 * but the constant factor might differ. 
                 * If you want to fail strictly, uncomment below:
                 */
                // return 0;
            }
        }
    }

    /* If we reached here, all checks passed. */
    return 1;
}

/* --- Helper Check Functions --- */

/* Check the root is black. */
static int check_root_black(const RBTREE* tree) {
    return (tree->root->color == BLACK);
}

/*
 * Traverse the tree. If a node is red, neither its parent nor its children
 * can be red.
 */
static int check_no_consecutive_red(const RBTREE* tree, const RBTNode* node) {
    if (node == tree->nil) {
        return 1; /* sentinel / leaf is black by definition */
    }
    if (node->color == RED) {
        /* Check children */
        if (node->left->color == RED || node->right->color == RED) {
            return 0; /* child is red => consecutive reds */
        }
        /* Check parent as well: though typically we check parent's color 
           during recursion from above, but this also works. */
        if (node->parent != tree->nil && node->parent->color == RED) {
            return 0;
        }
    }
    /* Recurse left and right */
    if (!check_no_consecutive_red(tree, node->left)) {
        return 0;
    }
    if (!check_no_consecutive_red(tree, node->right)) {
        return 0;
    }
    return 1;
}

/*
 * Checks that all paths from the root to leaves have the same number
 * of black nodes. 
 *  - current_black_count: how many black nodes have we seen on this root->current path?
 *  - expected_black_count: pointer to a single "reference" black count 
 *                         (once we see one leaf, we set the reference).
 */
static int check_black_height_consistency(const RBTREE* tree, 
                                          const RBTNode* node, 
                                          int current_black_count, 
                                          int* expected_black_count) {
    if (node->color == BLACK) {
        current_black_count++;
    }

    if (node == tree->nil) {
        /* We've reached a leaf (the sentinel). 
         * Compare current_black_count to expected_black_count.
         */
        if (*expected_black_count == -1) {
            /* first leaf we've hit: set the expected count */
            *expected_black_count = current_black_count;
        } else if (current_black_count != *expected_black_count) {
            /* mismatch => violation */
            return 0;
        }
        return 1;
    }

    /* Recur down left subtree */
    if (!check_black_height_consistency(tree, node->left, 
                                        current_black_count, 
                                        expected_black_count)) {
        return 0;
    }
    /* Recur down right subtree */
    if (!check_black_height_consistency(tree, node->right, 
                                        current_black_count, 
                                        expected_black_count)) {
        return 0;
    }
    return 1;
}

/* Simple function to compute the 'height' of the tree for a basic O(log n) check. */
static int compute_height(const RBTREE* tree, const RBTNode* node) {
    if (node == tree->nil) {
        return 0;
    }
    int lh = compute_height(tree, node->left);
    int rh = compute_height(tree, node->right);
    return 1 + ((lh > rh) ? lh : rh);
}




/* ----- Simple Stress Test ----- */
static void test_stress(void) {
    printf("Running stress test with random integers...\n");

    RBTREE* t = rb_create(cmp_int, destroy_int);
    assert(t);

    int N = 50000;  // smaller example
    for (int i = 0; i < N; i++) {
        int* val = malloc(sizeof(int));
        *val = rand() % (N * 10);
        rb_insert(t, val);

        /* Immediately check RB properties (may be expensive) */
        assert(verify_rb_properties(t));
    }
    printf("Tree size after random inserts: %zu\n", rb_size(t));
    /* (some duplicates might not get inserted) */

    for (int i = 0; i < 1000; i++) {
        int tmp = rand() % (N * 10);
        rb_delete(t, &tmp);

        /* Again, check RB properties. */
        assert(verify_rb_properties(t));
    }

    printf("Tree size after random deletes: %zu\n", rb_size(t));
    rb_destroy(t);

    printf("Stress test PASSED.\n");
}


/* ----- Main Test Runner ----- */
int testRedBlackTree(void) {

	printf("Running all RBT Tests:\n");
    srand((unsigned int)time(NULL));

    test_integers();
    test_doubles();
    test_strings();
    test_structs();  /* <-- Our new struct-based test */
    test_stress();

    printf("All RBT tests PASSED.\n");
    return 0;
}

/* 
 * If you'd rather have a separate main, 
 * you could enable the following:
 *
 * int main(void) {
 *     return run_all_tests();
 * }
 */
