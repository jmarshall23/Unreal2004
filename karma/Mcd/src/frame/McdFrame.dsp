# Microsoft Developer Studio Project File - Name="McdFrame" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=McdFrame - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "McdFrame.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "McdFrame.mak" CFG="McdFrame - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McdFrame - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "McdFrame - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "McdFrame - Win32 Check" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "McdFrame - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "McdFrame___Win32_Release"
# PROP BASE Intermediate_Dir "McdFrame___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /Oy- /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /Zi /O2 /Oy- /Ob2 /I "./" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../../MeGlobals/include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /D "_MECHECK" /D "MCDCHECK" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.rel/win32/McdFrame.lib"

!ELSEIF  "$(CFG)" == "McdFrame - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "McdFrame___Win32_Debug"
# PROP BASE Intermediate_Dir "McdFrame___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /W3 /Gm /GX /ZI /Od /I "./" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../../MeGlobals/include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /D "_MECHECK" /D "MCDCHECK" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.dbg/win32/McdFrame.lib"

!ELSEIF  "$(CFG)" == "McdFrame - Win32 Check"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "McdFrame___Win32_Check"
# PROP BASE Intermediate_Dir "McdFrame___Win32_Check"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "McdFrame___Win32_Check"
# PROP Intermediate_Dir "McdFrame___Win32_Check"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /Oy- /Ob2 /I "./" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../../MeGlobals/include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /MLd /W3 /GX /Zi /O2 /Oy- /Ob2 /I "./" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../../MeGlobals/include" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /D "_MECHECK" /D "MCDCHECK" /D "NDEBUG" /D "WIN32" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib.rel/win32/McdFrame.lib"
# ADD LIB32 /nologo /out:"../../../lib.chk/win32/McdFrame.lib"

!ENDIF 

# Begin Target

# Name "McdFrame - Win32 Release"
# Name "McdFrame - Win32 Debug"
# Name "McdFrame - Win32 Check"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\McdAggregate.cpp
# End Source File
# Begin Source File

SOURCE=.\McdBatch.cpp
# End Source File
# Begin Source File

SOURCE=.\McdFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\McdGeometry.cpp
# End Source File
# Begin Source File

SOURCE=.\McdGeometryInstance.cpp
# End Source File
# Begin Source File

SOURCE=.\McdInteractions.cpp
# End Source File
# Begin Source File

SOURCE=.\McdMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\McdModel.cpp
# End Source File
# Begin Source File

SOURCE=.\McdModelPair.cpp
# End Source File
# Begin Source File

SOURCE=.\McdModelPairContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\McdModelPairManager.cpp
# End Source File
# Begin Source File

SOURCE=.\McdNull.cpp
# End Source File
# Begin Source File

SOURCE=.\McduDebugDraw.cpp
# End Source File
# End Group
# Begin Group "Public Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\..\include\Mcd.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdAggregate.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdBatch.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdContact.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdCTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdCullingTable.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdGeometry.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdGeometryInstance.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdGeometryTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdInteractions.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdInteractionTable.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdIntersectResult.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdModel.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdModelPair.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdModelPairContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdModelPairManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdNull.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdSpace.h
# End Source File
# End Group
# Begin Group "Utility Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\..\include\McduDebugDraw.h
# End Source File
# End Group
# End Target
# End Project
