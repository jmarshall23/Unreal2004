/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:38 $ - Revision: $Revision: 1.13.8.3 $

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
#include <McdCompositeModel.h>

#define N_PENDULUMS 3

/* Simulation container */
McdSpaceID  space;
MstBridgeID bridge;
MdtWorldID  world;
/* Array of ball and socket joint IDs */
MdtBSJointID bs;
/* Array of dynamics bodies IDs */
MdtBodyID ball[3];
/* Array of spring IDs */
MdtSpringID  Spring[2];

int contactsUsed = 0;

/* Render context */
RRender *rc;
/* body geometry */
RGraphic *sphereG[3];
RGraphic *lineG[2];
RGraphic *planeG;

char *help[] = { "$ACTION3 add force to last the ball" };

MeReal height = 1.5;

/*
  The three fields correspond to red/green/blue
*/
float blue[]   = { 0,      0,  1.1f, 1 };
float orange[] = { 1.0f, 0.4f, 0,    1 };
float black[]  = { 0.0f, 0.0f, 0,    1 };

/*
  Initial line geometry. The lines will be given the same
  transformation matrix as the balls so that they can rotate with them
*/
MeReal lineOrigin[3];
MeReal lineEnd[3] ;

/*
  Timestep in seconds
*/
MeReal step = (MeReal)(0.03);


void MEAPI liftFirstBall(RRender *rc, void *userData)
{
  MeVector3 absForce;
  MeVector3 relForce = { -50, 0, 0 };

  MdtBodyEnable(ball[2]);

  MdtConvertPositionVector(ball[2], relForce, 0, absForce);
  MdtBodyAddForce(ball[2], absForce[0], absForce[1], absForce[2]);

}

/*
  First tick() performs collision detection between all balls. Then
  the world is stepped by 'step' seconds. Finally the new transformations
  are passed to the renderer.
*/
void MEAPI tick(RRender * rc, void *userData)
{
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge,space,world);
    MdtBodyGetPosition(ball[0],lineOrigin);
    MdtBodyGetPosition(ball[1],lineEnd);
    RGraphicLineMoveEnds(lineG[0],lineOrigin,lineEnd);
    MdtBodyGetPosition(ball[1],lineOrigin);
    MdtBodyGetPosition(ball[2],lineEnd);
    RGraphicLineMoveEnds(lineG[1],lineOrigin,lineEnd);
    MdtWorldStep(world, step);
}

void MEAPI_CDECL cleanup(void)
{
  RRenderContextDestroy(rc);
  MdtWorldDestroy(world);
}

/*
  Main routine.
*/

int MEAPI_CDECL main(int argc, const char *(argv[]))
{
    MeVector3 cameraLookAt;
    float color[4];
    MeMatrix4 tm;
    MeVector3 vtmp;
    MeCommandLineOptions* options;

    world = MdtWorldCreate(N_PENDULUMS, 10, 1, 1);
    space = McdSpaceAxisSortCreate(McdAllAxes, 20, 30,1);
    bridge = MstBridgeCreate(10);
    MstSetWorldHandlers(world);

    MdtWorldSetGravity(world, 0, -(MeReal)(10.0), 0);

    /* create balls */
    ball[0] = MdtBodyCreate(world);
    MdtBodySetPosition(ball[0],0,3*height+2, 0);


    ball[1] = MdtBodyCreate(world);
    MdtBodySetPosition(ball[1],0,2*height+2, 0);

    ball[2] = MdtBodyCreate(world);
    MdtBodySetPosition(ball[2], 0, 3*height+2, 0);

    /*
    Adding velocity damping makes the simulation more physically
    realistic. Velocity damping acts very much like air resistance.
    */
    MdtBodySetLinearVelocityDamping(ball[1], (MeReal)(0.1));
    MdtBodySetLinearVelocityDamping(ball[2], (MeReal)(0.2));


    /*
    Start the first pendulum raised.
    */
    MdtBodySetPosition(ball[1],  -height,  3*height+2, 0);
    MdtBodySetPosition(ball[2], -2*height, 3*height+2, 0);

    /*
    The second body parameter of zero means that the ball and
    socket joint is anchored to the static environment.
    */
    bs = MdtBSJointCreate(world);
    MdtBSJointSetBodies(bs,ball[0],0);

    /*
    Set the ball and socket's position. This is specified in
    world coordinates.
    */
    MdtBSJointSetPosition(bs,0,3*height+2, 0);

    /* create springs */

    Spring[0] = MdtSpringCreate(world);
    MdtSpringSetNaturalLength(Spring[0],(MeReal)(height*.75));
    MdtSpringSetStiffness(Spring[0],80.0);
    MdtSpringSetDamping(Spring[0], (MeReal)0.3);

    MdtBodyGetPosition(ball[0],vtmp);
    MdtSpringSetBodies(Spring[0],ball[0],ball[1]);
    MdtSpringSetPosition(Spring[0],0,vtmp[0],vtmp[1],vtmp[2]);
    MdtBodyGetPosition(ball[1],vtmp);
    MdtSpringSetPosition(Spring[0],1,vtmp[0],vtmp[1],vtmp[2]);
    MdtSpringEnable(Spring[0]);

    Spring[1] = MdtSpringCreate(world);
    MdtSpringSetNaturalLength(Spring[1],(MeReal)(height*.75));
    MdtSpringSetStiffness(Spring[1],40.0);
    MdtSpringSetDamping(Spring[1], (MeReal)0.3);
    MdtBodyGetPosition(ball[1],vtmp);
    MdtSpringSetBodies(Spring[1],ball[1],ball[2]);
    MdtSpringSetPosition(Spring[1],0,vtmp[0],vtmp[1],vtmp[2]);
    MdtBodyGetPosition(ball[2],vtmp);
    MdtSpringSetPosition(Spring[1],1,vtmp[0],vtmp[1],vtmp[2]);
    MdtSpringEnable(Spring[1]);

    MdtBodyEnable(ball[0]);
    MdtBodyEnable(ball[1]);
    MdtBodyEnable(ball[2]);
    MdtBSJointEnable(bs);

    /*
    Initialise rendering attributes.
    */

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
       return 1;

    /*
     Shapes that can be created are spheres, lines, cubes, cones,
     cylinders and user-defined meshes. See "MeViewer.h" for details.
    */

    sphereG[0] = RGraphicSphereCreate(rc, (MeReal)0.4, orange,
                                      MdtBodyGetTransformPtr(ball[0]));
    sphereG[1] = RGraphicSphereCreate(rc, (MeReal)0.5, blue,
                                      MdtBodyGetTransformPtr(ball[1]));
    sphereG[2] = RGraphicSphereCreate(rc, (MeReal)0.5, blue,
                                      MdtBodyGetTransformPtr(ball[2]));

    MdtBodyGetPosition(ball[0],lineOrigin);
    MdtBodyGetPosition(ball[1],lineEnd);
    MeMatrix4TMMakeIdentity(tm);
    lineG[0] = RGraphicLineCreate(rc, lineOrigin, lineEnd, blue, tm);
    MdtBodyGetPosition(ball[1],lineOrigin);
    MdtBodyGetPosition(ball[2],lineEnd);
    lineG[1] = RGraphicLineCreate(rc, lineOrigin, lineEnd, blue, tm);
    /*
    Ground
    */
    color[0] = color[1] = color[2] = color[3] = 1;
    planeG = RGraphicGroundPlaneCreate(rc, 30.0f,30, color, 0);
    RGraphicSetTexture(rc, planeG, "checkerboard");

    /*
    Camera attributes can be displayed from the renderer by pressing F7.
    */
    /* RCameraSetView(rc,(float)15,(float)-0.1,(float)0.5); */

    cameraLookAt[0] = 2.0f;
    cameraLookAt[1] = height;
    cameraLookAt[2] = 2.0f;
    RCameraSetLookAt(rc, cameraLookAt);

    RRenderSetActionNKey(rc,3,' ');

    RRenderSetActionNCallBack(rc, 3, liftFirstBall, 0);

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
