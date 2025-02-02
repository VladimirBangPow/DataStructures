#include "test_cstring.h"
#include "cstring.h"

#include <stdio.h>
#include <assert.h>
#include <string.h> // using <string.h> for some baseline checks in tests

// Helper function to compare our results with the standard C library (for reference)
static void test_c_strlen(void) {
    printf("\n-- test_c_strlen --\n");

    // Checking an empty string
    assert(c_strlen("") == 0);

    // Checking some sample strings
    assert(c_strlen("Hello") == 5);
    assert(c_strlen("Hello, World!") == 13);

    char buffer[50] = "Testing 123";
    // Compare with standard strlen
    assert(c_strlen(buffer) == strlen(buffer));

    printf("test_c_strlen passed!\n");
}

static void test_c_strcpy(void) {
    printf("\n-- test_c_strcpy --\n");

    char dest[50];
    const char* source = "Hello";

    c_strcpy(dest, source);
    assert(strcmp(dest, source) == 0);

    c_strcpy(dest, "");
    assert(strcmp(dest, "") == 0);

    printf("test_c_strcpy passed!\n");
}

static void test_c_strncpy(void) {

	printf("\n-- test_c_strncpy --\n");

    char dest[50];
    const char* source = "Hello";

    // Copy partial
    c_strncpy(dest, source, 2);
   	printf("%s\n", dest);
    // dest should be "He\0..."
    assert(dest[0] == 'H' && dest[1] == 'e' && dest[2] == '\0');

    // Copy full
    c_strncpy(dest, source, 5);
    // check if final is not explicitly terminated => we have 5 non-null chars
    // but we need to see if we are consistent
    assert(strncmp(dest, "Hello", 5) == 0);

    // Copy beyond the length of source
    // Should fill with '\0'
    c_strncpy(dest, source, 10);
    // "Hello" + 5 trailing zeros
    assert(strcmp(dest, "Hello") == 0); // once we hit '\0', the rest won't matter for strcmp

    printf("test_c_strncpy passed!\n");
}

static void test_c_strcmp(void) {
    printf("\n-- test_c_strcmp --\n");

    // Compare identical strings
    assert(c_strcmp("abc", "abc") == 0);

    // Compare s1 < s2
    assert(c_strcmp("abc", "abd") < 0);
    // Compare s1 > s2
    assert(c_strcmp("abd", "abc") > 0);

    // Compare with prefix condition
    assert(c_strcmp("abc", "abcd") < 0);
    assert(c_strcmp("abcd", "abc") > 0);

    // Compare with empty string
    assert(c_strcmp("", "") == 0);
    assert(c_strcmp("", "abc") < 0);
    assert(c_strcmp("abc", "") > 0);

    printf("test_c_strcmp passed!\n");
}

static void test_c_strncmp(void) {
    printf("\n-- test_c_strncmp --\n");

    // Compare first n=3 chars
    assert(c_strncmp("abcdef", "abcxyz", 3) == 0);
    // If we compare 4 chars, difference emerges
    assert(c_strncmp("abcdef", "abcxyz", 4) < 0);

    // Compare with smaller n
    assert(c_strncmp("abc", "abc", 2) == 0);

    // Compare zero n => always 0
    assert(c_strncmp("abc", "xyz", 0) == 0);

    printf("test_c_strncmp passed!\n");
}

static void test_c_strcat(void) {
    printf("\n-- test_c_strcat --\n");

    char dest[50] = "Hello";
    const char* src = " World";

    c_strcat(dest, src);
    assert(strcmp(dest, "Hello World") == 0);

    // Empty src
    c_strcat(dest, "");
    assert(strcmp(dest, "Hello World") == 0);

    printf("test_c_strcat passed!\n");
}

static void test_c_strncat(void) {
    printf("\n-- test_c_strncat --\n");

    char dest[50] = "Hello";
    c_strncat(dest, " WorldXYZ", 6); // " WorldX", but only 6 => " World"
    assert(strcmp(dest, "Hello World") == 0);

    // Edge case: n = 0
    c_strncat(dest, "Impossible", 0);
    assert(strcmp(dest, "Hello World") == 0);

    printf("test_c_strncat passed!\n");
}

static void test_c_strchr(void) {
    printf("\n-- test_c_strchr --\n");

    const char* s = "Hello World";
    char* result = c_strchr(s, 'W');
    assert(result != NULL && *result == 'W');

    result = c_strchr(s, 'X');
    assert(result == NULL);

    // Check for '\0'
    result = c_strchr(s, '\0');
    // Should return pointer to string terminator => s + strlen(s)
    assert(result == s + strlen(s));

    printf("test_c_strchr passed!\n");
}

static void test_c_strstr(void) {
    printf("\n-- test_c_strstr --\n");

    const char* s = "Hello World";
    // Substring "World"
    char* result = c_strstr(s, "World");
    assert(result != NULL && strcmp(result, "World") == 0);

    // Non-existent substring
    result = c_strstr(s, "xyz");
    assert(result == NULL);

    // Empty needle
    result = c_strstr(s, "");
    // By convention, returns s
    assert(result == s);

    // Substring at the beginning
    result = c_strstr(s, "Hello");
    assert(result == s);

    printf("test_c_strstr passed!\n");
}

// Driver function that calls all the sub-tests
void testCString(void) {
    test_c_strlen();
    test_c_strcpy();
    test_c_strncpy();
    test_c_strcmp();
    test_c_strncmp();
    test_c_strcat();
    test_c_strncat();
    test_c_strchr();
    test_c_strstr();

    printf("\nAll string library tests passed successfully!\n");
}
