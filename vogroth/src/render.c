/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "debug.h"
#include "gl_api.h"
#include "gl_shaders.h"
#include "gl_state.h"
#include "render.h"
#include "vector_math.h"
#include "video.h"

void render_init(void)
{
    gl_init_api();
    gl_init_shaders();
}

void render_fini(void)
{
    gl_fini_shaders();
    gl_fini_api();
    gl_state = RENDER_GL_STATE_NULL;
}

void render_begin_frame(void)
{
    struct vec2i surface_size = video_get_surface_size();

    pglViewport(0, 0, surface_size.x, surface_size.y);
}

void render_end_frame(void)
{
    gl_flush_errors();
}

void render_use_ui_transform(void)
{
    struct vec2i surface_size = video_get_surface_size();

    gl_use_transform(mat4f_ortho((struct vec3f) {0.0f, (float)surface_size.y, -1.0f},
                                 (struct vec3f) {(float)surface_size.x, 0.0f, 1.0f}));
}

void render_use_texture(struct texture *texture)
{
    if (texture == gl_state.texture) {
        return;
    }
    pglActiveTexture(GL_TEXTURE0 + RENDER_GL_TEXTURE_UNIT_TEXTURE);
    pglBindTexture(GL_TEXTURE_2D, texture ? texture->id : 0);
    gl_state.texture = texture;
}

void render_begin_sprites(struct sprite_batch *batch, enum sprite_mode mode)
{
    DASSERT(batch && batch->num_sprites && !gl_state.sprite_batch);

    switch (mode) {
    case SPRITE_MODE_MASK:
        gl_use_program(&gl_program_alpha_sprites);
        break;
    case SPRITE_MODE_RGB:
        gl_use_program(&gl_program_rgb_sprites);
        break;
    case SPRITE_MODE_RGB_MASK:
        gl_use_program(&gl_program_rgba_sprites);
        break;
    default:
        FATAL("Invalid sprite_mode");
    }

    gl_use_sprite_vertex_ptr(batch->verts);
    gl_state.sprite_batch = batch;
}

void render_draw_sprites(int first, int count)
{
    if (!count) {
        return;
    }
    DASSERT(gl_state.sprite_batch != NULL);
    DASSERT(first >= 0 && first <= gl_state.sprite_batch->num_sprites);
    DASSERT(count >= 0 && count <= gl_state.sprite_batch->num_sprites - first);
    pglDrawArrays(GL_QUADS, first * RENDER_VERTS_PER_SPRITE, count * RENDER_VERTS_PER_SPRITE);
}

void render_end_sprites(void)
{
    DASSERT(gl_state.sprite_batch != NULL);
    gl_state.sprite_batch = NULL;
}

void render_draw_sprites_now(struct sprite_batch *batch, enum sprite_mode mode,
                             int first, int count)
{
    render_begin_sprites(batch, mode);
    render_draw_sprites(first, count);
    render_end_sprites();
}
