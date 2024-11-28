# Microsoft Developer Studio Project File - Name="vis" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=vis - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vis.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vis.mak" CFG="vis - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vis - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "vis - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vis - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /Oy- /I "..\..\..\Mps\include" /I "..\..\include" /I "..\..\..\MeViewer\include" /I "..\..\..\MeGlobals\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "not_WITH_MPS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 Mps.lib McdParticle.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib MeGlobals.lib MeViewer.lib McdFrame.lib McdCommon.lib McdConvex.lib McdConvexCreateHull.lib McdRGHeightfield.lib McdTriangleMesh.lib /nologo /subsystem:console /machine:I386 /out:"../bin/vis.exe" /libpath:"C:\Program Files\Mathengine\SDK\lib" /libpath:"..\..\..\Mcd\lib" /libpath:"..\..\..\Mps\lib" /libpath:"..\..\..\lib" /libpath:"..\..\..\MeGlobals\lib" /libpath:"..\..\..\Mdt\lib" /libpath:"..\..\..\MdtBcl\lib" /libpath:"..\..\..\MdtKea\lib" /libpath:"..\..\..\MeViewer\lib" /libpath:"..\..\..\tools\glut"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "vis - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\MeViewer\include" /I "..\..\..\MeGlobals\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_ME_API_SINGLE" /D "LS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 McdPrimitives_debug.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib MeGlobals_debug.lib MeViewer_debug.lib McdFrame_debug.lib McdConvex_debug.lib McdConvexCreateHull_debug.lib McdRGHeightfield_debug.lib McdTriangleMesh_debug.lib /nologo /subsystem:console /debug /machine:I386 /out:"../bin/vis_debug.exe" /pdbtype:sept /libpath:"..\..\lib" /libpath:"..\..\..\mdtkea\lib" /libpath:"..\..\..\mdt\lib" /libpath:"..\..\..\lib" /libpath:"..\..\..\MeGlobals\lib" /libpath:"..\..\..\Mdt\lib" /libpath:"..\..\..\MdtBcl\lib" /libpath:"..\..\..\MdtKea\lib" /libpath:"..\..\..\MeViewer\lib" /libpath:"..\..\..\tools\glut"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "vis - Win32 Release"
# Name "vis - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\vis.cpp
# End Source File
# Begin Source File

SOURCE=.\vispoints.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
