/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:32 $ - Revision: $Revision: 1.21.6.2 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
   intellectual property rights in them belong to MathEngine. All
   rights conferred by law (including rights under international
   copyright conventions) are reserved to MathEngine. This software
   may also incorporate information which is confidential to
   MathEngine.

   Save to the extent permitted by law, or as otherwise expressly
   permitted by MathEngine, this software and the manuals must not
   be copied (in whole or in part), re-arranged, altered or adapted
   in any way without the prior written consent of the Company. In
   addition, the information contained in the software may not be
   disseminated without the prior written consent of MathEngine.

 */

/*
  This example shows the use of the car wheel joint. A car wheel joint
  can be used for all four wheels and models both steering and
  suspension.

  The car is mouse controlled. Left click and mouse up/down accelerates
  or reverses the car. Left click and mouse left/right steers the car.

  The evolution of the simulation can be turned on and off by pressing
  'a'.  When the simulation not being continously evolved it can be
  stepped one simulation step at a time by pressing 's'.
*/

#include <malloc.h>
#include "BoxCar.h"

/*
  Global variables.
*/

/* time stepsize */
const MeReal timeStep = (MeReal)(0.02);

/* gravity */
#define GRAV_MAG (MeReal)(9.81)

/* gravity  magnitude */
const MeReal grav_mag = GRAV_MAG;

/* gravity  vector */
const MeReal gravity[3] = { 0, 0, -GRAV_MAG };

/* simulation object */
MdtWorldID world;

/* car physics object */
Car car1;
MeReal pad_drive, pad_steer;
/*
  Initialize physics memory and parameters.
*/
void MdtInit(MdtWorldID world)
{
    MdtWorldSetEpsilon(world, (MeReal)(0.01));
    MdtWorldSetGamma(world, (MeReal)(0.2));
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
}


int autoEvolve = 1;

void MEAPI stepEvolve(RRender *rc, void *user_data)
{
    MeProfileStartSection("Dynamics",0);
    /* step the world by timeStep seconds */
    MdtWorldStep(world, timeStep);
    MeProfileEndSection("Dynamics");

    CarUpdate(&car1);
    CarCameraTrack(&car1);
    RCameraUpdate(rc);
}

static void MEAPI Tick(RRender *rc, void *user_data)
{
    if (autoEvolve)
        stepEvolve(rc,user_data);
}

void SetCarData1(CarData * c1)
{

    MeReal mrr;

    c1->wheelbase = 1.8f;
    c1->front_track = 1.5f;
    c1->rear_track = 1.75f;
    c1->front_wheel_radius = 0.3f;
    c1->rear_wheel_radius = 0.35f;
    c1->chassis_height_off_ground = 0.55f;

    c1->chassis_mass = 10;
    c1->chassis_CoM_upward_offset = -0.55f;
    c1->chassis_CoM_forward_offset = 0;

    mrr = c1->chassis_mass * c1->wheelbase * c1->front_track;

    c1->chassis_MoI_roll = 0.1f * mrr;
    c1->chassis_MoI_pitch = 0.25f * mrr;
    c1->chassis_MoI_yaw = 0.15f * mrr;

    c1->wheel_mass = 1;
    mrr = c1->wheel_mass * c1->front_wheel_radius * c1->front_wheel_radius;
    c1->wheel_MoI_axial = 1 * mrr;
    c1->wheel_MoI_radial = 1 * mrr;

    c1->front_suspension_travel = 0.25f;
    c1->rear_suspension_travel = 0.25f;
    c1->front_suspension_damp = 1.0f;
    c1->rear_suspension_damp = 1.0f;
    c1->front_suspension_equi = 0.5f;
    c1->rear_suspension_equi = 0.5f;
    c1->front_suspension_ztos = 1.2f;
    c1->rear_suspension_ztos = 1.2f;
    c1->front_suspension_soft = 0.02f;
    c1->rear_suspension_soft = 0.02f;

    c1->max_steering_angle = 45 * ME_PI / 180;

    c1->front_contact_softness = 0.01f;
    c1->rear_contact_softness = 0.01f;
    c1->front_contact_lateral_slip = 0.02f;
    c1->rear_contact_lateral_slip = 0.05f;

    c1->chassis_length = 2.3f;
    c1->chassis_width = 0.8f;
    c1->chassis_depth = 0.5f;
    c1->chassis_forward_offset = -0.05f;
    c1->front_wheel_width = 0.15f;
    c1->rear_wheel_width = 0.2f;
}

void MEAPI TurnRight(RRender *rc, void *user_data)
{
    pad_steer += 0.25f;
    if(pad_steer > 1.0f) pad_steer = 1.0f;
}
void MEAPI TurnLeft(RRender *rc, void *user_data)
{
    pad_steer -= 0.25f;
    if(pad_steer < -1.0f) pad_steer = -1.0f;
}
void MEAPI Accelerate(RRender *rc, void *user_data)
{
    pad_drive += 0.25f;
    if(pad_drive > 1.0f) pad_drive = 1.0f;
}
void MEAPI Brake(RRender *rc, void *user_data)
{
    pad_drive -= 0.25f;
    if(pad_drive > 1.0f) pad_drive = 1.0f;
}
void MEAPI toggleAutoEvolve(RRender *rc, void *user_data)
{
    autoEvolve = !autoEvolve;
}

void MEAPI_CDECL cleanup(void)
{
    MdtWorldDestroy(world);
    GraphicsDelete();
}

int MEAPI_CDECL main(int argc, const char **argv)
{
    static char *help[] =
    {
        "$LEFT  to steer left",
        "$RIGHT to steer right",
        "$UP    to accelerate",
        "$DOWN  to brake",
        "$UP2   to toggle auto-evolve",
        "$DOWN2 to step evolve"
    };
    const int helpNum = sizeof (help) / sizeof (help[0]);

    /* data structure for car parameters */
    CarData c1;

    /* allocate a pool of memory for Kea to use when solving */
    world = MdtWorldCreate(10, 10, 1, 1);

    /* initialize physics world */
    MdtInit(world);
    /* initialize mini-renderer */
    if (!GraphicsInit(argc,argv))
        return 1;
    /* draw grid on ground plane */
    GroundGraphics();

    /* initialize car parameters */
    SetCarData1(&c1);
    CarInit(&car1, world, gravity, &c1);

    CarCreateGraphics(&car1);

    /* disable mouse control of camera */
/*    REnableMouseCam(0);*/

    RRenderSetRightCallBack(rc, TurnRight, 0);
    RRenderSetLeftCallBack(rc, TurnLeft, 0);
    RRenderSetUpCallBack(rc, Accelerate, 0);
    RRenderSetDownCallBack(rc, Brake, 0);
    RRenderSetLeft2CallBack(rc, toggleAutoEvolve, 0);
    RRenderSetRight2CallBack(rc, stepEvolve, 0);

    RRenderCreateUserHelp(rc, help, helpNum);
    RRenderSetWindowTitle(rc, "BoxCar tutorial");
    RPerformanceBarCreate(rc);

#ifndef PS2
    atexit(cleanup);
#endif

    /* pass control to the renderer loop */
    RRun(rc, Tick, 0);

    return 0;
}
