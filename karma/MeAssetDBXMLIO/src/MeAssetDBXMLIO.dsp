# Microsoft Developer Studio Project File - Name="MeAssetDBXMLIO" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MeAssetDBXMLIO - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MeAssetDBXMLIO.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MeAssetDBXMLIO.mak" CFG="MeAssetDBXMLIO - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MeAssetDBXMLIO - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MeAssetDBXMLIO - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "MeAssetDBXMLIO - Win32 Check" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MeAssetDBXMLIO - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "../src" /I "../include" /I "../../MeAssetDB/include" /I "../../MeXML/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.rel/win32/MeAssetDBXMLIO.lib"

!ELSEIF  "$(CFG)" == "MeAssetDBXMLIO - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../src" /I "../include" /I "../../MeAssetDB/include" /I "../../MeXML/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_MECHECK" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/win32/MeAssetDBXMLIO.lib"

!ELSEIF  "$(CFG)" == "MeAssetDBXMLIO - Win32 Check"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MeAssetDBXMLIO___Win32_Check"
# PROP BASE Intermediate_Dir "MeAssetDBXMLIO___Win32_Check"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MeAssetDBXMLIO___Win32_Check"
# PROP Intermediate_Dir "MeAssetDBXMLIO___Win32_Check"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../src" /I "../include" /I "../../MeAssetDB/include" /I "../../MeXML/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MLd /W3 /GX /O2 /I "../src" /I "../include" /I "../../MeAssetDB/include" /I "../../MeXML/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /D "_MBCS" /D "_LIB" /D "_MECHECK" /D "MCDCHECK" /D "NDEBUG" /D "WIN32" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib.rel/win32/MeAssetDBXMLIO.lib"
# ADD LIB32 /nologo /out:"../../lib.chk/win32/MeAssetDBXMLIO.lib"

!ENDIF 

# Begin Target

# Name "MeAssetDBXMLIO - Win32 Release"
# Name "MeAssetDBXMLIO - Win32 Debug"
# Name "MeAssetDBXMLIO - Win32 Check"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MeAssetDBXMLInput_1_0.c
# End Source File
# Begin Source File

SOURCE=.\MeAssetDBXMLIO.c
# End Source File
# Begin Source File

SOURCE=.\MeAssetDBXMLOutput_1_0.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\MeAssetDBXMLIO.h
# End Source File
# Begin Source File

SOURCE=..\include\MeAssetDBXMLIOTypes.h
# End Source File
# End Group
# Begin Group "Internal Header Files"

# PROP Default_Filter ".hpp"
# Begin Source File

SOURCE=.\MeAssetDBXMLIO_1_0.h
# End Source File
# End Group
# End Target
# End Project
