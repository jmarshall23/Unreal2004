/*
  Copyright (c) 1997-2002 MathEngine PLC
  www.mathengine.com

  $Name: t-stevet-RWSpre-030110 $

  $Id: MeshTest.c,v 1.23.10.1 2002/04/04 15:28:48 richardm Exp $
*/

/*
  Overview:

  This program demonstrates dynamic simulation and collison detection
  of complex shape objects.
*/

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <crtdbg.h>
#endif

#include <Mdt.h>
#include <MeMath.h>
#include <MeApp.h>
#include <McdTriangleMesh.h>
#include <MeViewer.h>
#include <McduTriangleMeshIO.h>
#include "McduMeshSphere.h"

#include <RTriangleMesh.h>

MdtWorldID world;
McdSpaceID space;
MstBridgeID bridge;
MeApp    *meapp;

/*
  Physics representations.
*/

/* dynamics bodies (box) ID */
MdtBodyID box;
MdtBodyID ball;
MdtBodyID teapot;

/*
  Graphics representations.
*/

RGraphic *boxG;
RGraphic *ballG;
RGraphic *teapotG;
RGraphic *planeG;
RRender  *rc = 0;

/*
  Collision representations.
*/

McdModelID boxCM;
McdModelID ballCM;
McdModelID teapotCM;
McdModelID planeCM;

/* Collision geometry ID (box) */
McdGeometryID box_geom = 0;
McdGeometryID ball_geom = 0;
McdGeometryID teapot_geom = 0;
McdGeometryID plane_geom = 0;

int       autoBenchmark = 0;
int       autoEvolve = 1;

MeReal    step = (MeReal) 0.02;
MeReal    gravity[3] = { 0, -9.8f, 0 };

MeReal   *vertexPtr = 0;
int       vertexCount = 0;

#define bdim ((MeReal) 0.45)

void     *shpereData = 0;
void     *boxData = 0;

/*------------------------------------------------------------------------*/

MeMatrix4 tmPlane = {{1, 0, 0, 0},
             {0, 0,-1, 0},
             {0, 1, 0, 0},
             {0, 0, 0, 1}};

char     *help[2] = {
    "$ACTION2 - reset",
    "$ACTION3 - shoot",
};

/**
 *
 */
void
toggleBenchmark(void)
{
    autoBenchmark = !autoBenchmark;
    printf("%i\n", autoBenchmark);
}

void MEAPI
reset(RRender *rc, void *userData)
{
    /* box */
    MdtBodySetPosition(box, -3, 4, -2);
    MdtBodySetLinearVelocity(box, 0, 0, 0);
    MdtBodySetAngularVelocity(box, 0, 0, 0);
    MdtBodySetQuaternion(box, 1, 0, 0, 0);

    /* ball */
    MdtBodySetPosition(ball, -3, 2, -2);
    MdtBodySetLinearVelocity(ball, 0, 0, 0);
    MdtBodySetAngularVelocity(ball, 0, 0, 0);
    MdtBodySetQuaternion(ball, 1, 0, 0, 0);

    /* teapot */
    MdtBodySetPosition(teapot, 3, 3, 0);
    MdtBodySetLinearVelocity(teapot, 0, 0, 0);
    MdtBodySetAngularVelocity(teapot, 0, 0, 0);
    MdtBodySetQuaternion(teapot, 1, 0, 0, 0);
}

int       iShoot = 0;

/**
 *
 */
void MEAPI
Shoot(RRender *rc, void *userData)
{
    int       i;
    MeReal    v[3], p[3], lookat[3];
    MdtBodyID b;

    if ((iShoot % 2) == 0)
    b = box;
    else
    b = ball;

    MdtBodyEnable(b);

    RCameraGetPosition(rc, p);
    RCameraGetLookAt(rc, lookat);

    MdtBodySetPosition(b, p[0], p[1], p[2]);
    for (i = 0; i < 3; i++)
    v[i] = lookat[i] - p[i];

    MeVector3Normalize(v);

    for (i = 0; i < 3; i++)
    v[i] *= 12.0;

    MdtBodySetLinearVelocity(b, v[0], v[1], v[2]);
    MdtBodySetAngularVelocity(b, 1, 1, 1);

    iShoot++;
}

/**
 *
 */
void
stepEvolve()
{
    MeAppStep(meapp);
    MeProfileStartSection("Collision", 0);
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge, space, world);
    MeProfileEndSection("Collision");

    MeProfileStartSection("Dynamics", 0);
    MdtWorldStep(world, step);
    MeProfileEndSection("Dynamics");
}

/**
 *
 */
void
toggleAutoEvolve(void)
{
    autoEvolve = !autoEvolve;
}

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI
tick(RRender *rc, void *useData)
{
    stepEvolve();
}

/**
 *
 */
void
cleanup(void)
{
    /* graphics */

    RRenderContextDestroy(rc);

    /* dynamics */
    MdtWorldDestroy(world);

    McdGeometryDestroy(teapot_geom);
    McdModelDestroy(teapotCM);
    McdGeometryDestroy(box_geom);
    McdModelDestroy(boxCM);
    McdGeometryDestroy(ball_geom);
    McdModelDestroy(ballCM);

    McdGeometryDestroy(plane_geom);
    McdModelDestroy(planeCM);

    MeMemoryAPI.destroy(vertexPtr);
    MeMemoryAPI.destroy(shpereData);
    MeMemoryAPI.destroy(boxData);

    McdSpaceDestroy(space);
    MstBridgeDestroy(bridge);
    McdTerm();
}

/* #define USE_TERRAIN  */
#undef USE_TERRAIN

/**
 *
 */
int
main(int argc, const char **argv)
{
    MeMatrix4Ptr tm;
    MdtContactParamsID params;
    static float color[3][3] = { {1, 1, 0}, {0, 0.4f, 0.6f}, {0, 1, 1} };

    MeVector3 vertext[4] = { {20, 20, 0}, {-20, 20, 0}, {-20, -20, 0}, {20, -20, 0} };
    MeVector3 vertex[8] = {
    {bdim, bdim, -bdim}, {-bdim, bdim, -bdim}, {-bdim, bdim, bdim}, {bdim, bdim, bdim},
    {bdim, -bdim, -bdim}, {-bdim, -bdim, -bdim}, {-bdim, -bdim, bdim}, {bdim, -bdim, bdim}
    };

#ifdef USE_TERRAIN
    MeVector3 planeVertex[800];
    MeReal    xdel, ydel, x0, y0;
    int       i, j, xdim, ydim;
#endif

    MeMatrix4 relTM;
    MeMatrix3 massP;
    MeReal    volume;

    MeCommandLineOptions *options;

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
    return 1;

#if defined WIN32 && defined _DEBUG && 1
    {
        int       debugFlag = 0;
        debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        
        debugFlag |= _CRTDBG_ALLOC_MEM_DF;
        debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
        debugFlag |= _CRTDBG_LEAK_CHECK_DF;
        _CrtSetDbgFlag(debugFlag);
    }
#endif

    /*
       Dynamics.
     */

    /*
       Create and initialize a dynamics world.
     */

    world = MdtWorldCreate(3, 200);
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    MdtWorldSetAutoDisableVelocityThreshold(world, 0.05f);
    MdtWorldSetAutoDisableAngularVelocityThreshold(world, 0.05f);
    /*
       Create physical bodies.
     */

    /* box */
    box = MdtBodyCreate(world);
    MdtBodyEnable(box);
    MdtBodySetMass(box, 0.5f);
    MdtBodySetAngularVelocityDamping(box, 0.1f);
    MdtBodySetLinearVelocityDamping(box, 0.1f);

    /* ball */
    ball = MdtBodyCreate(world);
    MdtBodyEnable(ball);
    MdtBodySetMass(ball, 0.5f);
    MdtBodySetAngularVelocityDamping(ball, 0.1f);
    MdtBodySetLinearVelocityDamping(ball, 0.1f);

    /* teapot */
    teapot = MdtBodyCreate(world);
    MdtBodyEnable(teapot);
    MdtBodySetMass(teapot, 1.0f);
    MdtBodySetAngularVelocityDamping(teapot, 0.6f);
    MdtBodySetLinearVelocityDamping(teapot, 0.3f);

    reset(rc, 0);

    /*
       Collision initialization.
     */

    McdInit(1, 100);
    /* McdIntersectSetContactBufferSize( 350 ); */

    /* McdPrimitivesRegisterTypes(); */
    McdTriangleMeshRegisterType();

    /* McdPrimitivesRegisterInteractions(); */
    McdTriangleMeshTriangleMeshRegisterInteraction();

    /*
       Create a collision space.
     */
    space = McdSpaceAxisSortCreate(McdAllAxes, 50, 100);
    bridge = MstBridgeCreate(10);
    MstSetWorldHandlers(world);

    /*
       Create collision models.
     */

    /* OBJECT I */
#if 0
    // build a sphere - 128 vertices
    ball_geom = makeMeshSphere(1.2f, 3, 1, &shpereData, 0);
#else
    // build a 12 vertex cube
    ball_geom = McdTriangleMeshCreate(12);

    McdTriangleMeshAddTriangle(ball_geom, vertex[0], vertex[1], vertex[2]);
    McdTriangleMeshAddTriangle(ball_geom, vertex[0], vertex[2], vertex[3]);
    McdTriangleMeshAddTriangle(ball_geom, vertex[1], vertex[6], vertex[2]);
    McdTriangleMeshAddTriangle(ball_geom, vertex[1], vertex[5], vertex[6]);
    McdTriangleMeshAddTriangle(ball_geom, vertex[5], vertex[7], vertex[6]);
    McdTriangleMeshAddTriangle(ball_geom, vertex[5], vertex[4], vertex[7]);
    McdTriangleMeshAddTriangle(ball_geom, vertex[4], vertex[3], vertex[7]);
    McdTriangleMeshAddTriangle(ball_geom, vertex[4], vertex[0], vertex[3]);
    McdTriangleMeshAddTriangle(ball_geom, vertex[1], vertex[0], vertex[4]);
    McdTriangleMeshAddTriangle(ball_geom, vertex[1], vertex[4], vertex[5]);
    McdTriangleMeshAddTriangle(ball_geom, vertex[2], vertex[7], vertex[3]);
    McdTriangleMeshAddTriangle(ball_geom, vertex[2], vertex[6], vertex[7]);

    McdTriangleMeshBuild(ball_geom);
#endif

    ballCM = McdModelCreate(ball_geom);
    McdSpaceInsertModel(space, ballCM);
    McdModelSetBody(ballCM, ball);

    printf("** Ball **\nTriangles = %d\n", McdTriangleMeshGetTriangleCount(ball_geom));
    McdTriangleMeshGetMassProperties(ball_geom, relTM, massP, &volume);
    printf("Volume = %7.3f\n", volume);
    printf("Center of Mass: (%5.1f, %5.1f, %5.1f)\nInertia Matrix:\n", relTM[3][0], relTM[3][1],
       relTM[3][2]);
    MeMatrixPrint((MeReal *) massP, 3, 3, "%5.1f");

    /* OBJECT II */
#if 1
    // build a sphere - 128 vertices
    box_geom = makeMeshSphere(gMcdFramework, 0.8f, 3, 1, &boxData, 0);
#else
    // build a 12 vertex cube
    box_geom = McdTriangleMeshCreate(12);

    McdTriangleMeshAddTriangle(box_geom, vertex[0], vertex[1], vertex[2]);
    McdTriangleMeshAddTriangle(box_geom, vertex[0], vertex[2], vertex[3]);
    McdTriangleMeshAddTriangle(box_geom, vertex[1], vertex[6], vertex[2]);
    McdTriangleMeshAddTriangle(box_geom, vertex[1], vertex[5], vertex[6]);
    McdTriangleMeshAddTriangle(box_geom, vertex[5], vertex[7], vertex[6]);
    McdTriangleMeshAddTriangle(box_geom, vertex[5], vertex[4], vertex[7]);
    McdTriangleMeshAddTriangle(box_geom, vertex[4], vertex[3], vertex[7]);
    McdTriangleMeshAddTriangle(box_geom, vertex[4], vertex[0], vertex[3]);

    McdTriangleMeshAddTriangle(box_geom, vertex[1], vertex[0], vertex[4]);
    McdTriangleMeshAddTriangle(box_geom, vertex[1], vertex[4], vertex[5]);

    McdTriangleMeshAddTriangle(box_geom, vertex[2], vertex[7], vertex[3]);
    McdTriangleMeshAddTriangle(box_geom, vertex[2], vertex[6], vertex[7]);

    McdTriangleMeshBuild(box_geom);
#endif

    boxCM = McdModelCreate(box_geom);
    McdSpaceInsertModel(space, boxCM);
    McdModelSetBody(boxCM, box);

    printf("** Box **\nTriangles = %d\n", McdTriangleMeshGetTriangleCount(box_geom));
    McdTriangleMeshGetMassProperties(box_geom, relTM, massP, &volume);
    printf("Volume = %7.3f\n", volume);
    printf("Center of Mass: (%5.1f, %5.1f, %5.1f)\nInertia Matrix:\n", relTM[3][0], relTM[3][1],
       relTM[3][2]);
    MeMatrixPrint((MeReal *) massP, 3, 3, "%5.1f");

    /* teapot */
    teapot_geom =
    McduTriangleMeshCreateFromObj("../Resources/teapot566.obj", 1, 0, 0, 0.0, &vertexPtr,
  &vertexCount, 0);
    McdTriangleMeshBuild(teapot_geom);
    teapotCM = McdModelCreate(teapot_geom);
    McdSpaceInsertModel(space, teapotCM);
    McdModelSetBody(teapotCM, teapot);
    /* McduTriangleMeshWriteToObj("sgi.obj", teapot_geom, 0, 1e-6f); */

    printf("** TeaPot **\nTriangles = %d\n", McdTriangleMeshGetTriangleCount(teapot_geom));
    McdTriangleMeshGetMassProperties(teapot_geom, relTM, massP, &volume);
    printf("Volume = %7.3f\n", volume);
    printf("Center of Mass: (%5.1f, %5.1f, %5.1f)\nInertia Matrix:\n", relTM[3][0], relTM[3][1],
       relTM[3][2]);
    MeMatrixPrint((MeReal *) massP, 3, 3, "%5.1f");

    /* plane */
    plane_geom = McdTriangleMeshCreate(2);

#ifdef USE_TERRAIN
    xdim = 40;
    ydim = 20;
    xdel = 1;
    ydel = 2;
    x0 = -(xdim * xdel) * 0.5;
    y0 = -(ydim * ydel) * 0.5;

    for (j = 0; j < ydim; j++) {
    for (i = 0; i < xdim; i++) {
        planeVertex[i + j * xdim][0] = x0 + i * xdel;
        planeVertex[i + j * xdim][1] = y0 + j * ydel;
        planeVertex[i + j * xdim][2] = 0.5 * sin(i * 0.5);
    }
    }

    for (j = 0; j < ydim - 1; j++) {
    for (i = 0; i < xdim - 1; i++) {
        McdTriangleMeshAddTriangle(plane_geom, planeVertex[j * xdim + i],
                       planeVertex[j * xdim + i + 1],
                       planeVertex[(j + 1) * xdim + i + 1]);
        McdTriangleMeshAddTriangle(plane_geom, planeVertex[j * xdim + i],
                       planeVertex[(j + 1) * xdim + i + 1],
                       planeVertex[(j + 1) * xdim + i]);
    }
    }
#else

    McdTriangleMeshAddTriangle(plane_geom, vertext[0], vertext[1], vertext[2]);
    McdTriangleMeshAddTriangle(plane_geom, vertext[0], vertext[2], vertext[3]);
#endif

    McdTriangleMeshBuild(plane_geom);

    planeCM = McdModelCreate(plane_geom);
    McdModelSetTransformPtr(planeCM, tmPlane);
    McdSpaceInsertModel(space, planeCM);

    McdSpaceBuild(space);

    /* set number of contacts per pair */

    McdGetDefaultRequestPtr()->contactMaxCount = 6;

    /*
       Set parameters for contacts.
     */
    params = MstBridgeGetContactParams(bridge, 0, 0);

    MdtContactParamsSetType(params, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(params, 0.50f);
    MdtContactParamsSetRestitution(params, (MeReal) 0.3);

    /* ball */
    tm = MdtBodyGetTransformPtr(ball);
    ballG = RGraphicTriangleMeshCreate(rc, ball_geom, color[0], tm);

    /* box */
    tm = MdtBodyGetTransformPtr(box);
    boxG = RGraphicTriangleMeshCreate(rc, box_geom, color[0], tm);

    /* teapot */
    tm = MdtBodyGetTransformPtr(teapot);
    teapotG = RGraphicTriangleMeshCreate(rc, teapot_geom, color[1], tm);

    /* plane */
    planeG = RGraphicTriangleMeshCreate(rc, plane_geom, color[2], tmPlane);

    /*
       Set up camera.
     */
    RCameraRotateAngle(rc, 0);
    RCameraRotateElevation(rc, 0.5f);
    RCameraZoom(rc, 1.0f);

    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, Shoot, 0);

    RRenderCreateUserHelp(rc, help, 2);

    RRenderToggleUserHelp(rc);
    RPerformanceBarCreate(rc);

    RRenderSetWindowTitle(rc, "MeshMesh");

    meapp = MeAppCreate(world, space, rc);

    /*
       Cleanup after simulation.
     */
    atexit(cleanup);

    /*
       Run the Simulation.
     */
    RRun(rc, tick, 0);

    return 0;
}
