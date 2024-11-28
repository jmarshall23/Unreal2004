# Microsoft Developer Studio Project File - Name="Mdt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Mdt - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mdt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mdt.mak" CFG="Mdt - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Mdt - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Mdt - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Mdt - Win32 Check" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Mdt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /Oy- /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /Zi /O2 /Oy- /I "../include" /I "../src" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /FAs /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.rel/win32/Mdt.lib"

!ELSEIF  "$(CFG)" == "Mdt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /W3 /Gm /GX /ZI /Od /I "../include" /I "../src" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "_MECHECK" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/win32/Mdt.lib"

!ELSEIF  "$(CFG)" == "Mdt - Win32 Check"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Mdt___Win32_Check"
# PROP BASE Intermediate_Dir "Mdt___Win32_Check"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Mdt___Win32_Check"
# PROP Intermediate_Dir "Mdt___Win32_Check"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /Oy- /I "../include" /I "../src" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /FAs /FR /YX /FD /c
# ADD CPP /nologo /G6 /MLd /W3 /GX /Zi /O2 /Oy- /I "../include" /I "../src" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /D "_MBCS" /D "_LIB" /D "VS6" /D "_MECHECK" /D "MCDCHECK" /D "NDEBUG" /D "WIN32" /FAs /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib.rel/win32/Mdt.lib"
# ADD LIB32 /nologo /out:"../../lib.chk/win32/Mdt.lib"

!ENDIF 

# Begin Target

# Name "Mdt - Win32 Release"
# Name "Mdt - Win32 Debug"
# Name "Mdt - Win32 Check"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MdtAngular3.c
# End Source File
# Begin Source File

SOURCE=.\MdtBody.c
# End Source File
# Begin Source File

SOURCE=.\MdtBSJoint.c
# End Source File
# Begin Source File

SOURCE=.\MdtCarWheel.c
# End Source File
# Begin Source File

SOURCE=.\MdtConeLimit.c
# End Source File
# Begin Source File

SOURCE=.\MdtConstraint.c
# End Source File
# Begin Source File

SOURCE=.\MdtContact.c
# End Source File
# Begin Source File

SOURCE=.\MdtContactGroup.c
# End Source File
# Begin Source File

SOURCE=.\MdtContactParams.c
# End Source File
# Begin Source File

SOURCE=.\MdtFixedPath.c
# End Source File
# Begin Source File

SOURCE=.\MdtHinge.c
# End Source File
# Begin Source File

SOURCE=.\MdtLimit.c
# End Source File
# Begin Source File

SOURCE=.\MdtLinear1.c
# End Source File
# Begin Source File

SOURCE=.\MdtLinear2.c
# End Source File
# Begin Source File

SOURCE=.\MdtLOD.c
# End Source File
# Begin Source File

SOURCE=.\MdtMainLoop.c
# End Source File
# Begin Source File

SOURCE=.\MdtPartition.c
# End Source File
# Begin Source File

SOURCE=.\MdtPrismatic.c
# End Source File
# Begin Source File

SOURCE=.\MdtRPROJoint.c
# End Source File
# Begin Source File

SOURCE=.\MdtSkeletal.c
# End Source File
# Begin Source File

SOURCE=.\MdtSpring.c
# End Source File
# Begin Source File

SOURCE=.\MdtSpring6.c
# End Source File
# Begin Source File

SOURCE=.\MdtUniversal.c
# End Source File
# Begin Source File

SOURCE=.\MdtUserConstraint.c
# End Source File
# Begin Source File

SOURCE=.\MdtUtils.c
# End Source File
# Begin Source File

SOURCE=.\MdtWorld.c
# End Source File
# End Group
# Begin Group "Internal headers"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MdtUtils.h
# End Source File
# End Group
# Begin Group "public headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\Mdt.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtAlignment.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtAngular3.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtBody.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtBSJoint.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtCarWheel.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtCheckMacros.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtConeLimit.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtConstraint.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtContact.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtContactGroup.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtContactParams.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtDefaults.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtFixedPath.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtHinge.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtLimit.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtLinear1.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtLinear2.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtMainLoop.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtPartition.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtPrismatic.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtRPROJoint.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtSkeletal.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtSpring.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtSpring6.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtTypes.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtUniversal.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtUserConstraint.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtUtilities.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtWorld.h
# End Source File
# End Group
# End Target
# End Project
