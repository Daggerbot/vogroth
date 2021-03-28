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
