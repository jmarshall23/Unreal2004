# ====================================================
#  MathEngine Canada                     (c) 2000-2001
#
#  Makefile for example based on generic template and
#  common core rules and definitions.
#
#  Requires makefile.lib and makefile.common
# ====================================================

# The desired name for the resulting executable
OUTNAME 	:= chair

# The name of the directory that this example is found in.
DIRNAME 	:= $(OUTNAME)

# The list of source files needed for compilation.
SOURCES 	:= $(OUTNAME).c

# The location of the common makefile elements
MAKEDIR		:= ..

VIEWER	:= MEVIEWER2

# Global definitions of library and grouping macros
include $(MAKEDIR)/makefile.lib

# List of libraries to link against.
# Appears verbatim on link line, if tuning/customizing
LIB_LIST 	:=  -lMeApp -lMst -lMcdPrimitives -lMcdFrame -lMdt -lMdtKea -lMdtBcl -lMeViewer2 -lMeGlobals -lglut -lGL -lGLU -lXmu -lX11 -lm
LIB_LIST_DEBUG 	:= -lMeApp_debug -lMst_debug -lMcdPrimitives_debug -lMcdFrame_debug -lMdt_debug -lMdtKea_debug -lMdtBcl_debug -lMeViewer2_debug -lMeGlobals_debug -lglut -lGL -lGLU -lXmu -lX11 -lm
LIB_LIST_CHECK	:= $(LIB_ALL_CHECK)

# The generic rules for building MathEngine examples
include $(MAKEDIR)/makefile.common
