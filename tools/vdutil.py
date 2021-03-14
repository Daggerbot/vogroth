# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

#
# vdutil.py
#   Misc utilities used by other Python scripts in Vogroth Dawn.

def u8(n):
    assert type(n) is int and n >= 0 and n <= 0xFF
    return bytearray([n])

def u16le(n):
    assert type(n) is int and n >= 0 and n <= 0xFFFF
    return bytearray([n & 0xFF, n >> 8])

def u32le(n):
    assert type(n) is int and n >= 0 and n <= 0xFFFFFFFF
    return bytearray([n & 0xFF, (n >> 8) & 0xFF, (n >> 16) & 0xFF, n >> 24])
