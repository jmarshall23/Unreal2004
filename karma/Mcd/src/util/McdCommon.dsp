# Microsoft Developer Studio Project File - Name="McdCommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=McdCommon - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "McdCommon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "McdCommon.mak" CFG="McdCommon - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McdCommon - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "McdCommon - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "McdCommon - Win32 Check" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "McdCommon - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "McdCommon___Win32_Release"
# PROP BASE Intermediate_Dir "McdCommon___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /Oy- /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /Zi /O2 /Oy- /Ob2 /I "./" /I "../primitives" /I "../math" /I "../cx" /I "../rwbsp" /I "../space" /I "../util" /I "../frame" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../../../MeGlobals/include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /D "_MECHECK" /D "MCDCHECK" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.rel/win32/McdCommon.lib"

!ELSEIF  "$(CFG)" == "McdCommon - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "McdCommon___Win32_Debug"
# PROP BASE Intermediate_Dir "McdCommon___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /W3 /Gm /GX /ZI /Od /I "./" /I "../primitives" /I "../math" /I "../rwbsp" /I "../space" /I "../frame" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../cx" /I "../../../MeGlobals/include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /D "_MECHECK" /D "MCDCHECK" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.dbg/win32/McdCommon.lib"

!ELSEIF  "$(CFG)" == "McdCommon - Win32 Check"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "McdCommon___Win32_Check"
# PROP BASE Intermediate_Dir "McdCommon___Win32_Check"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "McdCommon___Win32_Check"
# PROP Intermediate_Dir "McdCommon___Win32_Check"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /Oy- /Ob2 /I "./" /I "../primitives" /I "../math" /I "../cx" /I "../rwbsp" /I "../space" /I "../util" /I "../frame" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../../../MeGlobals/include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /MLd /W3 /GX /Zi /O2 /Oy- /Ob2 /I "./" /I "../primitives" /I "../math" /I "../cx" /I "../rwbsp" /I "../space" /I "../util" /I "../frame" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../../../MeGlobals/include" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /D "_MECHECK" /D "MCDCHECK" /D "NDEBUG" /D "WIN32" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib.rel/win32/McdCommon.lib"
# ADD LIB32 /nologo /out:"../../../lib.chk/win32/McdCommon.lib"

!ENDIF 

# Begin Target

# Name "McdCommon - Win32 Release"
# Name "McdCommon - Win32 Debug"
# Name "McdCommon - Win32 Check"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "space"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\space\CxSmallSort.cpp
# End Source File
# Begin Source File

SOURCE=..\space\McdSpace.cpp
# End Source File
# End Group
# Begin Group "math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\math\GeomUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\math\McdVanillaCore.cpp
# End Source File
# Begin Source File

SOURCE=..\math\mesffnmin.cpp
# End Source File
# End Group
# Begin Group "contacts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\McdContact.cpp
# End Source File
# End Group
# End Group
# Begin Group "Public Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\..\include\McdContact.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdSpace.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\space\CxSmallSort.h
# End Source File
# Begin Source File

SOURCE=..\space\CxSmallSortUtils.h
# End Source File
# Begin Source File

SOURCE=..\space\CxSpace.h
# End Source File
# Begin Source File

SOURCE=.\CxTriangleNE.h
# End Source File
# Begin Source File

SOURCE=..\math\GeomUtils.h
# End Source File
# Begin Source File

SOURCE=..\math\lsTransform.h
# End Source File
# Begin Source File

SOURCE=..\math\lsVec3.h
# End Source File
# Begin Source File

SOURCE=..\math\lsVec4.h
# End Source File
# Begin Source File

SOURCE=..\math\mesffnmin.h
# End Source File
# Begin Source File

SOURCE=..\math\vectormath.h
# End Source File
# End Group
# End Target
# End Project
