/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_ASSETS_H
#define INCLUDED_ASSETS_H

#include "rw.h"

void assets_init(const char *path); /* path can be null to use default */
void assets_fini(void);
struct rw *assets_open(const char *name, char **out_err);

#endif /* INCLUDED_ASSETS_H */
