/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "assets.h"
#include "debug.h"
#include "game_defs.h"
#include "pixbuf.h"
#include "tiles.h"

#define TILESET_ASSET_NAME "tileset.x"
#define TILESET_SIGNATURE 0xA287F078u

/* Assumed when deserializing */
STATIC_ASSERT(sizeof(uint8_t) == 1);
STATIC_ASSERT(sizeof(uint16_t) == 2);
STATIC_ASSERT(sizeof(uint32_t) == 4);

static struct tileset tileset = {0};

struct tileset *tileset_init(void)
{
    char *err = NULL;
    struct rw *rw;
    uint32_t signature;
    uint8_t file_version;
    uint16_t texture_width, texture_height;
    uint16_t texture_format;
    uint16_t tile_width, tile_height;
    uint16_t tile_count;
    struct pixbuf image = {0};
    uint32_t image_data_size;
    int texture_dimension = 1;

    if (tileset.texture) {
        return &tileset;
    }

    rw = assets_open(TILESET_ASSET_NAME, &err);
    if (!rw) {
        FATAL("%s: %s", TILESET_ASSET_NAME, err);
    }

    /*
     * Read the file header. To support systems with a different byte order
     * from that used in the file format, we'll have to add byte order swapping.
     */
    rw_read_all(rw, 4, &signature);
    rw_read_all(rw, 1, &file_version);
    if (rw->eof) {
        goto io_error;
    } else if (signature != TILESET_SIGNATURE) {
        FATAL("%s: Invalid file signature", TILESET_ASSET_NAME);
    } else if (file_version != 0) {
        FATAL("%s: Invalid file version", TILESET_ASSET_NAME);
    }

    /* Read atlas header. */
    rw_read_all(rw, 2, &texture_width);
    rw_read_all(rw, 2, &texture_height);
    rw_read_all(rw, 2, &texture_format);
    rw_read_all(rw, 2, &tile_width);
    rw_read_all(rw, 2, &tile_height);
    rw_read_all(rw, 2, &tile_count);
    rw_read_all(rw, 4, &image_data_size);
    if (rw->eof) {
        goto io_error;
    } else if (texture_width < 1 || texture_height < 1) {
        FATAL("%s: Invalid texture size", TILESET_ASSET_NAME);
    } else if (tile_width != TILE_WIDTH || tile_height != TILE_HEIGHT) {
        FATAL("%s: Invalid tile size", TILESET_ASSET_NAME);
    } else if (tile_count < 1) {
        FATAL("%s: Empty tileset", TILESET_ASSET_NAME);
    }

    /* Read image data. */
    image.size = (struct vec2i){texture_width, texture_height};
    image.format = (enum pixel_format)texture_format;
    pixbuf_alloc(&image);
    if (image_data_size != image.buf_size) {
        FATAL("%s: Invalid image data size", TILESET_ASSET_NAME);
    }
    rw_read_all(rw, image.buf_size, image.buf);
    if (rw->eof) {
        goto io_error;
    }

    /* Create tileset texture. */
    while (texture_dimension < texture_width || texture_dimension < texture_height) {
        ASSERT(texture_dimension <= INT_MAX / 2);
        texture_dimension *= 2;
    }
    tileset.texture = texture_create((struct vec2i){texture_dimension, texture_dimension}, texture_format);
    texture_upload(tileset.texture, &image, (struct vec2i){0, 0});
    pixbuf_fini(&image);

    /* Success! */
    return &tileset;

io_error:
    if (rw->error) {
        FATAL("%s: %s", TILESET_ASSET_NAME, rw->error);
    } else {
        FATAL("%s: Unexpected end of stream", TILESET_ASSET_NAME);
    }
}

void tileset_fini(void)
{
    if (tileset.texture) {
        texture_destroy(tileset.texture);
    }
    tileset = (struct tileset){0};
}
