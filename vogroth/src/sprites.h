/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
