# Assuming GNU make (>v3.76)
# $Id: copyifnewer.mk,v 1.2.30.1 2002/04/11 03:45:34 jamesg Exp $
# ($Name: t-stevet-RWSpre-030110 $)

# vim:syntax=make:
#
# Copyright (c) MathEngine PLC 2000-2001
# This file is part of cowpat make

#######################################################################
#
# Simple makefile to copy one file to another if it is out of date.
#
# No error handling, help, or much else really (well, OK then here's a
# hint: specify ${src} and ${dst} on the command line).

.SUFFIXES:

${dst}: ${src} ; cp ${src} ${dst}
