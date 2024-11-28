#=============================================================================
# Unreal Core makefile for Linux.
#
# Revision history:
# 	* Created by Mike Danylchuk
#=============================================================================

# Unreal directory. (Required by makefile-header.)
UNREAL_DIR	= ../..

# Include global definitions.
include $(UNREAL_DIR)/makefile-header

#-----------------------------------------------------------------------------
# Directories.
#-----------------------------------------------------------------------------

SRC_DIRS	= $(LAUNCH_SRC)
INC_DIRS    = $(LAUNCH_INC)
BUILD_DIR	= $(LAUNCH_BUILD)

#-----------------------------------------------------------------------------
# Compiler/linker options.
#-----------------------------------------------------------------------------

CXX		= $(STD_CXX)

#ifeq ($(LINKSTYLE),shared)
  CXXFLAGS	= $(STD_CXXFLAGS) -I$(CORE_INC) -I$(ENGINE_INC) -I$(WINDOW_INC)
  LDFLAGS = $(STD_LDFLAGS)
  LIBS = $(STD_LIBS) $(ENGINE) $(CORE) $(KARMA_LIBS) $(ZLIB) $(OGG) $(VORBIS) $(WINDOW)
#endif

ifeq ($(LINKSTYLE),static)
#CXXFLAGS	= $(STD_CXXFLAGS) $(TARGET_INC)
#LIBS		= $(STD_LIBS) $(TARGET_LIB)
#LDFLAGS     = $(STD_LDFLAGS)
LIBS += $(IPDRV) $(FIRE) $(UWEB) $(XGAME) $(XINTERFACE) $(SDLDRV) $(OPENGLDRV) $(ALAUDIO) $(GAMERANGER_LIBS) $(OGG) $(VORBIS) $(NEWVEHICLES) $(ONSLAUGHT) $(SPEEX)
CXXFLAGS += -I$(ENGINE_SRC) -I$(FIRE_INC) -I$(IPDRV_INC) -I$(XINTERFACE_INC) \
            -I$(XGAME_INC) -I$(UWEB_INC) -I$(SDLDRV_INC) -I$(OPENGLDRV_INC) \
            -I$(OPENGLLNX_INC) -I$(ALAUDIO_INC) -I$(OPENAL_INC) -I$(SPEEX_INC) \
            -I$(ONSLAUGHT_INC) -I$(EDITOR_INC)

ifeq ($(DOPIXO),true)
#LDFLAGS += -Xlinker --allow-multiple-definition
LIBS += $(PIXODRV) $(PIXOMATIC_LIBS)
CXXFLAGS += -I$(PIXODRV_INC) -I$(PIXOMATIC_INC)
endif

endif

#-----------------------------------------------------------------------------
# Files.
#-----------------------------------------------------------------------------

OBJS =		\
	Launch.o

OUT = $(LAUNCH)

#-----------------------------------------------------------------------------
# Rules.
#-----------------------------------------------------------------------------

# Use general rules.
EXECUTABLE	= 1
include $(UNREAL_DIR)/makefile-general

#-----------------------------------------------------------------------------
# The End.
#-----------------------------------------------------------------------------
