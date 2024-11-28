# Microsoft Developer Studio Project File - Name="MeGlobals" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MeGlobals - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MeGlobals.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MeGlobals.mak" CFG="MeGlobals - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MeGlobals - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MeGlobals - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "MeGlobals - Win32 Check" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MeGlobals - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /Oy- /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /O2 /Oy- /I "../include" /I "../src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /U "DEBUG" /U "MeGlobalsTRACE" /U "MeGlobalsSTATS" /FR /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.rel/win32/MeGlobals.lib"

!ELSEIF  "$(CFG)" == "MeGlobals - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /W3 /Gm /Zi /Od /I "../include" /I "../src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "DEBUG" /D "MeGlobalsTRACE" /D "MeGlobalsSTATS" /D "_MECHECK" /D "MESTREAM_MEMBUFFER" /U "NDEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/win32/MeGlobals.lib"

!ELSEIF  "$(CFG)" == "MeGlobals - Win32 Check"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MeGlobals___Win32_Check"
# PROP BASE Intermediate_Dir "MeGlobals___Win32_Check"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MeGlobals___Win32_Check"
# PROP Intermediate_Dir "MeGlobals___Win32_Check"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /O2 /Oy- /I "../include" /I "../src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /U "DEBUG" /U "MeGlobalsTRACE" /U "MeGlobalsSTATS" /FR /YX /FD /c
# ADD CPP /nologo /G6 /MLd /W3 /O2 /Oy- /I "../include" /I "../src" /D "_WINDOWS" /D "_MECHECK" /D "MCDCHECK" /D "NDEBUG" /D "WIN32" /U "DEBUG" /U "MeGlobalsTRACE" /U "MeGlobalsSTATS" /FR /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib.rel/win32/MeGlobals.lib"
# ADD LIB32 /nologo /out:"../../lib.chk/win32/MeGlobals.lib"

!ENDIF 

# Begin Target

# Name "MeGlobals - Win32 Release"
# Name "MeGlobals - Win32 Debug"
# Name "MeGlobals - Win32 Check"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "win32"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=.\MeProfile_win32.c
# End Source File
# Begin Source File

SOURCE=.\MeSimpleFile_win32.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\MeASELoad.c
# End Source File
# Begin Source File

SOURCE=.\MeBounding.c
# End Source File
# Begin Source File

SOURCE=.\MeChunk.c
# End Source File
# Begin Source File

SOURCE=.\MeCommandLine.c
# End Source File
# Begin Source File

SOURCE=.\MeDebugDraw.c
# End Source File
# Begin Source File

SOURCE=.\MeDict.c
# End Source File
# Begin Source File

SOURCE=.\MeFileSearch.c
# End Source File
# Begin Source File

SOURCE=.\MeHash.c
# End Source File
# Begin Source File

SOURCE=.\MeHeap.c
# End Source File
# Begin Source File

SOURCE=.\MeIdPool.c
# End Source File
# Begin Source File

SOURCE=.\MeMath.c
# End Source File
# Begin Source File

SOURCE=.\MeMemory.c
# End Source File
# Begin Source File

SOURCE=.\MeMemoryCpp.cpp
# End Source File
# Begin Source File

SOURCE=.\MeMessage.c
# End Source File
# Begin Source File

SOURCE=.\MeMisc.c
# End Source File
# Begin Source File

SOURCE=.\MePool.c
# End Source File
# Begin Source File

SOURCE=.\MePoolx.c
# End Source File
# Begin Source File

SOURCE=.\MePrecision.c
# End Source File
# Begin Source File

SOURCE=.\MeProfile.c
# End Source File
# Begin Source File

SOURCE=.\MeSet.c
# End Source File
# Begin Source File

SOURCE=.\MeSimpleFile.c
# End Source File
# Begin Source File

SOURCE=.\MeStream.c
# End Source File
# Begin Source File

SOURCE=.\MeString.c
# End Source File
# Begin Source File

SOURCE=.\MeVersion.c
# End Source File
# End Group
# Begin Group "public headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\MeASELoad.h
# End Source File
# Begin Source File

SOURCE=..\include\MeAssert.h
# End Source File
# Begin Source File

SOURCE=..\include\MeBounding.h
# End Source File
# Begin Source File

SOURCE=..\include\MeCall.h
# End Source File
# Begin Source File

SOURCE=..\include\MeChunk.h
# End Source File
# Begin Source File

SOURCE=..\include\MeCommandLine.h
# End Source File
# Begin Source File

SOURCE=..\include\MeDebugDraw.h
# End Source File
# Begin Source File

SOURCE=..\include\MeDict.h
# End Source File
# Begin Source File

SOURCE=..\include\MeHash.h
# End Source File
# Begin Source File

SOURCE=..\include\MeHeap.h
# End Source File
# Begin Source File

SOURCE=..\include\MeIdPool.h
# End Source File
# Begin Source File

SOURCE=..\include\MeInline.h
# End Source File
# Begin Source File

SOURCE=..\include\MeMath.h
# End Source File
# Begin Source File

SOURCE=..\include\MeMemory.h
# End Source File
# Begin Source File

SOURCE=..\include\MeMessage.h
# End Source File
# Begin Source File

SOURCE=..\include\MeMisc.h
# End Source File
# Begin Source File

SOURCE=..\include\MePool.h
# End Source File
# Begin Source File

SOURCE=..\include\MePrecision.h
# End Source File
# Begin Source File

SOURCE=..\include\MeProfile.h
# End Source File
# Begin Source File

SOURCE=..\include\MeSimpleFile.h
# End Source File
# Begin Source File

SOURCE=..\include\MeStream.h
# End Source File
# Begin Source File

SOURCE=..\include\MeString.h
# End Source File
# Begin Source File

SOURCE=..\include\MeVersion.h
# End Source File
# End Group
# End Target
# End Project
