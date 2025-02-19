#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include <stdint.h>

/*
 * Each node has:
 *  - end_of_word_count: how many times this word is stored
 *  - is_end_of_word: indicates if this node marks the end of a word
 *  - a linked list of children (ChildMap)
 */
typedef struct ChildMap {
    int32_t codepoint;
    struct TrieNode *child;
    struct ChildMap *next;
} ChildMap;

typedef struct TrieNode {
    bool is_end_of_word;
    int end_of_word_count;
    ChildMap *children;
} TrieNode;

typedef struct Trie {
    TrieNode *root;
} Trie;

/* Public API */
Trie *trie_create(void);
void trie_free(Trie *trie);

void trie_insert(Trie *trie, const char *utf8_key);
bool trie_search(const Trie *trie, const char *utf8_key);
bool trie_starts_with(const Trie *trie, const char *utf8_prefix);

bool trie_delete(Trie *trie, const char *utf8_key);

bool trie_is_valid(const Trie *trie);

#endif /* TRIE_H */
