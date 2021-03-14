# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set(ASSETS_BUILD_FILE "${CMAKE_CURRENT_BINARY_DIR}/assets.ninja")
set(ASSETS_PACKAGE "${CMAKE_CURRENT_BINARY_DIR}/vogroth.pkz")
set(ASSETS_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/assets.yaml")

add_custom_command(
    OUTPUT "${ASSETS_BUILD_FILE}"
    COMMAND "${PYTHON}" "${TOP_SOURCE_DIR}/tools/assettool.py"
            "-i" "${CMAKE_CURRENT_BINARY_DIR}"
            "-o" "${ASSETS_PACKAGE}"
            "-p" "${PYTHON}"
            "-t" "${TOP_SOURCE_DIR}/tools"
            "${ASSETS_SOURCE_FILE}"
            "${ASSETS_BUILD_FILE}"
    DEPENDS "${ASSETS_SOURCE_FILE}"
            "${TOP_SOURCE_DIR}/tools/assettool.py")

add_custom_target("assets" ALL
    COMMAND "${NINJA}" "-f" "${ASSETS_BUILD_FILE}"
    DEPENDS "${ASSETS_BUILD_FILE}")
