/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "debug.h"
#include "gl_api.h"
#include "gl_state.h"
#include "memory.h"
#include "pixbuf.h"
#include "texture.h"

static GLenum get_gl_pixel_format(enum pixel_format format)
{
    switch (format) {
    case PIXEL_FORMAT_RGB_888:
        return GL_RGB;
    case PIXEL_FORMAT_RGBA_8888:
        return GL_RGBA;
    case PIXEL_FORMAT_ALPHA_8:
        return GL_ALPHA; /* deprecated in OpenGL 3.0 */
    case PIXEL_FORMAT_LUMINANCE_8:
        return GL_LUMINANCE; /* deprecated in OpenGL 3.0 */
    case PIXEL_FORMAT_LUMINANCE_ALPHA_88:
        return GL_LUMINANCE_ALPHA; /* deprecated in OpenGL 3.0 */
    default:
        FATAL("Unsupported pixel format for OpenGL: 0x%04" PRIu32, (uint32_t)format);
    }
}

static GLenum get_gl_pixel_type(UNUSED enum pixel_format format)
{
    return GL_UNSIGNED_BYTE;
}

struct texture *texture_create(struct vec2i size, enum pixel_format format)
{
    struct texture *texture;
    GLenum gl_pixel_format = get_gl_pixel_format(format);
    GLenum gl_pixel_type = get_gl_pixel_type(format);
    GLenum errcode;

    gl_flush_errors();

    texture = mem_alloc(sizeof(*texture));
    *texture = (struct texture){0};

    pglGenTextures(1, &texture->id);
    if (!texture->id) {
        FATAL("glGenTextures: %s", gl_strerror(pglGetError()));
    }
    pglActiveTexture(GL_TEXTURE0 + RENDER_GL_TEXTURE_UNIT_MANAGER);
    pglBindTexture(GL_TEXTURE_2D, texture->id);
    pglTexImage2D(GL_TEXTURE_2D, 0, (GLint)gl_pixel_format, size.x, size.y, 0,
                  gl_pixel_format, gl_pixel_type, NULL);
    pglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    pglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if ((errcode = pglGetError()) != GL_NO_ERROR) {
        FATAL("Initializing texture failed: %s", gl_strerror(errcode));
    }

    return texture;
}

void texture_destroy(struct texture *texture)
{
    DASSERT(texture != NULL);
    if (texture->id && pglDeleteTextures) {
        pglDeleteTextures(1, &texture->id);
    }
    mem_free(texture);

    /* Detach the texture from the renderer */
    if (gl_state.texture == texture) {
        gl_state.texture = NULL;
    }
}

void texture_upload(struct texture *texture, const struct pixbuf *src, struct vec2i offset)
{
    GLenum gl_pixel_format = get_gl_pixel_format(src->format);
    GLenum gl_pixel_type = get_gl_pixel_type(src->format);
    GLenum errcode;

    DASSERT(texture && texture->id && src);
    ASSERT(pixbuf_is_ideal(src));

    gl_flush_errors();

    pglActiveTexture(GL_TEXTURE0 + RENDER_GL_TEXTURE_UNIT_MANAGER);
    pglBindTexture(GL_TEXTURE_2D, texture->id);
    pglTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, offset.y, src->size.x, src->size.y,
                     gl_pixel_format, gl_pixel_type, src->buf);

    if ((errcode = pglGetError()) != GL_NO_ERROR) {
        FATAL("Uploading texture failed: %s", gl_strerror(errcode));
    }
}
