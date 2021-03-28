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

#ifndef INCLUDED_TEXTURE_H
#define INCLUDED_TEXTURE_H

#include "pixbuf.h"

struct texture;

struct texture *texture_create(struct vec2i size, enum pixel_format format);
void texture_destroy(struct texture *texture);
void texture_upload(struct texture *texture, const struct pixbuf *src, struct vec2i offset);

#endif /* INCLUDED_TEXTURE_H */
