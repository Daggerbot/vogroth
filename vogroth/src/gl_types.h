/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
    struct vec2f texture_coord;
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
