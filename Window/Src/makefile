#=============================================================================
# Unreal Window makefile.
#=============================================================================

# Unreal directory. (Required by makefile-header.)
UNREAL_DIR		= ../..

# Include global definitions.
include $(UNREAL_DIR)/makefile-header

#-----------------------------------------------------------------------------
# Directories.
#-----------------------------------------------------------------------------

SRC_DIRS		= $(WINDOW_SRC)
INC_DIRS		= $(WINDOW_INC)
BUILD_DIR		= $(WINDOW_BUILD)

#-----------------------------------------------------------------------------
# Compiler/linker options.
#-----------------------------------------------------------------------------

CXX			= $(STD_CXX)
CXXFLAGS		= $(STD_CXXFLAGS) $(EXT_CXXFLAGS) -I$(WINDOW_INC) -I$(WINDOW_SRC) \
                    -I$(CORE_INC) -I$(ENGINE_INC) \
	    			-DGPackage=GPackageWindow -DThisPackage=Window
LDFLAGS			= $(STD_LDFLAGS) $(EXT_LDFLAGS)$(WINDOW_SONAME)
LIBS			= $(STD_LIBS) $(CORE) $(ENGINE) $(KARMA_LIBS) $(OGG) $(VORBIS)
AS			= $(STD_AS)
ASFLAGS			= $(STD_ASFLAGS)

#-----------------------------------------------------------------------------
# Files.
#-----------------------------------------------------------------------------

WINDLLEXP := WINDOW_API
PRECOMPILED_HEADER := $(INC_DIRS)/Window.h
PRECOMPILE_ON := Window.cpp

OBJS =				\
    Controls.o \
    Properties.o \
    Dialogs.o \
    Utils.o \
    Window.o

OUT = $(WINDOW)

#-----------------------------------------------------------------------------
# Rules.
#-----------------------------------------------------------------------------

# Use general rules.
include $(UNREAL_DIR)/makefile-general

#-----------------------------------------------------------------------------
# The End.
#-----------------------------------------------------------------------------
