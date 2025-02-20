#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct TrieNode TrieNode;
typedef struct ChildMap ChildMap;

struct ChildMap {
    int32_t codepoint;
    TrieNode *child;
    struct ChildMap *next;
};

struct TrieNode {
    bool is_end_of_word;
    int end_of_word_count;
    ChildMap *children;
};

typedef struct Trie {
    TrieNode *root;
} Trie;

// Function declarations
Trie *trie_create(void);
void trie_free(Trie *trie);

void trie_insert(Trie *trie, const char *utf8_key);
bool trie_search(const Trie *trie, const char *utf8_key);
bool trie_starts_with(const Trie *trie, const char *utf8_prefix);

bool trie_delete(Trie *trie, const char *utf8_key);
bool trie_is_valid(const Trie *trie);

#endif
