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

set(RAW_ASSETS
    "shaders/glsl110/sprites.vert"
    "shaders/glsl110/sprites_alpha.frag"
    "shaders/glsl110/sprites_rgb.frag"
    "shaders/glsl110/sprites_rgba.frag"
)
set(MAPS
    "maps/test.json"
)
set(TILESETS
    "tiles/tileset.json"
)

#
# Compile intermediate assets.
#

set(MAP_TOOL "${TOP_SOURCE_DIR}/tools/mapcomp.py")
set(PACKAGE_TOOL "${TOP_SOURCE_DIR}/tools/pkz.py")
set(TILESET_TOOL "${TOP_SOURCE_DIR}/tools/tilesetcomp.py")

set(PACKAGE "vogroth.pkz")
set(PACKAGE_COMMAND ${PYTHON} ${PACKAGE_TOOL} "-o" ${PACKAGE})
set(PACKAGE_DEPENDS ${PACKAGE_TOOL})

foreach(FILE ${RAW_ASSETS})
    list(APPEND PACKAGE_COMMAND "${FILE}=${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
    list(APPEND PACKAGE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
endforeach()

foreach(FILE ${MAPS})
    get_filename_component(_DIR ${FILE} DIRECTORY)
    get_filename_component(_NAME ${FILE} NAME_WLE)
    set(_COMPILED "${_DIR}/${_NAME}.x")
    set(_DEPFILE "${_COMPILED}.d")
    list(APPEND PACKAGE_COMMAND "${_COMPILED}")
    list(APPEND PACKAGE_DEPENDS "${_COMPILED}")

    add_custom_command(
        OUTPUT ${_COMPILED}
        COMMAND ${PYTHON} ${MAP_TOOL} "-d" ${_DEPFILE} "-R" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}" ${_COMPILED}
        DEPENDS ${FILE} ${MAP_TOOL}
        DEPFILE "${CMAKE_CURRENT_BINARY_DIR}/${_DEPFILE}")

    unset(_COMPILED)
    unset(_DEPFILE)
    unset(_DIR)
    unset(_NAME)
endforeach()

foreach(FILE ${TILESETS})
    get_filename_component(_DIR ${FILE} DIRECTORY)
    get_filename_component(_NAME ${FILE} NAME_WLE)
    set(_COMPILED "${_DIR}/${_NAME}.x")
    set(_DEPFILE "${_COMPILED}.d")
    list(APPEND PACKAGE_COMMAND "${_COMPILED}")
    list(APPEND PACKAGE_DEPENDS "${_COMPILED}")

    add_custom_command(
        OUTPUT ${_COMPILED}
        COMMAND ${PYTHON} ${TILESET_TOOL} "-d" ${_DEPFILE} "-R" "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}" ${_COMPILED}
        DEPENDS ${FILE} ${TILESET_TOOL}
        DEPFILE "${CMAKE_CURRENT_BINARY_DIR}/${_DEPFILE}")

    unset(_COMPILED)
    unset(_DEPFILE)
    unset(_DIR)
    unset(_NAME)
endforeach()

#
# Build final asset package.
#

add_custom_command(
    OUTPUT ${PACKAGE}
    COMMAND ${PACKAGE_COMMAND}
    DEPENDS ${PACKAGE_DEPENDS})

add_custom_target("assets" ALL DEPENDS ${PACKAGE})
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}" DESTINATION "${VOGROTH_INSTALL_DATADIR}")
