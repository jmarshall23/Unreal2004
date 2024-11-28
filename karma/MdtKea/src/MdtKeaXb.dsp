# Microsoft Developer Studio Project File - Name="MdtKeaXb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=MdtKeaXb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MdtKeaXb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MdtKeaXb.mak" CFG="MdtKeaXb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MdtKeaXb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "MdtKeaXb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "MdtKeaXb - Xbox Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /I "../src" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MeViewer2/include" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mst/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /out:"../../lib.rel/xbox/MdtKea.lib"
# SUBTRACT LIB32 /nologo

!ELSEIF  "$(CFG)" == "MdtKeaXb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MdtKeaXb___Xbox_Debug"
# PROP BASE Intermediate_Dir "MdtKeaXb___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../include" /I "../src" /I "../../Mdt/include" /I "../../Mcd/include" /I "../../MeViewer2/include" /I "../../MdtBcl/include" /I "../../MdtKea/include" /I "../../MeGlobals/include" /I "../../Mst/include" /I "../../MeAssetDB/include" /I "../../MeAssetDBXMLIO/include" /I "../../MeAssetFactory/include" /I "../../MeXML/include" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_MECHECK" /D "MCDCHECK" /D "_ME_API_SINGLE" /YX /FD /G6 /Zvc6 /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib.dbg/xbox/MdtKea.lib"

!ENDIF 

# Begin Target

# Name "MdtKeaXb - Xbox Release"
# Name "MdtKeaXb - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Group "ps2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\calcA_ps2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\cf_block_ps2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\JM_block_ps2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaCalcConstraintForces_ps2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaCalcIworldandNonInertialForceandVhmf_ps2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaCalcJinvMandRHS_ps2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaCalcResultantForces_ps2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaDma_ps2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaIntegrate_ps2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaMathStuff_ps2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps244smalldense_ps2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2smalldense.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2smalldense_micro.dsm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2smalldense_micro.evsm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2sparse.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2sparse_micro.dsm
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_ps2sparse_micro.evsm
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "xbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\keaCheckCPU_xbox.cpp
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\keaCalcAcceleration_vanilla.cpp
# End Source File
# Begin Source File

SOURCE=.\keaCalcConstraintForces_vanilla.cpp
# End Source File
# Begin Source File

SOURCE=.\keaCalcIworldandNonInertialForceandVhmf_vanilla.cpp
# End Source File
# Begin Source File

SOURCE=.\keaCalcJinvMandRHS_vanilla.cpp
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

SOURCE=.\keaStuff.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadWriteKeaInputToFile.cpp
# End Source File
# End Group
# Begin Group "pc"

# PROP Default_Filter ".c .cpp"
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

# PROP Default_Filter ".c .cpp"
# Begin Source File

SOURCE=.\keaCalcAcceleration_sse.cpp
# End Source File
# Begin Source File

SOURCE=.\keaCalcConstraintForces_sse.cpp
# End Source File
# Begin Source File

SOURCE=.\keaCalcIworldandNonInertialForceandVhmf_sse.cpp
# End Source File
# Begin Source File

SOURCE=.\keaCalcJinvMandRHS_sse.cpp
# End Source File
# Begin Source File

SOURCE=.\keaCheckCPU_sse.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\keaMatrix_PcSparse_SSE.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
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

SOURCE=.\keaInternal.hpp
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

SOURCE=.\keaStuff.hpp
# End Source File
# Begin Source File

SOURCE=..\include\MdtKea.h
# End Source File
# Begin Source File

SOURCE=.\ReadWriteKeaInputToFile.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# End Group
# End Target
# End Project
