/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/25 17:57:35 $ - Revision: $Revision: 1.83.2.6 $

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
This example shows the collision detection, Dynamics Event Manager
and Kea Dynamics working on a complicated environment.
*/

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#endif

#include <Mst.h>
#include <MeMath.h>
#include <MeViewer.h>

/* Global declarations */

/* Number of pendulums down one side of grid */
#ifndef PS2
#   define NSIDE       20
#else
    /* ps2gl has some limits... */
#   define NSIDE       9
#   define MAX_MATRIX_SIZE     (36)
#endif

#ifdef PS2
#endif

#define NPENDULUMS  (NSIDE * NSIDE)

#define penRadius  (MeReal)0.5
#define penHeight  (MeReal)2.0
#define penJoint   (MeReal)5.0
#define penDensity (MeReal)8.0
#define penSpacing (MeReal)3.0

/*  MeReal penRadius  = (MeReal)0.5; */
/*  MeReal penHeight  = (MeReal)2.0; */
/*  MeReal penJoint   = (MeReal)5.0; */
/*  MeReal penDensity = (MeReal)8.0; */
/*  MeReal penSpacing = (MeReal)3.0; */

MeReal    thingDensity = 1.0;
MeVector3 thingDim = {3.5, 3.5, 3.5};
MeVector3 thingStart = {0.8, 20.0, 0.01};

/* color of pendulums in the corners */
float topLeftColor[4]     = {1, 0, 0, 1};
float topRightColor[4]    = {1, 1, 0, 1};
float bottomLeftColor[4]  = {0, 1, 0, 1};
float bottomRightColor[4] = {0, 0, 1, 1};

/* Force applied to the box to drag it around */
MeReal thingForce = 1000;

/* collision framework */
McdFrameworkID framework;

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;

/* Collision */
/* McdDtBridge *bridge; */
MstBridgeID bridge;
McdSpaceID space;

/* Physics representations */
MdtBodyID    thing;
MdtBodyID    pen[NPENDULUMS];
MdtBSJointID joint[NPENDULUMS];

/* Graphical representations */
RGraphic *groundG;
RGraphic *thingG;
RGraphic *penG[NPENDULUMS];

#ifdef PS2
RGraphic *lineG[NPENDULUMS];
#else
RNativeLine *nlines[NPENDULUMS];
#endif

/* Collision reps */
McdModelID groundCM;
McdModelID thingCM;
McdModelID penCM[NPENDULUMS];
McdGeometryID plane_prim, box_prim, pen_prim;

MeReal gravity[3] = { 0, -10, 0 };

/* Render context */
RRender *rc;

/* Timestep size */
MeReal step = (MeReal)(0.04);

MeALIGNDATA(MeMatrix4,groundTransform,16) =
{
    {1, 0,  0, 0},
    {0, 0, -1, 0},
    {0, 1,  0, 0},
    {0, 0,  0, 1}
};


/* Functions to add forces to block to drag it around. */
void MEAPI increaseXForce(RRender * rc, void *userData)
{
    MeVector3 r, v;
    MeVector3 up = { 0,1,0 };

    RCameraGetPosition (rc, r);
    MeVector3Subtract(v, rc->m_CameraLookAt, r);
    v[1] = 0;
    MeVector3Cross(r,v,up);
    MeVector3Normalize(r);
    MeVector3Scale(r, -thingForce);

    MdtBodyEnable(thing);
    MdtBodyAddForce(thing, r[0], 0, r[2]);
}


void MEAPI decreaseXForce(RRender * rc, void *userData)
{
    MeVector3 r, v;
    MeVector3 up = { 0,1,0 };

    RCameraGetPosition (rc, r);
    MeVector3Subtract(v, rc->m_CameraLookAt, r);
    v[1] = 0;
    MeVector3Cross(r,v,up);
    MeVector3Normalize(r);
    MeVector3Scale(r, thingForce);

    MdtBodyEnable(thing);
    MdtBodyAddForce(thing, r[0], 0, r[2]);
}


void MEAPI increaseZForce(RRender * rc, void *userData)
{
    MeVector3 r, v;

    RCameraGetPosition (rc, r);
    MeVector3Subtract(v, rc->m_CameraLookAt, r);
    v[1] = 0;
    MeVector3Normalize(v);
    MeVector3Scale(v, thingForce);

    MdtBodyEnable(thing);
    MdtBodyAddForce(thing, v[0], 0, v[2]);
}


void MEAPI decreaseZForce(RRender * rc, void *userData)
{
    MeVector3 r, v;

    RCameraGetPosition (rc, r);
    MeVector3Subtract(v, rc->m_CameraLookAt, r);
    v[1] = 0;
    MeVector3Normalize(v);
    MeVector3Scale(v, -thingForce);

    MdtBodyEnable(thing);
    MdtBodyAddForce(thing, v[0], 0, v[2]);
}


void ThingCameraTrack()
{
    MeVector3 pos;
    MdtBodyGetPosition(thing, pos);
    RCameraSetLookAt(rc,pos);
}



/*
Tick() is a callback function called from the renderer's main loop
to evolve the world by 'step' seconds
*/

int frameCt=0;

void MEAPI tick(RRender * rc, void *userData)
{
    /* Update collision */

    MeProfileStartSection("Mathengine", 0);

    MeProfileStartSection("Collision", 0);
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge,space,world);
    MeProfileEndSection("Collision");

    /* Update dynamics */
    MeProfileStartSection("Dynamics", 0);
    MdtWorldStep(world,step);
    MeProfileEndSection("Dynamics");

    MeProfileEndSection("Mathengine");

    ThingCameraTrack();
    frameCt++;
}


/* Reset boxes and pendulums to initial positions */
void MEAPI reset(RRender * rc, void *userData)
{
    int i,j;

    /* Offset to start making pendulums from */
    MeReal start = (NSIDE-1) * -penSpacing * (MeReal)0.5;

    MdtBodySetPosition(thing, thingStart[0], thingStart[1], thingStart[2]);
    MdtBodySetQuaternion(thing, 1, 0, 0, 0);
    MdtBodySetLinearVelocity(thing, 0, 0, 0);
    MdtBodySetAngularVelocity(thing, 0, 0, 0);
    MdtBodyEnable(thing);

    for (i = 0; i < NSIDE; i++)
    {
        for(j=0; j < NSIDE; j++)
        {
            MdtBodySetPosition(pen[(i*NSIDE)+j], start + i * penSpacing, penHeight, start + j * penSpacing);
            MdtBodySetQuaternion(pen[(i*NSIDE)+j], 1, 0, 0, 0);
            MdtBodySetLinearVelocity(pen[(i*NSIDE)+j], 0, 0, 0);
            MdtBodySetAngularVelocity(pen[(i*NSIDE)+j], 0, 0, 0);
            MdtBodyEnable(pen[(i*NSIDE)+j]);
        }
    }
}

void MEAPI_CDECL cleanup(void)
{
    McdSpaceDestroy(space);
    MstBridgeDestroy(bridge);
    McdFrameworkDestroyAllModelsAndGeometries(framework);
    McdTerm(framework);
    MdtWorldDestroy(world);
    RRenderContextDestroy(rc);
}


/* Main Routine */
int MEAPI_CDECL main(int argc, const char **argv)
{
    static char *help[] =
    {
        "$ACTION2: reset",
        "$UP, $DOWN, $LEFT, $RIGHT : drag block"
    };
    const int helpNum = 2;

    int i, j, k;
    float color[4] = {1,1,1,1};
    MeReal mass, start;
    MeMatrix3 I;
    AcmeReal lineStart[3] = { 0.0, penRadius, 0.0};
    AcmeReal lineEnd[3] = { 0.0, penJoint - penHeight, 0.0};

    float tempColorTop[4], tempColorBottom[4];
    MeReal propI, propJ;
    MdtContactParamsID props;

    MeCommandLineOptions* options;

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

    /* Initialise dynamics */
    world = MdtWorldCreate(NPENDULUMS + 1, NPENDULUMS + 100, 1, 1);

    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    /*
        In this demo, a lot of objects are stationary a lot of the time.
        To speed things up we tell Mdt to AutoDisable objects when they
        come to rest. Disabled MdtBodies are automatically re-Enabled
        when an Enabled body contacts them.
    */
    MdtWorldSetAutoDisable(world, 1);

#ifdef PS2
    MdtWorldSetMaxMatrixSize(world,MAX_MATRIX_SIZE);
#endif

    /* THING: */
    thing = MdtBodyCreate(world);

    mass = thingDensity * thingDim[0] * thingDim[1] * thingDim[2];

    MdtMakeInertiaTensorBox(mass, thingDim[0], thingDim[1], thingDim[2], I);

    MdtBodySetMass(thing, mass);
    MdtBodySetInertiaTensor(thing, (void *)I);

    /* Add a little (angular)velocity damping. */
    MdtBodySetAngularVelocityDamping(thing, 0.03);
    MdtBodySetLinearVelocityDamping(thing, 0.02);

    /* PENDULUMS: */

    mass = ME_PI * penRadius * penRadius * penDensity;
    MdtMakeInertiaTensorSphere(mass, penRadius, I);

    for (i = 0; i < NPENDULUMS; i++)
    {
        pen[i] = MdtBodyCreate(world);

        MdtBodySetMass(pen[i], mass);
        MdtBodySetInertiaTensor(pen[i], (void *)I);

        MdtBodySetLinearVelocityDamping(pen[i], (MeReal)(0.2));
        MdtBodySetAngularVelocityDamping(pen[i], (MeReal)(0.1));
    }

    /* reset pendulums to correct positions.. */
    reset(rc,0);

    /* offset to start making pendulums from */
    start = (NSIDE-1) * -penSpacing * (MeReal)0.5;

    /* now make all the joints */
    for (i = 0; i < NSIDE; i++)
    {
        for(j=0; j < NSIDE; j++)
        {
            joint[(i*NSIDE)+j] = MdtBSJointCreate(world);
            MdtBSJointSetBodies(joint[(i*NSIDE)+j],pen[(i*NSIDE)+j], 0);
            MdtBSJointSetPosition(joint[(i*NSIDE)+j], start + i * penSpacing, penJoint, start + j * penSpacing);
            MdtBSJointEnable(joint[(i*NSIDE)+j]);
        }
    }


    /* Collision detection */
    framework = McdInit(0, 500, 0,1);

    McdPrimitivesRegisterTypes(framework);
    McdPrimitivesRegisterInteractions(framework);

    /* max objects and pairs */
    space = McdSpaceAxisSortCreate(framework,McdAllAxes, NPENDULUMS + 2, 150);
    bridge = MstBridgeCreate(framework,1);
    MstSetWorldHandlers(world);

    /* Set parameters for contacts. */
    props = MstBridgeGetContactParams(bridge,
        MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsSetType(props, MdtContactTypeFriction2D);
    MdtContactParamsSetRestitution(props, 0.3);
#ifdef PS2
    MdtContactParamsSetFriction(props, MEINFINITY);
    MdtContactParamsSetPrimarySlip(props, 1.0f);
    MdtContactParamsSetSecondarySlip(props, 1.0f);
    MdtContactParamsSetMaxAdhesiveForce(props, MEINFINITY);
#else
    MdtContactParamsSetFriction(props, 2.0f);
#endif

    plane_prim = McdPlaneCreate(framework);
    groundCM = McdModelCreate(plane_prim);

    McdSpaceInsertModel(space, groundCM);
    McdModelSetBody(groundCM, 0);

    McdModelSetTransformPtr(groundCM, groundTransform);

    McdSpaceUpdateModel(groundCM);
    McdSpaceFreezeModel(groundCM);

    box_prim = McdBoxCreate(framework,thingDim[0],
        thingDim[1], thingDim[2]);
    thingCM = McdModelCreate(box_prim);
    McdModelSetBody(thingCM, thing);
    McdSpaceInsertModel(space, thingCM);



    pen_prim = McdBoxCreate(framework,2 * penRadius, 2 * penRadius, 2 * penRadius);
    for (i = 0; i < NPENDULUMS; i++)
    {
        penCM[i] = McdModelCreate(pen_prim);
        McdModelSetBody(penCM[i], pen[i]);
        McdSpaceInsertModel(space, penCM[i]);
    }

    McdSpaceBuild(space);

    /* Initialise rendering attributes */

    RCameraSetView(rc, 30,0,0.5);

    /* GROUND: */

    groundG = RGraphicGroundPlaneCreate(rc, 50.0f,30, color,0);
    RGraphicSetTexture(rc, groundG, "checkerboard");

    color[0] = 1;
    color[1] = 1;
    color[2] = 1;
    color[3] = 1;

    thingG =
        RGraphicBoxCreate(rc, thingDim[0], thingDim[1], thingDim[2],
        color, MdtBodyGetTransformPtr(thing));

    RGraphicSetTexture(rc, thingG, "wood1");

    /* make graphics (pretty colours) */
    for (i = 0; i < NSIDE; i++)
    {
        for(j=0; j < NSIDE; j++)
        {
            propI = (MeReal)i/((MeReal)NSIDE-1);
            propJ = (MeReal)j/((MeReal)NSIDE-1);

            for(k=0; k<3; k++)
            {
                tempColorTop[k] =
                    ((float)propI * (topRightColor[k] - topLeftColor[k])) + topLeftColor[k];
                tempColorBottom[k] =
                    ((float)propI * (bottomRightColor[k] - bottomLeftColor[k])) + bottomLeftColor[k];
                color[k] =
                    ((float)propJ * (tempColorTop[k] - tempColorBottom[k])) + tempColorBottom[k];
            }

            penG[(i*NSIDE)+j] =
                RGraphicBoxCreate(rc, 2*penRadius, 2*penRadius, 2*penRadius,
                color, MdtBodyGetTransformPtr(pen[(i*NSIDE)+j]));
#ifdef PS2
            lineG[(i*NSIDE)+j] =
                RGraphicLineCreate(rc, lineStart, lineEnd,
                color, MdtBodyGetTransformPtr(pen[(i*NSIDE)+j]));
#else
            nlines[(i*NSIDE)+j] = RNLineCreate(rc, lineStart, lineEnd, color, 
                MdtBodyGetTransformPtr(pen[(i*NSIDE)+j]));
#endif
        }
    }

    /* CONTROLS: */

    RRenderSetActionNCallBack(rc, 2, reset, 0);

    RRenderSetUpCallBack(rc, increaseZForce, 0);
    RRenderSetDownCallBack(rc, decreaseZForce, 0);
    RRenderSetRightCallBack(rc, increaseXForce, 0);
    RRenderSetLeftCallBack(rc, decreaseXForce, 0);

    RPerformanceBarCreate(rc);
    RRenderSetWindowTitle(rc,"ManyPendulums example");
    RRenderCreateUserHelp(rc, help, helpNum);
    RRenderToggleUserHelp(rc);

    /* Cleanup after simulation. */
#ifndef PS2
    atexit(cleanup);
#endif
    /* Run the Simulation. */

    /*
    RRun() executes the main loop.

      Pseudocode: while no exit-request { Handle user input call Tick() to
      evolve the simulation and update graphic transforms Draw graphics }
    */

    RRun(rc, tick, 0);

    return 0;
}
