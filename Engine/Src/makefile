#=============================================================================
# Unreal Engine makefile for non-Windows platforms.
#=============================================================================

# Unreal directory. (Required by makefile-header.)
UNREAL_DIR		= ../..

# Include global definitions.
include $(UNREAL_DIR)/makefile-header

#-----------------------------------------------------------------------------
# Directories.
#-----------------------------------------------------------------------------

SRC_DIRS		= $(ENGINE_SRC)
INC_DIRS		= $(ENGINE_INC)
BUILD_DIR		= $(ENGINE_BUILD)

#-----------------------------------------------------------------------------
# Compiler/linker options.
#-----------------------------------------------------------------------------

CXX			= $(STD_CXX)
CXXFLAGS		= $(STD_CXXFLAGS) $(EXT_CXXFLAGS) -I$(ENGINE_INC) -I$(ENGINE_SRC) -I$(CORE_INC) \
	    			-DGPackage=GPackageEngine -DThisPackage=Engine
LDFLAGS			= $(STD_LDFLAGS) $(EXT_LDFLAGS)$(ENGINE_SONAME)
LIBS			= $(STD_LIBS) $(CORE) $(KARMA_LIBS) $(OGG) $(VORBIS)
AS			= $(STD_AS)
ASFLAGS			= $(STD_ASFLAGS)

#-----------------------------------------------------------------------------
# Files.
#-----------------------------------------------------------------------------

WINDLLEXP := ENGINE_API
PRECOMPILED_HEADER := EnginePrivate.h
PRECOMPILE_ON := AFileLog.cpp

OBJS =				\
	UnTerrain.o		\
	AFileLog.o 		\
	AGameStats.o 		\
	Engine.o		\
	KarmaSupport.o		\
    KConstraint.o		\
	KDebugDraw.o		\
	KPhysic.o		\
	KTriListGen.o		\
	KIntersectFunctions.o		\
	KUtils.o		\
    KScript.o \
    KSkeletal.o \
    KFarfield.o \
    KDynStep.o \
	ULodMesh.o		\
	UnActCol.o		\
	UnActor.o		\
	UnAudio.o		\
	UnBeamEmitter.o 	\
	UnCamera.o		\
	UnCameraEffects.o	\
	UnCamMgr.o		\
	UnController.o		\
	UnEngine.o		\
	UnErrorChecking.o 	\
	UnFont.o		\
	UnGame.o		\
	UnGameUtilities.o	\
	UnIn.o			\
	UnLevAct.o		\
	UnLevel.o		\
	UnLevTic.o		\
	UnLodMesh.o		\
	UnMatineeTools.o 	\
	UnMesh.o		\
	UnMeshEmitter.o 	\
	UnModel.o		\
	UnModelLight.o		\
	UnMover.o		\
	UnNavigationPoint.o 	\
	UnParams.o		\
	UnParticleEmitter.o 	\
	UnParticleSystem.o 	\
	UnPath.o		\
	UnPawn.o		\
	UnPhysic.o		\
	UnPlayer.o		\
	UnPrim.o		\
	UnProjector.o		\
	UnReach.o		\
	UnRebuildTools.o 	\
	UnRender.o		\
	UnRenderBSP.o		\
	UnRenderResource.o 	\
	UnRenderUtil.o 		\
	UnRenderVisibility.o 	\
	UnRenderEditorActor.o 	\
	UnRenderLight.o       \
	UnRenderBatch.o       \
	UnRenderStaticMesh.o       \
	UnRenderPool.o       \
	UnRoute.o		\
	UnScript.o		\
	UnSkeletalMesh.o 	\
	UnSparkEmitter.o 	\
	UnSpriteEmitter.o 	\
	UnStaticMesh.o		\
	UnStats.o 		\
	UnTerrainTools.o	\
	UnTex.o			\
	UnTrace.o		\
	UnURL.o			\
	UnVehicle.o		\
	UnVertMesh.o		\
	UnBunch.o		\
	UnChan.o		\
	UnConn.o		\
	UnDemoPenLev.o		\
	UnDemoRec.o		\
	UnDownload.o		\
	UnNetDrv.o		\
	UnPenLev.o		\
	UnCanvas.o		\
	UnFPoly.o		\
	UnSceneManager.o	\
	ASceneSubtitles.o	\
	UnInteraction.o		\
	UnInteractionMaster.o	\
	UnMaterial.o		\
	AxEmitter.o		\
	AxProcMesh.o		\
	AxPickupBase.o  	\
	AxWeatherEffect.o   \
	UnStatGraph.o       \
	UnStaticMeshCollision.o       \
	UnPackageCheckInfo.o       \
	UnSkeletalTools.o       \
	USpline.o       \
	UnVolume.o       \
	UnFluidSurface.o       \
	UnManifest.o       \
	UnSecurity.o       \
	UnStaticMeshBuild.o       \
	UnScriptedTexture.o       \
	UnOctree.o       \
    xDataObject.o \
    xParticleMgr.o \
    xForceFeedback.o \
    UnCDKey.o \
    UnShadowProjector.o \
    UnPlayInfo.o \
    UnConvexVolume.o \
    UnTrailEmitter.o \
    NullDrv.o \
    UnSVehicle.o \
    UnSkeletalMeshCollision.o \
    UnVoiceChat.o \
    UnCacheManager.o \
    UExportCacheCommandlet.o \
    UStreamInterface.o \
    UnkDOP.o

OUT = $(ENGINE)

#-----------------------------------------------------------------------------
# Rules.
#-----------------------------------------------------------------------------

# Use general rules.
include $(UNREAL_DIR)/makefile-general

#-----------------------------------------------------------------------------
# The End.
#-----------------------------------------------------------------------------
