#ifndef STRING_LIB_H
#define STRING_LIB_H

#include <stddef.h> // for size_t

/**
 * A small "custom" string library. These functions mimic standard
 * C library behavior, but are written from scratch for demonstration.
 */

/**
 * Compute the length of the C-string str.
 * Returns the number of characters before the terminating '\0'.
 */
size_t c_strlen(const char* str);

/**
 * Copy the C-string src to dest (including the terminating '\0').
 * Returns dest.
 */
char* c_strcpy(char* dest, const char* src);

/**
 * Copy at most n characters of src to dest.
 * If src has fewer than n characters, the remainder of dest is padded with '\0'.
 * Returns dest.
 */
char* c_strncpy(char* dest, const char* src, size_t n);

/**
 * Compare two strings lexicographically.
 * Returns < 0 if s1 < s2, 0 if s1 == s2, > 0 if s1 > s2.
 */
int c_strcmp(const char* s1, const char* s2);

/**
 * Compare up to n characters of two strings.
 * Returns < 0 if s1 < s2, 0 if s1 == s2, > 0 if s1 > s2.
 */
int c_strncmp(const char* s1, const char* s2, size_t n);

/**
 * Concatenate src onto the end of dest (overwriting '\0' of dest),
 * and add a new '\0' terminator.
 * Returns dest.
 */
char* c_strcat(char* dest, const char* src);

/**
 * Append at most n characters from src onto dest, then add '\0'.
 * Returns dest.
 */
char* c_strncat(char* dest, const char* src, size_t n);

/**
 * Locate the first occurrence of character c in string s.
 * Returns pointer to that character in s, or NULL if not found.
 */
char* c_strchr(const char* s, int c);

/**
 * Locate the first occurrence of substring needle in haystack.
 * Returns pointer to the beginning of the substring, or NULL if not found.
 */
char* c_strstr(const char* haystack, const char* needle);


/**
 * A custom version of strtok that behaves similarly to the standard C library function.
 * 
 * Usage:
 *   - On the first call, pass the actual string in 'str'.
 *   - On subsequent calls, pass NULL in 'str' to continue tokenizing the same string.
 * 
 * The function modifies the original string by inserting '\0' at each delimiter.
 */
char* c_strtok(char* str, const char* delimiters);


#endif // STRING_LIB_H
