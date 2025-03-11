#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include <stdint.h>
#include "dynamic_array.h"

// A node in the trie:
typedef struct TrieNode {
    int32_t codepoint;        // The Unicode codepoint that *this* node represents
    bool    is_end_of_word;   
    int     end_of_word_count;
    // children: dynamic array of TrieNode* (pointers to child nodes)
    DynamicArray trieEdges;  
} TrieNode;

typedef struct Trie {
    TrieNode* root;
} Trie;

// Function declarations
Trie* trie_create(void);
void trie_free(Trie* trie);

void trie_insert(Trie* trie, const char* utf8_key);
bool trie_search(const Trie* trie, const char* utf8_key);
bool trie_starts_with(const Trie* trie, const char* utf8_prefix);

bool trie_delete(Trie* trie, const char* utf8_key);

#endif // TRIE_H
