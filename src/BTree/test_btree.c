#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "btree_internal.h" /* So we can see BTreeNode fields for tests */

/* ------------------------------------------------------------------
 * Checking B-Tree properties
 * ------------------------------------------------------------------ */
static int check_leaf_levels(const BTreeNode *node, int level) {
    if (node->leaf) {
        return level;
    }
    int result = -1;
    for (int i = 0; i <= node->nkeys; i++) {
        int tmp = check_leaf_levels(node->children[i], level + 1);
        if (tmp == -1) return -1;
        if (result == -1) result = tmp;
        else if (tmp != result) return -1; /* mismatch => error */
    }
    return result;
}

static bool check_node_properties(const BTree *tree, const BTreeNode *node, bool isRoot) {
    int t = tree->t;
    int n = node->nkeys;

    if (!isRoot) {
        /* each non-root must have >= (t-1) keys and <= (2t-1) */
        if (n < t - 1 || n > 2 * t - 1) return false;
    } else {
        /* root: up to 2t-1, but can have fewer than t-1 if the tree is small */
        if (n > 2 * t - 1) return false;
    }

    /* check sorted order of keys in node */
    for (int i = 0; i < n - 1; i++) {
        if (tree->cmp(node->keys[i], node->keys[i + 1]) > 0) {
            return false;
        }
    }

    if (!node->leaf) {
        for (int i = 0; i <= n; i++) {
            if (!check_node_properties(tree, node->children[i], false)) {
                return false;
            }
        }
    }
    return true;
}

static bool check_btree_properties(const BTree *tree) {
    if (!tree || !tree->root) return true;
    if (!check_node_properties(tree, tree->root, true)) {
        return false;
    }
    /* Check all leaves are on the same level */
    int level = check_leaf_levels(tree->root, 0);
    if (level == -1) return false;
    return true;
}

/* ------------------------------------------------------------------
 *  Comparators / Destroyers
 * ------------------------------------------------------------------ */
static int cmp_int(const void *a, const void *b) {
    int aa = *(const int *)a;
    int bb = *(const int *)b;
    return (aa > bb) - (aa < bb);
}
static void destroy_int(void *data) {
    free(data);
}

static int cmp_double(const void *a, const void *b) {
    double aa = *(const double *)a;
    double bb = *(const double *)b;
    if (aa < bb) return -1;
    if (aa > bb) return 1;
    return 0;
}
static void destroy_double(void *data) {
    free(data);
}

static int cmp_string(const void *a, const void *b) {
    return strcmp((const char*)a, (const char*)b);
}
static void destroy_string(void *data) {
    free(data);
}

/* ------------------------------------------------------------------
 *  Individual Tests
 * ------------------------------------------------------------------ */
static void test_int_btree(BTree *tree) {
    /* Insert 1..10 */
    for (int i = 1; i <= 10; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        btree_insert(tree, val);
        assert(check_btree_properties(tree) && "B-Tree property broken after int insert");
    }
    /* Search test */
    for (int i = 1; i <= 10; i++) {
        int key = i;
        int *found = (int *)btree_search(tree, &key);
        assert(found && (*found == key) && "Failed int search");
    }
    /* Remove test */
    for (int i = 1; i <= 10; i++) {
        int key = i;
        bool removed = btree_remove(tree, &key);
        assert(removed && "Failed removing existing int key");
        assert(check_btree_properties(tree) && "B-Tree property broken after int remove");
    }
    /* Confirm all removed */
    for (int i = 1; i <= 10; i++) {
        int key = i;
        assert(btree_search(tree, &key) == NULL && "Found removed key!");
    }
}

static void test_double_btree(BTree *tree) {
    for (int i = 0; i < 10; i++) {
        double *val = malloc(sizeof(double));
        *val = i + 0.5;
        btree_insert(tree, val);
        assert(check_btree_properties(tree));
    }
    for (int i = 0; i < 10; i++) {
        double key = i + 0.5;
        double *found = btree_search(tree, &key);
        assert(found && (*found == key) && "Double search fail");
    }
    for (int i = 0; i < 10; i++) {
        double key = i + 0.5;
        bool ok = btree_remove(tree, &key);
        assert(ok && check_btree_properties(tree));
    }
    /* Confirm none remain */
    for (int i = 0; i < 10; i++) {
        double key = i + 0.5;
        assert(btree_search(tree, &key) == NULL);
    }
}

static void test_string_btree(BTree *tree) {
    const char *words[] = {"apple", "banana", "orange", "pear", "grape"};
    int n = (int)(sizeof(words) / sizeof(words[0]));

    for (int i = 0; i < n; i++) {
        char *cpy = strdup(words[i]);
        btree_insert(tree, cpy);
        assert(check_btree_properties(tree));
    }
    for (int i = 0; i < n; i++) {
        char key[32];
        snprintf(key, sizeof(key), "%s", words[i]);
        char *found = btree_search(tree, key);
        assert(found && (strcmp(found, key) == 0));
    }
    for (int i = 0; i < n; i++) {
        bool removed = btree_remove(tree, words[i]);
        assert(removed && check_btree_properties(tree));
    }
    for (int i = 0; i < n; i++) {
        assert(btree_search(tree, words[i]) == NULL);
    }
}

static void stress_test(BTree *tree, int count) {
    /* Insert [1..count] */
    for (int i = 1; i <= count; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        btree_insert(tree, val);
        assert(check_btree_properties(tree));
    }
    /* Shuffle removal order */
    int *arr = malloc(count * sizeof(int));
    for (int i = 0; i < count; i++) arr[i] = i + 1;
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
    /* Remove in random order */
    for (int i = 0; i < count; i++) {
        int key = arr[i];
        bool removed = btree_remove(tree, &key);
        assert(removed && check_btree_properties(tree));
    }
    free(arr);
}

/* ------------------------------------------------------------------
 *  Public Test Function
 * ------------------------------------------------------------------ */
void testBTree(void) {
    srand((unsigned)time(NULL));

    /* 1) Integers */
    {
        BTree *tree = btree_create(3, cmp_int, destroy_int);
        assert(tree);
        test_int_btree(tree);
        btree_destroy(tree);
    }

    /* 2) Doubles */
    {
        BTree *tree = btree_create(3, cmp_double, destroy_double);
        assert(tree);
        test_double_btree(tree);
        btree_destroy(tree);
    }

    /* 3) Strings */
    {
        BTree *tree = btree_create(3, cmp_string, destroy_string);
        assert(tree);
        test_string_btree(tree);
        btree_destroy(tree);
    }

    /* 4) Stress test with ints */
    {
        BTree *tree = btree_create(3, cmp_int, destroy_int);
        assert(tree);
        stress_test(tree, 1000); /* adjust as desired */
        btree_destroy(tree);
    }

    printf("All B-Tree tests passed successfully!\n");
}
