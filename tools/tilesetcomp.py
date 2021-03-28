#!/usr/bin/python3
# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3 as published by
# the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program. If not, see <https://www.gnu.org/licenses/>.

# tilesetcomp.py
#   Compile tilesets.
#
# Usage:
#   tilesetcomp.py [OPTIONS] INFILE OUTFILE
#
# Options:
#   -d DEPFILE  Generate a depfile for make/ninja.
#   -R DIR      Generate dependencies relative to DIR.

import getopt
import os
import os.path
import sys

import json
import PIL.Image

import atlascomp
from vdutil import *

SIGNATURE = 0xA287F078

#
# Entry point
#
if __name__ == "__main__":
    #
    # Parse command line options
    #

    dep_path = None
    dep_base = None
    opts, args = getopt.getopt(sys.argv[1:], "d:R:")

    for opt, param in opts:
        if opt == "-d":
            assert dep_path is None
            dep_path = param
        elif opt == "-R":
            assert dep_base is None
            dep_base = param

    assert len(args) == 2
    in_path = args[0]
    src_dir = os.path.relpath(os.path.dirname(os.path.realpath(in_path)))
    out_path = args[1]

    if dep_base is None:
        dep_base = os.curdir

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
        assert len(name) > 0
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
            fp.write("{}: {}".format(ninja_unescape(os.path.relpath(out_path, dep_base)),
                                     ninja_unescape(os.path.relpath(in_path, dep_base))))
            for path in deps:
                fp.write(" {}".format(ninja_unescape(os.path.relpath(path, dep_base))))
            fp.write("\n")
