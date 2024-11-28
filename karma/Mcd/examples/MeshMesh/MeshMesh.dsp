# Microsoft Developer Studio Project File - Name="MeshMesh" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=MeshMesh - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MeshMesh.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MeshMesh.mak" CFG="MeshMesh - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MeshMesh - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "MeshMesh - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MeshMesh - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /Oy- /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /Oy- /I "../../../MeApp/include" /I "../../../Mst/include" /I "../../../MeViewer2/include" /I "../../../MdtKea/include" /I "../../../MeGlobals/include" /I "../../../Mdt/include" /I "../../../cx/include" /I "../../../Mcd/include" /I "../util" /I "../../../MdtBcl/include" /I "../../../tools/glut" /I "../include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 MeApp.lib McdTriangleMesh.lib McdPrimitives.lib McdFrame.lib McdCommon.lib MdtBcl.lib MeViewer2.lib Mdt.lib MdtKea.lib MeGlobals.lib kernel32.lib user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /map /debug /machine:I386 /out:"../bin/MeshMesh.exe" /libpath:"../../../lib.rel/win32" /libpath:"../../../tools/glut" /libpath:"../../../3rdParty/glut"
# SUBTRACT LINK32 /verbose

!ELSEIF  "$(CFG)" == "MeshMesh - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob1 /I "../../../MeApp/include" /I "../include" /I "../../../MeViewer2/include" /I "../../../MdtKea/include" /I "../../../MeGlobals/include" /I "../../../Mdt/include" /I "../../../cx/include" /I "../../../Mcd/include" /I "../util" /I "../../../MdtBcl/include" /I "../../../tools/glut" /I "../../../Mst/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 McdTriangleMesh.lib MeGlobals.lib McdPrimitives.lib McdFrame.lib McdCommon.lib MdtBcl.lib MeViewer2.lib Mdt.lib MdtKea.lib kernel32.lib user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /map /debug /machine:I386 /out:"../bin/MeshMesh_debug.exe" /pdbtype:sept /libpath:"../../../lib.dbg/win32" /libpath:"../../../lib.chk/win32" /libpath:"../../../tools/glut" /libpath:"../../../3rdParty/glut"

!ENDIF 

# Begin Target

# Name "MeshMesh - Win32 Release"
# Name "MeshMesh - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MeshTest.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\McdTriangleMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McduDrawTrace.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McduDrawTriangleMesh.h
# End Source File
# Begin Source File

SOURCE=..\include\McduMeshSphere.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McduTriangleMeshIO.h
# End Source File
# End Group
# End Target
# End Project
