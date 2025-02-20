#ifndef TEST_UNICODE_H
#define TEST_UNICODE_H

/*
 * The main test entry point for the Unicode library.
 * This function runs unit tests for:
 *   - utf8_next_codepoint
 *   - utf8_encode_codepoint
 *   - utf8_strlen
 *
 * It also includes a stress test that tries encoding/decoding many code points.
 */
void testUnicode(void);

#endif /* TEST_UNICODE_H */
