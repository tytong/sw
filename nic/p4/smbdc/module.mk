# {C} Copyright 2018 Pensando Systems Inc. All rights reserved
include ${MKDEFS}/pre.mk
MODULE_TARGET   = smbdc.p4bin
MODULE_PIPELINE = iris gft
MODULE_SRCS     = ${MODULE_SRC_DIR}/smbdc_resp_rxdma.p4 \
                  ${MODULE_SRC_DIR}/smbdc_resp_txdma.p4 \
                  ${MODULE_SRC_DIR}/smbdc_req_rxdma.p4 \
                  ${MODULE_SRC_DIR}/smbdc_req_txdma.p4

MODULE_NCC_OPTS = --p4-plus --pd-gen --asm-out --no-ohi \
                  --two-byte-profile --fe-flags="-I${TOPDIR}" \
                  --gen-dir ${BLD_P4GEN_DIR}

MODULE_DEPS     = $(shell find ${MODULE_DIR} -name '*.p4' -o -name '*.h') \
                  $(shell find ${MODULE_DIR}/../include -name '*') \
include ${MKDEFS}/post.mk
