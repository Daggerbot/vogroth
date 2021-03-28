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

# maptool.py
#   Compile Tiled maps.
#
# Usage:
#   maptool.py [OPTIONS] IN-FILE OUT-FILE
#
# Options:
#   -d DEPFILE  Generate dependency file for make/ninja.
#   -R DIR      Generate dependencies relative to DIR.

import getopt
import os
import os.path
import sys

import json

from vdutil import *

SIGNATURE = 0x1D78DC8E

class Tile:
    def __init__(self, name, gid):
        self.name = name
        self.gid = gid
        self.used = False

#
# Entry point.
#
if __name__ == "__main__":
    #
    # Parse command line args.
    #

    in_path = None
    out_path = None
    dep_path = None
    dep_dir = None
    dep_base = None
    tileset_path = None
    opts, args = getopt.getopt(sys.argv[1:], "d:R:")

    for opt, param in opts:
        if opt == "-d":
            assert dep_path is None
            dep_path = param
            dep_dir = os.path.relpath(os.path.dirname(os.path.realpath(dep_path)))
        elif opt == "-R":
            assert dep_base is None
            dep_base = param

    assert len(args) == 2
    in_path = args[0]
    src_dir = os.path.relpath(os.path.dirname(os.path.realpath(in_path)))
    out_path = args[1]
    out_dir = os.path.relpath(os.path.dirname(os.path.realpath(out_path)))

    if dep_base is None:
        dep_base = os.curdir

    #
    # Parse map file.
    #

    with open(in_path, "r") as fp:
        map_doc = json.load(fp)

    assert not "compressionlevel" in map_doc or map_doc["compressionlevel"] == -1
    assert not "infinite" in map_doc or map_doc["infinite"] == False
    assert len(map_doc["layers"]) == 1
    assert not "orientation" in map_doc or map_doc["orientation"] == "orthogonal"
    assert not "renderorder" in map_doc or map_doc["renderorder"] == "right-down"
    assert len(map_doc["tilesets"]) == 1
    assert map_doc["type"] == "map"

    map_width = map_doc["width"]
    map_height = map_doc["height"]
    firstgid = map_doc["tilesets"][0]["firstgid"]
    tileset_path = os.path.join(src_dir, map_doc["tilesets"][0]["source"])

    assert map_doc["layers"][0]["x"] == 0
    assert map_doc["layers"][0]["y"] == 0
    assert map_doc["layers"][0]["width"] == map_width
    assert map_doc["layers"][0]["height"] == map_height
    assert len(map_doc["layers"][0]["data"]) == map_width * map_height

    #
    # Parse tileset file.
    #

    with open(tileset_path, "r") as fp:
        tileset_doc = json.load(fp)

    tiles = []
    tile_name_map = {}
    tile_gid_map = {}

    for item in tileset_doc["tiles"]:
        name = item["type"]
        gid = item["id"]
        index = len(tiles)

        assert len(name) > 0
        assert not name in tile_name_map

        tile = Tile(name, gid)
        tiles.append(tile)
        tile_name_map[name] = index
        tile_gid_map[gid] = index

    #
    # Process map data.
    #

    map_data = []
    tile_name_data = bytearray()
    tile_name_offset = 0
    tile_name_offsets = []

    for gid in map_doc["layers"][0]["data"]:
        gid -= firstgid
        index = tile_gid_map[gid]
        tiles[index].used = True
        map_data.append(index)

    for index in range(len(tiles)):
        tile = tiles[index]
        if tile.used:
            uname = bytearray(tile.name + "\0", encoding="utf-8")
            tile_name_data.extend(uname)
            tile_name_offsets.append((index, tile_name_offset))
            tile_name_offset += len(uname)

    #
    # Write map file.
    #

    os.makedirs(out_dir, mode=0o755, exist_ok=True)

    with open(out_path, "wb") as fp:
        # Write file header.
        fp.write(u32le(SIGNATURE))
        fp.write(u8(0)) # file
        fp.write(u16le(map_width))
        fp.write(u16le(map_height))
        fp.write(u16le(len(tile_name_offsets)))
        fp.write(u16le(len(tile_name_data)))

        # Write tile name map.
        fp.write(tile_name_data)
        for index, name_offset in tile_name_offsets:
            fp.write(u16le(index))
            fp.write(u16le(name_offset))

        # Write map data.
        for index in map_data:
            fp.write(u16le(index))

    #
    # Write depfile.
    #

    if dep_path:
        os.makedirs(dep_dir, mode=0o755, exist_ok=True)
        with open(dep_path, "w") as fp:
            fp.write("{}: {} {}\n".format(ninja_unescape(os.path.relpath(out_path, dep_base)),
                                          ninja_unescape(os.path.relpath(in_path, dep_base)),
                                          ninja_unescape(os.path.relpath(tileset_path, dep_base))))
