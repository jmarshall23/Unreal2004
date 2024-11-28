# Microsoft Developer Studio Project File - Name="Mcd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Mcd - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mcd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mcd.mak" CFG="Mcd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Mcd - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Mcd - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Mcd - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /G6 /O2 /Oy- /I "./" /I "../cx" /I "../primitives" /I "../math" /I "../dtbridge" /I "../rwbsp" /I "../space" /I "../util" /I "../../include" /I "../../../MeGlobals/include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex"  /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.rel/win32/Mcd.lib"

!ELSEIF  "$(CFG)" == "Mcd - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /G6 /ZI /Od /I "./" /I "../cx" /I "../primitives" /I "../math" /I "../dtbridge" /I "../rwbsp" /I "../space" /I "../util" /I "../../include" /I "../../../MeGlobals/include" /I "../../../MdtKea/include" /I "../../../Mdt/include" /I "../../src/convex"  /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "LCE_USE_STATIC_LIB" /D "_MECHECK" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.dbg/win32/Mcd.lib"

!ENDIF 

# Begin Target

# Name "Mcd - Win32 Release"
# Name "Mcd - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "frame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\McdFrame.cpp
# End Source File
# End Group
# Begin Group "primitives"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\primitives\CxBox.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\CxBox.h
# End Source File
# Begin Source File

SOURCE=..\primitives\CxCone.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\CxCone.h
# End Source File
# Begin Source File

SOURCE=..\primitives\CxCylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\CxCylinder.h
# End Source File
# Begin Source File

SOURCE=..\primitives\CxPlane.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\CxPlane.h
# End Source File
# Begin Source File

SOURCE=..\primitives\CxSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\CxSphere.h
# End Source File
# Begin Source File

SOURCE=..\primitives\CxTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\CxTriangle.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxBoxBox.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxBoxBox.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxBoxCylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxBoxCylinder.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxBoxPlane.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxBoxPlane.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxBoxSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxBoxSphere.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConeBox.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConeBox.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConeCone.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConeCone.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConeCylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConeCylinder.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConeLineSegment.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConeLineSegment.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConePlane.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConePlane.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConeSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxConeSphere.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderCylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderCylinder.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderLineSegment.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderLineSegment.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderPlane.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderPlane.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxCylinderSphere.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxPrimitiveLineSegment.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxPrimitiveLineSegment.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxSpaceLineSegment.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxSpaceLineSegment.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxSpherePlane.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxSpherePlane.h
# End Source File
# Begin Source File

SOURCE=..\primitives\IxSphereSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\IxSphereSphere.h
# End Source File
# Begin Source File

SOURCE=..\primitives\McdBox.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdCone.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdCylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdPlane.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdPrimitives.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdPrimitivesRegisterInteractions.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdPrimitivesRegisterTypes.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdSphere.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdSphereBoxPlaneRegisterInteractions.cpp
# End Source File
# Begin Source File

SOURCE=..\primitives\McdSphereBoxPlaneRegisterTypes.cpp
# End Source File
# End Group
# Begin Group "terrain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\terrain\CxRGHeightField.cpp
# End Source File
# Begin Source File

SOURCE=..\terrain\CxRGHeightField.h
# End Source File
# Begin Source File

SOURCE=..\terrain\IxBoxRGHeightField.cpp
# End Source File
# Begin Source File

SOURCE=..\terrain\IxBoxRGHeightField.h
# End Source File
# Begin Source File

SOURCE=..\terrain\IxCylinderRGHeightField.cpp
# End Source File
# Begin Source File

SOURCE=..\terrain\IxCylinderRGHeightField.h
# End Source File
# Begin Source File

SOURCE=..\terrain\IxSphereRGHeightField.cpp
# End Source File
# Begin Source File

SOURCE=..\terrain\IxSphereRGHeightField.h
# End Source File
# Begin Source File

SOURCE=..\terrain\McdRGHeightField.cpp
# End Source File
# End Group
# Begin Group "cx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\cx\McdCheck.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxCoreErrorList.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxDispatcher.cpp
# End Source File
# Begin Source File

SOURCE=..\cx\CxDispatcher.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxDispatcherLS.cpp
# End Source File
# Begin Source File

SOURCE=..\cx\CxDispatcherLS.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxFramework.cpp
# End Source File
# Begin Source File

SOURCE=..\cx\CxFramework.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxGeometry.cpp
# End Source File
# Begin Source File

SOURCE=..\cx\CxGeometry.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxMemory.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxMessage.cpp
# End Source File
# Begin Source File

SOURCE=..\cx\CxMessage.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxObject.cpp
# End Source File
# Begin Source File

SOURCE=..\cx\CxObject.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxPair.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxPairHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\cx\CxPairHandler.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxTypes.h
# End Source File
# End Group
# Begin Group "space"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\space\CxSmallSort.cpp
# End Source File
# Begin Source File

SOURCE=..\space\CxSmallSort.h
# End Source File
# Begin Source File

SOURCE=..\cx\CxSpace.h
# End Source File
# End Group
# Begin Group "math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\math\EEMath.inl
# End Source File
# Begin Source File

SOURCE=..\math\lsReal.h
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

SOURCE=..\math\mesffnmin.cpp
# End Source File
# Begin Source File

SOURCE=..\math\mesffnmin.h
# End Source File
# Begin Source File

SOURCE=..\math\triutils.cpp
# End Source File
# Begin Source File

SOURCE=..\math\TriUtils.h
# End Source File
# Begin Source File

SOURCE=..\math\vectormath.h
# End Source File
# End Group
# Begin Group "convex"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\convex\GjkSup.cpp
# End Source File
# Begin Source File

SOURCE=..\convex\GjkSup.h
# End Source File
# Begin Source File

SOURCE=..\convex\GjkTables.cpp
# End Source File
# End Group
# Begin Group "util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\util\CxTriangleNE.h
# End Source File
# Begin Source File

SOURCE=..\util\lsConfig.h
# End Source File
# Begin Source File

SOURCE=..\util\mesfarray.h
# End Source File
# Begin Source File

SOURCE=..\util\mesfcore.h
# End Source File
# Begin Source File

SOURCE=..\util\mesfhashtable.h
# End Source File
# Begin Source File

SOURCE=..\util\mesflist.h
# End Source File
# End Group
# End Group
# Begin Group "Public Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\McdCone.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdCylinder.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdFrame.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdInit.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdPrimitives.h
# End Source File
# Begin Source File

SOURCE=..\..\include\McdRGHeightField.h
# End Source File
# End Group
# End Target
# End Project
