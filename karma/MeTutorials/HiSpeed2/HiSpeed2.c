/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:34 $ - Revision: $Revision: 1.30.2.1 $

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
#include <Mst.h>
#include <MeViewer.h>

#define nbWall 5
#define NBALLS1 12
#define NBALLS2 5

/* Global declarations */
MeReal ballDensity = 5;
MeReal ballRadius = 0.6;
int    NBALLS = 12;
int    nSubStep = 4;
MeReal step = (MeReal)(0.02);
const MeReal subStep = 0.005;


MeVector3 wallSize = {0.2, 22, 22};

/* Simulation container. */
MstUniverseID universe;
McdSpaceID space;
MstBridgeID bridge;
MdtWorldID world;


/* Physics representations */
typedef McdModelID MstObjectID;

MstObjectID ball[NBALLS1],   wall[nbWall];
RGraphic   *ballG[NBALLS1], *wallG[nbWall];

/* Collision reps */
McdGeometryID ball_prim, box_prim;

MeMatrix4 wallTM[nbWall];

MeReal gravity[3] = { 0, -10, 0 };

/* Render context */
RRender *rc;

MeBool autoEvolve = 0;
MeBool exampleModel = 1;
MeBool doSafeTimeCheck = 0;
MeReal fireVelocity = 200;
MeVector3 initialPosition = {5, 5, 0};

typedef struct Obj
{
     int    index;
     MeReal time;
}Obj;

int compareTwo(const void *a, const void *b)
{
     Obj *oa = (Obj *) a;
     Obj *ob = (Obj *) b;

     if (oa->time < ob->time)
     return -1;

     if (oa->time > ob->time)
     return 1;

     return 0;
}


void sortIndexes(int n, int *index, MeReal *x)
{
     int i;

     Obj *obj = (Obj *) alloca(n * sizeof (Obj));

     for (i = 0; i < n; i++)
     {
         obj[i].index = index[i];
         obj[i].time  = x[i];
     }

     qsort(obj, n, sizeof (Obj), &compareTwo);

     for (i = 0; i < n; i++) index[i] = obj[i].index;
 }


void toggleAutoEvolve(void)
{
    autoEvolve = !autoEvolve;
}

void toggleChangeExampleModel(void)
{
    exampleModel = !exampleModel;
    if(exampleModel) NBALLS = NBALLS1;
    else             NBALLS = NBALLS2;
}

void toggleSafeTimeCheck(void)
{
    doSafeTimeCheck = !doSafeTimeCheck;
    if(doSafeTimeCheck) printf("Do SafeTime Check ON \n");
    else                printf("Do SafeTime Check OFF\n");
}

int getContactAndSafeTimeToSetBodies(McdSpaceID s, MdtWorldID w, MstBridgeID b, MeReal timeStep)
{
    int i;
    MeBool pairOverflow;
    McdSpacePairIterator spaceIter;
    McdSafeTimeResult    safeTimeResult;
    MeBool               haveSafeTime = 0;

    /* Initialise iterator for this space. */
    McdSpacePairIteratorBegin(s, &spaceIter);
    McdSpaceEndChanges(s);

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

                /* Check safe time. */
                MeVector3 zero = {0,0,0};

                if( !pair ) continue;
                if( (!pair->model1) || (!pair->model2) ) continue;

                /* Check safe time. */
                if( !pair->model1->linearVelocity) pair->model1->linearVelocity = zero;
                if( !pair->model2->linearVelocity) pair->model2->linearVelocity = zero;
                if( !pair->model1->angularVelocity) pair->model1->angularVelocity = zero;
                if( !pair->model2->angularVelocity) pair->model2->angularVelocity = zero;

                McdSafeTime(pair, timeStep, &safeTimeResult);

                /* If we are in danger of passing through,
                   and are not currently touching, reduce time step. */
                if(0 < safeTimeResult.time && safeTimeResult.time < timeStep && !pair->responseData)
                {
                    if(body1)
                    {
                       if( (body1->safeTime > 0 && body1->safeTime > safeTimeResult.time) ||
                            body1->safeTime == MEINFINITY || body1->safeTime < 0)
                       {
                            body1->safeTime = safeTimeResult.time;
                       }
                    }

                    if(body2)
                    {
                        if( (body2->safeTime > 0 && body2->safeTime > safeTimeResult.time) ||
                             body2->safeTime == MEINFINITY || body2->safeTime < 0)
                        {
                             body2->safeTime = safeTimeResult.time;
                        }
                    }

                    haveSafeTime = 1;
                }

            }
        }

    }
    while(pairOverflow);

    return haveSafeTime;
}


int  stepTimeToIsland( MdtWorld *const w )
{
    int i, startSubStep = 0;
    MdtKeaParameters params;
    MdtBodyID body;
    int         index[NBALLS1];
    MeReal safeTimeIs[NBALLS1];
    MeReal stepTo;
    MeDictNode *node;

    /* call the partitioner */
    if(w->autoDisable)
    {
        MdtUpdatePartitions(&w->enabledBodyDict, &w->partitionOutput,
                            MdtAutoDisableLastPartition, &w->partitionParams);
    }
    else
    {
        MdtUpdatePartitions(&w->enabledBodyDict, &w->partitionOutput, 0, 0);
    }

    for( i = 0; i < w->partitionOutput.nPartitions; i++)
    {
        safeTimeIs[i] = MeMIN(step, MdtPartitionGetSafeTime(&w->partitionOutput, i));
        index[i] = i;
    }

    sortIndexes(w->partitionOutput.nPartitions, index, safeTimeIs);

    for ( i = 0; i < nSubStep; i++)
    {
        if(safeTimeIs[index[0]] > subStep*i && safeTimeIs[index[0]] < subStep*(i+1))
        {
           if( startSubStep < i ) startSubStep = i;
           break;
        }
    }

    stepTo = subStep*(startSubStep + 1);

    /* Step Kea with all partition of bodies and constraints. */
    for(i = 0; i < w->partitionOutput.nPartitions; i++)
    {

        int j = index[i];
        MeReal steplocal = safeTimeIs[j] > 0 ? safeTimeIs[j]: step;

        int maxRows;

        if( steplocal > stepTo)
        {
            steplocal = stepTo;
        }

        {

            maxRows = MdtPackPartition(&w->partitionOutput, j, steplocal, &w->params,
                                        w->keabodyArray, w->keatmArray, &w->constraints);


            MdtMakeKeaParameters(w, maxRows, steplocal, &params);


            MdtKeaAddConstraintForces(w->constraints, w->keabodyArray, w->keatmArray,
                                      w->partitionOutput.bodiesSize[j], params);

        }

        MdtKeaIntegrateSystem(w->keabodyArray, w->keatmArray, w->partitionOutput.bodiesSize[j], params);

        MeChunkPutMem(&(w->keaPool), params.memory_pool);

        MdtUnpackPartition(w->keabodyArray,    w->keatmArray,
                          &w->constraints, j, &w->partitionOutput);
    }

    /* Finally, reset forces on enabled bodies. */

    for(node = MeDictFirst(&w->enabledBodyDict); node; node = MeDictNext(&w->enabledBodyDict,node))
    {
        body = MeDictNodeGet(node);
        MEASSERT(body->enabledTime > -1);
        MdtBodyResetForces(body);
        body->safeTime -=  stepTo;
        if(body->safeTime < 0 ) body->safeTime = MEINFINITY;
    }

    return startSubStep;
}

void stepTime(MdtWorldID w, MeReal steptime)
{
    int i;
    MdtKeaParameters params;
    MdtBodyID body;
    MeDictNode *node;

    /* call the partitioner */
    if(w->autoDisable)
    {
        MdtUpdatePartitions(&w->enabledBodyDict, &w->partitionOutput,
                            MdtAutoDisableLastPartition, &w->partitionParams);
    }
    else
    {
        MdtUpdatePartitions(&w->enabledBodyDict, &w->partitionOutput, 0, 0);
    }

    /* Step Kea with all partition of bodies and constraints. */
    for(i = 0; i < w->partitionOutput.nPartitions; i++)
    {

        int maxRows = MdtPackPartition(&w->partitionOutput, i, steptime, &w->params,
                                        w->keabodyArray, w->keatmArray, &w->constraints);

        MdtMakeKeaParameters(w, maxRows, steptime, &params);

        MdtKeaAddConstraintForces(w->constraints, w->keabodyArray, w->keatmArray,
                                  w->partitionOutput.bodiesSize[i], params);

        MdtKeaIntegrateSystem(w->keabodyArray, w->keatmArray,  w->partitionOutput.bodiesSize[i],
                              params);

        MeChunkPutMem(&(w->keaPool), params.memory_pool);        

        MdtUnpackPartition(w->keabodyArray,    w->keatmArray,
                          &w->constraints, i, &w->partitionOutput);
    }

    /* Finally, reset forces on enabled bodies. */

    for(node = MeDictFirst(&w->enabledBodyDict); node; node = MeDictNext(&w->enabledBodyDict,node))
    {
        body = (MdtBodyID)MeDictNodeGet(node);
        MEASSERT(body->enabledTime > -1);
        MdtBodyResetForces(body);
        body->safeTime = MEINFINITY;
    }
}

void stepEvolve(void)
{
    int i;

    int haveSafeTime = 0;
    int startSubStep = 0;
    MeReal timestep = step;

    McdSpaceID  s = MstUniverseGetSpace(universe);
    MdtWorldID  w = MstUniverseGetWorld(universe);
    MstBridgeID b = MstUniverseGetBridge(universe);

    i = 0;

    McdSpacePathUpdateAll(s, step);
    haveSafeTime = getContactAndSafeTimeToSetBodies(s, w, b, step);
    McdSpaceBeginChanges(s);
    do
    {
        if( haveSafeTime )
        {
            MstBridgeUpdateTransitions(b,s,w);
            MstBridgeUpdateContacts(b,s,w);
            startSubStep = stepTimeToIsland(w);
            i += startSubStep;
            timestep -= (startSubStep + 1)*subStep;
            if( timestep <= 0 ) return;
        }
        else
        {
            stepTime(w,timestep);
            return;
        }
        McdSpacePathUpdateAll(s, timestep);
        haveSafeTime = getContactAndSafeTimeToSetBodies(s, w, b, timestep);
        McdSpaceBeginChanges(s);

    }
    while(i++ < nSubStep);
}


/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds.
*/
void tick(RRender * rc)
{
    if (autoEvolve) stepEvolve();
}

void MEAPI_CDECL cleanup(void)
{
    int i;
    McdGeometryDestroy(box_prim);
    for(i = 0; i < nbWall; i++)
    {
        McdModelDestroy(wall[i]);
    }

    McdGeometryDestroy(ball_prim);

    for(i=0; i<NBALLS1; i++)
         MstModelAndBodyDestroy(ball[i]);

    MstUniverseDestroy(universe);

    /* free rendering */
    RDeleteRenderContext(rc);
}

void reset(void)
{
    int i;

    for(i=0; i<NBALLS; i++)
    {
        McdModelDynamicsReset(ball[i]);

        if(exampleModel == 1)
        {

            McdModelDynamicsSetPosition(ball[i],
            initialPosition[0]+MeRealRandomInRange(-3,3),
            initialPosition[1]+MeRealRandomInRange(-3,3),
            initialPosition[2]+MeRealRandomInRange(-3,3));

            McdModelDynamicsSetLinearVelocity(ball[i],
            -fireVelocity + 50*MeRealRandomInRange(-1,1),
            0+5*MeRealRandomInRange(-3,3),
            0+5*MeRealRandomInRange(-3,3));
        }
        else
        {
            McdModelDynamicsSetPosition(ball[i],initialPosition[0]-i*2,
                                      initialPosition[1],
                                      initialPosition[2]);
            McdModelDynamicsSetLinearVelocity(ball[i], -fireVelocity - 50*i, 0, 0);
        }

    }
}

/* Main Routine */
int MEAPI_CDECL main(int argc, const char **argv)
{
    float color[4];
    int i, j;
    MstUniverseSizes sizes;
    MdtContactParamsID params;
    MeMatrix3 R;

    static char *help[] =
    {
        "Left Mouse Drag - move camera",
        "          Space - reset and fire!",
        "              a - toggle auto-evolve",
        "              s - step evolve",
        "              t - toggle SafeTime check",
        "              c - toggle configuration"
    };
    const int helpNum = sizeof (help) / sizeof (help[0]);
    MeCommandLineOptions* options = MeCommandLineOptionsCreate(argc, argv);

    /* Initialise simulation. */
    sizes = MstUniverseDefaultSizes;
    sizes.dynamicBodiesMaxCount = 80;
    sizes.dynamicConstraintsMaxCount = 200;
    sizes.materialsMaxCount = 1;
    sizes.collisionModelsMaxCount = 80;
    sizes.collisionPairsMaxCount = 450;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.collisionUserGeometryTypesMaxCount = 0;


    universe = MstUniverseCreate(&sizes);
    space = MstUniverseGetSpace(universe);
    world = MstUniverseGetWorld(universe);
    bridge = MstUniverseGetBridge(universe);


    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    MdtWorldSetAutoDisable(world, 0);

    box_prim = McdBoxCreate(wallSize[0], wallSize[1], wallSize[2]);

    MeMatrix4TMMakeIdentity(wallTM[0]);
    MeMatrix4TMSetPosition(wallTM[0], -10, 5, 0);

    MeMatrix4TMMakeIdentity(wallTM[1]);
    MeMatrix4TMSetPosition(wallTM[1], 10, 5, 0);

    MeMatrix3MakeRotationZ(R, PI/2);
    MeMatrix4TMMakeIdentity(wallTM[2]);
    MeMatrix4TMMakeFromRotationAndPosition(wallTM[2], R, 0, -5, 0);

    MeMatrix3MakeRotationZ(R, 3*PI/2);
    MeMatrix4TMMakeIdentity(wallTM[4]);
    MeMatrix4TMMakeFromRotationAndPosition(wallTM[4], R, 0, 15, 0);

    MeMatrix3MakeRotationY(R, PI/2);
    MeMatrix4TMMakeIdentity(wallTM[3]);
    MeMatrix4TMMakeFromRotationAndPosition(wallTM[3], R, 0, 5, 10);

    for(i = 0; i < nbWall; i++)
    {
        wall[i] = McdModelCreate(box_prim);
        McdSpaceInsertModel(space, wall[i]);
        McdModelSetTransformPtr(wall[i], wallTM[i]);
    }

    McdSpaceUpdateAll(space);

    for( i = 0; i < nbWall; i ++ )
    {
         McdSpaceFreezeModel(wall[i]);
    }

    /* turn off collision interaction between each walls */
    for ( i = 0; i < nbWall; i++ )
    {
        for ( j = i + 1; j < nbWall; j ++ )
        {
            McdSpaceDisablePair(wall[i], wall[j]);
        }
    }

    ball_prim = McdSphereCreate(ballRadius);
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

    params = MstBridgeGetContactParams(bridge,
                 MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsSetRestitution(params, 0.3);

    reset();

    /* Initialise rendering attributes */
    rc = RRenderContextCreate(options, 0, !MEFALSE);

    RCameraSetElevation(rc, -0.2);
    RCameraSetOffset(rc, 25);

    RUpdateCamera();

    RCreateUserHelp(help, helpNum);
    rc->m_title = "SafeTime tutorial";

    color[0] = 0.5;
    color[1] = 0;
    color[2] = 1;
    color[3] = 1;

    for(i=0; i<NBALLS; i++)
    {
        ballG[i] = RCreateSphere(rc, ballRadius, color,
                  (MeReal*)McdModelGetTransformPtr(ball[i]));
    }

    color[0] = 1;
    color[1] = 1;
    color[2] = 1;

    for( i = 0; i < nbWall; i ++ )
    {
        wallG[i] = RCreateCube(rc, wallSize[0], wallSize[1], wallSize[2],
        color, (MeReal*)McdModelGetTransformPtr(wall[i]));
    }

    /* Keys: */

#ifndef PS2
    RUseKey(' ', reset);
    RUseKey('s', stepEvolve);
    RUseKey('a', toggleAutoEvolve);
    RUseKey('t', toggleSafeTimeCheck);
    RUseKey('c', toggleChangeExampleModel);
#else
    RUsePadKey(PADsquare, reset);
#endif

    /* Cleanup after simulation. */
#ifndef PS2
    atexit(cleanup);
#endif
    /* Run the Simulation */

    RRun(rc, tick);

    return 0;
}
