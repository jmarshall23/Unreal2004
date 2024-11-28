# Microsoft Developer Studio Project File - Name="AssetViewer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=AssetViewer - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AssetViewer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AssetViewer.mak" CFG="AssetViewer - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AssetViewer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "AssetViewer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AssetViewer - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../MeAssetFactory" /I "../../MeAssetDB/include" /I "../../MeXML/include" /I "../../Mst/include" /I "../../MeApp/include" /I "../../include" /I "../../MdtBcl/include" /I "../../MeViewer2/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../tools/glut" /I "../../MeAssetDBXMLIO/include/" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 McdConvex.lib McdConvexCreateHull.lib MeAssetFactory.lib MeAssetDB.lib MeXML.lib MeAssetDBXMLIO.lib Mst.lib MeApp.lib McdFrame.lib McdCommon.lib McdPrimitives.lib MdtBcl.lib Mdt.lib MeViewer2.lib MdtKea.lib MeGlobals.lib kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib comctl32.lib netapi32.lib /nologo /subsystem:console /machine:I386 /out:"../bin.rel/win32/AssetViewer.exe" /libpath:"../../lib.rel/win32" /libpath:"../../tools/glut" /libpath:"../../3rdparty/glut"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "AssetViewer - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../MeAssetFactory/include" /I "../../MeAssetDB/include" /I "../../MeXML/include" /I "../../Mst/include" /I "../../MeApp/include" /I "../../include" /I "../../MdtBcl/include" /I "../../MeViewer2/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../tools/glut" /I "../../MeAssetDBXMLIO/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 McdConvex.lib McdConvexCreateHull.lib MeAssetFactory.lib MeAssetDB.lib MeXML.lib MeAssetDBXMLIO.lib Mst.lib MeApp.lib McdFrame.lib McdCommon.lib McdPrimitives.lib MdtBcl.lib Mdt.lib MeViewer2.lib MdtKea.lib MeGlobals.lib kernel32.lib user32.lib gdi32.lib advapi32.lib comdlg32.lib comctl32.lib netapi32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"LIBC" /out:"../bin.dbg/win32/AssetViewer.exe" /pdbtype:sept /libpath:"../../lib.dbg/win32" /libpath:"../../lib.chk/win32" /libpath:"../../tools/glut" /libpath:"../../3rdparty/glut"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "AssetViewer - Win32 Release"
# Name "AssetViewer - Win32 Debug"
# Begin Source File

SOURCE=.\AssetViewer.c
# End Source File
# Begin Source File

SOURCE=.\simulation.c
# End Source File
# Begin Source File

SOURCE=.\simulation.h
# End Source File
# Begin Source File

SOURCE=.\test.ka
# End Source File
# Begin Source File

SOURCE=.\visualization.c
# End Source File
# Begin Source File

SOURCE=.\visualization.h
# End Source File
# End Target
# End Project
