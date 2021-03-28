/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_UNICODE_H
#define INCLUDED_UNICODE_H

#include "memory.h"

#define UTF16_SURROGATE_MIN 0xD800
#define UTF16_SURROGATE_MAX 0xDFFF
#define REPLACEMENT_CHAR 0xFFFD
#define UNICODE_MAX 0x10FFFF

/*
 * Converts a UTF-8 string to UTF-16/32 (depending on the size of wchar_t).
 * Invalid sequences are replaced with REPLACEMENT_CHAR. If len is -1, the
 * input string is assumed to be null terminated. Use mem_free to free the
 * output string.
 */
wchar_t *utf8_to_wide(int len, const char *str, int *out_len);

/*
 * Converts a UTF-16/32 (depending on the size of wchar_t) string to UTF-8.
 * Invalid sequences are replaced with REPLACEMENT_CHAR. If len is -1, the
 * input string is assumed to be null terminated. Use mem_free to free the
 * output string.
 */
char *wide_to_utf8(int len, const wchar_t *str, int *out_len);

#endif /* INCLUDED_UNICODE_H */
