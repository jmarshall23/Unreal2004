/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   $Date: 2002/04/04 15:29:34 $ $Revision: 1.41.4.3 $

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
    This demo shows a simple example of using the 'SafeTime' functionality to
    prevent missing collisions between fast moving objects. We simply evolve
    each ball by the maximum we can to avoid passing through the box. Of
    course, this means the ball moves less distance during the frame than it
    should, but for this simple example this is fine.

    More complicated schemes can be devised, depending on the application.
 */

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <MeMath.h>
#include <Mst.h>
#include <MeViewer.h>

#define NBALLS 12

/* Global declarations */
MeReal ballDensity = 20;
MeReal ballRadius = 0.2;

MeVector3 wallSize = {1, 15, 15};
MeVector3 zero = {0,0,0};

/* Simulation container. */
MstUniverseID universe;


/* Physics representations */
McdModelID ball[NBALLS], wall[2];

RGraphic *ballG[NBALLS], *wallG[2];

/* Collision reps */

McdFrameworkID framework;
McdGeometryID ball_prim, box_prim;

MeALIGNDATA(MeMatrix4,wallTM0,16);
MeALIGNDATA(MeMatrix4,wallTM1,16);

MeReal gravity[3] = { 0, -5, 0 };

/* Render context */
RRender *rc;
RMenu *menu;

MeReal step = (MeReal)(0.02);

MeBool autoEvolve = 0;
MeBool doSafeTimeCheck = 0;
MeReal fireVelocity = 100;
MeVector3 initialPosition = {10, 0, 0};


void MEAPI toggleAutoEvolve(MeBool on)
{
    autoEvolve = on;
}

void MEAPI toggleSafeTimeCheck(MeBool on)
{
    doSafeTimeCheck = on;
}

void ConvertContact(MdtBclContactParams* params, McdContact* colC, MdtContactID dynC)
{
  MdtContactSetNormal(dynC, colC->normal[0], colC->normal[1], colC->normal[2]);
  MdtContactSetPosition(dynC, colC->position[0], colC->position[1], colC->position[2]);
  MdtContactSetPenetration(dynC,-(colC->separation));

  MdtContactSetParams(dynC, params);
}

void MEAPI stepEvolve(RRender *rc, void *userData)
{
    int i;
    MeBool pairOverflow;
    McdSpacePairIterator spaceIter;
    McdSafeTimeResult safeTimeResult;

    McdSpaceID s = MstUniverseGetSpace(universe);
    MdtWorldID w = MstUniverseGetWorld(universe);
    MstBridgeID b = MstUniverseGetBridge(universe);

    McdSpacePathUpdateAll(s, step);

    McdSpaceEndChanges(s);

    /* Initialise iterator for this space. */

    McdSpacePairIteratorBegin(s, &spaceIter);

    /* Keep getting pairs from farfield until we're done (ie. no overflow). */
    do
    {
        McdModelPairContainerReset(b->pairs);
        pairOverflow = McdSpaceGetPairs(s, &spaceIter, b->pairs);

        MstHandleTransitions(b->pairs, s, w, b);

        MstHandleCollisions(b->pairs, s, w, b);

        if(doSafeTimeCheck)
        {
            for( i = b->pairs->helloFirst ; i < b->pairs->stayingEnd ; ++i )
            {
                McdModelPair* pair = b->pairs->array[i];
                MdtBodyID body1 = (MdtBodyID)pair->model1->mBody;
                MdtBodyID body2 = (MdtBodyID)pair->model2->mBody;
                
                if(body1 || body2)
                {
                    int contactCount = 0;

                    MdtContactGroupID group = 
                        (MdtContactGroupID)pair->responseData;

                    if(group)
                        contactCount = MdtContactGroupGetCount(group);

                    /* Call the test to find estimated time of 'dangerous' 
                       collision (ie one where objects may miss each other). */
                    McdSafeTime(pair, step, &safeTimeResult);
                    
                    /*  If we are in danger of passing through,
                        and are not currently touching, reduce time step. */
                    if(0 < safeTimeResult.time && 
                       safeTimeResult.time < step && contactCount == 0)
                    {
                        if(body1)
                            MdtBodySetSafeTime(body1, safeTimeResult.time);
                        
                        if(body2)
                            MdtBodySetSafeTime(body2, safeTimeResult.time);
                    }
                }
            }
        }

    }
    while(pairOverflow);

    McdSpaceBeginChanges(s);

    MdtWorldStepSafeTime(w, step);
}

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds.
*/
void MEAPI tick(RRender *rc, void *userData)
{
    if (autoEvolve) stepEvolve(rc,userData);
}

void MEAPI_CDECL cleanup(void)
{
    MstUniverseDestroy(universe);
    /* free rendering */
    RRenderContextDestroy(rc);
    RMenuDestroy(menu);
}

void MEAPI reset(RRender *rc, void *userData)
{
    int i;

    for(i=0; i<NBALLS; i++)
    {
        McdModelDynamicsReset(ball[i]);

        McdModelDynamicsSetPosition(ball[i],
            initialPosition[0]+MeRealRandomInRange(-2,2),
            initialPosition[1]+MeRealRandomInRange(-2,2),
            initialPosition[2]+MeRealRandomInRange(-2,2));

        McdModelDynamicsSetLinearVelocity(ball[i],
            -fireVelocity,
            0+MeRealRandomInRange(-10,10),
            0+MeRealRandomInRange(-10,10));
    }
}

/* Main Routine */
int MEAPI_CDECL main(int argc, const char **argv)
{
    float color[4] = {0,0,0,0};
    int i, j;
    MstUniverseSizes sizes;
    MdtContactParamsID params;
    MeMatrix3 R;
    McdSpaceID s;
    MdtWorldID w;
    MstBridgeID b;
    MeVector3 lookAt={2.2,2.7,6.4};

    static char *help[3] =
    {
        "$ACTION1 - toggle options menu",
        "$ACTION2 - reset and fire!",
        "$ACTION3 - step evolve",
    };
    /* Initialize rendering. */
    MeCommandLineOptions* options;
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;

   /* Initialise simulation. */
    sizes = MstUniverseDefaultSizes;
    sizes.dynamicBodiesMaxCount = 50;
    sizes.dynamicConstraintsMaxCount = 200;
    sizes.materialsMaxCount = 1;
    sizes.collisionModelsMaxCount = 50;
    sizes.collisionPairsMaxCount = 150;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.collisionUserGeometryTypesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);
    s = MstUniverseGetSpace(universe);
    w = MstUniverseGetWorld(universe);
    b = MstUniverseGetBridge(universe);
    framework = MstUniverseGetFramework(universe);

    MdtWorldSetGravity(w, gravity[0], gravity[1], gravity[2]);

    MdtWorldSetAutoDisable(w, 0);

    box_prim = McdBoxCreate(framework,wallSize[0], wallSize[1], wallSize[2]);

    MeMatrix4TMMakeIdentity(wallTM0);
    MeMatrix4TMSetPosition(wallTM0, 0, 4.5f, 0);
    wall[0] = MstFixedModelCreate(universe,box_prim,wallTM0);
    McdModelSetAngularVelocityPtr(wall[0],zero);
    McdModelSetLinearVelocityPtr(wall[0],zero);

    MeMatrix3MakeRotationZ(R, ME_PI/2);
    MeMatrix4TMMakeFromRotationAndPosition(wallTM1,
        R,wallSize[1]/2, -2.5f, 0);
    wall[1] = MstFixedModelCreate(universe,box_prim,wallTM1);
    McdModelSetAngularVelocityPtr(wall[1],zero);
    McdModelSetLinearVelocityPtr(wall[1],zero);

    ball_prim = McdSphereCreate(framework, ballRadius);
    for(i=0; i<NBALLS; i++)
    {
        ball[i] = MstModelAndBodyCreate(universe, ball_prim, ballDensity);
    }

    for(i=0; i<NBALLS-1; i++)
    {
        for(j=i; j<NBALLS; j++)
        {
            McdSpaceDisablePair(ball[i], ball[j]);
        }
    }

    reset(rc,0);

    params = MstBridgeGetContactParams(b,
        MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());
    MdtContactParamsSetRestitution(params, 0.3);

    /* Initialize view, colours etc. */

    color[0] = 0.3;
    color[1] = 0;
    color[2] = 1;

    for(i=0; i<NBALLS; i++)
    {
        ballG[i] = RGraphicSphereCreate(rc, ballRadius, color,
            McdModelGetTransformPtr(ball[i]));
    }

    color[0] = .75;
    color[1] = .75;
    color[2] = .75;

    wallG[0] = RGraphicBoxCreate(rc, wallSize[0], wallSize[1], wallSize[2],
        color, McdModelGetTransformPtr(wall[0]));
    color[2]=0;
    wallG[1] = RGraphicBoxCreate(rc, wallSize[0], wallSize[1], wallSize[2],
        color, McdModelGetTransformPtr(wall[1]));


    /*
      Keyboard callbacks
    */

    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, stepEvolve, 0);

    menu = RMenuCreate(rc, "Options Menu");
    RMenuAddToggleEntry(menu, "SafeTime Check", toggleSafeTimeCheck, 0);
    RMenuAddToggleEntry(menu, "AutoEvolve", toggleAutoEvolve, 0);
    RRenderSetDefaultMenu(rc, menu);

    RMenuDisplay(menu);

    RPerformanceBarCreate(rc);
    RRenderSetWindowTitle(rc, "ConvexPrims tutorial");

    RRenderCreateUserHelp(rc, help, 3);
    RRenderToggleUserHelp(rc);

    RCameraSetView(rc, 25.0f,0.2f,0.3f);
    RCameraUpdate(rc);


    /* Cleanup after simulation. */
#ifndef PS2
    atexit(cleanup);
#endif
    /* Run the Simulation */

    RRun(rc, tick, 0);

    return 0;
}
