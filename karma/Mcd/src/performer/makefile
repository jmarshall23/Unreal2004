# These are the definitions that will be used in make's rules
# Configure doesn't really have to know about local definitions.
# This file is included at the beginning of the makefile

SOURCES_BASIC:=\
	McdPerformer.cpp \

OUTNAME:=McdPerformer

SOURCES:=$(SOURCES_BASIC)

#################################################################
#		Modify Cmd Line Macros
#
DEFINES   = WITH_OPENGL WITH_NULL
UNDEFINES = 
# This would be a specific file modifier:
# DEFINES_yes.c = -DWITH_OPENGL -DWITH_NULL
# UNDEFINES_yes.c = -DWITH_NULL
#################################################################

#################################################################
#		Produce Brief Output 
#		comment for command line output
VISIBLE_COMMANDS ?= none

#################################################################
#		Top Source Directory
#		if different from ./
T_SRC_DIR:=../

#################################################################
#		Extra Include Paths (relative to t_src_dir)
MODULE_INCLUDE_PATH = frame cx math util mesh

#################################################################
#		System Make Rules & Include
MAKESYS_DIR = ../../../build/makerules
MAKE_RULES:=$(MAKESYS_DIR)/makefile.common

$(MAKE_RULES): $(MAKE_RULES).in
	${MAKE} -C$(MAKESYS_DIR) makefile.common

include ${MAKE_RULES}

#################################################################
#		Local Module Rules
#

LIBPATH_PERFORMER = \
		-L/usr/lib32 \
		-L/usr/lib32/libpfdb \
		-L/usr/lib32/Performer \
		-L/usr/lib32/Performer/libpfdb \
		-L../../../lib.rel/irix_double 

LIBS_PERFORMER = -lpfpfb_ogl -lpfdu_ogl -lpfutil_ogl  -lpf_ogl -lGLU -lGL -lXsgivc -lXext -lXmu -lX11 -lfpe -lm



ptest: test.o
	${CXX} $< -o ptest ${LIBPATH_PERFORMER} -lMcdPerformer -lMcdTriangleMesh -lMcdPrimitives -lMcdFrame -lMeGlobals -lMeMessage ${LIBS_PERFORMER}
test.o: test.cpp
	${CXX} -DWITH_MAIN -DIRIX -g -I../../include -I../../../MeGlobals/include -I../mesh -I/usr/include -c -o $@ $<

	# ${CXX} -DWITH_MAIN -g -c -o test.o $<
EXTRA_CLEANS = test.o ptest
