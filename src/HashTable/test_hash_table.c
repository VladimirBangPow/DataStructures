#include "test_hash_table.h"
#include "hash_table.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * A helper function to print all (key, value) pairs in the hash table.
 * This is purely for demonstration/debugging, not required.
 */
static void printHashTable(const HashTable* ht)
{
    printf("HashTable contents:\n");
    // We'll just iterate through each bucket and print items
    extern void* htSearch(const HashTable*, const char*);
    // Actually no need for extern. We'll just read the structure here if we wanted.
    // But the structure is hidden. We'll just rely on public interface or remove this function.
    // 
    // For a real demonstration, we might adapt the code or do it differently.
    // 
    // We'll skip an internal iteration or do a quick test approach:
    // There's no direct iteration function in the public interface.
    // We'll do something else in test since the structure is hidden.
    printf("  (No direct iteration implemented in the public interface)\n");
}

/**
 * Tests basic insertion, search, removal, and resizing behavior.
 */
void testHashTable(void)
{
    printf("=====Hash Table Testing=====\n");

    // 1. Create a new hash table with initial capacity 8
    HashTable* ht = htCreate(8);
    assert(ht != NULL && "Hash table creation failed");

    printf("[Test] Insert key-value pairs\n");
    // We'll store some color associations: key=fruit, value=color
    // For demonstration, we store the color string pointer directly (static strings).
    // No need to free them, as they are literal strings.

    // Insert
    bool ok;
    ok = htInsert(ht, "apple",  "red");     assert(ok);
    ok = htInsert(ht, "banana", "yellow");  assert(ok);
    ok = htInsert(ht, "cherry", "red");     assert(ok);
    ok = htInsert(ht, "lemon",  "yellow");  assert(ok);
    ok = htInsert(ht, "plum",   "purple");  assert(ok);
    ok = htInsert(ht, "apple",  "green");   // update existing key
    assert(ok && "Updating existing key should succeed");

    // 2. Search
    printf("[Test] Search\n");
    void* val;
    val = htSearch(ht, "apple");   assert(val && strcmp((char*)val,"green")==0);
    val = htSearch(ht, "banana");  assert(val && strcmp((char*)val,"yellow")==0);
    val = htSearch(ht, "cherry");  assert(val && strcmp((char*)val,"red")==0);
    val = htSearch(ht, "lemon");   assert(val && strcmp((char*)val,"yellow")==0);
    val = htSearch(ht, "plum");    assert(val && strcmp((char*)val,"purple")==0);

    // search for something missing
    val = htSearch(ht, "orange");  
    assert(val == NULL && "Searching for non-existent key should return NULL");

    printf("[Test] Remove some keys\n");
    // Remove
    bool removed;
    removed = htRemove(ht, "banana");  assert(removed);
    removed = htRemove(ht, "cherry");  assert(removed);
    // remove something not in table
    removed = htRemove(ht, "non_existent");
    assert(!removed && "Removing non-existent key should return false");

    // Confirm they're removed
    val = htSearch(ht, "banana");  assert(val == NULL);
    val = htSearch(ht, "cherry");  assert(val == NULL);
    // apple, lemon, plum remain
    val = htSearch(ht, "apple");   assert(val && strcmp((char*)val,"green")==0);
    val = htSearch(ht, "lemon");   assert(val && strcmp((char*)val,"yellow")==0);
    val = htSearch(ht, "plum");    assert(val && strcmp((char*)val,"purple")==0);

    // 3. Cause resizing by inserting many keys
    printf("[Test] Stress insert to trigger resize\n");
    char keyBuf[64];
    for(int i = 0; i < 100; i++) {
        snprintf(keyBuf, sizeof(keyBuf), "key_%d", i);
        ok = htInsert(ht, keyBuf, (void*)(long)i); // store i as a pointer
        assert(ok);
    }
    // Check a few random ones
    val = htSearch(ht, "key_50"); assert(val && (long)val == 50);
    val = htSearch(ht, "key_99"); assert(val && (long)val == 99);

    // 4. Clean up
    htDestroy(ht);

    printf("=====All hash table tests passed!=====\n");
}
