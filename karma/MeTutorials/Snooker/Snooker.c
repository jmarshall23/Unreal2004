/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:38 $ - Revision: $Revision: 1.56.8.1 $

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

  A no-pockets European style of snooker where the objective is to hit
  combinations, not sink balls. Showing different contact properties of
  ball-ball, ball-table, and ball-cushion collisions.
*/

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <Mdt.h>
#include <MeMath.h>
#include <McdFrame.h>
#include <McdPrimitives.h>
#include <Mst.h>
#include <MeViewer.h>

/*
  Global declarations
*/

#define NBalls 6
#define NCushions 4

#define TABLELENGTH 4
#define TABLEWIDTH 2
#define CUSHIONWIDTH 0.1

MeReal ballDensity = 2000;
MeReal ballRadius = 0.05;

MeReal cushionPos[NCushions][3] =
{
    {TABLEWIDTH / 2, 0.0, 0.0},
    {-TABLEWIDTH / 2, 0.0, 0.0},
    {0.0, 0.0, TABLELENGTH / 2},
    {0.0, 0.0, -TABLELENGTH / 2}
};

MeReal cushionDim[NCushions][3] =
{
    {CUSHIONWIDTH, CUSHIONWIDTH, TABLELENGTH / 2},
    {CUSHIONWIDTH, CUSHIONWIDTH, TABLELENGTH / 2},
    {TABLEWIDTH / 2, CUSHIONWIDTH, CUSHIONWIDTH},
    {TABLEWIDTH / 2, CUSHIONWIDTH, CUSHIONWIDTH}
};

MeReal strikePower = 4;

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;
MstBridgeID bridge;
McdSpaceID space;

/*
  Physics representations
*/
MdtBodyID ball[NBalls];

/*
  Graphical representations
*/
RGraphic *groundG;
RGraphic *ballG[NBalls];
RGraphic *cushionG[4];

/*
  Collision reps
*/
McdModelID groundCM;
McdModelID ballCM[NBalls];
McdModelID cushionCM[4];
McdGeometryID plane_prim, ball_prim[NBalls], box_prim[4];

MeMatrix4 cushionTM[4];

MeReal gravity[3] = { 0, -5, 0 };

/* Render context */
RRender *rc;

int autoEvolve = 1;
int followBall = 0;

MeReal step = (MeReal)(0.02);

MeMatrix4 groundTransform =
{
    {  1,    0,  0, 0},
    {  0,    0, -1, 0},
    {  0,    1,  0, 0},
    {  0, -0.0,  0, 1}
};

MeMatrix4 groundRenderTransform =
{
    {  1,     0,  0,  0},
    {  0,     0, -1,  0},
    {  0,     1,  0,  0},
    {  0, -0.02,  0,  1}
};

void stepEvolve(void);

void MEAPI toggleFollow(RRender* rc, void* userdata)
{
    followBall = !followBall;
}

void MEAPI toggleHelp(RRender* rc, void* userdata)
{
    RRenderToggleUserHelp(rc);
}

void MEAPI toggleAutoEvolve(void)
{
    autoEvolve = !autoEvolve;
}

void killAll(void)
{
    int i;

    for (i = 0; i < NBalls; i++)
        MdtBodyDisable(ball[i]);
}

void wakeAll(void)
{
    int i;

    for (i = 0; i < NBalls; i++)
        MdtBodyEnable(ball[i]);
}

void MEAPI centerCueBall(RRender* rc, void* userdata)
{
    MeVector3 pos;

    MdtBodyGetPosition(ball[0], pos);

    RCameraSetLookAt(rc, pos);
}

void MEAPI shoot(RRender* rc, void* userdata)
{
    int i;
    MeVector3 v;
    MeVector3 campos;
    MdtBodyEnable(ball[0]);

    RCameraGetPosition(rc, campos);

    MeVector3Subtract(v, rc->m_CameraLookAt, campos);

    v[1] = 0;

    MeVector3Normalize(v);

    for (i = 0; i < 3; i++)
        v[i] *= strikePower;

    MdtBodySetLinearVelocity(ball[0], v[0], v[1], v[2]);

    MdtBodySetAngularVelocity(ball[0], 0, 0, 0);
}

void stepEvolve(void)
{
    /*
      These timer calls are for the OpenGL performance bar.
    */

    if (followBall)
        centerCueBall(rc, 0);

    MeProfileStartSection("Collision", 1);
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge,space,world);
    MeProfileStartSection("Dynamics", 0);
    MdtWorldStep(world,step);
    MeProfileEndSection("Dynamics");
}

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds.
*/
void MEAPI tick(RRender * rc, void* userdata)
{
    if (autoEvolve)
    {
        stepEvolve();
    }
}

void MEAPI_CDECL cleanup(void)
{
    /* free collision */
    int i;
    McdGeometryDestroy(plane_prim);
    McdModelDestroy(groundCM);
    for (i = 0; i < NBalls; i++)
    {
        McdGeometryDestroy(ball_prim[i]);
        McdModelDestroy(ballCM[i]);
    }
    for (i = 0; i < 4; i++)
    {
        McdGeometryDestroy(box_prim[i]);
        McdModelDestroy(cushionCM[i]);
    }
    McdSpaceDestroy(space);
    MstBridgeDestroy(bridge);
    McdTerm();

    /* free dynamics */
    MdtWorldDestroy(world);

    /* free rendering */
    RRenderContextDestroy(rc);

}

/*
  Callback to align friction box with relative surface velocity.
*/
int MEAPI BallContactCB(McdIntersectResult * result, MdtContactGroupID contacts)
{
    MdtBodyID body;
    MeVector3 v, p;
    MeReal mag, norm;

    if (!contacts[0])
        return 1;

    body = MdtContactGetBody(contacts[0],0);

    MdtContactGetPosition(contacts[0], p);
    MdtBodyGetVelocityAtPoint(body, p, v);

    /* Remove any upward component. */
    v[1] = 0;

    mag = MeSqrt(v[0] * v[0] + v[2] * v[2]);

    /*
      Do nothing if no relative surface velocity.
    */
    if (mag < 0.001)
        return 1;

    norm = (MeReal) 1.0 / mag;
    v[0] *= norm;
    v[2] *= norm;

    MdtContactSetDirection(contacts[0], v[0], v[1], v[2]);

    return 1;
}

/*
    Main Routine
 */

int MEAPI_CDECL main(int argc, const char **argv)
{
    int i;
    float color[3];

    MdtContactParamsID props;
    int ballMaterial, tableMaterial, cushionMaterial;
    MeMatrix3 I;
    MeCommandLineOptions *options;

    static char *help[] =
    {
        "Aim using $MOUSE to change camera position/orientation.",
        "$ACTION0 strike",
        "$ACTION1 center view on cue ball",
        "$ACTION2 toggle follow cam"
    };

    const int helpNum = sizeof (help) / sizeof (help[0]);


    /*
      Initialise dynamics
    */
    world = MdtWorldCreate(NBalls, 100);

    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);


    /*
      BALLS:
    */

    for (i = 0; i < NBalls; i++)
    {
        ball[i] = MdtBodyCreate(world);

        MdtBodySetMass(ball[i],
            ballDensity * ((MeReal) 1 / 3) * ME_PI * ballRadius *
            ballRadius * ballRadius);

        MdtMakeInertiaTensorSphere(MdtBodyGetMass(ball[i]), ballRadius,
            I);
        MdtBodySetInertiaTensor(ball[i], I);

        MdtBodySetPosition(ball[i], (MeReal) 0.01 * i, 0.1, (MeReal) 0.2 * i);

        MdtBodySetLinearVelocityDamping(ball[i], (MeReal)(0.003));
        MdtBodySetAngularVelocityDamping(ball[i], (MeReal)(0.0003));

        MdtBodyEnable(ball[i]);
    }

    /*
      Collision detection
    */

    McdInit(McdPrimitivesGetTypeCount(), 100);

    McdPrimitivesRegisterTypes();
    McdPrimitivesRegisterInteractions();

    /* max objects and pairs */
    space = McdSpaceAxisSortCreate(McdAllAxes, 50, 150,1);

    /* max contacts */
    bridge = MstBridgeCreate(10);
    MstSetWorldHandlers(world);

    ballMaterial = MstBridgeGetNewMaterial(bridge);
    tableMaterial = MstBridgeGetNewMaterial(bridge);
    cushionMaterial = MstBridgeGetNewMaterial(bridge);

    /*
      Set parameters for ball-table contact.
    */
    props = MstBridgeGetContactParams(bridge, ballMaterial, tableMaterial);

    MstBridgeSetPerPairCB(bridge, ballMaterial, tableMaterial, BallContactCB);

    MdtContactParamsSetType(props, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(props, 5.0);
    MdtContactParamsSetRestitution(props, 0.1);

    /*
      Set parameters for ball-ball contact.
    */
    props = MstBridgeGetContactParams(bridge, ballMaterial, ballMaterial);

    MdtContactParamsSetType(props, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(props, 1.0);
    MdtContactParamsSetRestitution(props, 0.5);

    /*
      Set parameters for ball-cushion contact.
    */
    props = MstBridgeGetContactParams(bridge, ballMaterial, cushionMaterial);

    MdtContactParamsSetType(props, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(props, 5.0);
    MdtContactParamsSetRestitution(props, 0.85);

    /*
      Table collision.
    */
    plane_prim = McdPlaneCreate();

    groundCM = McdModelCreate(plane_prim);

    McdModelSetMaterial(groundCM, tableMaterial);
    McdModelSetTransformPtr(groundCM, groundTransform);
    McdSpaceInsertModel(space, groundCM);
    McdModelSetBody(groundCM, 0);

    for (i = 0; i < NCushions; i++)
    {
        box_prim[i] = McdBoxCreate(2*cushionDim[i][0], 2*cushionDim[i][1],
            2*cushionDim[i][2]);
        cushionCM[i] = McdModelCreate(box_prim[i]);

        McdModelSetMaterial(cushionCM[i], cushionMaterial);

        McdModelSetBody(cushionCM[i], 0);

        MeMatrix4TMMakeIdentity(cushionTM[i]);
        cushionTM[i][3][0] = cushionPos[i][0];
        cushionTM[i][3][1] = cushionPos[i][1];
        cushionTM[i][3][2] = cushionPos[i][2];

        McdModelSetTransformPtr(cushionCM[i], cushionTM[i]);

        McdSpaceInsertModel(space, cushionCM[i]);
    }

    McdSpaceUpdateAll(space);
    McdSpaceFreezeModel(groundCM);
    for(i=0; i< NCushions; i++)
    {
      McdSpaceFreezeModel(cushionCM[i]);
    }

    /*
      Ball collision.
    */
    for (i = 0; i < NBalls; i++)
    {
        ball_prim[i] = McdSphereCreate(ballRadius);
        ballCM[i] = McdModelCreate(ball_prim[i]);

        McdModelSetMaterial(ballCM[i], ballMaterial);

        McdModelSetBody(ballCM[i], ball[i]);
        McdSpaceInsertModel(space, ballCM[i]);
    }

    /*
      Cushion collision.
    */
    McdSpaceBuild(space);


    /*
      Initialise rendering attributes
    */

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;


    /* Uncomment this to enable camera following cue ball, and thus disable
       some of the mouse camera functionality */
#if 0
    {
        MeVector3 lightdirection = {0, 0, 1};
        rc->m_DirectLight1.m_bUseLight = 1;
        MeVector3Copy(rc->m_DirectLight1.m_Direction, lightdirection);
        rc->m_CameraDist = 1.5;

        RCameraUpdate(rc);
    }
#endif

    /*
      GROUND:
    */

    color[0] = 0;
    color[1] = 0.5f;
    color[2] = 0;
    color[3] = 0;

    groundG = RGraphicBoxCreate(rc, TABLEWIDTH, TABLELENGTH, 0.04f, color,
        groundRenderTransform);

    /*
      CUE BALL:
    */
    color[0] = 0.85f;
    color[1] = 0.85f;
    color[2] = 0.85f;
    color[3] = 1.0;
    ballG[0] = RGraphicSphereCreate(rc,
        ballRadius, color, MdtBodyGetTransformPtr(ball[0]));

    /*
      OTHER BALLS:
    */
    for (i = 1; i < NBalls; i++)
    {
        color[0] = 0.85f;
        color[1] = 0.15f;
        color[2] = 0.0f;
        color[3] = 1.0;
        ballG[i] = RGraphicSphereCreate(rc,
            ballRadius, color, MdtBodyGetTransformPtr(ball[i]));
    }

    for (i = 0; i < NCushions; i++)
    {
        color[0] = 0.08f;
        color[1] = 0.2f;
        color[2] = 0.08f;
        color[3] = 0.0;
        cushionG[i] = RGraphicBoxCreate(rc,
            2 * cushionDim[i][0], 2 * cushionDim[i][1],
            2 * cushionDim[i][2], color, cushionTM[i]);
    }

    /*
      Keys:
    */

    RRenderSetActionNCallBack(rc, 1, shoot, 0);
    RRenderSetActionNCallBack(rc, 2, centerCueBall, 0);
    RRenderSetActionNCallBack(rc, 3, toggleFollow, 0);
    RRenderSetActionNCallBack(rc, 0, toggleHelp, 0);

    RRenderCreateUserHelp(rc, help, helpNum);
    strncpy(rc->m_AppName, "Snooker tutorial", 40);

    /*
      Cleanup after simulation.
    */

    atexit(cleanup);

    /* Initial Camera Position */

    centerCueBall(rc, 0);
    RCameraSetView(rc, 1.5, ME_PI/8, ME_PI/6);
    RRenderSetWindowTitle(rc, "Snooker tutorial");

    rc->m_PerformanceBar = RPerformanceBarCreate(rc);

    /*
      Run the Simulation
    */

    RRun(rc, tick, 0);

    return 0;
}
