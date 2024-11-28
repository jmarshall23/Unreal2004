# Microsoft Developer Studio Project File - Name="GMonkey_Win_PS2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GMonkey_Win_PS2 - Win32 PS2 EE Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GMonkey_Win_PS2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GMonkey_Win_PS2.mak" CFG="GMonkey_Win_PS2 - Win32 PS2 EE Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GMonkey_Win_PS2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GMonkey_Win_PS2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "GMonkey_Win_PS2 - Win32 PS2 EE Debug" (based on "Win32 (x86) Application")
!MESSAGE "GMonkey_Win_PS2 - Win32 PS2 EE Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=snCl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\renderware\v3.0\win32\rwsdk\include\d3d" /I "\tools\RW3forPC\rwsdk\include\d3d" /I "R:\rwsdk\include\d3d" /I "..\..\MeGlobals\include" /I "..\..\MdtKea\include" /I "..\..\Mdt\include" /I "..\..\Mcd\include" /I "..\..\MdtBcl\include" /I "..\..\Mst\include" /I "c:\rw3sdk\rwsdk\include\d3d" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "PERFORMANCE_METRICS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Mdt.lib MdtKea.lib McdPrimitives.lib McdRwBSP.lib McdFrame.lib McdCommon.lib Mst.lib McdRwBSP.lib MeGlobals.lib MdtBcl.lib rt2d.lib rwcore.lib rpworld.lib rtcharse.lib rpvrml.lib rtslerp.lib rplogo.lib rtimport.lib rtworld.lib rtpng.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dinput.lib QMDX.lib comctl32.lib /nologo /subsystem:windows /pdb:"./GMonkey_Win_PS2.pdb" /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcd.lib" /out:"./GreaseMonkey.exe" /libpath:"..\..\renderware\v3.04\win32\rwsdk\lib\d3d\\" /libpath:"c:\rw3sdk\rwsdk\lib\d3d\\" /libpath:"\tools\RW3forPC\rwsdk\lib\d3d\\" /libpath:"R:\rwsdk\lib\d3d\\" /libpath:"..\..\lib.rel\win32"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\renderware\v3.0\win32\rwsdk\include\d3d" /I "\tools\RW3forPC\rwsdk\include\d3d" /I "R:\rwsdk\include\d3d" /I "..\..\MeGlobals\include" /I "..\..\MdtKea\include" /I "..\..\Mdt\include" /I "..\..\Mcd\include" /I "..\..\MdtBcl\include" /I "..\..\Mst\include" /I "c:\rw3sdk\rwsdk\include\d3d" /D "_MEDEBUG" /D "RWDEBUG" /D "WIN32" /D "_WINDOWS" /D "PERFORMANCE_METRICS" /D "_MECHECK" /YX /FD /GZ  /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Mst.lib Mdt.lib MdtKea.lib McdPrimitives.lib McdRwBSP.lib McdFrame.lib McdCommon.lib McdRwBSP.lib MeGlobals.lib MdtBcl.lib rt2d.lib rwcore.lib rpworld.lib rtcharse.lib rpvrml.lib rtslerp.lib rplogo.lib rtimport.lib rtworld.lib rtpng.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dinput.lib QMDX.lib comctl32.lib Mdt.lib MdtKea.lib McdPrimitives.lib McdFrame.lib McdCommon.lib McdRwBSP.lib MeGlobals.lib MdtBcl.lib rt2d.lib rwcore.lib rpworld.lib rtcharse.lib rpvrml.lib rtslerp.lib rplogo.lib rtimport.lib rtworld.lib rtpng.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dinput.lib QMDX.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcd.lib" /out:"./GreaseMonkey_debug.exe" /pdbtype:sept /libpath:"..\..\renderware\v3.04\win32\rwsdk\lib\d3d\debug" /libpath:"c:\rw3sdk\rwsdk\lib\d3d\debug" /libpath:"\tools\RW3forPC\rwsdk\lib\d3d\debug" /libpath:"R:\rwsdk\lib\d3d\debug" /libpath:"..\..\lib.dbg\win32"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GMonkey_Win_PS2___Win32_PS2_EE_Debug"
# PROP BASE Intermediate_Dir "GMonkey_Win_PS2___Win32_PS2_EE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "PS2_EE_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /w /W0 /Od /I "\usr\local\rw\include\sky\\" /I "\usr\local\mathengine\toolkit-Latest\include" /D "SN_TARGET_PS2" /D "SKY" /D "PS2" /D "_MECHECK" /D "PERFORMANCE_METRICS" /debug -G0 /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libMdt_Debug.a libMdtKea_Debug.a libMcdFrame_Debug.a libMcdPrimitives_Debug.a libMcdDtBridge_Debug.a libMcdRwBSP_Debug.a libMeGlobals_Debug.a libMdtBcl_Debug.a librt2d.a librwcore.a librpworld.a librtcharse.a librpvrml.a librtslerp.a librplogo.a librtimport.a librtworld.a librtpng.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a libpc.a /nologo /subsystem:windows /pdb:none /debug /machine:I386 /out:".\GreaseMonkey_debug.elf" /libpath:"\usr\local\rw\lib\sky" /libpath:"\usr\local\mathengine\toolkit-Latest\lib" /D:SN_TARGET_PS2

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GMonkey_Win_PS2___Win32_PS2_EE_Release"
# PROP BASE Intermediate_Dir "GMonkey_Win_PS2___Win32_PS2_EE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "PS2_EE_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /YX /FD /GZ /c
# ADD CPP /nologo /ML /w /W0 /O2 /I "\usr\local\rw\include\sky" /I "\usr\local\mathengine\toolkit-Latest\include" /D "SN_TARGET_PS2" /D "SKY" /D "PS2" -G0 /c
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x809
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 libMdt.a libMdtKea.a libMcdFrame.a libMcdPrimitives.a libMcdDtBridge.a libMcdRwBSP.a libMeGlobals.a libMdtBcl.a librt2d.a librwcore.a librpworld.a librtcharse.a librpvrml.a librtslerp.a librplogo.a librtimport.a librtworld.a librtpng.a libgraph.a libdma.a libdev.a libpad.a libpkt.a libvu0.a libpc.a /nologo /pdb:none /machine:IX86 /out:".\GreaseMonkey.elf" /libpath:"\usr\local\rw\lib\sky" /libpath:"\usr\local\mathengine\toolkit-Latest\lib" /D:SN_TARGET_PS2

!ENDIF 

# Begin Target

# Name "GMonkey_Win_PS2 - Win32 Release"
# Name "GMonkey_Win_PS2 - Win32 Debug"
# Name "GMonkey_Win_PS2 - Win32 PS2 EE Debug"
# Name "GMonkey_Win_PS2 - Win32 PS2 EE Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "RW_Skeleton_App"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\rw_skel\camera.c
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\metrics.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\mouse.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\padmap.c
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\skeleton.c
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\terminal.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\vecfont.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Windows Specific"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\plat\win.c

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\plat\win.rc

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"
# PROP Exclude_From_Build 1
# ADD BASE RSC /l 0x809 /i "src\plat"
# ADD RSC /l 0x809 /i "src\plat"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\plat\winextra.c

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

# PROP Intermediate_Dir "PS2_Release"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "PS2 Specific"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=.\src\plat\sky.c

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\plat\skyextra.c

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\src\app\car.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\carAPI.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /Z<none>

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

# ADD CPP /O2

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\carsound.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

# PROP Intermediate_Dir "PS2_Release"
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\control.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\driver.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\events.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\hack.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\init.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\MdtCar.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\parser.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\RwFuncs.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\smoke.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\app\utils.cpp

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

# PROP Intermediate_Dir "PS2__Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\rw_skel\camera.h
# End Source File
# Begin Source File

SOURCE=.\src\app\car.hpp
# End Source File
# Begin Source File

SOURCE=.\src\app\carAPI.hpp
# End Source File
# Begin Source File

SOURCE=.\src\app\CarData.hpp
# End Source File
# Begin Source File

SOURCE=.\src\app\carsound.hpp
# End Source File
# Begin Source File

SOURCE=.\src\app\control.hpp
# End Source File
# Begin Source File

SOURCE=.\src\app\driver.hpp
# End Source File
# Begin Source File

SOURCE=.\src\app\init.hpp
# End Source File
# Begin Source File

SOURCE=.\src\app\MdtCar.hpp
# End Source File
# Begin Source File

SOURCE=..\..\platform_globals\include\MePrecision.h
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\metrics.h
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\mousedat.h
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\padmap.h
# End Source File
# Begin Source File

SOURCE=.\src\app\parser.hpp
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\platform.h
# End Source File
# Begin Source File

SOURCE=.\src\plat\platxtra.h
# End Source File
# Begin Source File

SOURCE=.\PS2_in_VC.h
# End Source File
# Begin Source File

SOURCE=.\src\app\RwFuncs.hpp
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\skeleton.h
# End Source File
# Begin Source File

SOURCE=.\src\app\smoke.hpp
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\terminal.h
# End Source File
# Begin Source File

SOURCE=.\src\app\utils.hpp
# End Source File
# Begin Source File

SOURCE=.\src\rw_skel\vecfont.h
# End Source File
# Begin Source File

SOURCE=.\src\plat\winextra.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\plat\twoE.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\crt0.s

!IF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Debug"

!ELSEIF  "$(CFG)" == "GMonkey_Win_PS2 - Win32 PS2 EE Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\makefile.ps2
# End Source File
# Begin Source File

SOURCE=.\ps2.lk
# End Source File
# End Target
# End Project
