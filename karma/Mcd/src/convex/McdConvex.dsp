# Microsoft Developer Studio Project File - Name="McdConvex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=McdConvex - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "McdConvex.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "McdConvex.mak" CFG="McdConvex - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McdConvex - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "McdConvex - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "McdConvex - Win32 Check" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "McdConvex - Win32 Release"

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
# ADD CPP /nologo /G6 /W3 /GX /O2 /Oy- /I "./" /I "../primitives" /I "../frame" /I "../terrain" /I "../qhull" /I "../math" /I "../space" /I "../util" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../../src/gjk" /I "../cx" /I "../../../MeGlobals/include" /D "NDEBUG" /D "_MERELEASE" /D "LCE_USE_STATIC_LIB" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_ME_API_SINGLE" /D "_ME_IMP_SINGLE" /D "MCDCHECK" /D "_MECHECK" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.rel/win32/McdConvex.lib"

!ELSEIF  "$(CFG)" == "McdConvex - Win32 Debug"

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
# ADD CPP /nologo /G6 /W3 /Gm /GX /ZI /Od /I "./" /I "../primitives" /I "../frame" /I "../terrain" /I "../qhull" /I "../math" /I "../space" /I "../util" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../../src/gjk" /I "../cx" /I "../../../MeGlobals/include" /D "_DEBUG" /D "_MECHECK" /D "LCE_USE_STATIC_LIB" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_ME_API_SINGLE" /D "_ME_IMP_SINGLE" /D "_MEDEVELOP" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.dbg/win32/McdConvex.lib"

!ELSEIF  "$(CFG)" == "McdConvex - Win32 Check"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "McdConvex___Win32_Check"
# PROP BASE Intermediate_Dir "McdConvex___Win32_Check"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "McdConvex___Win32_Check"
# PROP Intermediate_Dir "McdConvex___Win32_Check"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /Oy- /I "./" /I "../primitives" /I "../frame" /I "../terrain" /I "../qhull" /I "../math" /I "../space" /I "../util" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../../src/gjk" /I "../cx" /I "../../../MeGlobals/include" /D "NDEBUG" /D "_MERELEASE" /D "LCE_USE_STATIC_LIB" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_ME_API_SINGLE" /D "_ME_IMP_SINGLE" /D "MCDCHECK" /D "_MECHECK" /YX /FD /c
# ADD CPP /nologo /G6 /MLd /W3 /GX /O2 /Oy- /I "./" /I "../primitives" /I "../frame" /I "../terrain" /I "../qhull" /I "../math" /I "../space" /I "../util" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../../src/gjk" /I "../cx" /I "../../../MeGlobals/include" /D "_MERELEASE" /D "LCE_USE_STATIC_LIB" /D "_MBCS" /D "_LIB" /D "_ME_API_SINGLE" /D "_ME_IMP_SINGLE" /D "MCDCHECK" /D "_MECHECK" /D "NDEBUG" /D "WIN32" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib.rel/win32/McdConvex.lib"
# ADD LIB32 /nologo /out:"../../../lib.chk/win32/McdConvex.lib"

!ENDIF 

# Begin Target

# Name "McdConvex - Win32 Release"
# Name "McdConvex - Win32 Debug"
# Name "McdConvex - Win32 Check"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "convex"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=..\gjk\McdGjk.cpp
# End Source File
# Begin Source File

SOURCE=..\gjk\McdGjkMaximumPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\gjk\McdGjkPenetrationDepth.cpp
# End Source File
# Begin Source File

SOURCE=..\gjk\McdGjkRegistration.cpp
# End Source File
# Begin Source File

SOURCE=..\gjk\McdPlaneIntersect.cpp
# End Source File
# Begin Source File

SOURCE=..\gjk\McdPolygonIntersection.cpp
# End Source File
# End Group
# Begin Group "convexmesh"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\ConvexGeomUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\IxConvexLineSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\IxConvexPrimitives.cpp
# End Source File
# Begin Source File

SOURCE=.\IxConvexTriList.cpp
# End Source File
# Begin Source File

SOURCE=.\McdConvexMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\McdConvexMeshMassProps.c
# End Source File
# Begin Source File

SOURCE=.\McdConvexRegistration.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ConvexGeomUtils.h
# End Source File
# Begin Source File

SOURCE=.\IxConvexLineSegment.h
# End Source File
# Begin Source File

SOURCE=.\IxConvexPrimitives.h
# End Source File
# Begin Source File

SOURCE=.\LineSegment.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdQHullTypes.h
# End Source File
# End Group
# Begin Group "Public Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\McdConvexMesh.h
# End Source File
# End Group
# End Target
# End Project
