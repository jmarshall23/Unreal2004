# ====================================================
#  MathEngine Canada                          (c) 2001
#
#  Makefile for example based on generic template and
#  common core rules and definitions.
#
#  Requires makefile.lib and makefile.common
# ====================================================

# The desired name for the resulting executable
OUTNAME 	:= LoadPfb
OUTTYPE		:= EXE

# The name of the directory that this example is found in.
DIRNAME 	:= $(OUTNAME)

# The list of source files needed for compilation.
SOURCES 	:= $(OUTNAME).cpp

MAKEDIR		:= ..

# Offset for proper reference to includes/library
T_SRC_DIR	:= ..

# List of libraries to link against.
# Appears verbatim on link line, if tuning/customizing
LIBPATH_PERFORMER = \
		-L/usr/lib32 \
		-L/usr/lib32/libpfdb \
		-L/usr/lib32/Performer \
		-L/usr/lib32/Performer/libpfdb \
		-L../../lib.rel/irix_double 

LIBS_PERFORMER = -lpfpfb_ogl -lpfdu_ogl -lpfutil_ogl  -lpf_ogl -lGLU -lGL -lXsgivc -lXext -lXmu -lX11 -lfpe -lm

LIB_LIST 	:= $(LIB_ALL) -lMcdPerformer $(LIBPATH_PERFORMER) $(LIBS_PERFORMER)

# The generic rules for building MathEngine examples
MAKESYS_DIR =../../../build/makerules
MAKE_RULES:=$(MAKESYS_DIR)/makefile.common

include $(MAKE_RULES)

$(MAKE_RULES): $(MAKE_RULES).in
	${MAKE} -C$(MAKESYS_DIR) makefile.common
