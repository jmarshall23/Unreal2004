#################################################################
#		Makefile (make >= 3.77)

OUTNAME := Mdt

#################################################################
#		Source File Definition
# accumulate groups of sources into the SOURCES variable
SOURCES := \
	MdtAngular3.c \
	MdtBSJoint.c \
	MdtBody.c \
	MdtCarWheel.c \
	MdtConeLimit.c \
	MdtConstraint.c \
	MdtContact.c \
	MdtContactGroup.c \
	MdtContactParams.c \
	MdtFixedPath.c \
	MdtHinge.c \
	MdtLOD.c \
	MdtLimit.c \
	MdtLinear1.c \
	MdtLinear2.c \
	MdtMainLoop.c \
	MdtPartition.c \
	MdtPrismatic.c \
	MdtRPROJoint.c \
	MdtSkeletal.c \
	MdtSpring.c \
	MdtSpring6.c \
	MdtUniversal.c \
	MdtUserConstraint.c \
	MdtUtils.c \
	MdtWorld.c

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
