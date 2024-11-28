# Microsoft Developer Studio Project File - Name="McdPrimitivesXb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=McdPrimitivesXb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "McdPrimitivesXb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "McdPrimitivesXb.mak" CFG="McdPrimitivesXb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McdPrimitivesXb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "McdPrimitivesXb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "McdPrimitivesXb - Xbox Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /I "../math" /I "../space" /I "../frame" /I "../utils" /I "../convex" /I "../Qhull" /I "../primitives" /I "../../../Mdt/include" /I "../../../Mcd/include" /I "../../../MeViewer2/include" /I "../../../MdtBcl/include" /I "../../../MdtKea/include" /I "../../../MeGlobals/include" /I "../../../Mst/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:"../../../lib.rel/xbox/McdPrimitives.lib"
# SUBTRACT LIB32 /nologo

!ELSEIF  "$(CFG)" == "McdPrimitivesXb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "McdPrimitivesXb___Xbox_Debug"
# PROP BASE Intermediate_Dir "McdPrimitivesXb___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../include" /I "../math" /I "../space" /I "../frame" /I "../utils" /I "../convex" /I "../Qhull" /I "../primitives" /I "../../../Mdt/include" /I "../../../Mcd/include" /I "../../../MeViewer2/include" /I "../../../MdtBcl/include" /I "../../../MdtKea/include" /I "../../../MeGlobals/include" /I "../../../Mst/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_MECHECK" /D "MCDCHECK" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib.dbg/xbox/McdPrimitives.lib"

!ENDIF 

# Begin Target

# Name "McdPrimitivesXb - Xbox Release"
# Name "McdPrimitivesXb - Xbox Debug"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MovingBoxBoxIntersect.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Polynomial.h
# End Source File
# End Group
# Begin Group "Public Header Files"

# PROP Default_Filter ""
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
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "groupings"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\McdPrimitivesRegisterInteractions.cpp
# End Source File
# Begin Source File

SOURCE=.\McdPrimitivesRegisterTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\McdSphereBoxPlaneRegisterInteractions.cpp
# End Source File
# Begin Source File

SOURCE=.\McdSphereBoxPlaneRegisterTypes.cpp
# End Source File
# End Group
# Begin Group "boxplanesphere"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\IxBoxBox.cpp
# End Source File
# Begin Source File

SOURCE=.\IxBoxPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\IxBoxSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\IxPrimitiveLineSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\IxSpherePlane.cpp
# End Source File
# Begin Source File

SOURCE=.\IxSphereSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\McdBox.cpp
# End Source File
# Begin Source File

SOURCE=.\McdPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\McdSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\MovingBoxBoxIntersect.cpp
# End Source File
# End Group
# Begin Group "cylinder"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\IxBoxCylinder.cpp
# End Source File
# Begin Source File

SOURCE=.\IxCylinderCylinder.cpp
# End Source File
# Begin Source File

SOURCE=.\IxCylinderLineSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\IxCylinderPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\IxCylinderSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\McdCylinder.cpp
# End Source File
# End Group
# Begin Group "sphyl"

# PROP Default_Filter ".c .cpp"
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

# PROP Default_Filter ".c .cpp"
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
# End Target
# End Project
