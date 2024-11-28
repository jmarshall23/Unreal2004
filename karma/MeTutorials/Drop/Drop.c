/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:33 $ - Revision: $Revision: 1.31.8.1 $

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
Overview:

  Drop simulates a single falling ball. This is the simplest example
  using the Kea Solver of the Dynamics Toolkit, demonstrating a
  single body with no joints, constraints, or contacts and no applied
  forces other than gravity.
*/

#include <Mdt.h>
#include <MeViewer.h>

/* Render context */
RRender *rc;

/* Sphere Geometry, to be used by Viewer.  */
RGraphic *sphereG;

char *help[] = { "$ACTION2 resets the simulation" };

/* Colors */
float orange[4] = { 1.0f, 0.4f,   0.0f,   1.0f };
float blue[4]   = { 0.0f, 0.598f, 0.797f, 1.0f };

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;
/* Dynamics body corresponding to sphereG */
MdtBodyID body;

/* Simulation time step in seconds */
MeReal step = (MeReal)(0.03);


/*
tick() is a callback function called from the renderer's main loop to
evolve the world by 'step' seconds.
*/
void MEAPI Tick(RRender * rc, void *userData)
{
/*
  Evolve the world by step seconds. As a result, the body moves in the
  Dynamics world.
  */
    MdtWorldStep(world, step);

}


/*
Resets some dynamics body attributes
*/
void MEAPI reset(RRender * rc, void *userData)
{
    MdtBodySetPosition(body, 0, 10, 0);
    MdtBodySetLinearVelocity(body, 0, 0, 0);
    MdtBodySetAngularVelocity(body, 0, 0, 0);
    MdtBodySetQuaternion(body, 1, 0, 0, 0);
}

void MEAPI_CDECL cleanup(void)
{
/*
  Free all Dynamics.
  */

    MdtWorldDestroy(world);

    /*
  Free all Viewer graphics.
  */
    RRenderContextDestroy(rc);
}

/*
Main Routine
*/

int MEAPI_CDECL main(int argc, const char **argv)
{
    /*
      Initialise all rendering attributes and user-input.
    */

    MeCommandLineOptions* options;
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;

    RCameraSetView(rc, 40, 0.3f, 0);

    RRenderSetActionNCallBack(rc, 2, reset, 0);

    RRenderSetWindowTitle(rc,"Drop tutorial");
    RPerformanceBarCreate(rc);

    RRenderCreateUserHelp(rc, help, 1);
    RRenderToggleUserHelp(rc);

    /*
      Create a sphere geometry to be rendered by the Viewer.
    */
    sphereG = RGraphicSphereCreate(rc, 2, orange, 0);

    /*
      Initialise the MdtWorld.
    */

    world = MdtWorldCreate(1, 0);

    MdtWorldSetGravity(world, 0, -(MeReal)(9.8), 0);

    /*
      Initialise the MdtBody - use default mass (1) and moment of inertia
      (identity matrix).
    */
    body = MdtBodyCreate(world);

    /*
      Add the body to the simulation.
    */
    MdtBodyEnable(body);

    /*
      Updates the sphere's transformation matrix (used by Viewer) to match
      the Dynamics body's matrix
    */
    RGraphicSetTransformPtr(sphereG,MdtBodyGetTransformPtr(body));

    /*
      Initialise Dynamics properties.
    */
    reset(rc,0);

    /*
      Cleanup after simulation.
    */
    atexit(cleanup);

    /*
      Run the simulation.
      RRun() executes the main loop.

      Pseudocode: while no exit-request { Handle user input call Tick() to
      evolve the simulation and update graphic transforms Draw graphics }
    */
    RRun(rc, Tick,0 );

    return 0;
}
