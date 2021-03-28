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
