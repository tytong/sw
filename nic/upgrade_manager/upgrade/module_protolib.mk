# {C} Copyright 2018 Pensando Systems Inc. All rights reserved

include ${MKDEFS}/pre.mk
MODULE_TARGET   = libupgradeproto.so
MODULE_PIPELINE = iris gft
MODULE_INCS     = /usr/local/include \
                  ${TOPDIR}/nic/hal/third-party/google/include \
                  ${TOPDIR}/hal/third-party/grpc/include
MODULE_FLAGS    = -O3
MODULE_EXCLUDE_FLAGS = -O2
MODULE_PREREQS  = upgrade.proto
MODULE_SRCS     = ${BLD_PROTOGEN_DIR}/upgrade.pb.cc ${BLD_PROTOGEN_DIR}/upgrade.delphi.cc
include ${MKDEFS}/post.mk
