# Microsoft Developer Studio Project File - Name="SNQuadBike" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=SNQuadBike - Win32 PS2 EE Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "QuadBikeSN.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "QuadBikeSN.mak" CFG="SNQuadBike - Win32 PS2 EE Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SNQuadBike - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "SNQuadBike - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "SNQuadBike - Win32 PS2 EE Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "SNQuadBike - Win32 PS2 EE Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SNQuadBike - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SNQuadBike___Win32_Release"
# PROP BASE Intermediate_Dir "SNQuadBike___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../../MeApp/include" /I "../../../../Mst/include" /I "../../../../include" /I "../../../../MdtBcl/include" /I "../../../../MeViewer2/include" /I "../../../../MdtKea/include" /I "../../../../MeGlobals/include" /I "../../../../Mdt/include" /I "../../../../Mcd/include" /I "../../../../tools/glut" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"../../../../lib.rel/win32" /libpath:"../../../../tools/glut" /libpath:"../../../../3rdparty/glut"

!ELSEIF  "$(CFG)" == "SNQuadBike - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SNQuadBike___Win32_Debug"
# PROP BASE Intermediate_Dir "SNQuadBike___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "SNQuadBike___Win32_Debug"
# PROP Intermediate_Dir "SNQuadBike___Win32_Debug"
# PROP Ignore_Export_Lib 0
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

!ELSEIF  "$(CFG)" == "SNQuadBike - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SNQuadBike___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "SNQuadBike___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "SNQuadBike___Win32_PS2_EE_Debug"
# PROP Intermediate_Dir "SNQuadBike___Win32_PS2_EE_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Od /I "../../../../include" /I "../../../../3rdParty" /D "SN_TARGET_PS2" /D "PS2" /D "_DEBUG" /D "_MECHECK" /D "MCD_CHECK" /Fo"PS2_EE_Debug/" /FD /debug /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libMeApp.a libMst.a libMcdFrame.a libMcdCommon.a libMcdPrimitives.a libMcdFrame.a libMcdCommon.a libMcdPrimitives.a libMeViewer2.a libMdt.a libMdtBcl.a libMdtKea.a libMeGlobals.a libps2gl.a libps2glut.a libps2stuff.a libstdc++.a libsn.a libgcc.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a libpc.a /nologo /pdb:none /debug /machine:IX86 /out:"..\..\bin.dbg\SNQuadBike.elf" /libpath:"../../../../lib.dbg/ps2" /libpath:"../../../../lib.chk/ps2" /libpath:"../../../../3rdParty" /libpath:"../../../../3rdParty/ps2gl" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "SNQuadBike - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SNQuadBike___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "SNQuadBike___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /O2 /I "../../../../include" /I "../../../../3rdParty" /D "SN_TARGET_PS2" /D "PS2" /D "NDEBUG" /Fo"PS2_EE_Release/" /FD -G0 /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libMeApp.a libMst.a libMcdFrame.a libMcdCommon.a libMcdPrimitives.a libMcdFrame.a libMcdCommon.a libMcdPrimitives.a libMeViewer2.a libMdt.a libMdtBcl.a libMdtKea.a libMeGlobals.a libps2gl.a libps2glut.a libps2stuff.a libstdc++.a libsn.a libgcc.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a libpc.a /nologo /pdb:none /machine:IX86 /out:"..\..\bin.rel\SNQuadBike.elf" /libpath:"../../../../lib.dbg/ps2" /libpath:"../../../../lib.chk/ps2" /libpath:"../../../../3rdParty" /libpath:"../../../../3rdParty/ps2gl" /D:SN_TARGET_PS2

!ENDIF 

# Begin Target

# Name "SNQuadBike - Win32 Release"
# Name "SNQuadBike - Win32 Debug"
# Name "SNQuadBike - Win32 PS2 EE Debug"
# Name "SNQuadBike - Win32 PS2 EE Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\controls.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\rider.c
# End Source File
# Begin Source File

SOURCE=.\terrain.c
# End Source File
# Begin Source File

SOURCE=.\utils.c
# End Source File
# Begin Source File

SOURCE=.\vehicle.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\controls.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\3rdParty\PS2_in_VC.h
# End Source File
# Begin Source File

SOURCE=.\rider.h
# End Source File
# Begin Source File

SOURCE=.\terrain.h
# End Source File
# Begin Source File

SOURCE=.\utils.h
# End Source File
# Begin Source File

SOURCE=.\vehicle.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\..\..\..\3rdParty\app.cmd
# End Source File
# Begin Source File

SOURCE=..\..\..\..\3rdParty\crt0.s
# End Source File
# Begin Source File

SOURCE=.\QuadBikePastes.txt
# End Source File
# End Target
# End Project
