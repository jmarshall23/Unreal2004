/*
  Copyright (c) 1997-2002 MathEngine PLC
  www.mathengine.com

  $Name: t-stevet-RWSpre-030110 $

  $Id: MeshSgiLogoSpin.c,v 1.50.2.1 2002/04/04 15:28:49 richardm Exp $
*/

/*
  Overview:

  This program demonstrates dynamic simulation and collison detection
  of complex shape objects.
*/
#ifdef WIN32
#include <crtdbg.h>
#endif

#include <Mdt.h>
#include <MeMath.h>
#include <MeApp.h>
#include <McdTriangleMesh.h>
#include <MeViewer.h>
#include <McduTriangleMeshIO.h>  
#include "../include/McduMeshSphere.h"  
#include <RTriangleMesh.h>


#define Nballs        8
#define NContacts   200

MdtWorldID  world;
McdSpaceID  space;
MstBridgeID bridge;
MeApp*      meapp;
McdFrameworkID frame;

/* dynamics bodies (box) ID */
MdtBodyID ball[Nballs];
MdtBodyID sgiLogo;

MdtHingeID j_logo_ground;

/*
  Graphics representations.
*/

RGraphic* ballG[Nballs];
RGraphic* sgilogoG;
RGraphic* planeG;
RRender*  rc = 0;

/*
  Collision representations.
*/

McdModelID ballCM[Nballs];
McdModelID sgilogoCM;
McdModelID planeCM;

/* Collision geometry ID (box) */
McdGeometryID ball_geom[Nballs];
McdGeometryID sgilogo_geom = 0;
McdGeometryID plane_geom = 0;

int autoBenchmark = 0;

MeReal step = (MeReal) 0.02;
MeReal gravity[3] = { 0, -6, 0 };

MeReal *shpereData[Nballs];
MeReal *vertexPtr;
int vertexCount;

MeMatrix3 logoI;


/*------------------------------------------------------------------------*/

MeMatrix4 tmPlane =
{
    1, 0, 0, 0,
    0, 0, -1, 0,
    0, 1, 0, 0,
    0, -2, 0, 1
};

MeMatrix4 tmSgilogo = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,2,0,1};


char *help[6] =
{
    "$ACTION3 - shoot",
    "$ACTION5 - toggle pause",
    "$ACTION4 - toggle auto shooting",
};



/*-----------------------------------------------------------------------------*/
void SetAxisAngle(MeReal *q, const MeReal nX, const MeReal nY,
                  const MeReal nZ, MeReal angle)
{
    MeReal s_2 = -(MeReal)sin(0.5f*angle);
    q[1] = nX;
    q[2] = nY;
    q[3] = nZ;
    MeVector3Normalize(q+1);
    q[0] = (MeReal)cos(0.5f*angle);
    q[1] *= s_2;
    q[2] *= s_2;
    q[3] *= s_2;
}


/*-----------------------------------------------------------------------------*/
void Rotate(MeMatrix4* m, const MeReal nX, const MeReal nY,
                          const MeReal nZ, MeReal angle)
{
  MeReal q[4];
  SetAxisAngle(q, nX, nY, nZ, angle);
  MeQuaternionToTM( *m, q);
}


/**
 *
 */
void initBalls( MdtBodyID body, int ith )
{
  MeReal r = 6;
  MeReal x, z;
  MeReal y = (MeReal)3.5;

  MdtBodyEnable(body);
  x = r*MeSin(ith*2*ME_PI/Nballs);
  z = r*MeCos(ith*2*ME_PI/Nballs);

  MdtBodySetPosition(body,x,y,z);

  MdtBodySetLinearVelocity(body, -x*1.5f, 0.5f, -z*1.5f);
  MdtBodySetAngularVelocity(body, 0, 0, 0);
}


int sCount = 0;
int autoShooting = 1;
int autoEvolve = 1;
int iSteps = 0;


/**
 *
 */
void MEAPI shoot(RRender* rc, void* userData)
{
  int i;
  for (i=0; i<Nballs; i++) {
    initBalls( ball[i], i );
  }

  sCount = 0;
}

/**
* MeshSgiLogoSpin.c
*/
void stepEvolve()
{
  MeAppStep(meapp);
  MeProfileStartSection("Collision", 0);
  McdSpaceUpdateAll(space);
  MstBridgeUpdateContacts(bridge, space, world);
  MeProfileEndSection("Collision");

  if (iSteps<20)
      MdtBodyAddTorque(sgiLogo, 0, 100-iSteps*5.0f, 0);
    else
      MdtBodyAddTorque(sgiLogo, 0, 2, 0);

  MeProfileStartSection("Dynamics", 0);
  MdtWorldStep(world, step);
  MeProfileEndSection("Dynamics");

  if (sCount%200 == 0 && autoShooting ) shoot(rc, 0);
  sCount++;
  iSteps++;
}



/**
 *
 */
void MEAPI toggleAutoEvolve(RRender* rc, void* userData)
{
    autoEvolve = !autoEvolve;
}

/**
 *
 */
void MEAPI toggleAutoShoot(RRender* rc, void* userData)
{
    autoShooting = !autoShooting;
}

/**
*
*/
void MEAPI tick(RRender* rc, void* useData)
{
  if (autoEvolve) stepEvolve();
}

/**
 *
 */
void cleanup(void)
{
    int i;
    /* graphics */
    RRenderContextDestroy(rc);

    /* dynamics */
    MdtWorldDestroy(world);

    McdGeometryDestroy(sgilogo_geom);
    McdModelDestroy(sgilogoCM);

    for (i=0; i<Nballs; i++) {
      McdGeometryDestroy(ball_geom[i]);
      McdModelDestroy(ballCM[i]);
      MeMemoryAPI.destroy( shpereData[i] );
    }

    McdGeometryDestroy(plane_geom);
    McdModelDestroy(planeCM);

    MeMemoryAPI.destroy( vertexPtr );

    MeAppDestroy(meapp);

    McdSpaceDestroy(space);
    MstBridgeDestroy(bridge);
    McdTerm();
}

/**
 *
 */
int main(int argc, const char **argv)
{
    MeMatrix4Ptr tm;
    MeReal lookat[3];
    int i;
    MdtContactParamsID params;
    MeReal radius = 0.45f;

    static float color[3][3] = { {1, 1, 0}, {0, 0.4f, 0.6f}, {0, 1, 1} };

    MeVector3 vertext[4] = {{20,20,0}, {-20,20,0}, {-20,-20,0}, {20,-20,0}};

    int debugFlag = 0;

    MeCommandLineOptions* options;

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc) return 1;

#if defined WIN32 && defined _DEBUG && 1
    debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

    debugFlag |= _CRTDBG_ALLOC_MEM_DF;
    debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
    debugFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(debugFlag);
#endif

    /*
      Dynamics.
    */

    /*
      Create and initialize a dynamics world.
    */
    world = MdtWorldCreate(Nballs+1, 300);
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

     /*
      Create physical bodies.
    */
    sgiLogo = MdtBodyCreate(world);
    MdtBodyEnable(sgiLogo);
    MdtBodySetPosition(sgiLogo, 0, 2, 0);
    MdtBodySetMass(sgiLogo, 20.0f);
    MdtBodySetAngularVelocityDamping(sgiLogo, 1);
    MdtBodySetLinearVelocityDamping(sgiLogo, 0.1f);

    logoI[0][0] = 40;
    logoI[1][1] = 40;
    logoI[2][2] = 40;
    MdtBodySetInertiaTensor(sgiLogo, logoI);

    j_logo_ground = MdtHingeCreate(world);
    MdtHingeSetBodies(j_logo_ground, sgiLogo, 0);
    MdtHingeSetPosition(j_logo_ground, 0, 0, 0);
    MdtHingeSetAxis(j_logo_ground, 0, 1, 0);
    MdtHingeEnable(j_logo_ground);

    /* ball */

    for (i=0; i<Nballs; i++) {
      ball[i] = MdtBodyCreate(world);
      MdtBodyEnable(ball[i]);
      MdtBodySetPosition(ball[i], (MeReal)(-4+i), 3, 0);
      MdtBodySetMass(ball[i], 0.30f);
      MdtBodySetAngularVelocityDamping(ball[i], 0.1f);
      MdtBodySetLinearVelocityDamping(ball[i], 0.1f);
    }

    for (i=0; i<Nballs; i++) {
      initBalls( ball[i], i );
    }

    /*
      Collision initialization.
    */

    frame = McdInit(1, 100);

    /* McdPrimitivesRegisterTypes(); */
    McdTriangleMeshRegisterType();

    /* McdPrimitivesRegisterInteractions(); */
    McdTriangleMeshTriangleMeshRegisterInteraction();

    /*
      Create a collision space.and bridge
    */
    space = McdSpaceAxisSortCreate(McdAllAxes, Nballs+3, 500, 1);
    bridge = MstBridgeCreate(frame,10);
    MstSetWorldHandlers(world);

    params = MstBridgeGetContactParams(bridge,
        MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());
    MdtContactParamsSetType(params, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(params, (MeReal)0.2);
    MdtContactParamsSetRestitution(params, (MeReal)0.4);
    /*
      Create collision models.
    */
    for (i=0; i<Nballs; i++) {
      ball_geom[i] = makeMeshSphere(frame,radius, 3, 1, (void**)&shpereData[i],0 );
      ballCM[i] = McdModelCreate(ball_geom[i]);
      McdSpaceInsertModel(space, ballCM[i]);
      McdModelSetBody(ballCM[i], ball[i]);
    }

    /* teapot */
    sgilogo_geom = McduTriangleMeshCreateFromObj( "../Resources/SgiLogo.obj", 5, 0, 0, 0.0,&vertexPtr, &vertexCount, 0);
    McdTriangleMeshBuild(sgilogo_geom);
    sgilogoCM = McdModelCreate(sgilogo_geom);
    McdModelSetTransformPtr(sgilogoCM, tmSgilogo);
    McdSpaceInsertModel(space, sgilogoCM);
    McdModelSetBody(sgilogoCM, sgiLogo);

    printf("%d\n", McdTriangleMeshGetTriangleCount(sgilogo_geom) );

    /* plane */
    plane_geom = McdTriangleMeshCreate(frame,2);

    McdTriangleMeshAddTriangle( plane_geom, vertext[0], vertext[1], vertext[2]);
    McdTriangleMeshAddTriangle( plane_geom, vertext[0], vertext[2], vertext[3]);

    McdTriangleMeshBuild( plane_geom );

    planeCM = McdModelCreate(plane_geom);
    McdModelSetTransformPtr(planeCM, tmPlane);
    McdSpaceInsertModel(space, planeCM);

    /* set number of contacts per pair */
    McdGetDefaultRequestPtr()->contactMaxCount = 6;

    /* box */
    for (i=0; i<Nballs; i++) {
      tm = MdtBodyGetTransformPtr(ball[i]);
      ballG[i] = RGraphicSphereCreate(rc, radius, color[0], tm);
      RGraphicSetTexture(rc, ballG[i], "../Resources/wood");
    }

    /* Sgilogo */
    tm = MdtBodyGetTransformPtr(sgiLogo);
    sgilogoG = RGraphicTriangleMeshCreate(rc, sgilogo_geom, color[1], tm);

    /* plane */
    planeG = RGraphicBoxCreate(rc, 20,20,(MeReal)0.05, color[2], tmPlane);
    RGraphicSetTexture(rc, planeG, "../Resources/checkerboard");

    /* Set up camera. */
    RCameraPanX( rc, 0);
    RCameraPanY( rc, 0);
    RCameraPanZ( rc, 0);

    lookat[0] = 0;
    lookat[1] = 1.5f;
    lookat[2] = 10;
    RCameraSetLookAt(rc, lookat);

    RRenderSetActionNCallBack(rc, 3, shoot, 0);
    RRenderSetActionNCallBack(rc, 5, toggleAutoEvolve, 0);
    RRenderSetActionNCallBack(rc, 4, toggleAutoShoot, 0);
    RRenderCreateUserHelp(rc,help,3);
    RRenderToggleUserHelp(rc);

    RPerformanceBarCreate(rc);
    RRenderSetWindowTitle(rc,"MeshSpin");

    meapp = MeAppCreate(world, space, rc);

    /*
      Cleanup after simulation.
    */
#ifndef PS2
    atexit(cleanup);
#endif
    /*
      Run the Simulation.
    */
    RRun(rc, tick, 0);

    return 0;
}
