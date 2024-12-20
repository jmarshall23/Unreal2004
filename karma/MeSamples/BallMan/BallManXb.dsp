# Microsoft Developer Studio Project File - Name="BallManXb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=BallManXb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BallManXb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BallManXb.mak" CFG="BallManXb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BallManXb - Xbox Release" (based on "Xbox Application")
!MESSAGE "BallManXb - Xbox Debug" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "BallManXb - Xbox Release"

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
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /Oy- /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /W3 /GX /O2 /Oy- /I "../../util" /I "../../include" /I "../util" /I "../../Mst/include" /I "../../MdtBcl/include" /I "../../MeViewer2/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../tools/glut" /I "../../MeApp/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /c
# SUBTRACT CPP /nologo
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /o"Release/BallMan.bsc"
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6 /OPT:REF
# ADD LINK32 Mst.lib MdtBcl.lib McdFrame.lib McdCommon.lib McdConvexCreateHull.lib McdConvex.lib McdPrimitives.lib Mdt.lib MeViewer2.lib MdtKea.lib MeGlobals.lib MeApp.lib xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /machine:I386 /out:"../bin.rel/xbox/BallMan.exe" /libpath:"../../lib.rel/xbox" /subsystem:xbox /fixed:no /OPT:REF
# SUBTRACT LINK32 /nologo /verbose /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /nologo
# ADD XBE /nologo /testname:"BallManXb" /stack:0x20000 /out:"Release/BallMan.xbe"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO

!ELSEIF  "$(CFG)" == "BallManXb - Xbox Debug"

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
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /W3 /Gm /GX /Zi /Od /I "../../util" /I "../../include" /I "../util" /I "../../Mst/include" /I "../../MdtBcl/include" /I "../../MeViewer2/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../tools/glut" /I "../../MeApp/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_MECHECK" /D "MCDCHECK" /Fp"Debug/BallMan.pch" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnld.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 Mst.lib MdtBcl.lib McdFrame.lib McdCommon.lib McdConvexCreateHull.lib McdConvex.lib McdPrimitives.lib Mdt.lib MeViewer2.lib MdtKea.lib MeGlobals.lib MeApp.lib xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /incremental:no /debug /machine:I386 /nodefaultlib:"libcmt" /out:"../bin.dbg/xbox/BallMan.exe" /libpath:"../../lib.dbg/xbox" /libpath:"../../lib.chk/xbox" /subsystem:xbox /fixed:no /debugtype:vc6
XBE=imagebld.exe
# ADD BASE XBE /nologo /debug
# ADD XBE /nologo /stack:0x20000 /debug /out:"Debug/BallMan.xbe"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO

!ENDIF 

# Begin Target

# Name "BallManXb - Xbox Release"
# Name "BallManXb - Xbox Debug"
# Begin Source File

SOURCE=.\BallMan.c
# End Source File
# End Target
# End Project
