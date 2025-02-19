#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "trie.h"

/* 
 * A small helper structure for BFS / DFS to detect cycles.
 * We'll store a dynamic array of visited TrieNode pointers.
 */
typedef struct {
    TrieNode **data;
    size_t size;
    size_t capacity;
} NodeArray;

/* --------------------- UTF-8 Code Point Handling ---------------------- */

/*
 * Reads the next Unicode code point from a UTF-8 string.
 * Returns the code point (>= 0), or -1 on invalid UTF-8.
 * Advances *str to the start of the next character.
 */
static int32_t utf8_next_codepoint(const char **str) {
    if (!str || !*str) return -1;
    const unsigned char *s = (const unsigned char *)(*str);
    if (*s == 0) {
        return -1; // end of string
    }

    int32_t code = 0;
    int bytes = 0;

    // Determine how many bytes in this UTF-8 char
    if ((*s & 0x80) == 0) {
        // 1-byte ASCII: 0xxxxxxx
        code = *s++;
        bytes = 1;
    } else if ((*s & 0xe0) == 0xc0) {
        // 2-byte: 110xxxxx 10xxxxxx
        code = (*s & 0x1f);
        s++;
        bytes = 2;
    } else if ((*s & 0xf0) == 0xe0) {
        // 3-byte: 1110xxxx 10xxxxxx 10xxxxxx
        code = (*s & 0x0f);
        s++;
        bytes = 3;
    } else if ((*s & 0xf8) == 0xf0) {
        // 4-byte: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        code = (*s & 0x07);
        s++;
        bytes = 4;
    } else {
        // invalid or > 4 bytes
        (*str) = (const char *)s + 1;
        return -1;
    }

    // Pull in the continuation bytes
    for (int i = 1; i < bytes; i++) {
        if ((s[0] & 0xc0) != 0x80) {
            // not a valid continuation
            (*str) = (const char *)s + 1;
            return -1;
        }
        code = (code << 6) | (s[0] & 0x3f);
        s++;
    }

    // Advance the caller's pointer
    (*str) = (const char *)s;
    return code;
}

/* --------------------- Trie Node Creation / Freeing ---------------------- */

static TrieNode *create_trie_node(void) {
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for TrieNode.\n");
        exit(EXIT_FAILURE);
    }
    node->is_end_of_word = false;
    node->end_of_word_count = 0;
    node->children = NULL;
    return node;
}

/*
 * Free an entire node subtree recursively.
 */
static void free_node(TrieNode *node) {
    if (!node) return;
    // free each child
    ChildMap *cm = node->children;
    while (cm) {
        ChildMap *tmp = cm->next;
        free_node(cm->child);
        free(cm);
        cm = tmp;
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
 * Find the ChildMap entry matching codepoint.
 * Returns pointer to that entry or NULL if not found.
 */
static ChildMap *find_child_entry(TrieNode *node, int32_t codepoint) {
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
 * Insert a new child node for `codepoint` if not already existing.
 * Returns the ChildMap entry (which contains child pointer).
 */
static ChildMap *get_or_create_child_entry(TrieNode *node, int32_t codepoint) {
    // Check if already exists
    ChildMap *found = find_child_entry(node, codepoint);
    if (found) {
        return found;
    }
    // Create new
    ChildMap *new_entry = (ChildMap *)malloc(sizeof(ChildMap));
    if (!new_entry) {
        fprintf(stderr, "Failed to allocate ChildMap.\n");
        exit(EXIT_FAILURE);
    }
    new_entry->codepoint = codepoint;
    new_entry->child = create_trie_node();
    new_entry->next = node->children;
    node->children = new_entry;
    return new_entry;
}

/*
 * Remove the ChildMap for `codepoint` if it exists AND if we want to free that child.
 * Return true if we removed it from the list, false otherwise.
 */
static bool remove_child_entry(TrieNode *node, int32_t codepoint) {
    ChildMap *prev = NULL;
    ChildMap *cm = node->children;
    while (cm) {
        if (cm->codepoint == codepoint) {
            if (!prev) {
                node->children = cm->next;
            } else {
                prev->next = cm->next;
            }
            // We do NOT free cm->child here, that’s done by caller if needed
            free(cm);
            return true;
        }
        prev = cm;
        cm = cm->next;
    }
    return false;
}

/* --------------------- Insert / Search / StartsWith ---------------------- */

void trie_insert(Trie *trie, const char *utf8_key) {
    if (!trie || !utf8_key) return;

    TrieNode *current = trie->root;
    const char *p = utf8_key; 
    while (true) {
        int32_t code = utf8_next_codepoint(&p);
        if (code < 0) {
            // Either end of string or invalid byte => stop
            break;
        }
        // If we reached the null terminator code < 0 indicates end
        // so let's keep reading until code < 0.
        ChildMap *entry = get_or_create_child_entry(current, code);
        current = entry->child;
    }
    // Mark end
    current->is_end_of_word = true;
    current->end_of_word_count++;
}

bool trie_search(const Trie *trie, const char *utf8_key) {
    if (!trie || !utf8_key) return false;

    TrieNode *current = trie->root;
    const char *p = utf8_key;
    while (true) {
        int32_t code = utf8_next_codepoint(&p);
        if (code < 0) {
            // Reached end or invalid
            break;
        }
        ChildMap *found = find_child_entry(current, code);
        if (!found) {
            return false;
        }
        current = found->child;
    }
    // End => must be a valid word
    return (current->is_end_of_word && current->end_of_word_count > 0);
}

bool trie_starts_with(const Trie *trie, const char *utf8_prefix) {
    if (!trie || !utf8_prefix) return false;

    TrieNode *current = trie->root;
    const char *p = utf8_prefix;
    while (true) {
        int32_t code = utf8_next_codepoint(&p);
        if (code < 0) {
            // done reading prefix
            return true; 
        }
        ChildMap *found = find_child_entry(current, code);
        if (!found) {
            return false;
        }
        current = found->child;
    }
}

/* --------------------- Delete ---------------------- */

/*
 * Recursive helper for trie_delete().
 * Returns true if this node can be freed by its parent 
 * (i.e., no children remain and not an end of word).
 */
static bool trie_delete_helper(TrieNode *node, const char *utf8_key) {
    if (!node || !utf8_key) return false;

    // We read one code point
    const char *p = utf8_key;
    int32_t code = utf8_next_codepoint(&p);
    if (code < 0) {
        // Means we're at the end of the string or invalid
        // => This node should represent the end
        if (node->is_end_of_word && node->end_of_word_count > 0) {
            node->end_of_word_count--;
            if (node->end_of_word_count == 0) {
                node->is_end_of_word = false;
            }
            // If no children, can be freed
            if (!node->is_end_of_word && !node->children) {
                return true;
            }
        }
        return false; // word didn't exist or still needed
    }

    // code >= 0 => we have more characters
    ChildMap *found = find_child_entry(node, code);
    if (!found) {
        // word not found
        return false;
    }
    TrieNode *childNode = found->child;

    // Recurse to see if child can be freed
    bool childCanDie = trie_delete_helper(childNode, p);

    if (childCanDie) {
        // free the child
        free_node(childNode);
        // remove from linked list
        remove_child_entry(node, code);
    }

    // If we removed that child, check if the current node can be freed
    if (!node->is_end_of_word && !node->children) {
        return true;
    }
    return false;
}

bool trie_delete(Trie *trie, const char *utf8_key) {
    if (!trie || !utf8_key) return false;

    // We'll attempt a copy of the key so each recursion can parse from start
    // BUT we only do 1 codepoint at a time in the helper. 
    // Another approach is a separate function that consumes 1 codepoint 
    // from a pointer, but we do it all in one shot here.
    // 
    // We'll handle it by the helper reading the entire string from scratch each time 
    // which is inefficient. 
    // 
    // For a simpler approach, we do the entire parse in the helper anyway.
    // We'll just do it once: 
    bool can_die = trie_delete_helper(trie->root, utf8_key);
    // If root can die, that means the Trie is empty (but we don't free root).
    // We'll keep root around as an empty node.
    return true; // We can return whether the delete actually found a word.
}

/* --------------------- Validation (Cycle Check) ---------------------- */

/*
 * We'll do a DFS or BFS. If we revisit a node, there's a cycle => invalid.
 */
static bool nodearray_contains(const NodeArray *arr, TrieNode *node) {
    for (size_t i = 0; i < arr->size; i++) {
        if (arr->data[i] == node) {
            return true;
        }
    }
    return false;
}

static void nodearray_push(NodeArray *arr, TrieNode *node) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        arr->data = (TrieNode **)realloc(arr->data, arr->capacity * sizeof(TrieNode *));
    }
    arr->data[arr->size++] = node;
}

static bool trie_is_valid_dfs(TrieNode *node, NodeArray *visited) {
    if (!node) return true;

    // If we've seen this node, cycle
    if (nodearray_contains(visited, node)) {
        return false;
    }
    nodearray_push(visited, node);

    // For each child
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

    bool ok = trie_is_valid_dfs(trie->root, &visited);
    free(visited.data);
    return ok;
}
