# These are the definitions that will be used in make's rules
# Configure doesn't really have to know about local definitions.
# This file is included at the beginning of the makefile

# these file definitions are relative to the ../../src directory
# ?? Because that's the way it is.

OUTNAME:=McdConvexCreateHull

SOURCES_QHULL:= \
	convex/McdConvexHull.cpp \
	geom.c \
	geom2.c \
	global.c \
	io.c \
	mem.c \
	merge.c \
	poly.c \
	poly2.c \
	qhull.c \
	qset.c \
	stat.c \
	user.c

SOURCES:=$(SOURCES_QHULL)

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
MODULE_INCLUDE_PATH = QHull

#################################################################
#		System Make Rules & Include
MAKESYS_DIR = ../../../build/makerules
MAKE_RULES:=$(MAKESYS_DIR)/makefile.common

$(MAKE_RULES): $(MAKE_RULES).in
	${MAKE} -C$(MAKESYS_DIR) makefile.common

include ${MAKE_RULES}
