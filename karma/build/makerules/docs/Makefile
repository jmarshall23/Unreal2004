# $Id: Makefile,v 1.3.2.1 2002/04/11 03:45:35 jamesg Exp $
# $Name: t-stevet-RWSpre-030110 $

# vim:syntax=make:

# At the moment I create html using two methods sop that I can compare &
# contrast.

.PHONY: all
all: cowpat.html cowinfo.html ;

cowpat.html: cowpat.texi Makefile
	perl -S texi2html --no-section_navigation -split none -number $< -o $@

cowinfo.html: cowpat.texi Makefile
	makeinfo --html --force -o $@ $<

# cowold: Use old-fashioned version of texi2html
cowold.html: cowpat.texi Makefile
	perl -S texi2html -glossary -monolithic $< 

cowpat.dvi: cowpat.texi
	texi2dvi --clean --quiet -V $<

#SGML_CATALOG_FILES=C:\Tools\sgml\docbook-dtd-3.1\docbook.cat;C:\Tools\sgml\docbook-dsssl-1.73\catalog;C:\Tools\OpenJade-1.3\dsssl\catalog
#openjade -t html Bash-Prompt-HOWTO.sgml 2>1
