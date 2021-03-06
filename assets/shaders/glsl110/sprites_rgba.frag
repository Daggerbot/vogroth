/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#version 110

uniform sampler2D uni_Texture;

varying vec2 var_TextureCoord;
varying vec4 var_Color;

void main()
{
    gl_FragColor = texture2D(uni_Texture, var_TextureCoord) * var_Color;
    if (gl_FragColor.a <= 0.0) {
        discard;
    }
}
