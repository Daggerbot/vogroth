/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include "debug.h"
#include "gl_state.h"
#include "memory.h"
#include "sprites.h"
#include "texture.h"

struct sprite_batch *sprite_batch_create(void)
{
    struct sprite_batch *batch;

    batch = mem_alloc(sizeof(*batch));
    *batch = (struct sprite_batch){0};
    return batch;
}

void sprite_batch_destroy(struct sprite_batch *batch)
{
    DASSERT(batch != NULL);
    ASSERT(batch != gl_state.sprite_batch); /* Don't delete the active sprite batch */
    mem_free(batch->verts);
    mem_free(batch);
}

void sprite_batch_resize(struct sprite_batch *batch, int num_sprites)
{
    int old_size;

    DASSERT(batch != NULL);
    if (num_sprites == batch->num_sprites) {
        return;
    }
    ASSERT(batch != gl_state.sprite_batch); /* Don't resize the active sprite batch */
    ASSERT(num_sprites >= 0 && num_sprites <= INT_MAX / RENDER_VERTS_PER_SPRITE);
    old_size = batch->num_sprites;
    batch->num_sprites = num_sprites;
    batch->num_verts = num_sprites * RENDER_VERTS_PER_SPRITE;
    batch->verts = mem_realloc_array(batch->verts, (size_t)batch->num_verts, sizeof(*batch->verts));
    if (num_sprites > old_size) {
        memset(batch->verts + old_size * RENDER_VERTS_PER_SPRITE, 0,
               (size_t)(num_sprites - old_size) * sizeof(struct sprite_vertex) * RENDER_VERTS_PER_SPRITE);
    }
}

int sprite_batch_append(struct sprite_batch *batch, int num_sprites)
{
    int index;

    DASSERT(batch && num_sprites >= 0);
    ASSERT(num_sprites <= INT_MAX - batch->num_sprites);
    index = batch->num_sprites;
    sprite_batch_resize(batch, batch->num_sprites + num_sprites);
    return index;
}

void sprite_batch_put(struct sprite_batch *batch, int index, struct rect2i src_rect,
                      struct vec2i pos, struct vec4f color)
{
    struct sprite_vertex *verts;
    struct vec2i size = {src_rect.b.x - src_rect.a.x, src_rect.b.y - src_rect.a.y};

    DASSERT(batch && index >= 0 && index < batch->num_sprites);
    verts = batch->verts + index * RENDER_VERTS_PER_SPRITE;

    verts[0] = (struct sprite_vertex) {
        .position = {pos.x, pos.y},
        .texture_coord = {src_rect.a.x, src_rect.a.y},
        .color = color,
    };
    verts[1] = (struct sprite_vertex) {
        .position = {pos.x, pos.y + size.y},
        .texture_coord = {src_rect.a.x, src_rect.b.y},
        .color = color,
    };
    verts[2] = (struct sprite_vertex) {
        .position = {pos.x + size.x, pos.y + size.y},
        .texture_coord = {src_rect.b.x, src_rect.b.y},
        .color = color,
    };
    verts[3] = (struct sprite_vertex) {
        .position = {pos.x + size.x, pos.y},
        .texture_coord = {src_rect.b.x, src_rect.a.y},
        .color = color,
    };
}
