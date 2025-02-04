#include "test_skip_list.h"
#include "skip_list.h"
#include "../LinkedList/linkedlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <string.h>

// Compare two integers pointed to by 'a' and 'b'
static int intComparator(const void *a, const void *b)
{
    int aa = *(const int *)a;
    int bb = *(const int *)b;
    return (aa < bb) ? -1 : (aa > bb) ? 1 : 0;
}

// Free an int pointer (just a wrapper for free)
static void freeInt(void *data)
{
    free(data); 
}

/************************************************
 * The main test function
 ***********************************************/
void testSkipList(void)
{
    printf("Starting SkipList tests...\n");

    /* ----------------
       Test 1: Integers
       ----------------*/
    {
        printf("Test 1: Integer skip list...\n");
        // Create skip list for integers
        SkipList sl;
        slInit(&sl, intComparator, freeInt);
        int number = 1;
		slInsert(&sl, &number);
		int number2 = 2;
		slInsert(&sl, &number2);

        slFree(&sl);
        printf("Integer skip list test passed!\n");
    }

    printf("All SkipList tests passed!\n");
}