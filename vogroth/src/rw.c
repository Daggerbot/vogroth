/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <zip.h>

#include "debug.h"
#include "memory.h"
#include "rw.h"
#include "system.h"

#undef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))

int rw_close(struct rw *rw, char **out_err)
{
    int result;

    if (!rw) {
        return 0;
    }

    if (rw->close) {
        result = rw->close(rw);
        if (result) {
            DASSERT(rw->error);
            if (out_err) {
                mem_free(*out_err);
                *out_err = rw->error;
                rw->error = NULL;
            } else {
                LOG_ERROR("rw_close: %s", rw->error);
            }
        }
    } else {
        result = 0;
    }

    if (rw->error) {
        mem_free(rw->error);
    }
    mem_free(rw);
    return result;
}

size_t rw_read(struct rw *rw, size_t size, void *buf)
{
    size_t result;

    if (!size) {
        return 0;
    }
    DASSERT(rw && rw->read && buf);
    result = rw->read(rw, size, buf);
    DASSERT(result <= size);
    rw->eof = !result;
    return result;
}

size_t rw_read_all(struct rw *rw, size_t size, void *buf)
{
    size_t result;
    size_t total_read = 0;

    if (!size) {
        return 0;
    }
    DASSERT(rw && rw->read && buf);

    while (size > 0) {
        result = rw->read(rw, size, buf);
        rw->eof = !result;
        if (rw->eof) {
            break;
        }
        DASSERT(result <= size);
        size -= result;
        buf = (char *)buf + result;
        total_read += result;
    }

    return total_read;
}

size_t rw_read_to_buf(struct rw *rw, size_t size, struct buf *buf)
{
    char data[512];
    size_t pass_size;
    size_t result;
    size_t total_read = 0;

    if (!size) {
        return 0;
    }
    DASSERT(rw && rw->read && buf);

    while (size > 0) {
        pass_size = MIN(size, sizeof(data));
        result = rw->read(rw, pass_size, data);
        rw->eof = !result;
        if (rw->eof) {
            break;
        }
        DASSERT(result <= pass_size);
        buf_append(buf, result, data);
        size -= result;
        total_read += size;
    }

    return total_read;
}

size_t rw_write(struct rw *rw, size_t size, const void *buf)
{
    size_t result;

    if (!size) {
        return 0;
    }
    DASSERT(rw && rw->write && buf);
    result = rw->write(rw, size, buf);
    DASSERT(result <= size);
    if (!result) {
        DASSERT(rw->error != NULL);
    }
    return result;
}

int rw_flush(struct rw *rw)
{
    int result;

    DASSERT(rw);
    if (rw->flush) {
        result = rw->flush(rw);
        if (result) {
            DASSERT(rw->error != NULL);
        }
        return result;
    } else {
        return 0;
    }
}

/******************************************************************************/

static int rw_fclose(struct rw *rw)
{
    int result;

    result = fclose(rw->data);
    if (result) {
        str_put(&rw->error, strerror(errno));
    }
    return result;
}

static size_t rw_fread(struct rw *rw, size_t size, void *buf)
{
    size_t result;
    int errcode;

    result = fread(buf, 1, size, rw->data);
    errcode = errno;
    if (!result && ferror(rw->data)) {
        str_put(&rw->error, strerror(errcode));
    }
    return result;
}

static size_t rw_fwrite(struct rw *rw, size_t size, const void *buf)
{
    size_t result;

    result = fwrite(buf, 1, size, rw->data);
    if (!result) {
        str_put(&rw->error, strerror(errno));
    }
    return result;
}

static int rw_fflush(struct rw *rw)
{
    int result;

    result = fflush(rw->data);
    if (result) {
        str_put(&rw->error, strerror(errno));
    }
    return result;
}

struct rw *rw_fopen(const char *path, const char *mode, char **out_err)
{
    FILE *fp;
    struct rw *rw;

    fp = system_fopen(path, mode);
    if (!fp) {
        str_put(out_err, strerror(errno));
        return NULL;
    }

    rw = mem_alloc(sizeof(*rw));
    *rw = (struct rw) {
        .data = fp,
        .close = &rw_fclose,
        .read = &rw_fread,
        .write = &rw_fwrite,
        .flush = &rw_fflush,
    };

    return rw;
}

/******************************************************************************/

static int rw_zip_fclose(struct rw *rw)
{
    int result;

    result = zip_fclose(rw->data);
    if (result) {
        str_put(&rw->error, "zip_close failed");
    }
    return result;
}

static size_t rw_zip_fread(struct rw *rw, size_t size, void *buf)
{
    zip_int64_t result;

    result = zip_fread(rw->data, buf, size);
    if (result < 0) {
        str_put(&rw->error, zip_file_strerror(rw->data));
        return 0;
    }
    return (size_t)result;
}

struct rw *rw_zip_fopen(struct zip *zip, const char *name, char **out_err)
{
    struct zip_file *zfp;
    struct rw *rw;

    DASSERT(zip && name);
    zfp = zip_fopen(zip, name, 0);
    if (!zfp) {
        str_put(out_err, zip_strerror(zip));
        return NULL;
    }

    rw = mem_alloc(sizeof(*rw));
    *rw = (struct rw) {
        .data = zfp,
        .close = &rw_zip_fclose,
        .read = &rw_zip_fread,
    };

    return rw;
}
