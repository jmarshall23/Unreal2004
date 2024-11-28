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

SRC_DIRS	= $(SDLLAUNCH_SRC)
INC_DIRS    = $(SDLLAUNCH_INC)
BUILD_DIR	= $(SDLLAUNCH_BUILD)

#-----------------------------------------------------------------------------
# Compiler/linker options.
#-----------------------------------------------------------------------------

CXX		= $(STD_CXX)

#ifeq ($(LINKSTYLE),shared)
  CXXFLAGS	= $(STD_CXXFLAGS) $(SDL_CFLAGS)  -I$(CORE_INC) -I$(ENGINE_INC) \
              -I$(SDL12_INC)
  ifeq ($(TARGETTYPE),macosx)
    LDFLAGS		= $(STD_LDFLAGS) -Wl,-bind_at_load \
                          $(UNREAL_DIR)/System/libSDLmain-osx.a \
                          -framework Cocoa -framework OpenGL -framework Carbon \
                          -framework IOKit -framework ApplicationServices
                           # !!! FIXME: Lose IOKit with the shitty CD detection.
  else
    ifeq ($(COMPILER),gcc)
      LDFLAGS		= $(STD_LDFLAGS) -export-dynamic
    else
      LDFLAGS		= $(STD_LDFLAGS) -Qoption,link,--export-dynamic
    endif
  endif
  LIBS		= $(STD_LIBS) $(ENGINE) $(CORE) $(KARMA_LIBS) $(ZLIB) $(OGG) $(VORBIS)
#endif

ifeq ($(TARGETTYPE),linux)
LDFLAGS += -Wl,-rpath -Wl,. $(SDL12_LIBS)
else
LDFLAGS += $(SDL12_LIBS)
endif

ifeq ($(LINKSTYLE),static)
#CXXFLAGS	= $(STD_CXXFLAGS) $(TARGET_INC)
#LIBS		= $(STD_LIBS) $(TARGET_LIB)
#LDFLAGS     = $(STD_LDFLAGS)
LIBS += $(IPDRV) $(FIRE) $(UWEB) $(XGAME) $(XINTERFACE) $(SDLDRV) $(OPENGLDRV) $(ALAUDIO) $(GAMERANGER_LIBS) $(OGG) $(VORBIS) $(NEWVEHICLES) $(ONSLAUGHT) $(SPEEX)
#$(MP3)
CXXFLAGS += -I$(ENGINE_SRC) -I$(FIRE_INC) -I$(IPDRV_INC) -I$(XINTERFACE_INC) \
            -I$(XGAME_INC) -I$(UWEB_INC) -I$(SDLDRV_INC) -I$(OPENGLDRV_INC) \
            -I$(OPENGLLNX_INC) -I$(ALAUDIO_INC) -I$(OPENAL_INC) -I$(SPEEX_INC) \
            -I$(ONSLAUGHT_INC) -I$(EDITOR_INC)

ifeq ($(DOPIXO),true)
LDFLAGS += -Xlinker --allow-multiple-definition
LIBS += $(PIXODRV) $(PIXOMATIC_LIBS)
CXXFLAGS += -I$(PIXODRV_INC) -I$(PIXOMATIC_INC)
endif

endif

#-----------------------------------------------------------------------------
# Files.
#-----------------------------------------------------------------------------

OBJS =		\
	SDLLaunch.o

ifeq ($(TARGETTYPE),macosx)
  OBJS += mac_lame_ass_cd_check.o
endif

OUT = $(SDLLAUNCH)

#-----------------------------------------------------------------------------
# Rules.
#-----------------------------------------------------------------------------

# Use general rules.
EXECUTABLE	= 1
include $(UNREAL_DIR)/makefile-general

#-----------------------------------------------------------------------------
# The End.
#-----------------------------------------------------------------------------
