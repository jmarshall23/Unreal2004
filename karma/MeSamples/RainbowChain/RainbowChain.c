/* -*- mode: C; -*- */

/*
Copyright (c) 1997-2002 MathEngine PLC

    $Name: t-stevet-RWSpre-030110 $
  
    Date: $Date: 2002/05/01 13:14:08 $ - Revision: $Revision: 1.44.2.5 $
    
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

#include <Mst.h>
#include <MeMisc.h>
#include <MeViewer.h>
#include <MeMath.h>
#include <MeApp.h>

#define NBALLS 30

/* Render context */
RRender *rc;

/* graphics */
RGraphic *sphereG[NBALLS];
RGraphic *planeG;

MeApp* meapp;

McdFrameworkID framework;
MstUniverseID universe;

McdGeometryID ballGeom;
McdModelID ball[NBALLS];
MdtBSJointID joint[NBALLS-1];

McdGeometryID planeGeom;
McdModelID plane;


MeALIGNDATA(MeMatrix4,groundTM, 16) = 
{
    {1, 0,  0, 0},
    {0, 0, -1, 0},
    {0, 1,  0, 0},
    {0, 0,  0, 1}
};

MeReal step = (MeReal)(0.03);

MeReal ballRadius = 1;
MeReal ballSpacing = (MeReal)0.1;
MeReal yankForce = 8;
MeReal centerStiffness = 10;

char *help[] = {
    "$ACTION2 - reset",
        "$MOUSE - mouse force"
};

/* per timestep simulation function */

//int frame = 0;

void MEAPI tick(RRender * rc, void* userData)
{
    MeAppStep(meapp);
    MstUniverseStep(universe, step);
    //frame++;
}


/* put all the balls in their initial position */

void MEAPI Reset(RRender * rc, void* userData)
{
    int i;
    
    for(i=0; i<NBALLS; i++)
    {
        McdModelDynamicsReset(ball[i]);
        
        McdModelDynamicsSetPosition(ball[i], (i*(MeReal)0.1),
            (i+1) * (2*ballRadius + ballSpacing), 0);
        
        McdModelDynamicsEnable(ball[i]);
    }
    
    McdModelDynamicsSetLinearVelocity(ball[0],
        (MeReal)0.1, (MeReal)0.2, (MeReal)0.1);
}

/* Cleanup and exit */

void MEAPI_CDECL cleanup(void)
{
    int i;
    McdModelDestroy(plane);
    for(i=0;i<NBALLS;i++)
        McdModelDestroy(ball[i]);
    
    McdGeometryDestroy(ballGeom);
    McdGeometryDestroy(planeGeom);
    MstUniverseDestroy(universe);
    MeAppDestroy(meapp);
    RRenderContextDestroy(rc);
}

/* Main Routine */
int MEAPI_CDECL main(int argc, const char *(argv[]))
{
    int i;
    MstUniverseSizes sizes;
    MdtContactParamsID p;
    float color[4] = { 0, 1, 0, 1 };  /* RGB color */
    MeCommandLineOptions* options;

    /* Create the universe */
    
    sizes = MstUniverseDefaultSizes;
    sizes.collisionModelsMaxCount = NBALLS + 1;
    sizes.collisionPairsMaxCount = NBALLS * 16;
    sizes.collisionUserGeometryTypesMaxCount = 0;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.dynamicBodiesMaxCount = NBALLS;
    sizes.dynamicConstraintsMaxCount = NBALLS * 6;
    sizes.materialsMaxCount = 1;
    
    universe = MstUniverseCreate(&sizes);
    framework = MstUniverseGetFramework(universe);
    McdFrameworkSetDefaultContactTolerance(framework, 0.); 
    MdtWorldSetGravity(MstUniverseGetWorld(universe), 0, -10, 0);
    
    /* Create the plane */
    planeGeom = McdPlaneCreate(framework);
    plane = MstFixedModelCreate(universe, planeGeom, groundTM);
    
    /* Create the balls */
    
    ballGeom = McdSphereCreate(framework,ballRadius);
    
    for(i=0; i<NBALLS; i++)
    {
        ball[i] = MstModelAndBodyCreate(universe, ballGeom, (MeReal)(0.1));
        McdModelDynamicsSetDamping(ball[i], (MeReal)0.1, (MeReal)0.1);
    }
    
    /* Put them in the right place to be strung together */
    
    Reset(rc, 0);
    
    /* Join them with ball-and-socket joints */
    
    for(i=0; i<NBALLS-1; i++)
    {
        MeVector3 pos1, pos2, jointpos;
        
        joint[i] = MdtBSJointCreate(MstUniverseGetWorld(universe));
        McdModelDynamicsGetPosition(ball[i], pos1);
        McdModelDynamicsGetPosition(ball[i+1], pos2);
        
        MeVector3Add(jointpos, pos1,  pos2);
        MeVector3Scale(jointpos, 0.5f);
        
        MdtBSJointSetBodies(joint[i], McdModelGetBody(ball[i]), McdModelGetBody(ball[i+1]));
        MdtBSJointSetPosition(joint[i], jointpos[0], jointpos[1], jointpos[2]);
        
        MdtBSJointEnable(joint[i]);
    }
    
    /* Set friction and restitution parameters for all collisions */
    
    p = MstBridgeGetContactParams(MstUniverseGetBridge(universe),
        MstBridgeGetDefaultMaterial(),
        MstBridgeGetDefaultMaterial());
    
    MdtContactParamsSetFriction(p, (MeReal)5.0);
    MdtContactParamsSetType(p, MdtContactTypeFrictionZero);
    MdtContactParamsSetRestitution(p, (MeReal)0.3);
#ifdef PS2
    MdtContactParamsSetMaxAdhesiveForce(p, MEINFINITY);
#endif
    /* Initialise rendering attributes. */
    
    
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;
    RCameraRotateElevation(rc, (MeReal)1.1);
    RCameraRotateAngle(rc, (MeReal)0.2);
    RCameraZoom(rc, 10);
    RPerformanceBarCreate(rc);
    
    RRenderSetActionNCallBack(rc, 2, Reset, 0);
    
    RRenderSetWindowTitle(rc, "RainbowChain example");
    RRenderCreateUserHelp(rc, help, 2);
    RRenderToggleUserHelp(rc);
    
    /* Create graphics. */
    
    for(i=0; i<NBALLS; i++)
    {
        MeHSVtoRGB(((float)i/(float)NBALLS)*360, (float)0.8, 1, color);
        sphereG[i] = RGraphicSphereCreate(rc, ballRadius, color, McdModelGetTransformPtr(ball[i]));
    }
    
    color[0] = color[1] = color[2] = color[3] = 1;
    planeG = RGraphicGroundPlaneCreate(rc, 30.0f,30, color, 0);
    RGraphicSetTexture(rc, planeG, "checkerboard");
    
    /* Set up MeApp so we get mouse picking */
    
    meapp = MeAppCreateFromUniverse(universe, rc);
    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);
    
#ifndef PS2
    atexit(cleanup);
#endif
    RRun(rc, tick, 0);
    
    return 0;
}
