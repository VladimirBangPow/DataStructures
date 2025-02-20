#ifndef UNICODE_H
#define UNICODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>  // for size_t

/**
 * Reads the next Unicode code point from a UTF-8 string.
 * Returns the code point (>= 0) on success, or -1 on invalid UTF-8 or end of string.
 * Advances *str to the next character boundary.
 */
int32_t utf8_next_codepoint(const char **str);

/**
 * Encodes a single Unicode code point (0x0000..0x10FFFF, excluding surrogates)
 * into up to 4 UTF-8 bytes.
 * - Writes to `out[4]`.
 * - Returns number of bytes written (1..4), or 0 if invalid/out-of-range.
 */
int utf8_encode_codepoint(int32_t codepoint, char out[4]);

/**
 * Counts how many *valid* Unicode code points are in this null-terminated
 * UTF-8 string. 
 * Returns -1 if any invalid sequence is encountered.
 */
int64_t utf8_strlen(const char *utf8_string);

/**
 * Returns the *byte offset* (in `utf8_string`) of the code point at index `n`
 * (where 0-based indexing is by code point).
 * - If `n` is out of range, returns -1.
 * - If invalid sequences occur, returns -1.
 */
int64_t utf8_char_offset(const char *utf8_string, int64_t n);

/**
 * Extracts a substring of `length` code points starting at code point index `start`
 * from `src`, writing into `dst` (null-terminated).
 * - `dst_size` is the maximum number of bytes that can be written to `dst`.
 * - If successful, returns the number of code points copied.
 * - On invalid UTF-8 or if `start` or `length` goes out of range, writes an empty string and returns 0.
 */
int64_t utf8_substr(const char *src, int64_t start, int64_t length, char *dst, size_t dst_size);

/**
 * Finds the first occurrence of codepoint `cp` in `str` (null-terminated),
 * in terms of code point index (0-based).
 * - Returns the code point index, or -1 if not found or if invalid sequences appear.
 */
int64_t utf8_find_codepoint(const char *str, int32_t cp);

/**
 * A naive ASCII-only case folding function that:
 * - Returns the lowercase code point if `cp` is ASCII A-Z.
 * - Otherwise returns `cp` unchanged.
 * This is not a full Unicode case fold, just an example for ASCII.
 */
int32_t utf8_casefold_codepoint(int32_t cp);

/**
 * Applies the above case folding to each code point in `src`, writing to `dst`.
 * - Writes a null-terminated string.
 * - `dst_size` is the max byte capacity of `dst`.
 * - Returns the number of code points written, or -1 on invalid sequences.
 *
 * This is ASCII-only for demonstration. For real Unicode case folding,
 * you'd need a large data table or external library (ICU).
 */
int64_t utf8_strtolower(const char *src, char *dst, size_t dst_size);

#ifdef __cplusplus
}
#endif

#endif /* UNICODE_H */
