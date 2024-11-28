/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:36 $ - Revision: $Revision: 1.9.2.3 $

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
char *help[2] =
{
    "$ACTION2 - reset",
    "$MOUSE - mouse force"
};


/* World for the Dynamics Toolkit simulation */
MstUniverseID   universe;
McdSpaceID      space;
MdtWorldID      world;
MstBridgeID     bridge;
McdFrameworkID  frame;

/* Physics representations */
McdGeometryID   planeGeom;
McdModelID      plane;

#define NBOXES  10
MeVector3       boxSize = {2, 1, 2};
MeVector3       boxStart = {0, 1, 0};
MeVector3       boxSpacing = {0, 1.05f, 0}; 

McdGeometryID   boxGeom;
McdModelID      box[NBOXES];
RGraphic*       boxG[NBOXES];

/* Graphical representations */
RGraphic *groundG;

MeReal gravity[3] = { 0, -9.8, 0 };

/* Render context */
RRender *rc;
RMenu* menu;

MeApp* meapp;

MeReal step = (MeReal)(0.03);

MeMatrix4* groundTM;
MeMatrix4 baseGroundTM =
{
    1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, 0,
    0, -1, 0, 1
};

float boxColor[4][4] = {{0, 1, 0, 1},{0, 0, 1, 1},
{1, 0, 0, 1},{1, 1, 0, 1}};

float blue[4] = {0, 0, 1, 1};
float white[4] ={1, 1, 1, 1};

#define NUM_PART_COLORS (11)
float partColors[NUM_PART_COLORS][3] = 
{
    {0, 1, 0},
    {0, 0, 1},
    {1, 0, 0},
    {1, 1, 0},
    {0, 1, 1},
    {1, 0, 1},
    {1, 1, 1},
    {0.5, 0, 0},
    {0, 0, 0.5},
    {0, 0.5, 0},
    {0.5, 0.5, 0}
};

MdtLODParams lodParams;


void MEAPI GetPartitionColor(int pIx, float color[4])
{
    if(pIx == -1)
    {
        color[0] = color[1] = color[2] = (MeReal)0.3;
        color[3] = 1;
    }
    else
    {
        MeVector3Copy(color, partColors[pIx%NUM_PART_COLORS]);
        color[3] = 1;
    }
}

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender * rc,void *userdata)
{
    int i;

    RLineRemoveAll(rc);
    MeAppStep(meapp);
    MstUniverseStep(universe, step);

    for(i=0; i<NBOXES; i++)
    {
        float color[4];
        int pIx = MdtBodyGetPartition(McdModelGetBody(box[i]));
        GetPartitionColor(pIx, color);
        RGraphicSetColor(boxG[i], color);
    }
}

/* Reset boxes and balls to initial positions */
void MEAPI reset(RRender* rc, void* userData)
{
    int i;

    for(i=0; i<NBOXES; i++)
    {
        McdModelDynamicsSetQuaternion(box[i], 1, 0, 0, 0);
        McdModelDynamicsSetAngularVelocity(box[i], 0, 0, 0);
        McdModelDynamicsSetLinearVelocity(box[i], 0, 0, 0);
        McdModelDynamicsSetPosition(box[i], 
            boxStart[0] + i*boxSpacing[0],
            boxStart[1] + i*boxSpacing[1],
            boxStart[2] + i*boxSpacing[2]);
        McdModelDynamicsEnable(box[i]);
    }
}

void MEAPI_CDECL cleanup(void)
{
    MeAppDestroy(meapp);

    McdModelDestroy(plane);
    McdGeometryDestroy(planeGeom);

    MstUniverseDestroy(universe);
    RRenderContextDestroy(rc);
    RMenuDestroy(menu);

    MeMemoryAPI.destroyAligned(groundTM);
}

/* Menu handler */
void MEAPI sizeFunc(MeReal value)
{
    MdtWorldSetMaxMatrixSize(world, (int)value);
}

void MEAPI frFunc(MeReal value)
{
    lodParams.frictionRatio = value;
    MdtWorldSetLODParams(world, &lodParams);
}

void MEAPI zrbFunc(MeReal value)
{
    lodParams.zeroRowBonus = value;
    MdtWorldSetLODParams(world, &lodParams);
}

void MEAPI twbFunc(MeReal value)
{
    lodParams.toWorldBonus = value;
    MdtWorldSetLODParams(world, &lodParams);
}

void MEAPI pbFunc(MeReal value)
{
    lodParams.penetrationBias = value;
    MdtWorldSetLODParams(world, &lodParams);
}

void MEAPI nvbFunc(MeReal value)
{
    lodParams.normVelBias = value;
    MdtWorldSetLODParams(world, &lodParams);
}

void MEAPI rcbFunc(MeReal value)
{
    lodParams.rowCountBias = value;
    MdtWorldSetLODParams(world, &lodParams);
}

/* Line drawer. */
void MEAPI LineDraw(MeVector3 start, MeVector3 end, MeReal r, MeReal g, MeReal b)
{
    RLineAdd(rc, start, end, r, g, b);
}

/* Main Routine */

int MEAPI_CDECL main(int argc, const char * argv[])
{
    MstUniverseSizes sizes;
    MdtContactParamsID p;
    MeCommandLineOptions* options;
    int i;

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

    universe = MstUniverseCreate(&sizes);
    space = MstUniverseGetSpace(universe);
    world = MstUniverseGetWorld(universe);
    bridge = MstUniverseGetBridge(universe);
    frame = MstUniverseGetFramework(universe);

    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    /* For this example, dont turn things off. */
    MdtWorldSetAutoDisable(world, 0);

    MeDebugDrawAPI.line = LineDraw;
    MdtWorldSetDebugDrawing(world, MdtDebugDrawContacts);


    /* GROUND PLANE */
    planeGeom = McdPlaneCreate(frame);
    groundTM = (MeMatrix4*)MeMemoryAPI.createAligned(sizeof(MeMatrix4), 16);
    MeMatrix4Copy(*groundTM, baseGroundTM);
    plane = MstFixedModelCreate(universe, planeGeom, *groundTM);

    /* Set up contact parameters for collisions between boxes */
    p = MstBridgeGetContactParams(bridge,
            MstBridgeGetDefaultMaterial(), 
            MstBridgeGetDefaultMaterial());

    MdtContactParamsSetType(p, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(p, 5.0f);

    boxGeom = McdBoxCreate(frame, boxSize[0], boxSize[1], boxSize[2]);

    for(i=0; i<NBOXES; i++)
    {
        float color[4];
        int k;
        MeReal blend = (MeReal)i/(NBOXES-1);

        for(k=0; k<3; k++)
        {
            color[k] = boxColor[2*(i%2)+0][k] * blend
                + boxColor[2*(i%2)+1][k] * (1-blend);
        }
        color[4] = 1;

        
        box[i] = MstModelAndBodyCreate(universe, boxGeom, 1);
        McdModelDynamicsSetDamping(box[i], 0.1f, 0.1f);
        boxG[i] = RGraphicBoxCreate(rc, boxSize[0], boxSize[1], boxSize[2], 
            color, McdModelGetTransformPtr(box[i]));
    }

    reset(rc, 0);
    /* ***************** END PHYSICS ***************** */

    RPerformanceBarCreate(rc);

    groundG = RGraphicGroundPlaneCreate(rc, 30.0f,30, white, -1);
    RGraphicSetTexture(rc, groundG, "checkerboard");

    meapp = MeAppCreate(world, space, rc);

    /* CONTROLS: */
    RRenderSetActionNCallBack(rc, 2, reset, 0);

    RRenderSetWindowTitle(rc, "Empty example");
    RRenderCreateUserHelp(rc,help,2);
    RRenderToggleUserHelp(rc);

    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);

    {
        int sizeNeeded = NBOXES * 12;
        
        MdtWorldGetLODParams(world, &lodParams);

        menu = RMenuCreate(rc, "Options Menu");

        RMenuAddValueEntry(menu, "Max Matrix Size",
            sizeFunc, 300, 12, 4, (MeReal)sizeNeeded);
        
        RMenuAddValueEntry(menu, "Friction Ratio",
            frFunc, 1, 0, (MeReal)0.1, lodParams.frictionRatio);

        RMenuAddValueEntry(menu, "To World Bonus",
            twbFunc, 100, 0, 10, lodParams.toWorldBonus);

        RMenuAddValueEntry(menu, "Zero Row Bonus",
            zrbFunc, 100, 0, 10, lodParams.zeroRowBonus);

        RMenuAddValueEntry(menu, "Row Count Bias",
            rcbFunc, 100, 0, 10, lodParams.rowCountBias);
        
        RMenuAddValueEntry(menu, "Penetration Bias",
            pbFunc, 100, 0, 10, lodParams.penetrationBias);
        
        RMenuAddValueEntry(menu, "Norm Vel Bias",
            nvbFunc, 10, 0, 1, lodParams.normVelBias);
        

        MdtWorldSetMaxMatrixSize(world, sizeNeeded);
    }
    
    RRenderSetDefaultMenu(rc, menu);
    RMenuDisplay(menu);

    RCameraSetView(rc, 20, ME_PI/6, ME_PI/6);

    /* Cleanup after simulation. */
    atexit(cleanup);

    /* Run the Simulation. */
    RRun(rc, tick,0);

    return 0;
}
