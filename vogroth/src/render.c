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

void render_use_ui_transform(struct rect2i *out_bounds)
{
    struct vec2i surface_size = video_get_surface_size();

    gl_use_transform(mat4f_ortho((struct vec3f) {0.0f, (float)surface_size.y, -1.0f},
                                 (struct vec3f) {(float)surface_size.x, 0.0f, 1.0f}));

    if (out_bounds) {
        *out_bounds = (struct rect2i) {{0, 0}, surface_size};
    }
}

void render_use_texture(struct texture *texture)
{
    if (texture == gl_state.texture) {
        return;
    }
    pglActiveTexture(GL_TEXTURE0 + RENDER_GL_TEXTURE_UNIT_TEXTURE);
    pglBindTexture(GL_TEXTURE_2D, texture ? texture->id : 0);
    gl_state.texture = texture;

    if (texture && gl_state.program && gl_state.program->uni_texture_size >= 0) {
        pglUniform2f(gl_state.program->uni_texture_size, (float)texture->size.x, (float)texture->size.y);
    }
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

void render_draw_texture(struct texture *texture, struct vec2i pos)
{
    struct sprite_vertex quad[4];

    if (!texture) {
        return;
    }

    quad[0] = (struct sprite_vertex) {
        .position = pos,
        .texture_coord = {0, 0},
        .color = {1.0f, 1.0f, 1.0f, 1.0f},
    };
    quad[1] = (struct sprite_vertex) {
        .position = {pos.x, pos.y + texture->size.y},
        .texture_coord = {0, texture->size.y},
        .color = {1.0f, 1.0f, 1.0f, 1.0f},
    };
    quad[2] = (struct sprite_vertex) {
        .position = {pos.x + texture->size.x, pos.y + texture->size.y},
        .texture_coord = texture->size,
        .color = {1.0f, 1.0f, 1.0f, 1.0f},
    };
    quad[3] = (struct sprite_vertex) {
        .position = {pos.x + texture->size.x, pos.y},
        .texture_coord = {texture->size.x, 0},
        .color = {1.0f, 1.0f, 1.0f, 1.0f},
    };

    switch (texture->format) {
    case PIXEL_FORMAT_RGB_888:
        gl_use_program(&gl_program_rgb_sprites);
        break;
    case PIXEL_FORMAT_RGBA_8888:
        gl_use_program(&gl_program_rgba_sprites);
        break;
    default:
        FATAL("Unimplemented texture format");
    }

    render_use_texture(texture);
    gl_use_sprite_vertex_ptr(quad);
    pglDrawArrays(GL_QUADS, 0, 4);
}
