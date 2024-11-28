/***********************************************************************************************
*
*	$Id: RwFuncs.hpp,v 1.1.2.1 2002/03/01 19:55:03 richardm Exp $
*
************************************************************************************************/

#ifndef _RWFUNCS_H
#define _RWFUNCS_H

#include "rwcore.h"
#include "rpworld.h"

typedef struct
{
    RwInt32 totalAtomics;
    RwInt32 totalTriangles;
    RwInt32 totalVertices;
}
ClumpStatistics;


/*
 * Functions global over all source files...
 */
//extern RwBool Initialize3D(void *devParam, RwChar separator, RwChar *dumpPath);
extern void Terminate3D(void);

extern void RenderFrame(void);

extern RpClump *ClumpLoadDFF(RwChar *path);
extern RpClump *LoadDffFile(RwChar *fname);
extern RpClump *ClumpLoadWRL(RwChar *wrlPath, RwReal scale);

extern void ClumpGetBoundingSphere(RpClump *clump, RwSphere *clumpSphere);
extern void CameraSetViewFrustrum(RwCamera *camera, RwReal fov,
													RwReal aspect_ratio,
													RwReal near_clip,
													RwReal far_clip);
extern void CameraSetPos(RwCamera *camera, RwReal x, RwReal y, RwReal z);
extern void CameraSetDegAngles(RwCamera *camera, RwReal yaw, RwReal Pitch, RwReal roll);
extern RwBool CreateWorldFromTableBSP(RwChar *filename);
extern RwBool InitialiseGraphicsEngine(void *devParam);
extern RwBool CreateCameras(void);
extern void LightSetDegAngles(RpLight *light, RwReal yaw, RwReal pitch);
extern void LightSetPos(RpLight *light, RwReal x, RwReal y, RwReal z);
//extern RwBool *CreateWorldFromTableBSP(RwChar *filename);
//extern void DisplayErrorMessage(const RwChar *message);
//extern void DisplayWarningMessage(const RwChar *message);
//extern void DisplayDebugMessage(const RwChar *message);
extern void SetClumpPos(RpClump *clump, RwReal x, RwReal y, RwReal z);
extern void SetClumpAng(RpClump *clump, RwReal yaw, RwReal pitch);
extern RpLight *CreateLight(RpWorld *world, RwInt32 type, RwV3d *position, RwReal direction, RwReal pitch, RwRGBAReal *colour, RwReal radius , RwReal cone, RwInt32 flags);
extern RwBool CreateRearViewMirror(void);
extern RwBool ResizeRearViewMirror(void);

/*
 * Variables global over all source files...
 */
extern RpWorld  *World;
extern RwV3d Worigin;
extern RwBBox world_bbox;

//extern RpLight *MainLight;


extern RwBool ScreenInfoOn;


extern RwInt32 FrameCounter;
extern RwInt32 FramesPerSecond;

extern RwRaster *Charset;
extern RwRGBA BackgroundColor;
extern RwRGBA col_WHITE;
extern RwRGBA col_RED;
extern RwRGBA col_GREEN;
extern RwRGBA col_BLUE, col_YELLOW;
#endif
