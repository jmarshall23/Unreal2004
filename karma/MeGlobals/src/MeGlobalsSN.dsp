# Microsoft Developer Studio Project File - Name="MeGlobalsSN" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MeGlobalsSN - Win32 PS2 EE Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MeGlobalsSN.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MeGlobalsSN.mak" CFG="MeGlobalsSN - Win32 PS2 EE Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MeGlobalsSN - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MeGlobalsSN - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "MeGlobalsSN - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "MeGlobalsSN - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MeGlobalsSN - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "PS2" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "MeGlobalsSN - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MeGlobalsSN___Win32_Debug"
# PROP BASE Intermediate_Dir "MeGlobalsSN___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MeGlobalsSN___Win32_Debug"
# PROP Intermediate_Dir "MeGlobalsSN___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "PS2" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "MeGlobalsSN - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MeGlobalsSN___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "MeGlobalsSN___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../lib.dbg/ps2"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Od /I "D:\usr\local\sce\ee\include" /I "D:\usr\local\sce\common\include" /I "../../../../include" /I "../../../../3rdParty/ps2gl" /I "../../../../3rdParty" /D "PS2" /D "SN_TARGET_PS2" /D "_ME_CHECK" /D "MCD_CHECK" /D "WITH_PS2" /D "WITH_BENCHMARK" /D "WITH_OPENGL" /Fo"PS2_EE_Debug/" /FD /debug /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.dbg/ps2\MeGlobals.lib" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "MeGlobalsSN - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MeGlobalsSN___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "MeGlobalsSN___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../lib.rel/ps2"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /O2 /I "../../../../include" /I "../../../../3rdParty/ps2gl" /I "../../../../3rdParty" /D "PS2" /D "SN_TARGET_PS2" /D "WITH_PS2" /D "WITH_BENCHMARK" /D "WITH_OPENGL" /D "NDEBUG" /D "_ME_API_SINGLE" /Fo"PS2_EE_Release/" /FD /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.rel/ps2/MeGlobals.lib" /D:SN_TARGET_PS2

!ENDIF 

# Begin Target

# Name "MeGlobalsSN - Win32 Release"
# Name "MeGlobalsSN - Win32 Debug"
# Name "MeGlobalsSN - Win32 PS2 EE Debug"
# Name "MeGlobalsSN - Win32 PS2 EE Release"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
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

SOURCE=.\MeIDPool.c
# End Source File
# Begin Source File

SOURCE=.\MeMath.c
# End Source File
# Begin Source File

SOURCE=.\MeMemory.c
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

SOURCE=.\MeProfile_ps2.c
# End Source File
# Begin Source File

SOURCE=.\MeSet.c
# End Source File
# Begin Source File

SOURCE=.\MeSimpleFile.c
# End Source File
# Begin Source File

SOURCE=.\MeSimpleFile_ps2.c
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
# Begin Source File

SOURCE=.\version.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h;.hpp"
# Begin Source File

SOURCE=..\..\..\..\3rdParty\PS2_in_VC.h
# End Source File
# End Group
# End Target
# End Project