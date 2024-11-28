# Microsoft Developer Studio Project File - Name="McdCommonXb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=McdCommonXb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "McdCommonXb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "McdCommonXb.mak" CFG="McdCommonXb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McdCommonXb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "McdCommonXb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "McdCommonXb - Xbox Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /I "../math" /I "../space" /I "../frame" /I "../utils" /I "../convex" /I "../Qhull" /I "../primitives" /I "../../../Mdt/include" /I "../../../Mcd/include" /I "../../../MeViewer2/include" /I "../../../MdtBcl/include" /I "../../../MdtKea/include" /I "../../../MeGlobals/include" /I "../../../Mst/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:"../../../lib.rel/xbox/McdCommon.lib"
# SUBTRACT LIB32 /nologo

!ELSEIF  "$(CFG)" == "McdCommonXb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "McdCommonXb___Xbox_Debug"
# PROP BASE Intermediate_Dir "McdCommonXb___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../include" /I "../math" /I "../space" /I "../frame" /I "../utils" /I "../convex" /I "../Qhull" /I "../primitives" /I "../../../Mdt/include" /I "../../../Mcd/include" /I "../../../MeViewer2/include" /I "../../../MdtBcl/include" /I "../../../MdtKea/include" /I "../../../MeGlobals/include" /I "../../../Mst/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_MECHECK" /D "MCDCHECK" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:"../../../lib.dbg/xbox/McdCommon.lib"
# SUBTRACT LIB32 /nologo

!ENDIF 

# Begin Target

# Name "McdCommonXb - Xbox Release"
# Name "McdCommonXb - Xbox Debug"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\space\CxSmallSort.h
# End Source File
# Begin Source File

SOURCE=..\space\CxSmallSortUtils.h
# End Source File
# Begin Source File

SOURCE=..\space\CxSpace.h
# End Source File
# Begin Source File

SOURCE=.\CxTriangleNE.h
# End Source File
# Begin Source File

SOURCE=..\math\GeomUtils.h
# End Source File
# Begin Source File

SOURCE=..\math\lsTransform.h
# End Source File
# Begin Source File

SOURCE=..\math\lsVec3.h
# End Source File
# Begin Source File

SOURCE=..\math\lsVec4.h
# End Source File
# Begin Source File

SOURCE=..\math\mesffnmin.h
# End Source File
# Begin Source File

SOURCE=..\math\vectormath.h
# End Source File
# End Group
# Begin Group "Public Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\McdContact.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdSpace.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "space"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\space\CxSmallSort.cpp
# End Source File
# Begin Source File

SOURCE=..\space\McdSpace.cpp
# End Source File
# End Group
# Begin Group "math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\math\GeomUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\math\McdVanillaCore.cpp
# End Source File
# Begin Source File

SOURCE=..\math\mesffnmin.cpp
# End Source File
# End Group
# Begin Group "contacts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\McdContact.cpp
# End Source File
# End Group
# End Group
# End Target
# End Project
