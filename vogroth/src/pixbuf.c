/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "debug.h"
#include "memory.h"
#include "pixbuf.h"

#define ROW_ALIGN 4

int pixbuf_get_bytes_per_pixel(enum pixel_format format)
{
    switch (format) {
    case PIXEL_FORMAT_RED_8:
    case PIXEL_FORMAT_ALPHA_8:
    case PIXEL_FORMAT_LUMINANCE_8:
        return 1;
    case PIXEL_FORMAT_RG_88:
    case PIXEL_FORMAT_LUMINANCE_ALPHA_88:
        return 2;
    case PIXEL_FORMAT_RGB_888:
        return 3;
    case PIXEL_FORMAT_RGBA_8888:
        return 4;
    default:
        FATAL("Invalid pixel format 0x%04" PRIu32, (uint32_t)format);
    }
}

static int get_bytes_per_row(enum pixel_format format, int width)
{
    int bytes_per_pixel = pixbuf_get_bytes_per_pixel(format);

    ASSERT(width > 0 && width <= INT_MAX / bytes_per_pixel);
    return bytes_per_pixel * width;
}

int pixbuf_get_ideal_row_pitch(enum pixel_format format, int width)
{
    int bytes_per_row = get_bytes_per_row(format, width);
    int pad;

    pad = (ROW_ALIGN - bytes_per_row % ROW_ALIGN) % ROW_ALIGN;
    ASSERT(pad <= INT_MAX - bytes_per_row);
    return bytes_per_row + pad;
}

void pixbuf_alloc(struct pixbuf *pixbuf)
{
    DASSERT(pixbuf != NULL);
    ASSERT(pixbuf->size.x > 0 && pixbuf->size.y > 0);

    if (pixbuf->row_pitch > 0) {
        ASSERT(pixbuf->row_pitch >= get_bytes_per_row(pixbuf->format, pixbuf->size.x));
    } else if (pixbuf->row_pitch < 0) {
        ASSERT(pixbuf->row_pitch <= -get_bytes_per_row(pixbuf->format, pixbuf->size.x));
    } else {
        pixbuf->row_pitch = pixbuf_get_ideal_row_pitch(pixbuf->format, pixbuf->size.x);
    }

    if (pixbuf->row_pitch > 0) {
        ASSERT((size_t)pixbuf->size.y <= SIZE_MAX / (size_t)pixbuf->row_pitch);
        pixbuf->buf_size = (size_t)pixbuf->row_pitch * (size_t)pixbuf->size.y;
    } else {
        ASSERT(pixbuf->row_pitch >= -INT_MAX);
        ASSERT((size_t)pixbuf->size.y <= SIZE_MAX / (size_t)-pixbuf->row_pitch);
        pixbuf->buf_size = (size_t)-pixbuf->row_pitch * (size_t)pixbuf->size.y;
    }

    pixbuf->buf = mem_realloc(pixbuf->buf, pixbuf->buf_size);
}

void pixbuf_fini(struct pixbuf *pixbuf)
{
    if (pixbuf) {
        mem_free(pixbuf->buf);
        *pixbuf = PIXBUF_NULL;
    }
}

bool pixbuf_is_ideal(const struct pixbuf *pixbuf)
{
    DASSERT(pixbuf != NULL);
    return pixbuf->buf
           && pixbuf->row_pitch == pixbuf_get_ideal_row_pitch(pixbuf->format, pixbuf->size.x)
           && (uintptr_t)pixbuf->buf % ROW_ALIGN == 0;
}
