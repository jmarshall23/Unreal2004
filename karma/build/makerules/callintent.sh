#!/bin/bash
#
# $Id: callintent.sh,v 1.26.2.1 2002/02/26 18:25:27 piercarl Exp $
# $Name: t-stevet-RWSpre-030110 $
#
#
# HACK to use elate (or elatecon under win32) to run a command under
# intent, and then to pass the exit code back out.
#
# Actually there's quite a lot of assertion in this file in an attempt
# to pass a sensible error message back to the caller.
#
# A lot of the magic is to work around the fact that elatecon doesn't
# pass back return codes or stderr properly. Sigh.
#
# Even more subtlety is introduced by working on both win32 and
# linux. The latter doesn't seem to mount things, hence the 'ln -s'
# hack.

set -e
set -u

warn() { echo "$@" >&2; }
die() { warn "$@"; exit 32; }

# Definition for cygpath if you don't have it (i.e., you are on proper
# unix, not cygwin)
if [ -z $(type -t cygpath) ]; then
	cygpath() {
		case $1 in
		-*) case $2 in
			/*) echo "$2";;
			*/*)
				case $1 in
				*a*) dir=${2%/*}; if [ -n "$dir" ]; then cd "$dir"; fi; echo "$PWD/${2##*/}";;
				*) echo "$2";;
				esac
				;;
			*) echo "$PWD/$2";;
			esac
			;;
		*) echo "$1";;
		esac
	}
fi

# Sigh! The make from MinGW on windows can pass an absolute dos-style path as $0
nought=$(cygpath -u "$0")
cmd=${nought##*/}
cmdpath=${nought%/*}/
[ "x$cmdpath$cmd" != "x$nought" ] && die "$cmd: internal error, ($cmdpath$cmd) != ($nought)"

[ -z "$cmd" ] && cmd='-callintent.sh-'

PrintUsage()
{
	cat <<-EOM >&2
	Usage:
	$cmd [-v] [options] intent-commands...

	    -h     : help
	    -v     : verbosity please (or set \$CALLINTENTVERBOSE)
	    -i dir : where the root of your intent tree lives (\$INTENTROOT)
	    -I img : Elate image (\$ELATEIMG, defaults to develop.img)
	    -m mz  : mount _mz_ on /com/mathengine/mz

	    Win32:
	    -a     : Run "elate" rather than "elatecon" (for the ave)

	EOM
}

intentroot=${INTENTROOT:-}
verbose=${CALLINTENTVERBOSE:-}
elateimg=${ELATEIMG:-develop.img}
printusage=
mzdir=
on_linux=$(uname -s | grep -is linux || :) # Warning: might be under -e, hence the "|| :"
if [ -n "$on_linux" ] ; then
	# Probably on linux
	elatecon_path_under_intentroot=sys/platform/linux/ix86
	elateexe=elate
else
	# probably on cygwin
	elatecon_path_under_intentroot=sys/platform/win32
	elateexe=elatecon
fi

while getopts vhi:m:I:a opt; do
	case $opt in
	'v') verbose=yes;;
	'i') intentroot=$OPTARG;;
	'I') elateimg=$OPTARG;;
	'a') elateexe=elate;;
	'h') printusage=yes;;
	'm') mzdir=$OPTARG;;
	'?') # Force 	exit on bad parameters
		echo "Try \"$cmd -h\" for more information" >&2
		exit 3
		;;
	esac
done
shift $[OPTIND - 1]	# Eat parsed arguments

[ -n "$printusage" ] && { PrintUsage; exit 0; }

#intentroot='C:/_2/elate';
# Small sanity check for intent
if [ ! -d "$intentroot" ]; then
	warn -n "$cmd: Can't find intent: "
	if [ -n "$intentroot" ]; then
		warn "\"$intentroot\" is not a directory"
	else
		warn 'You must use -i (or export $INTENTROOT)'
	fi
	exit 1
fi

# Sanity check mz dir

if [ -n "$mzdir" ]; then
	if [ ! -d "$mzdir" ]; then
		warn "$cmd: Can't see [$mzdir]"
		exit 1
	else
		mzdir=$(cygpath -aw "$mzdir")
	fi
fi

host_abspwd=$(cygpath -wa "$PWD")
host_abspwd=${host_abspwd//\\\\/\/} # Convert all '\\' to '/'

declare -r intentroot=${intentroot%/} #strip any trailing slash (could become empty for "/")
declare -r elatecmd=$(cygpath -u "$intentroot/$elatecon_path_under_intentroot/$elateexe")
if [ ! -x "$elatecmd" ]; then
	warn "$cmd: Can't fire up intent: $elatecmd not found"
	exit 1
fi

set -u

if [ -z "$*" ]; then
	warn "$cmd: No intent commands provided (try -h)"
	exit 2
fi

# Work out paths in intent and cygwin
declare -r cygwin_intentroot=$(cygpath -u "$intentroot")

commandfile=me.in.$$
resultfile=me.out.$$
stdoutfile=me.stdout.$$
stderrfile=me.stderr.$$

declare -r cygwin_commandfile="$cygwin_intentroot/$commandfile"
declare -r cygwin_resultfile="$cygwin_intentroot/$resultfile"
declare -r cygwin_stdoutfile="$cygwin_intentroot/$stdoutfile"
declare -r cygwin_stderrfile="$cygwin_intentroot/$stderrfile"
declare -r intent_commandfile="/$commandfile"
declare -r intent_resultfile="/$resultfile"
declare -r intent_stdoutfile="/$stdoutfile"
declare -r intent_stderrfile="/$stderrfile"

unset commandfile resultfile stderrfile # Avoid confusion

trap "rm -f $cygwin_commandfile $cygwin_resultfile $cygwin_stdoutfile $cygwin_stderrfile" EXIT

# In intent, we cd into a directory that corresponds to our current
# working direcory on the host. For windows, this is under /device/hostfs
# in intent land, on linux we have to set a symbolic link.
if [ -z "${on_linux}" ]; then
	host_rootdir_in_elate=/device/hostfs/
else
	host_rootdir_in_elate=linuxslash
fi

intent_cwd=${host_rootdir_in_elate}$host_abspwd

if [ -n "$verbose" ]; then
	warn "$cmd: directory within intent = [$intent_cwd]"
	warn "$cmd: elate command = [$elatecmd]"
	warn "$cmd: elate image = [$elateimg]"
fi

result=9	# errno

intent_echo_commands=
[ -n "$verbose" ] && intent_echo_commands="echo {[[[$*]]]} >=2"

# Build intent shell script in $commandfile
# One great sadness is that elatecon merges any stderr output with stdout.
# Sigh! We have to catch any stderr in a file and cat it later.
if [ -e "$cygwin_commandfile" ]; then
	echo "$cmd: $cygwin_commandfile exists, bailing out" >&2;
	exit 7;
fi

capturestdout=
[ "x$elateexe" = xelate ] && capturestdout=">$intent_stdoutfile"

mzmountcommand=
[ -z "${on_linux}" -a -n "$mzdir" ] &&
	mzmountcommand="devstart {com/mathengine/mz} dev/name_alias -- -d/device/hostfs/{$mzdir} >>(2)$intent_stderrfile"

cat <<-INTENTCMDS >$cygwin_commandfile
	set me.stat 9
	echo \$me.stat >$intent_resultfile
	catch {
		$mzmountcommand
		cd $intent_cwd
		$intent_echo_commands
		$*
	} me.except me.stat {echo $cmd: intent error: \$me.except >=2} $capturestdout >>(2)$intent_stderrfile
	echo \$me.stat >$intent_resultfile
INTENTCMDS

# Run it
set +e
#set -x
elateexitcode=1
if [ -n "$on_linux" ]; then
	set -e
	cd $INTENTROOT
	[ -L $host_rootdir_in_elate ] && rm $host_rootdir_in_elate
	#ln -sf $win32abs_moth2ddir $host_rootdir_in_elate
	if ! ln -s / $host_rootdir_in_elate; then echo "Couldn't make link"; exit 88; fi
	if [ -n "$mzdir" ]; then
	    mkdir -p com/mathengine
	    ln -s "${mzdir%/.}" com/mathengine/mz
	fi
	$elatecmd -i0 -B$elatecon_path_under_intentroot/$elateimg -c"shell $intent_commandfile"
 	if [ -n "$mzdir" ]; then
 	    rm com/mathengine/mz
 	fi
	
elif [ "x$elateexe" = "xelate" ]; then
	# Oh, and yes, for elate, this is the correct quoting (sheesh!)
	$elatecmd -B$elatecon_path_under_intentroot/$elateimg -c\'shell $intent_commandfile\'
else
	$elatecmd "-B$elatecon_path_under_intentroot/$elateimg -c'shell $intent_commandfile'"
fi

if [ $? = 0 ]; then
	[ -f "$cygwin_stdoutfile" ] && cat $cygwin_stdoutfile
	cat $cygwin_stderrfile >&2

	# Report the result
	elateexitcode=$(cat $cygwin_resultfile)

	if [ -z "$elateexitcode" ]; then
		warn "$cmd Warning: dodgy elate exit code ($elateexitcode)"
		elateexitcode=99
	fi


	[ -n "$verbose" ] && warn "$cmd: elatecon exit code: $elateexitcode"
fi
exit $elateexitcode

