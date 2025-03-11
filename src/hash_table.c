#include "hash_table.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

/* 
 * A single linked-list node to hold (key, value) pairs in each bucket.
 */
typedef struct HashNode {
    char* key;              // dynamically copied string
    void* value;            // user data
    struct HashNode* next;
} HashNode;

/* 
 * The hash table structure: an array of bucket pointers, plus metadata.
 */
struct HashTable {
    HashNode** buckets;     // array of linked-list heads
    size_t capacity;        // number of buckets
    size_t size;            // number of (key, value) pairs stored
};

/* Load factor threshold for resizing (e.g., 0.75) */
static const float LOAD_FACTOR = 0.75f;

/* Forward declaration of static helper functions */
static size_t djb2(const char* str);
static bool htResize(HashTable* ht, size_t newCapacity);

/* ------------------------------------------------------------------------
 * djb2 Hash Function
 * 
 * A widely used string-hashing algorithm by Daniel J. Bernstein.
 * Produces reasonably good distribution for typical strings.
 * ---------------------------------------------------------------------- */
static size_t djb2(const char* str)
{
    size_t hash = 5381;
    int c;

    while ((c = *str++) != 0) {
        // hash * 33 + c
        hash = ((hash << 5) + hash) + (unsigned char)c;
    }
    return hash;
}

/* ------------------------------------------------------------------------
 * htCreate
 * 
 * Allocates a new HashTable with the specified initial capacity. 
 * If 'initialCapacity' is too small, we'll enforce a minimum (e.g. 8).
 * ---------------------------------------------------------------------- */
HashTable* htCreate(size_t initialCapacity)
{
    if (initialCapacity < 8) {
        initialCapacity = 8;
    }
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (!ht) {
        return NULL;
    }

    ht->buckets = (HashNode**)calloc(initialCapacity, sizeof(HashNode*));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }

    ht->capacity = initialCapacity;
    ht->size = 0;
    return ht;
}

/* ------------------------------------------------------------------------
 * htInsert
 * 
 * Inserts or updates a (key, value) pair. 
 * - If 'key' already exists, we replace the old value with the new one.
 * - If 'key' does not exist, we add a new node at the front of the bucket.
 * 
 * Returns false only if memory allocation for a new entry fails.
 * ---------------------------------------------------------------------- */
bool htInsert(HashTable* ht, const char* key, void* value)
{
    assert(ht != NULL);
    assert(key != NULL);

    // Check if we need to resize (grow) based on load factor
    float loadFactor = (float)ht->size / (float)ht->capacity;
    if (loadFactor >= LOAD_FACTOR) {
        size_t newCapacity = ht->capacity * 2;  // double the capacity
        if (!htResize(ht, newCapacity)) {
            // Could not resize, but we'll try to continue
            // There's a risk of performance degradation if we can't resize
        }
    }

    // Compute bucket index
    size_t index = djb2(key) % ht->capacity;
    HashNode* head = ht->buckets[index];

    // Check if the key already exists in this bucket
    for (HashNode* node = head; node != NULL; node = node->next) {
        if (strcmp(node->key, key) == 0) {
            // Key found, update value
            node->value = value;
            return true; // updated existing
        }
    }

    // Key not found, create a new node at the front
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    if (!newNode) {
        return false; // allocation failed
    }
    // Duplicate the string key
    char* dupKey = strdup(key);
    if (!dupKey) {
        free(newNode);
        return false;
    }

    newNode->key = dupKey;
    newNode->value = value;
    newNode->next = head;

    // Insert at the front of the bucket list
    ht->buckets[index] = newNode;
    ht->size++;

    return true;
}

/* ------------------------------------------------------------------------
 * htSearch
 * 
 * Returns the value associated with 'key' if present, or NULL otherwise.
 * ---------------------------------------------------------------------- */
void* htSearch(const HashTable* ht, const char* key)
{
    assert(ht != NULL);
    assert(key != NULL);

    size_t index = djb2(key) % ht->capacity;
    HashNode* node = ht->buckets[index];

    while (node) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }
    return NULL; // not found
}

/* ------------------------------------------------------------------------
 * htRemove
 * 
 * Removes the (key, value) pair if 'key' exists.
 * 
 * Returns true if removal succeeded, false if 'key' not found.
 * ---------------------------------------------------------------------- */
bool htRemove(HashTable* ht, const char* key)
{
    assert(ht != NULL);
    assert(key != NULL);

    size_t index = djb2(key) % ht->capacity;
    HashNode* node = ht->buckets[index];
    HashNode* prev = NULL;

    while (node) {
        if (strcmp(node->key, key) == 0) {
            // Found the node to remove
            if (prev) {
                prev->next = node->next;
            } else {
                // Removing head of the bucket
                ht->buckets[index] = node->next;
            }
            free(node->key); // free the duplicated key
            free(node);
            ht->size--;
            return true;
        }
        prev = node;
        node = node->next;
    }
    return false; // not found
}

/* ------------------------------------------------------------------------
 * htDestroy
 * 
 * Frees all internal data. Does NOT free the 'value' pointers. If your 
 * usage needs that, do it before calling htDestroy or adapt the code.
 * ---------------------------------------------------------------------- */
void htDestroy(HashTable* ht)
{
    if (!ht) return;
    // Free each bucket's linked list
    for (size_t i = 0; i < ht->capacity; i++) {
        HashNode* node = ht->buckets[i];
        while (node) {
            HashNode* next = node->next;
            free(node->key); // key was duplicated
            free(node);      
            node = next;
        }
    }
    free(ht->buckets);
    free(ht);
}

/* ------------------------------------------------------------------------
 * htResize (static helper)
 * 
 * Resizes the hash table to newCapacity by re-inserting all elements.
 * ---------------------------------------------------------------------- */
static bool htResize(HashTable* ht, size_t newCapacity)
{
    HashNode** newBuckets = (HashNode**)calloc(newCapacity, sizeof(HashNode*));
    if (!newBuckets) {
        return false; // allocation failed
    }

    // We'll re-link all existing entries into newBuckets
    for (size_t i = 0; i < ht->capacity; i++) {
        HashNode* node = ht->buckets[i];
        while (node) {
            HashNode* next = node->next;
            // Recompute index in the new table
            size_t newIndex = djb2(node->key) % newCapacity;

            // Insert at front of newBuckets[newIndex]
            node->next = newBuckets[newIndex];
            newBuckets[newIndex] = node;

            node = next;
        }
    }

    // Free the old bucket array (but not the nodes!)
    free(ht->buckets);

    // Update hash table metadata
    ht->buckets = newBuckets;
    ht->capacity = newCapacity;
    // Size remains the same
    return true;
}
