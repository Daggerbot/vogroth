/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "gl_api.h"
#include "render.h"

void sandbox_init(void)
{
}

void sandbox_fini(void)
{
}

void sandbox_render(void)
{
    struct rect2i bounds;

    pglClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    pglClear(GL_COLOR_BUFFER_BIT);
    render_use_ui_transform(&bounds);
}
