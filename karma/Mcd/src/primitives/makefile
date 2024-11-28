# These are the definitions that will be used in make's rules
# Configure doesn't really have to know about local definitions.
# This file is included at the beginning of the makefile

OUTNAME:=McdPrimitives

SOURCES_GROUPINGS:=\
	McdPrimitivesRegisterTypes.cpp \
	McdPrimitivesRegisterInteractions.cpp \
	McdSphereBoxPlaneRegisterTypes.cpp \
	McdSphereBoxPlaneRegisterInteractions.cpp

#	McdPrimitives.cpp

SOURCES_BOXPLANESPHERE:=\
	IxBoxPlane.cpp \
	IxBoxSphere.cpp \
	IxPrimitiveLineSegment.cpp \
	IxSpherePlane.cpp \
	IxSphereSphere.cpp \
	McdBox.cpp \
	McdPlane.cpp \
	McdSphere.cpp \
	MovingBoxBoxIntersect.cpp

#ifeq ($(PLATFORM),ps2)
#    SOURCES_BOXPLANESPHERE += IxBoxBox_ps2.cpp
#else
    SOURCES_BOXPLANESPHERE += IxBoxBox.cpp
#endif

SOURCES_CYLINDER:=\
        IxBoxCylinder.cpp \
        IxCylinderCylinder.cpp \
        IxCylinderPlane.cpp \
        IxCylinderSphere.cpp \
        IxCylinderLineSegment.cpp\
        McdCylinder.cpp
        
#	math/mesffnmin.cpp

SOURCES_CONE:=\
#	 McdCone.cpp \
#	 IxConeBox.cpp \
#	 IxConeCone.cpp \
#	 IxConePlane.cpp \
#	 IxConeSphere.cpp \
#	 IxConeCylinder.cpp \
#	 IxConeLineSegment.cpp 

SOURCES_GJK:=
#	convex/GjkSup.cpp \
#       convex/GjkTables.cpp

SOURCES_LINESEG:=
#	IxPrimitiveLineSegment.cpp 

SOURCES_TRIANGLE:=
#	CxTriangle.cpp 

SOURCES_CONVEX:=\
#        convex/McdConvexMesh.cpp \
#        convex/CxConvex.cpp \
#        convex/ConvexConvex.cpp \
#        convex/IxBoxConvex.cpp \
#        convex/IxSphereConvex.cpp \
#     	convex/IxConvexMeshLineSegment.cpp

SOURCES_SPHYL:=\
        IxSphylPrimitives.cpp \
        McdSphyl.cpp \
        Polynomial.cpp

#	Polynomial.cpp

SOURCES_TRILIST:=\
	IxBoxTriList.cpp \
	IxCylinderTriList.cpp \
	IxSphereTriList.cpp \
	McdTriangleList.cpp

#	IxPrimitiveTriangleNE.cpp



# accumulate groups of sources into the SOURCES variable
SOURCES:=$(SOURCES_GROUPINGS)
SOURCES+=$(SOURCES_BOXPLANESPHERE)
SOURCES+=$(SOURCES_CYLINDER)
SOURCES+=$(SOURCES_GJK)

SOURCES+=$(SOURCES_LINESEG)
SOURCES+=$(SOURCES_TRILIST)
SOURCES+=$(SOURCES_TRIANGLE)
SOURCES+=$(SOURCES_SPHYL)

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
MODULE_INCLUDE_PATH = frame cx math util primitives

#################################################################
#		System Make Rules & Include
MAKESYS_DIR = ../../../build/makerules
MAKE_RULES:=$(MAKESYS_DIR)/makefile.common

include ${MAKE_RULES}

$(MAKE_RULES): $(MAKE_RULES).in
	${MAKE} -C$(MAKESYS_DIR) makefile.common
