/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:08 $ - Revision: $Revision: 1.39.2.2 $

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

Balls are thrown at a plank bridge; the bridge responds to the
shock in a realistic way.

Dynamics Discussion:

The planks are attached to each other with single ball and
socket joints running down the centre of the bridge.


Collision Discussion:

This example illustrates how to selectively turn off collision
detection for pairs of models that are close or touching, but
whose geometrical interaction is being modeled by other means
(in this case, by imposing a geometrical constraint into the
dynamical model).

Note: try to use different values of NBoards and/or boxRadii[3]
*/

#include <stdlib.h>
#include <stdio.h>


#include <Mcd.h>
#include <Mst.h>
#include <MeViewer.h>


#define NBoards       9
#define NBalls        2
#define NContacts   200

/* * 1/sqrt(2) */
const MeReal SQRT2INV = 0.70710678118655f;

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;

/* Framework for the Collision Toolkit */
McdFrameworkID framework;
/* Space for the Collision Toolkit */
McdSpaceID space;
/* Connection between dynamics and collision */
MstBridgeID bridge;

/*
  Physics representations
*/

/* Array of dynamics bodies (boards) IDs */
MdtBodyID board[NBoards];
/* Array of dynamics bodies (balls) IDs  */
MdtBodyID ball[NBalls];
/* Pinter to ball and socket joint ID  */
MdtBSJointID joint[NBoards+1];

/*
  Graphics representations
*/

/* board graphic */
RGraphic *boardG[NBoards];
/* ball graphic */
RGraphic *ballG[NBalls];
/* Render context */
RRender *rc = 0;

/*
  Collision representations
*/

/* Array of Collision Model (boards) */
McdModelID boardCM[NBoards];
/* Array of Collision Model (balls) */
McdModelID ballCM[NBalls];

/* Collision geometry ID (box) */
McdGeometryID box_prim;
/* Collision geometry ID (ball) */
McdGeometryID ball_prim[NBalls];

/*
  Global variables
*/
int autoFireDelay = 500;
int autoEvolve = 1;
int autoShoot = 1;
int nSteps = 1;

/* simulation time step */
MeReal step = 0.03f;

/* gravity */
MeReal gravity[3] = { 0, -3, 0 };

/* board dimension */
MeReal boxRadii[3] = { 0.5f, 0.1f, 0.5f };
MeReal ballRadius = 0.5f;

/* used for three shooting speed */
int iSpeed = 0;

/*
  press F1 to get the help text
*/
char *help[] =
{
    "$RIGHT: Shoot",
    "$UP2: Toggle AutoEvolve",
    "$DOWN2: Toggle AutoShoot",
    "$UP: Increase timestep",
    "$DOWN: Decrease Timestep",
};

void MEAPI toggleAutoEvolve(RRender *rc, void *user_data)
{
    autoEvolve = !autoEvolve;
}

void MEAPI toggleAutoShoot(RRender *rc, void *user_data)
{
    autoShoot = !autoShoot;
}

/*
  Decrease time step by 0.001 second.
*/
void MEAPI decreaseTimeStep(RRender *rc, void *user_data)
{
    int i;

    if (step > 0.001)
        step -= 0.001f;

    i = (int) (400 * (0.01 / step));

    if (autoFireDelay > i)
        autoFireDelay = i;
}

/*
  Increase time step by 0.001 second.
*/
void MEAPI increaseTimeStep(RRender *rc, void *user_data)
{
    int i;

    if (step < 0.1)
        step += 0.001f;

    i = (int) (400 * (0.01 / step));

    if (autoFireDelay > i)
        autoFireDelay = i;
}

/*
  Generate a random number between start and end.
*/
MeReal Rnd(MeReal start, MeReal end)
{
    return start + ((end - start) * rand()) / (MeReal) RAND_MAX;
}

void MEAPI userShoot(RRender *rc, void *user_data)
{
    int i;
    MeReal v[3];
    MeReal norm;
    AcmeVector3 cam_pos;
    AcmeVector3 cam_lookat;

    autoFireDelay = (int) (400 * (0.01 / step));
    RCameraGetPosition(rc,cam_pos);
    RCameraGetLookAt(rc,cam_lookat);

    /*
      Place the ball at the 'eye' of the camera.
    */
    MdtBodySetPosition(ball[0],
        cam_pos[0] - 2*ballRadius, cam_pos[1], cam_pos[2]);
    MdtBodySetPosition(ball[1],
        cam_pos[0] + 2*ballRadius, cam_pos[1], cam_pos[2]);

    for (i = 0; i < 3; i++)
        v[i] = cam_lookat[i] - cam_pos[i];

    norm = MeRecipSqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    v[0] *= norm;
    v[1] *= norm;
    v[2] *= norm;

    /*
      scale up velocity to make the shot more powerful
    */
    for (i = 0; i < 3; i++)
        v[i] *= 10;

    /* fire the balls */
    MdtBodySetLinearVelocity(ball[0], v[0], v[1], v[2]);
    MdtBodySetLinearVelocity(ball[1], v[0], v[1], v[2]);

    /* but don't give it any spin */
    MdtBodySetAngularVelocity(ball[0], 0, 0, 0);
    MdtBodySetAngularVelocity(ball[1], 0, 0, 0);
}

/*
  Shoot the two balls onto the bridge.
*/
void Shoot(void)
{
    MeReal zOffset;
    MeReal pos[3];

    autoFireDelay = (int) (400 * (0.01 / step));

    zOffset = Rnd(-0.01f, 0.01f);

    MdtBodySetAngularVelocity(ball[0], 0, 0, -1.0f);
    MdtBodySetLinearVelocity(ball[0], 1.0f, 0.0f, 0.0f);

    pos[0] = -boxRadii[0] * NBoards / 2;
    pos[1] = boxRadii[1] * 2.0f > 2.0f ? boxRadii[1] * 2.0f : 2.0f;
    pos[2] = zOffset;

    MdtBodySetPosition(ball[0], pos[0], pos[1], pos[2]);

    zOffset = Rnd(-0.03f, 0.03f);

    MdtBodySetAngularVelocity(ball[1], 0, 0, 0.0f);
    MdtBodySetLinearVelocity(ball[1], -1.5f *(MeReal)iSpeed, 0.0f, 0.0f);

    if(iSpeed++ > 3) iSpeed = 0;

    pos[0] = boxRadii[0] * NBoards / 2;
    pos[1] = boxRadii[1] * 2.0f > 3.0f ? boxRadii[1] * 2.0f : 3.0f;
    pos[2] = zOffset;

    MdtBodySetPosition(ball[1], pos[0], pos[1], pos[2]);
}


void stepEvolve()
{
    int i;

    /*
      Evolution.
    */
    for (i = nSteps; i--;)
    {
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge,space,world);
    MdtWorldStep(world,step);
    }
}

/*
   Tick() is a callback function called from the renderer's main loop
   to evolve the world by 'step' seconds.
*/
void MEAPI tick(RRender * rc, void *user_data)
{
    if (autoEvolve)
    {
        stepEvolve();

        if (autoShoot && (autoFireDelay-- <= 0))
            Shoot();
    }
}

void MEAPI_CDECL cleanup(void)
{
    int i;

    /* graphics */
    RRenderContextDestroy(rc);

    /* dynamics */
    MdtWorldDestroy(world);

    /* collision */
    for (i = 0; i < NBoards; i++)
    {
        McdModelDestroy(boardCM[i]);
    }
    McdGeometryDestroy(box_prim);
    for (i = 0; i < NBalls; i++)
    {
        McdModelDestroy(ballCM[i]);
        McdGeometryDestroy(ball_prim[i]);
    }
    McdSpaceDestroy(space);
    MstBridgeDestroy(bridge);
    McdTerm(framework);
}

int MEAPI_CDECL main(int argc, const char **argv)
{
    int i, j;
    MeMatrix4Ptr tm;
    float colors[2][4] = { {1, 1, 1, 1}, {1, 1, 1, 1} };
    MeReal pos[3];
    MeReal ballHeight = boxRadii[1] * 2.0f > 3.0f
        ? boxRadii[1] * 2.0f : 3.0f;
    MeReal ballOffset = 0.01f;
    MdtBclContactParams *params;

    MeCommandLineOptions *options;

    /*
      Dynamics.
    */

    /*
      Set up a physical world.
    */
    world = MdtWorldCreate(30, NContacts);

    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    /*
      Create physical bodies.
    */

    for (i = 0; i < NBoards; i++)
    {
        board[i] = MdtBodyCreate(world);

        MdtBodyEnable(board[i]);

        MdtBodySetAngularVelocityDamping(board[i], 0.6f);
        MdtBodySetLinearVelocityDamping(board[i], 0.3f);

        if (i < NBoards / 2)
        {
            pos[0] = (i - NBoards / 2 + 0.5f) * 2.0f * boxRadii[0];
            pos[1] = 2 * boxRadii[0];
            pos[2] = 0.0f;
        }
        else if (i == NBoards / 2)
        {
            /*
              Set the board standing along y-axis.
            */
            pos[0] = -boxRadii[1];
            pos[1] = boxRadii[0] + boxRadii[1];
            pos[2] = 0.0f;

            MdtBodySetQuaternion(board[i], SQRT2INV, 0, 0, SQRT2INV);
        }
        else
        {
            pos[0] = (i - NBoards / 2 - 0.5f) * 2.0f * boxRadii[0];
            pos[1] = 0.0f;
            pos[2] = 0.0f;
        }

        MdtBodySetPosition(board[i], pos[0], pos[1], pos[2]);
    }

    for (i = 0; i < NBalls; i++)
    {
        ball[i] = MdtBodyCreate(world);

        MdtBodyEnable(ball[i]);
        MdtBodySetAngularVelocity(ball[i], 0.8f, 0.8f, 0.80f);

        pos[0] = -NBoards / 2 * boxRadii[0] + NBoards * boxRadii[0] * i;
        pos[1] = ballHeight;
        pos[2] = ballOffset;

        MdtBodySetPosition(ball[i], pos[0], pos[1], pos[2]);
    }

    /*
      Create joints between boards.
    */

    for (j = 0; j < NBoards + 1; j++)
    {
        if (j == 0)
        {
            /* fixed to the ground */
            joint[j] = MdtBSJointCreate(world);
            MdtBSJointSetBodies(joint[j],board[0],0);
        }
        else if (j == NBoards)
        {
            /* fixed to the ground */
            joint[j] = MdtBSJointCreate(world);
            MdtBSJointSetBodies(joint[j],board[j - 1],0);
        }
        else
        {
            joint[j] = MdtBSJointCreate(world);
            MdtBSJointSetBodies(joint[j],board[j - 1], board[j]);
        }

        if (j < NBoards / 2 + 1)
            MdtBSJointSetPosition(joint[j],
                (j - NBoards / 2) * 2 * boxRadii[0],
                2 * boxRadii[0] + boxRadii[1], 0);
        else
            MdtBSJointSetPosition(joint[j],
                (j - NBoards / 2 - 1) * 2 * boxRadii[0],
                boxRadii[1], 0);

        MdtBSJointEnable(joint[j]);
    }


    /*
      Collision Detection.
    */

    /*
      Collision initialization.
    */
    framework = McdInit(0, 100, 0);
    McdPrimitivesRegisterTypes(framework);
    McdPrimitivesRegisterInteractions(framework);

    /*
      Create a collision space.
    */
    space = McdSpaceAxisSortCreate(framework,McdAllAxes, 50, 100);
    bridge = MstBridgeCreate(framework,10);
    MstSetWorldHandlers(world);

    /*
      Create a collision model for each body and attach the model to
      its corresponding physical body.
    */
    box_prim = McdBoxCreate(framework,2*boxRadii[0],
        2*boxRadii[1], 2*boxRadii[2]);

    for (i = 0; i < NBoards; i++)
    {
        boardCM[i] = McdModelCreate(box_prim);

        McdSpaceInsertModel(space, boardCM[i]);
        McdModelSetBody(boardCM[i], board[i]);
    }

    for (i = 0; i < NBalls; i++)
    {
        ball_prim[i] = McdSphereCreate(framework,ballRadius);

        ballCM[i] = McdModelCreate(ball_prim[i]);

        McdSpaceInsertModel(space, ballCM[i]);
        McdModelSetBody(ballCM[i],ball[i]);
    }

    /*
      Disable collision interactions between neigboring boards.
    */
    for (i = 0; i < NBoards - 1; i++)
        McdSpaceDisablePair(boardCM[i], boardCM[i + 1]);

    /*
      Set parameters for contacts.
    */
    params = MstBridgeGetContactParams(bridge,
        MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsSetType(params, MdtContactTypeFriction2D);

    /*
      Rendering.
    */

    /*
      Create a render context.
    */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);

    /*
      Graphics for each object.
    */
    for (i = 0; i < NBoards; i++)
    {
        tm = MdtBodyGetTransformPtr(board[i]);

        boardG[i] = RGraphicBoxCreate(rc, boxRadii[0] * 2,
            boxRadii[1] * 2, boxRadii[2] * 2, colors[0], tm);

        RGraphicSetTexture(rc,boardG[i], "wood1");
    }

    for (i = 0; i < NBalls; i++)
    {
        tm = MdtBodyGetTransformPtr(ball[i]);
        ballG[i] = RGraphicSphereCreate(rc, ballRadius, colors[1], tm);

        RGraphicSetTexture(rc,ballG[i], "ME_logo2");
    }

    RRenderSetRightCallBack(rc, userShoot, 0);
    RRenderSetUp2CallBack(rc, toggleAutoEvolve, 0);
    RRenderSetDown2CallBack(rc, toggleAutoShoot, 0);
    RRenderSetUpCallBack(rc, increaseTimeStep, 0);
    RRenderSetDownCallBack(rc, decreaseTimeStep, 0);

    RRenderSetWindowTitle(rc, "Bridge example");
    RRenderCreateUserHelp(rc, help, 5);
    RRenderToggleUserHelp(rc);

    /*
      Set up camera.
    */
    RCameraSetView(rc,NBoards * 1.5f * boxRadii[0],0,0.5f);
    RCameraUpdate(rc);

#ifndef PS2
    atexit(cleanup);
#endif

    RRun(rc, tick, 0);

    return 0;
}
