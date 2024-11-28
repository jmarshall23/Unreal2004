# Microsoft Developer Studio Project File - Name="McdTriangleMeshXb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=McdTriangleMeshXb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "genericXb_lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "genericXb_lib.mak" CFG="McdTriangleMeshXb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McdTriangleMeshXb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "McdTriangleMeshXb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "McdTriangleMeshXb - Xbox Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /I "../src" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MeViewer2/include" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mst/include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.rel/xbox/McdTriangleMeshXb.lib"

!ELSEIF  "$(CFG)" == "McdTriangleMeshXb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "McdTriangleMeshXb___Xbox_Debug"
# PROP BASE Intermediate_Dir "McdTriangleMeshXb___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../include" /I "../src" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MeViewer2/include" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mst/include" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_MECHECK" /D "MCDCHECK" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/xbox/McdTriangleMeshXb.lib"

!ENDIF 

# Begin Target

# Name "McdTriangleMeshXb - Xbox Release"
# Name "McdTriangleMeshXb - Xbox Debug"
# Begin Group "public headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\McdTriangleMeshXb.h
# End Source File
# End Group
# Begin Group "Internal headers"

# PROP Default_Filter ""
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\McdTriangleMeshXb.c
# End Source File
# End Group
# End Target
# End Project
