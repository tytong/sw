# {C} Copyright 2018 Pensando Systems Inc. All rights reserved

include ${MKDEFS}/pre.mk
MODULE_PIPELINE = apollo artemis apulu
MODULE_TARGET = libflowtestagenthooks.lib
MODULE_SRCS   = $(shell find ${MODULE_SRC_DIR} -type f -name 'agenthooks.cc')
include ${MKDEFS}/post.mk
