# {C} Copyright 2020 Pensando Systems Inc. All rights reserved

include ${MKDEFS}/pre.mk
MODULE_TARGET   = libupgrade_core.lib
MODULE_PREREQS  = core_graceful.upgfsmgen core_hitless.upgfsmgen
include ${MKDEFS}/post.mk
