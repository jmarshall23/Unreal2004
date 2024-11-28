# These are the definitions that will be used in make's rules
# Configure doesn't really have to know about local definitions.
# This file is included at the beginning of the makefile

OUTNAME:=MeViewer2

MODULE_MAY_CONTAIN_EAC_CODE	:= 1
CREATE_PREEACED_SOURCE		:= 1

SOURCES_BASIC:= \
	frame/MeViewer.c \
	frame/RMenu.c \
	frame/RMouseCam.c \
	frame/RGeometryUtils.c

# This is not used
SOURCES_benchmark:=\
	backends/Render_benchmark.c

SOURCES_null:=\
	backends/Render_null.c

SOURCES_ogl:=\
	backends/Init_ogl.c \
	backends/Render_ogl.c

SOURCES_ps2:=\
	backends/Input_ps2.c

SOURCES_xbox:=\
        backends/Render_xbox.c
	
# Here and elsewhere we assume only platform with d3d is win32 and xbox.
SOURCES_d3d:=\
	backends/Init_d3d.c \
	backends/Render_d3d.c

#################################################################
#		Modify Cmd Line Macros
#

DEFINES += WITH_NULL  
ifeq '$(PLATFORM)' 'ps2'
	DEFINES +=  WITH_BENCHMARK WITH_PS2 WITH_OPENGL
endif
ifeq '$(PLATFORM)' 'xbox'
	DEFINES +=  
	WITH_BENCHMARK = NO
endif
ifeq '$(PLATFORM)' 'ngc'
	DEFINES +=
endif
ifeq '$(PLATFORM)' 'linux'
	DEFINES +=  WITH_BENCHMARK WITH_OPENGL
endif
ifeq '$(PLATFORM)' 'linux_hx'
	DEFINES +=  WITH_BENCHMARK # WITH_OPENGL
endif
ifeq '$(PLATFORM)' 'linux_hx_cc'
	DEFINES +=  WITH_BENCHMARK # WITH_OPENGL
endif
ifeq '$(PLATFORM)' 'win32'
    DEFINES += WITH_BENCHMARK WITH_OPENGL _ME_OGL_DELAYLOAD 
    ifneq '$(WITH_D3D)' 'NO'
	ifeq '$(DEF_PRECISION)' 'SINGLE'
	    DEFINES += WITH_D3D _ME_D3D_DELAYLOAD 
	endif
    endif
endif
UNDEFINES = 
# This would be a specific file modifier:
# DEFINES_yes.c = -DWITH_OPENGL -DWITH_NULL
# UNDEFINES_yes.c = -DWITH_NULL
#################################################################

#################################################################
#		Source File Definition
# accumulate groups of sources into the SOURCES variable
SOURCES:=$(SOURCES_BASIC)

ifneq '' '$(findstring WITH_BENCHMARK, $(DEFINES))'
SOURCES:=$(SOURCES) $(SOURCES_benchmark)
endif

ifneq '' '$(findstring WITH_NULL, $(DEFINES))'
SOURCES:=$(SOURCES) $(SOURCES_null)
endif

ifneq '' '$(findstring WITH_OPENGL, $(DEFINES))'
SOURCES:=$(SOURCES) $(SOURCES_ogl)
endif

ifneq '' '$(findstring WITH_PS2, $(DEFINES))'
SOURCES:=$(SOURCES) $(SOURCES_ps2)
endif

ifneq '' '$(findstring WITH_PS2HIG, $(DEFINES))'
SOURCES:=$(SOURCES) $(SOURCES_ps2hig)
MODULE_INCLUDE_PATH+=backends/micro
endif

ifneq '' '$(findstring _XBOX, $(DEFINES))'
SOURCES:=$(SOURCES) $(SOURCES_xbox)
endif

ifneq '' '$(findstring WITH_D3D, $(DEFINES))'
SOURCES:=$(SOURCES) $(SOURCES_d3d)
endif

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
MODULE_INCLUDE_PATH += ../include

#################################################################
#		System Make Rules & Include
MAKESYS_DIR =../../build/makerules
MAKE_RULES:=$(MAKESYS_DIR)/makefile.common

include ${MAKE_RULES}

$(MAKE_RULES): $(MAKE_RULES).in
	$(MAKE) -C$(MAKESYS_DIR) makefile.common
