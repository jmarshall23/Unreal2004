# Microsoft Developer Studio Project File - Name="MeAssetDBXMLIOXb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=MeAssetDBXMLIOXb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MeAssetDBXMLIOXb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MeAssetDBXMLIOXb.mak" CFG="MeAssetDBXMLIOXb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MeAssetDBXMLIOXb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "MeAssetDBXMLIOXb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "MeAssetDBXMLIOXb - Xbox Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../include" /I "../src" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MeViewer2/include" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mst/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:"../../lib.rel/xbox/MeAssetDBXMLIO.lib"
# SUBTRACT LIB32 /nologo

!ELSEIF  "$(CFG)" == "MeAssetDBXMLIOXb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MeAssetDBXMLIOXb___Xbox_Debug"
# PROP BASE Intermediate_Dir "MeAssetDBXMLIOXb___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../../MeAssetDB/include ../../MeAssetDBXMLIO/include" /I "../include" /I "../src" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MeViewer2/include" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mst/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_MECHECK" /D "MCDCHECK" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/xbox/MeAssetDBXMLIO.lib"

!ENDIF 

# Begin Target

# Name "MeAssetDBXMLIOXb - Xbox Release"
# Name "MeAssetDBXMLIOXb - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MeAssetDBXMLInput_1_0.c
# End Source File
# Begin Source File

SOURCE=.\MeAssetDBXMLIO.c
# End Source File
# Begin Source File

SOURCE=.\MeAssetDBXMLOutput_1_0.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\MeAssetDBXMLIO.h
# End Source File
# Begin Source File

SOURCE=.\MeAssetDBXMLIO_1_0.h
# End Source File
# Begin Source File

SOURCE=..\include\MeAssetDBXMLIOTypes.h
# End Source File
# End Group
# End Target
# End Project
