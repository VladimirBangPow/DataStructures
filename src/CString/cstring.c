#include "cstring.h"
#include <stddef.h> // for size_t

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
