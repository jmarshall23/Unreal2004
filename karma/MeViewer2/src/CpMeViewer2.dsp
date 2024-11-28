# Microsoft Developer Studio Project File - Name="CpMeViewer2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=CpMeViewer2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CpMeViewer2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CpMeViewer2.mak" CFG="CpMeViewer2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CpMeViewer2 - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "CpMeViewer2 - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "CpMeViewer2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f CpMeViewer2.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "CpMeViewer2.exe"
# PROP BASE Bsc_Name "CpMeViewer2.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "make -f MeMakefile PLATFORM=win32 BUILD=free CRT=static symbols=none opts=none"
# PROP Rebuild_Opt "clean"
# PROP Target_File "../../lib/_b_win32_free_static/CpMeViewer2.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "CpMeViewer2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f CpMeViewer2.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "CpMeViewer2.exe"
# PROP BASE Bsc_Name "CpMeViewer2.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "make -f MeMakefile PLATFORM=win32 BUILD=free CRT=staticd symbols=all opts=none"
# PROP Rebuild_Opt "clean"
# PROP Target_File "../../lib/_b_win32_free_staticd/CpMeViewer2.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "CpMeViewer2 - Win32 Release"
# Name "CpMeViewer2 - Win32 Debug"

!IF  "$(CFG)" == "CpMeViewer2 - Win32 Release"

!ELSEIF  "$(CFG)" == "CpMeViewer2 - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
