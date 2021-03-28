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

#ifndef INCLUDED_GL_SHADERS_H
#define INCLUDED_GL_SHADERS_H

#include "gl_types.h"

struct gl_program {
    GLuint id;

    /* Uniform indices */
    GLint uni_transform;
    GLint uni_texture;
    GLint uni_texture_size;

    /* Vertex attribute indices */
    GLint attr_position;
    GLint attr_texture_coord;
    GLint attr_color;
};
#define RENDER_GL_PROGRAM_INIT \
    { \
        .uni_transform = -1, \
        .uni_texture = -1, \
        .uni_texture_size = -1, \
        .attr_position = -1, \
        .attr_texture_coord = -1, \
        .attr_color = -1, \
    }
#define RENDER_GL_PROGRAM_NULL ((struct gl_program)RENDER_GL_PROGRAM_INIT)

void gl_init_shaders(void);
void gl_fini_shaders(void);
void gl_use_program(struct gl_program *program);

extern struct gl_program gl_program_alpha_sprites;
extern struct gl_program gl_program_rgb_sprites;
extern struct gl_program gl_program_rgba_sprites;

#endif /* INCLUDED_GL_SHADERS_H */
