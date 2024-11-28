# Microsoft Developer Studio Project File - Name="GreaseMonkey" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GreaseMonkey - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GreaseMonkey.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GreaseMonkey.mak" CFG="GreaseMonkey - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GreaseMonkey - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GreaseMonkey - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GreaseMonkey - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /Oy- /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /G6 /MD /W3 /GX /O2 /Oy- /I "src\RW\shared\skel" /I "src\RW\shared\skel\win" /I "src\RW\shared\democom" /I "src\karma\app" /I "src\karma\plat" /I "..\..\MeGlobals\include" /I "..\..\Mst\include" /I "..\..\MdtKea\include" /I "..\..\Mdt\include" /I "..\..\Mcd\include" /I "..\..\MdtBcl\include" /I "c:\rw3sdk\rwsdk\include\d3d" /D "NDEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "USE_SOUND" /D "PERFORMANCE_METRICS" /FR /YX /FD /c
# SUBTRACT CPP /nologo
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Mdt.lib MdtKea.lib McdFrame.lib McdCommon.lib McdPrimitives.lib Mst.lib MeGlobals.lib MdtBcl.lib rt2d.lib rpworld.lib rtbmp.lib rwcore.lib rtcharse.lib rtslerp.lib rplogo.lib rtimport.lib rtworld.lib rtpng.lib rpcollis.lib rtintsec.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dinput.lib QMDX.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc" /out:"./GreaseMonkey.exe" /libpath:"..\..\lib.rel\win32" /libpath:"..\..\lib.rel\win32_single_libc"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "GreaseMonkey - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\Mcd\include" /I "..\..\Mdt\include" /I "..\..\MdtKea\include" /I "..\..\MeGlobals\include" /I "..\..\Mst\include" /I "..\..\MdtBcl\include" /I "src\RW\shared\democom" /I "src\Karma\plat" /I "src\RW\shared\skel" /I "src\RW\shared\skel\win" /I "src\Karma\app" /D "RWDEBUG" /D "_MECHECK" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_SOUND" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Mdt.lib MdtKea.lib McdPrimitives.lib Mst.lib McdFrame.lib McdCommon.lib MeGlobals.lib MdtBcl.lib rt2d.lib rwcore.lib rpworld.lib rtcharse.lib rpcollis.lib rtintsec.lib rtslerp.lib rplogo.lib rtimport.lib rtbmp.lib rtworld.lib rtpng.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dinput.lib QMDX.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /out:"./GreaseMonkey_debug.exe" /pdbtype:sept /libpath:"..\..\lib.chk\win32_single_libc"

!ENDIF 

# Begin Target

# Name "GreaseMonkey - Win32 Release"
# Name "GreaseMonkey - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "RW_Skeleton_App"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\src\RW\shared\democom\camera.c
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\skeleton.c
# End Source File
# End Group
# Begin Group "Windows Specific"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\src\RW\shared\skel\win\win.c
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\win\win.rc

!IF  "$(CFG)" == "GreaseMonkey - Win32 Release"

!ELSEIF  "$(CFG)" == "GreaseMonkey - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\Karma\plat\winextra.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\Karma\app\car.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\carAPI.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\carsound.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\control.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\driver.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\events.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\hack.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\init.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\MdtCar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\mdtmodel.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\parser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\RwFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\smoke.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\RW\shared\democom\camera.h
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\car.hpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\carAPI.hpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\CarData.hpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\carsound.hpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\control.hpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\driver.hpp
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\events.h
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\init.hpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\KeaCart.hpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\MdtCar.hpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\mdtmodel.hpp
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\democom\menu.h
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\metrics.h
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\mouse.h
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\mousedat.h
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\parser.hpp
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\platform.h
# End Source File
# Begin Source File

SOURCE=.\src\Karma\plat\platxtra.h
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\democom\ptrdata.h
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\QMDX.hpp
# End Source File
# Begin Source File

SOURCE=.\src\Karma\plat\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\win\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\Karma\plat\resrc1.h
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\RwFuncs.hpp
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\skeleton.h
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\smoke.hpp
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\win\splash.h
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\terminal.h
# End Source File
# Begin Source File

SOURCE=.\src\Karma\app\utils.hpp
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\vecfont.h
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\win\win.h
# End Source File
# Begin Source File

SOURCE=.\src\Karma\plat\winextra.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\src\plat\TitleGlue.bmp
# End Source File
# Begin Source File

SOURCE=.\src\plat\twoB.bmp
# End Source File
# Begin Source File

SOURCE=.\src\plat\twoE.bmp
# End Source File
# End Group
# Begin Group "Html"

# PROP Default_Filter "Html"
# Begin Source File

SOURCE=.\Html\cardemo.html
# End Source File
# End Group
# Begin Source File

SOURCE=.\Scripts\grmonkey.cfg
# End Source File
# End Target
# End Project
