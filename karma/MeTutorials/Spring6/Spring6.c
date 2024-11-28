/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:38 $ - Revision: $Revision: 1.2.2.2 $

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

#include <math.h>

#include <Mdt.h>
#include <MeMath.h>
#include <MeViewer.h>
#include <MeApp.h>
#include "Mst.h"

/* Simulation container */
MstUniverseID   universe;
McdFrameworkID  frame;      
McdSpaceID      space;
MdtWorldID      world;
MeApp*          meapp;

/* Array of dynamics bodies IDs */
McdGeometryID   boxGeom;
McdModelID      boxM;
MdtBodyID       box;


/* Array of spring IDs */
MdtSpring6ID  spring;

int contactsUsed = 0;

/* Render context */
RRender *rc;

/* body geometry */
RGraphic *boxG;

RGraphic *sphereG;
RGraphic *planeG;

/*
  The three fields correspond to red/green/blue
*/
float blue[]   = { 0,      0,  1.1f, 1 };
float orange[] = { 1.0f, 0.4f, 0,    1 };
float black[]  = { 0.0f, 0.0f, 0,    1 };

char *help[1] =
{
    "$ACTION2 - next stiffness preset"
};

/*
  Timestep in seconds
*/
MeReal step = (MeReal)(0.03);

#define NUM_PRESETS (3)
int currentPreset = 0;

MeReal preset[NUM_PRESETS][6] = 
{
    {MEINFINITY, 50, MEINFINITY, MEINFINITY, MEINFINITY, MEINFINITY},
    {MEINFINITY, MEINFINITY, MEINFINITY, MEINFINITY, 2500, MEINFINITY},
    {50, 50, 50, 2500, 2500, 2500}
};

/*
  First tick() performs collision detection between all balls. Then
  the world is stepped by 'step' seconds. Finally the new transformations
  are passed to the renderer.
*/
void MEAPI tick(RRender * rc, void *userData)
{
    MeAppStep(meapp);
    MdtWorldStep(world, step);
}

void MEAPI_CDECL cleanup(void)
{
    MeAppDestroy(meapp);
    RRenderContextDestroy(rc);
    MdtWorldDestroy(world);
}

void MEAPI nextPreset(RRender* rc, void* userData)
{
    currentPreset = (currentPreset + 1)%NUM_PRESETS;

    MdtSpring6SetLinearStiffness(spring, 0, preset[currentPreset][0]);
    MdtSpring6SetLinearStiffness(spring, 1, preset[currentPreset][1]);
    MdtSpring6SetLinearStiffness(spring, 2, preset[currentPreset][2]);

    MdtSpring6SetAngularStiffness(spring, 0, preset[currentPreset][3]);
    MdtSpring6SetAngularStiffness(spring, 1, preset[currentPreset][4]);
    MdtSpring6SetAngularStiffness(spring, 2, preset[currentPreset][5]);
}

/* Main routine. */
int MEAPI_CDECL main(int argc, const char *(argv[]))
{
    MeVector3 cameraLookAt;
    float color[4];
    MeCommandLineOptions* options;

    /* Initialise rendering attributes. */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
       return 1;

    universe = MstUniverseCreate(&MstUniverseDefaultSizes);


    world = MstUniverseGetWorld(universe);
    MdtWorldSetGravity(world, 0, -(MeReal)(10.0), 0);

    frame = MstUniverseGetFramework(universe);
    space = MstUniverseGetSpace(universe);

    meapp = MeAppCreate(world, space, rc);

    /* Create box */
    box = MdtBodyCreate(world);
    MdtBodySetPosition(box, 0, 5, 0);

    boxGeom = McdBoxCreate(frame, 1, 1, 1);
    boxM = McdModelCreate(boxGeom);
    McdModelSetBody(boxM, box);
    McdSpaceInsertModel(space, boxM);

    /* create spring */
    spring = MdtSpring6Create(world);
    MdtSpring6SetBodies(spring, box, 0);
    MdtSpring6SetPosition(spring, 0, 0, 0);
    MdtSpring6SetAxes(spring, 
        1, 0, 0,
        0, 1, 0);

    /* reset stiffness/damping */
    currentPreset = NUM_PRESETS-1;
    nextPreset(rc, 0);

    MdtSpring6Enable(spring);

    /* graphics */

    boxG = RGraphicBoxCreate(rc, 1, 1, 1, orange, MdtBodyGetTransformPtr(box));

    sphereG = RGraphicSphereCreate(rc, (MeReal)0.3, blue, 0);

    /* Ground */
    color[0] = color[1] = color[2] = color[3] = 1;
    planeG = RGraphicGroundPlaneCreate(rc, 30.f,30, color, 0);
    RGraphicSetTexture(rc, planeG, "checkerboard");

    cameraLookAt[0] = 0;
    cameraLookAt[1] = (MeReal)2.5;
    cameraLookAt[2] = 0;
    RCameraSetLookAt(rc, cameraLookAt);
    RCameraRotateAngle(rc, ME_PI/8);

    RRenderSetActionNCallBack(rc, 2, nextPreset, 0);
    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);

    RPerformanceBarCreate(rc);
    RRenderSetWindowTitle(rc,"Spring example");
    RRenderCreateUserHelp(rc, help, 1);
    RRenderToggleUserHelp(rc);

  /*
    Cleanup after simulation.
  */
#ifndef PS2
  atexit(cleanup);
#endif

    RRun(rc, tick, 0);

    return 0;
}
