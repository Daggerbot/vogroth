/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_MATH_H
#define INCLUDED_MATH_H

#include "types.h"

static inline int min_int(int x, int y)
{
    return x < y ? x : y;
}

static inline int max_int(int x, int y)
{
    return x > y ? x : y;
}

#endif /* INCLUDED_MATH_H */
