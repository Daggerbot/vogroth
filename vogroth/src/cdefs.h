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

#ifndef INCLUDED_CDEFS_H
#define INCLUDED_CDEFS_H

/* LENGTHOF: Gets the number of values in an array. */
#define LENGTHOF(x) (sizeof(x) / sizeof(0[x]))

/* NORETURN: Attribute for functions that never return. */
#if defined(__STDC__) && __STDC_VERSION__ >= 201112L
# define NORETURN _Noreturn
#elif defined(__GNUC__)
# define NORETURN __attribute__((__noreturn__))
#elif defined(_MSC_VER)
# define NORETURN __declspec(noreturn)
#else
# define NORETURN
#endif

/* PRINTFLIKE: Attribute for functions with printf-like formatting syntax. */
#ifdef __GNUC__
# define PRINTFLIKE(f, v) __attribute__((__format__(__printf__, f, v)))
#else
# define PRINTFLIKE(f, v)
#endif

/* STATIC_ASSERT: Single parameter static_assert. */
#define STATIC_ASSERT(CONDITION) _Static_assert((CONDITION), #CONDITION)

/* UNUSED: Attribute which silences warnings about unused items. */
#ifdef __GNUC__
# define UNUSED __attribute__((__unused__))
#else
# define UNUSED
#endif

#endif /* INCLUDED_CDEFS_H */
