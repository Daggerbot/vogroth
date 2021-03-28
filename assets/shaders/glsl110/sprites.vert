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

uniform mat4 uni_Transform;
uniform vec2 uni_TextureSize;

attribute vec2 attr_Position;
attribute vec2 attr_TextureCoord;
attribute vec4 attr_Color;

varying vec2 var_TextureCoord;
varying vec4 var_Color;

void main()
{
    gl_Position = uni_Transform * vec4(attr_Position, 0.0, 1.0);
    var_TextureCoord = attr_TextureCoord / uni_TextureSize;
    var_Color = attr_Color;
}
