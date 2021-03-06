/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_VIDEO_H
#define INCLUDED_VIDEO_H

#include "types.h"

void video_init(void);
void video_fini(void);
struct vec2i video_get_surface_size(void);
void video_swap_buffers(void);

void *video_gl_get_proc_address(const char *name);

#endif /* INCLUDED_VIDEO_H */
