#!/usr/bin/perl

# this can be used instead of speechd, but it spawns festival for
#  every single line of text!
#
# You can't just hook up festival to a named pipe directly, since it doesn't
#  make noise until the whole file is read, not line-by-line.
#
# You should really install speechd ( http://www.speechio.org/speechd.html )
#  instead. Speechd can be used with rsynth and IBM viavoice, too. In that
#  case, point ut2004 at /dev/speech instead.
#
# --ryan, Epic Games.

use warnings;
use strict;

my $fn = "/tmp/speechpipe";
$fn = $ARGV[0] if defined $ARGV[0];
die("Can't open $fn: $!\n") if (not open(SPEECHPIPE, '<', $fn));

while (<SPEECHPIPE>) {
    chomp;
    #print "$_\n";
    system("echo \"$_\" |festival --tts");
}

close SPEECHPIPE;
exit 0;

