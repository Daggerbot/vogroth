/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdlib.h>
#include <string.h>

#if defined(__GLIBC__) || defined(_WIN32)
# include <malloc.h>
#endif

#include "debug.h"
#include "memory.h"
#include "system.h"

static NORETURN void alloc_failed(void)
{
    system_show_error_native(OSSTR "Allocation failed");
    abort();
}

void *mem_alloc(size_t size)
{
    return mem_realloc(NULL, size);
}

void *mem_alloc_array(size_t n, size_t size)
{
    return mem_realloc_array(NULL, n, size);
}

void *mem_realloc(void *mem, size_t size)
{
    if (size) {
        if (mem) {
            mem = realloc(mem, size);
        } else {
            mem = malloc(size);
        }
        if (!mem) {
            alloc_failed();
        }
        return mem;
    } else {
        return mem_free(mem);
    }
}

void *mem_realloc_array(void *mem, size_t n, size_t size)
{
    if (size && n > SIZE_MAX / size) { /* overflow check (n * size) */
        alloc_failed();
    }
    return mem_realloc(mem, n * size);
}

void *mem_free(void *mem)
{
    if (mem) {
        free(mem);
    }
    return NULL;
}

void buf_alloc(struct buf *buf, size_t size)
{
    DASSERT(buf != NULL);

    if (buf->alloc_size >= size) {
        return;
    }
    buf->data = mem_realloc(buf->data, size);

    /* Get the actual allocation size if possible. */
#ifdef __GLIBC__
    buf->alloc_size = malloc_usable_size(buf->data);
#elif defined(_WIN32)
    buf->alloc_size = _msize(buf->data);
#else
    buf->alloc_size = size;
#endif
}

void buf_fini(struct buf *buf)
{
    DASSERT(buf != NULL);
    mem_free(buf->data);
    *buf = BUF_NULL;
}

void buf_terminate(struct buf *buf)
{
    DASSERT(buf != NULL);
    buf_alloc(buf, buf->len + 1);
    buf->data[buf->len] = 0;
}

void buf_append(struct buf *buf, size_t size, const void *data)
{
    if (!size) {
        return;
    }
    DASSERT(buf && data);
    DASSERT(size <= SIZE_MAX - buf->len);
    buf_alloc(buf, buf->len + size);
    memcpy(buf->data + buf->len, data, size);
    buf->len += size;
}

void buf_appends(struct buf *buf, const char *src)
{
    size_t cat_len;

    DASSERT(buf != NULL);
    if (!src) {
        return;
    }
    cat_len = strlen(src);
    buf_alloc(buf, buf->len + cat_len + 1);
    memcpy(buf->data + buf->len, src, cat_len);
    buf->len += cat_len;
    buf->data[buf->len] = 0;
}

void buf_appendf(struct buf *buf, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    buf_vappendf(buf, fmt, args);
    va_end(args);
}

void buf_vappendf(struct buf *buf, const char *fmt, va_list args)
{
    va_list tmp_args;
    int result;
    size_t cat_len;

    DASSERT(buf != NULL);
    if (!fmt) {
        buf_terminate(buf);
        return;
    }

    /* Pass 1: Determine the length of the formatted string. */
    va_copy(tmp_args, args);
    result = vsnprintf(NULL, 0, fmt, tmp_args);
    va_end(tmp_args);
    if (result < 0) {
        buf_appends(buf, fmt);
        return;
    } else if (!result) {
        buf_terminate(buf);
        return;
    }
    cat_len = (size_t)result;

    /* Pass 2: Append formatted text to the buffer. */
    buf_alloc(buf, buf->len + cat_len + 1);
    result = vsnprintf(buf->data + buf->len, cat_len + 1, fmt, args);
    if (result < 0) {
        buf_appends(buf, fmt);
    }
}

char *str_clone(const char *src)
{
    if (src) {
        return strcpy(mem_alloc(strlen(src) + 1), src);
    } else {
        return NULL;
    }
}

char *str_printf(const char *fmt, ...)
{
    va_list args;
    char *str;

    va_start(args, fmt);
    str = str_vprintf(fmt, args);
    va_end(args);
    return str;
}

char *str_vprintf(const char *fmt, va_list args)
{
    return str_vappendf(NULL, fmt, args);
}

char *str_append(char *str, const char *src)
{
    size_t old_len;
    size_t cat_len;

    if (!src || !src[0]) {
        return str;
    }
    old_len = str ? strlen(str) : 0;
    cat_len = strlen(src);
    str = mem_realloc(str, old_len + cat_len + 1);
    memcpy(str + old_len, src, cat_len + 1);
    return str;
}

char *str_appendf(char *str, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    str = str_vappendf(str, fmt, args);
    va_end(args);
    return str;
}

char *str_vappendf(char *str, const char *fmt, va_list args)
{
    va_list tmp_args;
    int result;
    size_t old_len;
    size_t cat_len;

    /* Pass 1: Determine the length of the formatted string. */
    if (!fmt || !fmt[0]) {
        return str;
    }
    va_copy(tmp_args, args);
    result = vsnprintf(NULL, 0, fmt, tmp_args);
    va_end(tmp_args);
    if (result < 0) {
        return str_append(str, fmt);
    } else if (!result) {
        return str;
    }

    /* Pass 2: Append formatted text to the string. */
    old_len = str ? strlen(str) : 0;
    cat_len = (size_t)result;
    str = mem_realloc(str, old_len + cat_len + 1);
    result = vsnprintf(str + old_len, cat_len + 1, fmt, args);
    if (result < 0) {
        str[old_len] = 0;
        return str_append(str, fmt);
    }
    return str;
}

char *str_assign(char *str, const char *src)
{
    if (str) {
        str[0] = 0;
    }
    return str_append(str, src);
}

char *str_assignf(char *str, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    str = str_vassignf(str, fmt, args);
    va_end(args);
    return str;
}

char *str_vassignf(char *str, const char *fmt, va_list args)
{
    if (str) {
        str[0] = 0;
    }
    return str_vappendf(str, fmt, args);
}

void str_put(char **out_str, const char *src)
{
    if (out_str) {
        *out_str = str_assign(*out_str, src);
    }
}

void str_putf(char **out_str, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    str_vputf(out_str, fmt, args);
    va_end(args);
}

void str_vputf(char **out_str, const char *fmt, va_list args)
{
    if (out_str) {
        *out_str = str_vassignf(*out_str, fmt, args);
    }
}
