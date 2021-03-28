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

#ifndef INCLUDED_DEBUG_H
#define INCLUDED_DEBUG_H

#include "types.h"

enum log_level {
    LOG_LEVEL_NONE,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_DEBUG,
};

/*
 * Don't call these functions directly. They are used internally by the macros
 * defined below.
 */
void debug_log(enum log_level level, const char *fmt, ...) PRINTFLIKE(2, 3);
void debug_logd(enum log_level level, const char *file, int line, const char *fmt, ...) PRINTFLIKE(4, 5);
NORETURN void debug_fatal(const char *fmt, ...) PRINTFLIKE(1, 2);
NORETURN void debug_fatald(const char *file, int line, const char *fmt, ...) PRINTFLIKE(3, 4);

static inline void debug_discard(UNUSED const char *fmt, ...)
{
}

/*
 * LOG_ERROR, LOG_WARNING: Logs a message.
 * LOG_DEBUG: Logs a message on debug builds only.
 */
#ifdef NDEBUG
# define LOG_ERROR(...) debug_log(LOG_LEVEL_ERROR, __VA_ARGS__)
# define LOG_WARNING(...) debug_log(LOG_LEVEL_WARNING, __VA_ARGS__)
# define LOG_DEBUG(...) debug_discard(__VA_ARGS__)
#else
# define LOG_ERROR(...) debug_logd(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
# define LOG_WARNING(...) debug_logd(LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
# define LOG_DEBUG(...) debug_logd(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#endif

/*
 * FATAL: Shows an error message and exits.
 */
#ifdef NDEBUG
# define FATAL(...) debug_fatal(__VA_ARGS__)
#else
# define FATAL(...) debug_fatald(__FILE__, __LINE__, __VA_ARGS__)
#endif

/*
 * ASSERT: Shows an error message and exits if a condition is not met.
 * DASSERT: Like ASSERT, but only enabled on debug builds.
 */
#ifdef NDEBUG
# define ASSERT(CONDITION) \
    do { \
        if (!(CONDITION)) { \
            debug_fatal("Assertion failed: %s", #CONDITION); \
        } \
    } while (0)
# define DASSERT(CONDITION) ((void)(CONDITION))
#else
# define ASSERT(CONDITION) \
    do { \
        if (!(CONDITION)) { \
            debug_fatald(__FILE__, __LINE__, "Assertion failed: %s", #CONDITION); \
        } \
    } while (0)
# define DASSERT(CONDITION) \
    do { \
        if (!(CONDITION)) { \
            debug_fatald(__FILE__, __LINE__, "Assertion failed: %s", #CONDITION); \
        } \
    } while (0)
#endif

#endif /* INCLUDED_DEBUG_H */
