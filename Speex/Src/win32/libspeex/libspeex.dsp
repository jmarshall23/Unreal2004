# Microsoft Developer Studio Project File - Name="libspeex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libspeex - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libspeex.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libspeex.mak" CFG="libspeex - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libspeex - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libspeex - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath "..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libspeex - Win32 Release"

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
F90=df.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /GX /Ox /Ot /Og /Oi /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "EPIC_48K" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libspeex - Win32 Debug"

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
F90=df.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libspeex - Win32 Release"
# Name "libspeex - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\libspeex\bits.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\cb_search.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\denoise.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\exc_10_16_table.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\exc_10_32_table.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\exc_20_32_table.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\exc_5_256_table.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\exc_5_64_table.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\exc_8_128_table.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\filters.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\gain_table.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\gain_table_lbr.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\hexc_10_32_table.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\hexc_table.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\high_lsp_tables.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\lbr_48k_tables.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\lpc.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\lsp.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\lsp_tables_nb.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\ltp.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\math_approx.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\misc.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\modes.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\nb_celp.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\quant_lsp.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\sb_celp.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\smallft.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\speex_callbacks.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\speex_header.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\stereo.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\vbr.c
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\vq.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\libspeex\cb_search.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\filters.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\lpc.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\lsp.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\ltp.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\modes.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\nb_celp.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\quant_lsp.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\sb_celp.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\smallft.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\speex.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\speex_bits.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\speex_callbacks.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\speex_denoise.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\speex_header.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\speex_stereo.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\stack_alloc.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\vbr.h
# End Source File
# Begin Source File

SOURCE=..\..\libspeex\vq.h
# End Source File
# End Group
# End Target
# End Project
