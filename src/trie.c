#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/* Include your external Unicode library header: */
#include "unicode.h"

/* Include our trie and dynamic array headers */
#include "trie.h"
#include "dynamic_array.h"

/* 
 * Each element in the TrieNode's 'trieEdges' array will be:
 *   - codepoint: the Unicode code point
 *   - child: pointer to the next TrieNode
 */
typedef struct {
    int32_t   codepoint;
    TrieNode* child;
} TrieEdge;

/* --------------------- Node Creation and Freeing ---------------------- */

/*
 * Creates and returns a new TrieNode with an empty DynamicArray of trieEdges.
 */
static TrieNode* create_trie_node(void) {
    TrieNode* node = (TrieNode*) malloc(sizeof(TrieNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for TrieNode.\n");
        exit(EXIT_FAILURE);
    }
    node->is_end_of_word   = false;
    node->end_of_word_count = 0;
    
    // Initialize the dynamic array for trieEdges
    daInit(&node->trieEdges, 4);  // start with some small capacity (e.g., 4)
    
    return node;
}

/*
 * Recursively frees a TrieNode and its entire subtree.
 */
static void free_node(TrieNode* node) {
    if (!node) return;

    // Free each child's subtree
    size_t n = daSize(&node->trieEdges);
    for (size_t i = 0; i < n; i++) {
        TrieEdge* edge = (TrieEdge*) daGetMutable(&node->trieEdges, i);
        free_node(edge->child);
    }
    
    // Free the dynamic array itself
    daFree(&node->trieEdges);
    
    // Finally free this node
    free(node);
}

Trie* trie_create(void) {
    Trie* trie = (Trie*) malloc(sizeof(Trie));
    if (!trie) {
        fprintf(stderr, "Memory allocation failed for Trie.\n");
        exit(EXIT_FAILURE);
    }
    trie->root = create_trie_node();
    return trie;
}

void trie_free(Trie* trie) {
    if (!trie) return;
    free_node(trie->root);
    free(trie);
}

/* --------------------- TrieEdge Lookup Helpers ---------------------- */

/*
 * Finds an existing edge by codepoint.
 * Returns a pointer to the matching TrieEdge in node->trieEdges, or NULL if not found.
 */
static TrieEdge* find_edge(TrieNode* node, int32_t codepoint) {
    size_t n = daSize(&node->trieEdges);
    for (size_t i = 0; i < n; i++) {
        TrieEdge* edge = (TrieEdge*) daGetMutable(&node->trieEdges, i);
        if (edge->codepoint == codepoint) {
            return edge;
        }
    }
    return NULL;
}

/*
 * If there's already a edge for `codepoint`, return it.
 * Otherwise, create a new TrieEdge + child node, append it, and return the pointer.
 */
static TrieEdge* get_or_create_edge(TrieNode* node, int32_t codepoint) {
    // Check if edge already exists
    TrieEdge* found = find_edge(node, codepoint);
    if (found) {
        return found;
    }
    // Not found, create new
    TrieEdge newEntry;
    newEntry.codepoint = codepoint;
    newEntry.child     = create_trie_node();

    // Push into dynamic array
    daPushBack(&node->trieEdges, &newEntry, sizeof(newEntry));
    
    // The array might reallocate, so we retrieve the pointer to the newly added element
    size_t newIndex = daSize(&node->trieEdges) - 1;
    return (TrieEdge*) daGetMutable(&node->trieEdges, newIndex);
}

/*
 * Removes the edge for `codepoint` from node->trieEdges (if it exists),
 * but does NOT free the child node itself (caller must handle that).
 *
 * Returns true if the edge was found and removed, false otherwise.
 */
static bool remove_edge(TrieNode* node, int32_t codepoint) {
    size_t n = daSize(&node->trieEdges);
    for (size_t i = 0; i < n; i++) {
        TrieEdge* edge = (TrieEdge*) daGetMutable(&node->trieEdges, i);
        if (edge->codepoint == codepoint) {
            // Swap this edge with the last one, then pop
            if (i != n - 1) {
                // Get pointer to the last element
                TrieEdge* last = (TrieEdge*) daGetMutable(&node->trieEdges, n - 1);
                // Swap
                TrieEdge temp = *last;
                *last = *edge;
                *edge = temp;
            }
            // Now pop back to remove the last element
            daPopBack(&node->trieEdges, NULL, NULL);
            return true;
        }
    }
    return false;
}

/* --------------------- Insert / Search / StartsWith ---------------------- */

void trie_insert(Trie* trie, const char* utf8_key) {
    if (!trie || !utf8_key) return;

    TrieNode* current = trie->root;
    const char* p = utf8_key;

    while (true) {
        int32_t code = utf8_next_codepoint(&p);
        if (code < 0) {
            // code < 0 => end of string or invalid sequence
            break;
        }
        TrieEdge* edge = get_or_create_edge(current, code);
        current = edge->child;
    }
    // Mark the final node
    current->is_end_of_word = true;
    current->end_of_word_count++;
}

bool trie_search(const Trie* trie, const char* utf8_key) {
    if (!trie || !utf8_key) return false;

    TrieNode* current = trie->root;
    const char* p = utf8_key;

    while (true) {
        int32_t code = utf8_next_codepoint(&p);
        if (code < 0) {
            // end or invalid => stop
            break;
        }
        TrieEdge* found = find_edge(current, code);
        if (!found) {
            // No matching child => word not present
            return false;
        }
        current = found->child;
    }

    // We've consumed all code points. For it to be a valid word,
    // the node must be marked as an end and have a positive count.
    return (current->is_end_of_word && current->end_of_word_count > 0);
}

bool trie_starts_with(const Trie* trie, const char* utf8_prefix) {
    if (!trie || !utf8_prefix) return false;

    TrieNode* current = trie->root;
    const char* p = utf8_prefix;

    while (true) {
        int32_t code = utf8_next_codepoint(&p);
        if (code < 0) {
            // Reached end of prefix or invalid => prefix matched
            return true;
        }
        TrieEdge* found = find_edge(current, code);
        if (!found) {
            // Can't continue the prefix
            return false;
        }
        current = found->child;
    }
}

/* --------------------- Delete ---------------------- */

/*
 * A recursive helper that deletes one occurrence of 'utf8_key'.
 * Returns true if the current node can be freed (i.e., removed) by its parent.
 */
static bool trie_delete_helper(TrieNode* node, const char* utf8_key) {
    if (!node || !utf8_key) return false;

    const char* p = utf8_key;
    int32_t code = utf8_next_codepoint(&p);

    if (code < 0) {
        // Reached end of the string or invalid
        // => this node should represent the end if the word truly exists
        if (node->is_end_of_word && node->end_of_word_count > 0) {
            node->end_of_word_count--;
            if (node->end_of_word_count == 0) {
                node->is_end_of_word = false;
            }
            // If no trieEdges remain, signal that we can free this node
            if (!node->is_end_of_word && daIsEmpty(&node->trieEdges)) {
                return true;
            }
        }
        return false;
    }

    // Otherwise, go deeper
    TrieEdge* found = find_edge(node, code);
    if (!found) {
        // word not found
        return false;
    }
    TrieNode* childNode = found->child;

    bool childCanDie = trie_delete_helper(childNode, p);
    if (childCanDie) {
        // First, free the child's entire subtree
        free_node(childNode);
        // Then remove the child's edge from the array
        remove_edge(node, code);
    }

    // After potentially removing the child, check if this node is freeable
    if (!node->is_end_of_word && daIsEmpty(&node->trieEdges)) {
        return true;
    }
    return false;
}

bool trie_delete(Trie* trie, const char* utf8_key) {
    if (!trie || !utf8_key) return false;
    trie_delete_helper(trie->root, utf8_key);
    // Always return true indicating "delete attempted".
    // If you need to know whether the word was actually present, you'd track
    // the recursion result or check end_of_word_count changes.
    return true;
}
