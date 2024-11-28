require "dspHeader.pl";

use strict;

sub dspGetFiles
{
    my $dsp = shift;
    
    my $fhIn = new FileHandle;
    open($fhIn,$dsp)
    or die "Can\'t open $dsp for reading\n";
    
    my $start = '# Begin Group "Source Files"';
    my $end   = '# End Group';

    my %files = ();
    my $source = 0;
    while (<$fhIn>)
    {
        my $line = $_;

        if ($line =~ m/^$start/)
        {
            $source=1;
        }
        elsif ($line =~ m/^$end/)
        {
            $source=0;
        }

        next if (!$source);

        if ($line =~ m/SOURCE[\s]*=(.*)$/)
        {
            $line = $1;
            if ($line !~ m/^[\s]*$/)
            {
                my $file = $line;
                $file =~ s/\s//g;
                $file =~ s/^.[\\\/]//;
                $files{$file}++;
            }
        }
    }
    
    return \%files;
}
