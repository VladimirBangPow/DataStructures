#include "cstring.h"
#include <stddef.h> // for size_t and NULL
#include <stdbool.h>

size_t c_strlen(const char* str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

char* c_strcpy(char* dest, const char* src) {
    char* origDest = dest;
    while ((*dest++ = *src++) != '\0') {
        // copy until we hit '\0'
    }
    return origDest;
}

char* c_strncpy(char* dest, const char* src, size_t n) {
	size_t i = 0;
    // Copy up to (n-1) characters or until src ends
    while (i < n  && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    // Ensure a null terminator if n > 0
    if (n > 0) {
        dest[i] = '\0';
    }
    return dest;
}

int c_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    // Compare ASCII values
    return (unsigned char)(*s1) - (unsigned char)(*s2);
}

int c_strncmp(const char* s1, const char* s2, size_t n) {
    if (n == 0) return 0;
    while (--n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)(*s1) - (unsigned char)(*s2);
}

char* c_strcat(char* dest, const char* src) {
    char* origDest = dest;
    // Move dest pointer to the end (just before '\0')
    while (*dest != '\0') {
        dest++;
    }
    // Now copy src
    while ((*dest++ = *src++) != '\0') {
        // do nothing
    }
    return origDest;
}

char* c_strncat(char* dest, const char* src, size_t n) {
    char* origDest = dest;
    // Find the end of dest
    while (*dest != '\0') {
        dest++;
    }
    // Copy up to n chars from src
    while (n-- && *src != '\0') {
        *dest++ = *src++;
    }
    // Ensure the result is null-terminated
    *dest = '\0';
    return origDest;
}

char* c_strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) {
            // Cast away const to maintain typical C string function signature
            return (char*)s;
        }
        s++;
    }
    // Check if c is '\0'
    if ((char)c == '\0') {
        return (char*)s; // points to the '\0'
    }
    return NULL;
}

char* c_strstr(const char* haystack, const char* needle) {
    if (*needle == '\0') {
        // Empty needle => return haystack
        return (char*)haystack;
    }

    for (; *haystack != '\0'; haystack++) {
        // Check if the substring needle starts here
        const char* h = haystack;
        const char* n = needle;
        while (*h == *n && *h != '\0' && *n != '\0') {
            h++;
            n++;
        }
        if (*n == '\0') {
            // Reached end of needle => found match
            return (char*)haystack;
        }
    }
    // Not found
    return NULL;
}


/**
 * A static pointer to track our position in the string between calls.
 */
static char* saved_ptr = NULL;

char* c_strtok(char* str, const char* delimiters) {
    // If the user passed a non-NULL str, start tokenizing this new string.
    // Otherwise, continue from the previous saved_ptr.
    if (str != NULL) {
        saved_ptr = str;
    } else {
        // If str is NULL but saved_ptr is also NULL => no more tokens
        if (saved_ptr == NULL) {
            return NULL;
        }
    }

    // Skip leading delimiters
    char* token_start = saved_ptr;
    if (*token_start == '\0') {
        // No more tokens to extract
        saved_ptr = NULL;
        return NULL;
    }

    // Advance token_start until we find a non-delimiter
    {
        bool leadingDelimiter = true;
        while (*token_start != '\0' && leadingDelimiter) {
            // Check if *token_start is in the delimiter set
            const char* d = delimiters;
            leadingDelimiter = false;
            while (*d) {
                if (*token_start == *d) {
                    token_start++;
                    leadingDelimiter = true;
                    break;
                }
                d++;
            }
        }
    }

    // If we reached '\0' after skipping delimiters, there's no token
    if (*token_start == '\0') {
        saved_ptr = NULL;
        return NULL;
    }

    // Now we have a token start. Find the end of this token
    char* current = token_start;
    while (*current != '\0') {
        // Check if current char is a delimiter
        const char* d = delimiters;
        bool isDelim = false;
        while (*d) {
            if (*current == *d) {
                isDelim = true;
                break;
            }
            d++;
        }
        if (isDelim) {
            // Terminate the token here
            *current = '\0';
            // saved_ptr = next position after the '\0'
            saved_ptr = current + 1;
            return token_start;
        }
        current++;
    }

    // If we exit the loop, we reached the end of the string => no more delimiters
    // The token goes until '\0'
    saved_ptr = NULL;
    return token_start;
}


char* c_strrchr(const char* s, int c) {
    // We'll keep track of the last occurrence
    // c can be any int in C, but we only compare it as a char
    char* last = NULL;

    // Traverse the string
    while (*s != '\0') {
        if (*s == (char)c) {
            last = (char*)s; // cast away const so signature matches standard
        }
        s++;
    }

    // Special case: if searching for '\0', return pointer to string terminator
    if (c == '\0') {
        return (char*)s; // pointing at the terminating '\0'
    }

    return last;
}