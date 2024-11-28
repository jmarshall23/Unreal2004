#=============================================================================
# Unreal PixoDrv makefile for Linux.
#
# Revision history:
# 	* Created by Daniel Vogel (based on XDrv).
# 
#=============================================================================

# Unreal directory. (Required by makefile-header.)
UNREAL_DIR	= ../..

# Include global definitions.
include $(UNREAL_DIR)/makefile-header

#-----------------------------------------------------------------------------
# Directories.
#-----------------------------------------------------------------------------

SRC_DIRS	= $(PIXODRV_SRC)
INC_DIRS    = $(PIXODRV_INC)
BUILD_DIR	= $(PIXODRV_BUILD)

#-----------------------------------------------------------------------------
# Compiler/linker options.
#-----------------------------------------------------------------------------

CXX		= $(STD_CXX)
CXXFLAGS    = $(STD_CXXFLAGS) $(EXT_CXXFLAGS) \
              -I$(PIXODRV_INC) -I$(PIXOMATIC_INC) -I$(ENGINE_INC) \
              -I$(CORE_INC) -I$(SDL12_INC) -DGPackage=PixoDrv
LDFLAGS     = $(STD_LDFLAGS) $(EXT_LDFLAGS)$(PIXODRV_SONAME)
LIBS        = $(STD_LIBS) $(CORE) $(ENGINE)
AS          = $(STD_AS)
ASFLAGS     = $(STD_ASFLAGS)

#-----------------------------------------------------------------------------
# Files.
#-----------------------------------------------------------------------------

WINDLLEXP := PIXODRV_API
PRECOMPILED_HEADER := $(INC_DIRS)/PixoDrv.h
PRECOMPILE_ON := PixoDrv.cpp

OBJS =			\
	PixoDrv.o \
	PixoLighting.o \
	PixoMaterialState.o \
	PixoRenderDevice.o \
	PixoRenderInterface.o \
	PixoResource.o \
	utils_surface.o

OUT = $(PIXODRV)

#-----------------------------------------------------------------------------
# Rules.
#-----------------------------------------------------------------------------

# Use general rules.
include $(UNREAL_DIR)/makefile-general

#-----------------------------------------------------------------------------
# The End.
#-----------------------------------------------------------------------------
