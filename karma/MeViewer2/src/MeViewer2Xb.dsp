# Microsoft Developer Studio Project File - Name="MeViewer2Xb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=MeViewer2Xb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MeViewer2Xb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MeViewer2Xb.mak" CFG="MeViewer2Xb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MeViewer2Xb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "MeViewer2Xb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "MeViewer2Xb - Xbox Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /GX /O2 /I "./backends" /I "./frame" /I "../include" /I "../../Mst/include" /I "../../MdtBcl/include" /I "../../MeViewer2/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:"../../lib.rel/xbox/MeViewer2.lib"
# SUBTRACT LIB32 /nologo

!ELSEIF  "$(CFG)" == "MeViewer2Xb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MeViewer2Xb___Xbox_Debug"
# PROP BASE Intermediate_Dir "MeViewer2Xb___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "./backends" /I "./frame" /I "../include" /I "../../Mst/include" /I "../../MdtBcl/include" /I "../../MeViewer2/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_MECHECK" /D "MCDCHECK" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/xbox/MeViewer2.lib"

!ENDIF 

# Begin Target

# Name "MeViewer2Xb - Xbox Release"
# Name "MeViewer2Xb - Xbox Debug"
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

SOURCE=.\backends\Render_xbox.h
# End Source File
# Begin Source File

SOURCE=.\backends\Resource_d3d.h
# End Source File
# Begin Source File

SOURCE=.\frame\RMouseCam.h
# End Source File
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
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "ogl"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=.\backends\Init_ogl.c
# End Source File
# Begin Source File

SOURCE=.\backends\Render_ogl.c
# End Source File
# End Group
# Begin Group "benchmark"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=.\backends\Render_benchmark.c
# End Source File
# End Group
# Begin Group "d3d"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=.\backends\Init_d3d.c
# End Source File
# Begin Source File

SOURCE=.\backends\Render_d3d.c
# End Source File
# End Group
# Begin Group "basic"

# PROP Default_Filter ".c"
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
# Begin Group "null"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=.\backends\Render_null.c
# End Source File
# End Group
# Begin Group "xbox"

# PROP Default_Filter ".c"
# Begin Source File

SOURCE=.\backends\Render_xbox.c
# End Source File
# End Group
# End Group
# End Target
# End Project
