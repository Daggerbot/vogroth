# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

cmake_minimum_required(VERSION "3.16")

if(NOT TARGET "libzip::zip")
    include("FindPackageHandleStandardArgs")
    find_package("ZLIB")

    unset(LIBZIP_FOUND)
    unset(LIBZIP_VERSION)
    unset(LIBZIP_VERSION_MAJOR)
    unset(LIBZIP_VERSION_MINOR)
    unset(LIBZIP_VERSION_PATCH)
    unset(LIBZIP_INCLUDE_DIR)
    unset(LIBZIP_LIBRARY)

    find_path(LIBZIP_INCLUDE_DIR "zipconf.h"
        HINTS "$ENV{LIBZIPDIR}"
        PATHS "${LIBZIP_PREFIX}"
        PATH_SUFFIXES "include")

    find_library(LIBZIP_LIBRARY "zip"
        HINTS "$ENV{LIBZIPDIR}"
        PATHS "${LIBZIP_PREFIX}")

    if(LIBZIP_INCLUDE_DIR)
        file(READ "${LIBZIP_INCLUDE_DIR}/zipconf.h" _LIBZIP_ZIPCONF_H)
        string(REGEX MATCH "#[ \\t]*define[ \\t]+LIBZIP_VERSION_MAJOR[ \\t]+([0-9]+)" _LIBZIP_VERSION_MAJOR "${_LIBZIP_ZIPCONF_H}")
        string(REGEX MATCH "#[ \\t]*define[ \\t]+LIBZIP_VERSION_MINOR[ \\t]+([0-9]+)" _LIBZIP_VERSION_MINOR "${_LIBZIP_ZIPCONF_H}")
        string(REGEX MATCH "#[ \\t]*define[ \\t]+LIBZIP_VERSION_MICRO[ \\t]+([0-9]+)" _LIBZIP_VERSION_PATCH "${_LIBZIP_ZIPCONF_H}")

        if(_LIBZIP_VERSION_MAJOR AND _LIBZIP_VERSION_MINOR AND _LIBZIP_VERSION_PATCH)
            string(REGEX REPLACE "#[ \\t]*define[ \\t]+LIBZIP_VERSION_MAJOR[ \\t]+([0-9]+)" "\\1" _LIBZIP_VERSION_MAJOR "${_LIBZIP_VERSION_MAJOR}")
            string(REGEX REPLACE "#[ \\t]*define[ \\t]+LIBZIP_VERSION_MINOR[ \\t]+([0-9]+)" "\\1" _LIBZIP_VERSION_MINOR "${_LIBZIP_VERSION_MINOR}")
            string(REGEX REPLACE "#[ \\t]*define[ \\t]+LIBZIP_VERSION_MICRO[ \\t]+([0-9]+)" "\\1" _LIBZIP_VERSION_PATCH "${_LIBZIP_VERSION_PATCH}")
            set(LIBZIP_VERSION_MAJOR "${_LIBZIP_VERSION_MAJOR}" CACHE INTERNAL "")
            set(LIBZIP_VERSION_MINOR "${_LIBZIP_VERSION_MINOR}" CACHE INTERNAL "")
            set(LIBZIP_VERSION_PATCH "${_LIBZIP_VERSION_PATCH}" CACHE INTERNAL "")
            set(LIBZIP_VERSION "${LIBZIP_VERSION_MAJOR}.${LIBZIP_VERSION_MINOR}.${LIBZIP_VERSION_PATCH}" CACHE INTERNAL "")
        endif()

        unset(_LIBZIP_ZIPCONF_H)
        unset(_LIBZIP_VERSION_MAJOR)
        unset(_LIBZIP_VERSION_MINOR)
        unset(_LIBZIP_VERSION_PATCH)
    endif()

    if(LIBZIP_VERSION AND LIBZIP_LIBRARY AND ZLIB_FOUND)
        set(LIBZIP_FOUND ON CACHE INTERNAL "")
        add_library("libzip::zip" INTERFACE IMPORTED GLOBAL)
        target_include_directories("libzip::zip" SYSTEM INTERFACE ${LIBZIP_INCLUDE_DIR})
        target_link_libraries("libzip::zip" INTERFACE ${LIBZIP_LIBRARY} "ZLIB::ZLIB")
    endif()

    find_package_handle_standard_args("libzip"
        REQUIRED_VARS LIBZIP_LIBRARY LIBZIP_FOUND
        VERSION_VAR LIBZIP_VERSION)
endif()
