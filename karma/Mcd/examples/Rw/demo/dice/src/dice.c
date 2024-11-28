
/****************************************************************************
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */

/****************************************************************************
 *                                                                         
 * RwDice.c
 *
 * Copyright (C) 1999 Criterion Technologies.
 *
 * Author  : John Irwin.
 *                                                                         
 * Purpose : RenderWare 3.0 demo.
 *                         
 ****************************************************************************/

/*
 * Collision detection and physics
 */

#include <McdFrame.h>         /* MathEngine Collision Toolkit: prefix Mcd       */
#include <McdPrimitives.h>    /* Mcd primitive geometry types: prefix Mcd       */
#include <Mst.h>              /* Mst dynamics/collision bridge, prefix Mst */
#include <McdRwBSP.h>         /* for renderware functions */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "rwcore.h"
#ifdef RWLOGO
#include "rplogo.h"
#endif
#include "rpworld.h"
#include "rtcharse.h"
#include "rprandom.h"

/* 
 * Platform abstraction interface...
 */
#include "skeleton.h"
#include "camera.h"
#include "padmap.h"
#include "metrics.h"

int geoTypeMaxCount;
 
/* Defines */
#define NUMDICE 5
#define DIEHALFHEIGHT (RwReal)(0.5)
#define MINENERGY (RwReal)(0.1)
#define FRICTION (RwReal)(0.95)
#define TIMEOUT (RwReal)(7.0)
#define RESAMPLEDSIZE 50
#define MAXTIMESTEP 1
#define CAMERAWIDTH 640
#define CAMERAHEIGHT 480
#define LIGHTHEIGHT (RwReal)(20.0)

#define DEFAULT_ASPECTRATIO (RwReal)(4.0 / 3.0)

typedef struct
{
    RwReal elevation;
    RwReal azimuth;
    RwReal distance;
    RwReal hfov;
}
CameraView;

typedef enum
{
    MMNoAction, 
    MMCameraPoint, 
    MMCameraAdvance,
    MMCameraNearClipMove, 
    MMCameraFarClipMove, 
    MMCameraZoomLens
}
MMMode;

static MMMode MouseMoveMode = MMNoAction;

static RwReal  CurrentViewWindow = (RwReal)(0.577);

/*
 * Acceleration due to gravity!...
 */
static RwV3d Gravity = {(RwReal)(0.0), (RwReal)(-20.0), (RwReal)(0.0)};

static RwChar RollCaption[128] = 
#ifdef SKY
    RWSTRING("First Roll (R2 click to roll, R1 to pick)");
#else
    RWSTRING("First Roll (mouse-right click to roll, left to pick)");
#endif /* SKY */

static RwRaster *DieFaceRasters[6];
static RwRaster *Pointer = NULL;

static RwBool GravityOn = TRUE;
static RwBool ScreenInfoOn = FALSE;
static RwBool CameraPointing = FALSE;
static RwBool CameraTracking = FALSE;
static RwBool CameraZooming = FALSE;

static RpWorld *World = NULL;
static RpLight *MainLight = NULL;
static RpLight *AmbientLight = NULL;
static RwCamera *Camera = NULL;
static RwRaster *Charset = NULL;

#define NContacts   100

static MdtWorld *meWorld;
static MdtBody *ballBody;
static MdtBody *dieBody;
static RpClump *ballClump = NULL;
static RpClump *dieClump = NULL;

static MeReal bodyTransformationMatrix[16];

static MstBridge               *bridge;
static McdSpace                *space;  

static McdModel *table;
static McdModel *ball;
static McdModel *die;

static const RwV3d *Worigin;

static RwBool DiceRolling;

static CameraView PresetViews[] = 
{
    { (RwReal)(-51.0),   (RwReal)(9.0), (RwReal)(27.0), (RwReal)(60.0) },
    { (RwReal)(-21.0), (RwReal)(116.0), (RwReal)(11.0), (RwReal)(60.0) },
    { (RwReal)(-12.0), (RwReal)(-45.0), (RwReal)(13.0), (RwReal)(60.0) },
    { (RwReal)(-12.0), (RwReal)(135.0), (RwReal)(13.0), (RwReal)(60.0) },
    { (RwReal)(-90.0),  (RwReal)(45.0), (RwReal)(30.0), (RwReal)(60.0) }
};
#define NUMOFVIEWS (sizeof(PresetViews)/sizeof(CameraView))

static RwInt32 ViewIndex = 0;
static CameraView CurrentView;

static RwV3d Xaxis = { (RwReal)(1.0), (RwReal)(0.0), (RwReal)(0.0) };
static RwV3d Yaxis = { (RwReal)(0.0), (RwReal)(1.0), (RwReal)(0.0) };
static RwV3d Zaxis = { (RwReal)(0.0), (RwReal)(0.0), (RwReal)(1.0) };

static RwRGBA ForegroundColor = {255, 255, 255, 255};
static RwRGBA BackgroundColor = {0, 0, 0, 0};

static RwUInt32 lastFrameTime;
static RwUInt32 lastAnimTime;

static RwV3d DiceOnTablePos[5] = 
{
    {  (RwReal)(0.0), DIEHALFHEIGHT,  (RwReal)(0.0) },
    {  (RwReal)(1.5), DIEHALFHEIGHT,  (RwReal)(1.5) },
    { (RwReal)(-1.5), DIEHALFHEIGHT,  (RwReal)(1.5) },
    {  (RwReal)(1.5), DIEHALFHEIGHT, (RwReal)(-1.5) },
    { (RwReal)(-1.5), DIEHALFHEIGHT, (RwReal)(-1.5) }
};


static RwV3d InitPos[5] = 
{
    { (RwReal)(-4.0),  (RwReal)(2.0), (RwReal)(6.0) },
    { (RwReal)(-6.0),  (RwReal)(2.0), (RwReal)(4.0) },
    { (RwReal)(-4.0),  (RwReal)(2.0), (RwReal)(4.0) },
    { (RwReal)(-6.0),  (RwReal)(2.0), (RwReal)(6.0) },
    { (RwReal)(-5.0),  (RwReal)(4.0), (RwReal)(5.0) }
};

static RwV3d InitLinVel[5] = 
{
    { (RwReal)(13.0), (RwReal)(5.0), (RwReal)(-11.0) },
    { (RwReal)(11.0), (RwReal)(5.0), (RwReal)(-13.0) },
    { (RwReal)(12.0), (RwReal)(5.0), (RwReal)(-12.0) },
    { (RwReal)(13.0), (RwReal)(5.0), (RwReal)(-11.0) },
    { (RwReal)(12.0), (RwReal)(5.0), (RwReal)(-12.0) }
};

static RwInt32 FrameCounter = 0;
static RwInt32 FramesPerSecond = 0;

static RwV3d Zero = {(RwReal)(0.0), (RwReal)(0.0), (RwReal)(0.0)};

static RwMatrix *TempMat1;
static RwMatrix *TempMat2;

static RwChar DebugString[64];

#define MAXVERTS 1003
#define SAVEMODELSx

/*
 *****************************************************************************
 */
static RpAtomic *
GetFirstAtomic(RpAtomic *atomic, void *data)
{
    *(RpAtomic **)data = atomic;

    return NULL;
}


/*
 *****************************************************************************
 */
static RpMaterial * 
SetTextureFilterMode(RpMaterial *material, void *data)
{
    RwTextureFilterMode filterMode;

    filterMode = *(RwTextureFilterMode *)data;

    RwTextureSetFilterMode(RpMaterialGetTexture(material), filterMode);

    return material;
}



/*
 *****************************************************************************
 */
static void 
SimilarityTransform(RwMatrix *out, RwMatrix *in, RwMatrix *orientation)
{
    RwV3d *orientRight, *orientUp, *orientAt;
    RwV3d *tempRight, *tempUp, *tempAt, *tempPos;

    /* 
     * First calculate the transpose of the orientation matrix...
     */
    orientRight = RwMatrixGetRight(orientation);
    orientUp    = RwMatrixGetUp(orientation);
    orientAt    = RwMatrixGetAt(orientation);

    tempRight = RwMatrixGetRight(TempMat1);
    tempUp    = RwMatrixGetUp(TempMat1);
    tempAt    = RwMatrixGetAt(TempMat1);
    tempPos   = RwMatrixGetPos(TempMat1);

    tempRight->x = orientRight->x;
    tempRight->y = orientUp->x;
    tempRight->z = orientAt->x;

    tempUp->x = orientRight->y;
    tempUp->y = orientUp->y;
    tempUp->z = orientAt->y;

    tempAt->x = orientRight->z;
    tempAt->y = orientUp->z;
    tempAt->z = orientAt->z;

    tempPos->x = (RwReal)(0.0);
    tempPos->y = (RwReal)(0.0);
    tempPos->z = (RwReal)(0.0);

    RwMatrixUpdate(TempMat1);
        
    /* 
     * Similarity transformation: 
     * out = transpose(orientation) * in * orientation
     */
    RwMatrixMultiply(TempMat2, TempMat1, in);
    RwMatrixMultiply(out, TempMat2, orientation);

    return;
}



/*
 *****************************************************************************
 */
static void 
GetRandomVector(RwV3d *vec, RwBool normal)
{
    vec->x = (RwReal)(2.0) * 
        ((RwReal)(RpRandom()) / (RwUInt32MAXVAL>>1)) - (RwReal)(1.0);
    
    vec->y = (RwReal)(2.0) * 
        ((RwReal)(RpRandom()) / (RwUInt32MAXVAL>>1)) - (RwReal)(1.0);
    
    vec->z = (RwReal)(2.0) * 
        ((RwReal)(RpRandom()) / (RwUInt32MAXVAL>>1)) - (RwReal)(1.0);

    if( normal )
    {
        RwV3dNormalize(vec, vec);
    }

    return;
}



/*
 *****************************************************************************
 */
static RwReal 
GetRandomUnitReal(void)
{
    return (RwReal)(RpRandom()) / (RwUInt32MAXVAL>>1);
}





/*
 *****************************************************************************
 */
static void 
CameraSetView(RwCamera *camera, CameraView view)
{
    RwFrame *cameraFrame;
    RwMatrix *matrix;
    RwV3d at;

    cameraFrame = RwCameraGetFrame(camera);

    /*
     * Orient the camera...
     */
    RwFrameRotate(cameraFrame, &Xaxis, -view.elevation, rwCOMBINEREPLACE);
    RwFrameRotate(cameraFrame, &Yaxis, view.azimuth, rwCOMBINEPOSTCONCAT);

    /*
     * Position the camera...
     */
    matrix = RwFrameGetMatrix(cameraFrame);
    at = *RwMatrixGetAt(matrix);

    RwV3dScale(&at, &at, -view.distance);
    RwV3dAdd(&at, &at, Worigin);

    RwFrameTranslate(cameraFrame, &at, rwCOMBINEPOSTCONCAT);

    /*
     * Set the field-of-view...
     * Note: this is handled in the camera resize code so we simply force 
     * a resize event
     */
    CurrentViewWindow = (RwReal)tan(view.hfov * rwPI / 360.0);
    
    RsEventHandler(rsCAMERASIZE, NULL);

    return;
}



/*
 *****************************************************************************
 */
static void 
CameraPoint(RwReal deltaElevation, RwReal deltaAzimuth)
{
    if( !DiceRolling )
    {
        RwFrame *cameraFrame;
        RwV3d pos;

        CurrentView.elevation += deltaElevation;

        /*
         * Limit the camera's elevation so that it never quite reaches
         * exactly +90 or -90 degrees to avoid the singularity in these
         * situations...
         */
        if( CurrentView.elevation > (RwReal)(89.0) )
        {
            CurrentView.elevation = (RwReal)(89.0);
        }
        else if( CurrentView.elevation < (RwReal)(-89.0) )
        {
            CurrentView.elevation = (RwReal)(-89.0);
        }

        CurrentView.azimuth += deltaAzimuth;

        /*
         * Keep the azimuth in the range -180 to +180 degrees...
         */
        if( CurrentView.azimuth > (RwReal)(180.0) )
        {
            CurrentView.azimuth -= (RwReal)(360.0);
        }
        else if( CurrentView.azimuth < (RwReal)(-180.0) )
        {
            CurrentView.azimuth += (RwReal)(360.0);
        }

        cameraFrame = RwCameraGetFrame(Camera);

        /*
         * Remember where the camera is...
         */
        pos = *RwMatrixGetPos(RwFrameGetMatrix(cameraFrame));

        /*
         * Reset the camera's frame; we're dealing with absolute
         * orientations here...
         */
        RwFrameRotate(cameraFrame, &Xaxis, 
            -CurrentView.elevation, rwCOMBINEREPLACE);
    
        RwFrameRotate(cameraFrame, &Yaxis, 
            CurrentView.azimuth, rwCOMBINEPOSTCONCAT);

        /*
         * Put the camera back to where it was...
         */
        RwFrameTranslate(cameraFrame, &pos, rwCOMBINEPOSTCONCAT);
    }

    return;
}



/*
 *****************************************************************************
 */
static void 
CameraAdvance(RwReal distance)
{
    if( !DiceRolling )
    {
        RwFrame *frame;
        RwV3d at, pos;

        /*
         * Move the camera along it's look-at vector the given distance...
         */
        frame = RwCameraGetFrame(Camera);

        at = *RwMatrixGetAt(RwFrameGetMatrix(frame));

        RwV3dScale(&at, &at, distance);

        RwFrameTranslate(frame, &at, rwCOMBINEPOSTCONCAT);

        /*
         * Update the camera view distance parameter...
         */
        pos = *RwMatrixGetPos(RwFrameGetMatrix(frame));
        RwV3dSub(&pos, &pos, Worigin);
        CurrentView.distance = RwV3dLength(&pos);
    }

    return;
}



/*
 *****************************************************************************
 */
static void 
CameraZoomLens(RwReal delta)
{
    if( !DiceRolling )
    {
        CurrentViewWindow += delta;

        /*
         * Limit the HFoV to greater than 0.1 degrees...
         */
        if( CurrentViewWindow < (RwReal)(0.00086) )
        {
            CurrentViewWindow = (RwReal)(0.00086);
        }

        /*
         * Set the field-of-view...
         * Note: this is handled in the camera resize code so we simply force 
         * a resize event
         */
        RsEventHandler(rsCAMERASIZE, NULL);
        
        CurrentView.hfov = (RwReal)(360.0 * atan(CurrentViewWindow) / rwPI);
    }

    return;
}



/*
 *****************************************************************************
 */
static RpLight *
CreateMainLight(void)
{
    RpLight *light;
    RwFrame *lightFrame;
    RwV3d tempVec;

    light = RpLightCreate(rpLIGHTPOINT);
    
    if( light )
    {
        RpLightSetRadius(light, (RwReal)(100.0));

        /* 
         * This one needs a frame...
         */
        lightFrame = RwFrameCreate();
        
        if( lightFrame )
        {
            RwFrameRotate(lightFrame, &Xaxis, (RwReal)(90.0), rwCOMBINEREPLACE);

            tempVec = *Worigin;
            tempVec.y += LIGHTHEIGHT;
            RwFrameTranslate(lightFrame, &tempVec, rwCOMBINEPOSTCONCAT);

            RpLightSetFrame(light, lightFrame);

            RpWorldAddLight(World, light);

            return light;
        }

        RpLightDestroy(light);
    }

    return NULL;
}



/*
 *****************************************************************************
 */
static RpLight *
CreateAmbientLight(void)
{
    RpLight *light;

    light = RpLightCreate(rpLIGHTAMBIENT);
    
    if( light )
    {
        RpWorldAddLight(World, light);

        return light;
    }

    return NULL;
}



/*
 *****************************************************************************
 */
static RpWorld * 
CreateWorldFromTableBSP(RwChar *filename)
{
    RwStream *stream;
    RpWorld *world;
    RwSurfaceProperties surfProps;
    RwTextureFilterMode filterMode;

    RwImageGammaCorrectionSetGamma((RwReal)(1.4));

    /*
     * Read the table BSP from a binary format file...
     */
    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);

    if( stream )
    {
        if( RwStreamFindChunk(stream, rwID_WORLD, NULL, NULL) ) 
        {
            world = RpWorldStreamRead(stream);

            if( world )
            {
                RwStreamClose(stream, NULL);
                RwImageGammaCorrectionSetGamma((RwReal)(1.0));

#ifdef SAVEMODELS
                _bspSave(world, filename);
#endif /* SAVEMODELS */

                surfProps.ambient = (RwReal)(0.3);
                surfProps.diffuse = (RwReal)(0.7);
                surfProps.specular = (RwReal)(0.0);
                RpWorldSetSurfaceProperties(world, &surfProps);

                filterMode = rwFILTERLINEAR;
                RpWorldForAllMaterials(world, SetTextureFilterMode, 
                    (void *)(&filterMode));

                RpWorldSetFlags(world, RpWorldGetFlags(world) & ~rpWORLDPRELIT);

                Worigin = RpWorldGetOrigin(world);

                return(world);
            }
        }

        RwStreamClose(stream, NULL);
    }

    RwImageGammaCorrectionSetGamma((RwReal)(1.0));

    return NULL;
}



/*
 *****************************************************************************
 */
static void 
InitializeDice(void)
{

    return;
}



/*
 *****************************************************************************
 */
static RwBool 
CreateDieFaceRasters(void)
{
    RwInt32 i;
    RwImage *image1, *image2;
    static const RwChar *DieFaceImageNames[6] = 
    { 
        RWSTRING("die1x.bmp"), 
        RWSTRING("die2x.bmp"),
        RWSTRING("die3x.bmp"), 
        RWSTRING("die4x.bmp"),
        RWSTRING("die5x.bmp"), 
        RWSTRING("die6x.bmp")
    };

    for(i=0; i<6; i++)
    {
        image1 = RwImageRead(DieFaceImageNames[i]);

        if( image1 )
        {
            image2 = RwImageCreateResample(image1, RESAMPLEDSIZE, RESAMPLEDSIZE);

            RwImageDestroy(image1);

            if( image2 )
            {
                DieFaceRasters[i] = RwRasterCreate(RESAMPLEDSIZE, RESAMPLEDSIZE, 
                    0, rwRASTERTYPENORMAL);

                if( DieFaceRasters[i] )
                {
                    RwRasterSetFromImage(DieFaceRasters[i], image2);

                    RwImageDestroy(image2);
                }
                else
                {
                    RwImageDestroy(image2);

                    return FALSE;
                }
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }

    return TRUE;
}


/*
 *****************************************************************************
 */
static RwRaster * 
CreatePointerRaster(void)
{
    RwChar imagePath[256];
    RwImage *image1, *image2;
    RwRaster *raster;

    /*
     * Read the images used for creating the camera pointer raster...
     */
    rwsprintf(imagePath, RWSTRING("textures%c"), RsPathGetSeparator());
    RwImageSetPath(imagePath);

    image1 = RwImageReadMaskedImage(RWSTRING("pointer.bmp"), 
        RWSTRING("pointerm.bmp"));

    if( image1 )
    {
        /*
         * Resample the pointer image to half its size...
         */
        image2 = RwImageCreateResample(image1, RwImageGetWidth(image1) >> 1, 
            RwImageGetHeight(image1) >> 1);

        RwImageDestroy(image1);

        if( image2 )
        {
            /*
             * Create the pointer raster...
             */
            raster = RwRasterCreate(RwImageGetWidth(image2), 
                RwImageGetHeight(image2), 0, rwRASTERTYPENORMAL);

            if( raster )
            {
                /*
                 * ...and initialize its pixels from those in the image...
                 */
                RwRasterSetFromImage(raster, image2);

                RwImageDestroy(image2);

                return raster;
            }
        }
    }

    return NULL;
}






/*
 *****************************************************************************
 */
static RpClump * 
CreateClump(RwChar *filename)
{
    RwStream            *stream;
    RpClump             *clump;

    /*
     * Read the ball clump from a binary format file...
     */
    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);
    
    if( stream )
    {
        if( RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL ) ) 
        {
            clump = RpClumpStreamRead(stream);
        }

        RwStreamClose(stream, NULL );
    }

    if (!clump) {
        return NULL;
    }   
        
    RpWorldAddClump(World,clump);

    return clump;
}


/*
 *****************************************************************************
 */
static void
UpdateTM(RpClump *clump, MdtBody *body)
{
    RwFrame *frame;
    RwMatrix *matrix;
    MeMatrix4Ptr tm;

    frame = RpClumpGetFrame(clump);
    matrix = RwFrameGetMatrix(frame);

    tm = (MeMatrix4Ptr) MdtBodyGetTransformPtr(body);

    RwMatrixGetRight(TempMat1)->x = (RwReal)tm[0][0];
    RwMatrixGetRight(TempMat1)->y = (RwReal)tm[0][1];
    RwMatrixGetRight(TempMat1)->z = (RwReal)tm[0][2];
    RwMatrixGetUp(TempMat1)->x = (RwReal)tm[1][0];
    RwMatrixGetUp(TempMat1)->y = (RwReal)tm[1][1];
    RwMatrixGetUp(TempMat1)->z = (RwReal)tm[1][2];
    RwMatrixGetAt(TempMat1)->x = (RwReal)tm[2][0];
    RwMatrixGetAt(TempMat1)->y = (RwReal)tm[2][1];
    RwMatrixGetAt(TempMat1)->z = (RwReal)tm[2][2];
    RwMatrixGetPos(TempMat1)->x = (RwReal)tm[3][0];
    RwMatrixGetPos(TempMat1)->y = (RwReal)tm[3][1];
    RwMatrixGetPos(TempMat1)->z = (RwReal)tm[3][2];

    RwFrameTransform(frame, TempMat1, rwCOMBINEREPLACE);
}

/*
 *****************************************************************************
 */
static void
UpdateTMs()
{
    UpdateTM(ballClump,ballBody);
    UpdateTM(dieClump,dieBody);
}

/*
 *****************************************************************************
 */
static void
AdvanceSimulation(RwReal delta)
{
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge,space,meWorld);
    MdtWorldStep(meWorld,delta);
}


/*
 *****************************************************************************
 */
static void 
Render(void)
{
    RwInt32 crw, crh;
    RwRaster *camRas;

    RwCameraClear(Camera, &BackgroundColor, rwCAMERACLEARIMAGE|rwCAMERACLEARZ);

    camRas = RwCameraGetRaster(Camera);

    crw = RwRasterGetWidth(camRas);
    crh = RwRasterGetHeight(camRas);

    if( RwCameraBeginUpdate(Camera) )
    {
        RwChar caption[256];

        RpWorldRender(World);

        RtCharsetPrint(Charset, RollCaption, 15, 10);

        if( CameraPointing)
        {
            rwsprintf(caption, RWSTRING("Camera point (Elv, Azi): (%3.1f, %3.1f)"),
                CurrentView.elevation, CurrentView.azimuth);
        }
        else if( CameraTracking )
        {
            rwsprintf(caption, RWSTRING("Camera advance (Dist): %3.1f"),
                CurrentView.distance);
        }
        else if( CameraZooming )
        {
            rwsprintf(caption, RWSTRING("Camera zoom (HFoV): %.1f degs."),
                CurrentView.hfov);
        }
        else
        {
            rwsprintf(caption, RWSTRING(""));
        }

        RtCharsetPrint(Charset, caption, 15, crh-25);

#ifdef RWMETRICS
        RsMetricsRender();
#endif /* RWMETRICS */

        RwCameraEndUpdate(Camera);
    }

    RsCameraShowRaster(Camera);

    FrameCounter++;

    return;
}



/*
 *****************************************************************************
 */
static RwBool 
MovePointer(RwInt32 x, RwInt32 y)
{
    RwBool updateScreen;

    updateScreen = FALSE;

    if( !DiceRolling )
    {
    }

    return updateScreen;
}



/*
 *****************************************************************************
 */
static RwBool 
SelectPointer(RwV2d *pos)
{
    RwBool updateScreen;

    updateScreen = FALSE;

    if( !DiceRolling )
    {
        RpAtomic *pickedAtomic;

        pickedAtomic = RwCameraPickAtomicOnPixel(Camera, pos);

        if( pickedAtomic )
        {

        }
    }

    return updateScreen;
}



/*
 *****************************************************************************
 */
static RwBool 
StartRoll(RwInt32 x __RWUNUSED__, 
          RwInt32 y __RWUNUSED__)
{
    if( !DiceRolling )
    {
        DiceRolling = TRUE;
    }

    return FALSE;
}




/*
 *****************************************************************************
 */
RwBool 
RunSimulation(RwUInt32 milliseconds)
{
    RwBool updateScreen;

    updateScreen = FALSE;

    if( DiceRolling )
    {
        while (milliseconds > 0)
        {
            RwUInt32    deltaTime = milliseconds;

            if (deltaTime > MAXTIMESTEP)
            {
                deltaTime = (RwUInt32)MAXTIMESTEP;
            }

            AdvanceSimulation(deltaTime * 0.001f );

            UpdateTMs();

            milliseconds -= deltaTime;
        }

        updateScreen = TRUE;
    }

    return (updateScreen);
}

/*
 *****************************************************************************
 */
static void 
Terminate3D(void)
{
    RwInt32 i;
    RwFrame *frame;
    RpWorld *world;

#ifdef RWMETRICS
    RsMetricsClose();
#endif /* RWMETRICS */

    if( Pointer )
    {
        RwRasterDestroy(Pointer);
    }

    for(i=0; i<6; i++)
    {
        if( DieFaceRasters[i] )
        {
            RwRasterDestroy(DieFaceRasters[i]);
        }
    }

    if ( ballClump )
    {
        RpClumpDestroy(ballClump);
    }

    if ( dieClump )
    {
        RpClumpDestroy(dieClump);
    }

    if( Camera )
    {
        RpWorldRemoveCamera(World, Camera);

        CameraDestroy(Camera);
    }

    if( MainLight )
    {
        world = RpLightGetWorld(MainLight);

        if( world )
        {
            RpWorldRemoveLight(world, MainLight);
        }

        frame = RpLightGetFrame(MainLight);
        if( frame )
        {
            RpLightSetFrame(MainLight, NULL);
            RwFrameDestroy(frame);
        }

        RpLightDestroy(MainLight);
    }

    if( AmbientLight )
    {
        world = RpLightGetWorld(AmbientLight);

        if( world )
        {
            RpWorldRemoveLight(world, AmbientLight);
        }

        RpLightDestroy(AmbientLight);
    }

    if( World ) 
    {
        RpWorldDestroy(World);
    }

    if( Charset )
    {
        RwRasterDestroy(Charset);
    }

    if( TempMat1 )
    {
        RwMatrixDestroy(TempMat1);
    }

    if( TempMat2 )
    {
        RwMatrixDestroy(TempMat2);
    }

    RsRwTerminate();

    MstBridgeDestroy(bridge);
    McdTerm();

    MdtWorldDestroy(meWorld);

    return;
}



/*
 *****************************************************************************
 */
static RwBool 
Initialize3D(void *param)
{
    RwChar *pathname;
    short material1,material2;
    MdtBclContactParams *params;

/* #define DIE_COLLIDES_LIKE_TWO_SPHERES */
/* Does Not Work at Present */

#ifdef DIE_COLLIDES_LIKE_TWO_SPHERES
    static MeMatrix4 mem1,mem2,mem3,mem4;
#endif /* DIE_COLLIDES_LIKE_TWO_SPHERES */

    /* 
     * Do the standard skeleton initialisation...
     */
    if( !RsRwInitialise(param) )
    {
        return FALSE;
    }

    TempMat1 = RwMatrixCreate();
    if( TempMat1 == NULL )
    {
        return FALSE;
    }

    TempMat2 = RwMatrixCreate();
    if( TempMat2 == NULL )
    {
        return FALSE;
    }

    /* 
     * Set the path to find the textures at...
     */
    pathname = RsPathnameCreate(RWSTRING(".\\textures\\"));
    RwImageSetPath(pathname);
    RsPathnameDestroy(pathname);

    Charset = RtCharsetCreate(&ForegroundColor, &BackgroundColor);
    if( Charset == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create raster chraset."));
        
        return FALSE;
    }

    /*
     * Create and initialize the world and all it's got.
     */
    pathname = RsPathnameCreate(RWSTRING(".\\models\\dicetabl.bsp"));

    World = CreateWorldFromTableBSP(pathname);

    RsPathnameDestroy(pathname);

    if( World == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create world."));
        
        return FALSE;
    }

    /*
     * Lights...
     */
    MainLight = CreateMainLight();
    if( MainLight == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create main light."));
        
        return FALSE;
    }

    AmbientLight = CreateAmbientLight();
    if( AmbientLight == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create ambient light."));
        
        return FALSE;
    }
    
    /* 
     * Camera...
     */
    Camera = CameraCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, TRUE);

    if( Camera == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create camera."));

        return FALSE;
    }
    else
    {
        RwCameraSetNearClipPlane(Camera, (RwReal)(0.1));
        RwCameraSetFarClipPlane(Camera, (RwReal)(50.0));

        RpWorldAddCamera(World, Camera);

        CurrentView = PresetViews[0];
        CameraSetView(Camera, CurrentView);
    }

    /* 
     * Action!
     */

    /*
     * Read in the die's texture images and convert to rasters
     * (of the correct type) for display in the camera view...
     */
    if( !CreateDieFaceRasters() )
    {
        RsErrorMessage(RWSTRING("Cannot create die face rasters."));
        
        return FALSE;
    }

    /*
     * Create a camera pointer, which aids world navigation...
     */
    Pointer = CreatePointerRaster();
    if( Pointer == NULL )
    {
        RsWarningMessage(RWSTRING("Cannot create camera pointer."));
    }


#ifdef RWMETRICS
    RsMetricsOpen(Camera);
#endif /* RWMETRICS */

    /**********************
     * Initialise dynamics 
     **********************/
    meWorld = MdtWorldCreate(10, 200);

  	MdtWorldSetGravity(meWorld,Gravity.x,Gravity.y,Gravity.z);

    /**********************
     * Initialise collision 
     **********************/

    /* # geoTypes = #primitives + convex + bsp */
    geoTypeMaxCount = McdPrimitivesGetTypeCount() + 2;


    McdInit( geoTypeMaxCount, 100 );
    McdPrimitivesRegisterTypes();
    McdPrimitivesRegisterInteractions();
    McdRwBSPRegisterType();
    McdRwBSPPrimitivesRegisterInteractions();


    /* ------- create Mcd objects ------------- */

    space = McdSpaceAxisSortCreate(McdAllAxes,50,100,1);

    bridge = MstBridgeCreate(10);
    MstSetWorldHandlers(meWorld);

    material1 = MstBridgeGetDefaultMaterial();
    material2 = MstBridgeGetNewMaterial(bridge);

    params = MstBridgeGetContactParams(bridge,material1,material1);
    params->restitution = 1.0; 
    params->type = MdtContactTypeFriction2D;
    params->options |= MdtBclContactOptionBounce;

    params = MstBridgeGetContactParams(bridge,material1,material2);
    params->restitution = 0.5f;
    params->type = MdtContactTypeFriction2D;
    params->options |= MdtBclContactOptionBounce;

    table = McdModelCreate(McdRwBSPCreate(World));
    McdSpaceInsertModel(space,table);
    McdModelSetMaterial( table, material2 );

    McdSpaceUpdateModel(table);
    McdSpaceFreezeModel(table);

    pathname = RsPathnameCreate(RWSTRING(".\\models\\wheel.dff"));
    ballClump = CreateClump(pathname);
    if( !ballClump ) {
        RsErrorMessage(RWSTRING("Cannot create ball."));
        return(FALSE);
    }
    RsPathnameDestroy(pathname);

    ballBody = MdtBodyCreate( meWorld );
     
    MdtBodyEnable(ballBody);
    ball = McdModelCreate(McdSphereCreate((MeReal)0.717));
    McdSpaceInsertModel(space,ball);
    McdModelSetMaterial( ball, material1 );
    McdModelSetBody(ball, ballBody );

    MdtBodySetPosition(ballBody,Worigin->x+2,Worigin->y+10,Worigin->z);
    MdtBodySetAngularVelocity(ballBody,5,0,0);
    MdtBodySetLinearVelocity(ballBody,-5,0,2);

    pathname = RsPathnameCreate(RWSTRING(".\\models\\die.dff"));
    dieClump = CreateClump(pathname);
    if( !dieClump ) {
        RsErrorMessage(RWSTRING("Cannot create die."));
        return(FALSE);
    }
    RsPathnameDestroy(pathname);

    dieBody = MdtBodyCreate( meWorld );

    MdtBodyEnable(dieBody);

    /* DIE_COLLIDES_LIKE_TWO_SPHERES doesn't work at present */

#ifdef DIE_COLLIDES_LIKE_TWO_SPHERES 
    die = McdModelCreate(McdSphereCreate(0.5));
    McdSpaceInsertModel(space,die);
    McdModelSetMaterial( die, material1 );
    McdModelSetBody(die,dieBody);
    McdDtBridgeSetRelativeTransformToBodyPtr(die,mem1);
    McdModelSetTransformPtr(die,mem2);
    McdDtBridgeGetRelativeTransformToBodyPtr(die)[3][2] = 0.5;
    die = McdModelCreate(McdSphereCreate(0.5));
    McdSpaceInsertModel(space,die);
    McdModelSetMaterial( die, material1 );
    McdModelSetBody(die,dieBody);
    McdDtBridgeSetRelativeTransformToBodyPtr(die,mem3);
    McdModelSetTransformPtr(die,mem4);
    McdDtBridgeGetRelativeTransformToBodyPtr(die)[3][2] = -0.5;
#else
    die = McdModelCreate(McdBoxCreate(1,1,1));
    McdSpaceInsertModel(space,die);
    McdModelSetMaterial( die, material1 );
    McdModelSetBody(die, dieBody );
#endif

/*    McdSpaceDisablePair(die,ball); */

/*    McdIntersectSetMaxContactCount(0,0,3); */

    MdtBodySetPosition(dieBody,Worigin->x-2,Worigin->y+10,Worigin->z);
    MdtBodySetAngularVelocity(dieBody,3,0,3);
    MdtBodySetLinearVelocity(dieBody,5,0,2);

    UpdateTMs();

    return TRUE;
}



/*
 *****************************************************************************
 */
static RwBool
Initialise(void)
{
    /* 
     * Application can override any of the skeleton default values here...
     */
    if( RsInitialise() )
    {
        if (!RsGlobal.maximumWidth)
        {
            RsGlobal.maximumWidth = CAMERAWIDTH;
        }
        if (!RsGlobal.maximumHeight)
        {
            RsGlobal.maximumHeight = CAMERAHEIGHT;
        }

        /* 
         * Initialize the timers...
         */
        lastFrameTime = lastAnimTime = RsTimer();

        return TRUE;
    }

    return FALSE;
}



/*
 *****************************************************************************
 */
static RwBool 
AttachPlugins(void)
{
#ifdef RWLOGO
    if( !RpLogoPluginAttach() )
    {
        RsErrorMessage(RWSTRING("RpLogoPluginAttach failed."));
        
        return FALSE;
    }
#endif

    if( !RpWorldPluginAttach() )
    {
        RsErrorMessage(RWSTRING("RpWorldPluginAttach failed."));
        
        return FALSE;
    }

#if (defined(SKY) && !defined(PIPELINE2))
    RwSkyInitWorldPipeline();
#endif /* (defined(SKY) && !defined(PIPELINE2)) */

    if( !RpRandomPluginAttach() )
    {
        RsErrorMessage(RWSTRING("RpRandomPluginAttach failed."));
        
        return FALSE;
    }

    return TRUE;
}



/*
 *****************************************************************************
 */
static RsEventStatus 
HandleKeyDown(void *param)
{
    RsKeyStatus *keyStatus;

    keyStatus = (RsKeyStatus *)param;

    switch( keyStatus->keyCharCode )
    {
        case 'g':
        case 'G':
            GravityOn = !GravityOn;
            
            break;

        case 'n':
        case 'N':
            DiceRolling = FALSE;

            AdvanceSimulation(MAXTIMESTEP * (RwReal)(0.001));
            
            break;

        case 'r':
        case 'R':
            InitializeDice();
            
            break;

        case 'd':
        case 'D':
            ScreenInfoOn = !ScreenInfoOn;
                        
            break;

        case 'v':
        case 'V':
            ViewIndex = (++ViewIndex) % NUMOFVIEWS;
            CameraSetView(Camera, PresetViews[ViewIndex]);
            CurrentView = PresetViews[ViewIndex];
                        
            break;

        case rsESC:
            RsEventHandler(rsQUITAPP, NULL);
            
            break;
    }

    return rsEVENTPROCESSED;
}



/*
 *****************************************************************************
 */
static RsEventStatus
HandleLeftButtonDown(void *param)
{
    RsMouseStatus *mouseStatus;

    mouseStatus = (RsMouseStatus *)param;

    if( mouseStatus->control && mouseStatus->shift )
    {
        MouseMoveMode = MMNoAction;
    }
    else if( mouseStatus->control )
    {
        MouseMoveMode = MMCameraPoint;

        CameraPointing = TRUE;
    }
    else if( mouseStatus->shift ) 
    {
        MouseMoveMode = MMCameraZoomLens;

        CameraZooming = TRUE;
    }
    else
    {
        SelectPointer(&mouseStatus->pos);
        
        MouseMoveMode = MMNoAction;
    }

    return rsEVENTPROCESSED;
}



/*
 *****************************************************************************
 */
static RsEventStatus
HandleLeftButtonUp(void)
{
    MouseMoveMode = MMNoAction;

    CameraPointing = FALSE;
    CameraZooming = FALSE;

    return rsEVENTPROCESSED;
}



/*
 *****************************************************************************
 */
static RsEventStatus
HandleRightButtonDown(void *param)
{
    RsMouseStatus *mouseStatus;

    mouseStatus = (RsMouseStatus *)param;

    if( mouseStatus->control && mouseStatus->shift )
    {
        MouseMoveMode = MMNoAction;
    }
    else if( mouseStatus->control )
    {
        MouseMoveMode = MMCameraAdvance;

        CameraTracking = TRUE;
    }
    else if( mouseStatus->shift ) 
    {
        MouseMoveMode = MMNoAction;
    }
    else
    {
        StartRoll((RwInt32)mouseStatus->pos.x, (RwInt32)mouseStatus->pos.y);
        
        MouseMoveMode = MMNoAction;
    }

    return rsEVENTPROCESSED;
}



/*
 *****************************************************************************
 */
static RsEventStatus
HandleRightButtonUp(void)
{
    MouseMoveMode = MMNoAction;

    CameraTracking = FALSE;

    return rsEVENTPROCESSED;
}



/*
 *****************************************************************************
 */
static RsEventStatus 
HandleMouseMove(void *param)
{
    RsMouseStatus *mouseStatus;

    mouseStatus = (RsMouseStatus *)param;

    switch( MouseMoveMode )
    {
        case MMCameraPoint:
        {
            RwReal deltaEl, deltaAz;

            deltaEl = -mouseStatus->delta.y * (RwReal)(0.1);
            deltaAz = -mouseStatus->delta.x * (RwReal)(0.1);

            CameraPoint(deltaEl, deltaAz);

            break;
        }

        case MMCameraAdvance:
        {
            RwReal distance;

            distance = -mouseStatus->delta.y * (RwReal)(0.1);

            CameraAdvance(distance);

            break;
        }

        case MMCameraZoomLens:
        {
            RwReal delta;

            delta = -mouseStatus->delta.y * (RwReal)(0.002);

            CameraZoomLens(delta);

            break;
        }

        default:
        {
            MovePointer((RwInt32)mouseStatus->pos.x, (RwInt32)mouseStatus->pos.y);
            
            break;
        }
    }

    return rsEVENTPROCESSED;
}


/*
 *****************************************************************************
 */
RsEventStatus 
KeyboardHandler(RsEvent event, void *param)
{
    switch( event )
    {
        case rsKEYDOWN:
        {
            return HandleKeyDown(param);
        }

        default:
            return rsEVENTNOTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
RsEventStatus 
MouseHandler(RsEvent event, void *param)
{
    /* 
     * Simulate a mouse on pad platforms...
     */
    PadMouse(event, param);

    switch( event )
    {
        case rsLEFTBUTTONDOWN:
        {
            return HandleLeftButtonDown(param);
        }

        case rsLEFTBUTTONUP:
        {
            return HandleLeftButtonUp();
        }

        case rsRIGHTBUTTONDOWN:
        {
            return HandleRightButtonDown(param);
        }

        case rsRIGHTBUTTONUP:
        {
            return HandleRightButtonUp();
        }

        case rsMOUSEMOVE:
        {
            return HandleMouseMove(param);
        }

        default:
            return rsEVENTNOTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
RsEventStatus 
PadHandler(RsEvent event, void *param)
{
    switch( event )
    {
        case rsPADBUTTONDOWN:
        {
            RsKeyStatus ks;
            RsPadButtonStatus *pb;

            pb = (RsPadButtonStatus *)param;

            if( pb->padButtons & rsPADBUTTON1 )
            {
                ks.keyCharCode = 'd';

                RsKeyboardEventHandler(rsKEYDOWN, &ks);
            }

            if( pb->padButtons & rsPADBUTTON2 )
            {
                ks.keyCharCode = 'p';

                RsKeyboardEventHandler(rsKEYDOWN, &ks);
            }

            if( pb->padButtons & rsPADBUTTON3 )
            {
                ks.keyCharCode = 'v';

                RsKeyboardEventHandler(rsKEYDOWN, &ks);
            }

            return rsEVENTPROCESSED;
        }

        case rsPADBUTTONUP:
        {
            RsKeyStatus ks;
            RsPadButtonStatus *pb;

            pb = (RsPadButtonStatus *)param;

            if( pb->padButtons & rsPADBUTTON1 )
            {
                ks.keyCharCode = 'd';

                RsKeyboardEventHandler(rsKEYUP, &ks);
            }

            if( pb->padButtons & rsPADBUTTON2 )
            {
                ks.keyCharCode = 'p';

                RsKeyboardEventHandler(rsKEYUP, &ks);
            }

            if( pb->padButtons & rsPADBUTTON3 )
            {
                ks.keyCharCode = 'v';

                RsKeyboardEventHandler(rsKEYUP, &ks);
            }

            return rsEVENTPROCESSED;
        }

        default:
            return rsEVENTNOTPROCESSED;
    }

    return rsEVENTNOTPROCESSED;
}


/*
 *****************************************************************************
 */
static RwBool
AttachInputDevices(void)
{
    RsInputDeviceAttach(rsKEYBOARD, KeyboardHandler);
    
    RsInputDeviceAttach(rsMOUSE, MouseHandler);
    
    RsInputDeviceAttach(rsPAD, PadHandler);

    return TRUE;
}

/*
 *****************************************************************************
 */
static void
HandleIdle(void)
{
    RwUInt32 thisFrameTime, thisAnimTime;

    thisFrameTime = thisAnimTime = RsTimer();

    RunSimulation(50);

    lastAnimTime = thisAnimTime;

    /* 
     * Has a second elapsed since we last updated the FPS...
     */
    if( thisFrameTime > (lastFrameTime + 1000) )
    {
        /* 
         * Capture the frame counter...
         */
        FramesPerSecond = FrameCounter;
        
        FrameCounter = 0;
        
        lastFrameTime = thisFrameTime;
    }

    Render();

    return;
}

/*
 *****************************************************************************
 */
RsEventStatus
AppEventHandler(RsEvent event, void *param)
{
    switch( event )
    {
        case rsINITIALISE:
        {
            return Initialise() ? rsEVENTPROCESSED : rsEVENTERROR;
        }

        case rsCAMERASIZE:
        {
            CameraSize(Camera, param, CurrentViewWindow, DEFAULT_ASPECTRATIO);
            
            return rsEVENTPROCESSED;
        }

        case rsRWINITIALISE:
        {
            return Initialize3D(param) ? rsEVENTPROCESSED : rsEVENTERROR;
        }

        case rsRWTERMINATE:
        {
            Terminate3D();
            
            return rsEVENTPROCESSED;
        }

        case rsPLUGINATTACH:
        {
            return AttachPlugins() ? rsEVENTPROCESSED : rsEVENTERROR;
        }

        case rsINPUTDEVICEATTACH:
        {
            AttachInputDevices();

            return rsEVENTPROCESSED;
        }

        case rsIDLE:
        {
            HandleIdle();
            
            return rsEVENTPROCESSED;
        }

        default:
            return rsEVENTNOTPROCESSED;
    }

}

/*
 *****************************************************************************
 */


