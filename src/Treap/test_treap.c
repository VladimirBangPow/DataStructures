#include "treap.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* =========================
 * 1) Comparison & Free Fns
 * ========================= */

/* ----- Integers ----- */
static int compareInt(const void *a, const void *b)
{
    int x = *(int*)a;
    int y = *(int*)b;
    return (x > y) - (x < y); 
}

static void freeInt(void *p)
{
    free(p);
}

/* ----- Strings ----- */
static int compareString(const void *a, const void *b)
{
    return strcmp((const char*)a, (const char*)b);
}

static void freeString(void *p)
{
    free(p);
}

/* ----- Person struct ----- */
typedef struct {
    char *name;
    int age;
} Person;

/* Compare Person primarily by age, then by name */
static int comparePerson(const void *a, const void *b)
{
    const Person *p1 = (const Person*)a;
    const Person *p2 = (const Person*)b;

    if (p1->age != p2->age) {
        return (p1->age > p2->age) - (p1->age < p2->age);
    }
    /* If ages are equal, compare names lexicographically */
    return strcmp(p1->name, p2->name);
}

/* Free a Person struct (including its dynamically allocated name) */
static void freePerson(void *p)
{
    Person *person = (Person*)p;
    if (person->name) {
        free(person->name);
    }
    free(person);
}

/* =========================
 * 2) Treap Validation
 * ========================= */

/*
 * Validate the BST property by key (minKey < node->key < maxKey).
 */
static bool validateBST(TreapNode *node, 
                        int (*compare)(const void*, const void*), 
                        void *minKey, 
                        void *maxKey)
{
    if (!node) return true;

    /* Ensure current node's key is > minKey and < maxKey */
    if (minKey && compare(node->key, minKey) <= 0) return false;
    if (maxKey && compare(node->key, maxKey) >= 0) return false;

    /* Recursively check the subtrees with updated bounds */
    return validateBST(node->left, compare, minKey, node->key)
        && validateBST(node->right, compare, node->key, maxKey);
}

/*
 * Validate the max-heap property on priority:
 *   node->priority >= node->left->priority
 *   node->priority >= node->right->priority
 */
static bool validateHeap(TreapNode *node)
{
    if (!node) return true;

    if (node->left) {
        if (node->left->priority > node->priority) return false;
        if (!validateHeap(node->left)) return false;
    }
    if (node->right) {
        if (node->right->priority > node->priority) return false;
        if (!validateHeap(node->right)) return false;
    }
    return true;
}

/*
 * Checks that the Treap satisfies:
 *   1) BST property by compare (keys).
 *   2) Max-heap property by priority.
 */
static bool validateTreap(Treap *treap)
{
    if (!treap || !treap->root) return true;
    bool bstOK = validateBST(treap->root, treap->compare, NULL, NULL);
    bool heapOK = validateHeap(treap->root);
    return bstOK && heapOK;
}

/* =========================
 * 3) Stress Test
 * =========================
 * A test that:
 *   1) Inserts n random integers,
 *   2) Removes about half of them randomly,
 *   3) Checks treap properties after each operation,
 *   4) Clears the treap.
 */
static void stressTest(Treap *treap, int n)
{
    srand((unsigned int)time(NULL));

    /* Insert n random integers */
    for (int i = 0; i < n; i++) {
        int *val = (int*)malloc(sizeof(int));
        *val = rand() % (10*n);  /* random in range [0..10n-1] */
        assert(validateTreap(treap) && "Treap property violated after insert");
    }

    /* Randomly remove half of them. */
    for (int i = 0; i < n/2; i++) {
        int *val = (int*)malloc(sizeof(int));
        *val = rand() % (10*n);
        treapRemove(treap, val);
        /* Even if removal did nothing (not found), check treap properties. */
        assert(validateTreap(treap) && "Treap property violated after remove");
        free(val);
    }

    /* Finally, clear the treap. */
    treapClear(treap);
    assert(treap->root == NULL);
}

/* =========================
 * 4) Test Driver
 * =========================
 * The function to call to run all tests on the Treap.
 */
void testTreap(void)
{
    printf("Starting Treap tests...\n");

    /* ---------- (A) Test with Integers ---------- */
    {
        Treap *intTreap = treapCreate(compareInt, freeInt);
        assert(intTreap != NULL);

        /* Insert integers [0..9] */
        for (int i = 0; i < 10; i++) {
            int *val = (int*)malloc(sizeof(int));
            *val = i;
            bool inserted = treapInsert(intTreap, val);
            assert(inserted);
        }

        /* Search for them */
        for (int i = 0; i < 10; i++) {
            bool found = treapSearch(intTreap, &i);
            assert(found);
        }

        /* Try inserting duplicates (should fail) */
        for (int i = 0; i < 10; i++) {
            int *dup = (int*)malloc(sizeof(int));
            *dup = i;
            bool inserted = treapInsert(intTreap, dup);
            assert(!inserted);
            free(dup);
        }

        /* Remove some elements [0..4] */
        for (int i = 0; i < 5; i++) {
            bool removed = treapRemove(intTreap, &i);
            assert(removed);
        }

        /* Ensure they are gone */
        for (int i = 0; i < 5; i++) {
            bool found = treapSearch(intTreap, &i);
            assert(!found);
        }

        treapDestroy(intTreap);
    }

    /* ---------- (B) Test with Strings ---------- */
    {
        Treap *strTreap = treapCreate(compareString, freeString);
        assert(strTreap != NULL);

        /* Insert some strings */
        const char *words[] = {"apple", "banana", "cat", "dog", "elephant"};
        for (int i = 0; i < 5; i++) {
            char *dup = strdup(words[i]); /* duplicates literal for safe insertion */
            bool inserted = treapInsert(strTreap, dup);
            assert(inserted);
        }

        /* Search for them */
        for (int i = 0; i < 5; i++) {
            bool found = treapSearch(strTreap, (void*)words[i]);
            assert(found);
        }

        /* Remove "cat" */
        bool removed = treapRemove(strTreap, "cat");
        assert(removed);

        /* Ensure "cat" is gone */
        bool found = treapSearch(strTreap, "cat");
        assert(!found);

        treapDestroy(strTreap);
    }

    /* ---------- (C) Test with Person struct ---------- */
    {
        Treap *personTreap = treapCreate(comparePerson, freePerson);
        assert(personTreap != NULL);

        /* We'll create some Person objects */
        struct {
            const char *name;
            int age;
        } rawPeople[] = {
            {"Alice",   30},
            {"Bob",     25},
            {"Charlie", 35},
            {"Daisy",   40},
            {"Eve",     25},  /* same age as Bob, but different name */
        };
        int numPeople = sizeof(rawPeople)/sizeof(rawPeople[0]);

        /* Insert them into the treap */
        for (int i = 0; i < numPeople; i++) {
            Person *p = (Person*)malloc(sizeof(Person));
            p->name = strdup(rawPeople[i].name); /* allocate name */
            p->age  = rawPeople[i].age;
            bool inserted = treapInsert(personTreap, p);
            assert(inserted);
        }

        /* Verify they can be found */
        for (int i = 0; i < numPeople; i++) {
            Person temp;
            temp.name = (char*)rawPeople[i].name; /* no need to strdup for search */
            temp.age  = rawPeople[i].age;
            bool found = treapSearch(personTreap, &temp);
            assert(found);
        }

        /* Remove "Charlie" (age 35) */
        {
            Person temp;
            temp.name = "Charlie";
            temp.age  = 35;
            bool removed = treapRemove(personTreap, &temp);
            assert(removed);
            /* Ensure he's gone */
            bool stillFound = treapSearch(personTreap, &temp);
            assert(!stillFound);
        }

        treapDestroy(personTreap);
    }

    /* ---------- (D) Stress Test for scale + treap property checks ---------- */
    {
        Treap *largeTreap = treapCreate(compareInt, freeInt);
        assert(largeTreap != NULL);

        /* e.g. 1000 elements; adjust as needed for bigger stress test */
        stressTest(largeTreap, 1000);

        treapDestroy(largeTreap);
    }

    printf("All Treap tests passed successfully.\n");
}
