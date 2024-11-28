#! /bin/bash
# ($Id: genmak.sh,v 1.4.18.1 2002/04/11 03:45:34 jamesg Exp $)
# ($Name: t-stevet-RWSpre-030110 $)
#
# A Make helper
#
# Puts a set of makerules into $mak that build dependencies and object
# files. Doing it here in a shell script rather than in a makefile has
# several advantages:
#
#	* Shell quoting is a bit more amenable (we have to echo things that
#	  look like ("echo 'blah blah'). Nesting of quotes inside make was
#	  getting a bit fraught.
#
#	* Multiple echo lines in make are unaccountably slow on will's cygwin
#
#	* We can more easily kill intermediate files if something goes wrong
#

set -e
set -u

platform=$1; shift
makerulesdir=$1; shift
lq=$1; shift
rq=$1; shift
depcmd=$1; shift
obj=$1; shift
compilecmd=$1; shift
targetname=$1; shift
src=$1; shift # $<
mak=$1; shift # $@

#echo "platform=$platform" >&2
#echo "makerulesdir=$makerulesdir" >&2
#echo "lq=$lq" >&2
#echo "rq=$rq" >&2
#echo "depcmd=$depcmd" >&2
#echo "obj=$obj" >&2
#echo "compilecmd=$compilecmd" >&2
#echo "targetname=$targetname" >&2
#echo "src=$src" >&2
#echo "mak=$mak" >&2
#exit 99

builddir=${mak%/*}/
dep=${mak%%.mak}.d
intermediate=${dep}1
srcstem=${src#*/}
srcstem=${srcstem%.*}

trap '[ $? != 0 ] && { echo "cowpat: removing $mak"; rm -f $mak; }' EXIT

q='${q}'
nl='
'

case $platform in
	linux|elate)
		gendepscmd="${q}$depcmd >$intermediate"
		gendepscmd="$gendepscmd$nl	${q}sed -n"
		gendepscmd="$gendepscmd -e ${lq}s|$srcstem\.o[ :]*|${builddir}$srcstem.o $dep: |;p$rq"
		gendepscmd="$gendepscmd -e ${lq}s/.*://;s/^[ 	][ 	]*//;s/\\\\\$\$//;s/ *\$\$/:/;H;\$\${g;p;}${rq}"
		gendepscmd="$gendepscmd <$intermediate >$dep"
		;;
	win32)
		gendepscmd="${q}perl -w ${makerulesdir}makdeps.pl '$depcmd' '$builddir' '$dep'"
		;;
	*)
		echo "$0: Can't generate dependency command for platform $platform" >&2
		exit 33
		;;
esac

cat <<EOMAK >$mak
$dep:
	${q}echo ${lq}cowpat: ${targetname} - gendeps .d from $src$rq
	${q}rm -f $intermediate
	$gendepscmd

-include $dep

$obj:
	${q}$compilecmd

EOMAK
