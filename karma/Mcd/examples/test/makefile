# ====================================================
#  MathEngine Canada                     (c) 2000-2001
#
#  Makefile for example based on generic template and
#  common core rules and definitions.
#
#  Requires makefile.lib and makefile.common
# ====================================================

# The desired name for the resulting executable
OUTNAME := test

# The name of the directory that this example is found in.
DIRNAME := ${OUTNAME}

# The list of source files needed for compilation.
SOURCES := McdIntersect.t.cpp

# The location of the common makefile elements
MAKEDIR		:= ..

# Global definitions of library and grouping macros
include $(MAKEDIR)/makefile.lib

# List of libraries to link against.
# Appears verbatim on link line, if tuning/customizing
LIB_LIST 	:= $(LIB_ALL)
LIB_LIST_DEBUG 	:= $(LIB_ALL_DEBUG)
LIB_LIST_CHECK	:= $(LIB_ALL_CHECK)

# The generic rules for building MathEngine examples
include $(MAKEDIR)/makefile.common
