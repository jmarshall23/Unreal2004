#!/usr/bin/perl -w
#
# A Perl script to convert the Unreal source tree to Unix endlines.
#   Written by Ryan C. Gordon (icculus@clutteredmind.org)
#


use warnings;
use strict;

use File::Basename;

my $recurse = 0;
my $regexp = undef;
my $justmatch = 0;

sub convert_file {
    my $origfile = shift;
    my $newfile = $origfile . ".___d2u___";

    if (-d $origfile) {
        return if not $recurse;
        opendir(DIRH, $origfile) || die("Couldn't open directory [$origfile]: $!");
        my @dirfiles = readdir(DIRH);
        closedir(DIRH);
        foreach (@dirfiles) {
            next if (($_ eq ".") || ($_ eq ".."));
            convert_file($origfile . '/' . $_);
        }
        return;
    }

    return if ((defined $regexp) and (basename($origfile) !~ /$regexp/));
    print("$origfile matches.\n"), return if ($justmatch);

    print("Converting $origfile ...\n");

    open(INHANDLE, "$origfile") || die("can't open $origfile: $!");
    open(OUTHANDLE, ">$newfile") || die("can't open temp file: $!");
    while (<INHANDLE>) {
        s/\r//g;
        print(OUTHANDLE "$_");
    }

    close(INHANDLE);
    close(OUTHANDLE);
    rename("$newfile", "$origfile") || die("can't overwrite $origfile: $!");
}

#foreach (@ARGV) {
#    if ($_ eq '--recurse') {
#        $recurse = 1;
#        next;
#    }
#
#    if ($_ eq '--no-recurse') {
#        $recurse = 0;
#        next;
#    }
#
#    if ($_ eq '--match-and-exit') {
#        $justmatch = 1;
#        next;
#    }
#
#    if ($_ eq '--no-match-and-exit') {
#        $justmatch = 0;
#        next;
#    }
#
#    if (s/^--regexp=(.*)/$1/) {
#        $regexp = $_;
#        #print("Regexp is [$regexp].\n");
#        next;
#    }
#
#    convert_file($_);
#}

$recurse = 1;
$justmatch = 0;

$regexp = '.*?\.[tT][xX][tT]\Z';
convert_file('.');

$regexp = '.*?\.[cC]\Z';
convert_file('.');

$regexp = '.*?\.[cC][pP][pP]\Z';
convert_file('.');

$regexp = '.*?\.[hH]\Z';
convert_file('.');

$regexp = '.*?\.[hH][pP][pP]\Z';
convert_file('.');

$regexp = '.*[Mm][aA][kK][eE].*\Z';
convert_file('.');

$regexp = '.*?\.[uU][cC]\Z';
convert_file('.');

convert_file('./CHANGELOG-LINUX');

# end of d2u.pl ...

