# Microsoft Developer Studio Project File - Name="McdTriangleMesh" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=McdTriangleMesh - Win32 Release CPP
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "McdTriangleMesh.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "McdTriangleMesh.mak" CFG="McdTriangleMesh - Win32 Release CPP"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McdTriangleMesh - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "McdTriangleMesh - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "McdTriangleMesh - Win32 Release"

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
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /Oy- /I "./" /I "../primitives" /I "../frame" /I "../math" /I "../space" /I "../util" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../cx" /I "../../../MeGlobals/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.rel/win32/McdTriangleMesh.lib"

!ELSEIF  "$(CFG)" == "McdTriangleMesh - Win32 Debug"

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
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "./" /I "../primitives" /I "../frame" /I "../math" /I "../space" /I "../util" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../cx" /I "../../../MeGlobals/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_MECHECK" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.dbg/win32/McdTriangleMesh.lib"

!ENDIF 

# Begin Target

# Name "McdTriangleMesh - Win32 Release"
# Name "McdTriangleMesh - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BBoxFitter.cpp
# End Source File
# Begin Source File

SOURCE=.\BVTree.cpp
# End Source File
# Begin Source File

SOURCE=.\CxSODistTriTri.cpp
# End Source File
# Begin Source File

SOURCE=.\CxTriangleMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\IxTriangleMeshLineSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\IxTriangleMeshTriangleMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Jacobi.cpp
# End Source File
# Begin Source File

SOURCE=.\McdTriangleMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\McdTriangleMeshRegisterInteractions.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshMeshDist.cpp
# End Source File
# Begin Source File

SOURCE=.\ObbFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\ObbObb.cpp
# End Source File
# Begin Source File

SOURCE=.\RectRectDist.cpp
# End Source File
# Begin Source File

SOURCE=.\SSR.cpp
# End Source File
# Begin Source File

SOURCE=.\TriTriDist.cpp
# End Source File
# Begin Source File

SOURCE=.\TriTriIsctTest.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BBoxFitter.h
# End Source File
# Begin Source File

SOURCE=.\BVNode.h
# End Source File
# Begin Source File

SOURCE=.\BVTransform.h
# End Source File
# Begin Source File

SOURCE=.\BVTree.h
# End Source File
# Begin Source File

SOURCE=.\BVVec3.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxMemory_c.h
# End Source File
# Begin Source File

SOURCE=.\CxSODistTriTri.h
# End Source File
# Begin Source File

SOURCE=.\CxTriangleMesh.h
# End Source File
# Begin Source File

SOURCE=.\IxTriangleMeshLineSegment.h
# End Source File
# Begin Source File

SOURCE=.\Jacobi.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdTriangleMesh.h
# End Source File
# Begin Source File

SOURCE=.\ObbFinder.h
# End Source File
# Begin Source File

SOURCE=.\ObbObb.h
# End Source File
# Begin Source File

SOURCE=.\RectRectDist.h
# End Source File
# Begin Source File

SOURCE=.\SSR.h
# End Source File
# Begin Source File

SOURCE=.\TimeStamp.h
# End Source File
# Begin Source File

SOURCE=.\TriTriIsctTest.h
# End Source File
# End Group
# End Target
# End Project
