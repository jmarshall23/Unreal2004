# Microsoft Developer Studio Project File - Name="XBoxLaunch" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=XBoxLaunch - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XboxLaunch.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XboxLaunch.mak" CFG="XBoxLaunch - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XBoxLaunch - Xbox Release" (based on "Xbox Application")
!MESSAGE "XBoxLaunch - Xbox Debug" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "XBoxLaunch"
# PROP Scc_LocalPath ".."
CPP=cl.exe

!IF  "$(CFG)" == "XBoxLaunch - Xbox Release"

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
# ADD BASE CPP /nologo /W4 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /Zp4 /W4 /GX /Zi /O2 /I "..\..\Core\Inc" /I "..\..\Engine\Inc" /I "..\..\IpDrv\Inc" /I "..\..\XBoxAudio\Inc" /I "..\..\D3DDrv\Src" /I "..\..\Fire\Inc" /I "..\..\XBoxDrv\Inc" /I "..\..\metoolkit\include" /I "..\..\Engine\Src" /I "..\..\XInterface\Inc" /I "..\..\XGame\Inc" /D "NDEBUG" /D DO_CHECK_SLOW=0 /D "WIN32" /D "_XBOX" /D "UNICODE" /D "_UNICODE" /D CORE_API= /D ENGINE_API= /D RENDER_API= /D XBOXAUDIO_API= /D IPDRV_API= /D XINTERFACE_API= /D DECA_API= /D XGAME_API= /D "__STATIC_LINK" /D DO_GUARD=1 /D DO_GUARD_SLOW=0 /D DO_CHECK=1 /D DO_STAT=1 /D DO_STAT_SLOW=0 /D DO_CLOCK=1 /D DO_CLOCK_SLOW=0 /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 xapilib.lib d3d8.lib d3dx8.lib xkbd.lib xgraphics.lib dsound.lib xnet.lib xboxkrnl.lib Mdt.lib MdtKea.lib MdtBcl.lib Mst.lib McdFrame.lib McdCommon.lib McdPrimitives.lib McdConvex.lib McdConvexCreateHull.lib MeGlobals.lib xvoice.lib /nologo /map /debug /machine:I386 /libpath:"..\..\metoolkit\lib.rel\win32" /libpath:"..\..\metoolkit\src\components\lib.dbg\win32_single_msvcrt" /libpath:"..\..\metoolkit\lib.dbg\win32_single_msvcrt" /libpath:"..\..\metoolkit\lib.chk\win32_single_msvcrt" /subsystem:xbox /fixed:no /tmp /mapinfo:lines
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000
# ADD XBE /nologo /testid:"0xFFED30C9" /testname:"Unreal Championship" /stack:0x100000 /out:"../../System/UC.xbe"
# SUBTRACT XBE /limitmem
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO

!ELSEIF  "$(CFG)" == "XBoxLaunch - Xbox Debug"

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
# ADD BASE CPP /nologo /W4 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Zvc6 /c
# ADD CPP /nologo /Zp4 /W4 /Gm /GX /Zi /Od /I "..\..\Core\Inc" /I "..\..\Engine\Inc" /I "..\..\IpDrv\Inc" /I "..\..\XBoxAudio\Inc" /I "..\..\D3DDrv\Src" /I "..\..\Fire\Inc" /I "..\..\XBoxDrv\Inc" /I "..\..\metoolkit\include" /I "..\..\Engine\Src" /I "..\..\XInterface\Inc" /I "..\..\XGame\Inc" /D "_DEBUG" /D DO_CHECK_SLOW=1 /D "WIN32" /D "_XBOX" /D "UNICODE" /D "_UNICODE" /D CORE_API= /D ENGINE_API= /D RENDER_API= /D XBOXAUDIO_API= /D IPDRV_API= /D XINTERFACE_API= /D DECA_API= /D XGAME_API= /D "__STATIC_LINK" /D DO_GUARD=1 /D DO_GUARD_SLOW=0 /D DO_CHECK=1 /D DO_STAT=1 /D DO_STAT_SLOW=0 /D DO_CLOCK=1 /D DO_CLOCK_SLOW=0 /FR /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox /fixed:no /debugtype:vc6
# ADD LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xkbdd.lib xgraphicsd.lib dsoundd.lib xnetd.lib MeAssetFactory.lib MeAssetDB.lib MeXML.lib xboxkrnl.lib Mdt.lib MdtKea.lib MdtBcl.lib Mst.lib McdFrame.lib McdCommon.lib McdPrimitives.lib McdConvex.lib McdConvexCreateHull.lib MeGlobals.lib xvoiced.lib /nologo /incremental:no /debug /machine:I386 /libpath:"..\..\metoolkit_dummy\lib.dbg\win32" /libpath:"..\..\metoolkit\src\components\lib.dbg\win32_single_msvcrt" /libpath:"..\..\metoolkit\lib.dbg\win32_single_msvcrt" /subsystem:xbox /fixed:no /tmp
# SUBTRACT LINK32 /pdb:none
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /testid:"0xFFED30C9" /testname:"Unreal Championship" /stack:0x100000 /debug /out:"../../System/UC.xbe" /testratings:0xFFFFFFFF
# SUBTRACT XBE /limitmem
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO

!ENDIF 

# Begin Target

# Name "XBoxLaunch - Xbox Release"
# Name "XBoxLaunch - Xbox Debug"
# Begin Group "XBoxDrv"

# PROP Default_Filter ""
# Begin Group "XBoxDrv\Inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\XboxDrv\Inc\XboxDrv.h
# End Source File
# End Group
# Begin Group "XBoxDrv\Src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\XboxDrv\Src\XboxClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XboxDrv\Src\XboxDrv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XboxDrv\Src\XboxViewport.cpp
# End Source File
# End Group
# Begin Group "E3Code"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\XboxDrv\E3Code\E3USERIN.INI
# End Source File
# Begin Source File

SOURCE=..\..\XboxDrv\E3Code\specuc.txt
# End Source File
# Begin Source File

SOURCE=..\..\XboxDrv\E3Code\xe3uc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XboxDrv\E3Code\xe3uc.h
# End Source File
# End Group
# End Group
# Begin Group "XBoxLaunch"

# PROP Default_Filter ""
# Begin Group "XBoxLaunch\Inc"

# PROP Default_Filter ""
# End Group
# Begin Group "XBoxLaunch\Src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Launch.cpp
# End Source File
# End Group
# Begin Group "Fire"

# PROP Default_Filter ""
# Begin Group "Fire\Inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Fire\Inc\FireClasses.h
# End Source File
# Begin Source File

SOURCE=..\..\Fire\Inc\UnFireNative.h
# End Source File
# Begin Source File

SOURCE=..\..\Fire\Inc\UnFractal.h
# End Source File
# End Group
# Begin Group "Fire\Src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Fire\Src\UnFractal.cpp
# End Source File
# End Group
# End Group
# End Group
# Begin Group "XBoxAudio"

# PROP Default_Filter ""
# Begin Group "XBoxAudio\Inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\XboxAudio\Inc\XboxAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\XboxAudio\Inc\XboxAudioSubsystem.h
# End Source File
# Begin Source File

SOURCE=..\..\XboxAudio\Inc\XboxAudioVoice.h
# End Source File
# End Group
# Begin Group "XBoxAudio\Src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\XboxAudio\Src\XboxAudio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XboxAudio\Src\XboxAudioPrivate.h
# End Source File
# Begin Source File

SOURCE=..\..\XboxAudio\Src\XboxAudioSubsystem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XboxAudio\Src\XboxAudioVoice.cpp
# End Source File
# End Group
# End Group
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Group "Core\Inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Core\Inc\afxres.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FCodec.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FFeedbackContextWindows.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FFileManagerAnsi.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FFileManagerLinux.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FFileManagerPSX2.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FFileManagerWindows.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FMallocAnsi.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FMallocWindows.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FOutputDeviceFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FOutputDeviceNull.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FOutputDeviceStdout.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FOutputDeviceWindowsError.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\FRiffChunk.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\UnGnuG.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\UnLinker.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\UnMath.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\UnMsg.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\UnPSX2.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\UnUnix.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\UnVcWin32.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Inc\UnVcWn32SSE.h
# End Source File
# End Group
# Begin Group "Core\Src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Core\Src\Core.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\CorePrivate.h
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UExporter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnAnsi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnBits.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnCache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnClass.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnCoreNative.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnCoreNet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnCorSc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnLinker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnMath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnMem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnMisc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnObj.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnProp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnPSX2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnUnix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Core\Src\UnVcWin32.cpp
# End Source File
# End Group
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Group "Engine\Inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Engine\Inc\AActor.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AAIController.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AAmmunition.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ABrush.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ACamera.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AController.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ADecoration.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ADoor.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AEmitter.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AGameReplicationInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AInterpolationPoint.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AInventory.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AKActor.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AKCarWheelJoint.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AKConeLimit.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AKConstraint.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AKHinge.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AKVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ALadder.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ALadderVolume.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ALevelInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ALiftCenter.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ALight.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ALineOfSightTrigger.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AMatSubAction.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AMover.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ANavigationPoint.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ANote.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\APawn.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\APhysicsVolume.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\APickup.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\APlayerController.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\APlayerReplicationInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\APlayerStart.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\APowerups.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AProjectile.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AProjector.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ASceneManager.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AScout.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ASecurity.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AStationaryWeapons.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ATeamInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ATeleporter.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AVehicle.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AVolume.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AWarpZoneInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AWarpZoneMarker.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AWeapon.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AWeaponAttachment.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AWeaponFire.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AxEmitter.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AxPickUpBase.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AxProcMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AxWeatherEffect.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\AZoneInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\Bezier.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ENCVAG.H
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\Engine.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\EngineClasses.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\KTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\Palette.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\S3tc.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UBeamEmitter.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UCombiner.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UFinalBlend.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UI3DL2Listener.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UInteractionMaster.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\ULevelSummary.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UManifest.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UMatAction.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UMatSubAction.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UMeshEmitter.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UModifier.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnActor.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnAnim.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnAudio.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnBunch.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnCamera.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnCameraEffects.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnCanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnCDKey.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnChan.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnComponents.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnConn.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnConvexVolume.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnDDraw.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnDemoPenLev.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnDemoRec.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnDownload.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnEngine.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnEngineGnuG.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnEngineNative.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnEngineWin.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnFluidSurface.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnGame.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnIn.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnLevel.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnLodMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnMaterial.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnMatineeTools.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnModel.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnNet.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnNetDrv.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnObj.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnOctreePrivate.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnPackageCheckInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnParticleSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnPath.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnPenLev.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnPhysic.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnPlayer.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnPrim.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnProjector.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnReach.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnRebuildTools.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnRender.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnRenderResource.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnRenderUtil.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnRenDev.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnSkeletalMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnStatGraph.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnStaticMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnStats.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnTerrain.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnTerrainTools.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnTex.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnURL.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnVertMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UnVolume.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UParticleEmitter.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UPausedCameraControl.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UReachSpec.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\UShader.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\USparkEmitter.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\USpline.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\USpriteEmitter.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\USubActionCameraShake.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\USubActionFade.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\USubActionFOV.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\USubActionGameSpeed.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\USubActionOrientation.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\USubActionSceneSpeed.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\USubActionTrigger.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\xDecalMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\xOctTree.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\xParticleMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Inc\xTexShader.h
# End Source File
# End Group
# Begin Group "Engine\Src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Engine\Src\Amd3d.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\AStatLog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\AxEmitter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\AxPickupBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\AxProcMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\AxWeatherEffect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\Engine.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\EnginePrivate.h
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\KarmaSupport.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\KConstraint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\KDebugDraw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\KPhysic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\KTriListGen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\KUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\NullDrv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\palette.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\ULodMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnActCol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnActor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnAudio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnBeamEmitter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnBunch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnCamera.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnCameraEffects.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnCamMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnCanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnCDKey.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnChan.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnConn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnController.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnConvexVolume.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnDemoPenLev.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnDemoRec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnDownload.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnEngine.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnErrorChecking.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnFluidSurface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnFont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnFPoly.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnGame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnGameUtilities.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnIn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnInteraction.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnInteractionMaster.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnLevAct.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnLevel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnLevTic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnLodMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnManifest.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnMaterial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnMatineeTools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnMeshEmitter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnModelLight.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnMover.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnNavigationPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnNetDrv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnOctree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnPackageCheckInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnParams.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnParticleEmitter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnPath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnPawn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnPenLev.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnPhysic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnPlayer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnPrim.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnProjector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnReach.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRebuildTools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRender.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRenderBatch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRenderBSP.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRenderEditorActor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRenderLight.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRenderPool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRenderResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRenderStaticMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRenderUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRenderVisibility.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnRoute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnSceneManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnScript.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnScriptedTexture.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnSecurity.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnShadowProjector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnSkeletalMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnSkeletalTools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnSparkEmitter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnSpriteEmitter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnStatGraph.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnStaticMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnStaticMeshBuild.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnStaticMeshCollision.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnStats.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnTerrain.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnTex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnTrace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnURL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnVehicle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnVertMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\UnVolume.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\USpline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\xDataObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\xDecalMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Engine\Src\xParticleMgr.cpp
# End Source File
# End Group
# End Group
# Begin Group "D3DDrv"

# PROP Default_Filter ""
# Begin Group "D3DDrv\Inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\D3DDrv\Src\D3DRenderDevice.h
# End Source File
# Begin Source File

SOURCE=..\..\D3DDrv\Src\D3DRenderInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\D3DDrv\Src\D3DResource.h
# End Source File
# Begin Source File

SOURCE=..\..\D3DDrv\Src\xD3DHelper.h
# End Source File
# End Group
# Begin Group "D3DDrv\Src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\D3DDrv\Src\D3DDrv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\D3DDrv\Src\D3DMaterialState.cpp
# End Source File
# Begin Source File

SOURCE=..\..\D3DDrv\Src\D3DRenderDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\D3DDrv\Src\D3DRenderInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\D3DDrv\Src\D3DRenderState.cpp
# End Source File
# Begin Source File

SOURCE=..\..\D3DDrv\Src\D3DResource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\D3DDrv\Src\xD3DHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\D3DDrv\Src\xD3DRenderInterface.cpp
# End Source File
# End Group
# End Group
# Begin Group "IpDrv"

# PROP Default_Filter ""
# Begin Group "IpDrv\Inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\IpDrv\Inc\AInternetLink.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\ATcpLink.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\AUdpLink.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\GameSpyClasses.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\GameSpyClassesPublic.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\HTTPDownload.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\InternetLink.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\IpDrvClasses.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\UnIpDrv.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\UnIpDrvCommandlets.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\UnIpDrvNative.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\UnSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\UnSocketArchive.h
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Inc\UnTcpNetDriver.h
# End Source File
# End Group
# Begin Group "IpDrv\Src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\IpDrv\Src\HTTPDownload.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Src\InternetLink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Src\IpDrv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Src\TcpLink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Src\TcpNetDriver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Src\UCompressCommandlet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Src\UdpLink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Src\UnIpDrvNative.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Src\UnSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Src\MasterServerClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\IpDrv\Src\MasterServerUplink.cpp
# End Source File
# End Group
# End Group
# Begin Group "zlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\zlib\adler32.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\compress.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\crc32.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\deflate.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\gzio.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\infblock.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\infcodes.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\inffast.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\inflate.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\inftrees.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\infutil.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\trees.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\uncompr.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=..\..\zlib\zutil.c
# ADD CPP /W3
# End Source File
# End Group
# Begin Group "XInterface"

# PROP Default_Filter ""
# Begin Group "XInterface/Src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\XInterface\Src\AHudBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Src\AMenuBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Src\AMenuDefaults.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Src\AMenuDraw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Src\AMenuInput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Src\AMenuTick.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Src\AMenuTravel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Src\AMenuVirtualKeyboard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Src\XInterface.cpp
# End Source File
# End Group
# Begin Group "XInterface/Inc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\XInterface\Inc\AMenuBase.h
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Inc\AMenuSystemTest.h
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Inc\AMenuVirtualKeyboard.h
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Inc\XInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Inc\XInterfaceClasses.h
# End Source File
# Begin Source File

SOURCE=..\..\XInterface\Inc\XInterfaceNative.h
# End Source File
# End Group
# End Group
# Begin Group "XGame"

# PROP Default_Filter ""
# Begin Group "XGameSrc"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\..\XGame\Src\XGame.cpp
# End Source File
# Begin Source File

SOURCE=..\..\XGame\Src\XUtil.cpp
# End Source File
# End Group
# Begin Group "XGameInc"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\..\XGame\Inc\UxUtil.h
# End Source File
# Begin Source File

SOURCE=..\..\XGame\Inc\XGame.h
# End Source File
# Begin Source File

SOURCE=..\..\XGame\Inc\XGameClasses.h
# End Source File
# Begin Source File

SOURCE=..\..\XGame\Inc\XGameNative.h
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\..\System\DefaultXbox.ini
# End Source File
# Begin Source File

SOURCE=..\..\System\DefUserXbox.ini
# End Source File
# Begin Source File

SOURCE=..\..\System\doX.bat
# End Source File
# End Target
# End Project
