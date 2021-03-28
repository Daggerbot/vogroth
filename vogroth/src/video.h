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

#ifndef INCLUDED_VIDEO_H
#define INCLUDED_VIDEO_H

#include "types.h"

void video_init(void);
void video_fini(void);
struct vec2i video_get_surface_size(void);
void video_swap_buffers(void);

void *video_gl_get_proc_address(const char *name);

#endif /* INCLUDED_VIDEO_H */
