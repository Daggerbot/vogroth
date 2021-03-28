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

#include <SDL_timer.h>
#include <SDL_video.h>

#include "debug.h"
#include "game_defs.h"
#include "gl_api.h"
#include "video.h"

#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 480

static SDL_Window *window = NULL;
static SDL_GLContext gl_context = NULL;
static bool vsync_enabled = false;

static void init_window(void)
{
    if (window) {
        return;
    }

    SDL_GL_ResetAttributes();
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, RENDER_GL_MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, RENDER_GL_MINOR_VERSION);

    window = SDL_CreateWindow(GAME_TITLE,
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
                              SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    if (!window) {
        FATAL("Can't create game window: %s", SDL_GetError());
    }
}

static void init_gl_context(void)
{
    if (gl_context) {
        return;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        FATAL("Can't create OpenGL context: %s", SDL_GetError());
    }

    /* Try to enable vsync, but don't treat it as a hard error if we can't. */
    if (!SDL_GL_SetSwapInterval(-1)) {
        LOG_DEBUG("Enabled adaptive vsync");
        vsync_enabled = true;
    } else if (!SDL_GL_SetSwapInterval(1)) {
        LOG_DEBUG("Enabled standard vsync");
        vsync_enabled = true;
    } else {
        LOG_DEBUG("Can't enable vsync: %s", SDL_GetError());
    }
}

void video_init(void)
{
    init_window();
    init_gl_context();
}

void video_fini(void)
{
    if (gl_context) {
        SDL_GL_DeleteContext(gl_context);
        gl_context = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    vsync_enabled = false;
}

struct vec2i video_get_surface_size(void)
{
    struct vec2i size = {0, 0};

    if (window) {
        SDL_GetWindowSize(window, &size.x, &size.y);
    }
    return size;
}

void video_swap_buffers(void)
{
    if (!window) {
        return;
    }
    SDL_GL_SwapWindow(window);

    /*
     * Try not to render zillions of frames per second. This would be a huge
     * waste of system resources in such a simple game.
     */
    if (!vsync_enabled) {
        SDL_Delay(1);
    }
}

void *video_gl_get_proc_address(const char *name)
{
    return SDL_GL_GetProcAddress(name);
}
