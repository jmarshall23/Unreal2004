#=============================================================================
# PSX2 viewport manager makefile.
#
# Revision history:
# 	* Created by Brandon Reinhart 
#=============================================================================

# Unreal directory. (Required by makefile-header.)
UNREAL_DIR	= ../..

# Include global definitions.
include $(UNREAL_DIR)/makefile-header

@$(MAKE) $(ARGS) --directory=../IOPModule

#-----------------------------------------------------------------------------
# Directories.
#-----------------------------------------------------------------------------

SRC_DIRS	= $(PSX2DRV_SRC)
INC_DIRS	= $(PSX2DRV_INC)
BUILD_DIR	= $(PSX2DRV_BUILD)

#-----------------------------------------------------------------------------
# Compiler/linker options.
#-----------------------------------------------------------------------------

CXX		= $(STD_CXX)
CXXFLAGS	= $(STD_CXXFLAGS) \
			-I$(PSX2_INC) \
			-I$(PSX2DRV_INC) \
			-I$(ENGINE_INC) -I$(CORE_INC) \
			-DGPackage=GPackagePSX2Drv -DThisPackage=PSX2Drv
LDFLAGS		= $(STD_LDFLAGS) -shared -Wl,-soname,$(PSX2DRV_SONAME)
LIBS		= $(STD_LIBS)
AS		= $(STD_AS)
ASFLAGS		= $(STD_ASFLAGS)

#-----------------------------------------------------------------------------
# Files.
#-----------------------------------------------------------------------------

OBJS =			\
	PSX2Client.o	\
	PSX2Viewport.o	\
	PSX2Drv.o \
	PSX2Input.o \
	PSX2MemCard.o

OUT = $(PSX2DRV)

#-----------------------------------------------------------------------------
# Rules.
#-----------------------------------------------------------------------------

# Use general rules.
include $(UNREAL_DIR)/makefile-general

#-----------------------------------------------------------------------------
# The End.
#-----------------------------------------------------------------------------
