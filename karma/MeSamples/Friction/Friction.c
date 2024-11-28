/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:11 $ - Revision: $Revision: 1.12.4.4 $

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
#include "MeDebugDraw.h"

/* help text */
#define HELPSIZE (4)

char *help[HELPSIZE] =
{
    "$ACTION2 - reset",
    "$ACTION1 - toggle options menu",
    "$UP/$DOWN - increase/decrease ramp angle",
    "$MOUSE - mouse force"
};


#define         NUMBOXES (2)
MeVector3       boxSize = {1, 1, 1};
MeVector3       boxStart = {0, 2, 0};
MeVector3       boxSpace = {0, 1, 0};
float           boxColor[2][4] = {{1, 1, 0, 1},{1, 0.5f, 0, 1}};

MeVector3       rampSize = {5, 0.2f, 2};
MeVector3       rampStart = {0, 1, 0};
float           rampColor[4] = {0, 0, 1, 1};

/* World for the Dynamics Toolkit simulation */
MstUniverseID   universe;
McdSpaceID      space;
MdtWorldID      world;
MstBridgeID     bridge;
McdFrameworkID  framework;

/* Physics representations */
McdGeometryID   planeGeom;
McdModelID      plane;

McdGeometryID   boxGeom;
McdModelID      box[NUMBOXES];
RGraphic*       boxG[NUMBOXES];

McdGeometryID   rampGeom;
McdModelID      ramp;
RGraphic*       rampG;

MdtHingeID      hinge = 0;
MeReal          hingeAngle = 0;
MeReal          hingeAngleDelta = 0.02f;

#define         FRICTION_FORCE (2.0f)
#define         FRICTION_COEFF (0.2f)

#define         DEFAULT_MODEL (1)
#define         DEFAULT_DRAW (1)

/* Graphical representations */
RGraphic *groundG;

MeReal gravity[3] = { 0, -9.8, 0 };

/* Render context */
RRender *rc;
RMenu* menu;

MeApp* meapp;

MeReal step = (MeReal)(0.03);

MeALIGNDATA(MeMatrix4,groundTM,16) =
{
       {1,  0,  0, 0},
       {0,  0, -1, 0},
       {0,  1,  0, 0},
       {0, -1,  0, 1}
};

void MEAPI simpleDraw(MeVector3 start, MeVector3 end, MeReal r, MeReal g, MeReal b)
{
#ifndef PS2
    RLineAdd(rc, start, end, r, g, b);
#endif
}

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender * rc,void *userdata)
{
    RLineRemoveAll(rc);

    MeAppStep(meapp);
    MdtLimitController(MdtHingeGetLimit(hinge), hingeAngle, ME_PI/6, 1, 10000);
    MstUniverseStep(universe, step);
}

void MEAPI increaseTilt(RRender* rc, void* userData)
{
    hingeAngle += hingeAngleDelta;
    McdModelDynamicsEnable(ramp);
}

void MEAPI decreaseTilt(RRender* rc, void* userData)
{
    hingeAngle -= hingeAngleDelta;
    McdModelDynamicsEnable(ramp);
}

/* Reset boxes and balls to initial positions */
void MEAPI reset(RRender* rc, void* userData)
{
    int i;

    for(i=0; i<NUMBOXES; i++)
    {
        McdModelDynamicsSetPosition(box[i],
            boxStart[0] + boxSpace[0] * i,
            boxStart[1] + boxSpace[1] * i,
            boxStart[2] + boxSpace[2] * i);

        McdModelDynamicsSetQuaternion(box[i], 1, 0, 0, 0);
        McdModelDynamicsSetAngularVelocity(box[i], 0, 0, 0);
        McdModelDynamicsSetLinearVelocity(box[i], 0, 0, 0);
        McdModelDynamicsEnable(box[i]);
    }

    McdModelDynamicsSetPosition(ramp, rampStart[0], rampStart[1], rampStart[2]);
    McdModelDynamicsSetQuaternion(ramp, 1, 0, 0, 0);
    McdModelDynamicsSetAngularVelocity(ramp, 0, 0, 0);
    McdModelDynamicsSetLinearVelocity(ramp, 0, 0, 0);
    McdModelDynamicsEnable(ramp);

    hingeAngle = 0;

    if(hinge)
    {
        MdtLimitResetState(MdtHingeGetLimit(hinge));
    }
}

void MEAPI_CDECL cleanup(void)
{
    int i;

    MeAppDestroy(meapp);

    McdModelDestroy(plane);
    McdGeometryDestroy(planeGeom);

    for(i=0;i<NUMBOXES;i++)
        McdModelDestroy(box[i]);
    McdGeometryDestroy(boxGeom);

    MstUniverseDestroy(universe);
    RMenuDestroy(menu);
    RRenderContextDestroy(rc);
}

void MEAPI toggleFrictionModel(MeBool on)
{
    MdtContactParamsID params = MstBridgeGetContactParams(bridge, 
        MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    if(on)
        MdtContactParamsSetFrictionModel(params, MdtFrictionModelNormalForce);
    else
        MdtContactParamsSetFrictionModel(params, MdtFrictionModelBox);
}

void MEAPI toggleDrawing(MeBool on)
{
    if(on)
        MdtWorldSetDebugDrawing(world, MdtDebugDrawContactForce | MdtDebugDrawContacts);
    else
        MdtWorldSetDebugDrawing(world, 0);
}

/* Main Routine */

int MEAPI_CDECL main(int argc, const char * argv[])
{
    int i;
    float color[4];
    MstUniverseSizes sizes;
    MdtContactParamsID p;
    MeCommandLineOptions* options;

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
    sizes.dynamicConstraintsMaxCount = 600;
    sizes.materialsMaxCount = 2;
    sizes.collisionModelsMaxCount = 100;
    sizes.collisionPairsMaxCount = 600;
    sizes.collisionUserGeometryTypesMaxCount = 0;
    sizes.collisionGeometryInstancesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);
    space = MstUniverseGetSpace(universe);
    world = MstUniverseGetWorld(universe);
    bridge = MstUniverseGetBridge(universe);
    framework = MstUniverseGetFramework(universe);

    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
    MdtWorldSetAutoDisable(world, 0);
    MdtWorldSetGamma(world,0.1);

    MeDebugDrawAPI.line = simpleDraw;
    
    toggleDrawing(DEFAULT_DRAW);

    /* GROUND PLANE */
    planeGeom = McdPlaneCreate(framework);
    plane = MstFixedModelCreate(universe,planeGeom,groundTM);

    /* Set up contact parameters for collisions between boxes */
    p = MstBridgeGetContactParams(bridge,
            MstBridgeGetDefaultMaterial(), 
            MstBridgeGetDefaultMaterial());

    MdtContactParamsSetType(p, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(p, FRICTION_FORCE);
    MdtContactParamsSetFrictionCoeffecient(p, FRICTION_COEFF);

    toggleFrictionModel(DEFAULT_MODEL);

    boxGeom = McdBoxCreate(framework,boxSize[0], boxSize[1], boxSize[2]);

    for(i=0; i<NUMBOXES; i++)
    {
        box[i] = MstModelAndBodyCreate(universe, boxGeom, 1.0f);    
        boxG[i] = RGraphicBoxCreate(rc, boxSize[0], boxSize[1], boxSize[2],
            boxColor[i%2], McdModelGetTransformPtr(box[i]));
        RGraphicSetWireframe(boxG[i], 1);
    }

    rampGeom = McdBoxCreate(framework,rampSize[0], rampSize[1], rampSize[2]);
    ramp = MstModelAndBodyCreate(universe, rampGeom, 1.0f);

    rampG = RGraphicBoxCreate(rc, rampSize[0], rampSize[1], rampSize[2],
        rampColor, McdModelGetTransformPtr(ramp));

    /* Everything in its right place */
    reset(rc, 0);

    hinge = MdtHingeCreate(world);
    MdtHingeSetBodies(hinge, McdModelGetBody(ramp), 0);
    MdtHingeSetPosition(hinge, rampStart[0], rampStart[1], rampStart[2]);
    MdtHingeSetAxis(hinge, 0, 0, 1);
    MdtHingeEnable(hinge);

    /* ***************** END PHYSICS ***************** */



     RPerformanceBarCreate(rc);

    /* Make the graphics */
    color[0] = color[1] = color[2] = color[3] = 1;
    groundG = RGraphicGroundPlaneCreate(rc, 30.0f,30, color, -1);
    RGraphicSetTexture(rc, groundG, "checkerboard");

    meapp = MeAppCreate(world, space, rc);

    //RRenderSkydomeCreate(rc, "skydome", 2, 1);

    /* CONTROLS: */
    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetUpCallBack(rc, increaseTilt, 0);
    RRenderSetDownCallBack(rc, decreaseTilt, 0);


    RRenderSetWindowTitle(rc, "Friction example");
    RRenderCreateUserHelp(rc,help,HELPSIZE);
    RRenderToggleUserHelp(rc);

    RCameraSetView(rc, 5, 0, ME_PI/8);

    {
        MeVector3 lookAt;
        MeVector3Copy(lookAt, rampStart);
        lookAt[1] += 1;
        RCameraSetLookAt(rc, lookAt);
    }

    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);

    menu = RMenuCreate(rc, "Options Menu");
    RMenuAddToggleEntry(menu, "MdtFrictionModelNormalForce", 
        toggleFrictionModel, DEFAULT_MODEL);
    RMenuAddToggleEntry(menu, "Draw Contacts", toggleDrawing, DEFAULT_DRAW);

    RRenderSetDefaultMenu(rc, menu);

    /* Cleanup after simulation. */
    atexit(cleanup);

    /* Run the Simulation. */
    RRun(rc, tick,0);

    return 0;
}
