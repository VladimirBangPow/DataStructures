#include "unicode.h"

#include <stdlib.h>   // for malloc, free
#include <stdio.h>    // for fprintf
#include <stdbool.h>  // for bool
#include <string.h>   // for memcpy
#include <stdint.h>
#include <stddef.h>

/**
 * decode_utf8_codepoint_no_bits:
 *   Attempts to decode a single Unicode code point from the bytes at ptr.
 *
 *   On success:
 *     - returns the decoded code point (>= 0).
 *     - sets *bytes_used = number of bytes consumed (1..4).
 *   On failure:
 *     - returns -1.
 *     - sets *bytes_used to how many bytes we attempted/consumed.
 */
static int32_t decode_utf8_codepoint_no_bits(const unsigned char *ptr, int *bytes_used) {
    // If we see a 0 byte => end of string
    if (*ptr == 0) {
        *bytes_used = 0;
        return -1;
    }

    unsigned char b0 = ptr[0];
    int32_t code = 0;
    int needed = 0;

    // 1) Determine how many bytes by checking decimal ranges.
    //    (Normally we'd check leading bits with e.g. (b0 & 0xE0) == 0xC0,
    //     but here we do it purely by comparing decimal values.)
    if (b0 < 128) {
        // 0 <= b0 <= 127 => 1-byte sequence (ASCII)
        code = b0; // just that byte
        needed = 1;
    }
    else if (b0 >= 192 && b0 < 224) {
        // 192..223 => 2-byte sequence
        // The top bits are "110xxxxx" in binary, but we interpret them by decimal offset.
        // e.g., if b0=194 => code = (194 - 192)=2 => that represents the top 5 bits
        code = b0 - 192; 
        needed = 2;
    }
    else if (b0 >= 224 && b0 < 240) {
        // 224..239 => 3-byte sequence
        code = b0 - 224;
        needed = 3;
    }
    else if (b0 >= 240 && b0 < 248) {
        // 240..247 => 4-byte sequence
        code = b0 - 240;
        needed = 4;
    }
    else {
        // invalid or > 4-byte
        *bytes_used = 1; // skip one byte to avoid getting stuck
        return -1;
    }

    // 2) Read the continuation bytes. Each should be 128..191 (decimal).
    //    In bit terms, that’s 10xxxxxx (0x80..0xBF).
    for (int i = 1; i < needed; i++) {
        unsigned char bn = ptr[i];
        if (bn < 128 || bn >= 192) {
            // not a valid continuation (should be 128..191)
            *bytes_used = i;
            return -1;
        }
        // SHIFT code by 6 bits => "code = code << 6", but we do decimal:
        //   code = code * 64
        code = code * 64;

        // ADD the lower 6 bits => "code |= bn & 0x3F", but we do decimal:
        //   code = code + (bn - 128)
        code = code + (bn - 128);
    }

    *bytes_used = needed;
    return code;
}

/**
 * utf8_next_codepoint:
 *   A user-facing function that decodes the next code point from *str,
 *   updates *str to point after that character, and returns the code point or -1 on error.
 *
 *   This mimics the typical "utf8_next_codepoint" style but avoids bit ops.
 */
int32_t utf8_next_codepoint(const char **str) {
    if (!str || !*str) return -1;
    const unsigned char *s = (const unsigned char *)(*str);
    if (*s == 0) {
        return -1; // end of string
    }

    int bytes_used = 0;
    int32_t code = decode_utf8_codepoint_no_bits(s, &bytes_used);
    if (code < 0) {
        // error => skip 1 byte to avoid infinite loop
        (*str) = (const char *)(s + 1);
        return -1;
    }

    // Advance pointer by the number of bytes we used
    s += bytes_used;

    // Range checks:
    //  - code > 0x10FFFF => invalid in Unicode
    //  - code in 0xD800..0xDFFF => surrogate => invalid for UTF-8
    if (code > 0x10FFFF || (code >= 0xD800 && code <= 0xDFFF)) {
        (*str) = (const char *)s;
        return -1;
    }

    // success
    (*str) = (const char *)s;
    return code;
}

/*
 * utf8_encode_codepoint_no_bits:
 *   Encodes `codepoint` into the array `out[4]` using UTF-8 rules, but
 *   avoids bitwise operators. Returns how many bytes were written (1..4),
 *   or 0 if `codepoint` is invalid.
 *
 *   You can expand the comments to see how the arithmetic corresponds
 *   to splitting bits. (codepoint / 64) is effectively (codepoint >> 6),
 *   (codepoint % 64) is effectively (codepoint & 0x3F), etc.
 */
int utf8_encode_codepoint(int32_t codepoint, char out[4]) {
    // 1) Basic validity checks
    //    - codepoint < 0 or > 0x10FFFF => out of Unicode range
    //    - 0xD800..0xDFFF => surrogates (invalid in UTF-8)
    if (codepoint < 0 || codepoint > 0x10FFFF) {
        return 0;
    }
    if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
        return 0;
    }

    // 2) Encode based on range
    if (codepoint <= 0x7F) {
        // -- 1-byte sequence --
        // ASCII: direct
        // codepoint is in 0..127
        out[0] = (char)codepoint;  // e.g., 65 => 'A'
        return 1;

    } else if (codepoint <= 0x7FF) {
        // -- 2-byte sequence --
        // Byte 1: 192..223 (decimal), which is 110xxxxx in binary
        //   out[0] = 192 + (top 5 bits)
        // Byte 2: 128..191 (decimal), which is 10xxxxxx in binary
        //   out[1] = 128 + (lower 6 bits)

        // "top 5 bits" in decimal => codepoint / 64
        // "lower 6 bits" => codepoint % 64
        int32_t top5  = codepoint / 64;      // codepoint >> 6
        int32_t lower6 = codepoint % 64;     // codepoint & 0x3F

        out[0] = (char)(192 + top5);         // 192 = 0xC0
        out[1] = (char)(128 + lower6);       // 128 = 0x80
        return 2;

    } else if (codepoint <= 0xFFFF) {
        // -- 3-byte sequence --
        // Byte 1: 224..239 => 1110xxxx
        // Byte 2: 128..191 => 10xxxxxx
        // Byte 3: 128..191 => 10xxxxxx
        //
        // top4 = codepoint / 4096  (like codepoint >> 12)
        // mid6 = (codepoint / 64) % 64   (like (codepoint >> 6) & 0x3F)
        // low6 = codepoint % 64   (like codepoint & 0x3F)

        int32_t top4  = codepoint / 4096;  // 64*64 = 4096
        int32_t mid6  = (codepoint / 64) % 64;
        int32_t low6  = codepoint % 64;

        out[0] = (char)(224 + top4);   // 224 = 0xE0
        out[1] = (char)(128 + mid6);   // 128 = 0x80
        out[2] = (char)(128 + low6);
        return 3;

    } else {
        // -- 4-byte sequence --
        // Byte 1: 240..247 => 11110xxx
        // Byte 2: 128..191 => 10xxxxxx
        // Byte 3: 128..191 => 10xxxxxx
        // Byte 4: 128..191 => 10xxxxxx
        //
        // top3 = codepoint / 262144   (like >> 18)
        // mid6a = (codepoint / 4096) % 64
        // mid6b = (codepoint / 64) % 64
        // low6  = codepoint % 64

        int32_t top3   = codepoint / 262144;       // 64*64*64 = 262144
        int32_t mid6a  = (codepoint / 4096) % 64;
        int32_t mid6b  = (codepoint / 64) % 64;
        int32_t low6   = codepoint % 64;

        out[0] = (char)(240 + top3);    // 240 = 0xF0
        out[1] = (char)(128 + mid6a);
        out[2] = (char)(128 + mid6b);
        out[3] = (char)(128 + low6);
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
