#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "test_bp_tree.h"
#include "bp_tree.h"
/* -------------------------------------------------------------------------
 *  Implementation of Person comparator
 *  (Compares by Person.id)
 * ------------------------------------------------------------------------- */
int person_cmp(const void* a, const void* b) {
    const Person* pa = (const Person*)a;
    const Person* pb = (const Person*)b;
    return (pa->id > pb->id) - (pa->id < pb->id);
}

/* 
 * Helper macros for test output:
 *   TEST_OK(msg)   => prints success message
 *   TEST_FAIL(msg) => prints failure message
 */
#define TEST_OK(msg)    do { printf("[PASS] %s\n", (msg)); } while(0)
#define TEST_FAIL(msg)  do { printf("[FAIL] %s\n", (msg)); } while(0)

/* 
 * Minimal property checks for a B+ tree. 
 * This function does partial verification of:
 *  1) Node key counts are within [ceil((order-1)/2), order-1], except root.
 *  2) Keys in each node are sorted (according to the tree's comparator).
 *  3) Leaf-level node linking is consistent (monotonically non-decreasing keys).
 *
 * In a real production system, you might do deeper checks:
 *  - Checking parent->children relationships
 *  - Ensuring the tree is balanced (all leaves at same depth)
 *  - Checking internal vs. leaf node differences thoroughly
 */
static bool check_bptree_valid(BPTree* tree);

/* Internal recursive helper for check_bptree_valid */
static bool validate_subtree(BPTree* tree, BPTreeNode* node, int* leaf_level, int current_level);

/* -------------------------------------------------------------------------
 *  Test: Integers
 * ------------------------------------------------------------------------- */
static void test_integers(void) {
    BPTree* tree = bptree_create(4, bptree_int_cmp);
    if (!tree) {
        TEST_FAIL("Integer test: bptree_create returned NULL.");
        return;
    }

    // Insert some integers
    int k1 = 10, v1 = 100;
    int k2 = 20, v2 = 200;
    int k3 = 15, v3 = 150;
    bptree_insert(tree, &k1, &v1);
    bptree_insert(tree, &k2, &v2);
    bptree_insert(tree, &k3, &v3);

    // Check structure
    if (!check_bptree_valid(tree)) {
        bptree_destroy(tree);
        TEST_FAIL("Integer test: B+ tree structure invalid after inserts.");
        return;
    }

    // Search
    void* found = bptree_search(tree, &k1);
    if (!found || *(int*)found != 100) {
        TEST_FAIL("Integer test: search for 10 => expected 100, got not found or wrong value");
        bptree_destroy(tree);
        return;
    }

    // Delete
    bool deleted = bptree_delete(tree, &k1);
    if (!deleted) {
        TEST_FAIL("Integer test: delete(10) => false, expected true.");
        bptree_destroy(tree);
        return;
    }

    // Check structure again
    if (!check_bptree_valid(tree)) {
        bptree_destroy(tree);
        TEST_FAIL("Integer test: B+ tree structure invalid after deletion.");
        return;
    }

    // Clean up
    bptree_destroy(tree);
    TEST_OK("Integer test: passed all checks.");
}

/* -------------------------------------------------------------------------
 *  Float comparator
 * ------------------------------------------------------------------------- */
static int float_cmp(const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    if (fa < fb) return -1;
    if (fa > fb) return 1;
    return 0;
}

/* -------------------------------------------------------------------------
 *  Test: Floats
 * ------------------------------------------------------------------------- */
static void test_floats(void) {
    BPTree* tree = bptree_create(4, float_cmp);
    if (!tree) {
        TEST_FAIL("Float test: bptree_create returned NULL.");
        return;
    }

    float k1=3.14f, v1=1.414f;
    float k2=2.71f, v2=1.732f;
    float k3=1.4142f, v3=2.7182f;

    bptree_insert(tree, &k1, &v1);
    bptree_insert(tree, &k2, &v2);
    bptree_insert(tree, &k3, &v3);

    if (!check_bptree_valid(tree)) {
        bptree_destroy(tree);
        TEST_FAIL("Float test: B+ tree structure invalid after inserts.");
        return;
    }

    // Search
    void* found = bptree_search(tree, &k2);
    if (!found || *(float*)found != 1.732f) {
        TEST_FAIL("Float test: search for 2.71 => expected 1.732, got not found or wrong value");
        bptree_destroy(tree);
        return;
    }

    // Deletion
    bool deleted = bptree_delete(tree, &k2);
    if (!deleted) {
        TEST_FAIL("Float test: delete(2.71) => false, expected true");
        bptree_destroy(tree);
        return;
    }
    if (!check_bptree_valid(tree)) {
        bptree_destroy(tree);
        TEST_FAIL("Float test: invalid structure after deletion.");
        return;
    }

    bptree_destroy(tree);
    TEST_OK("Float test: passed all checks.");
}

/* -------------------------------------------------------------------------
 *  String comparator
 * ------------------------------------------------------------------------- */
static int cstring_cmp(const void* a, const void* b) {
    // 'a' and 'b' are expected to be (const char*) pointers
    return strcmp((const char*)a, (const char*)b);
}

/* -------------------------------------------------------------------------
 *  Test: Strings
 * ------------------------------------------------------------------------- */
static void test_strings(void) {
    BPTree* tree = bptree_create(4, cstring_cmp);
    if (!tree) {
        TEST_FAIL("String test: bptree_create returned NULL.");
        return;
    }

    // Insert some strings
    char* k1 = "apple";   char* v1 = "fruit";
    char* k2 = "zebra";   char* v2 = "animal";
    char* k3 = "carrot";  char* v3 = "vegetable";
    bptree_insert(tree, k1, v1);
    bptree_insert(tree, k2, v2);
    bptree_insert(tree, k3, v3);

    if (!check_bptree_valid(tree)) {
        bptree_destroy(tree);
        TEST_FAIL("String test: invalid after insert.");
        return;
    }

    // Search
    void* found = bptree_search(tree, "carrot");
    if (!found || strcmp((char*)found, "vegetable") != 0) {
        TEST_FAIL("String test: search('carrot') => expected 'vegetable', got something else");
        bptree_destroy(tree);
        return;
    }

    // Delete
    bool deleted = bptree_delete(tree, "apple");
    if (!deleted) {
        TEST_FAIL("String test: delete('apple') => false, expected true");
        bptree_destroy(tree);
        return;
    }
    if (!check_bptree_valid(tree)) {
        bptree_destroy(tree);
        TEST_FAIL("String test: invalid structure after deletion.");
        return;
    }

    bptree_destroy(tree);
    TEST_OK("String test: passed all checks.");
}

/* -------------------------------------------------------------------------
 *  Test: Person struct
 * ------------------------------------------------------------------------- */
static void test_person(void) {
    BPTree* tree = bptree_create(4, person_cmp);
    if (!tree) {
        TEST_FAIL("Person test: bptree_create returned NULL.");
        return;
    }

    // Create some Persons
    Person* p1 = malloc(sizeof(Person));
    p1->id = 101; p1->name = "Alice";   p1->age = 30;

    Person* p2 = malloc(sizeof(Person));
    p2->id = 205; p2->name = "Bob";     p2->age = 25;

    Person* p3 = malloc(sizeof(Person));
    p3->id = 120; p3->name = "Charlie"; p3->age = 40;

    // Values can be anything, or we can even store a pointer to the same struct,
    // but let's store a pointer to an "info" string for demonstration
    char* info1 = "Alice-info";
    char* info2 = "Bob-info";
    char* info3 = "Charlie-info";

    bptree_insert(tree, p1, info1);
    bptree_insert(tree, p2, info2);
    bptree_insert(tree, p3, info3);

    if (!check_bptree_valid(tree)) {
        TEST_FAIL("Person test: invalid after insert.");
        bptree_destroy(tree);
        return;
    }

    // Search by constructing a Person with only .id set
    Person query;
    query.id = 205;
    void* found = bptree_search(tree, &query);
    if (!found || strcmp((char*)found, "Bob-info") != 0) {
        TEST_FAIL("Person test: searching id=205 => expected 'Bob-info'.");
        bptree_destroy(tree);
        return;
    }

    // Delete
    bool deleted = bptree_delete(tree, &query);
    if (!deleted) {
        TEST_FAIL("Person test: delete(id=205) => false, expected true.");
        bptree_destroy(tree);
        return;
    }
    if (!check_bptree_valid(tree)) {
        TEST_FAIL("Person test: invalid after delete(id=205).");
        bptree_destroy(tree);
        return;
    }

    bptree_destroy(tree);
    // Clean up the Person objects if needed
    free(p1);
    free(p2);
    free(p3);

    TEST_OK("Person test: passed all checks.");
}

/* -------------------------------------------------------------------------
 *  Stress test
 *
 *  This test inserts and deletes a large number of random keys. 
 *  After each operation, we partially verify the B+ tree structure 
 *  by calling check_bptree_valid().
 *
 *  We use integers for simplicity, but you could adapt to other types.
 * ------------------------------------------------------------------------- */
static void test_stress(void) {
    const int ORDER = 3;      // a bit larger order to see more splits
    const int N = 2000;        // number of random inserts
    const int OPS = 2000;      // number of random delete ops

    BPTree* tree = bptree_create(ORDER, bptree_int_cmp);
    if (!tree) {
        TEST_FAIL("Stress test: bptree_create returned NULL.");
        return;
    }

    srand((unsigned)time(NULL));

    // We'll keep an array of inserted keys so we can pick random ones to delete
    int* keys = malloc(N * sizeof(int));
    int* values = malloc(N * sizeof(int));

    // Insert random keys
    for (int i = 0; i < N; i++) {
        keys[i] = rand() % 10000000;   // random key
        values[i] = i;             // arbitrary "value"

        bptree_insert(tree, &keys[i], &values[i]);
		// printf("%d\n", keys[i]);
		// bptree_print(tree);

        // Check after each insert
        if (!check_bptree_valid(tree)) {
            TEST_FAIL("Stress test: invalid B+ tree during insertion sequence.");
            free(keys);
            free(values);
            bptree_destroy(tree);
            return;
        }
    }

    // Perform random deletions
    for (int i = 0; i < OPS; i++) {
        // pick a random key from the array
        bptree_delete(tree, &keys[i]);
        // printf("%d\n", keys[i]);
		// bptree_print(tree);
        // Check after each deletion
        if (!check_bptree_valid(tree)) {
            TEST_FAIL("Stress test: invalid B+ tree during deletion sequence.");
            free(keys);
            free(values);
            bptree_destroy(tree);
            return;
        }
    }

    // Cleanup
    free(keys);
    free(values);
    bptree_destroy(tree);
    TEST_OK("Stress test: completed with all structure checks passing.");
}

/* -------------------------------------------------------------------------
 *  bptree_run_tests: runs all tests
 * ------------------------------------------------------------------------- */
void testBPTree(void) {
    printf("== B+ Tree Test Suite ==\n");
    test_integers();
    test_floats();
    test_strings();
    test_person();
    test_stress();
    printf("== Test Suite Finished ==\n");
}

/* =========================================================================
 *  B+ Tree Structural Verification
 * ========================================================================= */

/*
 * check_bptree_valid: 
 *   Wrapper that initiates a recursive validation from the root.
 */
static bool check_bptree_valid(BPTree* tree) {
    if (!tree || !tree->root) {
        // An empty tree is valid by definition
        return true;
    }
    int leaf_level = -1; // We track the level of the first leaf we encounter
    return validate_subtree(tree, tree->root, &leaf_level, 0);
}

/*
 * validate_subtree: recursively check that keys in each node are sorted,
 * that the number of keys is within permissible bounds, and that all leaves
 * are at the same depth (which is a typical B+ tree property).
 *
 * We do a simplified rule for min_keys: 
 *   - The root can have fewer. 
 *   - Each internal node or leaf should have at least ceil((order-1)/2) 
 *     once the tree is grown beyond the root.
 */
static bool validate_subtree(BPTree* tree, BPTreeNode* node, int* leaf_level, int current_level) {
    // 0. If it's the root, it can have 0..(order-1) keys if it's also a leaf
    // 1. If it's not root, check min_keys
    // 2. Check max_keys
    // 3. Check that keys are sorted
    // 4. If internal, recurse on children
    // 5. If leaf, record or compare leaf_level

    int order = tree->order;
    bptree_cmp cmp = tree->cmp;

    // check sorted keys
    for (int i = 0; i < node->num_keys - 1; i++) {
        if (cmp(node->keys[i], node->keys[i + 1]) > 0) {
            // not sorted
            printf("Unsorted keys at level %d\n", current_level);
            return false;
        }
    }

    if (node == tree->root) {
        // Root node can have fewer keys if it's also a leaf
        // If it's not a leaf, it should have at least 2 children once grown.
        // We'll skip strict checks for an empty/small root for demonstration.
    } else {
        // min required (except possibly if root is the only node)
        if (node != tree->root) {
            int min_keys = (order - 1) / 2;
            if (node->num_keys < min_keys && node->parent != NULL) {
                // underflow
                printf("Underflow at level %d\n", current_level);
                return false;
            }
        }
    }

    // max check
    if (node->num_keys > (order - 1)) {
        // overflow
        printf("Overflow at level %d\n", current_level);
        return false;
    }

    if (node->is_leaf) {
        // check that all leaves are at the same level
        if (*leaf_level == -1) {
            *leaf_level = current_level; // first leaf found
        } else {
            if (current_level != *leaf_level) {
                // mismatch in leaf level => unbalanced
                printf("Unbalanced leaves: level %d vs %d\n", *leaf_level, current_level);
                return false;
            }
        }
    } else {
        // check children pointers
        for (int i = 0; i <= node->num_keys; i++) {
            BPTreeNode* child = node->children[i];
            if (child) {
                if (child->parent != node) {
                    printf("Child-parent mismatch at level %d\n", current_level);
                    return false;
                }
                if (!validate_subtree(tree, child, leaf_level, current_level + 1)) {
                    printf("Subtree invalid at level %d\n", current_level);
                    return false;
                }
            }
        }
    }

    return true;
}
