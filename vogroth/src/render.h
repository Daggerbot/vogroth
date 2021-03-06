/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_RENDER_H
#define INCLUDED_RENDER_H

#include "types.h"

struct texture;

void render_init(void);
void render_fini(void);
void render_begin_frame(void);
void render_end_frame(void);

/* Sets the transform to world coordinates = screen coordinates */
void render_use_ui_transform(void);
void render_use_texture(struct texture *texture);

#endif /* INCLUDED_RENDER_H */
