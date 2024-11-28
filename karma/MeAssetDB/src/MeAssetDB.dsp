# Microsoft Developer Studio Project File - Name="MeAssetDB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MeAssetDB - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MeAssetDB.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MeAssetDB.mak" CFG="MeAssetDB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MeAssetDB - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MeAssetDB - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "MeAssetDB - Win32 Check" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MeAssetDB - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "../src" /I "../include" /I "../../MeXML/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.rel/win32/MeAssetDB.lib"

!ELSEIF  "$(CFG)" == "MeAssetDB - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../src" /I "../include" /I "../../MeXML/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_MECHECK" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/win32/MeAssetDB.lib"

!ELSEIF  "$(CFG)" == "MeAssetDB - Win32 Check"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MeAssetDB___Win32_Check"
# PROP BASE Intermediate_Dir "MeAssetDB___Win32_Check"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MeAssetDB___Win32_Check"
# PROP Intermediate_Dir "MeAssetDB___Win32_Check"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../src" /I "../include" /I "../../MeXML/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MLd /W3 /GX /O2 /I "../src" /I "../include" /I "../../MeXML/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /D "_MBCS" /D "_LIB" /D "_MECHECK" /D "MCDCHECK" /D "NDEBUG" /D "WIN32" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib.rel/win32/MeAssetDB.lib"
# ADD LIB32 /nologo /out:"../../lib.chk/win32/MeAssetDB.lib"

!ENDIF 

# Begin Target

# Name "MeAssetDB - Win32 Release"
# Name "MeAssetDB - Win32 Debug"
# Name "MeAssetDB - Win32 Check"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MeAssetDB.c
# End Source File
# Begin Source File

SOURCE=.\MeFAsset.c
# End Source File
# Begin Source File

SOURCE=.\MeFAssetPart.c
# End Source File
# Begin Source File

SOURCE=.\MeFGeometry.c
# End Source File
# Begin Source File

SOURCE=.\MeFGeometryFromMesh.c
# End Source File
# Begin Source File

SOURCE=.\MeFJoint.c
# End Source File
# Begin Source File

SOURCE=.\MeFModel.c
# End Source File
# Begin Source File

SOURCE=.\MeFPrimitive.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\MeAssetDB.h
# End Source File
# Begin Source File

SOURCE=..\include\MeAssetDBTypes.h
# End Source File
# End Group
# Begin Group "Internal Header files"

# PROP Default_Filter ".hpp"
# Begin Source File

SOURCE=.\MeAssetDBInternal.h
# End Source File
# End Group
# End Target
# End Project
