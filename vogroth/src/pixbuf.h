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

#ifndef INCLUDED_PIXBUF_H
#define INCLUDED_PIXBUF_H

#include "types.h"

enum pixel_format {
    PIXEL_FORMAT_NONE = 0,
    PIXEL_FORMAT_RED_8 = 1,
    PIXEL_FORMAT_RG_88 = 2,
    PIXEL_FORMAT_RGB_888 = 3,
    PIXEL_FORMAT_RGBA_8888 = 4,
    PIXEL_FORMAT_ALPHA_8 = 0x1906,
    PIXEL_FORMAT_LUMINANCE_8 = 0x1909,
    PIXEL_FORMAT_LUMINANCE_ALPHA_88 = 0x190A,
};

struct pixbuf {
    struct vec2i size;
    enum pixel_format format;
    int row_pitch;
    size_t buf_size;
    uint8_t *buf;
};
#define PIXBUF_INIT {0}
#define PIXBUF_NULL ((struct pixbuf)PIXBUF_INIT)

int pixbuf_get_bytes_per_pixel(enum pixel_format format);
/* Gets the most convenient pitch for use with the underlying render API. */
int pixbuf_get_ideal_row_pitch(enum pixel_format format, int width);

/*
 * Sets buf_size and reallocates buf. size and format must be set. row_pitch
 * must be valid or 0.
 */
void pixbuf_alloc(struct pixbuf *pixbuf);
void pixbuf_fini(struct pixbuf *pixbuf);

bool pixbuf_is_ideal(const struct pixbuf *pixbuf);

#endif /* INCLUDED_PIXBUF_H */
