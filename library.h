/*
 * This code is distributed under the terms of the GNU General Public License.
 * For more information, please refer to the LICENSE file in the root directory.
 * -------------------------------------------------
 * Copyright (C) 2025 Rodrigo R.
 * This program comes with ABSOLUTELY NO WARRANTY; for details type show w'.
 * This is free software, and you are welcome to redistribute it
 * under certain conditions; type show c' for details.
*/

#ifndef FLUENT_LIBC_STR_CONV_LIBRARY_H
#define FLUENT_LIBC_STR_CONV_LIBRARY_H

// ============= FLUENT LIB C =============
#ifndef FLUENT_LIBC_RELEASE
#   include <string_builder.h> // fluent_libc
#else
#   include <fluent/string_builder/string_builder.h> // fluent_libc
#endif

// ============= INCLUDES =============
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Parses a 4-character hexadecimal string and returns its integer value.
 *
 * \param str Pointer to a string containing at least 4 hexadecimal characters.
 * \return The integer value of the 4 hex digits, or -1 if an invalid character is found.
 *
 * Example: parse_hex4("1a2F") returns 6703.
 */
static inline int parse_hex4(const char *str) {
    int val = 0;
    for (int i = 0; i < 4; i++) {
        const char c = str[i];
        val <<= 4; // Shift left by 4 bits to make room for the next hex digit
        if      ('0' <= c && c <= '9') val |= (c - '0');         // Handle numeric digits
        else if ('a' <= c && c <= 'f') val |= (c - 'a' + 10);    // Handle lowercase hex letters
        else if ('A' <= c && c <= 'F') val |= (c - 'A' + 10);    // Handle uppercase hex letters
        else return -1; // invalid hex character found
    }
    return val;
}

/**
 * Encodes a Unicode code point as UTF-8 and appends the bytes to a string builder.
 *
 * \param codepoint The Unicode code point to encode (must be in range 0x0 to 0x10FFFF).
 * \param builder Pointer to a string_builder_t where the UTF-8 bytes will be appended.
 *
 * The function writes 1 to 4 bytes to the string builder, depending on the value of the code point:
 *   - 1 byte for code points <= 0x7F
 *   - 2 bytes for code points <= 0x7FF
 *   - 3 bytes for code points <= 0xFFFF
 *   - 4 bytes for code points <= 0x10FFFF
 * No output is produced for invalid code points (> 0x10FFFF).
 */
static inline void encode_utf8_sb(uint32_t codepoint, string_builder_t *builder) {
    if (codepoint <= 0x7F) {
        write_char_string_builder(builder, codepoint);
    } else if (codepoint <= 0x7FF) {
        write_char_string_builder(builder, 0xC0 | (codepoint >> 6));
        write_char_string_builder(builder, 0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0xFFFF) {
        write_char_string_builder(builder, 0xE0 | (codepoint >> 12));
        write_char_string_builder(builder, 0x80 | ((codepoint >> 6) & 0x3F));
        write_char_string_builder(builder, 0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0x10FFFF) {
        write_char_string_builder(builder, 0xF0 | (codepoint >> 18));
        write_char_string_builder(builder, 0x80 | ((codepoint >> 12) & 0x3F));
        write_char_string_builder(builder, 0x80 | ((codepoint >> 6) & 0x3F));
        write_char_string_builder(builder, 0x80 | (codepoint & 0x3F));
    }
}

static inline char *convert_escapes_to_utf8_sb(const char *input) {
    string_builder_t builder;
    init_string_builder(&builder, 64, 1.5); // start with 64 bytes, 1.5 growth factor

    const char *p = input;
    while (*p) {
        if (p[0] == '\\') {
            switch (p[1]) {
                case 'n': write_char_string_builder(&builder, '\n'); p += 2; break;
                case 'r': write_char_string_builder(&builder, '\r'); p += 2; break;
                case 't': write_char_string_builder(&builder, '\t'); p += 2; break;
                case '\\': write_char_string_builder(&builder, '\\'); p += 2; break;
                case '"': write_char_string_builder(&builder, '"'); p += 2; break;
                case 'u':
                    if (isxdigit(p[2]) && isxdigit(p[3]) &&
                        isxdigit(p[4]) && isxdigit(p[5])) {

                        int code1 = parse_hex4(p + 2);
                        if (code1 >= 0xD800 && code1 <= 0xDBFF &&  // high surrogate
                            p[6] == '\\' && p[7] == 'u' &&
                            isxdigit(p[8]) && isxdigit(p[9]) &&
                            isxdigit(p[10]) && isxdigit(p[11])) {

                            int code2 = parse_hex4(p + 8);
                            if (code2 >= 0xDC00 && code2 <= 0xDFFF) { // low surrogate
                                uint32_t high = code1 - 0xD800;
                                uint32_t low = code2 - 0xDC00;
                                uint32_t codepoint = (high << 10) + low + 0x10000;
                                encode_utf8_sb(codepoint, &builder);
                                p += 12;
                                break;
                            }
                        }

                        // single \uXXXX
                        encode_utf8_sb(code1, &builder);
                        p += 6;
                        break;
                    }
                    // invalid escape, just copy literally
                    write_char_string_builder(&builder, *p++);
                    break;
                default:
                    // unknown escape, copy literally '\' and next char
                    write_char_string_builder(&builder, *p++);
                    if (*p) write_char_string_builder(&builder, *p++);
                    break;
            }
        } else {
            write_char_string_builder(&builder, *p++);
        }
    }

    return collect_string_builder_no_copy(&builder);
}


#endif //FLUENT_LIBC_STR_CONV_LIBRARY_H