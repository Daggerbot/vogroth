/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
