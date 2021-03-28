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

#ifndef INCLUDED_SPRITES_H
#define INCLUDED_SPRITES_H

#include "types.h"

struct sprite_batch;

struct sprite_batch *sprite_batch_create(void);
void sprite_batch_destroy(struct sprite_batch *batch);
void sprite_batch_resize(struct sprite_batch *batch, int num_sprites);
/* Returns the index of the first appended sprite. */
int sprite_batch_append(struct sprite_batch *batch, int num_sprites);

void sprite_batch_put(struct sprite_batch *batch, int index, struct rect2i src_rect,
                      struct vec2i pos, struct vec4f color);

#endif /* INCLUDED_SPRITES_H */
