# Microsoft Developer Studio Project File - Name="MeGlobalsXb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=MeGlobalsXb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MeGlobalsXb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MeGlobalsXb.mak" CFG="MeGlobalsXb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MeGlobalsXb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "MeGlobalsXb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "MeGlobalsXb - Xbox Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /I "../src" /I "../../Mst/include" /I "../../MdtBcl/include" /I "../../MeViewer2/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:"../../lib.rel/xbox/MeGlobals.lib"
# SUBTRACT LIB32 /nologo

!ELSEIF  "$(CFG)" == "MeGlobalsXb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MeGlobalsXb___Xbox_Debug"
# PROP BASE Intermediate_Dir "MeGlobalsXb___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../include" /I "../src" /I "../../Mst/include" /I "../../MdtBcl/include" /I "../../MeViewer2/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_MECHECK" /D "MCDCHECK" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/xbox/MeGlobals.lib"

!ENDIF 

# Begin Target

# Name "MeGlobalsXb - Xbox Release"
# Name "MeGlobalsXb - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "xbox"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\MeProfile_xbox.c
# End Source File
# Begin Source File

SOURCE=.\MeSimpleFile_xbox.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\MeASELoad.c
# End Source File
# Begin Source File

SOURCE=.\MeBounding.c
# End Source File
# Begin Source File

SOURCE=.\MeChunk.c
# End Source File
# Begin Source File

SOURCE=.\MeCommandLine.c
# End Source File
# Begin Source File

SOURCE=.\MeDebugDraw.c
# End Source File
# Begin Source File

SOURCE=.\MeDict.c
# End Source File
# Begin Source File

SOURCE=.\MeFileSearch.c
# End Source File
# Begin Source File

SOURCE=.\MeHash.c
# End Source File
# Begin Source File

SOURCE=.\MeHeap.c
# End Source File
# Begin Source File

SOURCE=.\MeIDPool.c
# End Source File
# Begin Source File

SOURCE=.\MeMath.c
# End Source File
# Begin Source File

SOURCE=.\MeMemory.c
# End Source File
# Begin Source File

SOURCE=.\MeMemoryCpp.cpp
# End Source File
# Begin Source File

SOURCE=.\MeMessage.c
# End Source File
# Begin Source File

SOURCE=.\MeMisc.c
# End Source File
# Begin Source File

SOURCE=.\MePool.c
# End Source File
# Begin Source File

SOURCE=.\MePoolx.c
# End Source File
# Begin Source File

SOURCE=.\MePrecision.c
# End Source File
# Begin Source File

SOURCE=.\MeProfile.c
# End Source File
# Begin Source File

SOURCE=.\MeSet.c
# End Source File
# Begin Source File

SOURCE=.\MeSimpleFile.c
# End Source File
# Begin Source File

SOURCE=.\MeStream.c
# End Source File
# Begin Source File

SOURCE=.\MeString.c
# End Source File
# Begin Source File

SOURCE=.\MeVersion.c
# End Source File
# End Group
# Begin Group "public headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\MeASELoad.h
# End Source File
# Begin Source File

SOURCE=..\include\MeAssert.h
# End Source File
# Begin Source File

SOURCE=..\include\MeBounding.h
# End Source File
# Begin Source File

SOURCE=..\include\MeCall.h
# End Source File
# Begin Source File

SOURCE=..\include\MeChunk.h
# End Source File
# Begin Source File

SOURCE=..\include\MeCommandLine.h
# End Source File
# Begin Source File

SOURCE=..\include\MeDebugDraw.h
# End Source File
# Begin Source File

SOURCE=..\include\MeDict.h
# End Source File
# Begin Source File

SOURCE=..\include\MeHash.h
# End Source File
# Begin Source File

SOURCE=..\include\MeHeap.h
# End Source File
# Begin Source File

SOURCE=..\include\MeIdPool.h
# End Source File
# Begin Source File

SOURCE=..\include\MeInline.h
# End Source File
# Begin Source File

SOURCE=..\include\MeMath.h
# End Source File
# Begin Source File

SOURCE=..\include\MeMemory.h
# End Source File
# Begin Source File

SOURCE=..\include\MeMessage.h
# End Source File
# Begin Source File

SOURCE=..\include\MeMisc.h
# End Source File
# Begin Source File

SOURCE=..\include\MePool.h
# End Source File
# Begin Source File

SOURCE=..\include\MePrecision.h
# End Source File
# Begin Source File

SOURCE=..\include\MeProfile.h
# End Source File
# Begin Source File

SOURCE=..\include\MeSimpleFile.h
# End Source File
# Begin Source File

SOURCE=..\include\MeStream.h
# End Source File
# Begin Source File

SOURCE=..\include\MeString.h
# End Source File
# Begin Source File

SOURCE=..\include\MeVersion.h
# End Source File
# End Group
# End Target
# End Project
