# These are the definitions that will be used in make's rules
# Configure doesn't really have to know about local definitions.
# This file is included at the beginning of the makefile

# *Force* licensing off for PS2 builds
ifeq '$(PLATFORM)' 'ps2'
INCLUDE_LICENSING       := 0
endif

ifeq '$(INCLUDE_LICENSING)' '1'
LICENSE_MODIFIER	:= _l
LM 			:= $(LICENSE_MODIFIER)
INCLUDE_PATH 	 	 = $(T_SRC_DIR)$(LICENSE_SDK_PATH)/machind:$(T_SRC_DIR)$(LICENSE_SDK_PATH)/MeSource

DEFINES_IxTriangleMeshTriangleMesh_l.cpp	:= LICENSE_CHECK
DEFINES_MeshMeshDist_l.cpp			:= LICENSE_CHECK
endif

OUTNAME := McdTriangleMesh$(LICENSE_MODIFIER)


SOURCES_BASIC := \
	BBoxFitter.cpp 				\
	McdTriangleMesh.cpp 			\
	McdTriangleMeshRegisterInteractions.cpp \
	IxTriangleMeshTriangleMesh$(LM).cpp	\
	IxTriangleMeshLineSegment.cpp 		\
	ObbObb.cpp 				\
	CxSODistTriTri.cpp 			\
	BVTree.cpp 				\
	TriTriIsctTest.cpp			\
	TriTriDist.cpp 				\
	RectRectDist.cpp 			\
	SSR.cpp 				\
	Jacobi.cpp 				

#	CxTriangleMesh.cpp 			\
#	ObbFinder.cpp 				\

ifneq '$(PLATFORM)' 'ngc'

# WARNING WARNING! This should be compiled for 'ngc'
# too, but it uses the C++ STL, and we don't have time.
# It will be fixed later.

SOURCES_BASIC += MeshMeshDist$(LM).cpp

endif

SOURCES	:= $(SOURCES_BASIC)

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
MODULE_INCLUDE_PATH = frame cx math util

#################################################################
#		System Make Rules & Include
MAKESYS_DIR 	 = ../../../build/makerules
MAKE_RULES	:= $(MAKESYS_DIR)/makefile.common

include ${MAKE_RULES}

$(MAKE_RULES): $(MAKE_RULES).in
	$(MAKE) -C$(MAKESYS_DIR) makefile.common

#################################################################
#		Local Module Rules
#
test: test.o
	${CXX} $< -o matrix -lm
test.o: matrix.cxx
	${CXX} -DWITH_MAIN -g -c -o test.o $<
tetra: tetra.cxx
	${CXX} -DWITH_MAIN -DLINUX -O2 -I../../include -I../math -I../util -I../../../MeGlobals/include -o $@ $<

EXTRA_CLEANS = matrix test.o tetra.o tetra

#################################################################
#     Rules for licensing-included versions of files
#

ifeq '$(INCLUDE_LICENSING)' '1'

MeshMeshDist$(LM).cpp: MeshMeshDist.cpp
	cp -a $< $@

IxTriangleMeshTriangleMesh$(LM).cpp: IxTriangleMeshTriangleMesh.cpp
	cp -a $< $@

endif
