/*
  Copyright (c) 1997-2002 MathEngine PLC
  www.mathengine.com

  $Name: t-stevet-RWSpre-030110 $

  $Id: ConeAndFriction.c,v 1.7.10.1 2002/04/04 15:28:47 richardm Exp $
*/

/*
   Interaction of Cone/Sphere/Plane with different friction coefficients
*/

#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <crtdbg.h>
#endif

#include <Mdt.h>
#include <MeMath.h>
#include <MeApp.h>
#include <MeViewer.h>


MdtWorldID  world;
McdSpaceID  space;
MstBridgeID bridge;
MeApp*      meapp;

MdtContactParamsID params;

/*
  Physics representations.
*/

/* dynamics bodies (box) ID */
MdtBodyID cone;
MdtBodyID ball;

/*
  Graphics representations.
*/

RGraphic* coneG;
RGraphic* ballG;
RGraphic* planeG;
RRender*  rc = 0;

/*
  Collision representations.
*/

McdModelID coneCM;
McdModelID ballCM;
McdModelID planeCM;

/* Collision geometry ID (box) */
McdGeometryID cone_geom = 0;
McdGeometryID ball_geom = 0;
McdGeometryID plane_geom = 0;

int autoBenchmark = 0;
int autoEvolve = 1;

MeReal step = (MeReal) 0.02;
MeReal gravity[3] = { 0, -9.8f, 0 };

MeReal hcn  =  4;
MeReal rcn  =  2.5f;
MeReal rs   =  0.25f;


/*------------------------------------------------------------------------*/

MeMatrix4 tmPlane =
{
    1, 0, 0, 0,
    0, 0,-1, 0,
    0, 1, 0, 0,
    0, 0, 0, 1
};

char *help[2] =
{
    "$ACTION2 - reset",
    "$ACTION3 - shoot",
};


/**
 *
 */
void stepEvolve()
{
  MeAppStep(meapp);
  MeProfileStartSection("Collision", 0);
  McdSpaceUpdateAll(space);
  MstBridgeUpdateContacts(bridge, space, world);
  MeProfileEndSection("Collision");

  MeProfileStartSection("Dynamics", 0);
  MdtWorldStep(world, step);
  MeProfileEndSection("Dynamics");

  MeAppDrawContacts(meapp);
}

int ifric = 1;
int iq = 0;

void MEAPI reset(RRender* rc, void* useData)
{
    const MeReal SQRT2 = MeSqrt(2)/2.0f;

    MdtBodySetPosition(cone, 0,8,0);
    if ( (iq/2)%2 == 0)
      MdtBodySetQuaternion(cone, 1, 0, 0, 0);
    else
      MdtBodySetQuaternion(cone, SQRT2,-SQRT2,0,0);

    MdtBodySetAngularVelocity(cone, 0.0f,0,0);
    MdtBodySetLinearVelocity(cone, 0,0,0);

    MdtBodySetPosition(ball, 0,4,0.1f);
    MdtBodySetQuaternion(ball, 1,0,0,0);
    MdtBodySetAngularVelocity(ball, 0.1f,0,0);
    MdtBodySetLinearVelocity(ball, 0,0,0);

    if ( iq%2 == 0 ) {
      MdtContactParamsSetFriction(params, 0.5f);
      printf("friction coef. = 0.5\n");
    }
    else {
      MdtContactParamsSetFriction(params, 0.1f);
      printf("friction coef. = 0.1\n");
    }

    if ( (iq/4)%2 == 0) {
      ballG->m_pObject->m_bIsWireFrame = 0;
      coneG->m_pObject->m_bIsWireFrame = 0;
    } else {
      ballG->m_pObject->m_bIsWireFrame = 1;
      coneG->m_pObject->m_bIsWireFrame = 1;
    }

    iq++;
}


/**
 *
 */
void toggleAutoEvolve(void)
{
    autoEvolve = !autoEvolve;
}


/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender* rc, void* useData)
{
  stepEvolve();
}

/**
 *
 */
void cleanup(void)
{
    /* graphics */

    RRenderContextDestroy(rc);

    /* dynamics */
    MdtWorldDestroy(world);

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
    int material = 0;
    static float color[3][4] = { {0.5f, 0.5f, 0,1}, {0.5f, 0, 0,1}, {0, 1, 1,1} };
    int debugFlag = 0;
    MeCommandLineOptions *options;

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

    world = MdtWorldCreate(3, 200);
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    MdtWorldSetAutoDisableVelocityThreshold(world, 0.05f);
    MdtWorldSetAutoDisableAngularVelocityThreshold(world, 0.05f);

    /*
      Collision initialization.
    */

    McdInit(McdPrimitivesGetTypeCount(), 100);
    McdPrimitivesRegisterTypes();
    McdPrimitivesRegisterInteractions();

    /*
      Create a collision space.
    */
    space = McdSpaceAxisSortCreate(McdAllAxes, 50, 100, 1);
    bridge = MstBridgeCreate(10);
    MstSetWorldHandlers(world);

    /*
      Create physical bodies.
    */
    /* cone */
    cone = MdtBodyCreate(world);
    MdtBodyEnable(cone);
    MdtBodySetMass(cone, 0.5f);
    MdtBodySetAngularVelocityDamping(cone, 0.5f);
    MdtBodySetLinearVelocityDamping(cone, 0.1f);

    /* ball */
    ball = MdtBodyCreate(world);
    MdtBodyEnable(ball);
    MdtBodySetMass(ball, 0.5f);
    MdtBodySetAngularVelocityDamping(ball, 0.5f);
    MdtBodySetLinearVelocityDamping(ball, 0.1f);

        /*
      Set parameters for contacts.
    */
    params = MstBridgeGetContactParams(bridge,0,0);

    MdtContactParamsSetType(params, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(params, 0.5f);
    MdtContactParamsSetRestitution(params, (MeReal)0.0);

    /*
      Create collision models.
    */

    cone_geom = McdConeCreate( hcn, rcn );
    coneCM = McdModelCreate(cone_geom);
    McdSpaceInsertModel(space, coneCM);
    McdModelSetBody(coneCM,  cone);

    ball_geom = McdSphereCreate( rs );
    ballCM = McdModelCreate(ball_geom);
    McdSpaceInsertModel(space, ballCM);
    McdModelSetBody(ballCM, ball);

    /* plane */
    plane_geom = McdPlaneCreate();

    planeCM = McdModelCreate(plane_geom);
    McdModelSetTransformPtr(planeCM, tmPlane);
    McdSpaceInsertModel(space, planeCM);

    McdSpaceBuild(space);

    /* set number of contacts per pair */

    McdGetDefaultRequestPtr()->contactMaxCount = 3;

    /* cone */
    tm = MdtBodyGetTransformPtr(cone);
    coneG = RGraphicConeCreate(rc, rcn, hcn*0.75f, hcn*0.25f, color[0],  tm);
    // coneG->m_pObject->m_bIsWireFrame = 1;

    /* cone */
    tm = MdtBodyGetTransformPtr(ball);
    ballG = RGraphicSphereCreate(rc, rs, color[1],  tm);
    // ballG->m_pObject->m_bIsWireFrame = 1;

    /* plane */
    planeG = RGraphicBoxCreate(rc, 20,20,(MeReal)0.05, color[2], tmPlane);
    RGraphicSetTexture(rc, planeG, "checkerboard");

    reset(rc, 0);

    /*
      Set up camera.
    */
    RCameraRotateAngle( rc, 1.57f );
    RCameraRotateElevation( rc, 0.5f);
    RCameraZoom( rc, 1.0f);

    RRenderSetActionNCallBack(rc, 2, reset, 0);
    // RRenderSetActionNCallBack(rc, 3, Shoot, 0);

    RRenderCreateUserHelp(rc,help,2);

    RRenderToggleUserHelp(rc);
    RPerformanceBarCreate(rc);

    RRenderSetWindowTitle(rc,"Cone/Sphere/Plane Dynamical Interactions");

    meapp = MeAppCreate(world, space, rc);
    MeAppDrawContactsInit(meapp, color[2], 100);

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
