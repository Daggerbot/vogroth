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

#ifndef INCLUDED_TYPES_H
#define INCLUDED_TYPES_H

#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cdefs.h"

#define KiB (1024)
#define MiB (1024*1024)
#define GiB (1024*1024*1024)

struct vec2i {
    int x, y;
};

struct vec2f {
    float x, y;
};

struct vec3f {
    float x, y, z;
};

struct vec4f {
    float x, y, z, w;
};

struct rect2i {
    struct vec2i a, b;
};

struct mat4f {
    struct vec4f x, y, z, w;
};

#endif /* INCLUDED_TYPES_H */
