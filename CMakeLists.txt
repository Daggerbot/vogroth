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

cmake_minimum_required(VERSION "3.16")
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
project("vogroth" C)

include("GNUInstallDirs")

set(GDB "gdb" CACHE STRING "GNU debugger command")
set(PYTHON "python3" CACHE STRING "Command for running Python scripts")
set(VALGRIND "valgrind" CACHE STRING "Command for debugging memory")

set(TOP_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(VOGROTH_INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}")
set(VOGROTH_INSTALL_DATADIR "${CMAKE_INSTALL_DATADIR}/vogroth")
set(VOGROTH_INSTALL_DOCDIR "${CMAKE_INSTALL_DOCDIR}")

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.20")
    cmake_policy(SET CMP0116 OLD)
endif()

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
    set(VOGROTH_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}")
    set(VOGROTH_INSTALL_DATADIR "${CMAKE_INSTALL_PREFIX}")
    set(VOGROTH_INSTALL_DOCDIR "${CMAKE_INSTALL_PREFIX}")
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
          "LICENSE-CC-BY-SA-4.0.txt"
          "LICENSE-GPL-3.0.txt"
    DESTINATION "${VOGROTH_INSTALL_DOCDIR}")
