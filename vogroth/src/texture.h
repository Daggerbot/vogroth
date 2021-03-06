/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_TEXTURE_H
#define INCLUDED_TEXTURE_H

#include "pixbuf.h"

struct texture;

struct texture *texture_create(struct vec2i size, enum pixel_format format);
void texture_destroy(struct texture *texture);
void texture_upload(struct texture *texture, const struct pixbuf *src, struct vec2i offset);

#endif /* INCLUDED_TEXTURE_H */
