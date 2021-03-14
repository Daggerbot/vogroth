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
#include "tiles.h"

static struct tileset *tileset = NULL;

void sandbox_init(void)
{
    tileset = tileset_init();
}

void sandbox_fini(void)
{
    tileset_fini();
    tileset = NULL;
}

void sandbox_render(void)
{
    pglClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    pglClear(GL_COLOR_BUFFER_BIT);
    render_use_ui_transform();
    render_draw_texture(tileset->texture, (struct vec2i){0, 0});
}
