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

#include "debug.h"
#include "gl_api.h"
#include "gl_shaders.h"
#include "gl_state.h"

/*
 * Increase if we need more vertex attribute indices, but doing so will make
 * use_attrib_mask slightly slower.
 */
#define MAX_ATTRIBS 4
STATIC_ASSERT(MAX_ATTRIBS <= sizeof(gl_attrib_mask_t) * CHAR_BIT);

struct gl_state gl_state = RENDER_GL_STATE_INIT;

void gl_use_transform(struct mat4f transform)
{
    gl_state.transform = transform;
    if (gl_state.program && gl_state.program->uni_transform >= 0) {
        pglUniformMatrix4fv(gl_state.program->uni_transform, 1, GL_FALSE, &gl_state.transform.x.x);
    }
}

/*
 * Enables/disables vertex attribute arrays until only the ones specified here
 * are enabled. Negative indices are ignored.
 */
static void use_attrib_mask(int count, ...)
{
    static bool warned_max_attribs = false;

    gl_attrib_mask_t desired_mask = 0;
    gl_attrib_mask_t bit = 1;
    gl_attrib_mask_t enable;
    va_list args;
    GLint attrib;
    int i;

    /* Get desired_mask from var args */
    va_start(args, count);
    for (i = 0; i < count; ++i) {
        attrib = va_arg(args, GLint);
        if (attrib >= MAX_ATTRIBS) {
            if (!warned_max_attribs) {
                LOG_WARNING("Maximum vertex attributes exceeded");
                warned_max_attribs = true;
            }
        } else if (attrib >= 0) {
            desired_mask |= (gl_attrib_mask_t)1 << attrib;
        }
    }
    va_end(args);

    /* Enable/disable attributes to match desired_mask */
    for (i = 0; i < MAX_ATTRIBS; ++i) {
        enable = desired_mask & bit;
        if (enable != (gl_state.attrib_mask & bit)) {
            if (enable) {
                pglEnableVertexAttribArray((GLuint)i);
            } else {
                pglDisableVertexAttribArray((GLuint)i);
            }
        }
        bit <<= 1;
    }
    gl_state.attrib_mask = desired_mask;
}

void gl_use_sprite_vertex_ptr(const struct sprite_vertex *ptr)
{
    if (!gl_state.program) {
        return;
    }

    use_attrib_mask(3, gl_state.program->attr_position,
                       gl_state.program->attr_texture_coord,
                       gl_state.program->attr_color);

    if (gl_state.program->attr_position >= 0) {
        pglVertexAttribPointer((GLuint)gl_state.program->attr_position,
                               2, GL_INT, GL_FALSE, sizeof(*ptr), &ptr->position.x);
    }
    if (gl_state.program->attr_texture_coord >= 0) {
        pglVertexAttribPointer((GLuint)gl_state.program->attr_texture_coord,
                               2, GL_INT, GL_FALSE, sizeof(*ptr), &ptr->texture_coord.x);
    }
    if (gl_state.program->attr_color >= 0) {
        pglVertexAttribPointer((GLuint)gl_state.program->attr_color,
                               4, GL_FLOAT, GL_FALSE, sizeof(*ptr), &ptr->color.x);
    }
}
