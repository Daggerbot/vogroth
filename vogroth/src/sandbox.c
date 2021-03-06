/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string.h>

#include "gl_api.h"
#include "gl_shaders.h"
#include "gl_state.h"
#include "pixbuf.h"
#include "render.h"
#include "texture.h"

static struct texture *white_texture = NULL;

void sandbox_init(void)
{
    struct pixbuf pixbuf = PIXBUF_INIT;

    /* Create a white texture */
    pixbuf.size = (struct vec2i){1, 1};
    pixbuf.format = PIXEL_FORMAT_RGBA_8888;
    pixbuf_alloc(&pixbuf);
    memset(pixbuf.buf, -1, pixbuf.buf_size);
    white_texture = texture_create((struct vec2i){1, 1}, PIXEL_FORMAT_RGBA_8888);
    texture_upload(white_texture, &pixbuf, (struct vec2i){0, 0});
    pixbuf_fini(&pixbuf);
}

void sandbox_fini(void)
{
    texture_destroy(white_texture);
    white_texture = NULL;
}

void sandbox_render(void)
{
    static const struct sprite_vertex triangle[3] = {
        {{100, 100}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{100, 200}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{200, 100}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    };

    pglClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    pglClear(GL_COLOR_BUFFER_BIT);
    gl_use_program(&gl_program_rgb_sprites);
    render_use_ui_transform();
    render_use_texture(white_texture);
    gl_use_sprite_vertex_ptr(triangle);
    pglDrawArrays(GL_TRIANGLES, 0, 3);
}
