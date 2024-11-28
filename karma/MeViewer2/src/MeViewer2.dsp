# Microsoft Developer Studio Project File - Name="MeViewer2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MeViewer2 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MeViewer2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MeViewer2.mak" CFG="MeViewer2 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MeViewer2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MeViewer2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MeViewer2 - Win32 Release"

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
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /Oy- /I "backends" /I "frame" /I "../include" /I "../../../../include" /I "../../MeGlobals/include" /I "../../tools/glut" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "WITH_BENCHMARK" /D "WITH_D3D" /D "WITH_OPENGL" /D "WITH_NULL" /D "_ME_OGL_DELAYLOAD" /D "_ME_D3D_DELAYLOAD" /D "_ME_NOPROFILING" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.rel/win32/MeViewer2.lib"

!ELSEIF  "$(CFG)" == "MeViewer2 - Win32 Debug"

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
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "backends" /I "frame" /I "../include" /I "../../../../include" /I "../../MeGlobals/include" /I "../../tools/glut" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "WITH_BENCHMARK" /D "WITH_D3D" /D "WITH_OPENGL" /D "WITH_NULL" /D "_ME_OGL_DELAYLOAD" /D "_ME_D3D_DELAYLOAD" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/win32/MeViewer2.lib"

!ENDIF 

# Begin Target

# Name "MeViewer2 - Win32 Release"
# Name "MeViewer2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "basic"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\frame\MeViewer.c
# End Source File
# Begin Source File

SOURCE=.\frame\RGeometryUtils.c
# End Source File
# Begin Source File

SOURCE=.\frame\RMenu.c
# End Source File
# Begin Source File

SOURCE=.\frame\RMouseCam.c
# End Source File
# End Group
# Begin Group "benchmark"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\backends\Render_benchmark.c
# End Source File
# End Group
# Begin Group "null"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\backends\Render_null.c
# End Source File
# End Group
# Begin Group "ogl"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\backends\Init_ogl.c
# End Source File
# Begin Source File

SOURCE=.\backends\Render_ogl.c
# End Source File
# End Group
# Begin Group "d3d"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\backends\Init_d3d.c
# End Source File
# Begin Source File

SOURCE=.\backends\Render_d3d.c
# End Source File
# End Group
# End Group
# Begin Group "External Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\MeViewer.h
# End Source File
# Begin Source File

SOURCE=..\include\MeViewer_d3d.h
# End Source File
# Begin Source File

SOURCE=..\include\MeViewerTypes.h
# End Source File
# Begin Source File

SOURCE=..\include\RGeometryUtils.h
# End Source File
# Begin Source File

SOURCE=..\include\RMenu.h
# End Source File
# End Group
# Begin Group "Internal headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\backends\Init_ogl.h
# End Source File
# Begin Source File

SOURCE=.\backends\Render_benchmark.h
# End Source File
# Begin Source File

SOURCE=.\backends\Render_null.h
# End Source File
# Begin Source File

SOURCE=.\backends\Render_ogl.h
# End Source File
# Begin Source File

SOURCE=.\backends\Render_ps2.h
# End Source File
# Begin Source File

SOURCE=.\backends\Resource_d3d.h
# End Source File
# Begin Source File

SOURCE=.\frame\RMouseCam.h
# End Source File
# End Group
# End Target
# End Project
