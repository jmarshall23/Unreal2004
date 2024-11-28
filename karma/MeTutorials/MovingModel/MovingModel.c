/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:36 $ - Revision: $Revision: 1.7.2.2 $

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

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <MeMath.h>
#include <MeViewer.h>
#include <MeApp.h>

/* Global declarations */

/* Help text */
char *help[2] =
{
    "$ACTION2 - reset",
    "$MOUSE - mouse force"
};


/* Render context */
RRender *rc;
MeApp* meapp;

/* World for the Dynamics Toolkit simulation */
McdFrameworkID framework;
MstUniverseID universe;
McdSpaceID space;
MdtWorldID world;
MstBridgeID bridge;

MeReal gravity[3] = { 0, -7, 0 };
MeReal step = (MeReal)(0.03);

/**** Moving platform *****/
McdGeometryID platformGeom;
McdModelID platform;
RGraphic *platformG;

MeVector3 platformSize = {1, 0.5, 1};
float platformColor[4] = {1, 1, 1, 1};


MeMatrix4* platformTM;

#define NWAYPOINTS (4)

MeVector3 platformWaypoint[NWAYPOINTS] =
{
    {0, 2, 0},
    {2, 2, 1},
    {2, 4, -1},
    {0, 4, 0}
};

MeReal waypointSpeed = 4;
MeReal waypointTime = 0; /* time until next waypoint */
int waypointNext;
int waypointLast;
MeVector3 platformVel;

/* Material for floor. */
MstMaterialID platformMaterial;

/**** Boxes ****/
#define NBOXES (2)
McdGeometryID boxGeom[NBOXES];
McdModelID box[NBOXES];
RGraphic *boxG[NBOXES];

MeReal boxDensity = (MeReal)2.7;

/* Radius of each box. */
MeVector3 boxSize[NBOXES] =
{
    {0.6, 0.6, 0.6}, 
    {0.5, 0.5, 0.5}
};

/* Position of each box */
MeVector3 boxPos[NBOXES] =
{
    {0.07, 3, 0}, 
    {0, 4, 0}
};

/* Color of each box */
float boxColor[NBOXES][4] =
{
    {0.0 , 0.73, 0, 1}, 
    {0.73 , 0.73, 0, 1}
};

/**** Pendulum ****/
McdGeometryID ballGeom;
McdModelID ball;
RGraphic* ballG;

MeReal ballRadius = (MeReal)0.25;
MeReal ballDensity = (MeReal)2.7;
float ballColor[4] = {1, 0, 0, 1};

/* Initial position of ball. */
MeVector3 ballStart = {0, 1, 0};

MdtBSJointID joint;

/* Position of joint relative to moving platform. */
MeVector3 jointPosRel = {0, -0.5, 0};

/**** Ground *****/
McdGeometryID groundGeom;
McdModelID ground;
RGraphic *groundG;

MeMatrix4* groundTM;
MeMatrix4 baseGroundTM =
{
    {1,  0,  0, 0},
    {0,  0, -1, 0},
    {0,  1,  0, 0},
    {0, -1,  0, 1}
};

MeMatrix4 groundRenderTM =
{
    {1,     0,  0, 0},
    {0,     0, -1, 0},
    {0,     1,  0, 0},
    {0, -1.05,  0, 1}
};

/* Called for every contact against moving platform, so we can set contact velocity. */
MeBool MEAPI platformContactCB(McdIntersectResult* result, 
                             McdContact* colC,
                             MdtContactID dynC)
{
    /*  Set the velocity at the contact with the world so boxes stay on the
        moving paltform. */
    MdtContactSetWorldVelocity(dynC, 
        platformVel[0], platformVel[1], platformVel[2]);

    return 1;
}

void MEAPI tick(RRender * rc,void *userdata)
{
    MeVector3 newPos, oldPos, delta, jPos;
    MeReal alpha;
    int advance;

    /* Update kinematic moving platform position. */
    MeVector3Copy(oldPos, (*platformTM)[3]);

    waypointTime += step;

    alpha = waypointTime/waypointSpeed;
    advance = (int)alpha; /* waypoints to skip forward */
    alpha = alpha - (MeReal)advance; /* partial amount towards next waypoint */

    waypointNext = (waypointNext + advance)%NWAYPOINTS;
    waypointLast = (waypointNext == 0)?NWAYPOINTS - 1:waypointNext-1;
    waypointTime -= (advance * waypointSpeed);

    MeVector3Subtract(delta, 
        platformWaypoint[waypointNext], 
        platformWaypoint[waypointLast]);

    MeVector3Scale(delta, alpha);
    MeVector3Add(newPos, platformWaypoint[waypointLast], delta);
    MeMatrix4TMSetPositionVector(*platformTM, newPos);

    MeVector3Subtract(platformVel, newPos, oldPos);
    MeVector3Scale(platformVel, 1/step);

    /* Move joint that attaches pendulum to the moving platform. */
    MeMatrix4TMTransform(jPos, *platformTM, jointPosRel);
    MdtConstraintBodySetPosition(MdtBSJointQuaConstraint(joint), 
        1, jPos[0], jPos[1], jPos[2]);
    MdtConstraintSetWorldLinearVelocity(MdtBSJointQuaConstraint(joint), 
        platformVel[0], platformVel[1], platformVel[2]);

    /* Evolve the physics */
    MeAppStep(meapp);
    MstUniverseStep(universe, step);
}

/* Reset boxes and platform to initial positions */
void MEAPI reset(RRender* rc, void* userData)
{
    MeVector3 jPos;

    /* Reset box positions */
    int i;
    for (i = 0; i < NBOXES; i++)
    {
        McdModelDynamicsReset(box[i]);
        McdModelDynamicsSetPosition(box[i], boxPos[i][0], boxPos[i][1], boxPos[i][2]);
        McdModelDynamicsEnable(box[i]);
    }

    /* Reset moving platform positions */
    waypointLast = 0;
    MeMatrix4TMMakeIdentity(*platformTM);
    MeMatrix4TMSetPositionVector(*platformTM, platformWaypoint[waypointLast]);

    waypointNext = 1;
    waypointTime = 0;

    /* Reset joint position. */
    MeMatrix4TMTransform(jPos, *platformTM, jointPosRel);
    MdtBSJointSetPosition(joint, jPos[0], jPos[1], jPos[2]);
}

void MEAPI_CDECL cleanup(void)
{
    int i;

    MeAppDestroy(meapp);

    McdModelDestroy(ground);
    McdGeometryDestroy(groundGeom);

    for (i = 0; i < NBOXES; i++)
    {
      MstModelAndBodyDestroy(box[i]);
      McdGeometryDestroy(boxGeom[i]);
    }

    McdModelDestroy(platform);
    McdGeometryDestroy(platformGeom);

    MstUniverseDestroy(universe);
    RRenderContextDestroy(rc);

    MeMemoryAPI.destroyAligned(groundTM);
}

void MEAPI togglehelp(RRender* rc, void* userdata) 
{
    RRenderToggleUserHelp(rc);
}

/* Main Routine */
int MEAPI_CDECL main(int argc, const char * argv[])
{
    int i;
    float color[4];
    MstUniverseSizes sizes;
    MeCommandLineOptions* options;
    MdtBclContactParams *p;

    /* Initialise rendering attributes, eating command line parameters we
       recognize. */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

    /* ************* MAKE PHYSICS *************** */

    sizes = MstUniverseDefaultSizes;
    sizes.dynamicBodiesMaxCount = 100;
    sizes.dynamicConstraintsMaxCount = 300;
    sizes.materialsMaxCount = 2;
    sizes.collisionModelsMaxCount = 200;
    sizes.collisionPairsMaxCount = 1000;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.collisionUserGeometryTypesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);
    framework = MstUniverseGetFramework(universe);
    space = MstUniverseGetSpace(universe);
    world = MstUniverseGetWorld(universe);
    bridge = MstUniverseGetBridge(universe);

    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    /* CONTACT PARAMS */

    /* box-box/box-ground */
    p = MstBridgeGetContactParams(bridge,
            MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsSetType(p, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(p, 2);
    MdtContactParamsSetRestitution(p, 0.3);

    /* box-platform */
    platformMaterial = MstBridgeGetNewMaterial(bridge);

    p = MstBridgeGetContactParams(bridge,
            platformMaterial, MstBridgeGetDefaultMaterial());

    MdtContactParamsSetType(p, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(p, 2);
    MdtContactParamsSetRestitution(p, 0.3);
    
    MstBridgeSetPerContactCB(bridge, 
        platformMaterial, MstBridgeGetDefaultMaterial(),
        platformContactCB);


    /* GROUND PLANE */
    groundTM = (MeMatrix4*)MeMemoryAPI.createAligned(sizeof(MeMatrix4), 16);
    MeMatrix4Copy(*groundTM, baseGroundTM);
    groundGeom = McdPlaneCreate(framework);
    ground = MstFixedModelCreate(universe,groundGeom,*groundTM);

    /* Make the graphics */
    color[0] = color[1] = color[2] = color[3] = 1;
    groundG = RGraphicGroundPlaneCreate(rc, 30.0f,30, color, -1);
    RGraphicSetTexture(rc, groundG, "checkerboard");

    /* BOXES */
    for (i = 0; i < NBOXES; i++)
    {

        boxGeom[i] = McdBoxCreate(framework, 
            boxSize[i][0], boxSize[i][1], boxSize[i][2]);

        box[i] = MstModelAndBodyCreate(universe, boxGeom[i], boxDensity);
        McdModelDynamicsSetDamping(box[i], 0.2, 0.1);

        boxG[i] = RGraphicBoxCreate(rc, 
            boxSize[i][0], boxSize[i][1], boxSize[i][2], 
            boxColor[i], McdModelGetTransformPtr(box[i]));
    }

    /* PLATFORM */
    platformGeom = McdBoxCreate(framework, 
        platformSize[0], platformSize[1], platformSize[2]);

    platform = McdModelCreate(platformGeom);

    platformTM = MeMemoryAPI.createAligned(sizeof(MeMatrix4), 16);
    MeMatrix4TMMakeIdentity(*platformTM);
    McdModelSetTransformPtr(platform, *platformTM);
    McdModelSetMaterial(platform, platformMaterial);

    platformG = RGraphicBoxCreate(rc,
        platformSize[0], platformSize[1], platformSize[2],
        platformColor, McdModelGetTransformPtr(platform));
    RGraphicSetTexture(rc, platformG, "stone");
   
    /* PENDULUM */
    ballGeom = McdSphereCreate(framework, ballRadius);
    ball = MstModelAndBodyCreate(universe, ballGeom, ballDensity);
    ballG = RGraphicSphereCreate(rc, ballRadius, ballColor, 
        McdModelGetTransformPtr(ball));

    /* Create joint between pendulum and 'world'. */
    joint = MdtBSJointCreate(world);
    MdtBSJointSetBodies(joint, McdModelGetBody(ball), 0);
    MdtBSJointEnable(joint);

    reset(rc, 0);

    McdSpaceInsertModel(space, platform);
    McdSpaceUpdateModel(platform);




    /* ***************** END PHYSICS ***************** */

    meapp = MeAppCreate(world, space, rc);

    RPerformanceBarCreate(rc);
    RRenderSkydomeCreate(rc, "skydome", 2, 1);
    RCameraSetView(rc, 7, ME_PI/4, ME_PI/4);
    RCameraSetLookAt(rc, platformWaypoint[0]);
    

    /* CONTROLS: */
    RRenderSetActionNCallBack(rc, 2, reset, 0);

    RRenderSetWindowTitle(rc, "Moving Model Example");
    RRenderCreateUserHelp(rc, help, 2);
    RRenderToggleUserHelp(rc);

    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);

    /* Cleanup after simulation. */
    atexit(cleanup);

    /* Run the Simulation. */
    RRun(rc, tick,0);

    return 0;
}
