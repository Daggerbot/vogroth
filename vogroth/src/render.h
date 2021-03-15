/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_RENDER_H
#define INCLUDED_RENDER_H

#include "types.h"

struct sprite_batch;
struct texture;

enum sprite_mode {
    SPRITE_MODE_NONE,
    SPRITE_MODE_MASK,
    SPRITE_MODE_RGB,
    SPRITE_MODE_RGB_MASK,
};

void render_init(void);
void render_fini(void);
void render_begin_frame(void);
void render_end_frame(void);

/* Sets the transform to world coordinates = screen coordinates */
void render_use_ui_transform(struct rect2i *out_bounds);
void render_use_texture(struct texture *texture);

void render_begin_sprites(struct sprite_batch *batch, enum sprite_mode mode);
void render_draw_sprites(int first, int count);
void render_end_sprites(void);
/* Calls all 3 of the above functions */
void render_draw_sprites_now(struct sprite_batch *batch, enum sprite_mode mode,
                             int first, int count);

/* Functions for debugging purposes, not optimized */
void render_draw_texture(struct texture *texture, struct vec2i pos);

#endif /* INCLUDED_RENDER_H */
