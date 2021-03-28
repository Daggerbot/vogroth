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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "debug.h"
#include "game_defs.h"
#include "memory.h"
#include "system.h"
#include "unicode.h"

static HANDLE hStdError = NULL;
static HANDLE hConsoleMutex = NULL;

static char *exe_dir = NULL;
static char *default_assets_path = NULL;

static char *win32_strerror_alloc(uint32_t errcode)
{
    wchar_t *wstr = NULL;
    char *str;

    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                   NULL, errcode, 0, (LPWSTR)&wstr, 0, NULL);

    if (wstr) {
        str = wide_to_utf8(-1, wstr, NULL);
        LocalFree(wstr);
    } else {
        str = str_printf("Win32 error code %" PRIu32, errcode);
    }

    return str;
}

/*
 * Allocates and returns a string containing the full of the game executable.
 */
static char *get_exe_path_alloc(void)
{
    uint32_t wbuf_size = 64;
    wchar_t *wbuf = NULL;
    char *buf;
    DWORD result;
    DWORD errcode;

    while (1) {
        wbuf = mem_realloc_array(wbuf, wbuf_size, sizeof(*wbuf));
        SetLastError(0);
        result = GetModuleFileNameW(NULL, wbuf, wbuf_size);

        /*
         * The return value of GetModuleFileNameW in the event that the input
         * buffer is too short depends on which version of Windows we're on.
         * Let's just assume that it gives a false success value to be safe.
         */
        if (result > 0 && result < wbuf_size - 1) {
            buf = wide_to_utf8(-1, wbuf, NULL);
            mem_free(wbuf);
            return buf;
        }

        errcode = GetLastError();
        if (errcode && errcode != ERROR_INSUFFICIENT_BUFFER) {
            FATAL("GetModuleFileNameW: %s", win32_strerror_alloc(errcode));
        }

        ASSERT(wbuf_size <= UINT32_MAX / 2);
        wbuf_size *= 2;
    }
}

/*
 * Returns the directory containing the game executable.
 */
static const char *get_exe_dir(void)
{
    char *last_slash = NULL;
    char *p;

    if (exe_dir) {
        return exe_dir;
    }
    exe_dir = get_exe_path_alloc();

    /* Remove the final path component */
    for (p = exe_dir; *p; ++p) {
        if (*p == '/' || *p == '\\') {
            last_slash = p;
        }
    }
    if (last_slash) {
        *last_slash = 0;
    }

    return exe_dir;
}

FILE *system_fopen(const char *path, const char *mode)
{
    wchar_t *wpath;
    wchar_t *wmode;
    FILE *fp;
    int errcode;

    wpath = utf8_to_wide(-1, path, NULL);
    wmode = utf8_to_wide(-1, mode, NULL);
    fp = _wfopen(wpath, wmode);
    errcode = errno;
    mem_free(wpath);
    mem_free(wmode);
    errno = errcode;
    return fp;
}

void system_show_error_dialog(const char *msg)
{
    wchar_t *wmsg;

    wmsg = utf8_to_wide(-1, msg, NULL);
    system_show_error_native(wmsg);
    mem_free(wmsg);
}

void system_show_error_native(const wchar_t *msg)
{
    MessageBoxW(NULL, msg, L"Error", MB_OK | MB_ICONERROR);
}

void system_init_console(void)
{
    AttachConsole(ATTACH_PARENT_PROCESS);
    SetConsoleOutputCP(CP_UTF8);
    _wfreopen(L"NUL", L"r", stdin);
    _wfreopen(L"NUL", L"w", stdout);
    _wfreopen(L"CONOUT$", L"w", stderr);
    hStdError = GetStdHandle(STD_ERROR_HANDLE);

    hConsoleMutex = CreateMutexW(NULL, FALSE, NULL);
    if (!hConsoleMutex) {
        system_show_error_native(L"CreateMutexW failed");
        exit(EXIT_FAILURE);
    }
}

void system_fini_console(void)
{
}

void system_lock_console(void)
{
    DWORD result;

    result = WaitForSingleObject(hConsoleMutex, INFINITE);
    if (result != WAIT_OBJECT_0 && result != WAIT_ABANDONED) {
        system_show_error_native(L"WaitForSingleObject failed");
        exit(EXIT_FAILURE);
    }
}

void system_unlock_console(void)
{
    ReleaseMutex(hConsoleMutex);
}

void system_set_console_color(enum console_color color)
{
    WORD attr;

    if ((int)color >= 0 && (int)color <= 15) {
        attr = (WORD)color;
    } else {
        attr = (WORD)CONSOLE_COLOR_LIGHT_GRAY;
    }

    fflush(stderr);
    SetConsoleTextAttribute(hStdError, attr);
}

void system_fini_paths(void)
{
    exe_dir = mem_free(exe_dir);
    default_assets_path = mem_free(default_assets_path);
}

const char *system_get_default_assets_path(void)
{
    if (!default_assets_path) {
        default_assets_path = str_printf("%s/" ASSETS_PACKAGE_NAME, get_exe_dir());
    }
    return default_assets_path;
}
