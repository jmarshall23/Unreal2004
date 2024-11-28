# Microsoft Developer Studio Project File - Name="dominoes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=dominoes - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dominoes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dominoes.mak" CFG="dominoes - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dominoes - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dominoes - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dominoes - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /Oy- /I "../../../MeViewer/include" /I "../../../MdtKea/include" /I "../../../MeGlobals/include" /I "../../../Mdt/include" /I "../../../Mcd/include" /I "../../../MdtBcl/include" /I "../util" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 MdtKea.lib MeViewer.lib McdFrame.lib McdCommon.lib McdPrimitives.lib MeGlobals.lib Mdt.lib MdtBcl.lib kernel32.lib user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386 /out:"./bin/dominoes.exe" /libpath:"../../../MdtBcl/lib" /libpath:"../../../MeGlobals/lib" /libpath:"../../../Mcd/lib" /libpath:"../../../MeViewer/lib" /libpath:"../../../Mdt/lib" /libpath:"../../../MdtKea/lib" /libpath:"../../../tools/glut"

!ELSEIF  "$(CFG)" == "dominoes - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../MeViewer/include" /I "../../../MdtKea/include" /I "../../../MeGlobals/include" /I "../../../Mdt/include" /I "../../../Mcd/include" /I "../../../MdtBcl/include" /I "../util" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 MdtKea_debug.lib MeViewer_debug.lib McdFrame_debug.lib McdPrimitives_debug.lib MeGlobals_debug.lib Mdt_debug.lib MdtBcl_debug.lib kernel32.lib user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../bin/dominoes_debug.exe" /pdbtype:sept /libpath:"../../../cx/lib" /libpath:"../../../MdtBcl/lib" /libpath:"../../../MeGlobals/lib" /libpath:"../../../Mcd/lib" /libpath:"../../../MeViewer/lib" /libpath:"../../../Mdt/lib" /libpath:"../../../MdtKea/lib" /libpath:"../../../tools/glut"

!ENDIF 

# Begin Target

# Name "dominoes - Win32 Release"
# Name "dominoes - Win32 Debug"
# Begin Source File

SOURCE=.\cdDominoes.c
# End Source File
# End Target
# End Project
