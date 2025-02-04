
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
static int slIsEmpty(const SkipList *sl){
	if ((sl->head->next==NULL) && (sl->head->data==NULL)){
		return 1;
	}
	return 0;
}
void slInit(SkipList *sl, SkipListComparator cmp, SkipListFreeFunc freeFunc){
	srand((unsigned int) time(NULL));  // Seed the RNG

	/*
		1. list is empty, so we initialize an empty sl node
	*/

	sl->head = (SkipListNode *)malloc(sizeof(SkipListNode));
    if (!sl->head) {
        fprintf(stderr, "Failed to allocate skip list header.\n");
        exit(EXIT_FAILURE);
    }
	sl->cmp=cmp;
	sl->freeFunc=freeFunc;
    sl->head->data = NULL; // no real data in header
    sl->head->above = NULL;
	sl->head->below = NULL;
	sl->head->next = NULL;
	sl->head->prev = NULL;
	// printf("%p", sl->head);
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
	//b. If we see a value greater than ours, we over shot. We use the
	//node tower to go down. Now we need to look left and traverse.
	//c. If we see NULL before we see a value greater we go down 
	//using the last node tower. Then we repeat the process. If 
	//we see our value return false, if we see NULL before we see a value
	//greater we go down using the last node tower.
	//IF AT ANY POINT: we reach the bottom without finding our node
	//we insert after a value less than and before a value greater. 
	//(NULL to the left is like negative infinity, etc)
	SkipListNode *current=sl->head;

	if (slIsEmpty(sl)){
		sl->head->data=data;
		printf("sl is empty, initializing existing head with data %d\n", *( (int *)sl->head->data ) );

	} else{
		//Break things down into atomic actions and perform recursively:
		//when do we move right?
		//We move right when next-> is not NULL and next->data is less than our data
		//when do we move down?
		//when next-> is NULL or when next->data is greater than our data
		//when do we move left?
		//when prev-> is not NULL and prev->data is greater than our data
		//when do we add a node?
		//when do we exit?
		//when we find a match for our value, or when we are done building the tower

		//is it possible that our conditions will have us move left and right?
		//is it possible for next->data to be less than our data and prev->data to be greater?
		//no
		//is it possible that we can move down and right at the same time?
		//when next-data is greater than our data, it can't also be smaller than our data
		//no
		//is it possible that we can move left and down at the same time?
		//when next->data is greater, then prev->data cannot be greater
		//but when next is NULL, then prev-data can be greater
		//Rule of thumb, check right first, then left, then down, then add
		
		/* insert 2 
		    NULL<-------4-->NULL
			            |
			NULL<--3<--[4]-->NULL
		           |    |
				   v    v
		

		
		*/

	}

	return true;
}



bool slSearch(const SkipList *sl, const void *data){
	return true;
}

bool slRemove(SkipList *sl, const void *data){
	return true;
}


	/*
		insert 5
		head-->[NULL]
		we insert the data into the skiplistnode and return true
	
	*/
	/*
		insert 6
              NULL
			   ^
		       |
		NULL<--5-->NULL
		       |
			   v
		       NULL
		we traverse right until NULL or until we see something greater
		we see NULL. Now we want to go down until we see NULL. We see NULL
		immediately so we know that we are at the bottom. Insert 6 tower 
		
	*/

	/*
		insert 4
        head------>6-->
			   ^   ^
		       |   |
		    <--5-->6-->
		       |   |
			   v   v
		 Sometimes we need to traverse left in the same way we traverse right
		 So if 6 overshoots when we are searching for 4, we move down the 6
		 tower. We now need to traverse left until we see something less than 4
		 or NULL (whichever comes first). In this case we see NULL, so we know we
		 must move down, but wait! We are already at the base level because we see
		 NULL. So that means we insert our 4 tower before 5 at the base level
	
	*/


	/*
		insert 8

               ^
			   |
		head-->4---------->
               ^
		       |
	           4------>6-->
			   ^   ^   ^
		       |   |   |
		    <--4<--5-->6-->
		       |   |   |
			   v   v   v
		 Here we want to insert 8. So we see 4 at the head, and know
		 we must go right or down. The next pointer points to NULL, so
		 we must go down one level. We move right and see 6, that's not
		 far enough we must keep going until we reach something greater
		 or equal to 8, or NULL, whichever comes first. In this case we hit
		 NULL, so we go down once more. Here we test to see if we are at
		 the bottom level, and we are. So we know we must traverse right.
		 We see Null before we see anything greater than 8, so we place the
		 8 tower there.
	
	*/


	/*
		insert 7

               ^
			   |
		head-->4---------->
               ^
		       |
	           4------>6-->
			   ^   ^   ^
		       |   |   |
		    <--4<--5-->6-->8-->
		       |   |   |   |
			   v   v   v   v
		 Here we want to insert 7. So we see 4 at the head, and know
		 we must go right or down. The next pointer points to NULL, so
		 we must go down one level. We move right and see 6, that's not
		 far enough we must keep going until we reach something greater
		 or equal to 8, or NULL, whichever comes first. In this case we hit
		 NULL, so we go down once more. Here we test to see if we are at
		 the bottom level, and we are. So we know we must traverse right.
		 We see 8 before NULL, so we place the 7 tower right before 8.
	
	*/