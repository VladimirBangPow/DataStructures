#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "double_linkedlist.h"

/* 1) Enumerate possible data types */
typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_PERSON
} DataType;

/* 2) Person struct for demonstration */
typedef struct {
    char name[50];
    int age;
} Person;

/* 3) A universal data wrapper */
typedef struct {
    DataType type;
    union {
        int iVal;
        char sVal[100];    // enough space for test strings
        Person pVal;
    };
} UniversalData;

/* 4) Helper functions to create UniversalData objects */

static UniversalData createIntData(int val) {
    UniversalData u;
    u.type = TYPE_INT;
    u.iVal = val;
    return u;
}

static UniversalData createStringData(const char* str) {
    UniversalData u;
    u.type = TYPE_STRING;
    /* Copy the string safely into sVal */
    strncpy(u.sVal, str, sizeof(u.sVal));
    /* Ensure null-termination */
    u.sVal[sizeof(u.sVal) - 1] = '\0';
    return u;
}

static UniversalData createPersonData(const char* name, int age) {
    UniversalData u;
    u.type = TYPE_PERSON;
    strncpy(u.pVal.name, name, sizeof(u.pVal.name));
    u.pVal.name[sizeof(u.pVal.name) - 1] = '\0';
    u.pVal.age = age;
    return u;
}

/* 5) Print function that can handle all three types */
static void universalPrint(const void* data) {
    const UniversalData* u = (const UniversalData*)data;
    switch (u->type) {
    case TYPE_INT:
        printf("%d -> ", u->iVal);
        break;
    case TYPE_STRING:
        printf("\"%s\" -> ", u->sVal);
        break;
    case TYPE_PERSON:
        printf("{name: %s, age: %d} -> ", u->pVal.name, u->pVal.age);
        break;
    default:
        printf("Unknown -> ");
        break;
    }
}

/* 6) Test function for integers */
void testIntegerOperations(void) {
    printf("=== testIntegerOperations ===\n");

    DoubleLinkedList list;
    dllInit(&list);

    /* Insert integers at front/back */
    UniversalData d1 = createIntData(10);
    UniversalData d2 = createIntData(20);
    UniversalData d3 = createIntData(30);

    dllInsertFront(&list, &d1, sizeof(d1));
    dllInsertFront(&list, &d2, sizeof(d2));
    dllInsertBack(&list, &d3, sizeof(d3));
    /* List should now be: 20 -> 10 -> 30 -> NULL */

    printf("Current list: ");
    dllPrint(&list, universalPrint);

    /* Remove front: should remove 20 */
    UniversalData out;
    assert(dllRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_INT);
    assert(out.iVal == 20);

    /* List is now: 10 -> 30 -> NULL */
    printf("After removing front (20): ");
    dllPrint(&list, universalPrint);

    /* Remove back: should remove 30 */
    assert(dllRemoveBack(&list, &out) == 1);
    assert(out.type == TYPE_INT);
    assert(out.iVal == 30);

    /* List is now: 10 -> NULL */
    printf("After removing back (30): ");
    dllPrint(&list, universalPrint);

    /* Finally, remove front: should remove 10 */
    assert(dllRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_INT);
    assert(out.iVal == 10);

    /* List is now empty: removing again should fail */
    assert(dllRemoveFront(&list, &out) == 0);

    /* Cleanup */
    dllFree(&list);

    printf("[OK] testIntegerOperations passed.\n\n");
}

/* 7) Test function for strings */
void testStringOperations(void) {
    printf("=== testStringOperations ===\n");

    DoubleLinkedList list;
    dllInit(&list);

    /* Insert strings */
    UniversalData s1 = createStringData("Hello");
    UniversalData s2 = createStringData("World");
    UniversalData s3 = createStringData("Test123");

    dllInsertFront(&list, &s1, sizeof(s1)); // "Hello"
    dllInsertBack(&list, &s2, sizeof(s2));  // "World"
    dllInsertBack(&list, &s3, sizeof(s3));  // "Test123"
    /* List: Hello -> World -> Test123 -> NULL */

    printf("Current list: ");
    dllPrint(&list, universalPrint);

    /* Remove front: should remove "Hello" */
    UniversalData out;
    assert(dllRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_STRING);
    assert(strcmp(out.sVal, "Hello") == 0);

    /* List: World -> Test123 -> NULL */
    printf("After removing front (\"Hello\"): ");
    dllPrint(&list, universalPrint);

    /* Remove back: should remove "Test123" */
    assert(dllRemoveBack(&list, &out) == 1);
    assert(out.type == TYPE_STRING);
    assert(strcmp(out.sVal, "Test123") == 0);

    /* List: World -> NULL */
    printf("After removing back (\"Test123\"): ");
    dllPrint(&list, universalPrint);

    /* Cleanup */
    dllFree(&list);

    printf("[OK] testStringOperations passed.\n\n");
}

/* 8) Test function for Person structs */
void testPersonOperations(void) {
    printf("=== testPersonOperations ===\n");

    DoubleLinkedList list;
    dllInit(&list);

    /* Insert Person objects */
    UniversalData p1 = createPersonData("Alice", 30);
    UniversalData p2 = createPersonData("Bob",   40);

    dllInsertFront(&list, &p1, sizeof(p1)); // {Alice,30}
    dllInsertFront(&list, &p2, sizeof(p2)); // {Bob,40}, now front

    /* List: Bob -> Alice -> NULL */
    printf("Current list: ");
    dllPrint(&list, universalPrint);

    /* Remove front: Bob */
    UniversalData out;
    assert(dllRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_PERSON);
    assert(strcmp(out.pVal.name, "Bob") == 0);
    assert(out.pVal.age == 40);

    /* List: Alice -> NULL */
    printf("After removing front (Bob): ");
    dllPrint(&list, universalPrint);

    /* Remove back: Alice */
    assert(dllRemoveBack(&list, &out) == 1);
    assert(out.type == TYPE_PERSON);
    assert(strcmp(out.pVal.name, "Alice") == 0);
    assert(out.pVal.age == 30);

    /* List empty now */
    dllFree(&list);

    printf("[OK] testPersonOperations passed.\n\n");
}

/* 9) Mixed test: int, string, person in one list */
void testMixedOperations(void) {
    printf("=== testMixedOperations ===\n");

    DoubleLinkedList list;
    dllInit(&list);

    /* Create a mix of data types */
    UniversalData d1 = createIntData(999);
    UniversalData d2 = createStringData("Mixed");
    UniversalData d3 = createPersonData("Eve", 25);

    /* Insert them: front => int, back => string, back => person */
    dllInsertFront(&list, &d1, sizeof(d1)); // 999
    dllInsertBack(&list, &d2, sizeof(d2));  // "Mixed"
    dllInsertBack(&list, &d3, sizeof(d3));  // {Eve, 25}
    /* List: 999 -> "Mixed" -> {Eve,25} -> NULL */

    printf("Current list: ");
    dllPrint(&list, universalPrint);

    /* Remove front => 999 */
    UniversalData out;
    assert(dllRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_INT);
    assert(out.iVal == 999);

    /* List: "Mixed" -> {Eve,25} -> NULL */
    printf("After removing front (999): ");
    dllPrint(&list, universalPrint);

    /* Remove front again => "Mixed" */
    assert(dllRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_STRING);
    assert(strcmp(out.sVal, "Mixed") == 0);

    /* List: {Eve,25} -> NULL */
    printf("After removing front (\"Mixed\"): ");
    dllPrint(&list, universalPrint);

    /* Remove back => {Eve,25} */
    assert(dllRemoveBack(&list, &out) == 1);
    assert(out.type == TYPE_PERSON);
    assert(strcmp(out.pVal.name, "Eve") == 0);
    assert(out.pVal.age == 25);

    /* List empty */
    dllFree(&list);

    printf("[OK] testMixedOperations passed.\n\n");
}

/* 10) main() to run all tests */
void testDoubleLinkedList(void) {
    printf("=== TEST DOUBLY LINKED LIST ===\n");
    testIntegerOperations();
    testStringOperations();
    testPersonOperations();
    testMixedOperations();

    printf("All tests passed successfully!\n");
}
