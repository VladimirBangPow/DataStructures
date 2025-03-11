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

/**
 * Helper function to test a single scenario:
 *  1) We copy the input string to a local buffer (so we can modify it).
 *  2) We repeatedly call c_strtok() with the given delimiters.
 *  3) We compare the extracted tokens with the expected array of tokens.
 */
static void runTestCase(const char* input,
                        const char* delimiters,
                        const char* expectedTokens[], 
                        int expectedCount) 
{
    // Make a local modifiable copy of 'input'
    char buffer[200];
    strncpy(buffer, input, sizeof(buffer));
    // Ensure null-terminated
    buffer[sizeof(buffer)-1] = '\0';

    //printf("Testing input=\"%s\" with delimiters=\"%s\"...\n", input, delimiters);

    // We'll extract tokens into an array of char* for easier checking
    const char* actualTokens[50]; // up to 50 tokens
    int actualCount = 0;

    // 1) First call with 'buffer'
    char* token = c_strtok(buffer, delimiters);
    while (token != NULL) {
        actualTokens[actualCount++] = token;  // pointer to substring inside buffer
        // Subsequent calls with NULL
        token = c_strtok(NULL, delimiters);
    }

    // 2) Verify that the actual tokens match the expected tokens
    assert(actualCount == expectedCount && "Number of tokens does not match expectedCount");

    for (int i = 0; i < actualCount; i++) {
        assert(strcmp(actualTokens[i], expectedTokens[i]) == 0 && "Token mismatch");
    }

    //printf(" -> Passed! Extracted %d tokens as expected.\n", actualCount);
}

void test_c_strtok(void) {
	printf("\n-- test_c_strtok --\n");

    // Test Case 1: Basic space delimiter
    {
        const char* input = "Hello World from C";
        const char* delimiters = " ";
        const char* expected[] = { "Hello", "World", "from", "C" };
        runTestCase(input, delimiters, expected, 4);
    }

    // Test Case 2: Multiple delimiters (space & comma)
    {
        const char* input = "One,Two  Three,,  Four";
        const char* delimiters = " ,";  // space or comma
        const char* expected[] = { "One", "Two", "Three", "Four" };
        runTestCase(input, delimiters, expected, 4);
    }

    // Test Case 3: Leading delimiters
    {
        const char* input = "   Leading delim test";
        const char* delimiters = " ";
        const char* expected[] = { "Leading", "delim", "test" };
        runTestCase(input, delimiters, expected, 3);
    }

    // Test Case 4: Trailing delimiters
    {
        const char* input = "Trailing test   ";
        const char* delimiters = " ";
        const char* expected[] = { "Trailing", "test" };
        runTestCase(input, delimiters, expected, 2);
    }

    // Test Case 5: String with only delimiters
    {
        const char* input = "     ";
        const char* delimiters = " ";
        // No tokens
        const char* expected[1] = { NULL };
        runTestCase(input, delimiters, expected, 0);
    }

    // Test Case 6: Single token, no delimiters present
    {
        const char* input = "JustOneToken";
        const char* delimiters = " ,";
        const char* expected[] = { "JustOneToken" };
        runTestCase(input, delimiters, expected, 1);
    }

    // Test Case 7: Repeated delimiter chars
    {
        const char* input = "!!!Hello!!!World!!!";
        const char* delimiters = "!";
        // Should skip consecutive '!' and find "Hello", "World"
        const char* expected[] = { "Hello", "World" };
        runTestCase(input, delimiters, expected, 2);
    }

    // All tests passed if we reach here
    printf("test_c_strtok passed!\n");
}


/**
 * A helper function to test a single scenario.
 * We compare the pointer returned by my_strrchr to the pointer returned
 * by the standard library's strrchr for reference. Then we do additional checks.
 */
static void runTestCase2(const char* s, int c) {
    // printf("Testing c_strrchr with s=\"%s\" and c='%c' (or 0x%02X)\n", s, (char)c, (unsigned char)c);

    // Let’s find the expected result with standard strrchr (if available).
    // If you need strict portability, you can remove or wrap this.
    const char* expected = strrchr(s, c);

    // Use our custom function
    char* actual = c_strrchr(s, c);

    // 1) If both pointers are NULL, we are good. The character wasn't found.
    if (!expected && !actual) {
        // printf(" -> Both are NULL. OK.\n");
        return;
    }

    // 2) Otherwise, we check if they're both non-NULL and pointing to equivalent location
    // The standard library might return a pointer into s; likewise, we do the same.
    // Compare the difference from the start of the string.
    if (expected && actual) {
        // offset from the start
        ptrdiff_t expectedOffset = expected - s;
        ptrdiff_t actualOffset = actual - s;
        assert(expectedOffset == actualOffset && "Offsets do not match!");
        // printf(" -> Both point to s[%td]. OK.\n", actualOffset);

        // Double-check the character is indeed c
        assert(*actual == (char)c || (c == '\0' && *actual == '\0'));
    } else {
        // Mismatch: one is NULL, the other is not
        assert(0 && "One pointer is NULL while the other is not!");
    }
}

/**
 * Test driver for my_strrchr. Exercises various cases with asserts.
 */
void test_c_strrchr(void) {
    printf("\n--- test_c_strrchr ---\n");

    // 1) Character appears multiple times
    runTestCase2("Hello World, Hello Universe!", 'o');
    // Last 'o' is in "Hello Universe!" => index 25 if zero-based

    // 2) Character appears only once
    runTestCase2("abcdefg", 'f');

    // 3) Character does not appear at all
    runTestCase2("abcdefg", 'X');

    // 4) Searching for '\0' (null terminator)
    // We expect strrchr to return pointer to the terminating '\0'.
    runTestCase2("abcdefg", '\0');

    // 5) Empty string edge case
    runTestCase2("", 'a');
    runTestCase2("", '\0'); // should point to the '\0' at the start

    // 6) Last char is the one we’re searching for
    runTestCase2("abcxyz!", '!');

    // 7) Repeated char at the end
    runTestCase2("Test ???", '?');

    // 8) Some repeated middle char to confirm we get the last
    runTestCase2("Mississippi", 's'); // The last 's' is at index 5-based from end or so

    printf("test_c_strrchr passed!\n");
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
	test_c_strrchr();
    test_c_strstr();
	test_c_strtok();
    printf("\nAll string library tests passed successfully!\n");
}
