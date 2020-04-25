# {C} Copyright 2018 Pensando Systems Inc. All rights reserved

include ${MKDEFS}/pre.mk
MODULE_TARGET   = elb_model.bin
MODULE_ARCH     = x86_64
MODULE_ASIC     = elba
MODULE_LDPATHS  = $(abspath ${MODULE_DIR}/../../libs/elba)
MODULE_LDLIBS   = model_cc elbsim common_cc top_csr_compile sknobs \
                  mpuobj elbisa isa Judy python2.7 zmq \
                  gmp gmpxx pthread dl util \
                  :libcrypto.so.1.0.2k stdc++ m
include ${MKDEFS}/post.mk
