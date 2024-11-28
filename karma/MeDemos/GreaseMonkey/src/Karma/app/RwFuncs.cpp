/***********************************************************************************************
*
*   $Id: RwFuncs.cpp,v 1.1.2.2 2002/03/15 12:49:20 richardm Exp $
*
************************************************************************************************/

#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include "rwcore.h"
#include "rpworld.h"
#include "rtworld.h"
#include "rtcharse.h"
//#include "rpvrml.h"
#include "rpcollis.h"
#ifdef _DEBUG_WINDOW
#include "rt2d.h"
#endif
#include "skeleton.h"
#include "metrics.h"

#include "platxtra.h"
#include "camera.h"
#include "RwFuncs.hpp"

#ifdef _MEEEDEVELOP
#include <stdio.h>
#endif

//#include "carAPI.hpp"
#include "McdFrame.h"

#include "MdtCar.hpp"
#include "carAPI.hpp"
#include "utils.hpp"
#include "car.hpp"
#include "driver.hpp"
#include "smoke.hpp"
#include "carsound.hpp"
#include "control.hpp"

#include "MeMemory.h"

#define INITHFOV 60.0
#define DegToRad(x) ((x) * rwPI / 180.0f)
//#define FAR_CLIP_PLANE (500.0f)
//#define NEAR_CLIP_PLANE (0.5f)

RpWorld  *World = (RpWorld *)NULL;
RwV3d Worigin;
RwBBox world_bbox;
//RwMatrix *cam_mtx = NULL;
//RwFrame *cam_frame = NULL;

static RwReal cam_x = 0, cam_z = 0;

extern Driver* player;
extern int safe_time_on;

//test
//RwRaster *test_texture_raster = NULL;
//RwTexture *test_texture = NULL;
//RwCamera *test_camera;
#ifdef _DEBUG_WINDOW
RwCamera *debug_camera = 0;
Rt2dBrush *Brush =0;
Rt2dBBox WinBBox;
Rt2dFont *debug_font = 0;
#endif

/*****************************************************************************
 *
 * Local global variables...
 *
 ****************************************************************************/

static RwBool EngineInitialized;
static RwBool EngineOpened;
static RwBool EngineStarted;


//static RpLight *AmbientLight = NULL;
//RpLight *MainLight = NULL;

RwRaster *Charset = (RwRaster *)NULL;

static RwV3d Xaxis = {1.0f, 0.0f, 0.0f};
static RwV3d Yaxis = {0.0f, 1.0f, 0.0f};
static RwV3d Zaxis = {0.0f, 0.0f, 1.0f};
static RwV3d Zero = {0.0f, 0.0f, 0.0f};

RwRGBA col_WHITE = {255, 255, 255, 255};
RwRGBA col_RED = {255, 0, 0, 255};
RwRGBA col_GREEN = { 0, 255, 0, 255};
RwRGBA col_BLUE = { 0, 0, 255, 255};
RwRGBA col_YELLOW = { 200, 150, 0, 255};
//RwRGBA BackgroundColor = {128, 128, 128,   0};
RwRGBA BackgroundColor = {255, 255, 255, 0};

/*****************************************************************************
 *
 * Externally global variables...
 *
 ****************************************************************************/

//RwCamera *Camera = NULL; //, *shot_cam = NULL, *side_cam = NULL, *hoop_cam = NULL;
//RpClump *ball_clump = NULL;
//RpClump *arrow_clump = NULL;

//RwBool ClumpLoaded = FALSE;

RwInt32 ScreenInfoOn = 0;

RwInt32 FrameCounter = 0;
RwInt32 FramesPerSecond = 0;

/***********************************************************************************************
*
*   Destroy all clumps callback
*
************************************************************************************************/
static RpClump *
_destroyClumpCB(RpClump *c, void *data)
{
    RpWorldRemoveClump(World, c);
    RpClumpDestroy(c);

    return c; /* messy */
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void SetClumpPos(RpClump *clump, RwReal x, RwReal y, RwReal z)
{
    RwFrame *frame;
    RwV3d pos;

    frame = RpClumpGetFrame(clump);

    pos.x = x; pos.y = y; pos.z = z;

    RwFrameTranslate(frame, &pos, rwCOMBINEREPLACE);

    RwFrameTranslate(frame, &Worigin, rwCOMBINEPOSTCONCAT);

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void SetClumpAng(RpClump *clump, RwReal yaw, RwReal pitch)
{
    RwFrame *frame;
    RwV3d pos;

    frame = RpClumpGetFrame(clump);

    pos = *RwMatrixGetPos(RwFrameGetMatrix(frame)); //store position

    RwFrameSetIdentity(frame);

    RwFrameRotate(frame, &Zaxis, pitch, rwCOMBINEREPLACE);
    RwFrameRotate(frame, &Yaxis, yaw, rwCOMBINEPOSTCONCAT);

    RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT ); //put back in position

}
/******************************************************************************
*
*
*
********************************************************************************/
static RpLight *
CreateAmbientLight(RpWorld *world)
{
    RpLight *light;
    RwRGBAReal color;

    color.red = color.green = color.blue = 0.0; color.alpha = 1.0f;

    light = RpLightCreate(rpLIGHTAMBIENT);

    if( light )
    {
        /*
         * Add it to the world so it will illuminate geometry...
         */
        RpWorldAddLight(world, light);

        RpLightSetColor(light, &color );

        return light;
    }

    return (RpLight *)NULL;
}


/******************************************************************************
*
*
*
*******************************************************************************/
void LightSetPos(RpLight *light, RwReal x, RwReal y, RwReal z)
{
    RwFrame *frame;
    RwV3d   pos;

    frame = RpLightGetFrame(light);

    if(frame) {
        /* First translate back to the origin... */
        pos = *RwMatrixGetPos(RwFrameGetMatrix(frame));
        RwV3dScale(&pos, &pos, -1.0f);
        RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);

        /* Set new position */
        pos = Worigin;
        pos.x += x; pos.y += y; pos.z += z;
        RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT );
    }
}

/******************************************************************************
*
*
*
*******************************************************************************/
void LightSetDegAngles(RpLight *light, RwReal yaw, RwReal pitch)
{
    RwFrame *frame;
    RwV3d   pos;

    frame = RpLightGetFrame(light);
    if(frame) {

        pos = *RwMatrixGetPos(RwFrameGetMatrix(frame)); //store position

        RwFrameSetIdentity(frame);                      //put light back at origin and zero rotations

        RwFrameRotate(frame, &Xaxis, pitch, rwCOMBINEREPLACE); //rotate light
        RwFrameRotate(frame, &Yaxis, yaw, rwCOMBINEPOSTCONCAT); //rotate light

        RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT ); //put light back in position
    }
}

/******************************************************************************
*
*
*
*******************************************************************************/
RpLight *
CreateLight(RpWorld *world, RwInt32 type, RwV3d *position, RwReal direction, RwReal pitch, RwRGBAReal *colour, RwReal radius , RwReal cone, RwInt32 flags)
{
    RpLight *light;

    /*
     * Create a directional light of the default color/intensity...
     */


    light = RpLightCreate(type);

    if( light )
    {
        RwFrame *frame;

        RpLightSetColor(light, colour );
        RpLightSetRadius(light, radius );
        RpLightSetConeAngle(light, cone);
            RpLightSetFlags(light, flags);

        frame = RwFrameCreate();

        if( frame )
        {
            /*
             * Attach the light to the frame...
             */
            RpLightSetFrame(light, frame);

            /*
             * Add light to world so it will illuminate geometry...
             */
            RpWorldAddLight(world, light);

            /* Position Light */
            RwFrameTranslate(frame, &Worigin, rwCOMBINEREPLACE);
            RwFrameTranslate(frame, position, rwCOMBINEPOSTCONCAT);

            /* Orientate Light */
            RwFrameRotate(frame, RwMatrixGetUp(RwFrameGetMatrix(frame)), direction, rwCOMBINEPRECONCAT);
            RwFrameRotate(frame, RwMatrixGetRight(RwFrameGetMatrix(frame)), pitch, rwCOMBINEPRECONCAT);


            return light;
        }

        RpLightDestroy(light);
    }

    return (RpLight *)NULL;
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
void CameraSetViewFrustrum(RwCamera *camera, RwReal fov,
                                        RwReal aspect_ratio,
                                        RwReal near_clip,
                                        RwReal far_clip)
{
    RwV2d w;

    /*
     * Set the camera's view window so that it gives
     * a horizontal field-of-view of INITHFOV degrees...
     */
    w.x = (RwReal)tan(DegToRad(fov / 2.0));
    w.y = w.x * aspect_ratio;

    RwCameraSetViewWindow(camera, &w);

    /* Set the clipping planes  */
    RwCameraSetFarClipPlane(camera, far_clip);
    RwCameraSetNearClipPlane(camera, near_clip);

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void CameraSetPos(RwCamera *camera, RwReal x, RwReal y, RwReal z)
{
    RwFrame *cameraFrame;
    RwV3d pos;

    //get camera frame
    cameraFrame = RwCameraGetFrame(camera);

    if(cameraFrame)
    {
        /* First translate back to the origin... */
        pos = *RwMatrixGetPos(RwFrameGetMatrix(cameraFrame));
        RwV3dScale(&pos, &pos, -1.0f);
        RwFrameTranslate(cameraFrame, &pos, rwCOMBINEPOSTCONCAT);

        cam_x = x; cam_z = z; //for debugging

        /* Set new position */
        pos = Worigin;
        pos.x += x; pos.y += y; pos.z += z;
        RwFrameTranslate(cameraFrame, &pos, rwCOMBINEPOSTCONCAT);

    }
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
void CameraSetDegAngles(RwCamera *camera, RwReal yaw, RwReal pitch, RwReal roll)
{
    RwFrame *cameraFrame;
    RwMatrix *matrix;
    RwV3d up, at, right, pos;

    /* Get camera frame */
    cameraFrame = RwCameraGetFrame(camera);

    /* get camera rotation matrix   */
    matrix = RwFrameGetMatrix(cameraFrame);

    /* Move camera back to origin   */
    pos = *RwMatrixGetPos(matrix);
    RwFrameSetIdentity(cameraFrame);

    /* Rotate yaw */
    up = *RwMatrixGetUp(matrix);
    RwFrameRotate(cameraFrame, &up, yaw, rwCOMBINEPOSTCONCAT);

    /* Rotate Pitch */
    right = *RwMatrixGetRight(matrix);
    RwFrameRotate(cameraFrame, &right, pitch, rwCOMBINEPOSTCONCAT);

    /* Rotate roll  */
    at = *RwMatrixGetAt(matrix);
    RwFrameRotate(cameraFrame, &at, roll, rwCOMBINEPOSTCONCAT);

    /* move camera to original location */
    RwFrameTranslate(cameraFrame, &pos, rwCOMBINEPOSTCONCAT);

}

/*
 *****************************************************************************
 */
static RwCamera *
CreateCamera(RpWorld *world, int width, int height)
{
    RwCamera *camera;

    /*
     * Create a camera...
     */

    camera = RwCameraCreate();

    if( camera )
    {
        /*
         * ...and frame...
         */
        RwFrame *frame;

        frame = RwFrameCreate();

        if( frame )
        {
            RwRaster *frameBuffer;

            /*
             * Create the camera's frame buffer raster with
             * default device depth...
             */
            frameBuffer = RwRasterCreate(width,height, 0, rwRASTERTYPECAMERA);

            if( frameBuffer )
            {
                RwRaster *zBuffer;

                /*
                 * Create the camera's z-buffer raster with
                 * the default device depth...
                 */
                zBuffer = RwRasterCreate(width,height, 0, rwRASTERTYPEZBUFFER);

                if( zBuffer )
                {
                    /*
                     * Attach the frame buffer and z-buffer rasters to
                     * the camera...
                     */
                    RwCameraSetZRaster(camera, zBuffer);
                    RwCameraSetRaster(camera, frameBuffer);

                    /*
                     * Attach the camera to the frame...
                     */
                    RwCameraSetFrame(camera, frame);

                    /*
                     * Add the camera to the world so that it can
                     * render it...
                     */
                    RpWorldAddCamera(world, camera);

//                  cam_frame   = frame;
//                  cam_mtx     = RwFrameGetMatrix(cam_frame);

                    return camera;
                }

                RwRasterDestroy(frameBuffer);
            }

            RwFrameDestroy(frame);
        }

        RwCameraDestroy(camera);
    }

    return (RwCamera *) NULL;

}

/******************************************************************************
*
*
*
********************************************************************************/
static char *FloatToString(RwReal num, char *buffer)
{
  RwReal mul = 1;

  int temp1 = (int)num, temp2;

  temp2 = (int)(num*100) - (temp1*100); //2 decimal places

  sprintf(buffer,"%d.%d",temp1,temp2);

  return(buffer);
}


/******************************************************************************
*
*
*
********************************************************************************/
static void
DisplayOnScreenInfo(RwCamera *camera, RwRaster *cameraRaster)
{
    RwChar caption[128];
    int line;

    RwReal avg_ftime =0;
    RwInt32 rpc = 0,cpc=0,dpc=0;

    if(FramesPerSecond > 0) {
        avg_ftime = 1.0f/FramesPerSecond;
        rpc = (int)(0.1f*render_time/avg_ftime);
        cpc = (int)(0.1f*collision_time/avg_ftime);
        dpc = (int)(0.1f*dynamics_time/avg_ftime);
    }

    {
#ifdef PERFORMANCE_METRICS

    #ifdef PS2
        RwChar buf1[10],buf2[10],buf3[10];
          FloatToString(dynamics_time,buf1);
          FloatToString(collision_time,buf2);
          FloatToString(render_time,buf3);
          rwsprintf(caption, RWSTRING(" Render: %sms\tCollision: %sms\tPhysics: %sms"),buf3,buf2,buf1);
    #else
          rwsprintf(caption, RWSTRING("FPS: %d Render: %d%c(%2.2fms)\tCollision: %d%c(%2.2fms)\tPhysics: %d%c(%2.2fms)"), FramesPerSecond,rpc,'%',render_time,cpc,'%',collision_time,dpc,'%',dynamics_time);
    #endif
          RtCharsetPrint(Charset, caption, 10, 10);
#endif //PERFORMANCE_METRICS

#ifdef _MECHECK
            RwV2d v;
            drivers[game_cam.view_driver]->Get2DPosition(&v);
            rwsprintf(caption, RWSTRING("Car X = %4.2f, Y = %4.2f, POINT = %d"),v.x,v.y,drivers[game_cam.view_driver]->GetPointID() );
            RtCharsetPrint(Charset, caption, 10, 25);
    #else
        #ifdef PS2
            if(!ScreenInfoOn)
            {
                rwsprintf(caption, RWSTRING("Press L1 for Pad Controls"));
                RtCharsetPrint(Charset, caption, 10, 25);
            }
        #else
            #ifdef _XBOX
                if(!ScreenInfoOn)
                {
                   rwsprintf(caption, RWSTRING("Press Black button for Pad Controls"));
                 RtCharsetPrint(Charset, caption, 10, 25);
                }
            #else
                if(!ScreenInfoOn)
                  {
                    rwsprintf(caption, RWSTRING("Press F1 for Key Information"));
                    RtCharsetPrint(Charset, caption, 10, 25);
                  }
            #endif
        #endif
#endif

#ifndef PS2
            rwsprintf(caption, RWSTRING("Race Best %3.2f S"),drivers[0]->race_best );
            RtCharsetPrint(Charset, caption, 10, RsGlobal.maximumHeight-61);
            rwsprintf(caption, RWSTRING("Best Time %3.2f S"),drivers[game_cam.view_driver]->GetBestTime() );
            RtCharsetPrint(Charset, caption, 10, RsGlobal.maximumHeight-50);
            rwsprintf(caption, RWSTRING("Lap Time %3.2f S"),drivers[game_cam.view_driver]->GetLapTime() );
            RtCharsetPrint(Charset, caption, 10, RsGlobal.maximumHeight-39);
#endif
        }

#ifdef PS2
        if(ScreenInfoOn)
        {
            line = 25;
            rwsprintf(caption, RWSTRING("L ANALOGUE L/R => Steering"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
        //rwsprintf(caption, RWSTRING("R ANALOGUE U/D => Throtte / Brakes"));
            //          RtCharsetPrint(Charset, caption, 20, 36);
            rwsprintf(caption, RWSTRING("L ANALOGUE U/D => SHIFT GEAR U/D"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("R1 => Toggle PAUSE"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("R2 => Reset"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("L1 => Toggle Pad Info"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("L2 => Toggle Player Control of viewed car"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("TRIANGLE => Jump to next car"));
            RtCharsetPrint(Charset, caption, 20, line);  line+=11;
            rwsprintf(caption, RWSTRING("SQUARE => Brake"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("CROSS => Accelerate"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("CIRCLE => Toggle Dynamics level of Detail"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;

            rwsprintf(caption, RWSTRING("UP => Drivers View"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("DOWN => Towed View"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("LEFT => Spectator View"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("RIGHT => Drive By View"));
            RtCharsetPrint(Charset, caption, 20, line);  line+=11;
        }
#endif
#ifdef _XBOX
        if(ScreenInfoOn)
        {
            line = 25;
            rwsprintf(caption, RWSTRING("Left Stick => Steering"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("Left Trigger => Toggle PAUSE"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("Right Trigger => Reset"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("Black => Toggle Pad Info"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("White => Toggle Player Control of viewed car"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("A => Jump to next car"));
            RtCharsetPrint(Charset, caption, 20, line);  line+=11;
            rwsprintf(caption, RWSTRING("X => Brake"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("B => Accelerate"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("Y => Toggle Dynamics level of Detail"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;

            rwsprintf(caption, RWSTRING("D-Pad UP => Drivers View"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("D-Pad DOWN => Towed View"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("D-Pad LEFT => Spectator View"));
            RtCharsetPrint(Charset, caption, 20, line); line+=11;
            rwsprintf(caption, RWSTRING("D-Pad RIGHT => Drive By View"));
            RtCharsetPrint(Charset, caption, 20, line);  line+=11;
        }

#else

        if(ScreenInfoOn)
        {
            line = 25;
            if(input_control_method == CTRL_KEYBOARD)
            {
                rwsprintf(caption, RWSTRING("L/R ARROW => Steering"));
                RtCharsetPrint(Charset, caption, 10, line); line+=11;
                rwsprintf(caption, RWSTRING("U/D ARROW => Throttle / Brakes"));
                RtCharsetPrint(Charset, caption, 10, line);line+=11;
            }
            if(input_control_method == CTRL_MOUSE)
            {
                rwsprintf(caption, RWSTRING("L/R MOUSE => Steering"));
                RtCharsetPrint(Charset, caption, 10, line);line+=11;
                rwsprintf(caption, RWSTRING("L MOUSE BUTTON => Throttle"));
                RtCharsetPrint(Charset, caption, 10, line);line+=11;
                rwsprintf(caption, RWSTRING("R MOUSE BUTTON => Brake"));
                RtCharsetPrint(Charset, caption, 10, line);line+=11;
            }
            rwsprintf(caption, RWSTRING("1 => FIXED VIEW (Cycle through)"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("2 => DRIVE-BY VIEW"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("3 => SPECTATOR VIEW"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("4 => TOWED VIEW"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("5 => HELICOPTER VIEW"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("6 => PLAYER DRIVER VIEW"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("CTRL+L/R/U/D ARROW => Camera move in heli view"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("SHIFT+U/D ARROW => Camera zoom in heli view"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("P  => Toggle PAUSE"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("TAB  => Cycle through cars"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("B  => Return to Players car"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("R  => RESET viewed vehicle"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("F1  => Toggle INFO"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("X  => SHIFT DOWN (Wheel R Paddle)"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("SPACE  => SHIFT UP (Wheel L Paddle)"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("T => Toggle Auto/Manual transmission"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("ENTER => Toggle Player Control of Viewed Car"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
            rwsprintf(caption, RWSTRING("D => Toggle Dynamics level of Detail"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
			rwsprintf(caption, RWSTRING("S => Toggle SafeTime Collision Detection"));
            RtCharsetPrint(Charset, caption, 10, line);line+=11;
        }
#endif

        rwsprintf(caption, RWSTRING("%d MPH"),(RwInt32)(speedo.val));
        RtCharsetPrint(Charset, caption, speedo.centre[0]-20, speedo.centre[1]);

        RwInt32 gear = drivers[game_cam.view_driver]->GetCar()->Gear();
        rwsprintf(caption, gear > 0 ? RWSTRING("G %d") : RWSTRING("REV"), gear);
        RtCharsetPrint(Charset, caption, tacho.centre[0]-20, tacho.centre[1]-5);

        RwInt32 tranny = drivers[game_cam.view_driver]->GetCar()->TrannyType();
        rwsprintf(caption, tranny == 0 ? RWSTRING("MAN") : RWSTRING("AUTO"));
        RtCharsetPrint(Charset, caption, tacho.centre[0]-20, tacho.centre[1]+6);

        rwsprintf(caption, drivers[game_cam.view_driver]->GetControlMethod() == PLAYER_CONTROL ? RWSTRING("PLAYER CTRL") : RWSTRING("COMPUTER CTRL") );
        RtCharsetPrint(Charset, caption, RsGlobal.maximumWidth/2 - 35, RsGlobal.maximumHeight-30);
        rwsprintf(caption, ((int)drivers[game_cam.view_driver]->GetCar()->level_of_detail == LOD1 ? RWSTRING("LOD: HIGH") : RWSTRING("LOD: LOW")));
        RtCharsetPrint(Charset, caption, RsGlobal.maximumWidth/2 - 15, RsGlobal.maximumHeight-19);

        rwsprintf(caption, (safe_time_on?RWSTRING("ST ON"):RWSTRING("ST OFF")));
        RtCharsetPrint(Charset, caption, RsGlobal.maximumWidth - 55, 10);

#if _DEBUG
        {
            extern int frameno;
            static char framenocap[32] = "00000001";

            if ((frameno % 10) == 0)
                rwsprintf(framenocap, "FRAME %08d",frameno);
            RtCharsetPrint(Charset, framenocap, RsGlobal.maximumWidth - 120, 20);
        }
#endif

        if(paused)
        {
                rwsprintf(caption, RWSTRING("PAUSED"));
                RtCharsetPrint(Charset, caption, RsGlobal.maximumWidth/2 - 20, RsGlobal.maximumHeight/2);
        }
}
/****************************************/
//Draw lines between polygon vertices
/****************************************/
/*
RpWorld *_WorldPolygonCB( RpWorld *world, RpWorldSector *worldSector, RwInt32 index, RpWorldVertex *worldVertex, RpMaterial *material, void *data)
{
    RWIM3DVERTEX points[3];

    RWIM3DVERTEXSetPos (&points[0],worldVertex[0].vOC.x,worldVertex[0].vOC.y,worldVertex[0].vOC.z);
    RWIM3DVERTEXSetRGBA(&points[0], 0,0,0,255);
    RWIM3DVERTEXSetPos (&points[1],worldVertex[1].vOC.x,worldVertex[1].vOC.y,worldVertex[1].vOC.z);
    RWIM3DVERTEXSetRGBA(&points[1], 0,0,0,255);
    RWIM3DVERTEXSetPos (&points[2],worldVertex[2].vOC.x,worldVertex[2].vOC.y,worldVertex[2].vOC.z);
    RWIM3DVERTEXSetRGBA(&points[2], 0,0,0,255);

    if (RwIm3DTransform(points, 3, (RwMatrix *)NULL, 0))
    {

        RwIm3DRenderLine(0, 1);
        RwIm3DRenderLine(1, 2);
        RwIm3DRenderLine(2, 0);

        RwIm3DEnd();
    }

    return world;
}
*/
/******************************************************************************
*
*
*
*******************************************************************************/
void RenderDebugWindow(void)
{
#ifdef _DEBUG_WINDOW
    Rt2dPath *path;
    int i;
    RwReal x_pos;
//  RwV2d text_anchor;

    Rt2dCTMPush();
    Rt2dCTMSetIdentity();
    Rt2dCTMScale(1.0f, 0.9f);
    path = Rt2dPathCreate();
    for(i=0; i < debug_stream.num_channels; i++)
    {
        DEBUG_CHANNEL *c = &debug_stream.channel[i];
        DEBUG_SAMPLE *s = c->newest;
        x_pos = 1.0f;
        Rt2dPathMoveto(path, x_pos, s->y_val);
//      text_anchor.x = 0.5f; text_anchor.y = s->y_val;
        while(s->prev && x_pos > 0.0f)
        {
            x_pos -= s->delta_x;
            s = s->prev;
            Rt2dPathLineto(path, x_pos, s->y_val);
        }
        Rt2dBrushSetRGBA(Brush, &c->col, &c->col, &c->col, &c->col);
        Rt2dBrushSetWidth(Brush, 0.005f);
        Rt2dPathStroke(path, Brush);
        Rt2dPathEmpty(path);

//      RwReal h = Rt2dFontGetHeight(debug_font);
  //     Rt2dBrushSetRGBA(Brush, &col_RED, &col_RED, &col_RED, &col_RED);
    //  Rt2dBBox bbox;
      //  bbox.x = 0.5f;
//        bbox.y = WinBBox.h-h;
  //      bbox.w = WinBBox.w;
    //    bbox.h = h;
      //  debug_font = Rt2dFontFlow(debug_font, RWSTRING("Paths, Strokes & Fills"), h, &bbox, rt2dJUSTIFYCENTER, Brush);

//      Rt2dFontShow(debug_font, RWSTRING(c->label), 0.1f, &text_anchor, Brush);
    }
    Rt2dPathDestroy(path);
    Rt2dCTMPop();
#endif
}
/*
 *****************************************************************************
 */
void
RenderFrame(void)
{
    /*
     * Renders a single frame to the given device...
     */
    static int i = 0;

    RwRaster *camRas = RwCameraGetRaster(game_cam.cam);

    /*
     * Clear both the camera's frame buffer and z-buffer...
     */
    ResetPerformanceTimer();

    RwCameraClear(game_cam.cam, &BackgroundColor, rwCAMERACLEARZ | rwCAMERACLEARIMAGE);

    if( RwCameraBeginUpdate(game_cam.cam) )
    {

        //RwRenderStateSet(rwRENDERSTATEFOGENABLE, FALSE);
        //RwRenderStateSet(rwRENDERSTATEFOGTYPE, (void*)rwFOGTYPELINEAR);
        //RwRenderStateSet(rwRENDERSTATEFOGCOLOR,(void*)RWRGBALONG(255,255,255,255));

        RpWorldRender(World); // render world and objects
//		RpClumpRender(drivers[0]->car_cmpts[0].clump);
//		RpClumpRender(drivers[0]->car_cmpts[1].clump);
//		RpClumpRender(drivers[0]->car_cmpts[2].clump);
//		RpClumpRender(drivers[0]->car_cmpts[3].clump);
//		RpClumpRender(drivers[0]->car_cmpts[4].clump);

        //RwRenderStateSet(rwRENDERSTATEFOGENABLE, FALSE);
        //RpClumpRender(sky_clump);
        //RtWorldForAllPolygons(World, _WorldPolygonCB, NULL);

        DisplayOnScreenInfo(game_cam.cam, camRas);


#ifdef _MECHECK

        if (RwIm3DTransform(debug_lines.points, debug_lines.num_points, (RwMatrix *)NULL, 0))

        {
            int i;

            for(i = 0; i < debug_lines.num_points; i+=2) RwIm3DRenderLine(i, i+1);

            RwIm3DEnd();
        }
#endif

        SmokeRender();

#if 1
        /* Render meter bars    */
//      RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)test_texture_raster);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);
        RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)FALSE);//z-buffer disabled in order to draw speedometer dials

//      RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, power_bar.corners, 4);
        RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, speedo.corners, 2*NUM_ARC_SEGMENTS + 2, speedo.pt_index, 2*NUM_ARC_SEGMENTS*3);
        RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, tacho.corners, 2*NUM_ARC_SEGMENTS + 2, tacho.pt_index, 2*NUM_ARC_SEGMENTS*3);

#endif
		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)TRUE);

        RwCameraEndUpdate(game_cam.cam);
    }

    if(game_vars.rear_view_mirror && rvm.on)
    {
        RwCameraClear(rvm.cam, &BackgroundColor, rwCAMERACLEARZ);
        if( RwCameraBeginUpdate(rvm.cam) )
        {
            RpWorldRender(World); // render world and objects

            RwRaster *ras;
            RwUInt8 *pixadr;
            RwInt32 stride, width, height, i, j, src_id;
            RwInt32 pixel_size;
            RwInt32 id1, id2, temp;

            ras = RwCameraGetRaster(rvm.cam);
            width = RwRasterGetWidth(ras);
            height = RwRasterGetHeight(ras);
            pixel_size = RwRasterGetDepth(ras)/8; // in bytes

            stride = RwRasterGetStride(ras)/pixel_size; // stride in bytes

            /* Lock both rasters for subsequent operations, obtaining pointers to pixel data */
//OLD_RW            pixadr =(RwUInt8*)RwRasterLock(ras, (_RwRasterLockMode)rwRASTERLOCKREADWRITE);
            pixadr =(RwUInt8*)RwRasterLock(ras, 0, (RwRasterLockMode)(rwRASTERLOCKREAD|rwRASTERLOCKWRITE));

            if(pixadr)
              {
            if(pixel_size == 2)
              {
                RwInt16 *pixels = (RwInt16*)pixadr;
                for(j = 0; j < height; j++)
                {
                    src_id = j*stride;
                    for(i=0; i<width/2;i++)
                    {
                        id1 = src_id + i;
                        id2 = src_id + (width - i -1);
                        temp = pixels[id1];
                        pixels[id1] = pixels[id2];
                        pixels[id2] = (RwUInt16)temp;
                    }
                }
            }
            else if(pixel_size == 4)
            {
                RwInt32 *pixels = (RwInt32*)pixadr;
                for(j = 0; j < height; j++)
                {
                    src_id = j*stride;
                    for(i=0; i<width/2;i++)
                    {
                        id1 = src_id + i;
                        id2 = src_id + (width - i -1);
                        temp = pixels[id1];
                        pixels[id1] = pixels[id2];
                        pixels[id2] = temp;
                    }
                }
            }
              }
            RwRasterUnlock(ras);

            RwCameraEndUpdate(rvm.cam);
        }

    }

#ifdef _DEBUG_WINDOW
    RwCameraClear(debug_camera, &col_BLUE, rwCAMERACLEARIMAGE);

    if( RwCameraBeginUpdate(debug_camera) )
    {
        RenderDebugWindow();
        RwCameraEndUpdate(debug_camera);
    }

    RsCameraShowRaster(debug_camera);

#endif
    /*
     * Copy the rendering to the output device...
     */
#ifdef RWMETRICS
        RsMetricsRender();
#endif

#ifdef PS2
    render_time = ReadPerformanceTimer();
	RsCameraShowRaster(game_cam.cam);
#else
	RsCameraShowRaster(game_cam.cam);
    render_time = ReadPerformanceTimer();
#endif

    FrameCounter++;
}


/******************************************************************************
*
*
*
********************************************************************************/
RpClump *ClumpLoadDFF(RwChar *dffPath)
{
    RwStream *stream; //,*wstream;
    RpClump *clump;
//	RwChar buffer[250];
    /*
     * Textures used by the clump are expected to be found in the
     * folder "textures" relative to where the DFF lives...
     */

    /*
     * Open a stream connected to the disk file...
     */
    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, dffPath);

    if( stream )
    {
        /*
         * Find a clump chunk in the stream...
         */
        if( !RwStreamFindChunk(stream, rwID_CLUMP,
            (RwUInt32 *) NULL, (RwUInt32 *)NULL) )
        {
            RwStreamClose(stream, NULL);

            return (RpClump *)NULL;
        }

        /*
         * Read the clump chunk...
         */
        clump = RpClumpStreamRead(stream);
        RwStreamClose(stream, NULL);

//		sprintf(buffer,"%s_new",dffPath);
//		wstream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, dffPath);
//		RpClumpStreamWrite(clump, wstream);
//		RwStreamClose(wstream, NULL);

        if( clump )
        {
            RpWorldAddClump(World, clump);

            return clump;
        }
    }

    return (RpClump *)NULL;
}

/******************************************************************************
*
*
*
********************************************************************************/
RpClump *LoadDffFile(RwChar *fname)
{
    RpClump *clump = (RpClump *)NULL;

    clump = ClumpLoadDFF(fname);

    if( !clump )
    {
        RwChar errorMessage[100];

        rwsprintf(errorMessage, RWSTRING("Cannot load %s"), fname);

        RsErrorMessage(errorMessage);
    }

    return(clump);
}

/******************************************************************************
*
*
*
********************************************************************************/
RpClump *
ClumpLoadWRL(RwChar *wrlPath, RwReal scale)
{
    RpClump *clump = (RpClump *)NULL;
#ifdef USE_VRML

    /*
     * Read the VRML at the normal scale...
     */
    clump = RpVrmlClumpRead(wrlPath, scale);

    if( !clump )
    {
        RwChar errorMessage[100];

        rwsprintf(errorMessage, RWSTRING("Cannot load %s"), wrlPath);

        RsErrorMessage(errorMessage);
    }
    else
    {
        RpWorldAddClump(World, clump);
    }
#endif

    return clump;

}
/******************************************************************************
*
*
*
*******************************************************************************/
static RpMaterial *
SetTextureFilterMode(RpMaterial *material, void *data)
{
    RwTextureFilterMode filterMode;
    RwTexture *texture;

    filterMode = *(RwTextureFilterMode *)data;
    texture = RpMaterialGetTexture(material);
    if(texture) {
      RwTextureSetFilterMode(texture, filterMode );
    }else{
      printf("texture filter failure\n");
    }

    return(material);
}


/******************************************************************************
*
*
*
*******************************************************************************/
RwBool CreateWorldFromTableBSP(RwChar *filename)
{
    RwStream            *stream; //, *test_stream;
//    RwSurfaceProperties surfProps;
    RwTextureFilterMode filterMode;

    RwImageSetGamma(1.4f);

    /*
     * Read the table BSP from a binary format file...
     */
    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);

//	test_stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, "testwrite.bsp");

    if( stream )
    {
        if( RwStreamFindChunk(stream, rwID_WORLD,
                              (RwUInt32 *)NULL, (RwUInt32 *)NULL ) )
        {

            World = RpWorldStreamRead(stream);


            if( World )
            {
//				RpWorldStreamWrite(World, test_stream);
//                RwStreamClose(test_stream, NULL );

                RwStreamClose(stream, NULL );
               RwImageSetGamma(1.0f);
//OLD_RW                RwImageGammaCorrectionSetGamma(1.0f);

                //surfProps.ambient = 0.3f;
                //surfProps.diffuse = 0.7f;
                //surfProps.specular = 0.0f;

                //RpWorldSetSurfaceProperties(World, &surfProps );

                filterMode = rwFILTERLINEAR;

                RpWorldForAllMaterials(World, SetTextureFilterMode, (void *)(&filterMode) );

                //RpWorldSetFlags(World, RpWorldGetFlags(World) & ~rpWORLDPRELIT );

                Worigin = *RpWorldGetOrigin(World);
                world_bbox = *RpWorldGetBBox(World);

				RpCollisionWorldBuildData(World, 0); 

                return (RwBool)(TRUE);
            }
        }

        RwStreamClose(stream, NULL );
    }

    RwImageSetGamma(1.0f);
//OLD_RW    RwImageGammaCorrectionSetGamma(1.0f);

    return (RwBool)(FALSE);
}

/******************************************************************************
*
*
*
********************************************************************************/
void Terminate2D(void)
{
#ifdef _DEBUG_WINDOW
    Rt2dClose();

    RwCameraDestroy(debug_camera);
    Rt2dBrushDestroy(Brush);
    Rt2dFontDestroy(debug_font);

#endif
}

/******************************************************************************
*
*
*
********************************************************************************/
void
Terminate3D(void)
{
    /*
     * Clean-up all RenderWare components...
     */
    RwFrame *frame;


    if( game_cam.cam )
    {
        RwRaster *raster;

        RpWorldRemoveCamera(World, game_cam.cam);

        frame = RwCameraGetFrame(game_cam.cam);
        if( frame )
        {
            RwCameraSetFrame(game_cam.cam, (RwFrame *)NULL);
            RwFrameDestroy(frame);
        }

        raster = RwCameraGetRaster(game_cam.cam);
        if( raster )
        {
            RwCameraSetRaster(game_cam.cam, (RwRaster *)NULL);
            RwRasterDestroy(raster);
        }

        raster = RwCameraGetZRaster(game_cam.cam);
        if( raster )
        {
            RwCameraSetZRaster(game_cam.cam, (RwRaster *)NULL);
            RwRasterDestroy(raster);
        }

        RwCameraDestroy(game_cam.cam);
    }

    for(int i=0;i<num_lights;i++)
    {
        if(lights[i])
        {
            RpWorldRemoveLight(World, lights[i]);

            frame = RpLightGetFrame(lights[i]);
            if( frame )
            {
                RpLightSetFrame(lights[i], (RwFrame *)NULL);
                RwFrameDestroy(frame);
            }

            RpLightDestroy(lights[i]);
        }
    }

    /* destroy any clumps that we have created that are currently in the world */
    RpWorldForAllClumps(World, _destroyClumpCB, NULL);

    if( World )
    {
        RpWorldDestroy(World);
    }

    if( Charset )
    {
        RtCharsetDestroy(Charset);
    }

    Terminate2D();

    RsRwTerminate();

}

/******************************************************************************
*
*
*
*******************************************************************************/
RwBool CreateCameras(void)
{

//  game_cam.cam = CreateCamera(World);

    game_cam.cam = CameraCreate(RsGlobal.maximumWidth,RsGlobal.maximumHeight,1);

    if( game_cam.cam == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create camera."));

        return FALSE;
    }
    else
    {
        RpWorldAddCamera(World, game_cam.cam);
    }


#ifdef _DEBUG_WINDOW
    debug_camera = CameraCreate(RsGlobal.maximumWidth/2, RsGlobal.maximumHeight/2, FALSE);

    if( debug_camera == NULL )
    {
        RsErrorMessage(RWSTRING("Cannot create camera."));

        return FALSE;
    }
    else
    {
        RwV2d vw;
        Rt2dPath *path;

        vw.x = (RwReal)(1.0);
        vw.y = vw.x * RsGlobal.maximumHeight / RsGlobal.maximumWidth;
        RwCameraSetViewWindow(debug_camera, &vw);

        RwCameraSetNearClipPlane(debug_camera, (RwReal)(0.05));
        RwCameraSetFarClipPlane(debug_camera, (RwReal)(100.0));

        //Move these into Initialise2D when it exists
        Rt2dOpen(debug_camera);

        Rt2dDeviceSetCamera(debug_camera);
//      Rt2dDeviceSetFlat((RwReal)(0.5));

        Brush = Rt2dBrushCreate();

//      Rt2dFontSetPath(RWSTRING("./Fonts/"));
//      debug_font = Rt2dFontRead(RWSTRING("courier_new12"));

           /* get the User space bbox of camera view */

        Rt2dCTMPush();
        Rt2dCTMSetIdentity();
        path = Rt2dPathCreate();
        Rt2dDeviceGetClippath(path);
        Rt2dPathGetBBox(path, &WinBBox);
        Rt2dPathDestroy(path);
        Rt2dCTMPop();
    }

#endif

    return TRUE;
}

/******************************************************************************
*
*
*
*******************************************************************************/
RwBool CreateRearViewMirror(void)
{
    RwInt32 x,y,w,h;

    x = (RwInt32)(rvm.x_frac * RsGlobal.maximumWidth);
    y = (RwInt32)(rvm.y_frac * RsGlobal.maximumHeight);
    w = (RwInt32)(rvm.w_frac * RsGlobal.maximumWidth);
    h = (RwInt32)(rvm.h_frac * RsGlobal.maximumHeight);


    if(!game_cam.cam) return FALSE; //game cam must be set up first

    rvm.cam = RwCameraCreate();
    if(rvm.cam)
    {
        RwRect rect = {x,y,w,h};
        RwCameraSetFrame(rvm.cam, RwFrameCreate());
        RwCameraSetRaster(rvm.cam, RwRasterCreate(0,0,0, rwRASTERTYPECAMERA));

        RwRasterSubRaster(RwCameraGetRaster(rvm.cam),
                                    RwCameraGetRaster(game_cam.cam), &rect);

        RwCameraSetZRaster(rvm.cam, RwRasterCreate(0,0,0, rwRASTERTYPEZBUFFER));

        RwRasterSubRaster(RwCameraGetZRaster(rvm.cam),
                                    RwCameraGetZRaster(game_cam.cam), &rect);

        /* now check that everything is valid */
        if (RwCameraGetFrame(rvm.cam) && RwCameraGetRaster(rvm.cam) &&
            RwRasterGetParent(RwCameraGetRaster(rvm.cam)) &&
            RwCameraGetZRaster(rvm.cam) &&
            RwRasterGetParent(RwCameraGetZRaster(rvm.cam)) )
        {
            RpWorldAddCamera(World, rvm.cam);

            CameraSetViewFrustrum(rvm.cam, 60.0f, (RwReal)h/(RwReal)w, 0.5f, 2500.0f);  // fov, aspect ratio, near clip, far clip

            return TRUE;
        }
        else
        {
            RwFrameDestroy(RwCameraGetFrame(rvm.cam));
            RwCameraDestroy(rvm.cam);
            rvm.cam = (RwCamera *)NULL;
        }
    }

    return FALSE;
}
/******************************************************************************
*
*
*
*******************************************************************************/
RwBool ResizeRearViewMirror(void)
{
    RwInt32 x,y,w,h;

    x = (RwInt32)(rvm.x_frac * RsGlobal.maximumWidth);
    y = (RwInt32)(rvm.y_frac * RsGlobal.maximumHeight);
    w = (RwInt32)(rvm.w_frac * RsGlobal.maximumWidth);
    h = (RwInt32)(rvm.h_frac * RsGlobal.maximumHeight);

    if(rvm.cam && game_cam.cam)
    {
        RwRect rect = {x,y,w,h};

        RwRasterSubRaster(RwCameraGetRaster(rvm.cam),
                                    RwCameraGetRaster(game_cam.cam), &rect);

        RwRasterSubRaster(RwCameraGetZRaster(rvm.cam),
                                    RwCameraGetZRaster(game_cam.cam), &rect);
    }

    return FALSE;
}
/******************************************************************************
*
*
*
*******************************************************************************/

