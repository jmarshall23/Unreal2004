#! /bin/perl
#
# $Id: makdeps.pl,v 1.13.2.1 2002/04/11 03:45:34 jamesg Exp $
# $Name: t-stevet-RWSpre-030110 $
#
# Runs through Visual C++ pre-processed C looking for "#line " lines, and
# uses these to generate make dependencies in the output file. No output
# file is created if the compiler exits with an error.
#
# Opens a read pipe to the C compiler, and the output file directly. This
# is helpful in Makefiles, where shell trickery would otherwise have to be
# used to detect errors in all components of a pipeline.
#
# Note1:
#	The name of the file being pre-processed is extracted from the first
#	line of the pre-processed output.
#
# Note2:
#	System headers are excluded (those under */VC98/INCLUDE/..)

use strict;

die "Usage: $0 compile-command builddir output-file\n" if 3 != @ARGV;

my $pfix = "makdeps.pl:";

my $compilecmd = $ARGV[0];
my $builddir = $ARGV[1];
my $output = $ARGV[2];

# Might need
#use sigtrap qw(die normal.signals);
# to ensure cleanup?
END
{
	my $exit = $?;
	close OUT;
	if ($exit)
	{
		unlink $output and warn "$pfix Removing $output\n";
	}
} # END

# Builddir should end in a slash
die "$pfix builddir ($builddir) should end in a slash" if $builddir !~ m|/$|;

# Set up magic to read the input
local $SIG{PIPE} = sub { die "$pfix compiler pipe broke [$!]" };
# HACK: cygwin perl hangs on compiler read pipe open unless we wrap it with a "sh -c"
my $compiler = "$compilecmd|";
if ($^O =~ /cygwin/i)
{
	warn "$pfix You are using a cygwin perl release. Install a proper win32 perl for better speed.\n";
	$compiler = "sh -c '$compilecmd'|";
}
open (IN, $compiler) or die "$pfix Cannot connect to compiler [$!]\n";

# Read the input
# First line is the C file
$_ = <IN>;
unless (defined ($_))
{
	close IN; # Force $? to be set
	my $err = $? >> 8;
	die "$pfix Couldn't read from compiler, retcode $err [$!]\n"
}

/^#line 1 "([^"]+)\.(\w+)"/ or die "$pfix Compiler did not provide filename on line 1: [$_]\n";
my $file = $1;
my $ext = $2;

# Prepare the output before the perl
open OUT,">$output" or die "$pfix Cannot open output file ($output) [$!]\n";
binmode OUT;

# Put included files as keys of %seen
my %seen = ();
binmode IN; # Increases speed on win32 -- look out for superfluous \r
$/ = "\n#line "; # Decrease number of reads
while (<IN>)
{
	next unless /^\d+ "([^"]*)"/;
	$_ = $1;
	tr{\\}{/}s;
	next if m|/VC98/INCLUDE/|;	# NB: System headers excluded
	$seen{$_} = undef;
} # while

=pod
# A useful improvement is gained by reading in blocks rather than lines.
# However, the runtime is all in cl.exe at the moment, so it's not (quite)
# worth the complication:
	$/ = \8192;
	my $ll = ''; # last line of block
	while (<IN>)
	{
		$ll .= $_;
		while ($ll =~ /^#line \d+ "([^"]*)"/mg)
		{
			my $f = $1;
			$f =~ tr{\\}{/}s;
			next if $f =~ m|/VC98/INCLUDE/|;	# NB: System headers excluded
			++$seen{$f};
		}
		$ll = substr $_,rindex $_,"\n";
		#warn "[$ll]\n" if $ll =~ /#line/;
	}
=cut

if (close IN and $!) { die "$pfix Couldn't close connection to compiler [$!]\n"; }
my $compilerret = $? >> 8;
die "$pfix Compiler didn't complete, status = $compilerret [$!]\n" if $?;

delete $seen{"$file.$ext"}; # Ensure $file.$ext gets added only once

my $objfile = "$file.obj";
$objfile =~ s|.*/||;	# Strip any path prefix (the object lives in $builddir)

print OUT "$builddir$objfile: $file.$ext";
print OUT map {" \\\n $_"} sort keys %seen;
print OUT "\n\n";
# Now mention each depency file on its own, followed by a colon.
# WHY? See comment about "Rules without Commands or Prerequisites" in
# Make.rules
print OUT map {"$_:\n"} sort keys %seen;
close OUT;
