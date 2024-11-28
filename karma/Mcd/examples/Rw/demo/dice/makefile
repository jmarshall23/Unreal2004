###########################################################################
#
# File     : Makefile
#
# Abstract : Command line makefile used to build a RenderWare demo program.
#            This is a GNU Make makefile. A copy of GNU Make is required in 
#	     order to use this file.
#
###########################################################################
#
# This file is a product of Criterion Software Ltd.
#
# This file is provided as is with no warranties of any kind and is
# provided without any obligation on Criterion Software Ltd. or Canon Inc. 
# to assist in its use or modification.
#
# Criterion Software Ltd. will not, under any circumstances, be liable for 
# any lost revenue or other damages arising from the use of this file.
#
# Copyright (c) 1999 Criterion Software Ltd.
# All Rights Reserved.
#
# RenderWare is a trademark of Canon Inc.
#
###########################################################################

#
# include user custom options file if it exists
#
ifndef RWOPTIONS
RWOPTIONS = ../options.mak
endif
include $(RWOPTIONS)

# all directories are defined relative to DEMODIR
ifndef DEMODIR
DEMODIR = $(CURDIR)
# setting BASDEIR=$(CURDIR) enables absolute path names
# which makes a better job of debug symbols for profiling. debugging etc.
# see also http://www.fsf.org/software/make/make.html
endif

ifeq ($(RWMETRICS), 1)
RWLIBDIR = $(RWSDK)/lib/$(RWTARGET)/metrics
CFLAGS += -DRWMETRICS
else
ifeq ($(RWDEBUG), 1)
RWLIBDIR = $(RWSDK)/lib/$(RWTARGET)/debug
CFLAGS += -DRWDEBUG
else
RWLIBDIR = $(RWSDK)/lib/$(RWTARGET)
endif
endif

RWINCDIR = $(RWSDK)/include/$(RWTARGET)

# We use the demoskel's terminal

CFLAGS += -DRWTERMINAL

# Define Local Common Directories & Files

SKEL   = $(DEMODIR)/../../skel
SKELPS = $(SKEL)/platform
COMMON = $(DEMODIR)/../democom
SRC    = $(DEMODIR)/src

OBJ = \
	$(SRC)/dice.$(O) \
	$(COMMON)/camera.$(O) \
	$(COMMON)/padmap.$(O) \
        $(SKEL)/skeleton.$(O) \
	$(SKEL)/terminal.$(O) \
	$(SKELPS)/$(RWOS).$(O)

ifeq ($(RWOS), win)
OBJ += $(SKEL)/platform/win.res
endif

RWLIBS = \
	$(RWLIBDIR)/$(LP)rtpng.$(L) \
	$(RWLIBDIR)/$(LP)rprandom.$(L) \
	$(RWLIBDIR)/$(LP)rtcharse.$(L) \
	$(RWLIBDIR)/$(LP)rpworld.$(L) \
	$(RWLIBDIR)/$(LP)rwcore.$(L)   

ifeq ($(RWLOGO), 1)
RWLIBS += $(RWLIBDIR)/$(LP)rplogo.$(L)
CFLAGS += -DRWLOGO
endif

ifeq ($(RWMOUSE), 1)
OBJ += $(SKEL)/mouse.$(O)
CFLAGS += -DRWMOUSE
endif

ifeq ($(RWMETRICS), 1)
OBJ += $(SKEL)/vecfont.$(O) $(SKEL)/metrics.$(O)
endif

# Define the projects to be built

PROJS = dice.$(EXE)

all:  demo

include ../../makecom/$(RWCOMPILER)

CFLAGS += -I$(RWINCDIR) -Isrc -I$(COMMON) -I$(SKEL) -I$(SKELPS)

# Build it all

demo:  $(PROJS)

$(PROJS): $(OBJ) $(RWLIBS)
	$(LD) $(SYSOBJ) $(OBJ) $(RWLIBS) $(SYSLIBS) $(LOUT)$@ $(LFLAGS)

run: $(PROJS)
	$(RUN) $(PROJS)

distclean: clean
	$(RM) *.$(EXE)

clean:
	$(RM) $(OBJ)

# Rules

%.obj : %.c
	$(CC) $(CFLAGS) $(COUT)$@ $^

