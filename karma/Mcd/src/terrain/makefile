# These are the definitions that will be used in make's rules
# Configure doesn't really have to know about local definitions.
# This file is included at the beginning of the makefile

SOURCES_RGHF:=\
	IxSphereRGHeightField.cpp\
	IxBoxRGHeightField.cpp\
	IxCylinderRGHeightField.cpp\
	McdRGHeightField.cpp\
	IxRGHeightFieldLineSegment.cpp\

OUTNAME:=McdRGHeightField

# accumulate groups of sources into the SOURCES variable
SOURCES:=$(SOURCES_RGHF)

#################################################################
#		Modify Cmd Line Macros
#
DEFINES   = WITH_OPENGL WITH_NULL
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
MODULE_INCLUDE_PATH = frame cx math util terrain

#################################################################
#		System Make Rules & Include
MAKESYS_DIR = ../../../build/makerules
MAKE_RULES:=$(MAKESYS_DIR)/makefile.common

$(MAKE_RULES): $(MAKE_RULES).in
	${MAKE} -C$(MAKESYS_DIR) makefile.common

include ${MAKE_RULES}

test: test.o
	${CXX} $< -o matrix -lm
test.o: matrix.cxx
	${CXX} -DWITH_MAIN -g -c -o test.o $<
tetra: tetra.cxx
	${CXX} -DWITH_MAIN -DLINUX -O2 -I../../include -I../math -I../util -I../../../MeGlobals/include -o $@ $<

EXTRA_CLEANS = matrix test.o tetra.o tetra
