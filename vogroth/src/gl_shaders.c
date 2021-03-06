/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "assets.h"
#include "debug.h"
#include "gl_api.h"
#include "gl_shaders.h"
#include "gl_state.h"
#include "memory.h"

struct gl_program gl_program_alpha_sprites = RENDER_GL_PROGRAM_INIT;
struct gl_program gl_program_rgb_sprites = RENDER_GL_PROGRAM_INIT;
struct gl_program gl_program_rgba_sprites = RENDER_GL_PROGRAM_INIT;

static GLuint load_shader(const char *name, GLenum type)
{
    char *err = NULL;
    struct rw *rw;
    struct buf src = BUF_INIT;
    const GLchar *src_ptr;
    GLint src_len;
    GLuint id;
    GLint status = GL_FALSE;
    GLint info_log_len = 0;
    char *info_log;
    GLenum errcode;

    gl_flush_errors();

    /* Load shader source */
    rw = assets_open(name, &err);
    if (!rw) {
        FATAL("%s: %s", name, err);
    }
    rw_read_to_buf(rw, 1*MiB, &src);
    if (rw->error) {
        FATAL("%s: %s", name, rw->error);
    }
    rw_close(rw, NULL);
    src_ptr = src.data;
    src_len = (GLint)src.len;

    /* Initialize OpenGL shader */
    id = pglCreateShader(type);
    if (!id) {
        FATAL("glCreateShader: %s", gl_strerror(pglGetError()));
    }
    pglShaderSource(id, 1, &src_ptr, &src_len);
    pglCompileShader(id);
    pglGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        pglGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_log_len);
        if (info_log_len < 0) {
            info_log_len = 0;
        }
        info_log = mem_alloc((size_t)info_log_len + 1);
        info_log[0] = 0;
        pglGetShaderInfoLog(id, info_log_len + 1, &info_log_len, info_log);
        FATAL("%s: %s", name, info_log);
    }

    /* If by some chance there are errors we didn't catch */
    if ((errcode = pglGetError()) != GL_NO_ERROR) {
        FATAL("%s: %s", name, gl_strerror(errcode));
    }

    return id;
}

static void link_program(struct gl_program *program, const char *name, GLuint vert, GLuint frag)
{
    GLint status = GL_FALSE;
    GLint info_log_len = 0;
    char *info_log;
    GLenum errcode;

    gl_flush_errors();

    program->id = pglCreateProgram();
    if (!program->id) {
        FATAL("glCreateProgram: %s", gl_strerror(pglGetError()));
    }
    pglAttachShader(program->id, vert);
    pglAttachShader(program->id, frag);
    pglLinkProgram(program->id);
    pglGetProgramiv(program->id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        pglGetProgramiv(program->id, GL_INFO_LOG_LENGTH, &info_log_len);
        if (info_log_len < 0) {
            info_log_len = 0;
        }
        info_log = mem_alloc((size_t)info_log_len + 1);
        info_log[0] = 0;
        pglGetProgramInfoLog(program->id, info_log_len + 1, &info_log_len, info_log);
        FATAL("%s: %s", name, info_log);
    }

    /* Get uniform and vertex attribute indices */
    program->uni_transform = pglGetUniformLocation(program->id, "uni_Transform");
    program->uni_texture = pglGetUniformLocation(program->id, "uni_Texture");
    program->uni_texture_size = pglGetUniformLocation(program->id, "uni_TextureSize");

    program->attr_position = pglGetAttribLocation(program->id, "attr_Position");
    program->attr_texture_coord = pglGetAttribLocation(program->id, "attr_TextureCoord");
    program->attr_color = pglGetAttribLocation(program->id, "attr_Color");

    /* If by some chance there are errors we didn't catch */
    if ((errcode = pglGetError()) != GL_NO_ERROR) {
        FATAL("%s: %s", name, gl_strerror(errcode));
    }
}

static void fini_program(struct gl_program *program)
{
    if (program->id && pglDeleteProgram) {
        pglDeleteProgram(program->id);
    }
    *program = RENDER_GL_PROGRAM_NULL;
}

void gl_init_shaders(void)
{
    GLuint vert_sprites = load_shader("shaders/glsl110/sprites.vert", GL_VERTEX_SHADER);
    GLuint frag_sprites_alpha = load_shader("shaders/glsl110/sprites_alpha.frag", GL_FRAGMENT_SHADER);
    GLuint frag_sprites_rgb = load_shader("shaders/glsl110/sprites_rgb.frag", GL_FRAGMENT_SHADER);
    GLuint frag_sprites_rgba = load_shader("shaders/glsl110/sprites_rgba.frag", GL_FRAGMENT_SHADER);

    link_program(&gl_program_alpha_sprites, "alpha_sprites", vert_sprites, frag_sprites_alpha);
    link_program(&gl_program_rgb_sprites, "rgb_sprites", vert_sprites, frag_sprites_rgb);
    link_program(&gl_program_rgba_sprites, "rgba_sprites", vert_sprites, frag_sprites_rgba);

    pglDeleteShader(vert_sprites);
    pglDeleteShader(frag_sprites_alpha);
    pglDeleteShader(frag_sprites_rgb);
    pglDeleteShader(frag_sprites_rgba);
}

void gl_fini_shaders(void)
{
    fini_program(&gl_program_alpha_sprites);
    fini_program(&gl_program_rgb_sprites);
    fini_program(&gl_program_rgba_sprites);
}

void gl_use_program(struct gl_program *program)
{
    if (program == gl_state.program) {
        return;
    }
    DASSERT(program != NULL);
    pglUseProgram(program->id);
    gl_state.program = program;

    /*
     * Reinitialize all of the uniform states. This may result in some
     * redundant calls, but it eliminates undefined behavior if the caller
     * doesn't update them manually.
     */
    if (program->uni_transform >= 0) {
        pglUniformMatrix4fv(program->uni_transform, 1, GL_FALSE, &gl_state.transform.x.x);
    }
    if (program->uni_texture >= 0) {
        pglUniform1i(program->uni_texture, RENDER_GL_TEXTURE_UNIT_TEXTURE);
    }
    if (program->uni_texture_size >= 0) {
        //TODO when textures are implemented
    }
}
