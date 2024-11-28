#=============================================================================
# Unreal OpenAL makefile for Linux.
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

INC_DIRS	= $(ALAUDIO_INC)
SRC_DIRS	= $(ALAUDIO_SRC)
BUILD_DIR	= $(ALAUDIO_BUILD)

#-----------------------------------------------------------------------------
# Compiler/linker options.
#-----------------------------------------------------------------------------

CXX		= $(STD_CXX)
CXXFLAGS	= -I$(OPENAL_INC) $(STD_CXXFLAGS) $(EXT_CXXFLAGS) -I$(ALAUDIO_INC) \
			-I$(ENGINE_INC) -I$(SPEEX_INC) \
			-I$(OGG_INC) -I$(VORBIS_INC) -DAILCALL="" \
			-I$(CORE_INC) -DGPackage=GPackageALAudio -DThisPackage=ALAudio
LDFLAGS		= $(STD_LDFLAGS) $(EXT_LDFLAGS)$(ALAUDIO_SONAME)
LIBS		= $(STD_LIBS) $(CORE) $(ENGINE) $(OGG) $(VORBIS)

ifeq ($(strip $(TARGETTYPE)),win32)
  CXXFLAGS += -I../../DirectX9/include
  LDFLAGS += /LIBPATH:"../../DirectX9/lib"
endif

ifeq ($(strip $(TARGETTYPE)),win64)
  CXXFLAGS += -I../../DirectX9/include
  # !!! FIXME: CXXFLAGS += -I../../DirectX9_AMD64/include
  LDFLAGS += /LIBPATH:"../../DirectX9_AMD64/lib/amd64"
endif

#-----------------------------------------------------------------------------
# Files.
#-----------------------------------------------------------------------------

WINDLLEXP := ALAUDIO_API
PRECOMPILED_HEADER := ALAudioPrivate.h
PRECOMPILE_ON := ALAudio.cpp

OBJS =			\
	ALAudio.o   \
	ALAudioSubsystem.o \
	ALAudioStream.o \
	ALAudioCapture.o \
	ALAudioVoice.o

OUT = $(ALAUDIO)

#-----------------------------------------------------------------------------
# Rules.
#-----------------------------------------------------------------------------

# Use general rules.
include $(UNREAL_DIR)/makefile-general

#-----------------------------------------------------------------------------
# The End.
#-----------------------------------------------------------------------------
