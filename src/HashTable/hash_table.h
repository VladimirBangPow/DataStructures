#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>  // for size_t
#include <stdbool.h> // for bool

// Forward declaration of the HashTable type
typedef struct HashTable HashTable;

/**
 * Creates a new hash table with the specified initial capacity.
 * 
 * Returns a pointer to a new HashTable on success, or NULL on failure.
 */
HashTable* htCreate(size_t initialCapacity);

/**
 * Inserts or updates a (key, value) pair in the hash table. 
 * The key is a null-terminated string. Value is a void pointer 
 * so you can store anything. 
 *
 * Returns true if insertion is successful (new or updated), 
 * or false on allocation failure.
 */
bool htInsert(HashTable* ht, const char* key, void* value);

/**
 * Searches the hash table for the given string key.
 *
 * Returns the associated value pointer if found, or NULL if not found.
 */
void* htSearch(const HashTable* ht, const char* key);

/**
 * Removes the entry with the given key from the hash table if it exists.
 *
 * Returns true if the entry is removed, or false if the key is not found.
 */
bool htRemove(HashTable* ht, const char* key);

/**
 * Destroys the entire hash table, freeing all internal structures.
 * Does NOT free any 'value' pointers; the caller must do so if needed.
 */
void htDestroy(HashTable* ht);

#endif // HASH_TABLE_H
