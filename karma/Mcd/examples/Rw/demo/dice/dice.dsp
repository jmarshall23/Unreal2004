# Microsoft Developer Studio Project File - Name="dice" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=dice - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dice.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dice.mak" CFG="dice - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dice - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "dice - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dice - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dice___Win32_Release"
# PROP BASE Intermediate_Dir "dice___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /Oy- /I "..\..\..\rwsdk\include\d3d" /I "..\..\skel" /I "..\democom" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "RWLOGO" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /Oy- /Ob2 /I "../../../../../Mst/include" /I "../../../../../mdtkea/include" /I "../../../../../mdt/include" /I "../../../../../MeGlobals/include" /I "../../../../../renderware/v3.0/win32/rwsdk/include/d3d" /I "../../skel" /I "../democom" /I "../../../../include" /I "../../../../../MdtBcl/include" /I "../../../util" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /D "_ME_API_SINGLE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 rwcore.lib rpworld.lib rprandom.lib rplogo.lib rtpng.lib rtcharse.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"./dice.exe" /libpath:"..\..\..\rwsdk\lib\d3d"
# ADD LINK32 Mst.lib McdFrame.lib McdCommon.lib McdPrimitives.lib McdRwBsp.lib MdtKea.lib MdtBcl.lib Mdt.lib MeGlobals.lib rwcore.lib rpworld.lib rprandom.lib rplogo.lib rtpng.lib rtcharse.lib winmm.lib McdRwBSP.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /libpath:"../../../../../renderware/v3.0/win32/rwsdk/lib/d3d" /libpath:"../../../../lib" /libpath:"../../../../../MdtKea/lib" /libpath:"../../../../../Mdt/lib" /libpath:"../../../../../MdtBcl/lib" /libpath:"../../../../../MeGlobals/lib" /libpath:"../../../../../Mst/lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "dice - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dice___Win32_Debug"
# PROP BASE Intermediate_Dir "dice___Win32_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\rwsdk\include\d3d" /I "..\..\skel" /I "..\democom" /D "_DEBUG" /D "RWDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "RWLOGO" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../../../../mdtkea/include" /I "../../../../../Mst/include" /I "../../../../../mdt/include" /I "../../../../../MeGlobals/include" /I "../../../../../renderware/v3.0/win32/rwsdk/include/d3d" /I "../../skel" /I "../democom" /I "../../../../include" /I "../../../../../MdtBcl/include" /I "../../../util" /D "_DEBUG" /D "RWDEBUG" /D "_WINDOWS" /D "_MBCS" /D "WIN32" /D "_ME_API_SINGLE" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 rwcore.lib rpworld.lib rprandom.lib rplogo.lib rtpng.lib rtcharse.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"./dice.exe" /pdbtype:sept /libpath:"..\..\..\rwsdk\lib\d3d\debug"
# ADD LINK32 Mst_debug.lib McdFrame_debug.lib McdPrimitives_debug.lib McdRwBsp_debug.lib MdtKea_debug.lib MdtBcl_debug.lib Mdt_debug.lib MeGlobals_debug.lib rwcore.lib rpworld.lib rprandom.lib rplogo.lib rtpng.lib rtcharse.lib winmm.lib McdRwBSP_Debug.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /pdbtype:sept /libpath:"../../../../../renderware/v3.0/win32/rwsdk/lib/d3d/debug" /libpath:"../../../../lib" /libpath:"../../../../../Mdt/lib" /libpath:"../../../../../MdtKea/lib" /libpath:"../../../../../MdtBcl/lib" /libpath:"../../../../../MeGlobals/lib" /libpath:"../../../../../Mst/lib"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "dice - Win32 Release"
# Name "dice - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "skeleton"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\democom\camera.c
# End Source File
# Begin Source File

SOURCE=..\..\skel\mouse.c
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\democom\padmap.c
# End Source File
# Begin Source File

SOURCE=..\..\skel\skeleton.c
# End Source File
# Begin Source File

SOURCE=..\..\skel\terminal.c
# End Source File
# Begin Source File

SOURCE=..\..\skel\platform\win.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\dice.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "skeleton No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\democom\camera.h
# End Source File
# Begin Source File

SOURCE=..\..\skel\mouse.h
# End Source File
# Begin Source File

SOURCE=..\democom\padmap.h
# End Source File
# Begin Source File

SOURCE=..\..\skel\platform.h
# End Source File
# Begin Source File

SOURCE=..\..\skel\skeleton.h
# End Source File
# Begin Source File

SOURCE=..\..\skel\terminal.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\skel\platform\win.rc
# End Source File
# End Group
# End Target
# End Project
