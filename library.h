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

#endif //FLUENT_LIBC_STR_CONV_LIBRARY_H