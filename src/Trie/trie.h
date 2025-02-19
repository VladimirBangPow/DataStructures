#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#define ALPHABET_SIZE 128

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    bool is_end_of_word;
} TrieNode;

typedef struct Trie {
    TrieNode *root;
} Trie;

/* Existing functions */
Trie *trie_create(void);
void trie_insert(Trie *trie, const char *key);
bool trie_search(const Trie *trie, const char *key);
bool trie_starts_with(const Trie *trie, const char *prefix);
void trie_free(Trie *trie);

/* NEW functions */
bool trie_delete(Trie *trie, const char *key);
bool trie_is_valid(const Trie *trie);

#endif /* TRIE_H */
