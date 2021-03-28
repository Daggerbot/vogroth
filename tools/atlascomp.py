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

# atlascomp.py
#   Compiles multiple images into one.

import os.path
import sys

import PIL.Image

from vdutil import *

__all__ = [
    "compile",
    "serialize",
]

MAX_ATLAS_SIZE = 4096
ROW_ALIGNMENT = 4

MODE_NUMS = { # (format, bytes_per_pel)
    "RGB": (3, 3),
    "RGBA": (4, 4),
}

#
# Keeps an image and caches a hash code of it so we can more quickly determine
# that two images are not identical. Only accurate as long as the underlying
# image is not modified.
#
class HashedImage:
    def __init__(self, image):
        assert isinstance(image, PIL.Image.Image)

        self.image = image
        self.hash = 0
        self.width, self.height = image.size

        if image.mode == "RGB":
            for y in range(self.height):
                for x in range(self.width):
                    r, g, b = image.getpixel((x, y))
                    h = (r * 111 + g) * 111 + b
                    self.hash = (self.hash * 65599 + h) & 0xFFFFFFFF
        elif image.mode == "RGBA":
            for y in range(self.height):
                for x in range(self.width):
                    r, g, b, a = image.getpixel((x, y))
                    h = ((r * 111 + g) * 111 + b) * 111 + a
                    self.hash = (self.hash * 65599 + h) & 0xFFFFFFFF
        else:
            raise Exception("Unsupported pixel format")

    #
    # Sort by height, then width, then hash code, then contents.
    #
    def compare(self, other):
        assert type(other) is HashedImage
        assert self.image.mode == other.image.mode

        if self.height < other.height:
            return -1
        elif self.height > other.height:
            return 1
        elif self.width < other.width:
            return -1
        elif self.width > other.width:
            return 1
        elif self.hash < other.hash:
            return -1
        elif self.hash > other.hash:
            return 1
        else:
            assert self.image.mode == other.image.mode
            if self.image.mode == "RGB":
                for y in range(self.height):
                    for x in range(self.width):
                        xr, xg, xb = self.image.getpixel((x, y))
                        yr, yg, yb = other.image.getpixel((x, y))
                        if xr < yr:
                            return -1
                        elif xr > yr:
                            return 1
                        elif xg < yg:
                            return -1
                        elif xg > yg:
                            return 1
                        elif xb < yb:
                            return -1
                        elif xb > yb:
                            return 1
            elif self.image.mode == "RGBA":
                for y in range(self.height):
                    for x in range(self.width):
                        xr, xg, xb, xa = self.image.getpixel((x, y))
                        yr, yg, yb, ya = other.image.getpixel((x, y))
                        if xr < yr:
                            return -1
                        elif xr > yr:
                            return 1
                        elif xg < yg:
                            return -1
                        elif xg > yg:
                            return 1
                        elif xb < yb:
                            return -1
                        elif xb > yb:
                            return 1
                        elif xa < ya:
                            return -1
                        elif xa > ya:
                            return 1
            # Both images are identical.
            return 0

#
# Subimage compiled into an atlas.
#

class Subimage:
    x = 0
    y = 0
    width = 0
    height = 0
    offset_x = 0
    offset_y = 0
    original_width = 0
    original_height = 0

#
# Contains the state of atlas compilation.
#
class AtlasCompiler:
    #
    # Builds a list of source images sorted per the HashedImage.compare() method.
    #
    def __init__(self, sources, *, uniform=False):
        assert type(sources) is list and len(sources) > 0

        self.subimages = []
        self.image_map = []
        self.mode = None

        size = None

        for source_index in range(len(sources)):
            source = sources[source_index]

            if type(source) is str:
                source = PIL.Image.open(source)
            else:
                assert isinstance(source, PIL.Image.Image)

            subimage = Subimage()
            subimage.width, subimage.height = source.size
            subimage.offset_x = 0
            subimage.offset_y = 0
            subimage.original_width, subimage.original_height = source.size
            self.subimages.append(subimage)

            if self.mode is None:
                self.mode = source.mode
                assert self.mode == "RGB" or self.mode == "RGBA"
            else:
                assert source.mode == self.mode

            if uniform:
                if size is None:
                    size = source.size
                else:
                    assert source.size == size

            image = HashedImage(source)
            image_inserted = False

            for i in range(len(self.image_map)):
                order = image.compare(self.image_map[i][0])
                if order < 0:
                    self.image_map.insert(i, (image, [source_index]))
                    image_inserted = True
                    break
                elif order == 0:
                    # The images are identical; consolidate duplicates
                    existing_image, exiting_source_indices = self.image_map[i]
                    existing_source_indices.append(source_index)
                    image_inserted = True
                    break

            if not image_inserted:
                self.image_map.append((image, [source_index]))

    #
    # Generates the layout of the compiled atlas.
    #
    def generate_geometry(self):
        dimension = 1
        while True:
            if self.try_generate_geometry((dimension, dimension)):
                break
            else:
                dimension *= 2
                assert dimension <= MAX_ATLAS_SIZE

    #
    # Attempts to generate the layout of the compiled atlas with the specified size.
    # Returns True on success or False on failure.
    #
    def try_generate_geometry(self, atlas_size):
        self.atlas_width, self.atlas_height = atlas_size
        self.cropped_width = 0
        self.cropped_height = 0
        self.columns = [0] * self.atlas_width # Minimum available y-coordinate for each column
        self.pen_x = 0

        for image, source_indices in self.image_map:
            rect = self.try_alloc_slot((image.width, image.height))
            if rect == None:
                return False
            else:
                for i in source_indices:
                    subimage = self.subimages[i]
                    subimage.x, subimage.y, subimage.width, subimage.height = rect

        return True

    #
    # Attempts to allocate a slot for an image of the specified size.
    # Returns a rectangle (x, y, w, h) on success, or None on failure.
    #
    def try_alloc_slot(self, size):
        width, height = size
        start_x = self.pen_x

        while True:
            if self.atlas_width - self.pen_x >= width:
                # The current position is far enough from the right side of the atlas.
                # Determine how high up we can push the image.
                y = 0
                for x in range(width):
                    y = max(y, self.columns[self.pen_x + x])

                if self.atlas_height - y >= height:
                    # The current position is far enough from the bottom of the atlas.
                    # Reserve space and return the rect.
                    for x in range(width):
                        self.columns[self.pen_x + x] = y + height

                    self.cropped_width = max(self.cropped_width, self.pen_x + width)
                    self.cropped_height = max(self.cropped_height, y + height)

                    x = self.pen_x
                    self.pen_x = (self.pen_x + width) % self.atlas_width
                    return (x, y, width, height)

            self.pen_x = (self.pen_x + 1) % self.atlas_width
            if self.pen_x == start_x:
                # We're right back where we started and haven't found a suitable spot.
                return None

    #
    # Generates the atlas image from the generated geometry.
    #
    def generate_atlas(self):
        atlas = PIL.Image.new(self.mode, (self.cropped_width, self.cropped_height))

        for image, source_indices in self.image_map:
            subimage = self.subimages[source_indices[0]]
            for y in range(subimage.height):
                for x in range(subimage.width):
                    atlas.putpixel((subimage.x + x, subimage.y + y), image.image.getpixel((x, y)))

        return atlas

#
# Compiles an image atlas.
# Returns (image, subimages).
#
def compile(sources, *, uniform=False):
    compiler = AtlasCompiler(sources, uniform=uniform)
    compiler.generate_geometry()
    image = compiler.generate_atlas()
    return image, compiler.subimages

#
# Writes an image atlas to a file.
#
def serialize(fp, data, *, uniform=False):
    if isinstance(fp, str):
        with open(fp, "wb") as _fp:
            return serialize(fp, data)

    image, subimages = data
    width, height = image.size
    tile_width = subimages[0].original_width
    tile_height = subimages[0].original_height

    # Write atlas header
    fp.write(u16le(width))
    fp.write(u16le(height))
    fp.write(u16le(MODE_NUMS[image.mode][0]))
    bpp = MODE_NUMS[image.mode][1]

    if uniform:
        fp.write(u16le(tile_width))
        fp.write(u16le(tile_height))

    fp.write(u16le(len(subimages)))

    # Write image data
    padding = bytearray([0] * ((ROW_ALIGNMENT - width * bpp % ROW_ALIGNMENT) % ROW_ALIGNMENT))
    fp.write(u32le(height * (width * bpp + len(padding))))

    for y in range(height):
        for x in range(width):
            fp.write(bytearray(image.getpixel((x, y))))
        fp.write(padding)

    # Write tile data
    for subimage in subimages:
        fp.write(u16le(subimage.x))
        fp.write(u16le(subimage.y))
        if uniform:
            assert subimage.width == tile_width
            assert subimage.height == tile_height
        else:
            fp.write(u16le(subimage.width))
            fp.write(u16le(subimage.height))
