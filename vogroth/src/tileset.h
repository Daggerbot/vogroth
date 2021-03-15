/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_TILES_H
#define INCLUDED_TILES_H

#include "texture.h"

struct tile_def {
    char *name;
    struct rect2i src_rect;
};

struct tileset {
    struct texture *texture; /* owned */
    int num_tiles;
    char *name_data;
    struct tile_def *tiles;
};

struct tileset *tileset_init(void);
void tileset_fini(void);

#endif /* INCLUDED_TILES_H */
