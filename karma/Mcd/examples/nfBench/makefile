# ====================================================
#  MathEngine Canada                     (c) 2000-2001
#
#  Makefile for example based on generic template and
#  common core rules and definitions.
#
#  Requires makefile.lib and makefile.common
# ====================================================

# The desired name for the resulting executable
OUTNAME 	:= nfBench

# The name of the directory that this example is found in.
DIRNAME 	:= ${OUTNAME}

# The list of source files needed for compilation.
SOURCES 	:= ${OUTNAME}.cpp

# The location of the common makefile elements
MAKEDIR		:= ..

# Global definitions of library and grouping macros
include $(MAKEDIR)/makefile.lib

LIB_LIST 	:= -L../../../lib.rel/${PLATFORM} ${LIB_ALL}
LIB_LIST_DEBUG 	:= -L../../../lib.dbg/${PLATFORM} ${LIB_ALL} -lfpe
LIB_LIST_CHECK 	:= -L../../../lib.chk/${PLATFORM} ${LIB_ALL}

# The generic rules for building MathEngine examples
include $(MAKEDIR)/makefile.common

# bench: nfBench.cpp
# 	./nfBench -sphere -box -approach

