/***********************************************************************************************
*
*   $Id: driver.cpp,v 1.1.2.4 2002/03/13 13:17:33 richardm Exp $
*
************************************************************************************************/
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "rwcore.h"
#include "rpworld.h"
#include "RwFuncs.hpp"
#include "skeleton.h"

#include "platxtra.h"

//#include "carAPI.hpp"
#include "McdFrame.h"

#include "MdtCar.hpp"
#include "carAPI.hpp"
#include "utils.hpp"
#include "car.hpp"
#include "driver.hpp"
#include "parser.hpp"

#ifdef USE_SOUND
    #include "carsound.hpp"
    #include "stdio.h"
#endif // USE_SOUND


#include"smoke.hpp"

#include "MeMemory.h"

MeReal Driver::race_best = 0;

RwV2d startpoint = {60.0f, 50.0f};
RwV2d resetpoint = {185.0f, 225.0f};

/* Hardcoded racing course for computer controlled cars */

COURSE tracks[MAX_COURSES];
int num_tracks = 0;

int Driver::num_drivers = 0;



static void xInternalShape(Parser *pf);
static void xExtTexture(Parser *pf);
static void xCmptShapes(Parser *pf);
static void xCarDataFile(Parser *pf);
static void xSteeringGains(Parser *pf);
static void xThrottleGains(Parser *pf);
static void xShapeDir(Parser *pf);
static void xTexDir(Parser *pf);

static TEXT_2_PFUNC cfg_comtable[] = {
    {"SHAPE_DIR",   xShapeDir},
    {"TEXTURE_DIR", xTexDir},
    {"INTERNAL_SHAPE",xInternalShape},
    {"CMPT_SHAPES", xCmptShapes},
    {"EXTERNAL_TEXTURE", xExtTexture},
    {"CAR_DATA_FILE", xCarDataFile},
    {"STEERING_GAINS", xSteeringGains},
    {"THROTTLE_GAINS", xThrottleGains},
    {0,0}
};

Driver *curr_driver;

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xInternalShape(Parser *pf)//called when
{
    RwChar *fname, temp[MAX_PATHLEN];

    fname = RsPathnameCreate(RWSTRING(curr_driver->texture_path));
    RwImageSetPath(fname);
    RsPathnameDestroy(fname);

    /* Load DashBoard Shape */
    sprintf(temp,"%s%s",curr_driver->shape_dir,pf->GetWord());
    fname = RsPathnameCreate(RWSTRING(temp));
    curr_driver->internal_shape = LoadDffFile(fname);
    if(curr_driver->internal_shape)
    {
        if(RpClumpGetWorld(curr_driver->internal_shape))
        {
            RpWorldRemoveClump(World, curr_driver->internal_shape);
        }
    } else {
        RsErrorMessage(RWSTRING("Internal Shape Not Loaded"));
        RsPathnameDestroy(fname);
        //return FALSE;
    }
    RsPathnameDestroy(fname);

}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xShapeDir(Parser *pf)
{
    
        sprintf(curr_driver->shape_dir,"%s/", pf->GetWord());
    
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xTexDir(Parser *pf)
{
    
        sprintf(curr_driver->texture_path,"%s/", pf->GetWord());
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xExtTexture(Parser *pf)
{
    RwChar *fname;

    fname = RsPathnameCreate(RWSTRING(curr_driver->texture_path));

    RwImageSetPath(fname);

    RsPathnameDestroy(fname);

    curr_driver->car_texture = RwTextureRead(RWSTRING(pf->GetWord()), (char *)NULL);
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xCmptShapes(Parser *pf)
{
    RwChar *fname, temp[MAX_PATHLEN];
    int i = 0;
    RpClump *clump;

    fname = RsPathnameCreate(RWSTRING(curr_driver->texture_path));
    RwImageSetPath(fname);
    RsPathnameDestroy(fname);

    for(i = 0; i < NUM_CAR_CMPTS; i++)
    {
        sprintf(temp,"%s%s",curr_driver->shape_dir,pf->GetWord());

        /* Load component shape and set up clump pointer    */
        fname = RsPathnameCreate(RWSTRING(temp));
        clump = LoadDffFile(fname);
        RsPathnameDestroy(fname);

        curr_driver->car_cmpts[i].clump = clump;
    }
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xCarDataFile(Parser *pf)
{
    sprintf(curr_driver->car_data_file,"./Scripts/%s", pf->GetWord());
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xSteeringGains(Parser *pf)
{
    curr_driver->steering_p_gain = pf->GetFloat();
    curr_driver->steering_tcp_gain = pf->GetFloat(); //time compensated proportional
    curr_driver->steering_d_gain = pf->GetFloat();
    curr_driver->steering_slip_gain = pf->GetFloat();

}
/***********************************************************************************************
*
*
*
************************************************************************************************/
static void xThrottleGains(Parser *pf)
{
    curr_driver->throttle_p_gain = pf->GetFloat();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
RpAtomic *SetAtomicTexture(RpAtomic *atomic, void *data)
{
    RpMaterial *mat;

    mat = RpGeometryGetMaterial(RpAtomicGetGeometry(atomic), 0);

//    RwTextureDestroy(RpMaterialGetTexture(mat));
    RpMaterialSetTexture(mat, (RwTexture *)data);

    return atomic;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
Driver::Driver()
{
    Parser pf;
    int i, spid;
    MeVector4 q;
    MeReal height;

    curr_driver = this;
    ctrl_method = COMP_CONTROL;

    steering_p_gain =0; //10.0f; //0.03f;
    steering_d_gain =0; // -7.0f; //-0.015; //-0.0083f;
    steering_slip_gain =0; // 1.0f; //0.015f;
    throttle_p_gain =0; // 0.00063f;
    car_data_file[0] = 0;
    car_texture = (RwTexture *)NULL;
    trapped = 0;
    trapped_timer = 0;
    timer_state = 0;
    lap_timer = 0;
    best_time = 0;
    track = 0;

    id = num_drivers;

    /* Parse the .veh file  */
    if(pf.Open(game_vars.driver_files[id]))
    {
        pf.SetComTable(cfg_comtable);
        pf.Parse();

    }
    else
    {
        // ???
    }

    /* Assign the appropriate track and start point */
    for(i=0; i<num_tracks; i++)
    {
        if(!stricmp(game_vars.driver_tracks[id], tracks[i].name))
        {
            track = i;
        }
    }
    spid = LIMITS(0, game_vars.start_point[id], tracks[track].num_points-1);
    curr_point = &tracks[track].course_points[spid];

    num_drivers++;

    car = new Car();
    car->CreatePhysics();
    car->Init(car_data_file);

    height = GetGroundHeight(curr_point->pos.x + Worigin.x, -curr_point->pos.y + Worigin.z) - Worigin.y + car->chassis_height;
    car->SetPosition(curr_point->pos.x, height, -curr_point->pos.y);

    q[0] = MeCos(0.5f*curr_point->hdg); q[1] = 0; q[2] = 0; q[3] = MeSin(0.5f*curr_point->hdg);
    car->SetOrientation(q);

    LoadAndInitShapes();

#ifdef USE_SOUND
//  MeVector3 v;
//  MdtBodyGetPosition(&body[4], v);

    float pos[3] = {0.0, 0.0, 0.0};
    float vel[3] = {0.0, 0.0, 0.0};
    v8snd = PlayWav(0, 0.3f, pos, vel, 1);

    is_skidding = 0;
#endif // USE_SOUND

    curr_driver = (Driver *)NULL;
}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::Reset()
{
    MeReal height;

    car->Reset();
    height = GetGroundHeight(resetpoint.x + Worigin.x, resetpoint.y + Worigin.z) - Worigin.y + car->chassis_height;
    car->SetPosition(resetpoint.x, height, resetpoint.y);
    curr_point= &tracks[track].course_points[27];
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::ResetOnTrackIfOK()
{
    int i,j,pid;
    RwV2d pl_pos, cp;
    MeVector4 q;
    MeReal theta, height;

    /* First check there is no ther car just approaching the point  */
    for(i=0; i<game_vars.num_cars; i++)
    {
        if(i != id && !drivers[i]->trapped) //don't check yourself or other trapped drivers
        {

            if(drivers[i] == player) //dist check on player
            {
                drivers[i]->Get2DPosition(&pl_pos);

                for(j=-2;j<2;j++) {
                    pid = curr_point->id + j;

                    if(pid < 0) pid += tracks[track].num_points;

                    if(pid > tracks[track].num_points-1) pid -= tracks[track].num_points;

                    cp.x = tracks[track].course_points[pid].pos.x;
                    cp.y = tracks[track].course_points[pid].pos.y;

                    if(((pl_pos.x-cp.x)*(pl_pos.x-cp.x)+(pl_pos.y-cp.y)*(pl_pos.y-cp.y)) < 625) return;
                }
            }
            else
            {
                for(j=-2;j<2;j++) {
                    pid = curr_point->id + j;

                    if(pid < 0) pid += tracks[track].num_points;

                    if(pid > tracks[track].num_points-1) pid -= tracks[track].num_points;

                    if(drivers[i]->curr_point->id == pid) return;
                }
            }
        }
    }

    car->Reset();
    height = GetGroundHeight(curr_point->pos.x + Worigin.x, -curr_point->pos.y + Worigin.z) - Worigin.y + car->chassis_height;
    car->SetPosition(curr_point->pos.x, height, -curr_point->pos.y);
    trapped = 0;

    theta = 0.5f*curr_point->hdg;
    q[0] = MeCos(theta);    q[1] = 0; q[2] = 0; q[3] = MeSin(theta);

    car->SetOrientation(q);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
int Driver::OutsideWorld()
{
    RwV3d pos;

    pos.x = (RwReal)car_cmpts[CHASSIS_CMPT].me_mtx[12];
    pos.y = (RwReal)car_cmpts[CHASSIS_CMPT].me_mtx[13];
    pos.z = (RwReal)car_cmpts[CHASSIS_CMPT].me_mtx[14];

    if(pos.x > world_bbox.sup.x - Worigin.x) return 1;
    if(pos.x < world_bbox.inf.x - Worigin.x) return 1;
    if(pos.y > world_bbox.sup.y - Worigin.y) return 1;
    if(pos.y < world_bbox.inf.y - Worigin.y) return 1;
    if(pos.z > world_bbox.sup.z - Worigin.z) return 1;
    if(pos.z < world_bbox.inf.z - Worigin.z) return 1;

    return 0;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
Driver::~Driver()
{
	if(car_texture) RwTextureDestroy(car_texture);
	RpClumpDestroy (internal_shape);  //ian 
    delete car;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
int Driver::CheckIfTrapped()
{
    RwV2d vel;
    Get2DVelocity(&vel);

    if(this == player) return 0; //don't check the player

    if((vel.x*vel.x + vel.y*vel.y < 10) || (car_cmpts[4].me_mtx[5] < 0)) //going v slow or upside down
    {
        if(trapped)
        {
            if(trapped_timer > 5) //if trapped for more than 5 seconds
            {
                //trapped = 0;
                //trapped_timer = 0;
                return 1;
            }
        }
        else
        {
            trapped = 1;
            trapped_timer = 0;
        }
    }
    else if(trapped) trapped = 0;

    return 0;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::Update()
{
    trapped_timer += frame_delta_t; //increment general timer
    lap_timer += frame_delta_t;


    if(CheckIfTrapped()) ResetOnTrackIfOK(); //Reset car if trapped or upside down

    if(OutsideWorld()) Reset(); //Reset car if it has escaped world

    if(GetControlMethod() == COMP_CONTROL) DoControl(); //Computer control

    DoSmoke();

    DoLapTimes();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::DoLapTimes()
{
    RwV2d pos, *cp;
    RwReal d_sqrd;

        Get2DPosition(&pos);

        switch(timer_state)
        {
            default:
            case 0: cp = &tracks[track].course_points[1].pos;   break;
            case 1: cp = &tracks[track].course_points[15].pos;  break;
//          case 2: cp = &tracks[track].course_points[30].pos;  break;
            case 3: cp = &tracks[track].course_points[1].pos;   break;
        }

        d_sqrd = (cp->x-pos.x)*(cp->x-pos.x) + (cp->y-pos.y)*(cp->y-pos.y);

        switch(timer_state)
        {
        case 0:
            if(d_sqrd < 1000)
            {
                timer_state = 1;
            }
            else lap_timer = 0;
            break;
        case 1:
            if(d_sqrd < 5000)
            {
                timer_state = 3;
            }
            break;
//      case 2:
//          if(d_sqrd < 5000)
//          {
//              timer_state = 3;
//          }
//          break;
        case 3:
            if(d_sqrd < 1000)
            {
                timer_state = 1;
                if(best_time == 0 || lap_timer < best_time)
                {
                    best_time = lap_timer;
                    if(race_best == 0 || best_time < race_best) race_best = best_time;
                }
                lap_timer = 0;
            }
            break;
        default:
            timer_state = 0;
            break;
        }


}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::SetControlMethod(CTRL_METHOD m)
{
    ctrl_method = m;
    if (m == COMP_CONTROL)
        car->SetTrannyType(2);
    else
        car->SetTrannyType(2);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::TurnOffCarShape()
{
    int i;
    RpClump *clump;

    for(i=0;i<NUM_CAR_CMPTS;i++)
    {
        clump = car_cmpts[i].clump;
        if(RpClumpGetWorld(clump))
        {
            RpWorldRemoveClump(World, clump);
        }
    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::TurnOnCarShape()
{
    int i;
    RpClump *clump;

    for(i=0;i<NUM_CAR_CMPTS;i++)
    {
        clump = car_cmpts[i].clump;
        if(!RpClumpGetWorld(clump))
        {
            RpWorldAddClump(World, clump);
        }
    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::Get2DPosition(RwV2d *pos)
{
    pos->x = (RwReal)car->GetBodyPosCmpt(CHASSIS_CMPT,0);
    pos->y = (RwReal)car->GetBodyPosCmpt(CHASSIS_CMPT,1);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::Get2DVelocity(RwV2d *vel)
{
    vel->x = (RwReal)car->GetBodyVelCmpt(CHASSIS_CMPT,0);
    vel->y = (RwReal)car->GetBodyVelCmpt(CHASSIS_CMPT,1);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
RwReal Driver::GetMotorRPM()
{
 return (RwReal)car->GetMotorRPM();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::Upshift()
{
    car->Upshift();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::Downshift()
{
    car->Downshift();
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::DoSmoke()
{
    RwReal vel_mag, sideslip;
    RwV3d pos,vel;
    RwV2d curr_vel;
    int wheel_skid = 0;
    Get2DVelocity(&curr_vel);
    MdtCar *car = GetCar();

    if(paused) return;

    vel_mag = (RwReal)sqrt(curr_vel.x*curr_vel.x + curr_vel.y*curr_vel.y);

    if(vel_mag < 50.0f && !is_skidding) return;

    /* Insert Smoke on tyres */
    sideslip = (RwReal)MeFabs(car->GetWheelSlipAng(REAR_L_CMPT));

    if(sideslip > 0.25f && car->WheelOnTrack(REAR_L_CMPT)) {
        pos.x = Worigin.x + (RwReal)car->GetBodyPosCmpt(REAR_L_CMPT,0);
        pos.y = Worigin.y + (RwReal)car->GetBodyPosCmpt(REAR_L_CMPT,2) - (RwReal)car->car_data.rear_wheel_radius;
        pos.z = Worigin.z - (RwReal)car->GetBodyPosCmpt(REAR_L_CMPT,1);
        vel.x = (RwReal)car->GetBodyVelCmpt(REAR_L_CMPT,0);
        vel.y = (RwReal)car->GetBodyVelCmpt(REAR_L_CMPT,2) + 0.1f*vel_mag;
        vel.z = (RwReal)-car->GetBodyVelCmpt(REAR_L_CMPT,1);
        SmokeParticleCreate(&pos, &vel);
        wheel_skid = 1;
    }

    sideslip = (RwReal)MeFabs(car->GetWheelSlipAng(REAR_R_CMPT));

    if(sideslip > 0.25f && car->WheelOnTrack(REAR_R_CMPT)) {
        pos.x = Worigin.x + (RwReal)car->GetBodyPosCmpt(REAR_R_CMPT,0);
        pos.y = Worigin.y + (RwReal)car->GetBodyPosCmpt(REAR_R_CMPT,2) - (RwReal)car->car_data.rear_wheel_radius;
        pos.z = Worigin.z - (RwReal)car->GetBodyPosCmpt(REAR_R_CMPT,1);
        vel.x = (RwReal)car->GetBodyVelCmpt(REAR_R_CMPT,0);
        vel.y = (RwReal)car->GetBodyVelCmpt(REAR_R_CMPT,2) + 0.1f*vel_mag;
        vel.z = (RwReal)-car->GetBodyVelCmpt(REAR_R_CMPT,1);
        SmokeParticleCreate(&pos, &vel);
        wheel_skid = 1;
    }


#if USE_SOUND
    if(wheel_skid) {
        if(!is_skidding) {
            RwV3d campos = *RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(game_cam.cam)));
            RwV3d carpos = *RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(car_cmpts[4].clump)));

            float pos2[3] = {carpos.x - campos.x, carpos.y - campos.y, carpos.z - campos.z};
            float vel[3] = {0, 0, 0};
            channel1 = PlayWav(3, 1.0, pos2, vel, 1);
            is_skidding = 1;
        } else {
            RwV3d campos = *RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(game_cam.cam)));
            RwV3d carpos = *RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(car_cmpts[4].clump)));

            float pos2[3] = {carpos.x - campos.x, carpos.y - campos.y, carpos.z - campos.z};
            SetSoundPosition(channel1, pos2);

        }
    }
    else if(is_skidding) {
        StopSound(channel1);
        is_skidding = 0;
    }
#endif // USE_SOUND

}
/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::SetCurrentPointOnCourse()
{
    RwV2d *curr_pt_pos = &curr_point->pos;
    RwV2d *next_pt_pos = &tracks[track].course_points[curr_point->next_id].pos;
    RwV2d curr_pos, curr_vel, temp;
    RwReal dist_sqrd, vel_cmpt;
    RwReal time_to_curr_pt, time_to_next_pt;

    /* Get the current car state    */
    Get2DPosition(&curr_pos);
    Get2DVelocity(&curr_vel);

    /* Calculate time to current point if within tolerence  */
    RwV2dSub(&temp, curr_pt_pos, &curr_pos);
    dist_sqrd = RwV2dDotProduct(&temp, &temp);
    if(dist_sqrd < curr_point->tol1*curr_point->tol1) {
        if(dist_sqrd < curr_point->tol2*curr_point->tol2) {
            curr_point = &tracks[track].course_points[curr_point->next_id]; //Set current point to next point
            return;
        } else {
            vel_cmpt = RwV2dDotProduct(&temp, &curr_vel); //Velocity component towards point
            if(vel_cmpt > 0) {
                time_to_curr_pt = dist_sqrd / vel_cmpt;
            } else {
                curr_point = &tracks[track].course_points[curr_point->next_id]; //Set current point to next point
                return;
            }

            /* Calculate time to next point */
            RwV2dSub(&temp, next_pt_pos, &curr_pos);
            vel_cmpt = RwV2dDotProduct(&temp, &curr_vel); //Velocity component towards point
            if(vel_cmpt > 0) {
                time_to_next_pt = RwV2dDotProduct(&temp, &temp) / vel_cmpt;
            } else {
                return; //maintain current point
            }

            if(time_to_next_pt < time_to_curr_pt) {
                curr_point = &tracks[track].course_points[curr_point->next_id]; //Set current point to next point
                return;
            }
        }

    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::DriveToCurrentPoint()
{
    RwReal hdg_to_pt, vel_mag, curr_hdg, turn_rate, delta_hdg, slip, dist;
    RwV2d curr_pos, curr_vel, aim_pos;
    RwReal p_input, d_input, slip_input=0, steer_input;

    Get2DPosition(&curr_pos);
    Get2DVelocity(&curr_vel);

    vel_mag = (RwReal)sqrt(curr_vel.x*curr_vel.x + curr_vel.y*curr_vel.y);
    turn_rate = (RwReal)car->GetBodyAngVelCmpt(4,2);
    if(vel_mag > 1) {
        slip = (RwReal)car->GetBodySlipAng();
    } else {
        slip = 0.0f;
    }
    //hdg_to_pt = (RwReal)MeAtan2(curr_point->pos.y - curr_pos.y, curr_point->pos.x - curr_pos.x);
    RwV2dSub(&aim_pos, &curr_point->pos, &curr_pos);
    dist = RwV2dLength(&aim_pos)*0.0f;
    dist = MIN(dist, 50.0f);
    aim_pos.x = curr_point->pos.x - dist*(RwReal)MeCos(curr_point->hdg);
    aim_pos.y = curr_point->pos.y - dist*(RwReal)MeSin(curr_point->hdg);
    hdg_to_pt = (RwReal)MeAtan2(aim_pos.y - curr_pos.y, aim_pos.x - curr_pos.x);

    curr_hdg = (RwReal)MeAtan2(curr_vel.y, curr_vel.x);
    delta_hdg = (SmallestAngleRads(curr_hdg, hdg_to_pt));
//  steer_input = ((1.0f+steering_p_gain*frame_delta_t)*delta_hdg //tuned between 20Hz and 60Hz
//                  + turn_rate*steering_d_gain*frame_delta_t
//                  + slip*steering_slip_gain);
    p_input = (RwReal)(steering_p_gain+steering_tcp_gain*frame_delta_t)*delta_hdg;
    d_input = (RwReal)(turn_rate*steering_d_gain*frame_delta_t);

    if(car->GetBodyForwardVel() > 0) slip_input = slip*(RwReal)steering_slip_gain;
    steer_input = p_input + d_input + slip_input;
    SetSteering(steer_input); // /frame_delta_t);

#ifdef _DEBUG_WINDOW
    if(id == game_cam.view_driver)
    {
        DebugChannel(0,frame_delta_t, p_input);
        DebugChannel(1,frame_delta_t, -car->GetWheelSteerAngle(0));
        DebugChannel(2,frame_delta_t, d_input);
        DebugChannel(3,frame_delta_t, slip_input);

    }
#endif
    if(car->OnTrack())
    {
//      if(id>0 && drivers[id-1]->curr_point->id < curr_point->id+3)
//      {
//          SetThrottle(throttle_p_gain*((curr_point->spd-5.0f*id)-vel_mag)/frame_delta_t);
//      }
//      else
//      {
            SetThrottle(throttle_p_gain*(curr_point->spd - vel_mag)/frame_delta_t);
//      }
    }
    else
    {
        if(car->OnGround())
        {
            SetThrottle(throttle_p_gain*(40.0f - vel_mag)/frame_delta_t);
        }
    }
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::DoControl()
{
    SetCurrentPointOnCourse(); // Set current point to most appropriate point to head for

    DriveToCurrentPoint(); //Control Steering and Speed to get to point
}

/******************************************************************************
*
*
*
********************************************************************************/
RwBool Driver::LoadAndInitShapes(void)
{
    RwInt32 i;

    const MeReal *me_mtx;

    for(i = 0; i < NUM_CAR_CMPTS; i++)
    {

        /* Set the chassis texture  */
#if 1
        if(i == 4 && car_texture && car_cmpts[i].clump)
        {
            RpClumpForAllAtomics(car_cmpts[i].clump, SetAtomicTexture, (void*)car_texture);
          //  if(internal_shape)
            //   RpClumpForAllAtomics(internal_shape, SetAtomicTexture, (void*)car_texture);//used to apply the given callback function to all atomics in the specified clump

            //RpWorldRemoveClump(World, car_cmpts[i].clump); //for testing

        }
#endif
        /* Set up pointer to ME body transformation matrix  */
        me_mtx = car->GetCmptMeTransformationMatrix(i);

        //RpWorldRemoveClump(World, car_cmpts[i].clump); //for testing
        car_cmpts[i].me_mtx = me_mtx;

    }

    return TRUE;
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
static void SetShapeMatrix(RwMatrix *rw_mtx, const MeReal *me_mtx)
{
    rw_mtx->right.x = (RwReal)me_mtx[0];
    rw_mtx->right.y = (RwReal)me_mtx[1];
    rw_mtx->right.z = (RwReal)me_mtx[2];
    rw_mtx->up.x    = (RwReal)me_mtx[4];
    rw_mtx->up.y    = (RwReal)me_mtx[5];
    rw_mtx->up.z    = (RwReal)me_mtx[6];
    rw_mtx->at.x    = (RwReal)me_mtx[8];
    rw_mtx->at.y    = (RwReal)me_mtx[9];
    rw_mtx->at.z    = (RwReal)me_mtx[10];
    rw_mtx->pos.x   = (RwReal)me_mtx[12];
    rw_mtx->pos.y   = (RwReal)me_mtx[13];
    rw_mtx->pos.z   = (RwReal)me_mtx[14];

    //printf(" %d %d %d \n",(int)rw_mtx->pos.x,(int)rw_mtx->pos.y,(int)rw_mtx->pos.z);
}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::UpdateShapes(void)
{
    RwMatrix obj_mtx;
    RwFrame *frame;
    RwInt32 i;

//  car->PostEvolve();
    GetCar()->Update();

    /* Modify the position of the clump */
    for(i = 0; i < NUM_CAR_CMPTS; i++)
    {
        frame = RpClumpGetFrame(car_cmpts[i].clump);
        //obj_mtx = RwFrameGetMatrix(frame);
        SetShapeMatrix(&obj_mtx, car_cmpts[i].me_mtx);
        RwMatrixTranslate(&obj_mtx, &Worigin, rwCOMBINEPOSTCONCAT);
        /* Add the world offsets to the clump position  */
        RwFrameTransform(frame, &obj_mtx, rwCOMBINEREPLACE);
    }

#ifdef USE_SOUND
//  MeVector3 v;
//  MdtBodyGetPosition(&body[4], v);
    RwV3d campos = *RwMatrixGetPos(RwFrameGetMatrix(RwCameraGetFrame(game_cam.cam)));
    RwV3d carpos = *RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(car_cmpts[4].clump)));

    //float pos1[3] = {campos.x, campos.y, campos.z};
    //SetListenerPosition(pos1);
    //float vel[3] = {0.0, 0.0, 0.0};
    float pos2[3] = {carpos.x - campos.x, carpos.y - campos.y, carpos.z - campos.z};
    SetSoundPosition(v8snd, pos2);

    RwReal rpm = GetMotorRPM();


    float rFreq;
    float soundratio = 4000.0;
    if(rpm > 8500)
        rpm = 8500;

    if(rpm <= 600.0 )
        rFreq = 600.0f/soundratio;
    else
        rFreq = rpm/soundratio;
/*
    {
        char d[256];
        sprintf(d, "Wheelspeed = %f  Freq = %f\n", WheelSpeed, rFreq);
        OutputDebugString(d);

    }
*/

    SetSoundFrequency(v8snd, 0 /* bad hack */, rFreq);


#endif // USE_SOUND


}

/***********************************************************************************************
*
*
*
************************************************************************************************/
void Driver::SetSpeedo()
{
    RwReal spd;
    RwV2d v;

    Get2DVelocity(&v);

    spd = (RwReal)MeSqrt(v.x*v.x + v.y*v.y); //ft/sec
    SetMeterArc(&speedo, 1.4f * spd*0.686f);

    spd = GetMotorRPM();
    SetMeterArc(&tacho, spd);
}
