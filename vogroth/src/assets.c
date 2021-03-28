/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
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
