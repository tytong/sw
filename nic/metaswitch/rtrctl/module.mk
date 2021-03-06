# {C} Copyright 2019 Pensando Systems Inc. All rights reserved

include ${MKDEFS}/pre.mk
MODULE_TARGET   = rtrctl.gobin
MODULE_PREREQS  = agent_pdsproto.submake pdsgen.proto ms_pdsproto.submake
MODULE_PIPELINE = apulu
MODULE_ARCH     = x86_64
MODULE_FLAGS    = -ldflags="-s -w"
MODULE_DEPS     = $(shell find ${MODULE_SRC_DIR}/ -name '*.go')
include ${MKDEFS}/post.mk
