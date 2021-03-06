# Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

set(ASSETS_BUILD_TOOL "${TOP_SOURCE_DIR}/tools/pkztool.py")
set(ASSETS_INTERMEDIATE_DIR "obj")
set(ASSETS_MAKEFILE "${CMAKE_CURRENT_SOURCE_DIR}/assets.yaml")
set(ASSETS_PACKAGE "vogroth.pkz")

add_custom_target("assets" ALL
    COMMAND ${PYTHON} ${ASSETS_BUILD_TOOL} "-d" ${ASSETS_INTERMEDIATE_DIR} "-o" ${ASSETS_PACKAGE} ${ASSETS_MAKEFILE}
    BYPRODUCTS ${ASSETS_PACKAGE})

list(APPEND ADDITIONAL_CLEAN_FILES
    ${ASSETS_PACKAGE}
    ${ASSETS_INTERMEDIATE_DIR}
)
