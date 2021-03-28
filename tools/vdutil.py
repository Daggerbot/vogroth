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

def ninja_unescape(s):
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
