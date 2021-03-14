#!/usr/bin/python3
# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# pkz.py
#   Generates a zip archive from some inputs.
#
# Usage:
#   pkz.py -o OUTFILE INFILES...
#
# INFILE syntax: [[!][NAME]=]PATH

import getopt
import os
import os.path
import sys
import zipfile

__all__ = []

DEFAULT_COMPRESS_TYPE = zipfile.ZIP_DEFLATED
DEFAULT_COMPRESS_LEVEL = 9

class Input:
    def __init__(self, string):
        self.compress_type = DEFAULT_COMPRESS_TYPE
        self.compress_level = DEFAULT_COMPRESS_LEVEL

        index = string.find('=')
        if index == -1:
            self.name = string
            self.path = string
        else:
            self.name = string[:index]
            self.path = string[index+1:]

            if self.name.startswith("!"):
                self.compress_type = None
                self.compress_level = None
                self.name = self.name[1:]

            if len(self.name) == 0:
                self.name = self.path

if __name__ == "__main__":
    #
    # Parse command line args
    #

    inputs = None
    out_path = None

    opts, args = getopt.getopt(sys.argv[1:], "o:")

    for opt, param in opts:
        if opt == "-o":
            assert out_path is None
            out_path = param

    assert not out_path is None
    assert len(args) > 0
    inputs = [Input(s) for s in args]

    #
    # Make sure the input files are readable
    #

    for inp in inputs:
        assert os.access(inp.path, os.R_OK)

    #
    # Make the output archive
    #

    out_dir = os.path.dirname(os.path.realpath(out_path))
    os.makedirs(out_dir, mode=0o755, exist_ok=True)

    with zipfile.ZipFile(out_path, mode="w") as archive:
        for inp in inputs:
            archive.write(inp.path, inp.name, inp.compress_type, inp.compress_level)
