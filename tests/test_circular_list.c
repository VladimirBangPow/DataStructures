#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Include your circular list header, which in turn uses your dll code. */
#include "circular_list.h"
#include "test_circular_list.h"

/* 1) Enumerate possible data types for demonstration. */
typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_PERSON
} DataType;

/* 2) A simple struct we'll store in the list. */
typedef struct {
    char name[50];
    int age;
} Person;

/* 3) A universal wrapper that includes a 'type' field + union. */
typedef struct {
    DataType type;
    union {
        int iVal;
        char sVal[100];    // enough space for test strings
        Person pVal;
    };
} UniversalData;

/* 4) Helper creators for each type. */
static UniversalData makeIntData(int val) {
    UniversalData u;
    u.type = TYPE_INT;
    u.iVal = val;
    return u;
}

static UniversalData makeStringData(const char* str) {
    UniversalData u;
    u.type = TYPE_STRING;
    strncpy(u.sVal, str, sizeof(u.sVal));
    u.sVal[sizeof(u.sVal) - 1] = '\0'; // ensure null termination
    return u;
}

static UniversalData makePersonData(const char* name, int age) {
    UniversalData u;
    u.type = TYPE_PERSON;
    strncpy(u.pVal.name, name, sizeof(u.pVal.name));
    u.pVal.name[sizeof(u.pVal.name) - 1] = '\0'; // ensure null termination
    u.pVal.age = age;
    return u;
}

/* 5) A generic print function that checks the 'type' field. */
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

/* -------------------- TEST FUNCTIONS -------------------- */

/* Test storing and removing integers in a circular list. */
static void testCircularListInt(void) {
    printf("=== testCircularListInt ===\n");

    CircularList list;
    clistInit(&list);

    /* Insert some ints at the front/back */
    UniversalData d1 = makeIntData(100);
    UniversalData d2 = makeIntData(200);
    UniversalData d3 = makeIntData(300);

    clistInsertFront(&list, &d1, sizeof(d1)); // 100
    clistInsertFront(&list, &d2, sizeof(d2)); // 200 at front
    clistInsertBack(&list, &d3, sizeof(d3));  // 300 at back
    /* Expected: 200 -> 100 -> 300 -> (wrap) */

    printf("List after inserting 3 ints: ");
    clistPrint(&list, universalPrint);

    UniversalData out;
    /* Remove front => 200 */
    assert(clistRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_INT);
    assert(out.iVal == 200);

    printf("After removing front (200): ");
    clistPrint(&list, universalPrint);

    /* Remove back => 300 */
    assert(clistRemoveBack(&list, &out) == 1);
    assert(out.type == TYPE_INT);
    assert(out.iVal == 300);

    printf("After removing back (300): ");
    clistPrint(&list, universalPrint);

    /* Now only "100" remains. Remove front => 100. */
    assert(clistRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_INT);
    assert(out.iVal == 100);

    /* Now list is empty => removing should fail (return 0). */
    assert(clistRemoveFront(&list, &out) == 0);

    clistFree(&list);

    printf("[OK] testCircularListInt passed.\n\n");
}

/* Test storing and removing strings in a circular list. */
static void testCircularListString(void) {
    printf("=== testCircularListString ===\n");

    CircularList list;
    clistInit(&list);

    UniversalData s1 = makeStringData("Alpha");
    UniversalData s2 = makeStringData("Beta");
    UniversalData s3 = makeStringData("Gamma");

    clistInsertFront(&list, &s1, sizeof(s1));  // "Alpha"
    clistInsertBack(&list, &s2, sizeof(s2));   // "Beta"
    clistInsertBack(&list, &s3, sizeof(s3));   // "Gamma"
    /* Expected: Alpha -> Beta -> Gamma -> (wrap) */

    printf("List after inserting 3 strings: ");
    clistPrint(&list, universalPrint);

    UniversalData out;
    /* Remove front => "Alpha" */
    assert(clistRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_STRING);
    assert(strcmp(out.sVal, "Alpha") == 0);

    printf("After removing front (\"Alpha\"): ");
    clistPrint(&list, universalPrint);

    /* Remove back => "Gamma" */
    assert(clistRemoveBack(&list, &out) == 1);
    assert(out.type == TYPE_STRING);
    assert(strcmp(out.sVal, "Gamma") == 0);

    printf("After removing back (\"Gamma\"): ");
    clistPrint(&list, universalPrint);

    /* Now only "Beta" remains => remove it from front. */
    assert(clistRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_STRING);
    assert(strcmp(out.sVal, "Beta") == 0);

    /* Should be empty => removing again fails. */
    assert(clistRemoveFront(&list, &out) == 0);

    clistFree(&list);

    printf("[OK] testCircularListString passed.\n\n");
}

/* Test storing and removing a custom Person struct. */
static void testCircularListPerson(void) {
    printf("=== testCircularListPerson ===\n");

    CircularList list;
    clistInit(&list);

    UniversalData p1 = makePersonData("Alice", 30);
    UniversalData p2 = makePersonData("Bob",   40);

    clistInsertFront(&list, &p1, sizeof(p1)); // {Alice,30}
    clistInsertBack(&list, &p2, sizeof(p2));  // {Bob,40}

    /* Expected: Alice -> Bob -> (wrap) */
    printf("List after inserting 2 Persons: ");
    clistPrint(&list, universalPrint);

    UniversalData out;
    /* Remove front => {Alice, 30} */
    assert(clistRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_PERSON);
    assert(strcmp(out.pVal.name, "Alice") == 0);
    assert(out.pVal.age == 30);

    printf("After removing front (Alice): ");
    clistPrint(&list, universalPrint);

    /* Remove back => {Bob, 40} (the only node left) */
    assert(clistRemoveBack(&list, &out) == 1);
    assert(out.type == TYPE_PERSON);
    assert(strcmp(out.pVal.name, "Bob") == 0);
    assert(out.pVal.age == 40);

    /* Now empty => remove fails */
    assert(clistRemoveFront(&list, &out) == 0);

    clistFree(&list);

    printf("[OK] testCircularListPerson passed.\n\n");
}

/* Test a mix of data types in one circular list. */
static void testCircularListMixed(void) {
    printf("=== testCircularListMixed ===\n");

    CircularList list;
    clistInit(&list);

    UniversalData d1 = makeIntData(999);
    UniversalData d2 = makeStringData("Omega");
    UniversalData d3 = makePersonData("Zara", 22);

    clistInsertFront(&list, &d1, sizeof(d1)); // 999
    clistInsertBack(&list, &d2, sizeof(d2));  // "Omega"
    clistInsertBack(&list, &d3, sizeof(d3));  // {Zara,22}
    /* 999 -> "Omega" -> {Zara,22} -> (wrap) */

    printf("List after inserting 3 mixed data: ");
    clistPrint(&list, universalPrint);

    UniversalData out;
    /* Remove front => 999 */
    assert(clistRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_INT);
    assert(out.iVal == 999);

    printf("After removing front (999): ");
    clistPrint(&list, universalPrint);

    /* Remove front => "Omega" */
    assert(clistRemoveFront(&list, &out) == 1);
    assert(out.type == TYPE_STRING);
    assert(strcmp(out.sVal, "Omega") == 0);

    printf("After removing front (\"Omega\"): ");
    clistPrint(&list, universalPrint);

    /* Remove back => {Zara, 22} (the only node left) */
    assert(clistRemoveBack(&list, &out) == 1);
    assert(out.type == TYPE_PERSON);
    assert(strcmp(out.pVal.name, "Zara") == 0);
    assert(out.pVal.age == 22);

    /* Now empty => remove fails */
    assert(clistRemoveFront(&list, &out) == 0);

    clistFree(&list);

    printf("[OK] testCircularListMixed passed.\n\n");
}

/* -------------------- DRIVER FUNCTION -------------------- */

/*
 * runCircularListTests:
 *  Calls all the test functions defined above.
 *  You can call this from any main() function in your code base.
 */
void testCircularList(void) {
    testCircularListInt();
    testCircularListString();
    testCircularListPerson();
    testCircularListMixed();

    printf("All circular list tests completed!\n");
}
