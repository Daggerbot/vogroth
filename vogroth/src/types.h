/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
