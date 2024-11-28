use strict;

sub setDspHeader
{
    my $product = shift;
    my $type    = shift || 'lib';
    my $relDir  = shift;

    print "Creating dsp for $product ($type)\n";

    return setDspHeaderAutoconf($product,$relDir)    if ($type eq 'lib');
    return setDspHeaderAutoconfExe($product,$relDir) if ($type eq 'exe');
    return setDspHeaderMSLib($product) if ($type eq 'ms_lib');
    return setDspHeaderMSExe($product) if ($type eq 'ms_exe');

    die 'invalid dsp type: $type';
}

sub setDspHeaderAutoconf
{
    my $product = shift;
    my $relDir  = shift;

    $relDir =~ s/\//\\/g;

my $dspHeaderAutoconf =<<END_DSP_HEADER_AUTOCONF;
# Microsoft Developer Studio Project File - Name="$product" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=$product - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "$product.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "$product.mak" CFG="$product - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "$product - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "$product - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "\$(CFG)" == "$product - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f $product.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "$product.exe"
# PROP BASE Bsc_Name "$product.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "$relDir\\autoconf release"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "..\\lib\\$product.lib"
# PROP Bsc_Name "release\\${product}.dsp"
# PROP Target_Dir ""

!ELSEIF  "\$(CFG)" == "$product - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f $product.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "$product.exe"
# PROP BASE Bsc_Name "$product.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "$relDir\\autoconf debug"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "..\\lib\\${product}_debug.lib"
# PROP Bsc_Name "debug\\${product}.dsp"
# PROP Target_Dir ""

!ENDIF

# Begin Target

# Name "$product - Win32 Release"
# Name "$product - Win32 Debug"

!IF  "\$(CFG)" == "$product - Win32 Release"

!ELSEIF  "\$(CFG)" == "$product - Win32 Debug"

!ENDIF
END_DSP_HEADER_AUTOCONF

return $dspHeaderAutoconf;
}


sub setDspHeaderAutoconfExe
{
    my $product = shift;
    my $relDir  = shift;

    $relDir =~ s/\//\\/g;

my $dspHeaderAutoconfExe =<<END_DSP_HEADER_AUTOCONF_EXE;
# Microsoft Developer Studio Project File - Name="$product" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=$product - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "$product.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "$product.mak" CFG="$product - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "$product - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "$product - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "\$(CFG)" == "$product - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f $product.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "$product.exe"
# PROP BASE Bsc_Name "$product.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "$relDir\\autoconf_exe release"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "..\\lib\\$product.exe"
# PROP Bsc_Name "release\\${product}.dsp"
# PROP Target_Dir ""

!ELSEIF  "\$(CFG)" == "$product - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f $product.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "$product.exe"
# PROP BASE Bsc_Name "$product.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "$relDir\\autoconf_exe debug"
# PROP Rebuild_Opt "rebuild"
# PROP Target_File "..\\lib\\${product}_debug.exe"
# PROP Bsc_Name "debug\\${product}.dsp"
# PROP Target_Dir ""

!ENDIF

# Begin Target

# Name "$product - Win32 Release"
# Name "$product - Win32 Debug"

!IF  "\$(CFG)" == "$product - Win32 Release"

!ELSEIF  "\$(CFG)" == "$product - Win32 Debug"

!ENDIF
END_DSP_HEADER_AUTOCONF_EXE

return $dspHeaderAutoconfExe;
}


sub setDspHeaderMSLib
{
my $product = shift;

my $includes = '/I "../include" /I "../../MeGlobals/include" ';

my $dspHeaderMSLib =<<END_DSP_HEADER_MS_LIB;
# Microsoft Developer Studio Project File - Name="$product" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=$product - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "$product.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "$product.mak" CFG="$product - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "$product - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "$product - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "\$(CFG)" == "$product - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "\$(CFG)" == "$product - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od $includes /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF

# Begin Target

# Name "$product - Win32 Release"
# Name "$product - Win32 Debug"
END_DSP_HEADER_MS_LIB

return $dspHeaderMSLib;
}


sub setDspHeaderMSExe
{
my $product = shift;

my @includes =
(
'../../../MeMemory/include',
'../../../MdtBcl/include',
'../../../MeViewer/include',
'../../../MdtKea/include',
'../../../MeGlobals/include',
'../../../Mdt/include',
'../../../Mcd/include',
'../../../tools/glut',
'../../tools/glut',
'../../../include',
'../../include',
);

my @libs =
(
'McdFrame',
'McdPrimitives',
'McdDtBridge',
'MdtBcl',
'MeViewer',
'Mdt',
'MdtKea',
'MeMemory',
);
my @libpath =
(
'../../../MeMemory/lib',
'../../../MdtBcl/lib',
'../../../MeViewer/lib',
'../../../MdtKea/lib',
'../../../MdtGlobals/lib',
'../../../Mdt/lib',
'../../../Mcd/lib',
'../../../tools/glut',
'../../tools/glut',
'../../../lib',
'../../lib',
);

my $includes = '';
my $includes_debug = '';
foreach my $inc (@includes)
{
    $includes .= "/I \"$inc\" ";
    $includes_debug .= "/I \"$inc\" ";
}

my $libs = '';
my $libs_debug = '';
foreach my $lib (@libs)
{
    $libs .= "$lib.lib ";
    $libs_debug .= "${lib}_debug.lib ";
}

my $libpath = '';
my $libpath_debug = '';
foreach my $libp (@libpath)
{
    $libpath       .= "/libpath:\"$libp\" ";
    $libpath_debug .= "/libpath:\"$libp\" ";
}

my $dspHeaderMSExe =<<END_DSP_HEADER_MS_EXE;
# Microsoft Developer Studio Project File - Name="$product" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=$product - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "$product.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "$product.mak" CFG="$product - Win32 Release"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "$product - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "$product - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "\$(CFG)" == "$product - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 $includes /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 $libs kernel32.lib user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386 /out:"../bin/$product.exe" $libpath

!ELSEIF  "\$(CFG)" == "$product - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od $includes_debug /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 $libs_debug kernel32.lib user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../bin/${product}_debug.exe" /pdbtype:sept $libpath_debug

!ENDIF

# Begin Target

# Name "$product - Win32 Release"
# Name "$product - Win32 Debug"
END_DSP_HEADER_MS_EXE

return $dspHeaderMSExe;
}


sub setDspSourceStart
{
my $dspSourceStart =<<END_DSP_SRC_START;
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
END_DSP_SRC_START
return $dspSourceStart;
}


sub setDspSource
{
my $source = shift;

my $dspSource =<<END_DSP_SRC;
# Begin Source File

SOURCE=$source
# End Source File
END_DSP_SRC

return $dspSource;
}


sub setDspSourceEnd
{
my $dspSourceEnd =<<END_DSP_SRC_END;
# End Group
END_DSP_SRC_END

return $dspSourceEnd;
}



sub setDspHeaderFileStart
{
my $dspHeaderFileStart =<<END_DSP_SRC_START;
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
END_DSP_SRC_START

return $dspHeaderFileStart;
}



sub setDspHeaderFile
{
my $header = shift;

# ..\include\$header
my $dspHeaderFile =<<END_DSP_SRC;
# Begin Source File

SOURCE=$header
# End Source File
END_DSP_SRC

return $dspHeaderFile;
}


sub setDspHeaderFileEnd
{
my $dspHeaderFileEnd =<<END_DSP_SRC_END;
# End Group
END_DSP_SRC_END

return $dspHeaderFileEnd;
}



sub setDspFooter
{
my $dspFooter =<<END_DSP_FOOTER;
# Begin Group "Resource Files"
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
END_DSP_FOOTER

return $dspFooter;
}

1; #return true for require
