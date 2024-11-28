#################################################################
#		Makefile (make >= 3.77)

OUTNAME := MeAssetFactory

#################################################################
#		Source File Definition
# accumulate groups of sources into the SOURCES variable
SOURCES := \
           McdGeometryManager.c \
           MeAssetFactory.c \
           MeAssetFactoryCreateFuncs.c

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
MODULE_INCLUDE_PATH = ../../MeAssetDB/include

#################################################################
#		System Make Rules & Include
MAKESYS_DIR =../../build/makerules
MAKE_RULES:=$(MAKESYS_DIR)/makefile.common

-include ${MAKE_RULES}

$(MAKE_RULES): $(MAKE_RULES).in
	${MAKE} -C$(MAKESYS_DIR) makefile.common
