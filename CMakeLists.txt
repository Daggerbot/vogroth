# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION "3.16")
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
project("vogroth" C)

include("GNUInstallDirs")

if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
    message(FATAL "Ninja generator is required")
endif()

set(GDB "gdb" CACHE STRING "GNU debugger command")
set(NINJA "${CMAKE_MAKE_PROGRAM}" CACHE STRING "Ninja build command")
set(PYTHON "python3" CACHE STRING "Command for running Python scripts")
set(VALGRIND "valgrind" CACHE STRING "Command for debugging memory")

set(TOP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

#
# Included Conan packages
#

if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
    include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
    conan_basic_setup(NO_OUTPUT_DIRS SKIP_FPIC SKIP_STD)
endif()

#
# Set common compiler flags
#

unset(COMMON_C_FLAGS)
unset(COMMON_DEFINITIONS)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(COMMON_C_FLAGS
        "-std=c11"
        "-pedantic"
        "-Wall"
        "-Wconversion"
        "-Wextra"
    )

    if(CMAKE_GENERATOR STREQUAL "Ninja")
        list(APPEND COMMON_C_FLAGS "-fdiagnostics-color=always")
    endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    set(COMMON_C_FLAGS
        "-std=c11"
        "-pedantic"
        "-Wall"
        "-Wconversion"
        "-Wextra"
    )

    if(CMAKE_GENERATOR STREQUAL "Ninja")
        list(APPEND COMMON_C_FLAGS "-fcolor-diagnostics")
    endif()
endif()

#
# Configure target platform
#

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND COMMON_DEFINITIONS
        "_UNICODE"
        "_WIN32"
        "_WIN32_WINNT=0x0400"
        "_WINDOWS"
        "UNICODE"
        "WINDOWS"
        "WINVER=0x0400"
    )
endif()

#
# Build targets
#

add_subdirectory("vogroth")
add_subdirectory("assets")

#
# Test run
#

set(RUN_ARGS
    "-assets" "assets/vogroth.pkz"
)
set(RUN_COMMAND "$<TARGET_FILE:vogroth>" ${RUN_ARGS})

if(CMAKE_GENERATOR STREQUAL "Ninja" OR CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    add_custom_target("run"
        COMMAND ${RUN_COMMAND}
        DEPENDS "vogroth" "assets"
        USES_TERMINAL)

    add_custom_target("debug"
        COMMAND ${GDB} "--args" ${RUN_COMMAND}
        DEPENDS "vogroth" "assets"
        USES_TERMINAL)

    add_custom_target("valgrind"
        COMMAND ${VALGRIND} ${RUN_COMMAND}
        DEPENDS "vogroth" "assets"
        USES_TERMINAL)
endif()

#
# Install documentation
#

install(
    FILES "CREDITS.txt"
          "LICENSE-MPL-2.0.txt"
    DESTINATION "${CMAKE_INSTALL_FULL_DOCDIR}")
