
#include "skip_list.h"
#include <stdlib.h> // for malloc, free, rand
#include <stdio.h>  // for fprintf, NULL checks, etc.
#include <time.h>   // (optional) for seeding rand in slInit or externally

static int getLevelFromCoinFlips(){
	int currentLevel = 0;
	float probability = 0.5f;
    // Keep increasing the level while:
    //   1) a random float [0,1) is less than 'probability'
    //   2) we have not already reached the maximum level (maxLevel)
    while (((float)rand() / (float)RAND_MAX) < probability) {
        currentLevel++;
    }

    return currentLevel;
}

void slInit(SkipList *sl){
	srand((unsigned int) time(NULL));  // Seed the RNG

	/*
		1. list is empty, so we initialize an empty sl node
	*/

	sl->head = (SkipListNode *)malloc(sizeof(SkipListNode));
    if (!sl->head) {
        fprintf(stderr, "Failed to allocate skip list header.\n");
        exit(EXIT_FAILURE);
    }
    sl->head->data = NULL; // no real data in header
    sl->head->above = NULL;
	sl->head->below = NULL;
	sl->head->next = NULL;
	sl->head->prev = NULL;

	// printf("%d\n", getLevelFromCoinFlips());
	return;
}

void slFree(SkipList *sl){
	return;
}

bool slInsert(SkipList *sl, void *data){
	//Case 1: empty list. We need to initialize existing head with data
	//Case 2: One or more nodes in list. We need to start at head, 
	//and traverse right until we see our value, see NULL, or see 
	//a value greater than our value. 
	//a. If we see our value return false
	//b. If we see NULL we begin the insertion process by going all the way
	//down until we hit NULL. If we hit NULL, then we traverse right until
	//we hit a value greater than our own. Insert right before that value. Update
	//all four references. Below =NULL, next and prev are obvious. Above
	//is tricky
	//c. If we see a value greater than our own, we go down a level. If NULL
	//that means we insert right before the greater value. If not NULL we
	//traverse right again until we see a value greater than our own.
	// 

	/* insert(1)
	    --------- 8
       /     
	  2           8
      |
	  2     5     8
     /
	1 2 3 4 5 6 7 8 9
	1 found, return false
	*/

	/*
	    --------- 8
       /     
	  2           8
      |
	  2     5     8
      |
	  2 3 4 5 6 7 8 9
	  |
	  NULL
	1 not found, insert 1 tower before 2, return true
	
	*/
	return true;
}

bool slSearch(const SkipList *sl, const void *data){
	return true;
}

bool slRemove(SkipList *sl, const void *data){
	return true;
}
