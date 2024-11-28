# Microsoft Developer Studio Project File - Name="McdPrimitives" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=McdPrimitives - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "McdPrimitives.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "McdPrimitives.mak" CFG="McdPrimitives - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McdPrimitives - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "McdPrimitives - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "McdPrimitives - Win32 Check" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "McdPrimitives - Win32 Release"

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
# ADD CPP /nologo /G6 /W3 /GX /Zi /O2 /Oy- /Ob2 /I "./" /I "../frame" /I "../primitives" /I "../math" /I "../dtbridge" /I "../rwbsp" /I "../space" /I "../util" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../cx" /I "../../../MeGlobals/include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /D "_MECHECK" /D "MCDCHECK" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.rel/win32/McdPrimitives.lib"

!ELSEIF  "$(CFG)" == "McdPrimitives - Win32 Debug"

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
# ADD CPP /nologo /G6 /W3 /Gm /GX /ZI /Od /I "./" /I "../frame" /I "../../include/c_includes" /I "../primitives" /I "../math" /I "../rwbsp" /I "../space" /I "../util" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../cx" /I "../../../MeGlobals/include" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /D "_MECHECK" /D "MCDCHECK" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.dbg/win32/McdPrimitives.lib"

!ELSEIF  "$(CFG)" == "McdPrimitives - Win32 Check"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "McdPrimitives___Win32_Check"
# PROP BASE Intermediate_Dir "McdPrimitives___Win32_Check"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "McdPrimitives___Win32_Check"
# PROP Intermediate_Dir "McdPrimitives___Win32_Check"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /Oy- /Ob2 /I "./" /I "../frame" /I "../primitives" /I "../math" /I "../dtbridge" /I "../rwbsp" /I "../space" /I "../util" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../cx" /I "../../../MeGlobals/include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /MLd /W3 /GX /Zi /O2 /Oy- /Ob2 /I "./" /I "../frame" /I "../primitives" /I "../math" /I "../dtbridge" /I "../rwbsp" /I "../space" /I "../util" /I "../../include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex" /I "../cx" /I "../../../MeGlobals/include" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /D "_MECHECK" /D "MCDCHECK" /D "NDEBUG" /D "WIN32" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../lib.rel/win32/McdPrimitives.lib"
# ADD LIB32 /nologo /out:"../../../lib.chk/win32/McdPrimitives.lib"

!ENDIF 

# Begin Target

# Name "McdPrimitives - Win32 Release"
# Name "McdPrimitives - Win32 Debug"
# Name "McdPrimitives - Win32 Check"
# Begin Group "Source Files"

# PROP Default_Filter ".c .cpp"
# Begin Group "groupings"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=..\primitives\McdPrimitivesRegisterInteractions.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdPrimitivesRegisterTypes.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdSphereBoxPlaneRegisterInteractions.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdSphereBoxPlaneRegisterTypes.cpp
# End Source File
# End Group
# Begin Group "BoxPlaneSphere"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=..\primitives\IxBoxBox.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxBoxPlane.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxBoxSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\IxPrimitiveLineSegment.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxSpherePlane.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxSphereSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdBox.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdPlane.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\MovingBoxBoxIntersect.cpp
# End Source File
# End Group
# Begin Group "cylinder"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\primitives\IxBoxCylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderCylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderLineSegment.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderPlane.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdCylinder.cpp
# End Source File
# End Group
# Begin Group "sphyl"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\IxSphylPrimitives.cpp
# End Source File
# Begin Source File

SOURCE=.\McdSphyl.cpp
# End Source File
# Begin Source File

SOURCE=.\Polynomial.cpp
# End Source File
# End Group
# Begin Group "trilist"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\IxBoxTriList.cpp
# End Source File
# Begin Source File

SOURCE=.\IxCylinderTriList.cpp
# End Source File
# Begin Source File

SOURCE=.\IxSphereTriList.cpp
# End Source File
# Begin Source File

SOURCE=.\McdTriangleList.cpp
# End Source File
# End Group
# End Group
# Begin Group "Public Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\McdBox.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdCylinder.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdPlane.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdPrimitives.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdSphere.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdSphyl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdTriangleList.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MovingBoxBoxIntersect.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Polynomial.h
# End Source File
# End Group
# End Target
# End Project
