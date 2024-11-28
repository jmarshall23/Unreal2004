#=============================================================================
# Unreal D3D9Drv makefile
#
# Revision history:
# 	* Created by Ryan C. Gordon
#=============================================================================

# Unreal directory. (Required by makefile-header.)
UNREAL_DIR	= ../..

# Include global definitions.
include $(UNREAL_DIR)/makefile-header

#-----------------------------------------------------------------------------
# Directories.
#-----------------------------------------------------------------------------

INC_DIRS	= $(D3D9DRV_INC)
SRC_DIRS	= $(D3D9DRV_SRC)
BUILD_DIR	= $(D3D9DRV_BUILD)

#-----------------------------------------------------------------------------
# Compiler/linker options.
#-----------------------------------------------------------------------------

CXX		= $(STD_CXX)
CXXFLAGS	= -I$(OPENAL_INC) $(STD_CXXFLAGS) $(EXT_CXXFLAGS) -I$(D3D9DRV_INC) \
			  -I$(ENGINE_INC) -I$(CORE_INC) \
              -DGPackage=GPackageD3D9Drv -DThisPackage=D3D9Drv
LDFLAGS		= $(STD_LDFLAGS) $(EXT_LDFLAGS)$(D3D9DRV_SONAME)
LIBS		= $(STD_LIBS) $(CORE) $(ENGINE) $(OGG) $(VORBIS) \
              d3d9.lib d3dx9.lib

ifneq ($(strip $(ISWINDOWS)),true)
  $(error Why are you compiling this on a non-windows platform?)
endif

ifeq ($(strip $(TARGETTYPE)),win32)
  CXXFLAGS += -I../../DirectX9/include
  LDFLAGS += /LIBPATH:"../../DirectX9/lib"
endif

ifeq ($(strip $(TARGETTYPE)),win64)
  CXXFLAGS += -I../../DirectX9_AMD64/include
  LDFLAGS += /LIBPATH:"../../DirectX9_AMD64/lib/amd64"
endif

#-----------------------------------------------------------------------------
# Files.
#-----------------------------------------------------------------------------

WINDLLEXP := D3D9DRV_API
PRECOMPILED_HEADER := D3D9Drv.h
PRECOMPILE_ON := D3D9Drv.cpp

OBJS =			\
	D3D9Drv.o   \
    D3D9MaterialState.o \
    D3D9RenderDevice.o \
    D3D9RenderInterface.o \
    D3D9RenderState.o \
    D3D9Resource.o \
    xD3D9Helper.o \
    xD3D9RenderInterface.o

OUT = $(D3D9DRV)

#-----------------------------------------------------------------------------
# Rules.
#-----------------------------------------------------------------------------

# Use general rules.
include $(UNREAL_DIR)/makefile-general

#-----------------------------------------------------------------------------
# The End.
#-----------------------------------------------------------------------------
