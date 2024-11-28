# Microsoft Developer Studio Project File - Name="McdFrameXb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=McdFrameXb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "McdFrameXb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "McdFrameXb.mak" CFG="McdFrameXb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McdFrameXb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "McdFrameXb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "McdFrameXb - Xbox Release"

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
# ADD LIB32 /out:"../../../lib.rel/xbox/McdFrame.lib"
# SUBTRACT LIB32 /nologo

!ELSEIF  "$(CFG)" == "McdFrameXb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "McdFrameXb___Xbox_Debug"
# PROP BASE Intermediate_Dir "McdFrameXb___Xbox_Debug"
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
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.dbg/xbox/McdFrame.lib"

!ENDIF 

# Begin Target

# Name "McdFrameXb - Xbox Release"
# Name "McdFrameXb - Xbox Debug"
# Begin Group "Utility Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\McduDebugDraw.h
# End Source File
# End Group
# Begin Group "Public Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\Mcd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdAggregate.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdBatch.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdContact.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdCTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdCullingTable.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdGeometry.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdGeometryInstance.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdGeometryTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdInteractions.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdInteractionTable.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdIntersectResult.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdModel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdModelPair.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdModelPairContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdModelPairManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdNull.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdSpace.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\McdAggregate.cpp
# End Source File
# Begin Source File

SOURCE=.\McdBatch.cpp
# End Source File
# Begin Source File

SOURCE=.\McdFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\McdGeometry.cpp
# End Source File
# Begin Source File

SOURCE=.\McdGeometryInstance.cpp
# End Source File
# Begin Source File

SOURCE=.\McdInteractions.cpp
# End Source File
# Begin Source File

SOURCE=.\McdMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\McdModel.cpp
# End Source File
# Begin Source File

SOURCE=.\McdModelPair.cpp
# End Source File
# Begin Source File

SOURCE=.\McdModelPairContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\McdModelPairManager.cpp
# End Source File
# Begin Source File

SOURCE=.\McdNull.cpp
# End Source File
# Begin Source File

SOURCE=.\McduDebugDraw.cpp
# End Source File
# End Group
# End Target
# End Project
