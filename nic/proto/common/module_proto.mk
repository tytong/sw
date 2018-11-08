# {C} Copyright 2018 Pensando Systems Inc. All rights reserved

include ${MKDEFS}/pre.mk
MODULE_TARGET       = common.proto
MODULE_GEN_TYPES    = CC PY DELPHI
MODULE_INCS         = ${MODULE_DIR} \
                      ${TOPDIR}/nic \
                      ${TOPDIR}/nic/hal/third-party/google/include \
                      ${TOPDIR}/nic/delphi/proto/delphi \
                      /usr/local/include
MODULE_LDLIBS       = pthread
MODULE_POSTGEN_MK   = module_protolib.mk
MODULE_PREREQS = protoc-gen-delphi.gobin
MODULE_GEN_DIR = ${BLD_PROTOGEN_DIR}/common
include ${MKDEFS}/post.mk
