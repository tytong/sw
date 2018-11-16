# {C} Copyright 2018 Pensando Systems Inc. All rights reserved
#
# INCLUDE_MODULEMK
# - Function to include one module.mk file.
# Param:
# - Directory of the module.mk file

include ${MKINFRA}/utils.mk
include ${MKINFRA}/sources.mk
include ${MKINFRA}/recipes.mk

define PRINT_TARGET_DEBUG_INFO
    $$(info =================================================)
    $$(info Target           MODULE_TARGET        = $${${1}_TARGET})
    $$(info Pipeline         MODULE_PIPELINE      = $${${1}_PIPELINE})
ifeq "$${${1}_PIPELINE}" "${PIPELINE}"
    $$(info Source Files     MODULE_SRCS          = $${${1}_SRCS})
    $$(info Source DIRS      MODULE_SRC_DIRS      = $${${1}_SRC_DIRS})
    $$(info Bin DIR          MODULE_BIN_DIR       = $${${1}_BIN_DIR})
    $$(info Source EXTS      MODULE_SRC_EXTS      = $${${1}_SRC_EXTS})
    $$(info Module GenTypes  MODULE_GEN_TYPES     = $${${1}_GEN_TYPES})
    $$(info Module PGen MK   MODULE_POSTGEN_MK    = $${${1}_POSTGEN_MK})
    $$(info Shared Libs      MODULE_SOLIBS        = $${${1}_SOLIBS})
    $$(info Archive Libs     MODULE_ARLIBS        = $${${1}_ARLIBS})
    $$(info Includes         MODULE_INCS          = $${${1}_INCS})
    $$(info LD Lib Paths     MODULE_LDPATHS       = $${${1}_LDPATHS})
    $$(info LD Libs          MODULE_LDLIBS        = $${${1}_LDLIBS})
    $$(info LD Flags         MODULE_LDFLAGS       = $${${1}_LDFLAGS})
    $$(info Defs             MODULE_DEFS          = $${${1}_DEFS})
    $$(info Dependencies     MODULE_DEPS          = $${${1}_DEPS})
    $$(info Pre-Requisites   MODULE_PREREQS       = $${${1}_PREREQS})
    $$(info Pre-Requisites   MODULE_EXPORT_LIBS   = $${${1}_EXPORT_LIBS})
    $$(info Pre-Requisites   MODULE_EXPORT_BINS   = $${${1}_EXPORT_BINS})
    $$(info GCC Excl. Flags  MODULE_EXCLUDE_FLAGS = $${${1}_EXCLUDE_FLAGS})
    $$(info Gen. Dir         MODULE_GEN_DIR       = $${${1}_GEN_DIR})
    $$(info Derived Information:)
    $$(info --------------------)
    $$(info Module GID           = $${${1}_MODULE_GID})
    $$(info Module Name          = $${${1}_MODULE_NAME})
    $$(info Module MK Dir        = $${${1}_MK_DIR})
    $$(info Module MK            = $${${1}_MODULE_MK})
    $$(info Module SOLIB Deps    = $${${1}_SOLIB_DEPS})
    $$(info Module ARLIB Deps    = $${${1}_SOLIB_DEPS})
    $$(info Module Objects       = $${${1}_OBJS})
    $$(info Module MMD Deps      = $${${1}_MMD_DEPS})
endif
    $$(info )
endef

CLEAN_DIRS          :=
CXX_TARGETIDS       :=
P4_TARGETIDS        :=
ASM_TARGETIDS       :=
PROTO_TARGETIDS     :=
SVCGEN_TARGETIDS    :=
MOCKGEN_TARGETIDS   :=
GOIMPORTS_TARGETIDS :=
EXPORT_TARGETIDS    :=
GOBIN_TARGETIDS     :=
GTEST_TARGETIDS     :=
SUBMAKE_TARGETIDS   :=
SWIGCLI_TARGETIDS   :=
EXPORT_PREREQS      :=
define INCLUDE_MODULEMK
    MODULE_SRCS                 :=
    MODULE_SOLIBS               :=
    MODULE_ARLIBS               :=
    MODULE_INCS                 :=
    MODULE_LDPATHS              :=
    MODULE_LDLIBS               :=
    MODULE_LDFLAGS              :=
    MODULE_DEFS                 :=
    MODULE_DEPS                 :=
    MODULE_PIPELINE             :=
    MODULE_PREREQS              :=
    MODULE_ARCH_DIR             :=
    MODULE_POSTGEN_MK           :=
    MODULE_GEN_TYPES            :=
    MODULE_PROTOC_GOFAST_OPTS   :=
    MODULE_FLAGS                :=
    MODULE_CLEAN_DIRS           :=
    MODULE_BIN_DIR              :=
    MODULE_LIBS                 :=
    MODULE_GEN_DIR              :=
    MODULE_GOPKG                :=

    # MODULE_DIR can be used by module.mk to know their current
    # directory.
    export MODULE_DIR       = $(patsubst %/,%,$(dir ${1}))
    export MODULE_MK        = ${1}
    include ${1}

    TGID                     = $$(call TARGET_TO_TGID,$${MODULE_TARGET})
    $${TGID}_TARGET         := $${MODULE_TARGET}
    $${TGID}_MODULE_GID     := $$(call MODULE_PATH_TO_GID,${1})
    $${TGID}_MODULE_NAME    := ${MODULE_GID}
    $${TGID}_MK_DIR         := $${MODULE_DIR}
    $${TGID}_ARCH_DIR       := $${MODULE_ARCH_DIR}
    $${TGID}_MODULE_MK      := ${1}
    $${TGID}_POSTGEN_MK     := $${MODULE_POSTGEN_MK}
    $${TGID}_GEN_TYPES      := $${MODULE_GEN_TYPES}
    $${TGID}_SRCS           := $$(strip $$(call CANPATH,$${MODULE_SRCS}))
    $${TGID}_SRC_DIRS       := $$(patsubst %/,%,$$(sort $$(dir $$(call CANPATH,$${MODULE_SRCS}))))
    $${TGID}_BIN_DIR        := $${MODULE_BIN_DIR}
    $${TGID}_SRC_EXTS       := $$(sort $$(suffix $${MODULE_SRCS}))
    $${TGID}_DEFS           := $${MODULE_DEFS}
    $${TGID}_LDLIBS         := $$(addprefix -l,$${MODULE_LDLIBS})
    $${TGID}_LIBS           := $$(addprefix -l,$${MODULE_SOLIBS}) $$(addprefix -l,$${MODULE_ARLIBS}) $$(MODULE_LIBS)
    $${TGID}_SOLIB_DEPS     := $$(join $$(patsubst %,${BLD_OUT_DIR}/lib%_so/,$${MODULE_SOLIBS}),\
                                       $$(patsubst %,lib%.so,$${MODULE_SOLIBS}))
    $${TGID}_ARLIB_DEPS     := $$(join $$(patsubst %,${BLD_OUT_DIR}/lib%_a/,$${MODULE_ARLIBS}),\
                                       $$(patsubst %,lib%.a,$${MODULE_ARLIBS}))
    $${TGID}_INCS           := $$(addprefix -I,$${MODULE_INCS}) ${CONFIG_INCS}
    $${TGID}_LDPATHS        := $$(addprefix -L,$${MODULE_LDPATHS}) \
                               $$(addprefix $${RPATH_PREFIX},$${MODULE_LDPATHS}) ${CONFIG_LDPATHS}
    $${TGID}_LDFLAGS        := $${MODULE_LDFLAGS} ${CMD_LINKER_FLAGS}
    $${TGID}_PIPELINE       := $$(findstring ${PIPELINE},$${MODULE_PIPELINE})
    $${TGID}_ARCH           := $$(findstring ${ARCH},$${MODULE_ARCH})
    $${TGID}_DEPS           := $${MODULE_DEPS}
    $${TGID}_PREREQS        := $$(join $$(addprefix ${BLD_OUT_DIR}/,$$(subst .,_,$${MODULE_PREREQS})),\
                                       $$(addprefix /,$${MODULE_PREREQS}))
    $${TGID}_GEN_DIR        := $${MODULE_GEN_DIR}

    $${TGID}_EXCLUDE_FLAGS       := $${MODULE_EXCLUDE_FLAGS}

    # Set the common flags based on the target type
    $${TGID}_FLAGS := $${MODULE_FLAGS}
    ifeq "$$(suffix $${MODULE_TARGET})" ".a"
        $${TGID}_FLAGS              += ${CONFIG_ARLIB_FLAGS}
        $${TGID}_RECIPE_TYPE        := ARLIB
        $${TGID}_DEFS               += ${${PIPELINE}_DEFS}
        CXX_TARGETIDS               += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".so"
        $${TGID}_FLAGS              += ${CONFIG_SOLIB_FLAGS}
        $${TGID}_RECIPE_TYPE        := SOLIB
        $${TGID}_DEFS               += ${${PIPELINE}_DEFS}
        CXX_TARGETIDS               += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".gtest"
        $${TGID}_INCS               += ${CONFIG_GTEST_INCS}
        $${TGID}_LIBS               += ${CONFIG_GTEST_LIBS}
        $${TGID}_EXCLUDE_FLAGS      += ${CONFIG_GTEST_EXCLUDE_FLAGS}
        $${TGID}_FLAGS              += ${CONFIG_GTEST_FLAGS}
        $${TGID}_LDPATHS            += ${CONFIG_GTEST_LDPATHS}
        $${TGID}_RECIPE_TYPE        := BIN
        $${TGID}_RECIPE_SUBTYPE     := GTEST
        $${TGID}_DEFS               += ${${PIPELINE}_DEFS}
        GTEST_TARGETIDS             += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".p4bin"
        $${TGID}_RECIPE_TYPE        := P4BIN
        $${TGID}_NCC_OPTS           := ${CMD_NCC_OPTS} $${MODULE_NCC_OPTS}
        P4_TARGETIDS                += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".asmbin"
        $${TGID}_RECIPE_TYPE        := ASMBIN
        $${TGID}_CAPAS_OPTS         := ${CMD_CAPAS_OPTS} $${MODULE_CAPAS_OPTS}
        $${TGID}_DEFS               += ${${PIPELINE}_DEFS}
        ASM_TARGETIDS               += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".proto"
        $${TGID}_RECIPE_TYPE        := PROTO
        PROTO_TARGETIDS             += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".svcgen"
        $${TGID}_RECIPE_TYPE        := SVCGEN
        SVCGEN_TARGETIDS            += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".mockgen"
        $${TGID}_RECIPE_TYPE        := MOCKGEN
        $${TGID}_MOCKGEN_OPTS       := $${MODULE_MOCKGEN_OPTS}
        MOCKGEN_TARGETIDS           += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".goimports"
        $${TGID}_RECIPE_TYPE        := GOIMPORTS
        $${TGID}_GOIMPORTS_OPTS     := $${MODULE_GOIMPORTS_OPTS}
        GOIMPORTS_TARGETIDS         += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".export"
        $${TGID}_RECIPE_TYPE        := EXPORT
        $${TGID}_EXPORT_DIR         := $${MODULE_EXPORT_DIR}
        $${TGID}_EXPORT_LIBS        := $$(strip $$(call CANPATH,$${MODULE_EXPORT_LIBS}))
        $${TGID}_EXPORT_BINS        := $$(strip $$(call CANPATH,$${MODULE_EXPORT_BINS}))
        EXPORT_PREREQS              += $$(join $$(addprefix ${BLD_OUT_DIR}/,$$(subst .,_,$${MODULE_TARGET})),\
                                               $$(addprefix /,$${MODULE_TARGET}))
        EXPORT_TARGETIDS            += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".tenjin"
        $${TGID}_BASECMD            := $${MODULE_BASECMD}
        $${TGID}_GENERATOR          := $${MODULE_GENERATOR}
        $${TGID}_TEMPLATE           := $${MODULE_TEMPLATE}
        $${TGID}_OUTFILE            := $${MODULE_OUTFILE}
        $${TGID}_ARGS               := $${MODULE_ARGS}
        $${TGID}_RECIPE_TYPE        := TENJIN
        CXX_TARGETIDS               += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".gobin"
        $${TGID}_RECIPE_TYPE        := GOBIN
        GOBIN_TARGETIDS             += $${TGID}
        $${TGID}_GOPKG              := $${MODULE_GOPKG}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".submake"
        $${TGID}_RECIPE_TYPE        := SUBMAKE
        SUBMAKE_TARGETIDS           += $${TGID}
    else ifeq "$$(suffix $${MODULE_TARGET})" ".swigcli"
        $${TGID}_RECIPE_TYPE        := SWIGCLI
        SWIGCLI_TARGETIDS           += $${TGID}
    else
        $${TGID}_RECIPE_TYPE        := BIN
        $${TGID}_DEFS               += ${${PIPELINE}_DEFS}
        CXX_TARGETIDS               += $${TGID}
    endif

    CLEAN_DIRS += $${MODULE_CLEAN_DIRS}
endef

define PROCESS_MODULEMK_TARGETS
    ${1}_BLD_OUT_DIR    := $$(addprefix ${BLD_OUT_DIR}/,${1})
    ${1}_MKTARGET       := $$(addprefix $(addprefix ${BLD_OUT_DIR}/,${1}),/$${${1}_TARGET})
#    ifneq "$${${1}_RECIPE_TYPE}" "EXPORT"
#        ${1}_PREREQS    += ${EXPORT_PREREQS}
#    endif
endef

define PROCESS_MODULEMK_OBJS
    ifeq "$${${1}_RECIPE_TYPE}" "P4BIN"
        ${1}_OBJS   += $$(addprefix $${${1}_BLD_OUT_DIR}/,$$(addsuffix .p4o,$$(basename $${${1}_SRCS})))
    else ifeq "$${${1}_RECIPE_TYPE}" "ASMBIN"
        ${1}_OBJS   += $$(addprefix $${${1}_BLD_OUT_DIR}/,$$(addsuffix .bin,$$(basename $${${1}_SRCS})))
    else ifeq "$${${1}_RECIPE_TYPE}" "PROTO"
        ifeq "$$(filter CC,$${${1}_GEN_TYPES})" "CC"
            ${1}_OBJS += $$(addprefix $${${1}_BLD_OUT_DIR}/,$$(addsuffix .proto_ccobj,$$(basename $${${1}_SRCS})))
        endif
        ifeq "$$(filter PY,$${${1}_GEN_TYPES})" "PY"
            ${1}_OBJS += $$(addprefix $${${1}_BLD_OUT_DIR}/,$$(addsuffix .proto_pyobj,$$(basename $${${1}_SRCS})))
        endif
        ifeq "$$(filter C,$${${1}_GEN_TYPES})" "C"
            ${1}_OBJS += $$(addprefix $${${1}_BLD_OUT_DIR}/,$$(addsuffix .proto_cobj,$$(basename $${${1}_SRCS})))
        endif
        ifeq "$$(filter GO,$${${1}_GEN_TYPES})" "GO"
            ${1}_OBJS += $$(addprefix $${${1}_BLD_OUT_DIR}/,$$(addsuffix .proto_gobj,$$(basename $${${1}_SRCS})))
#            ${1}_OBJS += $${${1}_BLD_OUT_DIR}/.proto_goobj
        endif
        ifeq "$$(filter DELPHI,$${${1}_GEN_TYPES})" "DELPHI"
            ${1}_OBJS += $$(addprefix $${${1}_BLD_OUT_DIR}/,$$(addsuffix .proto_delphiobj,$$(basename $${${1}_SRCS})))
        endif
        ifeq "$$(filter GOMETRICS,$${${1}_GEN_TYPES})" "GOMETRICS"
            ${1}_OBJS += $$(addprefix $${${1}_BLD_OUT_DIR}/,$$(addsuffix .proto_gometricsobj,$$(basename $${${1}_SRCS})))
        endif
    else ifeq "$${${1}_RECIPE_TYPE}" "MOCKGEN"
        ${1}_OBJS   += $$(patsubst %.pb.go,%_mock.go,$${${1}_SRCS})
    else ifeq "$${${1}_RECIPE_TYPE}" "GOIMPORTS"
        ${1}_OBJS   += $$(addprefix $${${1}_BLD_OUT_DIR}/,$$(patsubst %.go,%.goimports_done,$${${1}_SRCS}))
    else ifeq "$${${1}_RECIPE_TYPE}" "EXPORT"
        ${1}_OBJS   += $$(addprefix ${BLD_LIB_DIR}/,$$(notdir $${${1}_EXPORT_LIBS})) \
                       $$(addprefix ${BLD_BIN_DIR}/,$$(notdir $${${1}_EXPORT_BIN}))
    else ifeq "$${${1}_RECIPE_TYPE}" "GOBIN"
        ${1}_OBJS   =
    else ifeq "$${${1}_RECIPE_TYPE}" "SUBMAKE"
        ${1}_OBJS   =
    else ifeq "$${${1}_RECIPE_TYPE}" "SWIGCLI"
        ${1}_OBJS   =
    else
        ${1}_OBJS   += $$(addprefix $${${1}_BLD_OUT_DIR}/,$$(addsuffix .o,$$(basename $${${1}_SRCS})))
    endif
    ${1}_GXX_FLAGS   = $$(filter-out $${${1}_EXCLUDE_FLAGS}, ${CMD_GXX_FLAGS})
    ${1}_GPP_FLAGS   = $$(filter-out $${${1}_EXCLUDE_FLAGS}, ${CMD_GPP_FLAGS})
endef

define PROCESS_MODULEMK_DEPS
    ifeq "$${${1}_RECIPE_TYPE}" "TENJIN"
        ${1}_DEPS   += $${${1}_GENERATOR} $${${1}_TEMPLATE} $${${1}_ARGS} $${${1}_MODULE_MK}
    else
        ${1}_DEPS       += $${${1}_SOLIB_DEPS} $${${1}_ARLIB_DEPS} ${COMMON_DEPS} $${${1}_PREREQS} $${${1}_MODULE_MK}
        ${1}_MMD_DEPS   := $${${1}_OBJS:%.o=%.d}
    endif
endef

define FILTER_TARGETS_BY_PIPELINE
    ifeq "$${${1}_PIPELINE}" "${PIPELINE}"
        ifeq "$${${1}_ARCH}" "${ARCH}"
            ALL_TARGETS += $${${1}_MKTARGET}
            ifeq "$${${1}_RECIPE_SUBTYPE}" "GTEST"
                ALL_GTEST_TARGETS += $${${1}_MKTARGET}
            endif
        endif
    endif
endef

CACHED_MODULES := ${TOPDIR}/.cached_modules
ifneq ($(wildcard ${CACHED_MODULES}),)
MODULES := $(file < ${CACHED_MODULES})
else
MODULES := $(shell find ${TOPDIR}/ -name 'module*.mk')
endif
MODULE_PATHS = $(strip $(call CANPATH,${MODULES}))
$(foreach modpath,${MODULE_PATHS}, \
    $(eval $(call INCLUDE_MODULEMK,${modpath})))

TARGETIDS := $(strip ${CXX_TARGETIDS} ${P4_TARGETIDS} \
                     ${ASM_TARGETIDS} ${PROTO_TARGETIDS} \
                     ${SVCGEN_TARGETIDS} ${MOCKGEN_TARGETIDS} \
                     ${GOIMPORTS_TARGETIDS} ${EXPORT_TARGETIDS} \
                     ${GOBIN_TARGETIDS} ${GTEST_TARGETIDS} \
                     ${SUBMAKE_TARGETIDS} ${SWIGCLI_TARGETIDS})
$(foreach tgid, ${TARGETIDS}, \
    $(eval $(call PROCESS_MODULEMK_TARGETS,${tgid})))
$(foreach tgid, ${TARGETIDS}, \
    $(eval $(call PROCESS_MODULEMK_OBJS,${tgid})))
$(foreach tgid, ${TARGETIDS}, \
    $(eval $(call PROCESS_MODULEMK_DEPS,${tgid})))
$(foreach tgid, ${TARGETIDS}, \
    $(eval $(call ADD_RECIPE,${tgid})))

ifneq ($(MAKECMDGOALS),clean)
$(foreach tgid,${TARGETIDS},\
    $(foreach mk,${${tgid}_POSTGEN_MK},\
        $(eval $(call ADD_RECIPE_FOR_POSTGEN_MK,${tgid},${mk}))))
endif

# ======================================================================
# Add pattern rule(s) for compiling proto code.
# ======================================================================
PROTO_TARGETIDS := $(strip ${PROTO_TARGETIDS})
$(foreach tgid,${PROTO_TARGETIDS},\
    $(foreach srcdir,${${tgid}_SRC_DIRS},\
        $(foreach gentype,${${tgid}_GEN_TYPES},\
            $(eval $(call ADD_SRC_RULE_PROTO_GEN_${gentype},${tgid},${srcdir})))))

MOCKGEN_TARGETIDS := $(strip ${MOCKGEN_TARGETIDS})
$(call ADD_SRC_RULE,${MOCKGEN_TARGETIDS},ADD_SRC_MOCKGEN_OBJECT_RULE)

GOIMPORTS_TARGETIDS := $(strip ${GOIMPORTS_TARGETIDS})
$(call ADD_SRC_RULE,${GOIMPORTS_TARGETIDS},ADD_SRC_GO_IMPORTS_RULE)

# ======================================================================
# Add pattern rule(s) for creating compiled object code from C++ source.
# ======================================================================
CXX_TARGETIDS := $(strip ${CXX_TARGETIDS})
$(call ADD_SRC_RULE,${CXX_TARGETIDS},ADD_SRC_CXX_OBJECT_RULE)
GTEST_TARGETIDS := $(strip ${GTEST_TARGETIDS})
$(call ADD_SRC_RULE,${GTEST_TARGETIDS},ADD_SRC_CXX_OBJECT_RULE)

# Add the 'make depend' eval rules for all CXX_TARGETS
$(foreach tgid, ${CXX_TARGETIDS}, \
    $(eval -include ${${tgid}_MMD_DEPS}))
$(foreach tgid, ${GTEST_TARGETIDS}, \
    $(eval -include ${${tgid}_MMD_DEPS}))

# ======================================================================
# Add pattern rule(s) for compiling P4 code.
# ======================================================================
P4_TARGETIDS := $(strip ${P4_TARGETIDS})
$(call ADD_SRC_RULE,${P4_TARGETIDS},ADD_SRC_P4_OBJECT_RULE)

# ======================================================================
# Add pattern rule(s) for compiling ASM code.
# ======================================================================
ASM_TARGETIDS := $(strip ${ASM_TARGETIDS})
$(call ADD_SRC_RULE,${ASM_TARGETIDS},ADD_SRC_ASM_OBJECT_RULE)

# ======================================================================
# Add rules for Exports
# ======================================================================
EXPORT_TARGETIDS := $(strip ${EXPORT_TARGETIDS})
$(foreach tgid,${EXPORT_TARGETIDS},$(foreach explib,${${tgid}_EXPORT_LIBS},\
    $(eval $(call ADD_SRC_EXPORT_LIB_RULE,${tgid},${explib}))))
$(foreach tgid,${EXPORT_TARGETIDS},$(foreach expbin,${${tgid}_EXPORT_BINS},\
    $(eval $(call ADD_SRC_EXPORT_BIN_RULE,${tgid},${expbin}))))

# ======================================================================
# Filter Targets by Pipeline.
# ======================================================================
$(foreach tgid, ${TARGETIDS}, \
    $(eval $(call FILTER_TARGETS_BY_PIPELINE,${tgid})))

print-target-debug-info:
	$(info Module Paths = ${MODULE_PATHS})
	$(info TargetIDs = ${TARGETIDS})
	$(info Make Targets = ${ALL_TARGETS})
	$(info ASM TargetIDs = ${ASM_TARGETIDS})
	$(info P4 TargetIDs = ${P4_TARGETIDS})
	$(info PROTO TargetIDs = ${PROTO_TARGETIDS})
	$(foreach tgid, ${TARGETIDS}, \
		$(eval $(call PRINT_TARGET_DEBUG_INFO,${tgid})))
