#! /bin/bash
#
# $Id: intent-builds-in-visualc.sh,v 1.3.2.1 2002/04/11 03:45:34 jamesg Exp $
# $Name: t-stevet-RWSpre-030110 $
#
# HACK to run make for intent, and post-porcess the error messages so that
# they look that VC++ errors. This allows you to do standard <F7>/<F4>
# compile-edit cycles under VC++ (sad, I know).
#
# This has to be bash, due to the use of the PIPESTATUS array

set -e
set -u
# set -x

warn() { echo "$@" >&2; }
die() { warn "$@"; exit 32; }

#export INTENTROOT=${INTENTROOT:-C:/Tools/Tao/intent1.2-pj12}
#export INTENTROOT=${INTENTROOT:-C:/Projects/tao/intent222}
export ELATEIMG=${ELATEIMG:-mothtools.img}

# Some magic to (magically) run stderr through perl
exec 3>&1;

# * Arbitrarily, do not convert carriage return warnings
# * Convert elate filenames back to dos
# * Convert to relative filenames
# * ignore errors in Ben's sound stuff for now
 
"$@" 2>&1 1>&3 3>&- | 1>&2 3>&- perl -pe '
		BEGIN { ($pwd = "$ARGV[0]/") =~ s|\\|/|g; @ARGV = ("-"); }
		s|^(\S*):(\d+):\s|$1($2) : | unless m{carriage return|as (un)?signed due to prototype|warning: unused parameter |^/lang/.* warning:|^sound/};
		s|^/|$ENV{INTENTROOT}/|o;
		s|^device/hostfs/(?=[a-z]:/)||;
		s|^\Q$pwd|./|o;
	' $(cygpath -wa $(pwd))

#echo "PIPESTATUS=[${PIPESTATUS[0]}]"
exit ${PIPESTATUS[0]} # We have to exit with elate's exit code, not perl's

