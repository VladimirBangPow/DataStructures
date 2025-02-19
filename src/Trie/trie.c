#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "trie.h"

static TrieNode *create_trie_node(void) {
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for TrieNode.\n");
        exit(EXIT_FAILURE);
    }
    node->is_end_of_word = false;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
    }
    return node;
}

Trie *trie_create(void) {
    Trie *trie = (Trie *)malloc(sizeof(Trie));
    if (!trie) {
        fprintf(stderr, "Memory allocation failed for Trie.\n");
        exit(EXIT_FAILURE);
    }
    trie->root = create_trie_node();
    return trie;
}

void trie_insert(Trie *trie, const char *key) {
    if (!trie || !key) return;
    TrieNode *current = trie->root;
    for (int i = 0; key[i] != '\0'; i++) {
        unsigned char index = (unsigned char)key[i];
        if (!current->children[index]) {
            current->children[index] = create_trie_node();
        }
        current = current->children[index];
    }
    current->is_end_of_word = true;
}

bool trie_search(const Trie *trie, const char *key) {
    if (!trie || !key) return false;
    TrieNode *current = trie->root;
    for (int i = 0; key[i] != '\0'; i++) {
        unsigned char index = (unsigned char)key[i];
        if (!current->children[index]) {
            return false;
        }
        current = current->children[index];
    }
    return current->is_end_of_word;
}

bool trie_starts_with(const Trie *trie, const char *prefix) {
    if (!trie || !prefix) return false;
    TrieNode *current = trie->root;
    for (int i = 0; prefix[i] != '\0'; i++) {
        unsigned char index = (unsigned char)prefix[i];
        if (!current->children[index]) {
            return false;
        }
        current = current->children[index];
    }
    return true;
}

/* Recursive helper for trie_delete(). 
   Returns true if the parent should delete this child node (i.e. child has no further use). */
static bool trie_delete_helper(TrieNode *node, const char *key, int depth) {
    if (!node) return false;

    // Base case: end of the string
    if (key[depth] == '\0') {
        if (node->is_end_of_word) {
            node->is_end_of_word = false;
            // Check if this node has no children
            for (int i = 0; i < ALPHABET_SIZE; i++) {
                if (node->children[i]) {
                    return false;  // can't delete this node as it has a child
                }
            }
            return true;  // no children -> this node can be deleted
        }
        return false; // the word doesn't exist as an end in this trie
    }

    unsigned char index = (unsigned char)key[depth];
    TrieNode *child = node->children[index];
    if (!child) {
        return false; // word not found in trie
    }

    bool should_delete_child = trie_delete_helper(child, key, depth + 1);
    if (should_delete_child) {
        free(child);
        node->children[index] = NULL;

        // If the current node is not end_of_word and has no other children,
        // we can safely delete it too.
        if (!node->is_end_of_word) {
            for (int i = 0; i < ALPHABET_SIZE; i++) {
                if (node->children[i]) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

bool trie_delete(Trie *trie, const char *key) {
    if (!trie || !key) return false;
    return trie_delete_helper(trie->root, key, 0);
}

/* Check for cycles or invalid pointers by doing a BFS or DFS.
   If we ever revisit a node we've seen before, it's invalid (cycle).
   Otherwise, if we traverse all reachable nodes without issue, it's valid. */
#include <stddef.h>

typedef struct Queue {
    TrieNode **data;
    size_t size;
    size_t capacity;
} Queue;

static void queue_init(Queue *q) {
    q->size = 0;
    q->capacity = 256;
    q->data = (TrieNode **)malloc(q->capacity * sizeof(TrieNode *));
}
static void queue_push(Queue *q, TrieNode *node) {
    if (q->size == q->capacity) {
        q->capacity *= 2;
        q->data = (TrieNode **)realloc(q->data, q->capacity * sizeof(TrieNode *));
    }
    q->data[q->size++] = node;
}
static TrieNode *queue_pop(Queue *q, size_t *idx) {
    if (*idx < q->size) {
        return q->data[(*idx)++];
    }
    return NULL;
}
static void queue_free(Queue *q) {
    free(q->data);
    q->data = NULL;
}

bool trie_is_valid(const Trie *trie) {
    if (!trie || !trie->root) return false;

    // BFS approach
    Queue queue;
    queue_init(&queue);

    // A simplistic approach: keep an array of visited node pointers
    // If we see the same pointer again, there's a cycle.
    // (In a typical trie, cycles shouldn't exist.)
    size_t visited_capacity = 256;
    size_t visited_size = 0;
    TrieNode **visited = (TrieNode **)malloc(sizeof(TrieNode *) * visited_capacity);

    queue_push(&queue, trie->root);

    size_t idx = 0; // queue pop index
    while (true) {
        TrieNode *node = queue_pop(&queue, &idx);
        if (!node) break;

        // Check if visited
        for (size_t i = 0; i < visited_size; i++) {
            if (visited[i] == node) {
                // Already visited => cycle or invalid link
                queue_free(&queue);
                free(visited);
                return false;
            }
        }
        // Add node to visited
        if (visited_size == visited_capacity) {
            visited_capacity *= 2;
            visited = (TrieNode **)realloc(visited, visited_capacity * sizeof(TrieNode *));
        }
        visited[visited_size++] = node;

        // Enqueue children
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (node->children[i]) {
                queue_push(&queue, node->children[i]);
            }
        }
    }

    queue_free(&queue);
    free(visited);

    return true;
}

/* Define your helper function at file scope. */
static void free_node(TrieNode *node) {
    if (!node) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            free_node(node->children[i]);
        }
    }
    free(node);
}

void trie_free(Trie *trie) {
    if (!trie) return;
    free_node(trie->root);
    free(trie);
}
