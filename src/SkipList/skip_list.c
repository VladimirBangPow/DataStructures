#include "skip_list.h"
#include <stdlib.h> // for malloc, free, rand
#include <stdio.h>  // for fprintf, NULL checks, etc.
#include <time.h>   // (optional) for seeding rand in slInit or externally

/**
 * Returns a "random level" in the range [1, maxLevel], based on a probability 'probability'.
 *
 * - You start at level 1.
 * - As long as the random value is < probability AND we haven't reached maxLevel, you go one level higher.
 * - 'probability' is typically between 0 and 1 (e.g., 0.5).
 * - This is commonly used in skip-list implementations to decide how tall a node tower is.
 
 * - In other words, this is Probabilistic Promotion: 
 * - When a new element is inserted into the skip list, a coin flip determines how many levels it will be promoted to.
 * - If the coin flip is heads, the element is promoted to the next level. This process continues until the coin flip is tails.
 */
static int slRandomLevel(int maxLevel, float probability)
{
    // We begin at level 1
    int currentLevel = 1;

    // Keep increasing the level while:
    //   1) a random float [0,1) is less than 'probability'
    //   2) we have not already reached the maximum level (maxLevel)
    while (((float)rand() / (float)RAND_MAX) < probability && currentLevel < maxLevel) {
        currentLevel++;
    }

    return currentLevel;
}

void slInit(SkipList *sl, int maxLevel, float probability,
            SkipListComparator cmp, SkipListFreeFunc freeFunc)
{
    // Optionally seed the RNG here or from main
    // srand((unsigned)time(NULL));  // can be done outside as well

    sl->maxLevel = maxLevel;
    sl->probability = probability;
    sl->level = 1; // empty skip list starts with level 1
    sl->cmp = cmp;
    sl->freeFunc = freeFunc;

    // Allocate header node
    sl->header = (SkipListNode *)malloc(sizeof(SkipListNode));
    if (!sl->header) {
        fprintf(stderr, "Failed to allocate skip list header.\n");
        exit(EXIT_FAILURE);
    }
    sl->header->data = NULL; // no real data in header

    // "forward" array for header is size maxLevel
    sl->header->forward = (SkipListNode **)malloc(sizeof(SkipListNode *) * maxLevel);
    if (!sl->header->forward) {
        fprintf(stderr, "Failed to allocate header forward array.\n");
        free(sl->header);
        exit(EXIT_FAILURE);
    }
    // Initialize all forward pointers to NULL
    for (int i = 0; i < maxLevel; i++) {
        sl->header->forward[i] = NULL;
    }
}

void slFree(SkipList *sl)
{
    // Free all nodes except header first
    SkipListNode *current = sl->header->forward[0]; // first actual node in level 0
    while (current) {
        SkipListNode *temp = current->forward[0]; // move to next
        // If we have a freeFunc, free the data
        if (sl->freeFunc) {
            sl->freeFunc(current->data);
        }
        free(current->forward);
        free(current);
        current = temp;
    }

    // Free the header node
    free(sl->header->forward);
    free(sl->header);
    sl->header = NULL;
    sl->level = 0;
}

bool slInsert(SkipList *sl, void *data)
{
    // update[] will hold pointers to nodes that must be updated
    // so they can point to our new node where needed.
    SkipListNode *update[sl->maxLevel];
    SkipListNode *current = sl->header;

    // 1. Search from top level down to find insertion point
    for (int i = sl->level - 1; i >= 0; i--) {
        // Move right while next is < data
        while (current->forward[i] &&
               sl->cmp(current->forward[i]->data, data) < 0)
        {
            current = current->forward[i];
        }
        update[i] = current;
    }
    // Now we are at level 0, just before where 'data' should be
    current = current->forward[0];
    // Check if data already exists
    if (current && sl->cmp(current->data, data) == 0) {
        // The data is already in the list, do not insert
        return false;
    }

    // 2. Generate random level for this node
    int newLevel = slRandomLevel(sl->maxLevel, sl->probability);
    // If newLevel is larger than the current skip list level, update
    if (newLevel > sl->level) {
        for (int i = sl->level; i < newLevel; i++) {
            update[i] = sl->header;
        }
        sl->level = newLevel;
    }

    // 3. Create the new node
    SkipListNode *newNode = (SkipListNode *)malloc(sizeof(SkipListNode));
    if (!newNode) {
        fprintf(stderr, "Failed to allocate skip list node.\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->forward = (SkipListNode **)malloc(sizeof(SkipListNode *) * newLevel);
    if (!newNode->forward) {
        fprintf(stderr, "Failed to allocate node->forward array.\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }

    // 4. Insert this node by adjusting forward pointers
    for (int i = 0; i < newLevel; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }
    return true;
}

bool slSearch(const SkipList *sl, const void *data)
{
    SkipListNode *current = sl->header;
    // Start from the top level down
    for (int i = sl->level - 1; i >= 0; i--) {
        while (current->forward[i] &&
               sl->cmp(current->forward[i]->data, data) < 0)
        {
            current = current->forward[i];
        }
    }
    // Now move one step in level 0
    current = current->forward[0];
    // Check if we found the data
    if (current && sl->cmp(current->data, data) == 0) {
        return true;
    }
    return false;
}

bool slRemove(SkipList *sl, const void *data)
{
    SkipListNode *update[sl->maxLevel];
    SkipListNode *current = sl->header;

    // 1. Find nodes that must be updated
    for (int i = sl->level - 1; i >= 0; i--) {
        while (current->forward[i] &&
               sl->cmp(current->forward[i]->data, data) < 0)
        {
            current = current->forward[i];
        }
        update[i] = current;
    }

    // 2. The candidate is the node right after update[0]
    current = current->forward[0];
    // Check if it actually holds 'data'
    if (current && sl->cmp(current->data, data) == 0) {
        // 3. Update forward pointers
        for (int i = 0; i < sl->level; i++) {
            if (update[i]->forward[i] != current) {
                break;
            }
            update[i]->forward[i] = current->forward[i];
        }
        // Free node data if we have a freeFunc
        if (sl->freeFunc) {
            sl->freeFunc(current->data);
        }
        free(current->forward);
        free(current);

        // 4. Adjust skip list level if top levels are now empty
        while (sl->level > 1 && sl->header->forward[sl->level - 1] == NULL) {
            sl->level--;
        }
        return true;
    }
    return false;
}
