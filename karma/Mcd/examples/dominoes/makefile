# ====================================================
#  MathEngine Canada                     (c) 2000-2001
#
#  Makefile for example based on generic template and
#  common core rules and definitions.
#
#  Requires makefile.lib and makefile.common
# ====================================================

# The desired name for the resulting executable
OUTNAME := dominoes

# The name of the directory that this example is found in.
DIRNAME := ${OUTNAME}

# The list of source files needed for compilation.
SOURCES := cdDominoes.c

# The location of the common makefile elements
MAKEDIR		:= ..

# Global definitions of library and grouping macros
include $(MAKEDIR)/makefile.lib

# List of libraries to link against.
# Appears verbatim on link line, if tuning/customizing
LIB_LIST 	:= -L../../../lib.rel/${PLATFORM} $(LIB_ALL)
LIB_LIST_DEBUG 	:= -L../../../lib.dbg/${PLATFORM} $(LIB_ALL)
LIB_LIST_CHECK	:= -L../../../lib.chk/${PLATFORM} $(LIB_ALL)

# LIB_LIST := \
# 	${LIB_MCDPRIMITIVES}\
# 	${LIB_MCDFRAME}\
# 	${LIB_MDTALL} \
# 	${LIB_RENDER} \
# 	${LIB_GLOBAL} \
# 	${LIB_SYSTEM}

# LIB_LIST_DEBUG := \
# 	${LIB_MCDPRIMITIVES_DEBUG} \
# 	${LIB_MCDFRAME_DEBUG} \
# 	${LIB_MDTALL_DEBUG} \
# 	${LIB_RENDER_DEBUG} \
# 	${LIB_GLOBAL_DEBUG} \
# 	${LIB_SYSTEM_DEBUG}

# The generic rules for building MathEngine examples
include $(MAKEDIR)/makefile.common
