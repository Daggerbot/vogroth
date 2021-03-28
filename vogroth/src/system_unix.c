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

#include "config.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_defs.h"
#include "system.h"

static pthread_mutex_t console_mutex = PTHREAD_MUTEX_INITIALIZER;

void system_show_error_dialog(UNUSED const char *msg)
{
}

void system_show_error_native(const oschar_t *msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    fflush(stderr);
}

void system_init_console(void)
{
    if (pthread_mutex_init(&console_mutex, NULL)) {
        fprintf(stderr, "ERROR: pthread_mutex_init: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void system_fini_console(void)
{
    pthread_mutex_destroy(&console_mutex);
}

void system_lock_console(void)
{
    if (pthread_mutex_lock(&console_mutex)) {
        fprintf(stderr, "ERROR: pthread_mutex_lock: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void system_unlock_console(void)
{
    pthread_mutex_unlock(&console_mutex);
}

void system_set_console_color(enum console_color color)
{
    switch (color) {
    case CONSOLE_COLOR_BLACK:
        fputs("\x1b[0;30m", stderr);
        break;
    case CONSOLE_COLOR_BLUE:
        fputs("\x1b[0;34m", stderr);
        break;
    case CONSOLE_COLOR_GREEN:
        fputs("\x1b[0;32m", stderr);
        break;
    case CONSOLE_COLOR_CYAN:
        fputs("\x1b[0;36m", stderr);
        break;
    case CONSOLE_COLOR_RED:
        fputs("\x1b[0;31m", stderr);
        break;
    case CONSOLE_COLOR_MAGENTA:
        fputs("\x1b[0;35m", stderr);
        break;
    case CONSOLE_COLOR_YELLOW:
        fputs("\x1b[0;33m", stderr);
        break;
    case CONSOLE_COLOR_LIGHT_GRAY:
        fputs("\x1b[0;37m", stderr);
        break;
    case CONSOLE_COLOR_DARK_GRAY:
        fputs("\x1b[1;30m", stderr);
        break;
    case CONSOLE_COLOR_BRIGHT_BLUE:
        fputs("\x1b[1;34m", stderr);
        break;
    case CONSOLE_COLOR_BRIGHT_GREEN:
        fputs("\x1b[1;32m", stderr);
        break;
    case CONSOLE_COLOR_BRIGHT_CYAN:
        fputs("\x1b[1;36m", stderr);
        break;
    case CONSOLE_COLOR_BRIGHT_RED:
        fputs("\x1b[1;31m", stderr);
        break;
    case CONSOLE_COLOR_BRIGHT_MAGENTA:
        fputs("\x1b[1;35m", stderr);
        break;
    case CONSOLE_COLOR_BRIGHT_YELLOW:
        fputs("\x1b[1;33m", stderr);
        break;
    case CONSOLE_COLOR_WHITE:
        fputs("\x1b[1;37m", stderr);
        break;
    default:
        fputs("\x1b[0m", stderr);
        break;
    }
}

void system_fini_paths(void)
{
}

const char *system_get_default_assets_path(void)
{
    return DATADIR "/games/" GAME_ID "/" ASSETS_PACKAGE_NAME;
}
