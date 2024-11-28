/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

    $Date: 2002/04/04 15:29:34 $ $Revision: 1.43.8.3 $

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
  This example demonstrates a hinge joint. A body is connected to the
  static environment via a hinge joint. This means that every degree of
  freedom is constrained apart from its rotation.

  The hinge can be motorized and rotational limits can be imposed.
*/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <Mdt.h>
#include <MeViewer.h>

/* Render context */
RRender *rc;

/* box graphic   */
RGraphic *bodyG;

/* bar graphics */
MeMatrix4 axleTM;
RGraphic* axleG;

MeMatrix4 refLineTM;
RGraphic* refLineG;

RMenu* menu;

/* RGB+A colour */
float color[2][4] = { {1, 1, 1, 1}, {0, 0, 1, 1} };

/* Switched on during initialisation. */
int direction = 1;

const MeReal defaultVelocity = 10;
MeReal desiredVelocity;

const MeReal defaultStiffness = 20;
MeReal limitStiffness;

const MeReal defaultDamping = 1;
MeReal limitDamping;

const MeReal defaultRestitution = (MeReal)0.2;

const MeReal defaultMaxForce = 2;
MeReal maxForce;

MeBool hardLimits = 1;
MeBool motorOn = 1;


/* Use one body by default. */
unsigned int NumBodies = 1;


/*
  Here were are defining the joint's high and low limits. If the limits
  are enabled via a call to MdtHingeSetLimits() the cube will not rotate
  further than the specified angles.

  The range of the limit is PI < limit < PI in either direction.
*/
#define HI_LIMIT (3*ME_PI)
#define LO_LIMIT (-3*ME_PI)

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;

/* Dynamics bodies corresponding to bodyG */
MdtBodyID body;

/* hinge joint */
MdtHingeID hinge;

/* the world's timestep */
MeReal step = (MeReal)(0.02);

/*
  tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender * rc, void* userData)
{
    /* By default the world will be partitioned */
    MeProfileStartSection("Dynamics", 0);
    MdtWorldStep(world, step);
    MeProfileEndSection("Dynamics");
}

void ToggleLimit(void)
{
    MdtLimitID limit = MdtHingeGetLimit(hinge);

    MdtLimitActivateLimits(limit, !MdtLimitIsActive(limit));
}

void MEAPI ToggleMotor(MeBool on)
{
    MdtLimitID limit = MdtHingeGetLimit(hinge);
    motorOn = on;
    MdtLimitActivateMotor(limit, motorOn);
}

void MEAPI SwitchDirection(MeBool on)
{
    direction *= -1;
    MdtLimitSetLimitedForceMotor(MdtHingeGetLimit(hinge), 
        direction * desiredVelocity, maxForce);
    MdtLimitActivateMotor(MdtHingeGetLimit(hinge), motorOn);
}

void MEAPI SetDesiredVelocity(MeReal value)
{
    desiredVelocity = value;
    MdtLimitSetLimitedForceMotor(MdtHingeGetLimit(hinge), 
        direction * desiredVelocity, maxForce);
    MdtLimitActivateMotor(MdtHingeGetLimit(hinge), motorOn);
}

void MEAPI SetMaxForce(MeReal value)
{
    maxForce = value;
    MdtLimitSetLimitedForceMotor(MdtHingeGetLimit(hinge), 
        direction * desiredVelocity, maxForce);
    MdtLimitActivateMotor(MdtHingeGetLimit(hinge), motorOn);
}

void MEAPI ToggleHardLimits(MeBool on)
{
    MdtLimitID limit = MdtHingeGetLimit(hinge);

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
    MdtLimitID limit = MdtHingeGetLimit(hinge);

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
    MdtLimitID limit = MdtHingeGetLimit(hinge);

    limitDamping = value;

    MdtSingleLimitSetDamping(MdtLimitGetLowerLimit(limit), limitDamping);
    MdtSingleLimitSetDamping(MdtLimitGetUpperLimit(limit), limitDamping);
}

void MEAPI SetLimitRestitution(MeReal value)
{
    MdtLimitID limit = MdtHingeGetLimit(hinge);

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
    MeMatrix3 R;
    MeVector3 pos;

    static char *help[] =
    {
        "$ACTION1 - toggle options menu.",
    };

    /* initialize mini-renderer */
    MeCommandLineOptions* options;
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

    world = MdtWorldCreate(1, 1, 1, 1);
    MdtWorldSetAutoDisable(world,0);

    /* Initialise the bodies attached by this joint:    */
    body = MdtBodyCreate(world);
    MdtBodySetLinearVelocity(body, 0, 0, 0);
    MdtBodySetAngularVelocity(body, 0, 0, 0);
    MdtBodyEnable(body);

    MdtBodySetAngularVelocityDamping(body, (MeReal)0.05);

    /* Initialise corresponding graphic: */
    bodyG = RGraphicBoxCreate(rc, 4, 2, 4, color[0], 
        MdtBodyGetTransformPtr(body));
    RGraphicSetTexture(rc, bodyG, "stone");

    RRenderSetWindowTitle(rc, "Hinge example");
    RRenderCreateUserHelp(rc, help, 1);
    RRenderToggleUserHelp(rc);

    MdtBodySetPosition(body, -5, 0, 0);
    MdtBodySetQuaternion(body, 1, 0, 0, 0);

    /*
       The hinge is either between an MdtBody and the static environment
       or between two MdtBody's.
     */
    hinge = MdtHingeCreate(world);
    MdtHingeSetBodies(hinge, body, 0);
    limit = MdtHingeGetLimit(hinge);
    MdtHingeSetPosition(hinge, 0, 0, 0);
    MdtHingeSetAxis(hinge, 1, 0, 0);

    MeMatrix4TMMakeIdentity(axleTM);
    MeMatrix3MakeRotationY(R, ME_PI/2);
    MeMatrix4TMMakeFromRotationAndPosition(axleTM, R, -5, 0, 0);

    axleG = RGraphicCylinderCreate(rc, 
        (MeReal)0.3, (MeReal)5, color[1], axleTM);

    MeMatrix4TMMakeIdentity(refLineTM);
    MeMatrix4TMSetPosition(refLineTM, -7.5, 0, 0);

    refLineG = RGraphicCylinderCreate(rc, 
        (MeReal)0.3, 10, color[1], refLineTM);

    ToggleMotor(1);             /* Turn motor on. */
    ToggleLimit();             /* Turn limits on. */

    /* Set high and low extension limits */
    MdtSingleLimitSetStop(MdtLimitGetLowerLimit(limit), LO_LIMIT);
    MdtSingleLimitSetStop(MdtLimitGetUpperLimit(limit), HI_LIMIT);
    MdtLimitSetLimitedForceMotor(limit, desiredVelocity, maxForce);

    MdtHingeEnable(hinge);

    /* Set up hinge to defaults. */
    SetDesiredVelocity(defaultVelocity);
    SetMaxForce(defaultMaxForce);
    SetLimitStiffness(defaultStiffness);
    SetLimitDamping(defaultDamping);
    SetLimitRestitution(defaultRestitution);


    /* Set up camera */
    RCameraRotateAngle(rc, (MeReal)0.8);
    RCameraRotateElevation(rc, (MeReal)0.25);
    MdtBodyGetPosition(body, pos);
    RCameraSetLookAt(rc, pos);
    RLightSwitchOff(rc, kRAmbient);

    RPerformanceBarCreate(rc);

    menu = RMenuCreate(rc, "Main Menu");
    RMenuAddToggleEntry(menu, "Motor On", ToggleMotor, 1);
    RMenuAddToggleEntry(menu, "Go Clockwise", SwitchDirection, 1);
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

    /* Cleanup after simulation */
#ifndef PS2
    atexit(cleanup);
#endif

    /* Run the Simulation */

    /*
       RRun() executes the main loop.

       Pseudocode: while no exit-request { Handle user input call tick() to
       evolve the simulation and update graphic transforms Draw graphics }
     */
    RRun(rc, tick, 0);

    return 0;
}
