# Microsoft Developer Studio Project File - Name="MeViewer2SN" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MeViewer2SN - Win32 PS2 EE Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MeViewer2SN.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MeViewer2SN.mak" CFG="MeViewer2SN - Win32 PS2 EE Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MeViewer2SN - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MeViewer2SN - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "MeViewer2SN - Win32 PS2 EE Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "MeViewer2SN - Win32 PS2 EE Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MeViewer2SN - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MeViewer2SN___Win32_Release"
# PROP BASE Intermediate_Dir "MeViewer2SN___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MeViewer2SN___Win32_Release"
# PROP Intermediate_Dir "MeViewer2SN___Win32_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MeViewer2SN___Win32_Debug"
# PROP BASE Intermediate_Dir "MeViewer2SN___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MeViewer2SN___Win32_Debug"
# PROP Intermediate_Dir "MeViewer2SN___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MeViewer2SN___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "MeViewer2SN___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../lib.dbg/ps2"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Od /I "frame" /I "backends" /I "../../../../include" /I "../../../../3rdParty/ps2gl" /I "../../../../3rdParty" /D "PS2" /D "SN_TARGET_PS2" /D "_ME_CHECK" /D "MCD_CHECK" /D "WITH_PS2" /D "WITH_BENCHMARK" /D "WITH_OPENGL" /Fo"PS2_EE_Debug/" /FD /debug /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.dbg/ps2/MeViewer2.lib" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MeViewer2SN___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "MeViewer2SN___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../lib.rel/ps2"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /O2 /I "frame" /I "backends" /I "../../../../include" /I "../../../../3rdParty/ps2gl" /I "../../../../3rdParty" /D "PS2" /D "SN_TARGET_PS2" /D "WITH_PS2" /D "WITH_BENCHMARK" /D "WITH_OPENGL" /D "NDEBUG" /D "_ME_API_SINGLE" /Fo"PS2_EE_Release/" /FD /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809
BSC32=snBsc.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=snLib.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.rel/ps2/MeViewer2.lib" /D:SN_TARGET_PS2

!ENDIF 

# Begin Target

# Name "MeViewer2SN - Win32 Release"
# Name "MeViewer2SN - Win32 Debug"
# Name "MeViewer2SN - Win32 PS2 EE Debug"
# Name "MeViewer2SN - Win32 PS2 EE Release"
# Begin Group "Source Files"

# PROP Default_Filter ".c;.cpp"
# Begin Group "frame"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\frame\MeViewer.c
# End Source File
# Begin Source File

SOURCE=.\frame\RGeometryUtils.c
# End Source File
# Begin Source File

SOURCE=.\frame\RMenu.c
# End Source File
# Begin Source File

SOURCE=.\frame\RMouseCam.c
# End Source File
# End Group
# Begin Group "backend"

# PROP Default_Filter ".c;.cpp"
# Begin Source File

SOURCE=.\backends\Init_d3d.c

!IF  "$(CFG)" == "MeViewer2SN - Win32 Release"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 Debug"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Debug"

# ADD CPP /I "frame/" /I "backends/"
# SUBTRACT CPP /I "frame"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Release"

# ADD CPP /I "frame/" /I "backends/"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\backends\Init_ogl.c

!IF  "$(CFG)" == "MeViewer2SN - Win32 Release"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 Debug"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Debug"

# ADD CPP /I "frame/" /I "backends/"
# SUBTRACT CPP /I "frame"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Release"

# ADD CPP /I "frame/" /I "backends/"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\backends\Input_ps2.c

!IF  "$(CFG)" == "MeViewer2SN - Win32 Release"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 Debug"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Debug"

# ADD CPP /I "frame/" /I "backends/"
# SUBTRACT CPP /I "frame"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Release"

# ADD CPP /I "frame/" /I "backends/"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\backends\Render_benchmark.c

!IF  "$(CFG)" == "MeViewer2SN - Win32 Release"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 Debug"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Debug"

# ADD CPP /I "frame/" /I "backends/"
# SUBTRACT CPP /I "frame"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Release"

# ADD CPP /I "frame/" /I "backends/"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\backends\Render_d3d.c

!IF  "$(CFG)" == "MeViewer2SN - Win32 Release"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 Debug"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Debug"

# ADD CPP /I "frame/" /I "backends/"
# SUBTRACT CPP /I "frame"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Release"

# ADD CPP /I "frame/" /I "backends/"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\backends\Render_null.c

!IF  "$(CFG)" == "MeViewer2SN - Win32 Release"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 Debug"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Debug"

# ADD CPP /I "frame/" /I "backends/"
# SUBTRACT CPP /I "frame"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Release"

# ADD CPP /I "frame/" /I "backends/"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\backends\Render_ogl.c

!IF  "$(CFG)" == "MeViewer2SN - Win32 Release"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 Debug"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Debug"

# ADD CPP /I "frame/" /I "backends/"
# SUBTRACT CPP /I "frame"

!ELSEIF  "$(CFG)" == "MeViewer2SN - Win32 PS2 EE Release"

# ADD CPP /I "frame/" /I "backends/"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\version.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h;.hpp"
# Begin Source File

SOURCE=.\backends\Init_ogl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\3rdParty\PS2_in_VC.h
# End Source File
# Begin Source File

SOURCE=.\backends\Render_benchmark.h
# End Source File
# Begin Source File

SOURCE=.\backends\Render_null.h
# End Source File
# Begin Source File

SOURCE=.\backends\Render_ogl.h
# End Source File
# Begin Source File

SOURCE=.\backends\Resource_d3d.h
# End Source File
# Begin Source File

SOURCE=.\frame\RMouseCam.h
# End Source File
# End Group
# End Target
# End Project
