/**
 * File: test_unicode.c
 *
 * A test suite for your extended Unicode utilities:
 *   - utf8_next_codepoint
 *   - utf8_encode_codepoint
 *   - utf8_strlen
 *   - utf8_char_offset
 *   - utf8_substr
 *   - utf8_find_codepoint
 *   - utf8_casefold_codepoint
 *   - utf8_strtolower
 *
 * Includes stress testing for code points [1..0x2000],
 * skipping surrogates, to avoid embedded '\0'.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <assert.h>
 
 #include "unicode.h"   // The library under test
 #include "test_unicode.h"

 /* A helper for printing progress in the stress test */
 static void print_progress(size_t current, size_t total) {
     const int bar_width = 50;
     double ratio = (double)current / (double)total;
     int complete = (int)(ratio * bar_width);
 
     printf("\r[");
     for (int i = 0; i < bar_width; i++) {
         if (i < complete) {
             printf("=");
         } else {
             printf(" ");
         }
     }
     printf("] %3d%% (%zu/%zu)", (int)(ratio * 100.0), current, total);
     fflush(stdout);
 }
 
 /* -------------------- Basic Single Code Point Tests -------------------- */
 
 static void test_single_codepoint(int32_t codepoint) {
     char utf8_bytes[4];
     int n = utf8_encode_codepoint(codepoint, utf8_bytes);
 
     // If it's out of valid range or surrogate, we expect n==0
     if (codepoint < 0 || codepoint > 0x10FFFF ||
         (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
         assert(n == 0 && "Should return 0 for invalid/surrogate codepoint");
         return;
     }
     // Otherwise, we expect 1..4
     assert(n >= 1 && n <= 4);
 
     // Make a temporary null-terminated string
     char buffer[8];
     memcpy(buffer, utf8_bytes, n);
     buffer[n] = '\0';
 
     const char *p = buffer;
     int32_t decoded = utf8_next_codepoint(&p);
     assert(decoded == codepoint && "Mismatch in encode->decode single codepoint");
 
     // Another decode => end => -1
     int32_t no_more = utf8_next_codepoint(&p);
     assert(no_more == -1 && "Should be end after single codepoint");
 }
 
 /* -------------------- Known String & Invalid Sequences -------------------- */
 
 static void test_known_string(void) {
     const char *str = "Hello 你好";
     // Code points: 'H','e','l','l','o',' ', '你','好' => total 8
     int64_t length = utf8_strlen(str);
     assert(length == 8 && "utf8_strlen should be 8 for 'Hello 你好'");
 
     // Decode each code point
     const char *p = str;
     for (int i = 0; i < 8; i++) {
         int32_t c = utf8_next_codepoint(&p);
         assert(c >= 0 && "Should decode valid codepoint here");
     }
     // Next decode => end => -1
     int32_t end = utf8_next_codepoint(&p);
     assert(end == -1 && "Should be end of string now");
 }
 
 static void test_invalid_sequences(void) {
     // Single continuation byte
     {
         const char *p = "\x80";
         int32_t c = utf8_next_codepoint(&p);
         assert(c == -1 && "Standalone continuation byte should fail");
     }
 
     // Incomplete 2-byte: 0xC2
     {
         const char *p = "\xC2";
         int32_t c = utf8_next_codepoint(&p);
         assert(c == -1 && "Truncated 2-byte seq should fail");
     }
 
     // Surrogate in encode
     {
         char out[4];
         int n = utf8_encode_codepoint(0xD800, out);  // Surrogate
         assert(n == 0 && "Should treat surrogate halves as invalid");
     }
 }
 
 /* -------------------- Tests for New Functions -------------------- */
 
 /* Test utf8_char_offset */
 static void test_char_offset(void) {
     // Using the same known string: "Hello 你好"
     // The code points are: [ 'H'(0), 'e'(1), 'l'(2), 'l'(3), 'o'(4), ' '(5), '你'(6), '好'(7) ]
     const char *str = "Hello 你好";
     // utf8_strlen => 8
     // Let's confirm offsets
     // ASCII 'H' => offset 0
     // ASCII 'e' => offset 1
     // ASCII 'l' => offset 2
     // ASCII 'l' => offset 3
     // ASCII 'o' => offset 4
     // ASCII ' ' => offset 5
     // '你' => might be offset 7 or 6 depending on multi-byte
     // Actually let's do a direct check with the function
 
     // We'll get the total code points
     int64_t length = utf8_strlen(str);
     assert(length == 8);
 
     // We'll iterate each code point index and call utf8_char_offset
     for (int64_t i = 0; i < length; i++) {
         int64_t byte_off = utf8_char_offset(str, i);
         assert(byte_off >= 0 && "utf8_char_offset should be valid");
 
         // Now if we decode from that offset, we should be at the correct code point
         const char *start_p = &str[byte_off];
         int32_t c1 = utf8_next_codepoint(&start_p);
 
         // Compare it to decoding from the beginning i times
         const char *full_p = str;
         for (int64_t j = 0; j < i; j++) {
             int32_t skip_c = utf8_next_codepoint(&full_p);
             (void)skip_c; // ignore
         }
         int32_t c2 = utf8_next_codepoint(&full_p);
 
         assert(c1 == c2 && "utf8_char_offset leads to correct code point");
     }
 
     // Check out-of-range code point index => -1
     int64_t bad_off = utf8_char_offset(str, 9999);
     assert(bad_off == -1 && "Should fail on out-of-range index");
 }
 
 /* Test utf8_substr */
 static void test_substr(void) {
     // "Hello 你好" => 8 code points
     // We'll extract code points [0..4] => "Hello"
     // Then [6..2] => "你好"
     const char *str = "Hello 你好";
 
     char buffer[64];
     // Extract first 5 code points => "Hello"
     int64_t got = utf8_substr(str, 0, 5, buffer, sizeof(buffer));
     assert(got == 5 && "Should extract 5 code points");
     assert(strcmp(buffer, "Hello") == 0 && "Extracted substring should be 'Hello'");
 
     // Extract code points from index=6 of length=2 => "你好"
     got = utf8_substr(str, 6, 2, buffer, sizeof(buffer));
     assert(got == 2 && "Should extract 2 code points");
     // Compare with literal "你好"
     assert(strcmp(buffer, "你好") == 0 && "Should get '你好'");
 
     // If we ask for out-of-range => 0
     got = utf8_substr(str, 10, 2, buffer, sizeof(buffer));
     assert(got == 0 && "Out-of-range substring returns 0 code points");
     assert(strcmp(buffer, "") == 0 && "Should write an empty string on out-of-range");
 
     // Very small buffer => partial or 0
     char tiny[2];
     got = utf8_substr(str, 0, 5, tiny, sizeof(tiny));
     // We can't store "H" plus the null terminator => we can store 1 char + '\0'
     // Actually we do have space for "H" + '\0', so we might get "H" if the library checks carefully.
     // Implementation might store partial code points or none, let's just see if it doesn't crash.
     assert(got >= 0 && "Should not crash on tiny buffer");
 }
 
 /* Test utf8_find_codepoint */
 static void test_find_codepoint(void) {
     const char *str = "Hello 你好";
 
     // find 'H' => index 0
     int64_t idx = utf8_find_codepoint(str, 'H');
     assert(idx == 0 && "Should find 'H' at index 0");
 
     // find 'l' => first 'l' is index 2
     idx = utf8_find_codepoint(str, 'l');
     assert(idx == 2 && "First 'l' is code point index 2");
 
     // find '你' => code point index 6
     // 'H'(0) 'e'(1) 'l'(2) 'l'(3) 'o'(4) ' '(5) '你'(6) '好'(7)
     idx = utf8_find_codepoint(str, 0x4F60); // '你'
     assert(idx == 6 && "Should find '你' at index 6");
 
     // not found => -1
     idx = utf8_find_codepoint(str, 'Z');
     assert(idx == -1 && "Should not find 'Z' in 'Hello 你好'");
 }
 
 /* Test utf8_casefold_codepoint and utf8_strtolower */
 static void test_casefold_and_strtolower(void) {
     // ASCII-only approach: only A-Z => a-z
     // We'll test code points
     assert(utf8_casefold_codepoint('A') == 'a');
     assert(utf8_casefold_codepoint('Z') == 'z');
     // non-ASCII => unchanged
     assert(utf8_casefold_codepoint(0x410) == 0x410 && "Non-ASCII should be unchanged");
     // Surrogate => no sense, but let's see if we consider it invalid
     // Actually we skip checking that for the casefold.
 
     // test utf8_strtolower on a string
     // Example: "HelLo 你好 WORLD"
     const char *src = "HelLo 你好 WORLD";
     char buffer[128];
     int64_t count = utf8_strtolower(src, buffer, sizeof(buffer));
     assert(count > 0 && "Should convert successfully");
     // Expect => "hello 你好 world"
     assert(strcmp(buffer, "hello 你好 world") == 0 &&
            "Should lower only ASCII letters");
 
     // If we pass invalid sequences or no space => see if it returns -1, etc.
     char tiny[2];
     int64_t bad = utf8_strtolower("ABC", tiny, 2);
     // Possibly it can store "a" + '\0', let's see. Should not crash.
     assert(bad >= 0);
 }
 
 /* -------------------- Stress Test for 1..0x2000 -------------------- */
 
 static void stress_test_all_codepoints(void) {
     const int32_t high_limit = 0x2000;
 
     size_t estimated_size = 4 * (high_limit + 1);
     char *big_buffer = (char *)malloc(estimated_size);
     assert(big_buffer && "Failed to allocate big buffer for stress test");
 
     char *write_ptr = big_buffer;
     size_t total_codes = 0;
 
     for (int32_t code = 1; code <= high_limit; code++) {
         if (code >= 0xD800 && code <= 0xDFFF) {
             // skip surrogates
             continue;
         }
         char temp[4];
         int n = utf8_encode_codepoint(code, temp);
         if (n > 0) {
             memcpy(write_ptr, temp, n);
             write_ptr += n;
             total_codes++;
         }
     }
     *write_ptr = '\0';
 
     // Check length
     int64_t length = utf8_strlen(big_buffer);
     assert(length == (int64_t)total_codes && "utf8_strlen mismatch in stress test");
 
     // Decode all, verify
     const char *reader = big_buffer;
     size_t decoded_count = 0;
     for (int32_t code = 1; code <= high_limit; code++) {
         if (code >= 0xD800 && code <= 0xDFFF) {
             continue;
         }
         char temp[4];
         int n = utf8_encode_codepoint(code, temp);
         if (n > 0) {
             int32_t actual = utf8_next_codepoint(&reader);
             assert(actual == code && "Mismatch encode->decode in stress test");
             decoded_count++;
             if ((decoded_count % 256) == 0) {
                 print_progress(decoded_count, total_codes);
             }
         }
     }
     printf("\n");
 
     int32_t end_val = utf8_next_codepoint(&reader);
     assert(end_val == -1 && "Should be end after final codepoint");
 
     free(big_buffer);
     printf("[STRESS TEST] Completed. Encoded/decoded %zu code points up to 0x%04X.\n",
            decoded_count, high_limit);
 }
 
 /* -------------------- Main Test Entry Point -------------------- */
 
 void testUnicode(void) {
     printf("=== Running Extended Unicode Tests ===\n");
 
     // Basic single code point tests
     test_single_codepoint('A');
     test_single_codepoint(0x7FF);
     test_single_codepoint(0x800);
     test_single_codepoint(0xFFFF);
     test_single_codepoint(0x10000);
     test_single_codepoint(0x10FFFF);
     // Surrogate or out-of-range
     test_single_codepoint(0xD800);
     test_single_codepoint(0x110000);
     printf(" - Single code point encode/decode tests passed.\n");
 
     // Known string test
     test_known_string();
     printf(" - Known string decode checks passed.\n");
 
     // Invalid sequences
     test_invalid_sequences();
     printf(" - Invalid sequence checks passed.\n");
 
     // Extended function tests
     test_char_offset();
     printf(" - utf8_char_offset test passed.\n");
 
     test_substr();
     printf(" - utf8_substr test passed.\n");
 
     test_find_codepoint();
     printf(" - utf8_find_codepoint test passed.\n");
 
     test_casefold_and_strtolower();
     printf(" - utf8_casefold_codepoint + utf8_strtolower tests passed.\n");
 
     // Stress test
     printf("\n=== Unicode Stress Test ===\n");
     stress_test_all_codepoints();
 
     printf("\nAll Unicode tests passed!\n");
}
