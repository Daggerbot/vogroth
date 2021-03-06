/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
