/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include <wchar.h>

#include "unicode.h"

/*
 * Decodes a UTF-8 sequence at the front of the input string.
 */
static uint32_t decode_utf8(int len, const char str[static len], int out_seq_len[static 1])
{
    STATIC_ASSERT(sizeof(uint8_t) == sizeof(char));

    uint8_t unit;
    uint32_t value;
    int i;

    if (len <= 0 || !str) {
        *out_seq_len = 0;
        return 0;
    }

    unit = ((const uint8_t *)str)[0];

    if (!(unit & 0x80)) {
        /* 0xxxxxxx (ASCII) */
        *out_seq_len = 1;
        return unit;
    } else if ((unit & 0xE0) == 0xC0) {
        /* 110xxxxx 10xxxxxx */
        value = unit & 0x1F;
        *out_seq_len = 2;
    } else if ((unit & 0xF0) == 0xE0) {
        /* 1110xxxx 10xxxxxx 10xxxxxx */
        value = unit & 0x0F;
        *out_seq_len = 3;
    } else if ((unit & 0xF8) == 0xF0) {
        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        value = unit & 0x07;
        *out_seq_len = 4;
    } else if ((unit & 0xC0) == 0x80) {
        /* Input string starts on a UTF-8 tail byte */
        *out_seq_len = 1;
        while (*out_seq_len < len) {
            unit = ((const uint8_t *)str)[*out_seq_len];
            if ((unit & 0xC0) != 0x80) {
                break;
            }
            ++*out_seq_len;
        }
        return REPLACEMENT_CHAR;
    } else {
        /* Invalid UTF-8 byte */
        *out_seq_len = 1;
        return REPLACEMENT_CHAR;
    }

    for (i = 1; i < *out_seq_len; ++i) {
        if (i == len) {
            /* Incomplete sequence */
            *out_seq_len = i;
            return REPLACEMENT_CHAR;
        }

        unit = ((const uint8_t *)str)[i];
        if ((unit & 0xC0) != 0x80) {
            /* Sequence not understood */
            *out_seq_len = i;
            return REPLACEMENT_CHAR;
        }

        value = (value << 6) | (unit & 0x3F);
    }

    if (value > UNICODE_MAX) {
        /* Code point out of range */
        return REPLACEMENT_CHAR;
    } else if (value >= UTF16_SURROGATE_MIN && value <= UTF16_SURROGATE_MAX) {
        /* Code point reserved */
        return REPLACEMENT_CHAR;
    }

    return value;
}

/*
 * Decodes a UTF-16 sequence at the front of the input string.
 */
static uint32_t decode_utf16(int len, const uint16_t str[static len], int out_seq_len[static 1])
{
    uint16_t unit;
    uint32_t value;

    if (len <= 0 || !str) {
        *out_seq_len = 0;
        return 0;
    }

    unit = str[0];

    if ((unit & 0xF800) != 0xD800) {
        /* Single character sequence */
        *out_seq_len = 1;
        return unit;
    } else if (!(unit & 0x0400)) {
        /* Surrogate pair sequence */
        if (len < 2) {
            /* Incomplete sequence */
            *out_seq_len = len;
            return REPLACEMENT_CHAR;
        }

        value = ((uint32_t)(unit & 0x03FF) << 10) + 0x10000;
        unit = str[1];
        if ((unit & 0xFC00) != 0xDC00) {
            /* Sequence not understood */
            *out_seq_len = 1;
            return REPLACEMENT_CHAR;
        }

        value |= unit & 0x03FF;
        *out_seq_len = 2;
        return value;
    } else {
        /* Input string starts on the second half of a surrogate pair */
        *out_seq_len = 1;
        return REPLACEMENT_CHAR;
    }
}

/*
 * Encodes a UTF-8 sequence and returns its length.
 */
static int encode_utf8(uint32_t ch, char out_seq[static 4])
{
    uint8_t *seq = (uint8_t *)&out_seq[0];

    if (ch <= 0x7F) {
        /* 0xxxxxxx (ASCII) */
        seq[0] = (uint8_t)ch;
        return 1;
    } else if (ch <= 0x7FF) {
        /* 110xxxxx 10xxxxxx */
        seq[0] = (uint8_t)(0xC0 | (ch >> 6));
        seq[1] = (uint8_t)(0x80 | (ch & 0x3F));
        return 2;
    } else if (ch <= 0xFFFF) {
        /* 1110xxxx 10xxxxxx 10xxxxxx */
        if (ch >= UTF16_SURROGATE_MIN && ch <= UTF16_SURROGATE_MAX) {
            /* Code point reserved */
            return encode_utf8(REPLACEMENT_CHAR, out_seq);
        }
        seq[0] = (uint8_t)(0xE0 | (ch >> 12));
        seq[1] = (uint8_t)(0x80 | ((ch >> 6) & 0x3F));
        seq[2] = (uint8_t)(0x80 | (ch & 0x3F));
        return 3;
    } else if (ch <= UNICODE_MAX) {
        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        seq[0] = (uint8_t)(0xF0 | (ch >> 18));
        seq[1] = (uint8_t)(0x80 | ((ch >> 12) & 0x3F));
        seq[2] = (uint8_t)(0x80 | ((ch >> 6) & 0x3F));
        seq[3] = (uint8_t)(0x80 | (ch & 0x3F));
        return 4;
    } else {
        /* Code point out of range */
        return encode_utf8(REPLACEMENT_CHAR, out_seq);
    }
}

/*
 * Encodes a UTF-16 sequence and returns its length.
 */
static int encode_utf16(uint32_t ch, uint16_t out_seq[static 2])
{
    if (ch <= 0xFFFF) {
        /* Single character sequence */
        out_seq[0] = (uint16_t)ch;
        return 1;
    } else if (ch <= UNICODE_MAX) {
        /* Surrogate pair sequence */
        out_seq[0] = (uint16_t)(0xD800 | ((ch - 0x10000) >> 10));
        out_seq[1] = (uint16_t)(0xDC00 | (ch & 0x03FF));
        return 2;
    } else {
        /* Code point out of range */
        return encode_utf16(REPLACEMENT_CHAR, out_seq);
    }
}

wchar_t *utf8_to_wide(int len, const char *str, int *out_len_ptr)
{
    STATIC_ASSERT(sizeof(wchar_t) == sizeof(uint16_t) || sizeof(wchar_t) == sizeof(uint32_t));

    int out_len = 0;
    const char *in_pos;
    int in_remaining;
    uint32_t ch;
    int seq_len;
    wchar_t discard[2];
    wchar_t *out_str;
    wchar_t *out_pos;

    if (!len || !str) {
        if (out_len_ptr) {
            *out_len_ptr = 0;
        }
        return NULL;
    } else if (len < 0) {
        len = (int)strlen(str);
    }

    /* Pass 1: Determine the length of the encoded string */
    in_pos = str;
    in_remaining = len;

    while (in_remaining) {
        ch = decode_utf8(in_remaining, in_pos, &seq_len);
        in_pos += seq_len;
        in_remaining -= seq_len;

        if (sizeof(wchar_t) >= sizeof(uint32_t)) {
            ++out_len;
        } else {
            out_len += encode_utf16(ch, (uint16_t *)&discard[0]);
        }
    }

    /* Pass 2: Encode the output string */
    in_pos = str;
    in_remaining = len;
    out_str = mem_alloc_array((size_t)out_len + 1, sizeof(*out_str));
    out_pos = out_str;

    while (in_remaining) {
        ch = decode_utf8(in_remaining, in_pos, &seq_len);
        in_pos += seq_len;
        in_remaining -= seq_len;

        if (sizeof(wchar_t) >= sizeof(uint32_t)) {
            *out_pos++ = (wchar_t)ch;
        } else {
            out_pos += encode_utf16(ch, (uint16_t *)out_pos);
        }
    }

    *out_pos = 0;
    if (out_len_ptr) {
        *out_len_ptr = out_len;
    }
    return out_str;
}

char *wide_to_utf8(int len, const wchar_t *str, int *out_len_ptr)
{
    STATIC_ASSERT(sizeof(wchar_t) == sizeof(uint16_t) || sizeof(wchar_t) == sizeof(uint32_t));

    int out_len = 0;
    const wchar_t *in_pos;
    int in_remaining;
    uint32_t ch;
    int seq_len;
    char discard[4];
    char *out_str;
    char *out_pos;

    if (!len || !str) {
        if (out_len_ptr) {
            *out_len_ptr = 0;
        }
        return NULL;
    } else if (len < 0) {
        len = (int)wcslen(str);
    }

    /* Pass 1: Determine the length of the encoded string */
    in_pos = str;
    in_remaining = len;

    while (in_remaining) {
        if (sizeof(wchar_t) >= sizeof(uint32_t)) {
            ch = *in_pos++;
            --in_remaining;
        } else {
            ch = decode_utf16(in_remaining, in_pos, &seq_len);
            in_pos += seq_len;
            in_remaining -= seq_len;
        }

        out_len += encode_utf8(ch, discard);
    }

    /* Pass 2: Encode the output string */
    in_pos = str;
    in_remaining = len;
    out_str = mem_alloc((size_t)out_len + 1);
    out_pos = out_str;

    while (in_remaining) {
        if (sizeof(wchar_t) >= sizeof(uint32_t)) {
            ch = *in_pos++;
            --in_remaining;
        } else {
            ch = decode_utf16(in_remaining, in_pos, &seq_len);
            in_pos += seq_len;
            in_remaining -= seq_len;
        }

        out_pos += encode_utf8(ch, out_pos);
    }

    *out_pos = 0;
    if (out_len_ptr) {
        *out_len_ptr = out_len;
    }
    return out_str;
}
