# {C} Copyright 2019 Pensando Systems Inc. All rights reserved

include ${MKDEFS}/pre.mk
MODULE_TARGET   = agent.bin
MODULE_PIPELINE = apollo artemis
MODULE_INCS     = ${MODULE_GEN_DIR}
MODULE_FLAGS    = ${${PIPELINE}_PDS_FLOW_TEST_FLAGS}
MODULE_SOLIBS   = pdsproto trace logger svc pdsapi memhash sltcam ftlv6 ftlv4 \
				  rfc_${PIPELINE} pdsagentcore slhash \
                  ${NIC_${PIPELINE}_NICMGR_LIBS}
MODULE_LDLIBS   = ${NIC_THIRDPARTY_GOOGLE_LDLIBS} \
                  ${NIC_COMMON_LDLIBS} edit ncurses
include ${MKDEFS}/post.mk
