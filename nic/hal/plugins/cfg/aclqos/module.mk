# {C} Copyright 2018 Pensando Systems Inc. All rights reserved
include ${MKDEFS}/pre.mk
MODULE_TARGET   = libcfg_plugin_aclqos.so
MODULE_PREREQS  = hal.svcgen
MODULE_PIPELINE = iris gft
include ${MKDEFS}/post.mk
