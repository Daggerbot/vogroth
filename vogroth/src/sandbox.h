/*
 * Copyright (c) 2021 Marty Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_SANDBOX_H
#define INCLUDED_SANDBOX_H

#include "types.h"

void sandbox_init(void);
void sandbox_fini(void);
void sandbox_render(void);

#endif /* INCLUDED_SANDBOX_H */
