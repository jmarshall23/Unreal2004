#=============================================================================
# Unreal XGame makefile for non-Windows platforms.
#=============================================================================

# Unreal directory. (Required by makefile-header.)
UNREAL_DIR		= ../..

# Include global definitions.
include $(UNREAL_DIR)/makefile-header

#-----------------------------------------------------------------------------
# Directories.
#-----------------------------------------------------------------------------

SRC_DIRS		= $(XGAME_SRC)
INC_DIRS		= $(XGAME_INC)
BUILD_DIR		= $(XGAME_BUILD)

#-----------------------------------------------------------------------------
# Compiler/linker options.
#-----------------------------------------------------------------------------

CXX			= $(STD_CXX)
CXXFLAGS		= $(STD_CXXFLAGS) $(EXT_CXXFLAGS) -I$(XGAME_INC) -I$(CORE_INC) \
	    			-I$(ENGINE_INC) -DGPackage=GPackageXGame -DThisPackage=XGame
LDFLAGS			= $(STD_LDFLAGS) $(EXT_LDFLAGS)$(XGAME_SONAME)
LIBS			= $(STD_LIBS) $(CORE) $(ENGINE)
AS			= $(STD_AS)
ASFLAGS			= $(STD_ASFLAGS)

#-----------------------------------------------------------------------------
# Files.
#-----------------------------------------------------------------------------

WINDLLEXP := XGAME_API
PRECOMPILED_HEADER := $(INC_DIRS)/XGame.h
PRECOMPILE_ON := XGame.cpp

OBJS = \
	XGame.o \
	XUtil.o

OUT = $(XGAME)

#-----------------------------------------------------------------------------
# Rules.
#-----------------------------------------------------------------------------

# Use general rules.
include $(UNREAL_DIR)/makefile-general

#-----------------------------------------------------------------------------
# The End.
#-----------------------------------------------------------------------------
