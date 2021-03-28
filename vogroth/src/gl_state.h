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
    struct sprite_batch *sprite_batch;
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
