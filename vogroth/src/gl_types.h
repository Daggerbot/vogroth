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

#ifndef INCLUDED_GL_TYPES_H
#define INCLUDED_GL_TYPES_H

#include "pixbuf.h"

/*
 * Define the OpenGL types without dragging in all of <GL/gl.h>. If any of
 * these are wrong, we'll get an error when pulling in the actual types.
 */
typedef unsigned int GLbitfield;
typedef unsigned char GLboolean;
typedef char GLchar;
typedef float GLclampf;
typedef double GLdouble;
typedef unsigned int GLenum;
typedef float GLfloat;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLuint;
typedef void GLvoid;

struct sprite_vertex {
    struct vec2i position;
    struct vec2i texture_coord;
    struct vec4f color;
};

#define RENDER_VERTS_PER_SPRITE 4
struct sprite_batch {
    int num_sprites;
    int num_verts;
    struct sprite_vertex *verts;
};

struct texture {
    GLuint id;
    struct vec2i size;
    enum pixel_format format;
};

#endif /* INCLUDED_GL_TYPES_H */
