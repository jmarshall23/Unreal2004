#################################################################
#		Makefile (make >= 3.77)

# These are the definitions that will be used in make's rules
# Configure doesn't really have to know about local definitions.
# This file is included at the beginning of the makefile

OUTNAME	:= McdCommon

# SOURCES_COMMON are to be put in a separate library: McdCommon.[lib,a]

SOURCES_space := \
	space/CxSmallSort.cpp \
	space/McdSpace.cpp

SOURCES_math  := \
	math/GeomUtils.cpp \
	math/McdVanillaCore.cpp \
	math/mesffnmin.cpp

#	math/MathUtils.cpp

SOURCES_contacts := \
	McdContact.cpp


SOURCES	:= $(SOURCES_space) $(SOURCES_math) $(SOURCES_contacts)


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
T_SRC_DIR:=../

#################################################################
#		Extra Include Paths (relative to t_src_dir)
MODULE_INCLUDE_PATH := frame cx math space util

#################################################################
#		System Make Rules & Include
MAKESYS_DIR	 = ../../../build/makerules
MAKE_RULES	:= $(MAKESYS_DIR)/makefile.common

include ${MAKE_RULES}

$(MAKE_RULES): $(MAKE_RULES).in
	$(MAKE) -C$(MAKESYS_DIR) makefile.common
