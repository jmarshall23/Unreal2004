# Microsoft Developer Studio Project File - Name="ToppleSN" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ToppleSN - Win32 PS2 EE Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ToppleSN.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ToppleSN.mak" CFG="ToppleSN - Win32 PS2 EE Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ToppleSN - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ToppleSN - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "ToppleSN - Win32 PS2 EE Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "ToppleSN - Win32 PS2 EE Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ToppleSN - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "ToppleSN - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "ToppleSN - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ToppleSN___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "ToppleSN___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ToppleSN___Win32_PS2_EE_Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Od /I "../../../3rdParty/ps2gl" /I "../../../3rdParty" /I "../../../include" /D "PS2" /D "SN_TARGET_PS2" /D "_ME_CHECK" /D "MCD_CHECK" /D "WITH_PS2" /D "WITH_BENCHMARK" /D "WITH_OPENGL" /Fo"PS2_EE_Debug/" /FD /debug /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libMeApp.a libMeAssetDB.a libMeAssetDBXMLIO.a libMeAssetFactory.a libMeXML.a libMst.a libMeViewer2.a libMdt.a libMdtBcl.a libMdtKea.a libMcdPrimitives.a libMcdConvex.a libMcdConvexCreateHull.a libMcdCommon.a libMcdFrame.a libMcdPrimitives.a libMcdConvex.a libMcdConvexCreateHull.a libMcdCommon.a libMcdFrame.a libMeGlobals.a libps2glut.a libps2gl.a libps2stuff.a libsn.a libgraph.a libdma.a libdev.a libpkt.a libpad.a libpc.a libstdc++.a libvu0.a /nologo /pdb:none /debug /machine:IX86 /out:"../../bin.dbg/ps2/Topple.elf" /libpath:"../../lib.dbg/ps2" /libpath:"../../lib.chk/ps2" /libpath:"../../../lib.chk/ps2" /libpath:"../../../3rdParty/ps2gl" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "ToppleSN - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ToppleSN___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "ToppleSN___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /O2 /I "../../../3rdParty" /I "../../../3rdParty/ps2gl" /I "../../../include" /D "PS2" /D "SN_TARGET_PS2" /D "WITH_PS2" /D "WITH_BENCHMARK" /D "WITH_OPENGL" /D "NDEBUG" /D "_ME_API_SINGLE" /Fo"PS2_EE_Release/" /FD /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libMeApp.a libMeAssetDB.a libMeAssetDBXMLIO.a libMeAssetFactory.a libMeXML.a libMst.a libMeViewer2.a libMdt.a libMdtBcl.a libMdtKea.a libMcdPrimitives.a libMcdConvex.a libMcdConvexCreateHull.a libMcdCommon.a libMcdFrame.a libMcdPrimitives.a libMcdConvex.a libMcdConvexCreateHull.a libMcdCommon.a libMcdFrame.a libMeGlobals.a libps2glut.a libps2gl.a libps2stuff.a libpc.a libstdc++.a libsn.a libgraph.a libdma.a libdev.a libpkt.a libpad.a libvu0.a /nologo /pdb:none /machine:IX86 /out:"../../bin.rel/ps2/Topple.elf" /libpath:"../../lib.rel/ps2" /libpath:"../../../lib.rel/ps2" /libpath:"../../../" /libpath:"../../../3rdParty/ps2gl" /D:SN_TARGET_PS2

!ENDIF 

# Begin Target

# Name "ToppleSN - Win32 Release"
# Name "ToppleSN - Win32 Debug"
# Name "ToppleSN - Win32 PS2 EE Debug"
# Name "ToppleSN - Win32 PS2 EE Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Topple.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\3rdParty\PS2_in_VC.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\3rdParty\app.cmd
# End Source File
# Begin Source File

SOURCE=..\..\..\3rdParty\crt0.s
# End Source File
# Begin Source File

SOURCE=..\..\..\3rdParty\ps2.lk
# End Source File
# End Target
# End Project
