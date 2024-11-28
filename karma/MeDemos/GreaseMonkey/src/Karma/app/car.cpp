/***********************************************************************************************
*
*   $Id: car.cpp,v 1.1.2.4 2002/03/13 13:17:33 richardm Exp $
*
************************************************************************************************/
#include<malloc.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "McdFrame.h"
#include "rwcore.h"
#include "rpworld.h"
#include "RwFuncs.hpp"
#include "skeleton.h"
#include "platxtra.h"
#include "metrics.h"

#ifndef _XBOX
    #ifdef WIN32
        #include "win.h"
    #endif
#else
    #include <xtl.h>
#endif

#include "MdtCar.hpp"
#include "carAPI.hpp"
#include "utils.hpp"
#include "car.hpp"
#include "carsound.hpp"
#include "driver.hpp"
#include "smoke.hpp"
#include "init.hpp"

#include "MeMemory.h"

METER_ARC speedo, tacho;

#if _DEBUG
int paused = 0;
int frameno = 0;
#else
int paused = 1;
#endif


//RwReal camera_angle = 0.0f;
MeReal frame_delta_t = (MeReal)0.0;

float render_time, dynamics_time, collision_time;

GAME_VARIABLES game_vars;
GAME_CAMERA game_cam;
MIRROR rvm; //rear view mirror
RpLight *lights[MAX_LIGHTS];

Driver *player = (Driver *)NULL;
Driver **drivers = (Driver **)NULL;

void CameraLookAt(RwCamera *cam, RpClump *clump, RwReal *curr_dir, RwReal *curr_pitch, RwReal lag);
void UpdateAllShapes(void);

RwV3d drive_by_cam_position = {30.0f, 3.0f, 10.0f};
RwV3d driver_head_pos = {-1.15f,1.05f,-0.65f};

RwV3d fixed_to_car_cam_positions[NUM_F1_CAMS] = {

    {-15,   3,  0},
    {-10,   2,  -4},
    {-10,   2,  4},
    {0,     1,  0}
};

RwV3d cam_positions_around_track[NUM_F2_CAMS] = {
    {20.0f,     15.0f,      00.0f },
    {520.0f,    15.0f,      -70.0f },
    {200.0f,    15.0f,      -270.0f },
    {-30.0f,    15.0f,      350.0f },
    {-420.0f,   15.0f,      150.0f }
};


/***********************************************************************************************
*
*
*
************************************************************************************************/
void ResetCamera(CAMERA_MODE mode, RpClump *clump)
{

    RwV3d temp, *c_pos, *b_pos;
    RwReal grnd_height;
    RwReal dx, dy, dz, h_dist;

    /* If changing from inside view, need to switch on car shape etc.   */
    if(drivers[game_cam.view_driver]->internal_shape)
    {
        if(game_cam.mode == CM_FIXED_IN_CAR)
        {
            drivers[game_cam.view_driver]->TurnOnCarShape();

            if(RpClumpGetWorld(drivers[game_cam.view_driver]->internal_shape))
            {
                RpWorldRemoveClump(World, drivers[game_cam.view_driver]->internal_shape);
            }
        }
    }
    rvm.on  = 0; // default to off

    switch(mode) {
        case CM_FIXED_TO_CAR:
            if(clump) {
                game_cam.mode = mode;
                game_cam.view_clump = clump;

                RwMatrixSetIdentity(&game_cam.offset);

                RwMatrixRotate(&game_cam.offset, RwMatrixGetUp(&game_cam.offset), 90, rwCOMBINEPOSTCONCAT);
                RwMatrixTranslate(&game_cam.offset, &fixed_to_car_cam_positions[game_cam.index] , rwCOMBINEPOSTCONCAT);

            } else {
                game_cam.mode = CM_FIXED_DEFAULT;
            }
            break;
        case CM_FIXED_IN_CAR:
            if(clump) {
                /* Turn off Car components and turn on dashboard    */
                if(drivers[game_cam.view_driver]->internal_shape)
                    {
                        drivers[game_cam.view_driver]->TurnOffCarShape();
                        if(!RpClumpGetWorld(drivers[game_cam.view_driver]->internal_shape))
                        {
                            RpWorldAddClump(World, drivers[game_cam.view_driver]->internal_shape);
                        }
                    }

                game_cam.mode = mode;
                game_cam.view_clump = clump;

                /* Set up view position */
                RwMatrixSetIdentity(&game_cam.offset);
                RwMatrixRotate(&game_cam.offset, RwMatrixGetUp(&game_cam.offset), 90, rwCOMBINEPOSTCONCAT);
                RwMatrixTranslate(&game_cam.offset, &driver_head_pos , rwCOMBINEPOSTCONCAT);


                rvm.on = 1;
            } else {
                game_cam.mode = CM_FIXED_DEFAULT;
            }
            break;
        case CM_FIXED_VIEW_POINT:
            if(clump) {
                game_cam.mode = mode;
                game_cam.view_clump = clump;
                RwMatrixTransform(&game_cam.offset, RwFrameGetMatrix(RpClumpGetFrame(clump)) , rwCOMBINEREPLACE);
                RwMatrixTranslate(&game_cam.offset, &drive_by_cam_position , rwCOMBINEPRECONCAT);
                RwMatrixTransform(RwFrameGetMatrix(RwCameraGetFrame(game_cam.cam)), &game_cam.offset , rwCOMBINEREPLACE);
                c_pos = RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(game_cam.cam)));
                grnd_height = GetGroundHeight(c_pos->x, c_pos->z);
                if(c_pos->y < grnd_height + CAM_MIN_HEIGHT) c_pos->y = grnd_height+CAM_MIN_HEIGHT;
            } else {
                game_cam.mode = CM_FIXED_DEFAULT;
            }
            break;
        case CM_MOVING_VIEW_POINT: //find nearest fixed cam point to move to
            if(clump) {
                RwReal dist, min_dist = 10000000000.0f;
                RwInt32 i, index = 0;
                RwV3d clmp_pos = *RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(clump)));
                RwV3dSub(&clmp_pos, &clmp_pos, &Worigin);
                for(i = 0; i < NUM_F2_CAMS; i++) {
                    RwV3dSub(&temp, &cam_positions_around_track[i], &clmp_pos);
                    dist = RwV3dDotProduct(&temp, &temp);
                    if(dist < min_dist) 
					{
                        min_dist = dist;
                        index = i;
                    }
                }
                game_cam.mode = mode;
                game_cam.view_clump = clump;
                game_cam.index = index;

                RwMatrixSetIdentity(&game_cam.offset);
                RwMatrixTranslate(&game_cam.offset, &Worigin , rwCOMBINEREPLACE);
                RwMatrixTranslate(&game_cam.offset, &cam_positions_around_track[game_cam.index] , rwCOMBINEPOSTCONCAT);
                RwMatrixCopy(RwFrameGetMatrix(RwCameraGetFrame(game_cam.cam)), &game_cam.offset);
            } else {
                game_cam.mode = CM_FIXED_DEFAULT;
            }
            break;
        case CM_FOLLOW:
            if(clump) {
                game_cam.mode = mode;
                game_cam.view_clump = clump;

                RwMatrixSetIdentity(&game_cam.offset);
                RwMatrixTranslate(&game_cam.offset, &fixed_to_car_cam_positions[0] , rwCOMBINEREPLACE);

                c_pos = RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(game_cam.cam)));
                b_pos = RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(game_cam.view_clump)));

                /* Calculate angles to look at clump    */
                dx = b_pos->x - c_pos->x;
                dy = b_pos->y - c_pos->y;
                dz = b_pos->z - c_pos->z;
                h_dist = (RwReal)MeSqrt(dx*dx + dz*dz);
                game_cam.dir = (RwReal)RadsToDegs(MeAtan2(dx, dz));
                game_cam.pitch = (RwReal)RadsToDegs(atanf(-dy/MAX(h_dist,0.1f)));

            } else {
                game_cam.mode = CM_FIXED_DEFAULT;
            }
            break;
        case CM_MOVEABLE:
            if(clump) {
                game_cam.mode = mode;
                game_cam.view_clump = clump;

                /* Project the cars positon forward by its horizontal velocity  */
                RwV2d vel;
                drivers[game_cam.view_driver]->Get2DVelocity(&vel);

                c_pos = RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(game_cam.cam)));
                b_pos = RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(game_cam.view_clump)));

                c_pos->x = b_pos->x + vel.x;
                c_pos->y = b_pos->y + 10.0f;
                c_pos->z = b_pos->z - vel.y;

                RwV3dSub(&temp, b_pos, c_pos);
                game_cam.dist = (RwReal)RwV3dNormalize(&temp, &temp);
                game_cam.dir = (RwReal)RadsToDegs(MeAtan2((MeReal)-temp.x, (MeReal)-temp.z));
                game_cam.pitch = (RwReal)RadsToDegs((MeReal)asinf(temp.y));

                temp.x = 0.0f; temp.y = 0.0f; temp.z = game_cam.dist;

                //RwMatrixSetIdentity(&game_cam.offset);
                RwMatrixTranslate(&game_cam.offset, &temp, rwCOMBINEREPLACE);
                RwMatrixRotate(&game_cam.offset, RwMatrixGetUp(&game_cam.offset), game_cam.dir, rwCOMBINEPOSTCONCAT);
                RwMatrixRotate(&game_cam.offset, RwMatrixGetRight(&game_cam.offset), game_cam.pitch, rwCOMBINEPOSTCONCAT);


            } else {
                game_cam.mode = CM_FIXED_DEFAULT;
            }
            break;
        case CM_FIXED_DEFAULT:
        default:
            break;
    }

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void InitialiseLight(RpLight *light)
{
//  LightSetPos(light, 0.0f, 50.0f, 0.0f); //x,  y,  z

    LightSetDegAngles(light, 0.0f, 45.0f);//yaw, pitch : Degrees
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
void CameraLagFollow(RwCamera *cam, RpClump *clump, RwReal lag)
{
    RwV3d c_pos, end_pos;
    RwMatrix b_mtx, c_mtx; //temp;
    RwReal grnd_height, len;
    RwFrame *c_frame;

    c_frame = RwCameraGetFrame(cam);

    /* Current camera position  */
    c_mtx = *RwFrameGetMatrix(c_frame);
    c_pos = *RwMatrixGetPos(&c_mtx);

    /* Get point that camera is moving towards  */
    b_mtx = *RwFrameGetMatrix(RpClumpGetFrame(clump)); //Matrix of vehicle chassis
//  RwMatrixMultiply(&temp, &game_cam.offset,&b_mtx);
//  end_pos = *RwMatrixGetPos(&temp);
    end_pos = *RwMatrixGetPos(&b_mtx);
    end_pos.y += fixed_to_car_cam_positions[0].y;

    /* Calculate vector to move camera by   */
    RwV3dSub(&end_pos, &end_pos, &c_pos);
    lag = MIN(lag*(RwReal)frame_delta_t, 1.0f);
    len = RwV3dNormalize(&end_pos, &end_pos);
    RwV3dScale(&end_pos, &end_pos, (len+fixed_to_car_cam_positions[0].x)*lag);
    RwV3dAdd(&end_pos,&end_pos, &c_pos);

    /* Check that camera will not go under ground   */
    grnd_height = GetGroundHeight(end_pos.x, end_pos.z);
    if(end_pos.y < grnd_height + CAM_MIN_HEIGHT) end_pos.y = grnd_height+CAM_MIN_HEIGHT;

    RwMatrixTranslate(&c_mtx, &end_pos, rwCOMBINEREPLACE);
    /* Move camera  */
    RwFrameTransform(c_frame, &c_mtx, rwCOMBINEREPLACE);

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void CameraLagLookAt(RwCamera *cam, RpClump *clump, RwReal *curr_dir, RwReal *curr_pitch, RwReal lag)
{
    RwReal dx, dy, dz, h_dist, c_dir, c_pitch, dir_diff;
    RwV3d c_pos, b_pos;

    c_pos = *RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(cam)));
    b_pos = *RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(clump)));

    /* Calculate angles to look at clump    */
    dx = b_pos.x - c_pos.x;
    dy = b_pos.y - c_pos.y;
    dz = b_pos.z - c_pos.z;
    h_dist = (RwReal)MeSqrt(dx*dx + dz*dz);
    c_dir = (RwReal)RadsToDegs(MeAtan2(dx, dz));
    c_pitch = (RwReal)RadsToDegs(atanf(-dy/MAX(h_dist,0.1f)));

    /* Aim camera with lagged position  */
    lag = MIN(lag*(RwReal)frame_delta_t, 1.0f);
    dir_diff = SmallestAngleDegs(*curr_dir, c_dir);
    *curr_dir += lag*dir_diff;
    if(*curr_dir > 180.0f) *curr_dir -= 360.0f;
    if(*curr_dir < -180.0f) *curr_dir += 360.0f;
    *curr_pitch += lag*(c_pitch-*curr_pitch);
    CameraSetDegAngles(cam, *curr_dir, *curr_pitch, 0.0f);//yaw, pitch, roll : Degrees
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void CameraLookAt(RwCamera *cam, RpClump *clump)
{
    RwReal dx, dy, dz, h_dist, c_dir, c_pitch, grnd_height;
    RwV3d c_pos, b_pos;
    RwMatrix temp;

    c_pos = *RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(cam)));
    b_pos = *RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(clump)));

    /* Calculate angles to look at clump    */
    dx = b_pos.x - c_pos.x;
    dy = b_pos.y - c_pos.y;
    dz = b_pos.z - c_pos.z;
    h_dist = (RwReal)MeSqrt(dx*dx + dz*dz);
    c_dir = (RwReal)RadsToDegs(MeAtan2(dx, dz));
    c_pitch = (RwReal)RadsToDegs(atanf(-dy/MAX(h_dist,0.1f)));

    /* Aim camera with lagged position  */
    RwMatrixSetIdentity(&temp);
    RwMatrixRotate(&temp, RwMatrixGetUp(&temp), c_dir, rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(&temp, RwMatrixGetRight(&temp), c_pitch, rwCOMBINEPOSTCONCAT);

    /* Check that camera will not go under ground   */
    grnd_height = GetGroundHeight(c_pos.x, c_pos.z);
    if(c_pos.y < grnd_height + CAM_MIN_HEIGHT) c_pos.y = grnd_height+CAM_MIN_HEIGHT;

    RwMatrixTranslate(&temp, &c_pos, rwCOMBINEPOSTCONCAT);

    //RwMatrixCopy(RwFrameGetMatrix(RwCameraGetFrame(cam)), &temp);
    RwFrameTransform(RwCameraGetFrame(cam), &temp, rwCOMBINEREPLACE);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void UpdateRearViewMirror(void)
{
    RwMatrix *b_mtx;
    RwFrame *c_frame;

    if(!rvm.on || !rvm.cam) return;

    b_mtx = RwFrameGetMatrix(RpClumpGetFrame(game_cam.view_clump)); //Matrix of vehicle chassis
    c_frame = RwCameraGetFrame(rvm.cam);
            //RwFrameSetIdentity(c_frame);
    RwFrameTransform(c_frame, b_mtx, rwCOMBINEREPLACE);
    RwFrameTransform(c_frame, &rvm.offset, rwCOMBINEPRECONCAT);

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void UpdateCameras(void)
{
    RwMatrix *b_mtx;
    RwFrame *c_frame;
    static int count = 0;

    switch(game_cam.mode) {
        case CM_FIXED_TO_CAR:
            b_mtx = RwFrameGetMatrix(RpClumpGetFrame(game_cam.view_clump)); //Matrix of vehicle chassis
            c_frame = RwCameraGetFrame(game_cam.cam);
            //RwFrameSetIdentity(c_frame);
            RwFrameTransform(c_frame, b_mtx, rwCOMBINEREPLACE);
            RwFrameTransform(c_frame, &game_cam.offset, rwCOMBINEPRECONCAT);
            break;
        case CM_FIXED_IN_CAR:
            b_mtx = RwFrameGetMatrix(RpClumpGetFrame(game_cam.view_clump)); //view clump is still vehicle chassis
            c_frame = RwCameraGetFrame(game_cam.cam);
            //RwFrameSetIdentity(c_frame);
            RwFrameTransform(c_frame, b_mtx, rwCOMBINEREPLACE);
            RwFrameTransform(c_frame, &game_cam.offset, rwCOMBINEPRECONCAT);

            /* Set Dashboard clump matrix to be the same as the view clump  */
            RwFrameTransform(RpClumpGetFrame(drivers[game_cam.view_driver]->internal_shape) , b_mtx, rwCOMBINEREPLACE);
            break;
        case CM_FIXED_VIEW_POINT:
                //CameraLagLookAt(game_cam.cam, game_cam.view_clump, &game_cam.dir, &game_cam.pitch, game_cam.lag);
                count++;
                if(count > 2.5*FramesPerSecond) {
                    ResetCamera(game_cam.mode, game_cam.view_clump);
                    count = 0;
                }
                CameraLookAt(game_cam.cam, game_cam.view_clump);

            break;
        case CM_MOVING_VIEW_POINT: {
                count++;
                if(count > 1.0*FramesPerSecond) {
                    ResetCamera(game_cam.mode, game_cam.view_clump);
                    count = 0;
                }
                //CameraLagLookAt(game_cam.cam, game_cam.view_clump, &game_cam.dir, &game_cam.pitch, game_cam.lag);
                CameraLookAt(game_cam.cam, game_cam.view_clump);
            }
            break;
        case CM_FOLLOW:
          CameraLagFollow(game_cam.cam, game_cam.view_clump, game_cam.pos_lag);
            CameraLagLookAt(game_cam.cam, game_cam.view_clump, &game_cam.dir, &game_cam.pitch, game_cam.ang_lag);
            break;
        case CM_MOVEABLE:
            b_mtx = RwFrameGetMatrix(RpClumpGetFrame(game_cam.view_clump)); //Matrix of vehicle chassis
            c_frame = RwCameraGetFrame(game_cam.cam);
            RwFrameSetIdentity(c_frame);
            RwFrameTranslate(c_frame, RwMatrixGetPos(b_mtx), rwCOMBINEPOSTCONCAT);
            //RwFrameTransform(c_frame, b_mtx, rwCOMBINEPOSTCONCAT);
            RwFrameTransform(c_frame, &game_cam.offset, rwCOMBINEPRECONCAT);
            b_mtx = RwFrameGetMatrix(c_frame);
            CameraLookAt(game_cam.cam, game_cam.view_clump);
            break;
        default:
            break;
    }

    UpdateRearViewMirror();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void PreEvolveAllCars(void)
{
    int i;
    for(i = 0; i < game_vars.num_cars; i++)
    {
        drivers[i]->GetCar()->PreEvolve(); //reset contact info
    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void DriverUpdate(void)
{
    int i;

    for(i = 0; i < game_vars.num_cars; i++)
    {
        drivers[i]->Update();
    }

}


/***********************************************************************************************
*
*
*
************************************************************************************************/
void DoFrame(void)
{

    frame_delta_t = GetFrameRate();

    if(!paused) ResetDebugLines();

    if(frame_delta_t > 0.0f)
    {
        SmokeUpdate((RwReal)frame_delta_t);

        if(!paused) {
//#ifdef _DEBUG_WINDOW
//  DebugChannel(0,frame_delta_t, frame_delta_t); //time taken for last frame
//  DebugChannel(1,frame_delta_t, render_time*0.001f); //time taken for last frame
//  DebugChannel(2,frame_delta_t, collision_time*0.001f); //time taken for last frame
//  DebugChannel(3,frame_delta_t, dynamics_time*0.001f); //time taken for last frame
//#endif
            PreEvolveAllCars();
            SysEvolve(frame_delta_t);

            DriverUpdate();
            UpdateAllShapes();

            drivers[game_cam.view_driver]->SetSpeedo();

#if _DEBUG
            frameno++;
#endif
        }

#if 0
        /* Draw points along racing line    */

#if _MECHECK

        int i;
        MeVector3 p1, p2;

        for(i = 0; i < tracks[0].num_points-1; i++)
        {
            p1[0] = tracks[0].course_points[i].pos.x;       p1[2] = 1.0f; p1[1] = tracks[0].course_points[i].pos.y;
            p2[0] = tracks[0].course_points[i+1].pos.x; p2[2] = 1.0f; p2[1] = tracks[0].course_points[i+1].pos.y;
            DebugLine(p1,p2,255,0,0);
        }
#endif
#endif
        UpdateCameras();

        /* Redraw the window    */
        RenderFrame();

    }

    /* Reset lines now that we have rendered them   */
    //if(!paused)   ResetDebugLines();

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void TerminateCarDemo(void)
{
    int i=0;
	ResetCamera(CM_MOVEABLE, game_cam.view_clump);//force camera outside
    /* delete cars and drivers  */
    for(i=0;i<game_vars.num_cars;i++)
    {
        delete drivers[i];
    }
    free(drivers);

#ifdef RWMETRICS
	RsMetricsClose();
#endif

	SmokeDestroy();

    Terminate3D();


#ifdef USE_SOUND
    TerminateSoundSystem();
#endif
    TerminateCarSim();  //close Mathengine
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
void UpdateAllShapes(void)
{
    int i;

    for(i=0; i<game_vars.num_cars; i++)
    {
        drivers[i]->UpdateShapes();
    }
}
