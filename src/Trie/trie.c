#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/* Include your external Unicode library header: */
#include "../Tools/unicode.h"

/* Include the trie header */
#include "trie.h"

/*
 * A small helper structure for BFS/DFS to detect cycles.
 * We'll store a dynamic array of visited TrieNode pointers.
 */
typedef struct {
    TrieNode **data;
    size_t size;
    size_t capacity;
} NodeArray;

/* --------------------- Node Creation and Freeing ---------------------- */

/*
 * Creates and returns a new TrieNode with no children and no end-of-word count.
 */
static TrieNode *create_trie_node(void) {
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for TrieNode.\n");
        exit(EXIT_FAILURE);
    }
    node->is_end_of_word = false;
    node->end_of_word_count = 0;
    node->children = NULL;  // ChildMap linked list starts empty
    return node;
}

/*
 * Recursively frees a TrieNode and its entire subtree.
 */
static void free_node(TrieNode *node) {
    if (!node) return;
    ChildMap *cm = node->children;
    while (cm) {
        ChildMap *next = cm->next;
        free_node(cm->child);
        free(cm);
        cm = next;
    }
    free(node);
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

void trie_free(Trie *trie) {
    if (!trie) return;
    free_node(trie->root);
    free(trie);
}

/* --------------------- ChildMap Handling ---------------------- */

/*
 * Looks in node->children for an entry matching `codepoint`.
 * Returns pointer to that ChildMap entry, or NULL if not found.
 */
static ChildMap *find_child_entry(const TrieNode *node, int32_t codepoint) {
    ChildMap *cm = node->children;
    while (cm) {
        if (cm->codepoint == codepoint) {
            return cm;
        }
        cm = cm->next;
    }
    return NULL;
}

/*
 * If there's already a child for `codepoint`, return it.
 * Otherwise, create a new ChildMap entry + TrieNode child, link it, and return it.
 */
static ChildMap *get_or_create_child_entry(TrieNode *node, int32_t codepoint) {
    ChildMap *found = find_child_entry(node, codepoint);
    if (found) {
        return found;
    }
    // Not found, create new
    ChildMap *new_entry = (ChildMap *)malloc(sizeof(ChildMap));
    if (!new_entry) {
        fprintf(stderr, "Failed to allocate ChildMap.\n");
        exit(EXIT_FAILURE);
    }
    new_entry->codepoint = codepoint;
    new_entry->child = create_trie_node();
    new_entry->next = node->children;  // insert at head of linked list
    node->children = new_entry;
    return new_entry;
}

/*
 * Removes the ChildMap entry for `codepoint` from node->children (if it exists),
 * but does NOT free the child node itself (caller must handle that).
 *
 * Returns true if the entry was found and removed, false otherwise.
 */
static bool remove_child_entry(TrieNode *node, int32_t codepoint) {
    ChildMap *prev = NULL;
    ChildMap *cm = node->children;
    while (cm) {
        if (cm->codepoint == codepoint) {
            // Unlink this entry
            if (!prev) {
                node->children = cm->next;
            } else {
                prev->next = cm->next;
            }
            free(cm);
            return true;
        }
        prev = cm;
        cm = cm->next;
    }
    return false;
}

/* --------------------- Insert / Search / StartsWith ---------------------- */

/*
 * Inserts a UTF-8 string into the trie:
 *   - For each code point, find/create a child node.
 *   - Mark the final node as an end of word, incrementing its end_of_word_count.
 */
void trie_insert(Trie *trie, const char *utf8_key) {
    if (!trie || !utf8_key) return;

    TrieNode *current = trie->root;
    const char *p = utf8_key;

    while (true) {
        int32_t code = utf8_next_codepoint(&p);  // from ../Tools/unicode.h
        if (code < 0) {
            // code < 0 => end of string or invalid sequence
            break;
        }
        ChildMap *entry = get_or_create_child_entry(current, code);
        current = entry->child;
    }
    // Mark the final node
    current->is_end_of_word = true;
    current->end_of_word_count++;
}

/*
 * Searches for a UTF-8 string in the trie.
 *   - For each code point, follow the correct child if it exists.
 *   - If we reach the end, we check if is_end_of_word && end_of_word_count > 0.
 */
bool trie_search(const Trie *trie, const char *utf8_key) {
    if (!trie || !utf8_key) return false;

    TrieNode *current = trie->root;
    const char *p = utf8_key;

    while (true) {
        int32_t code = utf8_next_codepoint(&p);
        if (code < 0) {
            // end or invalid => stop
            break;
        }
        ChildMap *found = find_child_entry(current, code);
        if (!found) {
            // No child => word not present
            return false;
        }
        current = found->child;
    }

    // We have consumed all code points. For it to be a valid word,
    // the node must be is_end_of_word with a positive count
    return (current->is_end_of_word && current->end_of_word_count > 0);
}

/*
 * Checks if there is any stored word that begins with the given UTF-8 prefix.
 *   - Similar to search, but we don't require the final node to be an end-of-word.
 *   - If we can follow the prefix, we return true.
 */
bool trie_starts_with(const Trie *trie, const char *utf8_prefix) {
    if (!trie || !utf8_prefix) return false;

    TrieNode *current = trie->root;
    const char *p = utf8_prefix;

    while (true) {
        int32_t code = utf8_next_codepoint(&p);
        if (code < 0) {
            // we've reached the end of prefix or invalid => prefix satisfied
            return true;
        }
        ChildMap *found = find_child_entry(current, code);
        if (!found) {
            return false; // can't follow the prefix
        }
        current = found->child;
    }
}

/* --------------------- Delete ---------------------- */

/*
 * A recursive helper that deletes one occurrence of 'utf8_key'.
 * Returns true if the current node can be freed by its parent.
 */
static bool trie_delete_helper(TrieNode *node, const char *utf8_key) {
    if (!node || !utf8_key) return false;

    // Read the next code point from utf8_key
    const char *p = utf8_key;
    int32_t code = utf8_next_codepoint(&p);
    if (code < 0) {
        // Reached the end of the string or invalid
        // => this node should represent the end if the word truly exists
        if (node->is_end_of_word && node->end_of_word_count > 0) {
            node->end_of_word_count--;
            if (node->end_of_word_count == 0) {
                node->is_end_of_word = false;
            }
            // If no children remain, we can free this node
            if (!node->is_end_of_word && !node->children) {
                return true;
            }
        }
        return false;
    }

    // We have more code points => must go deeper
    ChildMap *found = find_child_entry(node, code);
    if (!found) {
        // word not found in trie
        return false;
    }
    TrieNode *childNode = found->child;

    // Recursively delete in the child
    bool childCanDie = trie_delete_helper(childNode, p);
    if (childCanDie) {
        // Free the child's subtree
        free_node(childNode);
        // Remove the child from node->children
        remove_child_entry(node, code);
    }

    // After removing that child, if this node is not an end and has no children,
    // we can also free it (propagate deletion up)
    if (!node->is_end_of_word && !node->children) {
        return true;
    }
    return false;
}

/*
 * Deletes one occurrence of 'utf8_key' from the trie.
 * If the word was inserted multiple times, you must call delete multiple times
 * to remove all occurrences. Returns true if the deletion was attempted
 * (though not strictly whether the word existed).
 */
bool trie_delete(Trie *trie, const char *utf8_key) {
    if (!trie || !utf8_key) return false;
    trie_delete_helper(trie->root, utf8_key);
    // We always return true to indicate "we did the delete operation".
    // If you want to know if the word actually existed, you'd track the
    // recursion result or check if end_of_word_count was decremented.
    return true;
}

/* --------------------- Validation (Cycle Check) ---------------------- */

/*
 * Returns true if 'arr' already contains 'node'.
 */
static bool nodearray_contains(const NodeArray *arr, const TrieNode *node) {
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
static void nodearray_push(NodeArray *arr, TrieNode *node) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        arr->data = (TrieNode **)realloc(arr->data, arr->capacity * sizeof(TrieNode *));
        if (!arr->data) {
            fprintf(stderr, "Realloc failed while doing trie validation.\n");
            exit(EXIT_FAILURE);
        }
    }
    arr->data[arr->size++] = node;
}

/*
 * DFS to detect cycles: if we revisit a node, there's a cycle => invalid.
 */
static bool trie_is_valid_dfs(TrieNode *node, NodeArray *visited) {
    if (!node) return true;

    // If we've seen this node already => cycle
    if (nodearray_contains(visited, node)) {
        return false;
    }
    nodearray_push(visited, node);

    // Visit children
    ChildMap *cm = node->children;
    while (cm) {
        if (!trie_is_valid_dfs(cm->child, visited)) {
            return false;
        }
        cm = cm->next;
    }
    return true;
}

bool trie_is_valid(const Trie *trie) {
    if (!trie || !trie->root) return false;

    NodeArray visited;
    visited.size = 0;
    visited.capacity = 128;
    visited.data = (TrieNode **)malloc(visited.capacity * sizeof(TrieNode *));
    if (!visited.data) {
        fprintf(stderr, "Memory allocation failed in trie_is_valid.\n");
        return false;
    }

    bool ok = trie_is_valid_dfs(trie->root, &visited);
    free(visited.data);
    return ok;
}
