#include "unicode.h"

#include <stdlib.h>   // for malloc, free
#include <stdio.h>    // for fprintf
#include <stdbool.h>  // for bool
#include <string.h>   // for memcpy

/*
 * Reads the next Unicode code point from a UTF-8 string.
 * (Already discussed in prior examples.)
 */
int32_t utf8_next_codepoint(const char **str) {
    if (!str || !*str) return -1;
    const unsigned char *s = (const unsigned char *)(*str);
    if (*s == 0) {
        return -1; // end of string
    }

    int32_t code = 0;
    int bytes = 0;

    // Determine how many bytes in this UTF-8 char
    if ((s[0] & 0x80) == 0) {
        // 1-byte ASCII: 0xxxxxxx
        code = s[0];
        s++;
        bytes = 1;
    } else if ((s[0] & 0xe0) == 0xc0) {
        // 2-byte: 110xxxxx 10xxxxxx
        code = (s[0] & 0x1f);
        s++;
        bytes = 2;
    } else if ((s[0] & 0xf0) == 0xe0) {
        // 3-byte: 1110xxxx 10xxxxxx 10xxxxxx
        code = (s[0] & 0x0f);
        s++;
        bytes = 3;
    } else if ((s[0] & 0xf8) == 0xf0) {
        // 4-byte: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        code = (s[0] & 0x07);
        s++;
        bytes = 4;
    } else {
        // invalid or >4-byte UTF-8
        (*str) = (const char *)(s + 1);
        return -1;
    }

    // Pull in the continuation bytes
    for (int i = 1; i < bytes; i++) {
        if ((s[0] & 0xc0) != 0x80) {
            // not a valid continuation
            (*str) = (const char *)(s + 1);
            return -1;
        }
        code = (code << 6) | (s[0] & 0x3f);
        s++;
    }

    // Surrogate check and range check
    if (code < 0 || code > 0x10FFFF) {
        (*str) = (const char *)s;
        return -1;
    }
    // Surrogates 0xD800..0xDFFF => invalid in UTF-8
    if (code >= 0xD800 && code <= 0xDFFF) {
        (*str) = (const char *)s;
        return -1;
    }

    // Advance the caller's pointer
    (*str) = (const char *)s;
    return code;
}

/*
 * Encodes a single Unicode code point into UTF-8 bytes.
 * Returns number of bytes (1..4), or 0 if invalid.
 */
int utf8_encode_codepoint(int32_t codepoint, char out[4]) {
    // Range check
    if (codepoint < 0 || codepoint > 0x10FFFF) {
        return 0;
    }
    // Surrogates not allowed
    if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
        return 0;
    }

    if (codepoint <= 0x7F) {
        // 1-byte
        out[0] = (char)codepoint;
        return 1;
    } else if (codepoint <= 0x7FF) {
        // 2-byte
        out[0] = (char)(0xC0 | (codepoint >> 6));
        out[1] = (char)(0x80 | (codepoint & 0x3F));
        return 2;
    } else if (codepoint <= 0xFFFF) {
        // 3-byte
        out[0] = (char)(0xE0 | (codepoint >> 12));
        out[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        out[2] = (char)(0x80 | (codepoint & 0x3F));
        return 3;
    } else {
        // 4-byte
        out[0] = (char)(0xF0 | (codepoint >> 18));
        out[1] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
        out[2] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        out[3] = (char)(0x80 | (codepoint & 0x3F));
        return 4;
    }
}

/*
 * utf8_strlen: Count how many valid code points until we hit '\0'
 * or an invalid sequence. Returns -1 if invalid is encountered.
 */
int64_t utf8_strlen(const char *utf8_string) {
    if (!utf8_string) return -1;

    int64_t count = 0;
    const char *p = utf8_string;
    while (true) {
        int32_t c = utf8_next_codepoint(&p);
        if (c < 0) {
            // if c == -1 because we reached the '\0', it means end. 
            // But utf8_next_codepoint also returns -1 on invalid.
            // Let's distinguish: if *p == 0 => we truly ended. If not => invalid.
            if (*p == '\0') {
                // normal end
                break;
            }
            // invalid
            return -1;
        }
        count++;
    }
    return count;
}

/*
 * Returns the byte offset of the nth code point (0-based) in a null-terminated UTF-8 string.
 * If n is out of range or an invalid sequence is found, returns -1.
 */
int64_t utf8_char_offset(const char *utf8_string, int64_t n) {
    if (!utf8_string || n < 0) return -1;

    const char *p = utf8_string;
    int64_t index = 0; // code point index
    int64_t byte_offset = 0;

    while (true) {
        if (*p == '\0') {
            // Reached end of string - if we haven't found n yet, out of range
            return (index == n) ? byte_offset : -1;
        }
        // See where we are before reading
        byte_offset = (p - utf8_string);

        int32_t c = utf8_next_codepoint(&p);
        if (c < 0) {
            // invalid sequence
            return -1;
        }
        if (index == n) {
            // The offset we want
            return byte_offset;
        }
        index++;
    }
    // unreachable logically
}

/*
 * Extracts a substring of `length` code points starting at code point `start`.
 * Writes null-terminated result to `dst` if there's space.
 * Returns number of code points written, or 0 on error/out-of-range.
 */
int64_t utf8_substr(const char *src, int64_t start, int64_t length, char *dst, size_t dst_size) {
    if (!src || !dst || dst_size == 0 || start < 0 || length < 0) {
        if (dst_size > 0) {
            dst[0] = '\0';
        }
        return 0;
    }

    // Move to the start code point
    const char *p = src;
    int64_t idx = 0;
    while (idx < start) {
        if (*p == '\0') {
            // out of range
            dst[0] = '\0';
            return 0;
        }
        int32_t c = utf8_next_codepoint(&p);
        if (c < 0) {
            // invalid
            dst[0] = '\0';
            return 0;
        }
        idx++;
    }

    // Now read `length` code points from here, encode to dst
    size_t written_bytes = 0;
    int64_t count_cp = 0; // how many code points we've actually written

    while (count_cp < length) {
        if (*p == '\0') {
            // reached end
            break;
        }
        // read next code point
        const char *prev_p = p;
        int32_t c = utf8_next_codepoint(&p);
        if (c < 0) {
            // invalid
            break;
        }

        // encode c into dst
        char temp[4];
        int n = utf8_encode_codepoint(c, temp);
        if (n <= 0) {
            // shouldn't happen if decode was valid, but in case
            break;
        }
        // check space
        if (written_bytes + n >= dst_size) {
            // no space for these bytes + null terminator
            break;
        }
        // copy
        memcpy(&dst[written_bytes], temp, n);
        written_bytes += n;
        count_cp++;
    }
    // null terminate
    if (written_bytes < dst_size) {
        dst[written_bytes] = '\0';
    } else {
        // if we exactly used up dst_size - 1 for bytes, we do:
        dst[dst_size - 1] = '\0';
    }
    return count_cp;
}

/*
 * Finds the first occurrence of `cp` in `str`, returning the code point index (0-based).
 * Returns -1 if not found or invalid sequences encountered.
 */
int64_t utf8_find_codepoint(const char *str, int32_t cp) {
    if (!str) return -1;
    // check if cp is in range
    if (cp < 0 || cp > 0x10FFFF ||
        (cp >= 0xD800 && cp <= 0xDFFF)) {
        // never valid
        return -1;
    }

    const char *p = str;
    int64_t index = 0;
    while (*p != '\0') {
        const char *before = p;
        int32_t c = utf8_next_codepoint(&p);
        if (c < 0) {
            // invalid sequence
            return -1;
        }
        if (c == cp) {
            return index;
        }
        index++;
    }
    // not found
    return -1;
}

/*
 * A naive ASCII-only case folding for a single code point:
 * If 'A'..'Z', make 'a'..'z'. Else return unchanged.
 */
int32_t utf8_casefold_codepoint(int32_t cp) {
    // ASCII range for uppercase letters: 0x41..0x5A
    if (cp >= 0x41 && cp <= 0x5A) {
        return cp + 0x20; // convert to lowercase
    }
    return cp; // no change for everything else
}

/*
 * Converts all code points in `src` to lowercase (ASCII only),
 * writing to `dst`. Returns the number of code points written,
 * or -1 on invalid sequences.
 */
int64_t utf8_strtolower(const char *src, char *dst, size_t dst_size) {
    if (!src || !dst || dst_size == 0) {
        if (dst_size > 0) {
            dst[0] = '\0';
        }
        return -1;
    }

    size_t written_bytes = 0;
    int64_t count_cp = 0; 

    const char *p = src;
    while (true) {
        if (*p == '\0') {
            // end
            break;
        }
        int32_t c = utf8_next_codepoint(&p);
        if (c < 0) {
            // invalid
            dst[0] = '\0';
            return -1;
        }
        c = utf8_casefold_codepoint(c);

        // encode
        char temp[4];
        int n = utf8_encode_codepoint(c, temp);
        if (n <= 0) {
            // invalid or can't encode
            dst[0] = '\0';
            return -1;
        }

        // space check: we need n bytes + 1 for null at the end
        if ((written_bytes + n) >= dst_size) {
            // can't fit
            break;
        }
        memcpy(&dst[written_bytes], temp, n);
        written_bytes += n;
        count_cp++;
    }

    // null-terminate
    if (written_bytes < dst_size) {
        dst[written_bytes] = '\0';
    } else {
        dst[dst_size - 1] = '\0';
    }

    return count_cp;
}
