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

if(NOT TARGET "SDL2::SDL2")
    include("FindPackageHandleStandardArgs")

    unset(SDL2_FOUND)
    unset(SDL2_main_FOUND)
    unset(SDL2_VERSION)
    unset(SDL2_VERSION_MAJOR)
    unset(SDL2_VERSION_MINOR)
    unset(SDL2_VERSION_PATCH)
    unset(SDL2_INCLUDE_DIR)
    unset(SDL2_LIBRARY)
    unset(SDL2_main_LIBRARY)

    find_path(SDL2_INCLUDE_DIR "SDL_version.h"
        HINTS "$ENV{SDL2DIR}"
        PATHS "${SDL2_PREFIX}"
        PATH_SUFFIXES "include/SDL2" "include")

    find_library(SDL2_LIBRARY "SDL2"
        HINTS "$ENV{SDL2DIR}"
        PATHS "${SDL2_PREFIX}")

    find_library(SDL2_main_LIBRARY "SDL2main"
        HINTS "$ENV{SDL2DIR}"
        PATHS "${SDL2_PREFIX}")

    if(SDL2_INCLUDE_DIR)
        file(READ "${SDL2_INCLUDE_DIR}/SDL_version.h" _SDL2_VERSION_H)
        string(REGEX MATCH "#[ \\t]*define[ \\t]+SDL_MAJOR_VERSION[ \\t]+([0-9]+)" _SDL2_VERSION_MAJOR "${_SDL2_VERSION_H}")
        string(REGEX MATCH "#[ \\t]*define[ \\t]+SDL_MINOR_VERSION[ \\t]+([0-9]+)" _SDL2_VERSION_MINOR "${_SDL2_VERSION_H}")
        string(REGEX MATCH "#[ \\t]*define[ \\t]+SDL_PATCHLEVEL[ \\t]+([0-9]+)" _SDL2_VERSION_PATCH "${_SDL2_VERSION_H}")

        if(_SDL2_VERSION_MAJOR AND _SDL2_VERSION_MINOR AND _SDL2_VERSION_PATCH)
            string(REGEX REPLACE "#[ \\t]*define[ \\t]+SDL_MAJOR_VERSION[ \\t]+([0-9]+)" "\\1" _SDL2_VERSION_MAJOR "${_SDL2_VERSION_MAJOR}")
            string(REGEX REPLACE "#[ \\t]*define[ \\t]+SDL_MINOR_VERSION[ \\t]+([0-9]+)" "\\1" _SDL2_VERSION_MINOR "${_SDL2_VERSION_MINOR}")
            string(REGEX REPLACE "#[ \\t]*define[ \\t]+SDL_PATCHLEVEL[ \\t]+([0-9]+)" "\\1" _SDL2_VERSION_PATCH "${_SDL2_VERSION_PATCH}")
            if(_SDL2_VERSION_MAJOR STREQUAL "2")
                set(SDL2_VERSION_MAJOR "${_SDL2_VERSION_MAJOR}" CACHE INTERNAL "")
                set(SDL2_VERSION_MINOR "${_SDL2_VERSION_MINOR}" CACHE INTERNAL "")
                set(SDL2_VERSION_PATCH "${_SDL2_VERSION_PATCH}" CACHE INTERNAL "")
                set(SDL2_VERSION "${SDL2_VERSION_MAJOR}.${SDL2_VERSION_MINOR}.${SDL2_VERSION_PATCH}" CACHE INTERNAL "")
            endif()
        endif()

        unset(_SDL2_VERSION_H)
        unset(_SDL2_VERSION_MAJOR)
        unset(_SDL2_VERSION_MINOR)
        unset(_SDL2_VERSION_PATCH)
    endif()

    if(SDL2_VERSION AND SDL2_LIBRARY)
        set(SDL2_FOUND ON CACHE INTERNAL "")
        add_library("SDL2::SDL2" INTERFACE IMPORTED GLOBAL)
        target_include_directories("SDL2::SDL2" SYSTEM INTERFACE "${SDL2_INCLUDE_DIR}")
        target_link_libraries("SDL2::SDL2" INTERFACE "${SDL2_LIBRARY}")
    endif()

    if(SDL2_VERSION AND SDL2_main_LIBRARY)
        set(SDL2_main_FOUND ON CACHE INTERNAL "")
        add_library("SDL2::main" INTERFACE IMPORTED GLOBAL)
        target_link_libraries("SDL2::main" INTERFACE "${SDL2_main_LIBRARY}")
    endif()

    find_package_handle_standard_args("SDL2"
        REQUIRED_VARS SDL2_LIBRARY SDL2_FOUND
        VERSION_VAR SDL2_VERSION
        HANDLE_COMPONENTS)
endif()
