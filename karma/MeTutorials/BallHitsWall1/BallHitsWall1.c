/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:31 $ - Revision: $Revision: 1.52.4.3 $

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

  BallHitsWall1 uses the Collision Toolkit alone, without the Dynamics
  Toolkit. The scenario: a ball is thrown at a wall, bounces off the
  wall, bounces off the floor, and then exits the scene. The motion of
  the ball is not being driven by the dynamics engine, rather, it is
  scripted by hand.

  Note: There is no gravity in this simulation.
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <MeAssert.h>
#include <Mcd.h>
#include <McdPrimitives.h>
#include <MeMath.h>
#include <MeViewer.h>
#include <MeMemory.h>

/* define to get cube collision */
#if 0
#define USE_CUBE
#endif

/*
  Globals
*/
#define MAX_BODIES 10

#define MAX_PAIRS 100

/*
  render context
*/
RRender *rc;

/*
  Framework, worlds and Spaces
*/
McdFrameworkID framework;
McdSpaceID space;
McdModelPairContainer *pairs;

/*
  Collision reps
*/
McdModelID groundCM;

/*
  collision model geometry representations
*/

McdGeometryID plane_prim, box_prim, box_prim1, ball_prim;

/*
  Graphics reps
*/
RGraphic *ballG;
RGraphic *boxG;
RGraphic *planeG;


/*
  time step
*/

MeReal step = 0.02f;
/*
  ball radius
*/
MeReal radius = 0.5;
/*
  wall dimensions
*/
MeReal wallDims[3] = { 0.5f, 2.0f, 5.0f };
/*
  floor dimensions
*/
MeReal floorDims[3] = { 6.0f, 0.05f, 5.2f };

/*
  Colors
*/
float white[4]  = { 1.0f, 1.0f, 1.0f,     1.0f };
float orange[4] = { 1.0f, 0.4f, 0.0f,     1.0f };
float green[4]  = { 0.0f, 1.0f, 0.0f,     1.0f };
float blue[4]   = { 0.0f, 0.598f, 0.797f, 1.0f };

/*
  rotate ground plane collision model 90 about x-axis
*/
MeMatrix4 groundTransform =
  {
      {  1,  0,  0,  0},
      {  0,  0, -1,  0},
      {  0,  1,  0,  0},
      {  0,  0,  0,  1}
  };

/*
  help text
*/

char *help[2] =
{
    "$ACTION2: reset",
    "$ACTION3: shoot"
};

/*
  Structure that holds information about an object
*/
typedef struct Object
{
    McdModelID m_object;
    MeMatrix4 m_TM;
    MeReal *m_position;
    MeReal m_velocity[3];
}
Object;

/*
  Object instances
*/
Object box;
Object ball;


/*
  Set vector 'a' to be the sum of vector 'b' and the product of scalar
  'x' and vector 'c'. This is mainly used to drive the motion of the
  ball.
*/
void MultiplyAddVec(MeReal *a, const MeReal *b, const MeReal x,
    const MeReal *c)
{
    int i;

    for (i = 0; i < 3; i++)
        a[i] = b[i] + x * c[i];
}

/*
  Shoot the ball
*/
void MEAPI shoot(RRender * rc, void *userData)
{
    ball.m_TM[3][0] = 0.0;
    ball.m_TM[3][1] = 4.0;
    ball.m_TM[3][2] = 0.0;
    ball.m_velocity[0] = 2.0;
    ball.m_velocity[1] = -2.0;
}

/*
  Reset the scene
*/
void MEAPI reset(RRender * rc, void *userData)
{
    MeMatrix4TMMakeIdentity(ball.m_TM);
    ball.m_TM[3][0] = 0.0;
    ball.m_TM[3][1] = 4.0;
    ball.m_TM[3][2] = 0.0;
    ball.m_velocity[0] = 0.0;
    ball.m_velocity[1] = 0.0;
    ball.m_velocity[2] = 0.0;

    MeMatrix4TMMakeIdentity(box.m_TM);
    box.m_TM[3][0] = 3;
    box.m_TM[3][1] = wallDims[1];
    box.m_TM[3][2] = 0;
}

/*
  Update the positions of the objects
*/
void move()
{
    MultiplyAddVec(box.m_position, box.m_position, step,
        box.m_velocity);
    MultiplyAddVec(ball.m_position, ball.m_position, step,
        ball.m_velocity);
}

/*
  Handle collision for a given list of McdModelPairLinks. If a
  collision is is detected, change velocity of the ball, given the
  contact information
*/

void handleCollision(McdModelPairContainer *pairs)
{
    static McdContact contacts[10];
    MeReal k;
    int i;
    McdIntersectResult result;

    result.contactMaxCount = 10;
    result.contacts = contacts;


    for( i = pairs->helloFirst ; i < pairs->stayingEnd ; ++i )
    {
        McdModelPairID pair = pairs->array[i];

        /*
          get contact info
        */
        McdIntersect(pair, &result);
        if (result.contactCount)
          printf("\nNumber of contacts: %d", result.contactCount);

        /*
          calculate change of velocity (along normal) scalar
        */
        k = -2 * MeVector3Dot(ball.m_velocity, result.normal);
        /*
          change velocity
        */
        MultiplyAddVec(ball.m_velocity, ball.m_velocity, k,
            result.normal);
    }
}



/*
  This function is called every frame by the renderer.
*/
void MEAPI tick(RRender * rc, void *userData)
{
    MeBool pairOverflow;
    McdSpacePairIterator spaceIter;

    move();

    /*
      evolve space
    */

    McdSpaceUpdateAll(space);
    McdSpaceEndChanges(space);

    /* Initialise iterator for this space, after updating space. */
    McdSpacePairIteratorBegin(space, &spaceIter);

    do
    {
        McdModelPairContainerReset(pairs);
        pairOverflow = McdSpaceGetPairs(space, &spaceIter, pairs);
        MEASSERT(!pairOverflow);
        McdGoodbyeEach(pairs);
        McdHelloEach(pairs);
        handleCollision(pairs);
    } while(pairOverflow);

    McdSpaceBeginChanges(space);
}

void MEAPI_CDECL cleanup(void)
{
    /*
      memory release. Collision objects must be explicitly deleted
      by the user.
    */
    RRenderContextDestroy(rc);
    
    McdModelDestroy(groundCM);
    McdGeometryDestroy(plane_prim);

#ifdef USE_CUBE
    McdModelDestroy(box.m_object);
    McdGeometryDestroy(box_prim);
#else
    McdModelDestroy(ball.m_object);
    McdGeometryDestroy(ball_prim);
#endif
    McdSpaceDestroy(space);
    McdTerm(framework);
    McdModelPairContainerDestroy(pairs);
    McdBoxDestroy(box_prim1);
}

/*
  Main Routine
*/
int MEAPI_CDECL main(int argc, const char * argv[])
{
    float color[4] = {0,0,0,0};
    MeCommandLineOptions* options;

    MeReal scale = 1.0;
    
    /* Capture the command line */
    options = MeCommandLineOptionsCreate(argc, argv);
    /*
      Initialize renderer
    */
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

    /*
      Initialize toolkit
    */

    framework = McdInit(0, 100, 0, scale);
    McdPrimitivesRegisterTypes(framework);
    McdPrimitivesRegisterInteractions(framework);
    space =
        McdSpaceAxisSortCreate(framework,McdAllAxes, MAX_BODIES, 2 * MAX_BODIES,1);
    pairs = McdModelPairContainerCreate(MAX_PAIRS);

    color[0] = color[1] = color[2] = color[3] = 1;
    planeG = RGraphicGroundPlaneCreate(rc, 30.0f,30, color, 0);
    RGraphicSetTexture(rc, planeG, "checkerboard");

    RCameraSetView(rc,(float)15,(float)-0.1,(float)0.5);

    plane_prim = McdPlaneCreate(framework);
    groundCM = McdModelCreate(plane_prim);
    McdSpaceInsertModel(space, groundCM);
    McdModelSetTransformPtr(groundCM, groundTransform);

    /*
      optimize
    */
    /* Inform the system that groundCM 's transform will not change value*/
    McdSpaceUpdateModel(groundCM);
    McdSpaceFreezeModel(groundCM);

    /*
      box
    */
    box_prim1 = McdBoxCreate(framework,2*wallDims[0], 2*wallDims[1], 2*wallDims[2]);
    MeMatrix4TMMakeIdentity(box.m_TM);
    box.m_TM[3][0] = 3;
    /*
      raise above floor
    */
    box.m_TM[3][1] = wallDims[1];
    box.m_TM[3][2] = 0;
    box.m_position = &box.m_TM[3][0];
    box.m_object = McdModelCreate(box_prim1);
    McdModelSetTransformPtr(box.m_object, box.m_TM);
    McdSpaceInsertModel(space, box.m_object);
    MeMatrix4TMMakeIdentity(ball.m_TM);
    ball.m_TM[3][0] = 0;
    ball.m_TM[3][1] = 4;
    ball.m_TM[3][2] = 0;
    ball.m_position = &ball.m_TM[3][0];

    boxG = RGraphicBoxCreate(rc, 2 * wallDims[0], 2 * wallDims[1],
        2 * wallDims[2], blue,
        (MeMatrix4Ptr) McdModelGetTransformPtr(box.m_object));
#ifdef USE_CUBE
    box_prim = McdBoxCreate(framework,2*radius, 2*radius, 2*radius);
#else
    ball_prim = McdSphereCreate(framework,radius);
#endif


#ifdef USE_CUBE
    ball.m_object = McdModelCreate(box_prim);
#else
    ball.m_object = McdModelCreate(ball_prim);
#endif

    McdModelSetTransformPtr(ball.m_object, ball.m_TM);
    McdSpaceInsertModel(space, ball.m_object);

#ifdef USE_CUBE
    ballG = RGraphicBoxCreate(rc, 2*radius, 2*radius, 2*radius, orange,
        McdModelGetTransformPtr(ball.m_object));
#else
    ballG = RGraphicSphereCreate(rc, radius, white,
        McdModelGetTransformPtr(ball.m_object));
    RGraphicSetTexture(rc, ballG, "ME_ball3");
#endif

    /*
      Build Space
    */
    McdSpaceBuild(space);

    /*
      Keyboard callbacks
    */

    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, shoot, 0);

    /*
      On screen help text
    */
    RRenderSetWindowTitle(rc,"BallHitsWall1 tutorial");
    RPerformanceBarCreate(rc);

    RRenderCreateUserHelp(rc, help, 2);
    RRenderToggleUserHelp(rc);

    /*
      cleanup after simulation
    */
    atexit(cleanup);

    /*
      run the simulation loop
    */
    RRun(rc, tick, 0);

    return 0;
}
