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
#include "sprites.h"
#include "texture.h"

static struct texture *texture = NULL;
static struct sprite_batch *sprite_batch = NULL;

void sandbox_init(void)
{
    struct pixbuf pixbuf = PIXBUF_INIT;

    /* Create a white texture */
    pixbuf.size = (struct vec2i){32, 32};
    pixbuf.format = PIXEL_FORMAT_RGBA_8888;
    pixbuf_alloc(&pixbuf);
    memset(pixbuf.buf, -1, pixbuf.buf_size);
    texture = texture_create((struct vec2i){32, 32}, PIXEL_FORMAT_RGBA_8888);
    texture_upload(texture, &pixbuf, (struct vec2i){0, 0});
    pixbuf_fini(&pixbuf);

    /* Create a sprite batch */
    sprite_batch = sprite_batch_create();
    sprite_batch_resize(sprite_batch, 3);
    sprite_batch_put(sprite_batch, 0, (struct rect2i){{0, 0}, {32, 32}}, (struct vec2i){0, 0}, (struct vec4f){1.0f, 0.0f, 0.0f, 1.0f});
    sprite_batch_put(sprite_batch, 1, (struct rect2i){{0, 0}, {32, 32}}, (struct vec2i){32, 0}, (struct vec4f){0.0f, 1.0f, 0.0f, 1.0f});
    sprite_batch_put(sprite_batch, 2, (struct rect2i){{0, 0}, {32, 32}}, (struct vec2i){64, 0}, (struct vec4f){0.0f, 0.0f, 1.0f, 1.0f});
}

void sandbox_fini(void)
{
    sprite_batch_destroy(sprite_batch);
    sprite_batch = NULL;
    texture_destroy(texture);
    texture = NULL;
}

void sandbox_render(void)
{
    pglClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    pglClear(GL_COLOR_BUFFER_BIT);
    gl_use_program(&gl_program_rgb_sprites);
    render_use_ui_transform();
    render_use_texture(texture);
    render_draw_sprites_now(sprite_batch, SPRITE_MODE_RGB, 0, 3);
}
