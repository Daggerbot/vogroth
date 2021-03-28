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

#version 110

uniform sampler2D uni_Texture;

varying vec2 var_TextureCoord;
varying vec4 var_Color;

void main()
{
    gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(uni_Texture, var_TextureCoord).a) * var_Color;
    if (gl_FragColor.a <= 0.0) {
        discard;
    }
}
