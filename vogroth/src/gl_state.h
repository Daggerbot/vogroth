/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_GL_STATE_H
#define INCLUDED_GL_STATE_H

#include "gl_types.h"
#include "vector_math.h"

typedef uint32_t gl_attrib_mask_t;

struct gl_state {
    struct gl_program *program;
    struct mat4f transform;
    struct texture *texture;
    gl_attrib_mask_t attrib_mask;
};
#define RENDER_GL_STATE_INIT \
    { \
        .transform = MAT4F_IDENTITY_INIT, \
    }
#define RENDER_GL_STATE_NULL ((struct gl_state)RENDER_GL_STATE_INIT)

void gl_use_transform(struct mat4f transform);
void gl_use_sprite_vertex_ptr(const struct sprite_vertex *ptr);

extern struct gl_state gl_state;

#endif /* INCLUDED_GL_STATE_H */
