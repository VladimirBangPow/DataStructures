#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "test_stack.h"

/* ---------------------------------------------------------------------------
   Formatter Functions
   --------------------------------------------------------------------------- */
static void intFormatter(const void* data, char* outBuffer, size_t outBufferSize) {
    int val = *(const int*)data;
    snprintf(outBuffer, outBufferSize, "%d", val);
}

static void stringFormatter(const void* data, char* outBuffer, size_t outBufferSize) {
    // We assume data is a null-terminated C string
    const char* str = (const char*)data;
    snprintf(outBuffer, outBufferSize, "%s", str);
}

typedef struct {
    int id;
    float amount;
    char name[20];
} MyStruct;

static void myStructFormatter(const void* data, char* outBuffer, size_t outBufferSize) {
    const MyStruct* s = (const MyStruct*)data;
    snprintf(outBuffer, outBufferSize, "id=%d,amt=%.1f,%s", s->id, s->amount, s->name);
}



/* ---------------------------------------------------------------------------
   Test: Integer Stack
   --------------------------------------------------------------------------- */
static void testStackInts(void) {
    printf("\n=== testStackInts ===\n");
    Stack s;
    stackInit(&s);

    // Push some integers
    int vals[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        stackPush(&s, &vals[i], sizeof(vals[i]));
    }
    // Stack top-> [50, 40, 30, 20, 10]

    // Check top (peek)
    int peekVal;
    int rc = stackPeek(&s, &peekVal);
    assert(rc == 1 && peekVal == 50);

    // Pop two items
    int popped;
    rc = stackPop(&s, &popped);
    assert(rc == 1 && popped == 50);
    rc = stackPop(&s, &popped);
    assert(rc == 1 && popped == 40);
    // Now top-> [30, 20, 10]

    // Print the top 3 with a max width of 8
    printHead(&s, 3, 8, intFormatter);

    // Pop remaining
    rc = stackPop(&s, &popped);
    assert(rc == 1 && popped == 30);
    rc = stackPop(&s, &popped);
    assert(rc == 1 && popped == 20);
    rc = stackPop(&s, &popped);
    assert(rc == 1 && popped == 10);

    // Stack empty
    assert(stackIsEmpty(&s));

    // Clean up
    stackClear(&s);  // should be no-op
    assert(stackIsEmpty(&s));
    printf("testStackInts PASSED.\n");
}

/* ---------------------------------------------------------------------------
   Test: String Stack
   --------------------------------------------------------------------------- */
static void testStackStrings(void) {
    printf("\n=== testStackStrings ===\n");
    Stack s;
    stackInit(&s);

    // Push some strings
    const char* words[] = {"hello", "world", "this is a longer string", "C language"};
    for (int i = 0; i < 4; i++) {
        stackPush(&s, words[i], strlen(words[i]) + 1);
    }
    // top-> ["C language", "this is a longer string", "world", "hello"]

    // Peek
    char buf[50];
    int rc = stackPeek(&s, buf);
    assert(rc == 1 && strcmp(buf, "C language") == 0);

    // Pop 1
    rc = stackPop(&s, buf);
    assert(rc == 1 && strcmp(buf, "C language") == 0);

    // Print top 2
    printHead(&s, 2, 15, stringFormatter); // top-> "this is a longer string", "world"

    // Pop all
    rc = stackPop(&s, buf);
    assert(rc == 1 && strcmp(buf, "this is a longer string") == 0);
    rc = stackPop(&s, buf);
    assert(rc == 1 && strcmp(buf, "world") == 0);
    rc = stackPop(&s, buf);
    assert(rc == 1 && strcmp(buf, "hello") == 0);

    // Now empty
    assert(stackIsEmpty(&s));

    stackClear(&s);
    printf("testStackStrings PASSED.\n");
}

/* ---------------------------------------------------------------------------
   Test: Struct Stack
   --------------------------------------------------------------------------- */
static void testStackStructs(void) {
    printf("\n=== testStackStructs ===\n");
    Stack s;
    stackInit(&s);

    // Create some structs
    MyStruct m1 = {1, 9.99f, "Alpha"};
    MyStruct m2 = {2, 3.14f, "Beta"};
    MyStruct m3 = {3, 42.0f, "Gamma"};
    stackPush(&s, &m1, sizeof(m1));
    stackPush(&s, &m2, sizeof(m2));
    stackPush(&s, &m3, sizeof(m3));
    // top-> [m3, m2, m1]

    // Peek
    MyStruct temp;
    int rc = stackPeek(&s, &temp);
    assert(rc == 1 && temp.id == 3 && temp.amount == 42.0f);

    // Print entire stack
    printStackRange(&s, 0, 999, 25, myStructFormatter);

    // Pop one
    rc = stackPop(&s, &temp);
    assert(rc == 1 && temp.id == 3 && strcmp(temp.name, "Gamma") == 0);

    // Pop next
    rc = stackPop(&s, &temp);
    assert(rc == 1 && temp.id == 2 && temp.amount == 3.14f);

    // Pop last
    rc = stackPop(&s, &temp);
    assert(rc == 1 && temp.id == 1);

    // Should be empty now
    assert(stackIsEmpty(&s));

    // Clean up
    stackClear(&s);
    printf("testStackStructs PASSED.\n");
}



/* ---------------------------------------------------------------------------
   Main Test Runner
   --------------------------------------------------------------------------- */
void testStack(void) {
    printf("=== BEGIN STACK TESTS ===\n");

    testStackInts();
    testStackStrings();
    testStackStructs();

    printf("\n=== ALL STACK TESTS PASSED SUCCESSFULLY ===\n");
}
