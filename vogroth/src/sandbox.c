/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "gl_api.h"
#include "gl_shaders.h"
#include "gl_state.h"
#include "render.h"

void sandbox_init(void)
{
}

void sandbox_fini(void)
{
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
    render_use_ui_transform();
    gl_use_program(&gl_program_rgb_sprites);
    gl_use_sprite_vertex_ptr(triangle);
    pglDrawArrays(GL_TRIANGLES, 0, 3);
}
