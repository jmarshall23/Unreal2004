# Microsoft Developer Studio Project File - Name="CpMeGlobals" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=CpMeGlobals - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CpMeGlobals.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CpMeGlobals.mak" CFG="CpMeGlobals - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CpMeGlobals - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "CpMeGlobals - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "CpMeGlobals - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f CpMeGlobals.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "CpMeGlobals.exe"
# PROP BASE Bsc_Name "CpMeGlobals.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "mmake -R -f MeMakefile PLATFORM:=win32 buildcontext:=karma BUILD:=free CRT:=dll OPTS:=full SYMBOLS:=none codegoing:=intoexe doreldefs:=release profiling:=no inline:=specified builddir:=_w_Release/ msw:=-j4"
# PROP Rebuild_Opt "clean all"
# PROP Target_File "../../lib/_w_Release/MeGlobals.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "CpMeGlobals - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f CpMeGlobals.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "CpMeGlobals.exe"
# PROP BASE Bsc_Name "CpMeGlobals.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "make -f MeMakefile PLATFORM:=win32 buildcontext:=karma BUILD:=checked CRT:=dlld OPTS:=none SYMBOLS:=all codegoing:=intoexe doreldefs:=debug profiling:=no inline:=none builddir:=_w_Debug/ "cdefs:=MeGlobalsTRACE MeGlobalsSTATS""
# PROP Rebuild_Opt "clean all"
# PROP Target_File "../../lib/_b_win32_free_staticd/MeGlobals.lib"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "CpMeGlobals - Win32 Release"
# Name "CpMeGlobals - Win32 Debug"

!IF  "$(CFG)" == "CpMeGlobals - Win32 Release"

!ELSEIF  "$(CFG)" == "CpMeGlobals - Win32 Debug"

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
# Begin Source File

SOURCE=.\MeMakefile
# End Source File
# End Target
# End Project
