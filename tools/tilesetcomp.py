#!/usr/bin/python3
# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# tilesetcomp.py
#   Compile tilesets.
#
# Usage:
#   tilesetcomp.py [OPTIONS] INFILE OUTFILE
#
# Options:
#   -d DEPFILE  Generate a depfile for make/ninja.

import getopt
import os
import os.path
import sys

import json
import PIL.Image

import atlascomp
from vdutil import *

SIGNATURE = 0xA287F078

def unescape(s):
    out = ""
    for c in s:
        if c == ' ':
            out += "\\ "
        elif c == '$':
            out += "\\$"
        elif c == '\\':
            out += "\\\\"
        else:
            out += c
    return out

#
# Entry point
#
if __name__ == "__main__":
    #
    # Parse command line options
    #

    dep_path = None
    opts, args = getopt.getopt(sys.argv[1:], "d:")

    for opt, param in opts:
        if opt == "-d":
            assert dep_path is None
            dep_path = param

    assert len(args) == 2
    in_path = args[0]
    src_dir = os.path.relpath(os.path.dirname(os.path.realpath(in_path)))
    out_path = args[1]

    #
    # Parse tileset file
    #

    with open(in_path, "r") as fp:
        doc = json.load(fp)

    assert doc["grid"]["orientation"] == "orthogonal"
    assert doc["type"] == "tileset"

    tile_width = doc["tilewidth"]
    assert isinstance(tile_width, int) and tile_width > 0
    tile_height = doc["tileheight"]
    assert isinstance(tile_height, int) and tile_height > 0

    #
    # Load tile images
    #

    name_map = {}
    name_data = bytearray()
    name_offset = 0
    name_offsets = []
    images = []
    deps = []

    for index in range(len(doc["tiles"])):
        item = doc["tiles"][index]

        name = item["type"]
        assert not name in name_map
        name_map[name] = index
        name_bytes = bytearray(name + "\0", encoding="utf-8")
        name_offsets.append(name_offset)
        name_data.extend(name_bytes)
        name_offset += len(name_bytes)

        path = os.path.join(src_dir, item["image"])
        image = PIL.Image.open(path)
        assert image.size == (tile_width, tile_height)
        images.append(image)

        if dep_path:
            deps.append(path)

    #
    # Compile atlas image
    #

    data = atlascomp.compile(images, uniform=True)

    #
    # Write output file
    #

    os.makedirs(os.path.dirname(os.path.realpath(out_path)), mode=0o755, exist_ok=True)

    with open(out_path, "wb") as fp:
        fp.write(u32le(SIGNATURE))
        fp.write(u8(0)) # file version
        atlascomp.serialize(fp, data, uniform=True)

        # Write tile names
        fp.write(u16le(len(name_data)))
        fp.write(name_data)
        for offset in name_offsets:
            fp.write(u16le(offset))

    #
    # Write dependency file
    #

    if dep_path:
        os.makedirs(os.path.dirname(os.path.realpath(dep_path)), mode=0o755, exist_ok=True)

        with open(dep_path, "w") as fp:
            fp.write("{}: {}".format(out_path, in_path))
            for path in deps:
                fp.write(" {}".format(unescape(path)))
            fp.write("\n")
