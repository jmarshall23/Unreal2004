use strict;
use FileHandle;

require "dspHeader.pl";

sub dspFromTemplate
{
    my $name = shift;
    my $template = shift;
    my $templateDir = shift;
    my $relDir = shift;
    my $raSourceFiles = shift;
    my $raHeaderFiles = shift;

    my $fh = new FileHandle;
    my $fhOut = new FileHandle;

    open($fh,"$templateDir/$template.dsp")
    or die "can't open $template.dsp for reading\n";
    open($fhOut,"> $name.dsp")
    or die "can't open $name.dsp for writting\n";

    print "Generating $name.dsp from $template.dsp\n";

    my $source_start = '# Begin Group "Source Files"';
    my $source_end = '# End Group';
    my $source_section = 0;
    my $source_found = 0;
    my $target_end = '# End Target';
    while (<$fh>)
    {
        my $line = $_;
        $line =~ s/$template/$name/g;	

        if ($source_section)
        {
            if ($line =~ m/$source_end/)
            {
                $source_section = 0;
                if (!$source_found)
                {
                    writeSourceFiles($fhOut,$raSourceFiles);
                    $source_found = 1;
                }	
            }
        }
        else
        {
            if ($line =~ m/$source_start/)
            {
                $source_section = 1;
            }
            elsif (!$source_found && $line =~ m/$target_end/)
            {
                writeSourceFiles($fhOut,$raSourceFiles);
                $source_found = 1;
            }
        }
        print $fhOut $line;
    }
    
    close ($fh);
    close ($fhOut);
}

sub writeSourceFiles
{
    my $fhOut = shift;
    my $raSourceFiles = shift;
    if (defined $raSourceFiles)
    {
        foreach my $file (sort keys %{$raSourceFiles})
        {
            print $fhOut "".setDspSource($file);
        }
    }
}
sub writeHeaderFiles
{
    my $fhOut = shift;
    my $raHeaderFiles = shift;
    if (defined $raHeaderFiles)
    {
        foreach my $file (sort keys %{$raHeaderFiles})
        {
            print $fhOut "".setDspHeaderFile($file);
        }
    }
}

sub dspwrite
{
    my $dsp = shift;
    my $type = shift;
    my $relDir = shift;
    my $raSourceFiles = shift;
    my $raHeaderFiles = shift;

    my $fhOut = new FileHandle;
    open($fhOut,"> $dsp.dsp")
    or die "Can\'t open $dsp for writing\n";

    print $fhOut "".setDspHeader($dsp,$type,$relDir);
    print $fhOut "".setDspSourceStart();
    writeSourceFiles($fhOut,$raSourceFiles);
    print $fhOut "".setDspSourceEnd();
    print $fhOut "".setDspHeaderFileStart();
    writeHeaderFiles($fhOut,$raHeaderFiles);
    print $fhOut "".setDspHeaderFileEnd();
    print $fhOut "".setDspFooter();

    close($fhOut);
}

sub sourceGetFilesFromMakefile
{
    my $verbose = shift;

    (-f "makefile")
    or die "No makefile found (you make need to run .\configure)\n";
    
    my $fhIn = new FileHandle;
    print "Running make -p\n" if (defined $verbose);
    return undef if (!open($fhIn,"make -p |"));
    
    my $name = undef;
    my %files = ();
    while (<$fhIn>)
    {
        my $line = $_;

        # ignore comments
        next if ($line =~ m/^[\s]*\#/);
    
        if ($line =~ m/OUTNAME[\s]*:=[\s]*(.*)$/)
        {
            $name = $1;
        }

        if ($line =~ m/SOURCES[\s]*:=[\s]*(.*)$/)
        {
            # Remove SOURCES:=
            $line = $1;

            my @files = split(/[\s]+/,$line);
            foreach my $file (@files)
            {
#                print "== $file\n";
                # Expand OUTNAME
                $file =~ s/\$OUTNAME/$name/;
                $file =~ s/\$\(OUTNAME\)/$name/;
                #
                $file =~ s/\s//g;
                $files{$file}++;
            }
        }
    }
    
    return ($name,\%files);
}

sub sourceGetFiles
{
    my $source = shift;
    
    my $fhIn = new FileHandle;
    return undef if (!open($fhIn,$source));
    
    my $name = undef;
    my %files = ();
    my $source = 0;
    while (<$fhIn>)
    {
        my $line = $_;

        # ignore comments
        next if ($line =~ m/^[\s]*\#/);
    
        if ($line =~ m/OUTNAME[\s]*[:+]=[\s]*(.*)$/)
        {
            $name = $1;
        }

        if ($line =~ m/SOURCES[\s]*[:+]=[\s]*(.*)$/)
        {
            # start of source files
            $source = 1;
            
            # Remove SOURCES:= or SOURCES+=
            $line = $1;
        }
        
        if ($source)
        {
            if ($line =~ m/^(.*)\\[\s]*$/)
            {
                $line = $1;
            }
            else
            { 
                # Last source file
                $source = 0;
            }

            if ($line !~ m/^[\s]*$/)
            {
                my $file = $line;
                # Expand OUTNAME
                $file =~ s/\$OUTNAME/$name/;
                $file =~ s/\$\(OUTNAME\)/$name/;
                #
                $file =~ s/\s//g;
                $files{$file}++;
            }
        }
    }
    
    return ($name,\%files);
}
