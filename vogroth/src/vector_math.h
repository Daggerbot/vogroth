/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_VECTOR_MATH_H
#define INCLUDED_VECTOR_MATH_H

#include "types.h"

#undef min
#undef max

#define MAT4F_IDENTITY_INIT \
    {{1.0f, 0.0f, 0.0f, 0.0f}, \
     {0.0f, 1.0f, 0.0f, 0.0f}, \
     {0.0f, 0.0f, 1.0f, 0.0f}, \
     {0.0f, 0.0f, 0.0f, 1.0f}}
#define MAT4F_IDENTITY ((struct mat4f)MAT4F_IDENTITY_INIT)

/* Returns an orthographic projection matrix. */
struct mat4f mat4f_ortho(struct vec3f min, struct vec3f max);

#endif /* INCLUDED_VECTOR_MATH_H */
