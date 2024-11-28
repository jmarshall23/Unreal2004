#!/usr/bin/perl

use strict;
use Cwd;
use Getopt::Long;

package dspUtil;

###############################################################################
sub getRelativeDir
  {
	my ($dir) = @_;
	my $cwdDir = main::cwd();
	my $retDir = '';

	# Add slash to make pattern matching simpler
	$dir .= '/';
	$cwdDir .= '/';
	
	# Find length that matches
	my $len = length($cwdDir);
	$len = length($dir) if (length($dir) < $len);
	my $start = 0;
	for (my $i = 0; $i < $len; $i++)
	  {
		$start = $i if (substr($cwdDir,$i,1) eq '/');
		if (substr($cwdDir,$i,1) ne substr($dir,$i,1))
		  {
			last;
		  }
	  }
	
	# Ignore matching part
	$retDir = substr($cwdDir,$start);
	# Replace each directory level with ..
	$retDir =~ s|(?<=/)([^/]*)(?=/)|\.\.|g;
	# Strip leading and trailing slashes
	$retDir = substr($retDir,1,-1);
	# Strip relative slash if subdirectory of current directory
	$dir =~ s|^/|| if (!$retDir);
	# Append input location (ignoring trailing slash)
	$retDir .= substr($dir,$start,-1);

	return $retDir;
  }
###############################################################################
sub getFileDate
  {
    my $file = shift;
    return (stat($file))[9];
  }

1; # so require passes
