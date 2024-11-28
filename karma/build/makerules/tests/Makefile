# $Header: /proj/Toolkits/build/makerules/tests/Makefile,v 1.1.8.1 2002/04/11 03:45:35 jamesg Exp $
# $Name: t-stevet-RWSpre-030110 $

# vim:syntax=make:isk+=&,-,.

#######################################################################
#
# Check out cowpat using various tests.
#
# Nothing too clever, just loads of hand-written tests
#

export q:=@
export indent:=

&tests:=Makefile.cowpatdefs

.PHONY: all ${&tests}
all: ${&tests}

${&tests}: Makefile.%:
	${q}echo '${MAKELEVEL} ${indent}[$(patsubst Makefile.%,%,$@)]'
	${q}${MAKE} --no-print-directory -f $@
