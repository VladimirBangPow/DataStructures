/**
 * File: test_trie.c
 *
 * Compile with something like:
 *      gcc -Wall -o test_trie trie.c test_trie.c -I.
 *
 * Then run:
 *      ./test_trie
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include <assert.h>
 #include <unistd.h>    // For getcwd()
#include <limits.h>    // For PATH_MAX

 #include "trie.h"
 #include "test_trie.h"
 // If ChildEntry is declared in trie.c, you might need to replicate it here, 
 // or include a suitable header that declares it.
 #ifndef CHILDENTRY_DEFINED
 typedef struct {
     int32_t   codepoint;
     TrieNode* child;
 } ChildEntry;
 #define CHILDENTRY_DEFINED
 #endif
 
 /*
  * A macro for limiting how many words we read/insert/delete
  * during the stress test. Adjust as needed or pass via compiler:
  *   gcc -Wall -DSTRESS_LIMIT=500 -o test_trie trie.c test_trie.c -I.
  */
 #ifndef STRESS_LIMIT
 #define STRESS_LIMIT 2000
 #endif
 
 
 /* --------------------- Validation (Cycle Check) ---------------------- */
 
 /*
  * We’ll still use a small helper structure for DFS to detect cycles:
  * Storing visited TrieNode pointers in a simple array (NodeArray).
  */
 typedef struct {
     TrieNode** data;
     size_t     size;
     size_t     capacity;
 } NodeArray;
 
 /*
  * Returns true if 'arr' already contains 'node'.
  */
 static bool nodearray_contains(const NodeArray* arr, const TrieNode* node) {
     for (size_t i = 0; i < arr->size; i++) {
         if (arr->data[i] == node) {
             return true;
         }
     }
     return false;
 }
 
 /*
  * Pushes 'node' onto the visited array, resizing if needed.
  */
 static void nodearray_push(NodeArray* arr, TrieNode* node) {
     if (arr->size == arr->capacity) {
         arr->capacity *= 2;
         arr->data = (TrieNode**)realloc(arr->data, arr->capacity * sizeof(TrieNode*));
         if (!arr->data) {
             fprintf(stderr, "Realloc failed while doing trie validation.\n");
             exit(EXIT_FAILURE);
         }
     }
     arr->data[arr->size++] = node;
 }
 
 /*
  * DFS to detect cycles: if we revisit a node, there's a cycle => invalid.
  * NOTE: We now iterate over the dynamic array `node->trieEdges`.
  */
 static bool trie_is_valid_dfs(TrieNode* node, NodeArray* visited) {
     if (!node) return true;
 
     // If we've seen this node already => cycle
     if (nodearray_contains(visited, node)) {
         return false;
     }
     nodearray_push(visited, node);
 
     // Visit each child in the dynamic array
     size_t n = daSize(&node->trieEdges);
     for (size_t i = 0; i < n; i++) {
         // We stored (codepoint, child) pairs in the dynamic array
         ChildEntry* entry = (ChildEntry*) daGetMutable(&node->trieEdges, i);
         if (entry && entry->child) {
             if (!trie_is_valid_dfs(entry->child, visited)) {
                 return false;
             }
         }
     }
     return true;
 }
 
 /*
  * Checks if the entire Trie has no cycles starting from trie->root.
  */
 static bool trie_is_valid(const Trie* trie) {
     if (!trie || !trie->root) return false;
 
     NodeArray visited;
     visited.size = 0;
     visited.capacity = 128;
     visited.data = (TrieNode**)malloc(visited.capacity * sizeof(TrieNode*));
     if (!visited.data) {
         fprintf(stderr, "Memory allocation failed in trie_is_valid.\n");
         return false;
     }
 
     bool ok = trie_is_valid_dfs(trie->root, &visited);
     free(visited.data);
     return ok;
 }
 
 
 /*
  * A small helper to print a progress bar to the terminal.
  * current: how many items have been processed so far
  * total: total items to process
  */
 static void print_progress_bar(size_t current, size_t total) {
     const int bar_width = 50;  // Length of the progress bar
     double ratio = (double)current / (double)total;
     int complete = (int)(ratio * bar_width);
 
     // Overwrite the same line each time
     printf("\r[");
     for (int i = 0; i < bar_width; i++) {
         if (i < complete) {
             printf("=");
         } else {
             printf(" ");
         }
     }
     printf("] %3d%% (%zu/%zu)", (int)(ratio * 100.0), current, total);
 
     fflush(stdout);
 }
 
 /*
  * Basic test of insertion and search with different data.
  * (Strings, IP addresses, special characters, etc.)
  */
 static void test_basic_trie(void) {
     Trie* trie = trie_create();
     assert(trie && "Failed to create trie");
 
     // 1: Simple string
     trie_insert(trie, "hello");
     assert(trie_search(trie, "hello") && "Should find 'hello' after insertion");
 
     // 2: Another simple string
     trie_insert(trie, "world");
     assert(trie_search(trie, "world") && "Should find 'world' after insertion");
 
     // 3: IP-like data
     const char* ip1 = "192.168.0.1";
     trie_insert(trie, ip1);
     assert(trie_search(trie, ip1) && "Should find '192.168.0.1' after insertion");
 
     // 4: Partial prefix
     assert(trie_starts_with(trie, "he") && "Should detect prefix 'he'");
 
     // 5: Nonexistent key
     assert(!trie_search(trie, "nonexistent") && "Should not find 'nonexistent' in trie");
 
     // 6: Symbol-laden string
     const char* symbol_str = "&c";
     trie_insert(trie, symbol_str);
     assert(trie_search(trie, symbol_str) && "Should find '&c' after insertion");
 
     trie_free(trie);
 }
 
 /*
  * Stress test: read words from a CSV file, store them in memory,
  * then do the following:
  *  1) Insert each word, verify search() succeeds, verify trie_is_valid() is true.
  *  2) Delete each word, verify search() fails, verify trie_is_valid() is true.
  * This test uses only up to STRESS_LIMIT words for demonstration/performance purposes.
  */
 static void test_stress_trie(void) {
    char cwd[PATH_MAX];
     
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
         printf("Current working dir: %s\n", cwd);
     } else {
         perror("getcwd() error");
         return;
     }
     const char* filename = "../data/words.csv";  // Adjust path if needed
     FILE* fp = fopen(filename, "r");
     assert(fp && "Could not open words.csv. Check path or file location!");
 
     printf("[STRESS TEST] Loading dictionary from %s...\n", filename);
 
     // Read all lines into a dynamic array, up to STRESS_LIMIT
     char** words = NULL;
     size_t capacity = 0;
     size_t size = 0;
 
     char line[1024];
     while (fgets(line, sizeof(line), fp) && size < STRESS_LIMIT) {
         // Strip newline
         char* newline = strchr(line, '\n');
         if (newline) {
             *newline = '\0';
         }
 
         // Remove surrounding quotes if present
         size_t len = strlen(line);
         if (len > 1 && line[0] == '"' && line[len - 1] == '"') {
             line[len - 1] = '\0';
             memmove(line, line + 1, len - 1);
         }
 
         // Expand array if needed
         if (size == capacity) {
             capacity = (capacity == 0) ? 256 : capacity * 2;
             words = (char**)realloc(words, capacity * sizeof(char*));
             assert(words && "Memory reallocation failed for words array");
         }
         // Store a copy of this word
         words[size] = strdup(line);
         assert(words[size] && "Memory allocation failed for word copy");
         size++;
     }
     fclose(fp);
 
     if (size == STRESS_LIMIT) {
         printf("[STRESS TEST] Reached STRESS_LIMIT (%d). Only first %d words will be tested.\n", 
                 STRESS_LIMIT, STRESS_LIMIT);
     }
 
     printf("[STRESS TEST] Read %zu words into memory.\n", size);
 
     // Create trie
     Trie* trie = trie_create();
     assert(trie && "Failed to create trie for stress test");
 
     // (1) Insert each word, then check
     printf("[STRESS TEST] Inserting words (limit=%d)...\n", STRESS_LIMIT);
     for (size_t i = 0; i < size; i++) {
         trie_insert(trie, words[i]);
 
         bool found = trie_search(trie, words[i]);
         assert(found && "Word not found immediately after insertion");
 
         bool valid = trie_is_valid(trie);
         assert(valid && "Trie became invalid after insertion");
 
         // Update progress bar
         print_progress_bar(i + 1, size);
     }
     printf("\nDone inserting.\n");
 
     // (2) Delete each word, then check
     printf("[STRESS TEST] Deleting words...\n");
     for (size_t i = 0; i < size; i++) {
         bool deleted = trie_delete(trie, words[i]);
         assert(deleted && "Tried to delete a word that wasn't found in the trie");
 
         bool should_not_find = trie_search(trie, words[i]);
         assert(!should_not_find && "Word should not be found after deletion");
 
         bool valid = trie_is_valid(trie);
         assert(valid && "Trie became invalid after deletion");
 
         // Update progress bar
         print_progress_bar(i + 1, size);
     }
     printf("\nDone deleting.\n");
 
     // Clean up
     trie_free(trie);
 
     // Free local array
     for (size_t i = 0; i < size; i++) {
         free(words[i]);
     }
     free(words);
 
     printf("[STRESS TEST] Completed all insert/delete checks (limit=%d).\n", STRESS_LIMIT);
 }
 
 
 /*
  * Driver function that runs both basic and stress tests on the Trie.
  */
 void testTrie(void) {
     printf("=== Running Basic Tests ===\n");
     test_basic_trie();
 
     printf("\n=== Running Stress Test ===\n");
     test_stress_trie();
 
     printf("\nAll tests passed!\n");
 }
 