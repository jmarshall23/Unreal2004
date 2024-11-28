/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

    $Date: 2002/04/04 15:29:36 $ $Revision: 1.28.4.6 $

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
  This example demonstrates a prismatic joint. Like a hinge joint,
  a prismatic joint can have limits and be motorized.
*/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <Mdt.h>
#include <MeViewer.h>

RRender *rc;
RMenu   *menu;

#define NBODIES (2)

#define POLE_LENGTH     50.0f
#define POLE_Y           0.0f

/* User help is displayed by pressing F1 */
#define NUM_HELP_ITEMS 1
char *help[] =
{
    "$ACTION1 - Menu Options"
};

float color[NBODIES][4] = { {0, 0.6f, 0.9f, 1}, {0, 0.9f, 0.6f, 1} };

/* World for simulation */
MdtWorldID world;

/* Dynamics body corresponding to cubeG */
MdtBodyID body[NBODIES];

/* Graphics for bodies */
RGraphic *cube[NBODIES];

/* Prismatic joint */
MdtPrismaticID prism;

/* Timestep of the simulation */
MeReal step = (MeReal) 0.03f;

#define HI_LIMIT 20
#define LO_LIMIT -20

/* Cylinder */
RGraphic *pole;
float poleColor[4] = {0.0f, 0.0f, 0.5f, 1.0f};

/* Switched on during initialisation. */
int direction = 1;

const MeReal defaultVelocity = 10.0f;
MeReal desiredVelocity;

const MeReal defaultStiffness = 20.0f;
MeReal limitStiffness;

const MeReal defaultDamping = 1.0f;
MeReal limitDamping;

const MeReal defaultRestitution = 0.2f;

const MeReal defaultMaxForce = 2.0f;
MeReal maxForce;

MeBool hardLimits = 1;
MeBool motorOn = 1;

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI Tick(RRender * rc, void* userdata)
{
#if 0
    const MdtLimitID Limit = MdtPrismaticGetLimit(prism);
    const MdtSingleLimitID LowerLimit = MdtLimitGetLowerLimit(Limit);
    const MeReal Stiffness = MdtSingleLimitGetStiffness(LowerLimit);
    const char *const szCaption4 = "Limit stiffness: ";
#endif

    /* These timer calls are for the OpenGL performance bar */
    MeProfileStartSection("Dynamics",0);

    /* evolve the world */
    MdtWorldStep(world, step);
    MeProfileEndSection("Dynamics");
}

/* RMenu Handlers */
void ToggleLimit(void)
{
    MdtLimitID limit = MdtPrismaticGetLimit(prism);

    MdtLimitActivateLimits(limit, !MdtLimitIsActive(limit));
}

void MEAPI ToggleMotor(MeBool on)
{
    MdtLimitID limit = MdtPrismaticGetLimit(prism);
    motorOn = on;
    MdtLimitActivateMotor(limit, motorOn);
}

void MEAPI SwitchDirection(MeBool on)
{
    direction *= -1;
    MdtLimitSetLimitedForceMotor(MdtPrismaticGetLimit(prism),
        direction * desiredVelocity, maxForce);
    MdtLimitActivateMotor(MdtPrismaticGetLimit(prism), motorOn);
}

void MEAPI SetDesiredVelocity(MeReal value)
{
    desiredVelocity = value;
    MdtLimitSetLimitedForceMotor(MdtPrismaticGetLimit(prism),
        direction * desiredVelocity, maxForce);
    MdtLimitActivateMotor(MdtPrismaticGetLimit(prism), motorOn);
}

void MEAPI SetMaxForce(MeReal value)
{
    maxForce = value;
    MdtLimitSetLimitedForceMotor(MdtPrismaticGetLimit(prism),
        direction * desiredVelocity, maxForce);
    MdtLimitActivateMotor(MdtPrismaticGetLimit(prism), motorOn);
}

void MEAPI ToggleHardLimits(MeBool on)
{
    MdtLimitID limit = MdtPrismaticGetLimit(prism);

    hardLimits = on;

    if(hardLimits)
    {

        MdtSingleLimitSetStiffness(MdtLimitGetLowerLimit(limit), MEINFINITY);
        MdtSingleLimitSetStiffness(MdtLimitGetUpperLimit(limit), MEINFINITY);
    }
    else
    {
        MdtSingleLimitSetStiffness(MdtLimitGetLowerLimit(limit),
            limitStiffness);
        MdtSingleLimitSetStiffness(MdtLimitGetUpperLimit(limit),
            limitStiffness);
    }
}

void MEAPI SetLimitStiffness(MeReal value)
{
    MdtLimitID limit = MdtPrismaticGetLimit(prism);

    limitStiffness = value;

    if(hardLimits)
    {
        MdtSingleLimitSetStiffness(MdtLimitGetLowerLimit(limit), MEINFINITY);
        MdtSingleLimitSetStiffness(MdtLimitGetUpperLimit(limit), MEINFINITY);
    }
    else
    {
        MdtSingleLimitSetStiffness(MdtLimitGetLowerLimit(limit),
            limitStiffness);
        MdtSingleLimitSetStiffness(MdtLimitGetUpperLimit(limit),
            limitStiffness);
    }

}

void MEAPI SetLimitDamping(MeReal value)
{
    MdtLimitID limit = MdtPrismaticGetLimit(prism);

    limitDamping = value;

    MdtSingleLimitSetDamping(MdtLimitGetLowerLimit(limit), limitDamping);
    MdtSingleLimitSetDamping(MdtLimitGetUpperLimit(limit), limitDamping);
}

void MEAPI SetLimitRestitution(MeReal value)
{
    MdtLimitID limit = MdtPrismaticGetLimit(prism);

    MdtSingleLimitSetRestitution(MdtLimitGetLowerLimit(limit), value);
    MdtSingleLimitSetRestitution(MdtLimitGetUpperLimit(limit), value);
}


void MEAPI_CDECL cleanup(void)
{
    MdtWorldDestroy(world);
    RRenderContextDestroy(rc);
    RMenuDestroy(menu);
}


/* Main Routine */
int MEAPI_CDECL main(int argc, const char **argv)
{
    MdtLimitID limit = 0;
    int i;
    MeCommandLineOptions* options;

    /* Initialise dynamics */
    world = MdtWorldCreate(2, 1, 1, 1);
    MdtWorldSetAutoDisable(world,0);

    /* Initialise the bodies attached by this joint: */

    body[0] = MdtBodyCreate(world); /* the pole */
    MdtBodySetPosition(body[0], 0.0f, POLE_Y, -10.0f);

    for (i = 1; i < NBODIES; i++)
    {
        body[i] = MdtBodyCreate(world);
        /* Both bodies start in the same place */
        MdtBodySetPosition(body[i], 0.0f, 0.0f, -10.0f);
    }

    prism = MdtPrismaticCreate(world);
    MdtPrismaticSetBodies(prism,body[0],body[1]);
    limit = MdtPrismaticGetLimit(prism);

    /* Prismatic joints do not have a SetPosition() function */
    MdtPrismaticSetAxis(prism, 0.0f, 1.0f, 0.0f);

    /* Set high and low extension limits */
    MdtSingleLimitSetStop(MdtLimitGetLowerLimit(limit), LO_LIMIT);
    MdtSingleLimitSetStop(MdtLimitGetUpperLimit(limit), HI_LIMIT);

    MdtLimitActivateLimits(limit, 1);

    for (i = 0; i < NBODIES; i++)
        MdtBodyEnable(body[i]);

    MdtPrismaticEnable(prism);

    /* Initialise rendering attributes */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;

    RCameraSetView(rc,70.0f,0.4f,0.4f);
    RCameraUpdate(rc);

    for (i = 0; i < NBODIES; i++)
    {
        cube[i] = RGraphicBoxCreate(rc,
            (MeReal)10 - i,(MeReal)5 - i,(MeReal)10 - i,
            color[i], 0);
        RGraphicSetTransformPtr(cube[i], MdtBodyGetTransformPtr(body[i]));
        RGraphicSetTexture(rc, cube[i], "stone");
    }

    pole = RGraphicBoxCreate(rc, 2.0f, POLE_LENGTH, 2.0f,
        poleColor, MdtBodyGetTransformPtr(body[0]));

    /* Set up control menu */
    SetDesiredVelocity(defaultVelocity);
    SetMaxForce(defaultMaxForce);
    SetLimitStiffness(defaultStiffness);
    SetLimitDamping(defaultDamping);
    SetLimitRestitution(defaultRestitution);


    menu = RMenuCreate(rc, "Main Menu");
    RMenuAddToggleEntry(menu, "Motor On", ToggleMotor, 1);
    RMenuAddToggleEntry(menu, "Forward Direction", SwitchDirection, 1);
    RMenuAddValueEntry(menu, "Desired Velocity",
        SetDesiredVelocity, 20, 0, 1, defaultVelocity);
    RMenuAddValueEntry(menu, "Max Force",
        SetMaxForce, 30, 0, 1, defaultMaxForce);
    RMenuAddToggleEntry(menu, "Hard Limits", ToggleHardLimits, 1);
    RMenuAddValueEntry(menu, "Limit Stiffness",
        SetLimitStiffness, 60, 0, 1, defaultStiffness);
    RMenuAddValueEntry(menu, "Limit Damping ",
        SetLimitDamping, 10, 0, 1, defaultDamping);
    RMenuAddValueEntry(menu, "Limit Restitution ",
        SetLimitRestitution, 1, 0, (MeReal)0.1, defaultRestitution);

    RRenderSetDefaultMenu(rc, menu);
    RMenuDisplay(menu);

    RRenderSetWindowTitle(rc,"Prismatic example");
    RRenderCreateUserHelp(rc, help, NUM_HELP_ITEMS);
    RPerformanceBarCreate(rc);

    /*
       Cleanup after simulation.O
     */
#ifndef PS2
    atexit(cleanup);
#endif
    /*
       Run the Simulation.
     */

    /*
       RRun() executes the main loop.

       Pseudocode: while no exit-request { Handle user input call Tick() to
       evolve the simulation and update graphic transforms Draw graphics }
     */

    RRun(rc, Tick, 0);

    return 0;
}
