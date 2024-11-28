# Microsoft Developer Studio Project File - Name="Mst" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Mst - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Mst.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mst.mak" CFG="Mst - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Mst - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Mst - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Mst - Win32 Check" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Mst - Win32 Release"

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
# ADD CPP /nologo /G6 /W3 /GX /Zi /O2 /I "../include" /I "../src" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /FAs /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.rel/win32/Mst.lib"

!ELSEIF  "$(CFG)" == "Mst - Win32 Debug"

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
# ADD CPP /nologo /G6 /W3 /Gm /GX /ZI /Od /I "../include" /I "../src" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "_MECHECK" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/win32/Mst.lib"

!ELSEIF  "$(CFG)" == "Mst - Win32 Check"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Mst___Win32_Check"
# PROP BASE Intermediate_Dir "Mst___Win32_Check"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Mst___Win32_Check"
# PROP Intermediate_Dir "Mst___Win32_Check"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /I "../include" /I "../src" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /FAs /FR /YX /FD /c
# ADD CPP /nologo /G6 /MLd /W3 /GX /Zi /O2 /I "../include" /I "../src" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /D "_MBCS" /D "_LIB" /D "VS6" /D "_MECHECK" /D "MCDCHECK" /D "NDEBUG" /D "WIN32" /FAs /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib.rel/win32/Mst.lib"
# ADD LIB32 /nologo /out:"../../lib.chk/win32/Mst.lib"

!ENDIF 

# Begin Target

# Name "Mst - Win32 Release"
# Name "Mst - Win32 Debug"
# Name "Mst - Win32 Check"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MstBridge.c
# End Source File
# Begin Source File

SOURCE=.\MstModelDynamics.c
# End Source File
# Begin Source File

SOURCE=.\MstUniverse.c
# End Source File
# Begin Source File

SOURCE=.\MstUtils.c
# End Source File
# End Group
# Begin Group "public headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\Mst.h
# End Source File
# Begin Source File

SOURCE=..\include\MstBridge.h
# End Source File
# Begin Source File

SOURCE=..\include\MstModelDynamics.h
# End Source File
# Begin Source File

SOURCE=..\include\MstTypes.h
# End Source File
# Begin Source File

SOURCE=..\include\MstUniverse.h
# End Source File
# Begin Source File

SOURCE=..\include\MstUtils.h
# End Source File
# End Group
# End Target
# End Project
