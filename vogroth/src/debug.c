/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "memory.h"
#include "system.h"

/*
 * The __FILE__ token, as used in the debug macros, evaluates to a longer path
 * than we need for the purpose of logging messages. This returns only the last
 * path component.
 */
static const char *get_short_src_path(const char *path)
{
    const char *match;

    if (!path) {
        return NULL;
    }
    while (1) {
        match = strchr(path, '/');
#ifdef _WIN32
        if (!match) {
            match = strchr(path, '\\');
        }
#endif
        if (match) {
            path = match + 1;
        } else {
            return path;
        }
    }
}

#ifdef NDEBUG
void debug_log(enum log_level level, const char *fmt, ...)
#else
void debug_logd(enum log_level level, const char *file, int line, const char *fmt, ...)
#endif
{
    va_list args;

    switch (level) {
    case LOG_LEVEL_ERROR:
        system_lock_console();
        system_set_console_color(CONSOLE_COLOR_BRIGHT_RED);
        fputs("ERROR: ", stderr);
        system_set_console_color(CONSOLE_COLOR_NORMAL);
        break;
    case LOG_LEVEL_WARNING:
        system_lock_console();
        system_set_console_color(CONSOLE_COLOR_BRIGHT_YELLOW);
        fputs("WARNING: ", stderr);
        system_set_console_color(CONSOLE_COLOR_NORMAL);
        break;
#ifndef NDEBUG
    case LOG_LEVEL_DEBUG:
        system_lock_console();
        system_set_console_color(CONSOLE_COLOR_BRIGHT_GREEN);
        fputs("DEBUG: ", stderr);
        system_set_console_color(CONSOLE_COLOR_NORMAL);
        break;
#endif
    default:
        return;
    }

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

#ifndef NDEBUG
    system_set_console_color(CONSOLE_COLOR_DARK_GRAY);
    fprintf(stderr, " (%s:%d)", get_short_src_path(file), line);
#endif

    system_set_console_color(CONSOLE_COLOR_NORMAL);
    fputc('\n', stderr);
    fflush(stderr);
    system_unlock_console();
}

#ifdef NDEBUG
NORETURN void debug_fatal(const char *fmt, ...)
#else
NORETURN void debug_fatald(const char *file, int line, const char *fmt, ...)
#endif
{
    va_list args;
    struct buf msg = BUF_INIT;

    system_lock_console();
    system_set_console_color(CONSOLE_COLOR_BRIGHT_RED);
    fputs("FATAL: ", stderr);
    system_set_console_color(CONSOLE_COLOR_RED);
    va_start(args, fmt);
    buf_vappendf(&msg, fmt, args);
    va_end(args);
    fputs(msg.data, stderr);

#ifndef NDEBUG
    file = get_short_src_path(file);
    system_set_console_color(CONSOLE_COLOR_DARK_GRAY);
    fprintf(stderr, " (%s:%d)", file, line);
    buf_appendf(&msg, " (%s:%d)", file, line);
#endif

    system_set_console_color(CONSOLE_COLOR_NORMAL);
    fputc('\n', stderr);
    fflush(stderr);
    system_show_error_dialog(msg.data);
    buf_fini(&msg);
    exit(EXIT_FAILURE);
}
