/***********************************************************************************************
*
*   $Id: control.cpp,v 1.1.2.1 2002/03/01 19:55:03 richardm Exp $
*
************************************************************************************************/
#include <malloc.h>
#include <math.h>
#include <stddef.h>
#include "rwcore.h"
#include "rpworld.h"
//#include "rtcharse.h"

#include "RwFuncs.hpp"

#include "../plat/platxtra.h"

//#include "carAPI.hpp"
#include "McdFrame.h"

#include "MdtCar.hpp"
#include "carAPI.hpp"

#include "utils.hpp"
#include "car.hpp"
#include "control.hpp"
#include "driver.hpp"

#include "MeMemory.h"
//extern Driver *player;

//extern  void SetKartXYPosition(MeReal, MeReal);

static MeReal kart_throttle = 0.0, kart_steering = 0.0;
CTRL_INPUT_METHOD input_control_method;
#ifdef __cplusplus
extern "C"
{
#endif
/***********************************************************************************************
*
*
*
************************************************************************************************/
void SetPlayerSteering(MeReal s)
{
    if(player)  player->SetSteering(s);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void SetPlayerThrottle(MeReal t)
{
    if(player)  player->SetThrottle(t);
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void wUpshift(void)
{

    if(player)
            player->Upshift();

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void wDownshift(void)
{
    if(player)
            player->Downshift();
}

#ifdef __cplusplus
}
#endif


/***********************************************************************************************
*
*
*
************************************************************************************************/
void xFlipCar(void) {


}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xTakeControlOfCar(int id)
{
    int old_id = game_vars.player_car-1;

    if(id < 0 || id > game_vars.num_cars-1) return;

    /* Completely relinquish control of old car */
    if(player)
    {
        player->SetControlMethod(COMP_CONTROL);
        player->GetCar()->Shift(1);
        player = (Driver *)NULL;
        game_vars.player_car = 0;
    }


    /* Only take control of car if we weren't already driving it so that
        we can give it back to the computer */
    if(id != old_id)
    {
        game_vars.player_car = id+1;
        player = drivers[id];
        player->SetControlMethod(PLAYER_CONTROL);
        player->GetCar()->Shift(1);

    }
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void xToggleScreenInfo(void)
{
    //ScreenInfoOn += 1;
    //if(ScreenInfoOn > 2) ScreenInfoOn = 0;
    ScreenInfoOn = !ScreenInfoOn;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xTogglePause(void)
{
    paused = !paused;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xReset(int i)
{
    MeReal x,y,z;
    static MeReal theta = 0;
    MeVector4 q;
    Driver *d = drivers[game_cam.view_driver];

    if(!d) return;

    Car* c = d->GetCar();

    x = c->GetBodyPosCmpt(CHASSIS_CMPT,0);
    y = c->GetBodyPosCmpt(CHASSIS_CMPT,2);
    z = -c->GetBodyPosCmpt(CHASSIS_CMPT,1);

    c->Init(d->car_data_file);

    c->Reset();
    c->SetPosition(x,y+1.0f,z);

    if(d == player)
    {
        theta += DegsToRads(45);
        q[0] = MeCos(theta);    q[1] = 0; q[2] = 0; q[3] = MeSin(theta);
        //q[0] = MeCos(theta);  q[3] = 0; q[2] = 0; q[1] = MeSin(theta);

        c->SetOrientation(q);
    }
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
void xSetTranny(int type)
{
    if(player) player->SetTranny(type);
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void xAlterSteering(MeReal inc)
{
    kart_steering += inc*frame_delta_t;
    kart_steering = LIMITS(-1.0f, kart_steering, 1.0f);

    if(player) player->SetSteering(kart_steering);

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xAlterThrottle(MeReal inc)
{

    kart_throttle += inc*frame_delta_t;
    kart_throttle = LIMITS(-1.0f, kart_throttle, 1.0f);

    if(player)  player->SetThrottle(kart_throttle);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void DecaySteering(void)
{
    if(kart_steering > 0) {
        kart_steering -= MIN(2.0f*frame_delta_t, kart_steering);
        if(player) player->SetSteering(kart_steering);
    } else if(kart_steering < 0) {
        kart_steering += MIN(2.0f*frame_delta_t, -kart_steering);
        if(player) player->SetSteering(kart_steering);
    }

    //kart_steering = 0.99*kart_steering;
    //SetSteering(kart_steering);

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void DecayThrottle(void)
{
    if(kart_throttle > 0) {
        kart_throttle -= MIN(4.0f*frame_delta_t, kart_throttle);
        if(player) player->SetThrottle(kart_throttle);
    } else if(kart_throttle < 0) {
        kart_throttle += MIN(4.0f*frame_delta_t, -kart_throttle);
        if(player) player->SetThrottle(kart_throttle);
    }
    //kart_throttle = 0.95*kart_throttle;
    //SetThrottle(kart_throttle);
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
void xCameraRight(void)
{
    RwV3d up = {0,1,0};
    RwV3d temp;

    game_cam.dir += CAM_ROT_INC*(RwReal)frame_delta_t;

    temp.x = 0.0f; temp.y = 0.0f; temp.z = game_cam.dist;

    RwMatrixSetIdentity(&game_cam.offset);

    RwMatrixRotate(&game_cam.offset, RwMatrixGetUp(&game_cam.offset), game_cam.dir, rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(&game_cam.offset, RwMatrixGetRight(&game_cam.offset), game_cam.pitch, rwCOMBINEPOSTCONCAT);

    RwMatrixTranslate(&game_cam.offset, &temp, rwCOMBINEPRECONCAT);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xCameraLeft(void)
{
    RwV3d up = {0,1,0};
    RwV3d temp;

    game_cam.dir -= CAM_ROT_INC*(RwReal)frame_delta_t;

    temp.x = 0.0f; temp.y = 0.0f; temp.z = game_cam.dist;

    RwMatrixSetIdentity(&game_cam.offset);

    RwMatrixRotate(&game_cam.offset, RwMatrixGetUp(&game_cam.offset), game_cam.dir, rwCOMBINEPOSTCONCAT);
    RwMatrixRotate(&game_cam.offset, RwMatrixGetRight(&game_cam.offset), game_cam.pitch, rwCOMBINEPOSTCONCAT);

    RwMatrixTranslate(&game_cam.offset, &temp, rwCOMBINEPRECONCAT);
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void xCameraUp(void)
{
    RwV3d *right =  RwMatrixGetRight(RwFrameGetMatrix(RwCameraGetFrame(game_cam.cam)));
    RwV3d temp;

    if(game_cam.pitch > -85.0f + CAM_ROT_INC*frame_delta_t)
    {
        game_cam.pitch -= CAM_ROT_INC*(RwReal)frame_delta_t;

        temp.x = 0.0f; temp.y = 0.0f; temp.z = game_cam.dist;

        RwMatrixSetIdentity(&game_cam.offset);

        RwMatrixRotate(&game_cam.offset, RwMatrixGetUp(&game_cam.offset), game_cam.dir, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(&game_cam.offset, RwMatrixGetRight(&game_cam.offset), game_cam.pitch, rwCOMBINEPOSTCONCAT);

        RwMatrixTranslate(&game_cam.offset, &temp, rwCOMBINEPRECONCAT);
    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xCameraDown(void)
{
    RwV3d *right =  RwMatrixGetRight(RwFrameGetMatrix(RwCameraGetFrame(game_cam.cam)));
    RwV3d temp;

    if(game_cam.pitch < - CAM_ROT_INC*frame_delta_t)
    {
        game_cam.pitch += CAM_ROT_INC*(RwReal)frame_delta_t;

        temp.x = 0.0f; temp.y = 0.0f; temp.z = game_cam.dist;

        RwMatrixSetIdentity(&game_cam.offset);

        RwMatrixRotate(&game_cam.offset, RwMatrixGetUp(&game_cam.offset), game_cam.dir, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(&game_cam.offset, RwMatrixGetRight(&game_cam.offset), game_cam.pitch, rwCOMBINEPOSTCONCAT);

        RwMatrixTranslate(&game_cam.offset, &temp, rwCOMBINEPRECONCAT);
    }
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void xCameraIn(void)
{
    RwV3d temp;

    if(game_cam.dist > 1.0 + CAM_DIST_INC*(RwReal)frame_delta_t) {
        game_cam.dist -= CAM_DIST_INC*(RwReal)frame_delta_t;

        temp.x = 0.0f; temp.y = 0.0f; temp.z = game_cam.dist;

        RwMatrixSetIdentity(&game_cam.offset);

        RwMatrixRotate(&game_cam.offset, RwMatrixGetUp(&game_cam.offset), game_cam.dir, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(&game_cam.offset, RwMatrixGetRight(&game_cam.offset), game_cam.pitch, rwCOMBINEPOSTCONCAT);

        RwMatrixTranslate(&game_cam.offset, &temp, rwCOMBINEPRECONCAT);
    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xCameraOut(void)
{
    RwV3d temp;

    if(game_cam.dist < 200.0 - CAM_DIST_INC*(RwReal)frame_delta_t) {
        game_cam.dist += CAM_DIST_INC*(RwReal)frame_delta_t;

        temp.x = 0.0f; temp.y = 0.0f; temp.z = game_cam.dist;

        RwMatrixSetIdentity(&game_cam.offset);

        RwMatrixRotate(&game_cam.offset, RwMatrixGetUp(&game_cam.offset), game_cam.dir, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(&game_cam.offset, RwMatrixGetRight(&game_cam.offset), game_cam.pitch, rwCOMBINEPOSTCONCAT);

        RwMatrixTranslate(&game_cam.offset, &temp, rwCOMBINEPRECONCAT);
    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xNextCar(void)
{
    //if(game_cam.mode == CM_FIXED_IN_CAR) return; //Can't change car if inside

    /* If were internal need to switch back on external shape of this car   */
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

    game_cam.view_driver++;
    if(game_cam.view_driver >= drivers[0]->num_drivers) game_cam.view_driver = 0;
    ResetCamera(game_cam.mode, drivers[game_cam.view_driver]->GetChassisClump());
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xViewPlayer(void)
{
    if(!player) return;

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

    game_cam.view_driver = player->GetID();
    ResetCamera(game_cam.mode, drivers[game_cam.view_driver]->GetChassisClump());
}


/***********************************************************************************************
*
*
*
************************************************************************************************/
void xUpshift(void)
{

    if(player)
            player->Upshift();

}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xDownshift(void)
{
    if(player)
            player->Downshift();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void xToggleLOD(void)
{
    if(drivers[game_cam.view_driver]->GetCar()->level_of_detail == LOD2)
    {
        drivers[game_cam.view_driver]->GetCar()->SetLevelOfDetail(LOD1);
    } else {
        drivers[game_cam.view_driver]->GetCar()->SetLevelOfDetail(LOD2);
    }
}
