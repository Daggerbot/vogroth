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

set(VOGROTH_SOURCES
    "src/assets.c"
    "src/debug.c"
    "src/gl_api.c"
    "src/gl_shaders.c"
    "src/gl_state.c"
    "src/main.c"
    "src/memory.c"
    "src/pixbuf.c"
    "src/render.c"
    "src/rw.c"
    "src/sandbox.c"
    "src/sprites.c"
    "src/texture.c"
    "src/vector_math.c"
    "src/video.c"
)
set(VOGROTH_WIN32_SOURCES
    "src/system_win32.c"
    "src/unicode.c"
    "vogroth.rc"
)
set(VOGROTH_UNIX_SOURCES
    "src/system_unix.c"
)

#
# Configure target platform
#

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND VOGROTH_SOURCES ${VOGROTH_WIN32_SOURCES})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(FATAL_ERROR "MacOS is not yet supported")
elseif(UNIX)
    list(APPEND VOGROTH_SOURCES ${VOGROTH_UNIX_SOURCES})
else()
    message(FATAL_ERROR "Unsupported CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
endif()

#
# Find dependencies
#

find_package("libzip" REQUIRED)
find_package("SDL2" REQUIRED)

#
# Build game executable
#

add_executable("vogroth" ${VOGROTH_SOURCES})
target_include_directories("vogroth" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include" "src")
target_link_libraries("vogroth" PRIVATE
    "libzip::zip"
    "SDL2::SDL2"
)

if(COMMON_C_FLAGS)
    target_compile_options("vogroth" PRIVATE $<$<COMPILE_LANGUAGE:C>:${COMMON_C_FLAGS}>)
endif()

if(COMMON_DEFINITIONS)
    target_compile_definitions("vogroth" PRIVATE ${COMMON_DEFINITIONS})
endif()

if(MINGW)
    target_link_options("vogroth" PRIVATE "-mwindows" "-municode")
endif()

install(TARGETS "vogroth")

#
# Generate a build configuration header
#

configure_file("config.h.in" "include/config.h" @ONLY)
