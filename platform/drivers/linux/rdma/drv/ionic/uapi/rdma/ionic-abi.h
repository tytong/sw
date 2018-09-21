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

#ifndef IONIC_ABI_H
#define IONIC_ABI_H

#include <linux/types.h>

/* XXX make abi version 1 for release */
#define IONIC_ABI_VERSION	5

struct ionic_ctx_req {
	__u32 fallback;
	__u32 rsvd;
};

struct ionic_ctx_resp {
	__u32 fallback;
	__u32 page_shift;

	__aligned_u64 dbell_offset;

	__u16 version;
	__u8 qp_opcodes[7];
	__u8 admin_opcodes[7];

	__u8 sq_qtype;
	__u8 rq_qtype;
	__u8 cq_qtype;
	__u8 admin_qtype;

	__u8 max_stride;
	__u8 rsvd[3];
};

struct ionic_qdesc {
	__aligned_u64 addr;
	__u32 size;
	__u16 mask;
	__u8 depth_log2;
	__u8 stride_log2;
};

struct ionic_ah_resp {
	__u32 ahid;
};

struct ionic_cq_req {
	struct ionic_qdesc cq;
	__u8 compat;
	__u8 rsvd[3];
};

struct ionic_cq_resp {
	__u32 cqid;
};

struct ionic_qp_req {
	struct ionic_qdesc sq;
	struct ionic_qdesc rq;
	__u8 compat;
	__u8 rsvd[3];
};

struct ionic_qp_resp {
	__u32 qpid;
	__u32 rsvd;
	__aligned_u64 sq_cmb_offset;
};

struct ionic_srq_req {
	struct ionic_qdesc rq;
	__u8 compat;
	__u8 rsvd[3];
};

struct ionic_srq_resp {
	__u32 qpid;
};

#endif /* IONIC_ABI_H */
