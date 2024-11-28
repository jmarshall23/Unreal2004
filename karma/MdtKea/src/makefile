#################################################################
#		Makefile (make >= 3.77)

ifeq '$(PLATFORM)' 'win32'
    ifeq '$(mefpu)'	''
	mefpu		:=x86:sse
    endif
endif

OUTNAME := MdtKea

#################################################################
#		Source File Definition

MODULE_MAY_CONTAIN_EAC_CODE = 1

SOURCES_common:=\
	ReadWriteKeaInputToFile.cpp \
	keaCalcAcceleration_vanilla.cpp \
	keaCalcConstraintForces_vanilla.cpp \
	keaCalcIworldandNonInertialForceandVhmf_vanilla.cpp \
	keaCalcJinvMandRHS_vanilla.cpp \
	keaDebug.cpp \
	keaFunctions.cpp \
	keaLCPSolver.cpp \
	keaLCP_new.cpp \
	keaMakejlenandbl2body.cpp \
	keaMatrix.cpp \
	keaMatrix_tester.cpp \
	keaMemory.cpp \
	keaPrintBasicTypes.cpp \
	keaRbdCore_unified.cpp \
	keaStuff.cpp

#	carSolver.cpp

SOURCES_ps2 := \
	JM_block_ps2.cpp \
	MicrocodeBkMat1212_chol.dsm \
	MicrocodeBkMat1212_sgemm_strsm.dsm \
	MicrocodeBkMat1212_ssyrk.dsm \
	calcA_ps2.cpp \
	cf_block_ps2.cpp \
	keaCalcConstraintForces_ps2.cpp \
	keaCalcIworldandNonInertialForceandVhmf_ps2.cpp \
	keaCalcJinvMandRHS_ps2.cpp \
	keaCalcResultantForces_ps2.cpp \
	keaDma_ps2.cpp \
	keaIntegrate_ps2.cpp \
	keaFunctions.cpp \
	keaMatrix_ps244smalldense_ps2.cpp \
	keaMatrix_ps2smalldense.cpp \
	keaMatrix_ps2sparse.cpp \
	keaMatrix_ps2smalldense_micro.dsm \
	keaMatrix_ps2sparse_micro.dsm \
	keaMatrix_PcSparse.cpp \
	keaMatrix_PcSparse_vanilla.cpp \
	keaLCPSolver.cpp \
	keaCalcAcceleration_vanilla.cpp \
	keaCalcConstraintForces_vanilla.cpp \
	keaCalcIworldandNonInertialForceandVhmf_vanilla.cpp \
	keaCalcJinvMandRHS_vanilla.cpp

SOURCES_pc:= \
	keaIntegrate_pc.cpp \
	keaMatrix_PcSparse.cpp \
	keaMatrix_PcSparse_vanilla.cpp

#	carSolver_pc.cpp

SOURCES_pc_opt:= \
	keaCalcAcceleration_sse.cpp \
	keaCalcConstraintForces_sse.cpp \
	keaCalcIworldandNonInertialForceandVhmf_sse.cpp \
	keaCalcJinvMandRHS_sse.cpp \
	keaCheckCPU_sse.cpp \
	keaMatrix_PcSparse_sse.cpp 

SOURCES_xbox:=\
	keaCheckCPU_xbox.cpp

allsources: $(SOURCES_common) $(SOURCES_pc) $(SOURCES_pc_opt)

SOURCES				:=$(SOURCES_common)

ifeq ($(PLATFORM),ps2)
#   DEFINES                     =_BUILD_VANILLA
    SOURCES			:=$(SOURCES) $(SOURCES_ps2)
    SONY_COMPILER_PATH		?=/usr/local/sce/ee
    AS				:=
    MODULE_INCLUDE_PATH		+=$(SONY_COMPILER_PATH)/include
    MODULE_LIBRARY_PATH		+=$(SONY_COMPILER_PATH)/lib
endif

ifeq ($(PLATFORM),win32)
    WIN32_VANILLA		:=0

    # We worry about floating-point options on Win32
    # We allow fpu=opt1:opt2:op3...
    # where optn is
    #    vanilla    -- Standard C
    #    x86        -- PII optimised
    #    sse        -- PIII optimised
    #    sse2       -- P4 optimised
    #
    # At the moment we only support
    #    fpu=vanilla (the default)
    #    fpu=x86:sse

    orig_mefpu			:=$(mefpu)
    mefpuq			?=vanilla
    # for now, make x86:sse into "sse x86"
    override mefpu		:=$(sort $(subst :, ,$(mefpu)))
    ifeq ($(mefpu), vanilla)
	WIN32_VANILLA		:=1
    else
	ifneq ($(mefpu), sse x86)
	    $(error Unrecognized value of 'mefpu': $(orig_mefpu))
	endif
    endif

    # Double precision version must use vanilla source
    ifeq ($(DEF_PRECISION),DOUBLE)
	WIN32_VANILLA		:=1
    endif

    # Setup defines and source files
    ifeq ($(WIN32_VANILLA),1)
	DEFINES			 =MEFPARCH=MEFPVANILLA _BUILD_VANILLA
	SOURCES			+=$(SOURCES_pc)
    else
#	DEFINES			 =MEFPARCH=(MEFPX86|MEFPSSE)
	SOURCES			+=$(SOURCES_pc) $(SOURCES_pc_opt)
    endif
endif

ifeq ($(PLATFORM),xbox)
    WIN32_VANILLA		:=0
    override mefpu		:=x86:sse

    # Double precision version must use vanilla source
    ifeq ($(DEF_PRECISION),DOUBLE)
	  $(error no double precision for XBox)
    endif

#   DEFINES			 =MEFPARCH=(MEFPX86|MEFPSSE)
#   SOURCES			+=$(SOURCES_pc_opt)
#   SOURCES			+=$(SOURCES_pc)
#   SOURCE			+=$(SOURCES_xbox)

    # Plain vanilla build for now as keaSSEi.h breaks build via some weirdness

    DEFINES			 =_BUILD_VANILLA
    SOURCES			:=$(SOURCES) $(SOURCES_pc)
endif

ifneq (,$(filter linux%,${PLATFORM})) # Defines for linux based builds 
    DEFINES		  	 =_BUILD_VANILLA
    SOURCES			:=$(SOURCES) $(SOURCES_pc)
endif
ifeq ($(PLATFORM),macos)
    DEFINES		  	 =_BUILD_VANILLA
    SOURCES			:=$(SOURCES) $(SOURCES_pc)
endif

ifeq ($(PLATFORM),irix)
    DEFINES			 =_BUILD_VANILLA
    SOURCES			:=$(SOURCES) $(SOURCES_pc)
endif

ifeq ($(PLATFORM),ngc)
    DEFINES			 =_BUILD_VANILLA
    SOURCES			:=$(SOURCES) $(SOURCES_pc)
endif

#################################################################
#		Modify Cmd Line Macros
#
ifneq '$(origin ME_TIMEOUT)' 'undefined'
    DEFINES			:=$(DEFINES) ME_TIMEOUT=$(ME_TIMEOUT)
    DIR_MOD			:=$(if $(DIR_MOD),$(strip $(DIR_MOD))T,_T)
endif

# This would be a specific file modifier:
# DEFINES_yes.c = -DWITH_OPENGL -DWITH_NULL
# UNDEFINES_yes.c = -DWITH_NULL
#################################################################

#################################################################
#		Produce Brief Output
#		comment for command line output
VISIBLE_COMMANDS ?= none

#################################################################
#		Top Source Directory
#		if different from ./
#T_SRC_DIR:=

#################################################################
#		Extra Include Paths (relative to t_src_dir)
MODULE_INCLUDE_PATH =
#################################################################
#		System Make Rules & Include
MAKESYS_DIR =../../build/makerules
MAKE_RULES:=$(MAKESYS_DIR)/makefile.common

-include ${MAKE_RULES}

$(MAKE_RULES): $(MAKE_RULES).in
	${MAKE} -C$(MAKESYS_DIR) makefile.common

