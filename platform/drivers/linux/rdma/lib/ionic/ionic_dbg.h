/*
 * Copyright (c) 2018 Pensando Systems, Inc.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef IONIC_DBG_H
#define IONIC_DBG_H

#define IONIC_DEBUG
#define IONIC_DEBUG_FILE stderr

#include "ionic.h"

#ifdef IONIC_DEBUG
#include <stdio.h>
#define ionic_dbg(ctx, fmt, args...) do { \
	if (ctx->dbg_file)					\
		fprintf(ctx->dbg_file, "%s:%d: " fmt "\n",	\
			__func__, __LINE__, ##args);		\
} while (0)
#else
static inline void ionic_dbg(struct ionic_ctx *ctx, const char *fmt, ...)
	__attribute__((format(printf, 2, 3)));
static inline void ionic_dbg(struct ionic_ctx *ctx, const char *fmt, ...) {}
#endif

static inline void ionic_dbg_xdump(struct ionic_ctx *ctx, const char *str,
				   const void *ptr, size_t size)
{
	const uint8_t *ptr8 = ptr;
	int i;

	for (i = 0; i < size; i += 8)
		ionic_dbg(ctx, "%s: %02x %02x %02x %02x %02x %02x %02x %02x",
			  str,
			  ptr8[i + 0], ptr8[i + 1], ptr8[i + 2], ptr8[i + 3],
			  ptr8[i + 4], ptr8[i + 5], ptr8[i + 6], ptr8[i + 7]);
}

#endif
