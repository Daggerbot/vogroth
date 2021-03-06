/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
