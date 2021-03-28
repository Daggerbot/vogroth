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

#ifndef INCLUDED_GL_API_H
#define INCLUDED_GL_API_H

#ifdef _WIN32
# include <wtypes.h> /* for APIENTRY */
#endif

#ifdef __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glext.h>
#else
# include <GL/gl.h>
# include <GL/glext.h>
#endif

#ifndef GLAPIENTRY
# ifdef APIENTRY
#  define GLAPIENTRY APIENTRY
# else
#  define GLAPIENTRY
# endif
#endif

#include "gl_types.h"

/* Minimum supported OpenGL version */
#define RENDER_GL_MAJOR_VERSION 2
#define RENDER_GL_MINOR_VERSION 0

/* Texture unit indices */
#define RENDER_GL_TEXTURE_UNIT_MANAGER 0
#define RENDER_GL_TEXTURE_UNIT_TEXTURE 1

void gl_init_api(void);
void gl_fini_api(void);

/*
 * Gets a string describing an OpenGL error code. Subsequent calls may
 * invalidate the previously returned string. Not thread-safe. This is okay as
 * long as we only use OpenGL from one thread.
 */
const char *gl_strerror(GLenum errcode);

/*
 * Reports and clears any set OpenGL error flags.
 */
void gl_flush_errors(void);

/*
 * Macro which evaluates another macro x for each OpenGL function that we're
 * using.
 */
#define FOREACH_GL_FUNCTION(x) \
    x(void, ActiveTexture, GLenum) \
    x(void, AttachShader, GLuint, GLuint) \
    x(void, BindTexture, GLenum, GLuint) \
    x(void, Clear, GLbitfield) \
    x(void, ClearColor, GLclampf, GLclampf, GLclampf, GLclampf) \
    x(void, CompileShader, GLuint) \
    x(GLuint, CreateProgram, void) \
    x(GLuint, CreateShader, GLenum) \
    x(void, DeleteProgram, GLuint) \
    x(void, DeleteShader, GLuint) \
    x(void, DeleteTextures, GLsizei, const GLuint *) \
    x(void, Disable, GLenum) \
    x(void, DisableVertexAttribArray, GLuint) \
    x(void, DrawArrays, GLenum, GLint, GLsizei) \
    x(void, Enable, GLenum) \
    x(void, EnableVertexAttribArray, GLuint) \
    x(void, GenTextures, GLsizei, GLuint *) \
    x(GLint, GetAttribLocation, GLuint, const GLchar *) \
    x(GLenum, GetError, void) \
    x(void, GetProgramiv, GLuint, GLenum, GLint *) \
    x(void, GetProgramInfoLog, GLuint, GLsizei, GLsizei *, GLchar *) \
    x(void, GetShaderiv, GLuint, GLenum, GLint *) \
    x(void, GetShaderInfoLog, GLuint, GLsizei, GLsizei *, GLchar *) \
    x(const GLchar *, GetString, GLenum) \
    x(GLint, GetUniformLocation, GLuint, const GLchar *) \
    x(void, LinkProgram, GLuint) \
    x(void, ShaderSource, GLuint, GLsizei, const GLchar **, const GLint *) \
    x(void, TexImage2D, GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *) \
    x(void, TexParameteri, GLenum, GLenum, GLint) \
    x(void, TexSubImage2D, GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) \
    x(void, Uniform1i, GLint, GLint) \
    x(void, Uniform2f, GLint, GLfloat, GLfloat) \
    x(void, UniformMatrix4fv, GLint, GLsizei, GLboolean, const GLfloat *) \
    x(void, UseProgram, GLuint) \
    x(void, VertexAttribPointer, GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *) \
    x(void, Viewport, GLint, GLint, GLsizei, GLsizei)

/*
 * Declare the above API functions as function pointers with the 'pgl' prefix
 * instead of 'gl' to avoid name conflicts.
 */
#define DO(RETURN, NAME, ...) extern RETURN(GLAPIENTRY *pgl##NAME)(__VA_ARGS__);
FOREACH_GL_FUNCTION(DO)
#undef DO

#endif /* INCLUDED_GL_API_H */
