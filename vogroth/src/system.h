/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_SYSTEM_H
#define INCLUDED_SYSTEM_H

#include <stdio.h>

#include "types.h"

/*
 * oschar_t: The target platform's preferred char type.
 * OSSTR: Encodes the following string as an oschar_t string.
 */
#ifdef _WIN32
# define OSSTR L""
typedef wchar_t oschar_t;
#else
# define OSSTR
typedef char oschar_t;
#endif

enum console_color {
    CONSOLE_COLOR_NORMAL = -1,
    CONSOLE_COLOR_BLACK = 0,
    CONSOLE_COLOR_BLUE = 1,
    CONSOLE_COLOR_GREEN = 2,
    CONSOLE_COLOR_CYAN = 3,
    CONSOLE_COLOR_RED = 4,
    CONSOLE_COLOR_MAGENTA = 5,
    CONSOLE_COLOR_YELLOW = 6,
    CONSOLE_COLOR_LIGHT_GRAY = 7,
    CONSOLE_COLOR_DARK_GRAY = 8,
    CONSOLE_COLOR_BRIGHT_BLUE = 9,
    CONSOLE_COLOR_BRIGHT_GREEN = 10,
    CONSOLE_COLOR_BRIGHT_CYAN = 11,
    CONSOLE_COLOR_BRIGHT_RED = 12,
    CONSOLE_COLOR_BRIGHT_MAGENTA = 13,
    CONSOLE_COLOR_BRIGHT_YELLOW = 14,
    CONSOLE_COLOR_WHITE = 15,
};

/*
 * The MSVCRT version of fopen() can't work with UTF-8 strings as far as I
 * know. Let's use this as a workaround.
 */
#ifdef _WIN32
FILE *system_fopen(const char *path, const char *mode);
#else
# define system_fopen fopen
#endif

/*
 * Shows an error dialog with a UTF-8 string. Does nothing if the platform has
 * no native error dialog.
 */
void system_show_error_dialog(const char *msg);

/*
 * Shows an error message in a platform-dependent way. Uses oschar_t to avoid
 * making any internal allocations, as this is how allocation errors are
 * reported.
 */
void system_show_error_native(const oschar_t *msg);

void system_init_console(void);
void system_fini_console(void);
void system_lock_console(void);
void system_unlock_console(void);
void system_set_console_color(enum console_color color);

void system_fini_paths(void);
const char *system_get_default_assets_path(void);

#endif /* INCLUDED_SYSTEM_H */
