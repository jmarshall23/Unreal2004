/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:08 $ - Revision: $Revision: 1.48.2.1 $

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

#include <stdlib.h>
#include <stdio.h>

#include <Mdt.h>
#include <MeMath.h>
#include <MeViewer.h>
#include <McdFrame.h>
#include <McdPrimitives.h>
#include <Mst.h>
#include <MeApp.h>

#define NTorus          10
#define NLinks          9
#define NJoints         18
#define NContacts       200

/*
  Physics reps.
*/
MdtBodyID torus[NTorus];
/* invisible bodies */
MdtBodyID link[NLinks];
MdtHingeID joint[NJoints];
MdtHingeID joint_hinge;

/*
  Graphics reps.
*/
RGraphic* torusG[NTorus];
RGraphic* ballG;
RRender*  rc = 0;

MdtWorldID  world;
MstBridgeID bridge;
McdSpaceID  space;
McdFrameworkID frame;
MeApp*      meapp;

MdtBodyID   ball;
McdModelID  ballCM;
McdModelID  torusCM[NTorus];

McdGeometryID cyl_prim;
McdGeometryID ball_prim = 0;

int autoEvolve = 1;
int nSteps = 1;
MeReal step = 0.04f;
MeReal gravity[3] = { 0, -3, 0 };
MeReal ypos_start = 12;
MeReal outerR, innerR;

char *help[4] =
{
    "a - toggle pause",
    "d - to toggle shooting (trigger)",
    "w - increase time step by 0.001 s (faster)",
    "s - decrease time step by 0.001 s (slower)"
};


void SetAxisAngle(MeReal *q, const MeReal nX, const MeReal nY,
    const MeReal nZ, MeReal angle)
{
    MeReal s_2 = -MeSin(0.5f * angle);

    q[1] = nX;
    q[2] = nY;
    q[3] = nZ;

    MeVector3Normalize(q + 1);

    q[0] = MeCos(0.5f * angle);
    q[1] *= s_2;
    q[2] *= s_2;
    q[3] *= s_2;
}

/*
  Generate a random number between start and end.
*/
MeReal Rnd(MeReal start, MeReal end)
{
    return start + ((end - start) * rand()) / (MeReal) RAND_MAX;
}

/*
  Decrease time step by 0.001 second.
*/
void MEAPI decreaseTimeStep(RRender* rc, void* userData)
{
    if (step > 0.0001)
        step -= 0.001f;
}

/*
  Increase time step by 0.001 second.
*/
void MEAPI increaseTimeStep(RRender* rc, void* userData)
{
    if (step < 0.05)
        step += 0.001f;
}

void MEAPI toggleAutoEvolve(RRender* rc, void* userData)
{
    autoEvolve = !autoEvolve;
}

int is = 0;
int ir = 0;
int shoot = 1;
int iang = 0;
MeReal inc_ang = (MeReal) ME_PI / 4.0f;
MeReal speed = 12.0;

void MEAPI toggleAutoShooting(RRender* rc, void* userData)
{
    shoot = !shoot;
}


void stepEvolve()
{
    MeReal pos[3];

    MeAppStep(meapp);
    MeProfileStartSection("Collision", 0);
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge, space, world);
    MeProfileEndSection("Collision");

    MeProfileStartSection("Dynamics", 0);
    MdtWorldStep(world, step);


    is++;
    if (is > 200 * (0.04f / step))
    {
        if (shoot)
        {
            pos[0] = speed * MeSin(iang * inc_ang);
            pos[1] = ypos_start - (outerR - innerR) * (NTorus - 1);
            pos[2] = speed * MeCos(iang * inc_ang);

            MdtBodySetPosition(ball, pos[0], pos[1], pos[2]);

            MdtBodySetLinearVelocity(ball, -pos[0] * 1.5f,
                Rnd(-speed, speed), -pos[2] * 1.5f);

            MdtBodySetAngularVelocity(ball, -pos[2] * 0.1f,
                0, -pos[0] * 0.1f);

            iang++;

            if (iang > 10000)
                iang = 0;
        }

        is = 0;
    }

    MeProfileEndSection("Dynamics");
}

void MEAPI tick(RRender* rc, void* useData)
{
    if (autoEvolve) stepEvolve();
}

void MEAPI_CDECL cleanup(void)
{
    int i;

    /* graphics */
    RRenderContextDestroy(rc);

    /* dynamics */
    MdtWorldDestroy(world);

    /* collision */
    McdGeometryDestroy(ball_prim);
    McdModelDestroy(ballCM);

    McdGeometryDestroy(cyl_prim);
    for (i = 0; i < NTorus; i++)
    {
      McdModelDestroy(torusCM[i]);
    }
    McdSpaceDestroy(space);
    MstBridgeDestroy(bridge);
    McdTerm();

    MeAppDestroy(meapp);
}

int MEAPI_CDECL main(int argc, const char **argv)
{
    int i, j;
    MeMatrix4Ptr tm;
    MeReal pos[3];
    float colors[2][4] = { {0, 0.40f, 0.60f, 1.0f}, {1, 1, 0, 1} };
    MeReal q[4];
    MeReal ballR = 3.0f;

    /*
      Graphics.
    */
    MeCommandLineOptions* options;

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc) return 1;

    outerR = 2.0f;
    innerR = 0.4f;

    /*
      Set up a physical world.
    */
    world = MdtWorldCreate(NTorus + NLinks + 1,
        NContacts + NTorus + NLinks);

    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
    SetAxisAngle(q, 0, 1.0f, 0, (MeReal) (ME_PI / 2.0f));

    /*
      Collision initialization.
    */
    frame = McdInit(McdPrimitivesGetTypeCount(), 100);

    McdPrimitivesRegisterTypes();
    McdPrimitivesRegisterInteractions();

    /*
      Create a collision space.
    */
    space = McdSpaceAxisSortCreate(McdAllAxes, 50, 100,1);

    bridge = MstBridgeCreate(frame,10);
    MstSetWorldHandlers(world);

    cyl_prim = McdCylinderCreate(outerR + innerR, 2*innerR);

    for (i = 0; i < NTorus; i++)
    {
        torus[i] = MdtBodyCreate(world);
        MdtBodyEnable(torus[i]);

        pos[0] = 0;
        pos[1] = ypos_start - i * (outerR - innerR) * 2.0f;
        pos[2] = 0;

        MdtBodySetMass(torus[i], 0.5f);
        MdtBodySetPosition(torus[i], pos[0], pos[1], pos[2]);

        if (i % 2 != 0)
            MdtBodySetQuaternion(torus[i], q[0], q[1], q[2], q[3]);

        MdtBodySetAngularVelocityDamping(torus[i], 0.3f);
        MdtBodySetLinearVelocityDamping(torus[i], 0.2f);

        /*
          Graphics.
        */
        tm = MdtBodyGetTransformPtr(torus[i]);
        torusG[i] = RGraphicTorusCreate(rc, outerR-innerR, outerR+innerR, colors[0], tm);

        torusCM[i] = McdModelCreate(cyl_prim);
        McdModelSetBody(torusCM[i], torus[i]);
        McdSpaceInsertModel(space, torusCM[i]);
    }

    MdtBodySetAngularVelocityDamping(torus[0], 20.0f);

    /* turn off collision interaction between each link and the next */
    for (i = 0; i < NTorus - 1; i++)
        McdSpaceDisablePair(torusCM[i], torusCM[i + 1]);

    /*
      Fix the first torus to the ground with a hinge joint.
    */
    joint_hinge = MdtHingeCreate(world);
    MdtHingeSetBodies(joint_hinge,torus[0],0);
    MdtHingeSetPosition(joint_hinge, 0, ypos_start, 0);
    MdtHingeSetAxis(joint_hinge, 0, 1, 0);
    MdtHingeEnable(joint_hinge);

    for (i = 0; i < NLinks; i++)
    {
        link[i] = MdtBodyCreate(world);
        MdtBodyEnable(link[i]);

        pos[0] = 0;
        pos[1] = ypos_start - (i * 2 + 1) * (outerR - innerR);
        pos[2] = 0;

        MdtBodySetMass(link[i], 0.5f);
        MdtBodySetPosition(link[i], pos[0], pos[1], pos[2]);
        MdtBodySetAngularVelocityDamping(link[i], 0.1f);
        MdtBodySetLinearVelocityDamping(link[i], 0.1f);
    }

    ball = MdtBodyCreate(world);
    MdtBodyEnable(ball);

    pos[0] = 10;
    pos[1] = ypos_start - (outerR - innerR) * NTorus;
    pos[2] = 0;

    MdtBodySetMass(ball, 1.5f);
    MdtBodySetPosition(ball, pos[0], pos[1], pos[2]);
    MdtBodySetAngularVelocityDamping(ball, 0.1f);
    MdtBodySetLinearVelocityDamping(ball, 0.1f);
    MdtBodySetLinearVelocity(ball, -speed * 1.5f, 0, 0);
    tm = MdtBodyGetTransformPtr(ball);

    ballG = RGraphicSphereCreate(rc, ballR, colors[1], tm);
    RGraphicSetTexture(rc, ballG, "ME_ball3");
    ball_prim = McdSphereCreate(ballR);
    ballCM = McdModelCreate(ball_prim);

    McdSpaceInsertModel(space, ballCM);
    McdModelSetBody(ballCM, ball);

    McdSpaceBuild(space);

    j = 0;

    for (i = 0; i < NLinks; i++)
    {
        joint[j] = MdtHingeCreate(world);
        MdtHingeSetBodies(joint[j],torus[i],link[i]);
        MdtHingeSetPosition(joint[j], 0,
            ypos_start - i * (outerR - innerR) * 2.0f, 0);

        if (i % 2 == 0)
            MdtHingeSetAxis(joint[j], 0, 0, 1);
        else
            MdtHingeSetAxis(joint[j], 1, 0, 0);

        MdtHingeEnable(joint[j]);

        j++;

        joint[j] = MdtHingeCreate(world);
        MdtHingeSetBodies(joint[j],torus[i+1],link[i]);
        MdtHingeSetPosition(joint[j], 0,
            ypos_start - (i + 1) * (outerR - innerR) * 2.0f, 0);

        if (i % 2 == 0)
            MdtHingeSetAxis(joint[j], 1, 0, 0);
        else
            MdtHingeSetAxis(joint[j], 0, 0, 1);

        MdtHingeEnable(joint[j]);

        j++;
    }

    RRenderSetUp2CallBack(rc, increaseTimeStep, 0);      /* w */
    RRenderSetLeft2CallBack(rc, toggleAutoEvolve, 0);    /* a */
    RRenderSetRight2CallBack(rc, toggleAutoShooting, 0); /* d */
    RRenderSetDown2CallBack(rc, decreaseTimeStep, 0);    /* s */

    /*
      Set up camera and lighting.
    */
    RCameraRotateAngle( rc, 0 );
    RCameraRotateElevation( rc, 0.35f);
    RCameraZoom( rc, 20.0f);
    RRenderSetWindowTitle(rc, "BallnChain example");
    RRenderCreateUserHelp(rc,help,4);
    RRenderToggleUserHelp(rc);

    RPerformanceBarCreate(rc);

    meapp = MeAppCreate(world, space, rc);

    /*
      Cleanup.
    */
#ifndef PS2
    atexit(cleanup);
#endif

    /*
      Simulation.
    */
    RRun(rc, tick, 0);

    return 0;
}
