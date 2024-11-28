# ====================================================
#  Copyright (c) 1997-2002 MathEngine PLC
#
#  Makefile for example based on generic template and
#  common core rules and definitions.
#
#  Requires makefile.lib and makefile.common
# ====================================================

# The desired name for the resulting executable
OUTNAME 	:= ConeUserGeometry
OUTTYPE		:= EXE

# The name of the directory that this example is found in.
DIRNAME 	:= $(OUTNAME)

# The list of source files needed for compilation.
SOURCES 	:= ConeSample.c \
                   ConeGeometry.c


# The location of the common makefile elements
MAKEDIR		:= ..

# Settings for the common code for building MathEngine examples
MAKESYS_DIR 	 = ../../build/makerules
MAKE_RULES	:= $(MAKESYS_DIR)/makefile.common

# List of libraries to link against.
LIB_LIST	:= $(LIB_ALL)

# The generic rules for building MathEngine examples
include $(MAKE_RULES)
$(MAKE_RULES): $(MAKE_RULES).in
	$(MAKE) -C$(MAKESYS_DIR) makefile.common
