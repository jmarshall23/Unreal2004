#################################################################
#		Makefile (make >= 3.77)

OUTNAME := Mst

#################################################################
#		Source File Definition
# accumulate groups of sources into the SOURCES variable
SOURCES:=\
	MstBridge.c \
	MstModelDynamics.c \
	MstUniverse.c \
	MstUtils.c 

ifeq '$(PLATFORM)' 'ps2'
	SONY_COMPILER_PATH ?=/usr/local/sce/ee
	MODULE_INCLUDE_PATH = $(SONY_COMPILER_PATH)/include $(EE_COMPILER_PATH)/gcc/include
	MODULE_LIBRARY_PATH = $(SONY_COMPILER_PATH)/lib $(EE_COMPILER_PATH)/gcc/lib
endif

#################################################################
#		Modify Cmd Line Macros
#
DEFINES   =
UNDEFINES = 
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
