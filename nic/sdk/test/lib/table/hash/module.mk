# {C} Copyright 2018 Pensando Systems Inc. All rights reserved
include ${MKDEFS}/pre.mk
MODULE_TARGET   = hash_test.gtest
MODULE_SOLIBS   = logger shmmgr ht tcam hash p4pd_mock indexer
MODULE_LDLIBS   = rt
include ${MKDEFS}/post.mk
