# Microsoft Developer Studio Project File - Name="GreaseMonkeyXb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=GreaseMonkeyXb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GreaseMonkeyXb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GreaseMonkeyXb.mak" CFG="GreaseMonkeyXb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GreaseMonkeyXb - Xbox Release" (based on "Xbox Application")
!MESSAGE "GreaseMonkeyXb - Xbox Debug" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "GreaseMonkeyXb - Xbox Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /W3 /GX /O2 /I "src\RW\shared\skel" /I "src\RW\shared\skel\win" /I "src\RW\shared\democom" /I "src\karma\app" /I "src\karma\plat" /I "..\..\MeGlobals\include" /I "..\..\Mst\include" /I "..\..\MdtKea\include" /I "..\..\Mdt\include" /I "..\..\Mcd\include" /I "..\..\MdtBcl\include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "PERFORMANCE_METRICS" /FR /YX /FD /G6 /Zvc6 /c
# SUBTRACT CPP /nologo
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /o"Release/GreaseMonkey.bsc"
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6 /OPT:REF
# ADD LINK32 Mdt.lib MdtKea.lib McdFrame.lib McdCommon.lib McdPrimitives.lib Mst.lib MeGlobals.lib MdtBcl.lib rt2d.lib rpworld.lib rtbmp.lib rwcore.lib rtcharse.lib rtslerp.lib rplogo.lib rtimport.lib rtworld.lib rtpng.lib rpcollis.lib rtintsec.lib xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /machine:I386 /nodefaultlib:"libc" /out:"./GreaseMonkey.exe" /libpath:"..\..\lib.rel\xbox" /libpath:"..\..\lib.rel\xbox_single_libc" /subsystem:xbox /fixed:no /debugtype:vc6 /OPT:REF
# SUBTRACT LINK32 /nologo
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000
# ADD XBE /nologo /testname:"GreaseMonkey" /stack:0x40000 /out:"Release/GreaseMonkey.xbe"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO

!ELSEIF  "$(CFG)" == "GreaseMonkeyXb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GreaseMonkeyXb___Xbox_Debug"
# PROP BASE Intermediate_Dir "GreaseMonkeyXb___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "src\RW\shared\skel" /I "src\RW\shared\skel\win" /I "src\RW\shared\democom" /I "src\karma\app" /I "src\karma\plat" /I "..\..\MeGlobals\include" /I "..\..\Mst\include" /I "..\..\MdtKea\include" /I "..\..\Mdt\include" /I "..\..\Mcd\include" /I "..\..\MdtBcl\include" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "PERFORMANCE_METRICS" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 Mdt.lib MdtKea.lib McdFrame.lib McdCommon.lib McdPrimitives.lib Mst.lib MeGlobals.lib MdtBcl.lib rt2d.lib rpworld.lib rtbmp.lib rwcore.lib rtcharse.lib rtslerp.lib rplogo.lib rtimport.lib rtworld.lib rtpng.lib rpcollis.lib rtintsec.lib xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /incremental:no /pdb:"Debug/GreaseMonkey.pdb" /debug /machine:I386 /nodefaultlib:"libc" /libpath:"..\..\lib.chk\xbox" /libpath:"..\..\lib.chk\xbox_single_libc" /subsystem:xbox /fixed:no /debugtype:vc6
# SUBTRACT LINK32 /nologo /verbose /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /stack:0x40000 /debug
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO

!ENDIF 

# Begin Target

# Name "GreaseMonkeyXb - Xbox Release"
# Name "GreaseMonkeyXb - Xbox Debug"
# Begin Group "Html"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Html\cardemo.html
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
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
# Begin Group "Header Files"

# PROP Default_Filter ""
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
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "RW_Skeleton_App"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\RW\shared\democom\camera.c
# End Source File
# Begin Source File

SOURCE=.\src\RW\shared\skel\skeleton.c
# End Source File
# End Group
# Begin Group "Xbox Specific"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\RW\shared\skel\xbox\xbox.c
# End Source File
# Begin Source File

SOURCE=.\src\Karma\plat\xboxextra.c
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
# PROP Exclude_From_Build 1
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
# Begin Source File

SOURCE=.\Scripts\grmonkey.cfg
# End Source File
# End Target
# End Project
