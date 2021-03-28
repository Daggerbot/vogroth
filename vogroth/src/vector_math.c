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

#include "vector_math.h"

struct mat4f mat4f_ortho(struct vec3f min, struct vec3f max)
{
    return (struct mat4f) {
        {2.0f / (max.x - min.x), 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f / (max.y - min.y), 0.0f, 0.0f},
        {0.0f, 0.0f, 2.0f / (max.z - min.z), 0.0f},
        {(min.x + max.x) / (min.x - max.x),
         (min.y + max.y) / (min.y - max.y),
         (min.z + max.z) / (min.z - max.z),
         1.0f}};
}
