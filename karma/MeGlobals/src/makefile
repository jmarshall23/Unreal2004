#################################################################
#		Makefile (make >= 3.77)

OUTNAME := MeGlobals

#################################################################
#		Source File Definition
# accumulate groups of sources into the SOURCES variable
SOURCES:=\
	MeASELoad.c \
	MeBounding.c \
	MeChunk.c \
	MeCommandLine.c \
	MeDebugDraw.c \
	MeDict.c \
	MeFileSearch.c \
	MeHash.c \
	MeHeap.c \
	MeIDPool.c \
	MeMath.c \
	MeMemory.c \
	MeMemoryCpp.cpp \
	MeMessage.c \
	MeMisc.c \
	MePool.c \
	MePoolx.c \
	MePrecision.c \
	MeProfile.c \
	MeSet.c \
	MeSimpleFile.c \
	MeStream.c \
	MeString.c \
	MeVersion.c

SOURCES_win32:=\
	MeProfile_win32.c \
	MeSimpleFile_win32.c

SOURCES_xbox:=\
	MeProfile_xbox.c \
	MeSimpleFile_xbox.c
	
SOURCES_linux:=\
	MeProfile_linux.c \
	MeSimpleFile_linux.c
	
SOURCES_macos:=\
	MeProfile_macos.c \
	MeSimpleFile_macos.c

SOURCES_irix:=\
	MeProfile_irix.c \
	MeSimpleFile_irix.c

SOURCES_irix_o32:=\
	MeProfile_irix_o32.c \
	MeSimpleFile_irix_o32.c

SOURCES_mips3:=\
	MeProfile_mips3.c \
	MeSimpleFile_mips3.c

SOURCES_ps2:=\
	MeProfile_ps2.c \
	MeSimpleFile_ps2.c

SOURCES_ngc:=\
	MeProfile_ngc.c \
	MeSimpleFile_ngc.c

ifeq '$(PLATFORM)' 'win32'
    SOURCES	+= $(SOURCES_win32)  
endif

ifeq '$(PLATFORM)' 'xbox'
    SOURCES	+= $(SOURCES_xbox)
endif

ifeq '$(PLATFORM)' 'linux'
    SOURCES	+= $(SOURCES_linux) 
endif

ifeq '$(PLATFORM)' 'macos'
    SOURCES	+= $(SOURCES_macos) 
endif

ifeq '$(PLATFORM)' 'linux_hx'
    SOURCES	+= $(SOURCES_linux) 
endif

ifeq '$(PLATFORM)' 'linux_hx_cc'
    SOURCES	+= $(SOURCES_linux) 
endif

ifeq '$(PLATFORM)' 'irix'
    SOURCES	+=$(SOURCES_irix)
endif

ifeq '$(PLATFORM)' 'irix_o32'
    SOURCES	+=$(SOURCES_irix_o32)
endif

ifeq '$(PLATFORM)' 'mips3'
    SOURCES	+=$(SOURCES_mips3) 
endif

ifeq '$(PLATFORM)' 'ps2'
    SOURCES	+=$(SOURCES_ps2)
endif

ifeq '$(PLATFORM)' 'ngc'
    SOURCES	+=$(SOURCES_ngc)
endif

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
