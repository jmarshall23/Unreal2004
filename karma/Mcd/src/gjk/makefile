# These are the definitions that will be used in make's rules
# Configure doesn't really have to know about local definitions.
# This file is included at the beginning of the makefile

OUTNAME:=McdConvexGjk

SOURCES:=\
	McdGjk.cpp \
	McdGjkMaximumPoint.cpp \
	McdGjkPenetrationDepth.cpp \
	McdGjkRegistration.cpp \
	McdPlaneIntersect.cpp \
	McdPolygonIntersection.cpp

#	McdGjk.h \
#	McdGjkMaximumPoint.h \
#	McdPlaneIntersect.h \
#	McdPolygonIntersection.h

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
MODULE_INCLUDE_PATH = frame cx util QHull terrain primitives convex gjk

#################################################################
#		System Make Rules & Include
MAKESYS_DIR = ../../../build/makerules
MAKE_RULES:=$(MAKESYS_DIR)/makefile.common

$(MAKE_RULES): $(MAKE_RULES).in
	${MAKE} -C$(MAKESYS_DIR) makefile.common

include ${MAKE_RULES}
