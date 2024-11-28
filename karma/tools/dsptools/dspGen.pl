#!/usr/bin/perl

use strict;
use Cwd;
use Getopt::Long;

# This is also a permitted substitution variable
my $substExeDir = getExeDir();
# Add the pl file location to include path 
unshift (@INC, "$substExeDir");

my $templateDir = "$substExeDir/templates";

require "dspUtil.pm";
my $substRelativeDir = dspUtil::getRelativeDir($substExeDir);
#
require 'source2dsp.pl';
require 'dsp2source.pl';

my $flagVerbose = 0;
my $flagWarnings = 0;

if (!defined $ARGV[0])
{
    print "\nUSAGE: $0 template [dspname]\n\n";
    exit 1;
}

my $template = $ARGV[0];
my $output  = $ARGV[1];
$template =~ s/\.dsp$// if (defined $template); # remove .dsp if present
$output   =~ s/\.dsp$// if (defined $output);   # remove .dsp if present

my ($name,$raSourceFiles) = sourceGetFilesFromMakefile();
die "No source files found in makefile\n" if (!$raSourceFiles);
die "No output name found in makefile\n" if (!defined $output and !defined $name);
my $output ||= $name;


dspFromTemplate($output,$template,
                $templateDir,$substRelativeDir,
                $raSourceFiles);

exit 0;

###############################################################################
sub getExeDir
  {
	# The following gets the current path of the this file 
	my $exeDir   = ($0 =~ m/(.*\/)[^\/]/) ? $1 : undef; # linux dir
	$exeDir    ||= ($0 =~ m/(.*\\)[^\\]/) ? $1 : undef; # dos/win dir
	$exeDir    ||= '.';

	# get full path (convert from ../something to /here/there/something)
	my $oldDir = cwd;
	chdirSafe($exeDir);
	$exeDir = cwd();
	chdirSafe($oldDir);

	return $exeDir;
  }
###############################################################################
sub chdirSafe
  {
	my ($dir) = @_;

	return chdir($dir) if ((defined $dir) and ($dir ne ''));
	return 1;
  }
