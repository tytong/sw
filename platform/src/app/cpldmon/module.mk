# {C} Copyright 2019 Pensando Systems Inc. All rights reserved
include ${MKDEFS}/pre.mk
MODULE_TARGET   := cpldmon.bin
MODULE_PIPELINE = iris
MODULE_SOLIBS   := pal
MODULE_LDLIBS   := ${SDK_THIRDPARTY_CAPRI_LDLIBS}

MODULE_SOLIBS   = halproto utils sdk_asicrw_if sensor pal \
				  logger catalog sdkfru sdkpal pciemgr_if
MODULE_LDLIBS   = :libprotobuf.so.14 ev grpc++ \
                  ${NIC_COMMON_LDLIBS} \
                  ${SDK_THIRDPARTY_CAPRI_LDLIBS} \
                  ${NIC_CAPSIM_LDLIBS}
MODULE_FLAGS    = -DCAPRI_SW ${NIC_CSR_FLAGS}
MODULE_SRCS     := ${MODULE_SRC_DIR}/cpldmon.cc
include ${MKDEFS}/post.mk
