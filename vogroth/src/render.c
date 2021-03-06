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
