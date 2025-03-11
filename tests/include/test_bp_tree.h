#ifndef BPTREE_TEST_H
#define BPTREE_TEST_H

#include <stdbool.h>
#include "bp_tree.h"

/* A simple struct for demonstration. We'll compare Persons by their 'id'. */
typedef struct {
    int   id;
    char* name;
    int   age;
} Person;

/* Comparator for Person, comparing by 'id'. */
int person_cmp(const void* a, const void* b);

/* 
 * Run all B+ Tree tests:
 *  1) Integer tests
 *  2) Float tests
 *  3) String tests
 *  4) Person struct tests
 *  5) Stress test with random inserts/deletes
 */
void testBPTree(void);

/* 
 * You can declare additional specialized comparator(s) or test-only functions here 
 * if needed.
 */

#endif // BPTREE_TEST_H
