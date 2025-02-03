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
        slInit(&sl);
        
        slFree(&sl);
        printf("Integer skip list test passed!\n");
    }

    printf("All SkipList tests passed!\n");
}