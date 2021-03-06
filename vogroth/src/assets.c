/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <errno.h>
#include <string.h>

#include <zip.h>

#include "assets.h"
#include "debug.h"
#include "system.h"

static struct zip *zip = NULL;

void assets_init(const char *path)
{
    FILE *fp;
    struct zip_error zerr = {0};
    struct zip_source *source;

    if (zip) {
        return;
    }
    if (!path) {
        path = system_get_default_assets_path();
    }
    LOG_DEBUG("Loading assets from: %s", path);

    fp = system_fopen(path, "rb");
    if (!fp) {
        FATAL("%s: %s", path, strerror(errno));
    }

    zip_error_init(&zerr);
    source = zip_source_filep_create(fp, 0, -1, &zerr);
    if (!source) {
        FATAL("%s: %s", path, zip_error_strerror(&zerr));
    }

    zip = zip_open_from_source(source, ZIP_RDONLY, &zerr);
    if (!zip) {
        FATAL("%s: %s", path, zip_error_strerror(&zerr));
    }
}

void assets_fini(void)
{
    if (zip) {
        if (zip_close(zip)) {
            LOG_ERROR("zip_close failed");
        }
        zip = NULL;
    }
}

struct rw *assets_open(const char *name, char **out_err)
{
    return rw_zip_fopen(zip, name, out_err);
}
