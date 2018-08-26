# {C} Copyright 2018 Pensando Systems Inc. All rights reserved
#####################################################################
################# SRC SPECIFIC RULE GENERATORS ###################
#####################################################################
EXTS_CXX    := %.cc %.cpp
EXTS_C      := %.c
EXTS_GO     := %.go
EXTS_ASM    := %.asm %.s
EXTS_P4     := %.p4

define ADD_SRC_CXX_OBJECT_RULE
${${1}_BLD_OUT_DIR}/%.o: ${${1}_SRC_DIR}/${2} ${${1}_MODULE_MK}
	${AT}mkdir -p $$(dir $$@)
	${NAT}${AT}echo ${COMMON_GCC_SHORT_NAME} $$(call CANPATH,$$@)
	${AT}$(strip ${CMD_GXX} -c -o $$@ ${${1}_FLAGS} ${${1}_GXX_FLAGS} ${${1}_INCS} ${${1}_DEFS} $$<)
endef

define ADD_SRC_P4_OBJECT_RULE
$${${1}_BLD_OUT_DIR}/%.p4o: $${${1}_SRC_DIR}/${2} $${${1}_MODULE_MK} $${${1}_DEPS}
	${AT}mkdir -p $$(dir $$@)
	${NAT}${AT}echo ${COMMON_NCC_SHORT_NAME} $$(call CANPATH,$$@) "=>" $$(call CANPATH,$$@_build.log)
	${AT}$(strip ${CMD_NCC} ${${1}_NCC_OPTS} $$<) > $$(call CANPATH,$$@_build.log)
	${AT}touch $$@
endef

define ADD_SRC_ASM_OBJECT_RULE
$${${1}_BLD_OUT_DIR}/%.bin: $${${1}_SRC_DIR}/${2} $${${1}_MODULE_MK} $${${1}_DEPS}
	${AT}mkdir -p $$(dir $$@)
	${NAT}${AT}echo ${COMMON_CAPAS_SHORT_NAME} $$(call CANPATH,$$@)
	${AT}$(strip ${CMD_CAPAS} ${${1}_CAPAS_OPTS} $$< ${${1}_INCS} ${${1}_DEFS} -o $$@ > $${${1}_BLD_OUT_DIR}/$$(patsubst %.bin,%.sym,$$(shell basename $$@)))
	${AT}touch $$@
endef

