# {C} Copyright 2018 Pensando Systems Inc. All rights reserved

include ${MKDEFS}/pre.mk
MODULE_TARGET = twheel_test.gtest
MODULE_SOLIBS = twheel slab shmmgr logger
MODULE_LDLIBS = rt
MODULE_ARCH   = x86_64
include ${MKDEFS}/post.mk
