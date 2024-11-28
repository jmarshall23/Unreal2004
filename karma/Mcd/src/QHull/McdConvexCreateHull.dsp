# Microsoft Developer Studio Project File - Name="McdConvexCreateHull" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=McdConvexCreateHull - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "McdConvexCreateHull.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "McdConvexCreateHull.mak" CFG="McdConvexCreateHull - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McdConvexCreateHull - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "McdConvexCreateHull - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "McdConvexCreateHull - Win32 Check" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "McdConvexCreateHull - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /Oy- /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /Oy- /I "../cx" /I "../../../MeGlobals/include" /I "../../include" /I "../../../Mcd/src/frame" /I "." /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "WIN32" /D "_ME_API_SINGLE" /D "_ME_IMP_SINGLE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.rel/win32/McdConvexCreateHull.lib"

!ELSEIF  "$(CFG)" == "McdConvexCreateHull - Win32 Debug"

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
# ADD CPP /nologo /G6 /W3 /Gm /GX /ZI /Od /I "../cx" /I "../../../MeGlobals/include" /I "../../../Mcd/src/frame" /I "../../include" /I "." /D "_DEBUG" /D "_MEDEVELOP" /D "_MBCS" /D "_LIB" /D "WIN32" /D "_ME_API_SINGLE" /D "_ME_IMP_SINGLE" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.dbg/win32/McdConvexCreateHull.lib"

!ELSEIF  "$(CFG)" == "McdConvexCreateHull - Win32 Check"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "McdConvexCreateHull___Win32_Check"
# PROP BASE Intermediate_Dir "McdConvexCreateHull___Win32_Check"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "McdConvexCreateHull___Win32_Check"
# PROP Intermediate_Dir "McdConvexCreateHull___Win32_Check"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /Oy- /I "../cx" /I "../../../MeGlobals/include" /I "../../include" /I "../../../Mcd/src/frame" /I "." /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "WIN32" /D "_ME_API_SINGLE" /D "_ME_IMP_SINGLE" /YX /FD /c
# ADD CPP /nologo /G6 /MLd /W3 /GX /O2 /Oy- /I "../cx" /I "../../../MeGlobals/include" /I "../../include" /I "../../../Mcd/src/frame" /I "." /D "_MBCS" /D "_LIB" /D "_ME_API_SINGLE" /D "_ME_IMP_SINGLE" /D "_MECHECK" /D "MCDCHECK" /D "NDEBUG" /D "WIN32" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib.rel/win32/McdConvexCreateHull.lib"
# ADD LIB32 /nologo /out:"../../../lib.chk/win32/McdConvexCreateHull.lib"

!ENDIF 

# Begin Target

# Name "McdConvexCreateHull - Win32 Release"
# Name "McdConvexCreateHull - Win32 Debug"
# Name "McdConvexCreateHull - Win32 Check"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "QHull"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\geom.c
# End Source File
# Begin Source File

SOURCE=.\geom2.c
# End Source File
# Begin Source File

SOURCE=.\global.c
# End Source File
# Begin Source File

SOURCE=.\io.c
# End Source File
# Begin Source File

SOURCE=..\convex\McdConvexHull.cpp
# End Source File
# Begin Source File

SOURCE=.\mem.c
# End Source File
# Begin Source File

SOURCE=.\merge.c
# End Source File
# Begin Source File

SOURCE=.\poly.c
# End Source File
# Begin Source File

SOURCE=.\poly2.c
# End Source File
# Begin Source File

SOURCE=.\qhull.c
# End Source File
# Begin Source File

SOURCE=.\qset.c
# End Source File
# Begin Source File

SOURCE=.\stat.c
# End Source File
# Begin Source File

SOURCE=.\user.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\geom.h
# End Source File
# Begin Source File

SOURCE=.\mem.h
# End Source File
# Begin Source File

SOURCE=.\merge.h
# End Source File
# Begin Source File

SOURCE=.\poly.h
# End Source File
# Begin Source File

SOURCE=.\qhull.h
# End Source File
# Begin Source File

SOURCE=.\qhull_a.h
# End Source File
# Begin Source File

SOURCE=.\qhullio.h
# End Source File
# Begin Source File

SOURCE=.\qset.h
# End Source File
# Begin Source File

SOURCE=.\stat.h
# End Source File
# Begin Source File

SOURCE=.\user.h
# End Source File
# End Group
# End Target
# End Project
