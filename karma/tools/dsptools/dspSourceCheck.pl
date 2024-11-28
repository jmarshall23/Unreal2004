#!/usr/bin/perl

use strict;
use Cwd;
use Getopt::Long;

# This is also a permitted substitution variable
my $substExeDir = getExeDir();
my $substWorkingDir = cwd();
# Add the pl file location to include path 
unshift (@INC, "$substExeDir");
#
require 'source2dsp.pl';
require 'dsp2source.pl';

my $flagVerbose = 0;
my $flagWarnings = 0;

GetOptions("verbose" => \$flagVerbose,
           "warnings" => \$flagWarnings);

my $dsp     = (glob('*.dsp'))[0];

# Check files first
die "No makefile found\n" if (!-f "makefile");
die "No $dsp found\n"  if (!-f $dsp);

my ($name,$raSourceFiles) = sourceGetFilesFromMakefile();
die "No source files found in makefile\n" if (!$raSourceFiles);
my $raDspSourceFiles = dspGetFiles($dsp);
die "No source files found in $dsp\n" if (!$raDspSourceFiles);


showFiles("makefile",$raSourceFiles) if ($flagVerbose);
showFiles($dsp, $raDspSourceFiles)   if ($flagVerbose);

my $errors = 0;

foreach my $file (sort keys %{$raDspSourceFiles})
{
    if (!$raSourceFiles->{$file})
    {
	MSerror("$substWorkingDir/makefile","$file is present in $dsp but not makefile\n");
        $errors ++;
    }
}

my $makein = 'makefile.defs';
my $makename = undef;
my $raSourceInFiles = undef;
if (-f $makein)
{
    # Try makefile.def if it exists
    ($makename,$raSourceInFiles) = sourceGetFiles($makein);
}
else 
{
    # Try makefile.in if makefile.def is not found
    $makein = 'makefile.in';
    ($makename,$raSourceInFiles) = sourceGetFiles($makein);
}
showFiles($makein, $raSourceInFiles) if ($flagVerbose);

if ($raSourceInFiles)
{
    if (getFileDate($makein) > getFileDate("makefile"))
    {
        print "WARNING: $makein is newer than makefile must rerun configure\n";
        $errors ++;
    }
    foreach my $file (sort keys %{$raDspSourceFiles})
    {
        if (!$raSourceInFiles->{$file})
        {
            MSerror("$substWorkingDir/$makein","$file is present in $dsp but not $makein\n");
            $errors ++;
        }
    }
#    foreach my $file (sort keys %{$raSourceInFiles})
#    {
#        if (!$raSourceFiles->{$file})
#        {
#            print "WARNING: $file is present in $makein but not makefile: must rerun configure\n";
#            $errors ++;
#        }
#    }
}


if ($flagWarnings)
{
    foreach my $file (sort keys %{$raSourceFiles})
    {
        if (!$raDspSourceFiles->{$file})
        {
            print "WARNING: $file is present in makefile but not $dsp\n";
        }
    }
}

exit $errors;


###############################################################################
sub showFiles
{
    my ($file,$source);

    print "---$file---\n";
    foreach my $file (sort keys %{$source})
    {
        print "$file\n";
    }
}
###############################################################################
sub getExeDir
  {
	# The following gets the current path of the this file 
	my $exeDir   = ($0 =~ m/(.*\/)[^\/]/) ? $1 : undef; # linux dir
	$exeDir    ||= ($0 =~ m/(.*\\)[^\\]/) ? $1 : undef; # dos/win dir
	$exeDir    ||= '.';

	# get full path (convert from ../something to /here/there/something)
	my $oldDir = cwd;
	chdir_safe($exeDir);
	$exeDir = cwd();
	chdir_safe($oldDir);

	return $exeDir;
  }
###############################################################################
sub chdir_safe
  {
	my ($dir) = @_;

	return chdir($dir) if ((defined $dir) and ($dir ne ''));
	return 1;
  }
###############################################################################
sub getFileDate
  {
    my $file = shift;
    return (stat($file))[9];
  }
###############################################################################
sub MSerror
  {
    my $file = shift;
    my $error = shift; 
    my $line = shift || "1";
    my $no = shift || "C1001";

    $file =~ s|\/|\\|g;

    print "$file($line) : error $no: $error\n";
  }