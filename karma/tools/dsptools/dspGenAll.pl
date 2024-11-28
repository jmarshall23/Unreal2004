#!/usr/bin/perl

use strict;
use Cwd;
use Getopt::Long;
use FileHandle;

# This is also a permitted substitution variable
my $substExeDir = getExeDir();
# Add the pl file location to include path 
unshift (@INC, "$substExeDir");

require "dspUtil.pm";
my $substRelativeDir = dspUtil::getRelativeDir($substExeDir);
#
if (!defined $ARGV[0])
{
    print "\nUSAGE: $0 [type]\n\n";
    print "Creates .dsp for each example in dir 'example/src'\n\n";
    print "type is autoconf (default) or MS\n\n";
    exit 1;
}

my $type = $ARGV[0] || 'autoconf';

my $fh = new FileHandle;
opendir($fh,'.');
while (defined (my $file = readdir($fh)))
{
    next if (!-d $file);
    next if ($file eq '.');
    next if ($file eq '..');
    next if ($file eq 'CVS');
    next if ($file eq 'bin');

    next if (!-d "$file\\src");

    my $template = '';
    if ($type eq 'MS')
    {
        if ($file =~ m/CD$/)
        {
            $template = 'ExampleCD.dsp';
        }
        else
        {
            $template = 'Example.dsp';
        }
    }
    else
    {
        $template = 'autoconfExe.dsp';
    }

    my $cmd = 
    "cd $file\\src && ".
    "perl ../../$substRelativeDir/dspGen.pl $template $file\n";

#    print "$cmd\n";
    system ($cmd);
}

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
