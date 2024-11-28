# Copyright (c) 1997-2002 MathEngine PLC
# $Name: t-stevet-RWSpre-030110 $
# $Id: makefile.mk,v 1.8.4.1 2002/04/04 16:19:52 richardm Exp $

.INCLUDEDIRS:			$I ../../MakeConf ../../MakeCfgs/MeTK

.INCLUDE:			MakeCfgProj.mk
.INCLUDE:			MakeCfgHost.mk

.INCLUDE:			MakeDefPlatf.mk
.INCLUDE:			MakeDefHost.mk
.INCLUDE:			MakeDefSite.mk

.INCLUDE:			MakeProjPlatf.mk

MAKE			:=${MAKECMD} ${MFLAGS} ${MAKEMACROS}

DEST			=$_$_

IA_BIN			=${DEST}bin$/
IA_LIB			=${DEST}lib$/
IC_LIB			=${DEST}lib$/
IA_INC			=${DEST}include$/
IC_INC			=${DEST}include$/
IC_DOC			=${DEST}doc$/
IA_ETC			=${DEST}etc$/

_API_CD			:=$_include
_API_CP			:=${_API_CD}$/

_API_C			:=MdtBcl.h MdtBclLimit.h
_LIB_C			:=${LIB_}MdtBcl${_LIB}

HDRS			:=MdtBclMath.hpp
SRCS			:=MdtBcl.cpp MdtBclLimit.cpp

.IF "${TARG_ARCH}" == "MIPSEE"
  SRCS			+:=
.END

.IF "${TARG_ARCH}" == "IA32"
  SRCS			+:=
  .IF "${TARG_ARCH_SSE}" == "yes"
    SRCS		+:=
  .ELSE
    SRCS		+:=
  .END
.END

OBJS			:=${SRCS:db:+${_OBJ}}
OBJS			!:=$(strip ${OBJS:^$+})

TARG			:=$+${_LIB_C}

_BUILD_INCL		+:=${C_I}$_$_MeGlobals/include
_BUILD_INCL		+:=${C_I}$_$_MeMemory/include
_BUILD_INCL		+:=${C_I}$_$_MdtKea/include
_BUILD_INCL		+:=${C_I}$_$_MdtBcl/include
_BUILD_INCL		+:=${C_I}$_$_Mdt/include
_BUILD_INCL		+:=${C_I}$_src

C_INCL			+:=${_BUILD_INCL}
.IF "${C_DEV}" == "${CXX_DEV}"
  CXX_INCL		+:=${_BUILD_INCL}
.END

all:				builddirs

builddirs:			$-

$-:;								-${MKDIR} $@

.IF "${LD_DEV}" == "VS6"
  LD_ALREADY_DEFAULTLIB	:=$(or $(foreach,O,${LD_OPTS} $(eq,$O,/nodefaultlib t $(nil))))
  .IF "${LD_ALREADY_DEFAULTLIB}" != "t"
    LD_OPTS		+:=/nodefaultlib
  .END
.END

LD_LIBS			+:=${LD_LIBS_SYS} ${LD_LIBS_RT_CXX}

.INCLUDE:			MakeDefRules.mk
.INCLUDE:			MakeProjRules.mk

clean:
	$(strip ${RMF} ${OBJS})

realclean:			clean
	$(strip ${RMF} ${TARG})

${TARG}:			${OBJS}

.IF "${API_C}" == "yes"
  all:				${TARG}

  install:			${_API_C:^${IC_INC}}
  install:			${_LIB_C:^${IA_LIB}}

  .IF "${LD_DEV}" == "VS6"
#   all:			$+${_DLL_C}
#   install:			${IA_LIB}${MDT_DLL_C}
  .END
.END

${IC_INC}%.h:			${_API_CP}%.h;			${INSDAT} $< $@
${IA_LIB}%:			$+%;				${INSDAT} $< $@

.INCLUDE .IGNORE:		makedeps.mk
