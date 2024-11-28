# Microsoft Developer Studio Project File - Name="MdtKea" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MdtKea - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MdtKea.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MdtKea.mak" CFG="MdtKea - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MdtKea - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MdtKea - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "MdtKea - Win32 Debug Vanilla" (based on "Win32 (x86) Static Library")
!MESSAGE "MdtKea - Win32 Release Vanilla" (based on "Win32 (x86) Static Library")
!MESSAGE "MdtKea - Win32 Check" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MdtKea - Win32 Release"

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
# ADD CPP /nologo /G6 /W3 /GX /Zi /O2 /Oy- /I "../include" /I "../src" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../MdtBcl/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "FORCE_VERSION_USE" /D "_ME_NOPROFILING" /D "_NOTIC" /FAs /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.rel/win32/MdtKea.lib"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

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
# ADD CPP /nologo /G6 /W3 /Gm /GX /ZI /Od /I "../include" /I "../src" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../MdtBcl/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "_MECHECK" /D "IA32" /D "FORCE_VERSION_USE" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/win32/MdtKea.lib"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MdtKea___Win32_Debug_Vanilla"
# PROP BASE Intermediate_Dir "MdtKea___Win32_Debug_Vanilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MdtKea___Win32_Debug_Vanilla"
# PROP Intermediate_Dir "MdtKea___Win32_Debug_Vanilla"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "../include" /I "../src" /I "../../MeGlobals/include" /I "../../MeMemory/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "_MECHECK" /D "IA32" /D "USE_INTEL_COMPILER" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "../src" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../MdtBcl/include" /I "../../MeMemory/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "_MECHECK" /D "IA32" /D "FORCE_VERSION_USE" /D "_BUILD_VANILLA" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib.dbg/win32/MdtKea.lib"
# ADD LIB32 /nologo /out:"../../lib.dbg/win32/MdtKea.lib"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MdtKea___Win32_Release_Vanilla"
# PROP BASE Intermediate_Dir "MdtKea___Win32_Release_Vanilla"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MdtKea___Win32_Release_Vanilla"
# PROP Intermediate_Dir "MdtKea___Win32_Release_Vanilla"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Zd /O2 /Oy- /I "../include" /I "../src" /I "../../MeGlobals/include" /I "../../MeMemory/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "_ME_NOPROFILING" /Fr /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zd /O2 /Oy- /I "../include" /I "../src" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../MdtBcl/include" /I "../../MeMemory/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "_ME_NOPROFILING" /D "_NOTIC" /D "FORCE_VERSION_USE" /D "_BUILD_VANILLA" /Fr /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib.rel/win32/MdtKea.lib"
# ADD LIB32 /nologo /out:"../../lib.rel/win32/MdtKea.lib"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MdtKea___Win32_Check"
# PROP BASE Intermediate_Dir "MdtKea___Win32_Check"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MdtKea___Win32_Check"
# PROP Intermediate_Dir "MdtKea___Win32_Check"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /Zi /O2 /Oy- /I "../include" /I "../src" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../MdtBcl/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "VS6" /D "FORCE_VERSION_USE" /D "_ME_NOPROFILING" /D "_NOTIC" /FAs /FR /YX /FD /c
# ADD CPP /nologo /G6 /MLd /W3 /GX /Zi /O2 /Oy- /I "../include" /I "../src" /I "../../MeGlobals/include" /I "../../Mdt/include" /I "../../MdtBcl/include" /D "_MBCS" /D "_LIB" /D "VS6" /D "FORCE_VERSION_USE" /D "_ME_NOPROFILING" /D "_NOTIC" /D "_MECHECK" /D "MCDCHECK" /D "NDEBUG" /D "WIN32" /FAs /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../lib.rel/win32/MdtKea.lib"
# ADD LIB32 /nologo /out:"../../lib.chk/win32/MdtKea.lib"

!ENDIF 

# Begin Target

# Name "MdtKea - Win32 Release"
# Name "MdtKea - Win32 Debug"
# Name "MdtKea - Win32 Debug Vanilla"
# Name "MdtKea - Win32 Release Vanilla"
# Name "MdtKea - Win32 Check"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ps2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\calcA_ps2.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cf_block_ps2.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\JM_block_ps2.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaCalcConstraintForces_ps2.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaCalcIworldandNonInertialForceandVhmf_ps2.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaCalcJinvMandRHS_ps2.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaCalcResultantForces_ps2.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaDma_ps2.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaIntegrate_ps2.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaMathStuff_ps2.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps244smalldense_ps2.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2smalldense.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2smalldense_micro.dsm

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2smalldense_micro.evsm
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2sparse.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2sparse_micro.dsm

!IF  "$(CFG)" == "MdtKea - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2sparse_micro.evsm
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\carSolver.cpp
# End Source File
# Begin Source File

SOURCE=.\keaCalcAcceleration_vanilla.cpp
# End Source File
# Begin Source File

SOURCE=.\keaCalcConstraintForces_vanilla.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaCalcIworldandNonInertialForceandVhmf_vanilla.cpp
# End Source File
# Begin Source File

SOURCE=.\keaCalcJinvMandRHS_vanilla.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\keaFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\keaLCP_new.cpp
# End Source File
# Begin Source File

SOURCE=.\keaLCPSolver.cpp
# End Source File
# Begin Source File

SOURCE=.\keaMakejlenandbl2body.cpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_tester.cpp
# End Source File
# Begin Source File

SOURCE=.\keaMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\keaPrintBasicTypes.cpp
# End Source File
# Begin Source File

SOURCE=.\keaRbdCore_unified.cpp
# End Source File
# Begin Source File

SOURCE=keaStuff.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadWriteKeaInputToFile.cpp
# End Source File
# End Group
# Begin Group "pc"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\carSolver_pc.cpp
# End Source File
# Begin Source File

SOURCE=.\keaIntegrate_pc.cpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_PcSparse.cpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_PcSparse_vanilla.cpp
# End Source File
# End Group
# Begin Group "pc_opt"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\keaCalcAcceleration_sse.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaCalcConstraintForces_sse.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1
# SUBTRACT BASE CPP /Fr
# SUBTRACT CPP /Fr

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaCalcIworldandNonInertialForceandVhmf_sse.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaCalcJinvMandRHS_sse.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaCheckCPU_sse.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\keaMatrix_PcSparse_SSE.cpp

!IF  "$(CFG)" == "MdtKea - Win32 Release"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug"

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Debug Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Release Vanilla"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MdtKea - Win32 Check"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\calcA.hpp
# End Source File
# Begin Source File

SOURCE=..\include\carsolver.h
# End Source File
# Begin Source File

SOURCE=..\include\carsolver_pc.h
# End Source File
# Begin Source File

SOURCE=..\include\carsolver_ps2.h
# End Source File
# Begin Source File

SOURCE=.\cf_block.hpp
# End Source File
# Begin Source File

SOURCE=.\JM_block.hpp
# End Source File
# Begin Source File

SOURCE=.\keaCalcConstraintForces.hpp
# End Source File
# Begin Source File

SOURCE=.\keaCalcIworldandNonInertialForceandVhmf.h
# End Source File
# Begin Source File

SOURCE=.\keaCalcJinvMandRHS.hpp
# End Source File
# Begin Source File

SOURCE=.\keaCheckCPU_sse.hpp
# End Source File
# Begin Source File

SOURCE=.\keaCTicks.h
# End Source File
# Begin Source File

SOURCE=.\keaDebug.h
# End Source File
# Begin Source File

SOURCE=.\keaEeDefs.hpp
# End Source File
# Begin Source File

SOURCE=.\keaFunctions.hpp
# End Source File
# Begin Source File

SOURCE=.\keaIntegrate.hpp
# End Source File
# Begin Source File

SOURCE=keaInternal.hpp
# End Source File
# Begin Source File

SOURCE=.\keaLCPSolver.hpp
# End Source File
# Begin Source File

SOURCE=.\keaMakejlenandbl2body.hpp
# End Source File
# Begin Source File

SOURCE=.\keaMathStuff.hpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix.hpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_PcSparse.hpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_PcSparse_SSE.hpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_PcSparse_vanilla.hpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps244smalldense.hpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2smalldense.hpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2sparse.hpp
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_tester.hpp
# End Source File
# Begin Source File

SOURCE=.\keaMemory.hpp
# End Source File
# Begin Source File

SOURCE=.\keaPerformanceTimer.hpp
# End Source File
# Begin Source File

SOURCE=.\keaPrintBasicTypes.h
# End Source File
# Begin Source File

SOURCE=.\keaSparseMatrix_ps2.hpp
# End Source File
# Begin Source File

SOURCE=.\KeaSSEi.h
# End Source File
# Begin Source File

SOURCE=keaStuff.hpp
# End Source File
# Begin Source File

SOURCE=..\include\MdtKea.h
# End Source File
# Begin Source File

SOURCE=..\include\MdtKeaProfile.h
# End Source File
# Begin Source File

SOURCE=.\ReadWriteKeaInputToFile.h
# End Source File
# End Group
# End Target
# End Project
