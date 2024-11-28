# These are the definitions that will be used in make's rules
# Configure doesn't really have to know about local definitions.
# This file is included at the beginning of the makefile

OUTNAME :=McdConvex

SOURCES_CONVEX := \
	gjk/McdGjk.cpp \
	gjk/McdGjkMaximumPoint.cpp \
	gjk/McdGjkPenetrationDepth.cpp \
	gjk/McdGjkRegistration.cpp \
	gjk/McdPlaneIntersect.cpp \
	gjk/McdPolygonIntersection.cpp

SOURCES_CONVEXMESH := \
	ConvexGeomUtils.cpp \
	IxConvexLineSegment.cpp \
	IxConvexPrimitives.cpp \
	IxConvexTriList.cpp \
	McdConvexMesh.cpp \
	McdConvexMeshMassProps.c \
	McdConvexRegistration.cpp

# these belong in the convex hull library
#	cnvHull.c
#	CxConvexHull.cpp
#	McdConvexHull.cpp

SOURCES :=$(SOURCES_CONVEX)
SOURCES +=$(SOURCES_CONVEXMESH)

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
MODULE_INCLUDE_PATH = frame util QHull primitives convex gjk math

#################################################################
#		System Make Rules & Include
MAKESYS_DIR = ../../../build/makerules
MAKE_RULES:=$(MAKESYS_DIR)/makefile.common

$(MAKE_RULES): $(MAKE_RULES).in
	${MAKE} -C$(MAKESYS_DIR) makefile.common

include ${MAKE_RULES}
