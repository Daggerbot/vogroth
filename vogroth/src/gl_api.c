/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>

#include "debug.h"
#include "gl_api.h"
#include "video.h"

/*
 * Define the 'pgl' OpenGL API functions.
 */
#define DO(RETURN, NAME, ...) \
    RETURN(GLAPIENTRY *pgl##NAME)(__VA_ARGS__) = NULL; \
    STATIC_ASSERT(sizeof(pgl##NAME) == sizeof(void *));
FOREACH_GL_FUNCTION(DO)
#undef DO

/*
 * Make a list of the 'pgl' OpenGL API functions. This allows us to work with
 * them in a loop which should result in smaller and faster generated code.
 */
struct symdef {
    const char *name;
    void **ptr;
};

static const struct symdef symdefs[] = {
#define DO(RETURN, NAME, ...) {"gl" #NAME, (void **)&pgl##NAME},
    FOREACH_GL_FUNCTION(DO)
#undef DO
};

static void *require_proc_address(const char *name)
{
    void *sym;

    sym = video_gl_get_proc_address(name);
    if (!sym) {
        FATAL("Missing OpenGL function: %s", name);
    }
    return sym;
}

static void check_version(void)
{
    const char *verstr;
    int major, minor;

    if (!pglGetString) {
        *(void **)&pglGetString = require_proc_address("glGetString");
    }
    verstr = pglGetString(GL_VERSION);
    if (!verstr) {
        FATAL("Can't query GL_VERSION");
    }
    if (sscanf(verstr, "%d.%d", &major, &minor) != 2) {
        FATAL("Can't parse GL_VERSION: %s", verstr);
    }
    if (major < RENDER_GL_MAJOR_VERSION || (major == RENDER_GL_MAJOR_VERSION && minor < RENDER_GL_MINOR_VERSION)) {
        FATAL("Unsupported OpenGL version: %s; need at least %d.%d",
              verstr, RENDER_GL_MAJOR_VERSION, RENDER_GL_MINOR_VERSION);
    }
    LOG_DEBUG("OpenGL %s", verstr);
}

static void load_functions(void)
{
    unsigned i;

    for (i = 0; i < LENGTHOF(symdefs); ++i) {
        *symdefs[i].ptr = require_proc_address(symdefs[i].name);
    }
}

/*
 * Sets the 'pgl' function pointers to null. If there are any lingering
 * renderer resources to clean up after this is called, they will know not to
 * call these functions if they are null.
 */
static void unload_functions(void)
{
    unsigned i;

    for (i = 0; i < LENGTHOF(symdefs); ++i) {
        *symdefs[i].ptr = NULL;
    }
}

void gl_init_api(void)
{
    check_version();
    load_functions();
}

void gl_fini_api(void)
{
    unload_functions();
}

const char *gl_strerror(GLenum errcode)
{
    static char buf[32] = {0};

    switch (errcode) {
    case GL_NO_ERROR:
        return "No error";
    case GL_INVALID_ENUM:
        return "Invalid enum";
    case GL_INVALID_OPERATION:
        return "Invalid operation";
    case GL_INVALID_VALUE:
        return "Invalid value";
    case GL_OUT_OF_MEMORY:
        return "Out of memory";
    case GL_STACK_OVERFLOW:
        return "Stack overflow";
    case GL_STACK_UNDERFLOW:
        return "Stack underflow";
    case GL_TABLE_TOO_LARGE:
        return "Table too large";
    default:
        snprintf(buf, sizeof(buf), "OpenGL error code 0x%04X", errcode);
        return buf;
    }
}

void gl_flush_errors(void)
{
    GLenum errcode;
    int num_errs = 0;

    while ((errcode = pglGetError()) != GL_NO_ERROR) {
        LOG_ERROR("OpenGL: %s", gl_strerror(errcode));
        if (++num_errs == 50) {
            FATAL("Too many OpenGL errors");
        }
    }
}
