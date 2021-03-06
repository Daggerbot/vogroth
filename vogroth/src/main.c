/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#ifdef _WIN32
# include <windows.h>
#endif

#include <SDL_events.h>

#include "assets.h"
#include "debug.h"
#include "render.h"
#include "sandbox.h"
#include "system.h"
#include "unicode.h"
#include "video.h"

static bool quit_requested = false;

static void handle_window_event(const SDL_WindowEvent *event)
{
    switch (event->event) {
    case SDL_WINDOWEVENT_CLOSE:
        quit_requested = true;
        break;

    default:
        break;
    }
}

static void handle_event(const SDL_Event *event)
{
    switch (event->type) {
    case SDL_QUIT:
        quit_requested = true;
        break;

    case SDL_WINDOWEVENT:
        handle_window_event(&event->window);
        break;

    default:
        break;
    }
}

static void main_loop(void)
{
    SDL_Event event;

    while (!quit_requested) {
        while (SDL_PollEvent(&event)) {
            handle_event(&event);
            if (quit_requested) {
                return;
            }
        }

        render_begin_frame();
        sandbox_render();
        render_end_frame();
        video_swap_buffers();
    }
}

/*
 * Common main function used on all platforms.
 */
static int vogroth_main(int argc, char **argv)
{
    const char *assets_path = NULL;
    int i;

    system_init_console();

    for (i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-assets")) {
            if (i + 1 >= argc) {
                FATAL("Missing argument for %s", argv[i]);
            }
            assets_path = argv[++i];
        } else if (argv[i][0] == '-') {
            FATAL("Invalid option: %s", argv[i]);
        } else {
            FATAL("Unexpected operand: %s", argv[i]);
        }
    }

    LOG_DEBUG("Initializing...");
    assets_init(assets_path);
    video_init();
    render_init();
    sandbox_init();

    LOG_DEBUG("Game started!");
    main_loop();

    LOG_DEBUG("Shutting down...");
    sandbox_fini();
    render_fini();
    video_fini();
    assets_fini();
    system_fini_paths();
    system_fini_console();
    return 0;
}

/*
 * Entry point on most platforms.
 */
#ifndef _WIN32
int main(int argc, char *argv[])
{
    return vogroth_main(argc, argv);
}
#endif /* !defined(_WIN32) */

/*
 * Entry point on Windows.
 */
#ifdef _WIN32
int WINAPI wWinMain(
    UNUSED HINSTANCE hInstance,
    UNUSED HINSTANCE hPrevInstance,
    UNUSED LPWSTR lpCmdLine,
    UNUSED int nShowCmd)
{
    int argc = __argc;
    wchar_t **wargv = __wargv;
    char **argv;
    int i;
    int result;

    /* Convert the command line to UTF-8 */
    if (argc < 0) {
        argc = 0;
    }
    argv = mem_alloc_array((size_t)argc, sizeof(*argv));
    for (i = 0; i < argc; ++i) {
        argv[i] = wide_to_utf8(-1, wargv[i], NULL);
    }

    result = vogroth_main(argc, argv);

    for (i = 0; i < argc; ++i) {
        mem_free(argv[i]);
    }
    mem_free(argv);
    return result;
}
#endif /* defined(_WIN32) */
