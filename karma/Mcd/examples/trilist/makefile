# ====================================================
#  MathEngine Canada                          (c) 2001
#
#  Makefile for example based on generic template and
#  common core rules and definitions.
#
#  Requires makefile.lib and makefile.common
# ====================================================

# The desired name for the resulting executable
OUTNAME 	:= trianglelist
OUTTYPE		:= EXE

# The name of the directory that this example is found in.
DIRNAME 	:= $(OUTNAME)

# The list of source files needed for compilation.
SOURCES 	:= trianglelist.cpp

MAKEDIR		:= ..

# Offset for proper reference to includes/library
T_SRC_DIR	:= ..

# List of libraries to link against.
# Appears verbatim on link line, if tuning/customizing
LIB_LIST 	:= $(LIB_ALL)

# The generic rules for building MathEngine examples
MAKESYS_DIR =../../../build/makerules
MAKE_RULES:=$(MAKESYS_DIR)/makefile.common

include $(MAKE_RULES)

$(MAKE_RULES): $(MAKE_RULES).in
	${MAKE} -C$(MAKESYS_DIR) makefile.common
