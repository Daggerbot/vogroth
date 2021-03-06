/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
