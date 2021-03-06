#!/usr/bin/python3
# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import getopt
import os
import os.path
import sys
import yaml
import zipfile

DEFAULT_COMPRESSION_TYPE = zipfile.ZIP_DEFLATED
DEFAULT_COMPRESSION_LEVEL = 9

#
# Prints a formatted string to stderr.
#

def eprintf(fmt, *args, **kwargs):
    print(fmt.format(*args), file=sys.stderr, **kwargs)

#
# Shows an error message and exits.
#

def error(fmt, *args, usage=False):
    eprintf("error: " + fmt, *args)
    if usage:
        eprintf("usage: {} -o <output-file> <input-file>", sys.argv[0])
        eprintf("options:")
        eprintf("  -d DIR   Set intermediate directory")
        eprintf("  -o FILE  Set output path")
    exit(1)

#
# Shows a warning message and exits.
#

def warn(fmt, *args):
    eprintf("warning: " + fmt, *args)

#
# File entry in the final assets package
#

class Output:
    def __init__(self, name):
        self.name = name
        self.path = None
        self.compression_type = None
        self.compression_level = None

#
# Entry point
#

if __name__ == "__main__":
    #
    # Handle command line args
    #

    input_path = None
    output_path = None
    intermediate_dir = None

    try:
        opts, args = getopt.getopt(sys.argv[1:], "d:o:")
    except getopt.GetoptError as e:
        error("{}", str(e), usage=True)

    for opt, param in opts:
        if opt == "-d":
            if not intermediate_dir is None:
                error("Multiple intermediate directories specified", usage=True)
            intermediate_dir = param
        elif opt == "-o":
            if not output_path is None:
                error("Multiple output paths specified", usage=True)
            output_path = param
        else:
            error("Unimplemented option: {}", opt)

    if len(args) == 0:
        error("Missing input path", usage=True)
    elif len(args) > 1:
        error("Multiple input paths specified", usage=True)

    input_path = args[0]
    input_dir = os.path.dirname(os.path.realpath(input_path))

    if output_path is None:
        error("Missing output path", usage=True)
    if intermediate_dir is None:
        intermediate_dir = "."

    #
    # Parse assets makefile
    #

    with open(input_path, "r") as fp:
        makefile = yaml.load(fp, yaml.SafeLoader)
        assert type(makefile) is dict

    outputs = {}

    for entry in makefile["assets"]:
        if entry["name"] in outputs:
            error("{}: Duplicate asset name: {}", input_path, entry["name"])

        output = Output(entry["name"])

        if "type" in entry:
            error("{}: Asset types are not implemented", input_path)
        else:
            output.path = os.path.join(input_dir, output.name)
            output.compression_type = DEFAULT_COMPRESSION_TYPE
            output.compression_level = DEFAULT_COMPRESSION_LEVEL

        outputs[output.name] = output

    #
    # Build intermediate files
    #

    need_rebuild = False

    #
    # Build final package
    #

    if not need_rebuild:
        try:
            makefile_mtime = os.path.getmtime(input_path)
            output_mtime = os.path.getmtime(output_path)
            if makefile_mtime > output_mtime:
                need_rebuild = True

            for output in outputs.values():
                if os.path.getmtime(output.path) > output_mtime:
                    need_rebuild = True
                    break
        except FileNotFoundError:
            need_rebuild = True

    if need_rebuild:
        os.makedirs(os.path.dirname(os.path.realpath(output_path)), mode=0o755, exist_ok=True)
        with zipfile.ZipFile(output_path, mode="w") as zfp:
            for output in outputs.values():
                zfp.write(output.path,
                          arcname=output.name,
                          compress_type=output.compression_type,
                          compresslevel=output.compression_level)
