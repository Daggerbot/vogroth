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

#ifndef INCLUDED_RW_H
#define INCLUDED_RW_H

#include "types.h"

struct buf;
struct rw;
struct zip;

typedef int(*rw_close_t)(struct rw *rw);
typedef size_t(*rw_read_t)(struct rw *rw, size_t size, void *buf);
typedef size_t(*rw_write_t)(struct rw *rw, size_t size, const void *buf);
typedef int(*rw_flush_t)(struct rw *rw);

struct rw {
    void *data;
    rw_close_t close;
    rw_read_t read;
    rw_write_t write;
    rw_flush_t flush;
    bool eof;
    char *error;
    intptr_t extra[];
};

int rw_close(struct rw *rw, char **out_err);
size_t rw_read(struct rw *rw, size_t size, void *buf);
size_t rw_read_all(struct rw *rw, size_t size, void *buf); /* Calls the inner read() function in a loop */
size_t rw_read_to_buf(struct rw *rw, size_t size, struct buf *buf);
size_t rw_write(struct rw *rw, size_t size, const void *buf);
int rw_flush(struct rw *rw);

struct rw *rw_fopen(const char *path, const char *mode, char **out_err);
struct rw *rw_zip_fopen(struct zip *zip, const char *name, char **out_err);

#endif /* INCLUDED_RW_H */
