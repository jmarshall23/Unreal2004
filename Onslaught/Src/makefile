#=============================================================================
# Unreal Onslaught makefile for non-Windows platforms.
#=============================================================================

# Unreal directory. (Required by makefile-header.)
UNREAL_DIR		= ../..

# Include global definitions.
include $(UNREAL_DIR)/makefile-header

#-----------------------------------------------------------------------------
# Directories.
#-----------------------------------------------------------------------------

SRC_DIRS		= $(ONSLAUGHT_SRC)
INC_DIRS		= $(ONSLAUGHT_INC)
BUILD_DIR		= $(ONSLAUGHT_BUILD)

#-----------------------------------------------------------------------------
# Compiler/linker options.
#-----------------------------------------------------------------------------

CXX			= $(STD_CXX)
CXXFLAGS		= $(STD_CXXFLAGS) $(EXT_CXXFLAGS) -I$(ONSLAUGHT_INC) \
                    -I$(CORE_INC) -I$(ENGINE_INC) -I$(XGAME_INC) \
                    -I$(IPDRV_INC) \
                    -DGPackage=GPackageOnslaught -DThisPackage=Onslaught
LDFLAGS			= $(STD_LDFLAGS) $(EXT_LDFLAGS)$(ONSLAUGHT_SONAME)
LIBS			= $(STD_LIBS) $(CORE) $(ENGINE)
AS			= $(STD_AS)
ASFLAGS			= $(STD_ASFLAGS)

#-----------------------------------------------------------------------------
# Files.
#-----------------------------------------------------------------------------

WINDLLEXP := ONSLAUGHT_API
PRECOMPILED_HEADER := $(INC_DIRS)/OnslaughtPrivate.h
PRECOMPILE_ON := Onslaught.cpp

OBJS =				\
	ONSChopperCraft.o \
	ONSHoverCraft.o \
	ONSPlaneCraft.o \
	ONSTreadCraft.o \
	ONSVehicle.o \
	ONSWeapon.o \
	ONSWheeledCraft.o \
	Onslaught.o

OUT = $(ONSLAUGHT)

#-----------------------------------------------------------------------------
# Rules.
#-----------------------------------------------------------------------------

# Use general rules.
include $(UNREAL_DIR)/makefile-general

#-----------------------------------------------------------------------------
# The End.
#-----------------------------------------------------------------------------
