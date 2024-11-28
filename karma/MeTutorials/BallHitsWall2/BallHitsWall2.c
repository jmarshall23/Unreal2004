/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:31 $ - Revision: $Revision: 1.49.2.2 $

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

  BallHitsWall2 reproduces the behaviour of BallHitsWall1, only now
  the Dynamics Toolkit is being used to control the motion.
*/

#include <math.h>
#include <stdlib.h>

#include <Mdt.h>
#include <Mcd.h>
#include <Mst.h>
#ifndef MCDCHECK
#define MCDCHECK
#endif
#include <McdInteractionTable.h>
#include <MeViewer.h>

/* define to attach hinge joint to wall */
#if 0
#define USE_HINGE
#endif

#define MAX_BODIES          (10)
#define MAX_CONSTRAINTS     (10)

/* rendering context */
RRender *rc;

/*
  Dynamics world, collision space, and dynamics-collision bridge.
*/
McdFrameworkID  framework;
MdtWorldID      world;
McdSpaceID      space;
MstBridgeID     bridge;

/*
  Physics reps.
*/
MdtBodyID box,ball;

/*
  Collision reps.
*/

McdGeometryID plane_prim, box_prim1, ball_prim;
McdModelID groundCM, boxCM, ballCM;

/*
  Graphics reps.
*/
RGraphic *planeG;
RGraphic *boxG;
RGraphic *ballG;

#ifdef USE_HINGE
MdtHingeID hinge;
#endif

/*
  Misc.
*/

/* world gravity */
MeReal gravity[3] = { 0.0f, -5.0f, 0.0f };
/* time step */
MeReal step = 0.02f;

MeReal boxMass = 50.0f;
MeReal ballMass = 1.0f;
MeReal radius = 0.5f;

/* wall dimensions */
MeReal wallDims[3] = { 1.0f, 4.0f, 10.0f };
/* wall position */
MeReal wallPos[3] = { 3.0f, 2.001f, 0.0f };
/* floor dimensions */
MeReal floorDims[3] = { 6.0f, 0.05f, 5.2f };

/*
  Colors.
*/

float white[4]  = { 1.0f, 1.0f,   1.0f,   0.0f };
float orange[4] = { 1.0f, 0.4f,   0.0f,   0.0f };
float green[4]  = { 0.0f, 1.0f,   0.0f,   0.0f };
float blue[4]   = { 0.0f, 0.598f, 0.797f, 0.0f };

/*
  Rotate ground plane collision model 90 about x-axis.
*/
MeMatrix4 groundTransform =
    {
      {  1,  0,  0,  0},
      {  0,  0, -1,  0},
      {  0,  1,  0,  0},
      {  0,  0,  0,  1}
    };

MeMatrix3 boxOrientation =
{
    {  1,  0,  0},
    {  0,  1,  0},
    {  0,  0,  1}
};

/*
  User help.
*/

char *help[2] =
{
    "$ACTION2: reset",
    "$ACTION3: shoot"
};


/*
  Shoot the ball.
*/
void MEAPI shoot(RRender * rc, void *userData)
{
    MdtBodyEnable(ball);
    MdtBodyEnable(box);
    MdtBodySetPosition(ball, 0, 4, 0);
    MdtBodySetLinearVelocity(ball, 4.0f, -4.0f, 0.0);
    MdtBodySetAngularVelocity(ball, 0, 0, 0);
}

/*
  Reset the scene.
*/
void MEAPI reset(RRender * rc, void *userData)
{
    MdtBodyEnable(ball);
    MdtBodySetPosition(ball, 0, 4, 0);
    MdtBodySetLinearVelocity(ball, 0, 0, 0);
    MdtBodySetAngularVelocity(ball, 0, 0, 0);

    MdtBodySetPosition(box, wallPos[0], wallPos[1], wallPos[2]);
    MdtBodySetOrientation(box, boxOrientation);
    MdtBodySetLinearVelocity(box, 0, 0, 0);
    MdtBodySetAngularVelocity(box, 0, 0, 0);
}


/*
  This function is called every frame by the renderer.
*/
void MEAPI tick(RRender * rc, void *userData)
{
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge,space,world);
    MdtWorldStep(world,step);

}

void MEAPI_CDECL cleanup(void)
{
    /*
      Memory release.
    */
    RRenderContextDestroy(rc);

    /* free dynamics */
    MdtWorldDestroy(world);

    /* free collision */
    McdModelDestroy(groundCM);
    McdGeometryDestroy(plane_prim);
    
    McdModelDestroy(boxCM);
    McdGeometryDestroy(box_prim1);

    McdModelDestroy(ballCM);
    McdGeometryDestroy(ball_prim);
    
    McdSpaceDestroy(space);
    MstBridgeDestroy(bridge);
    McdTerm(framework);
}

/*
  Main routine.
*/
int MEAPI_CDECL main(int argc, const char **argv)
{
    MeCommandLineOptions* options;

    /*
      Kea contact properties.
    */
    MdtContactParamsID params;
    /*
      Inertia tensor.
    */
    MeMatrix3 I;

    /*
      Initialize renderer
    */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

    /*
     * Create a dynamics world with a maximum of 10 bodies,
     * 10 constraints and 1 material.
     */
    world = MdtWorldCreate(MAX_BODIES, MAX_CONSTRAINTS);
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    /* disable the body if it comes to rest */
    MdtWorldSetAutoDisable(world, 1);

    /*
      Initialise Mcd system.
    */

    framework = McdInit(0, 100, 0);
    McdPrimitivesRegisterTypes(framework);
    McdPrimitivesRegisterInteractions(framework);

    /*
    McdInit(3);
    McdSphereBoxPlaneRegisterTypes();
    McdSphereBoxPlaneRegisterInteractions();
    */

    /* initialize the bridge between collision and dynamics */

    space = McdSpaceAxisSortCreate(framework,McdAllAxes, MAX_BODIES, 2 * MAX_BODIES,1);
    bridge = MstBridgeCreate(framework,10);
    MstSetWorldHandlers(world);

    /* ground graphic */

    planeG = RGraphicGroundPlaneCreate(rc, 30.0f,30, white, 0);
    RGraphicSetTexture(rc, planeG, "checkerboard");

    /* Top at y=0 */

    /* ground collision */
    plane_prim = McdPlaneCreate(framework);
    groundCM = McdModelCreate(plane_prim);
    McdSpaceInsertModel(space, groundCM);
    McdModelSetTransformPtr(groundCM, groundTransform);

    /*
      Inform the system that groundCM 's transform will
      not change value.
     */
    McdSpaceUpdateModel(groundCM);
    McdSpaceFreezeModel(groundCM);

    /*
      Wall
    */
    /* wall dynamics */
    box = MdtBodyCreate(world);
    MdtBodyEnable(box);
    MdtBodySetPosition(box, wallPos[0], wallPos[1], wallPos[2]);
    MdtBodySetMass(box, boxMass);
    MdtMakeInertiaTensorBox(boxMass,
        wallDims[0], wallDims[1], wallDims[2], I);
    MdtBodySetInertiaTensor(box, I);

    /* wall graphics */
    boxG = RGraphicBoxCreate(rc,
        wallDims[0], wallDims[1],
        wallDims[2], blue, MdtBodyGetTransformPtr(box));

    /* wall collision */
    box_prim1 = McdBoxCreate(framework,wallDims[0], wallDims[1], wallDims[2]);
    boxCM = McdModelCreate(box_prim1);
    McdSpaceInsertModel(space, boxCM);

    /* assign a dynamics body to the wall */
    McdModelSetBody(boxCM, box);

    /*
      Ball.
    */
    /* ball dynamics */
    ball = MdtBodyCreate(world);
    MdtBodyEnable(ball);
    MdtBodySetPosition(ball, 0.0, 4.0, 0.0);
    MdtBodySetMass(ball, ballMass);

    /* ball graphic */
    ballG = RGraphicSphereCreate(rc, radius, orange, MdtBodyGetTransformPtr(ball));

    /* ball collision */
    ball_prim = McdSphereCreate(framework,radius);
    ballCM = McdModelCreate(ball_prim);
    McdSpaceInsertModel(space, ballCM);

    /* assign a dynamics body to the ball */
    McdModelSetBody(ballCM, ball);

#ifdef USE_HINGE
     hinge = MdtHingeCreate(world);
     MdtHingeSetBodies(hinge,box,0);
     MdtHingeSetPosition(hinge,
         wallPos[0], wallPos[1], wallPos[2]+wallDims[2]/2);
     MdtHingeSetAxis(hinge, 0, 1, 0);
     MdtHingeEnable(hinge);
#endif

    /*
      Set parameters for contacts.
    */
    params = MstBridgeGetContactParams(bridge,
        MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsSetType(params, MdtContactTypeFriction2D);
    MdtContactParamsSetRestitution(params, 0.5);
    MdtContactParamsSetSoftness(params, (MeReal)0.0005);

    /*
      Build space.
    */
    McdSpaceBuild(space);

    /*
      Camera position.
    */

    RCameraSetView(rc,(float)15,(float)-0.1,(float)0.5);


    /*
      Keyboard callbacks.
    */

    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, shoot, 0);

    /*
      On screen help text.
    */
    RRenderSetWindowTitle(rc,"BallHitsWall2 tutorial");
    RPerformanceBarCreate(rc);

    RRenderCreateUserHelp(rc, help, 2);
    RRenderToggleUserHelp(rc);

    /*
      Cleanup after simulation.
    */
#ifndef PS2
    atexit(cleanup);
#endif
    /*
      Run the simulation loop.
    */
    RRun(rc, tick, 0);

    return 0;
}
