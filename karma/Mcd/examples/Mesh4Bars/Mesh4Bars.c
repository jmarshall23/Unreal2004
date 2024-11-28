/*
  Copyright (c) 1997-2002 MathEngine PLC
  www.mathengine.com

  $Name: t-stevet-RWSpre-030110 $

  $Id: Mesh4Bars.c,v 1.39.10.1 2002/04/04 15:28:48 richardm Exp $
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

#include <RTriangleMesh.h>

/* number of  shooting boxs */
#define Nboxs        1
/* maximum number of contacts */
#define NContacts   200

MdtWorldID  world;
McdSpaceID  space;
MstBridgeID bridge;
MeApp*      meapp;

/*
  Physics representations.
*/

/* dynamics bodies (box) ID */
MdtBodyID box;
MdtBodyID teapot[2];
MdtBodyID crank;
MdtBodyID rod;

MdtHingeID j_crank_ground;
MdtHingeID j_crank_rod;
MdtHingeID j_rod_box;
MdtHingeID j_box_ground;

/*
  Graphics representations.
*/

RGraphic* boxG;
RGraphic* boxExtG;
RGraphic* teapotG[2];
RGraphic* crankG;
RGraphic* rodG;

RRender *rc = 0;

/*
  Collision representations.
*/

McdModelID boxCM;
McdModelID teapotCM[2];

/* Collision geometry ID */
McdGeometryID box_geom    = 0;
McdGeometryID boxExt_geom    = 0;
McdGeometryID teapot_geom[2];

int autoBenchmark = 0;

MeReal step = (MeReal) 0.01;
MeReal gravity[3] = { 0, -9.8f, 0 };

MeReal *vertexPtr[2];

MeMatrix4 tm1, tm2;
MeMatrix4 tm3 = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
MeMatrix4 tm4 = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1} };
MeReal r3[3] = {-3.5f,0,0};
MeReal r4[3] = { 3.5f,0,0};

MeMatrix4Ptr  tm;
MeMatrix3     crankI;

#define bxy 3
#define bz 4

#define bexy (bxy+(MeReal)0.1)
#define bez (bz+(MeReal)0.1)

/*------------------------------------------------------------------------*/

char *help[1] =
{
    "$ACTION5 - toggle pause",
};

/**
 *
 */
void toggleBenchmark(void)
{
    autoBenchmark = !autoBenchmark;
    printf("%i\n", autoBenchmark);
}

/*-------------------------------------------------------------------------*/

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
}


int autoEvolve = 1;

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
void TxV( MeMatrix4Ptr tm, MeReal vin[3], MeReal vout[3])
{
  int i, j;
  for (i=0; i<3; i++) {
    vout[i] = 0.0f;
    for (j=0; j<3; j++) {
      vout[i] += tm[j][i]*vin[j];
    }
    vout[i] += tm[3][i];
  }
}

/**
 *
 */
void setTM( const MeMatrix4Ptr tmi, MeMatrix4Ptr tmo)
{
  int i, j;
  for (i=0; i<4; i++) {
    for (j=0; j<4; j++)
      tmo[i][j] = tmi[i][j];
  }
}


int isteps = 0;

void MEAPI tick(RRender* rc, void* useData)
{
    MeReal resl[3];

    if (autoEvolve) {
      if ( isteps < 200)
        MdtBodyAddTorque(crank, 0, 0, (MeReal)(260-1*isteps));
      else
        MdtBodyAddTorque(crank, 0, 0, 60);

      isteps++;

      stepEvolve();
    }

    tm = MdtBodyGetTransformPtr(rod);
    TxV( tm, r3, resl);
    tm3[3][0] = resl[0];
    tm3[3][1] = resl[1];
    tm3[3][2] = resl[2];

    TxV( tm, r4, resl);
    tm4[3][0] = resl[0];
    tm4[3][1] = resl[1];
    tm4[3][2] = resl[2];

}

/**
 *
 */
void cleanup(void)
{
    int i = 0;
    /* graphics */
    RRenderContextDestroy(rc);

    /* dynamics */
    MdtWorldDestroy(world);

    for (i=0; i<2; i++) {
      McdGeometryDestroy(teapot_geom[i]);
      McdModelDestroy(teapotCM[i]);
      MeMemoryAPI.destroy( vertexPtr[i] );
    }

    McdGeometryDestroy(boxExt_geom);
    McdGeometryDestroy(box_geom);
    McdModelDestroy(boxCM);

    McdSpaceDestroy(space);
    MstBridgeDestroy(bridge);
    McdTerm();

    MeAppDestroy(meapp);
}

/**
 *
 */
int main(int argc, const char **argv)
{
    int i, material = 1;
    MdtContactParamsID  params;

    static float color[5][3] = { {0, 0.5f, 0}, {1,1,0}, {0, 0.4f,0.6f},{0,1,1},{0,0,1} };
    static MeReal transl[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
    MeReal resl[3];    
     
    MeVector3 vertexb[8] = {{bxy,bxy,bz}, {-bxy,bxy,bz}, {-bxy,-bxy,bz}, {bxy,-bxy,bz}, 
    			 {bxy,bxy,-bz}, {-bxy,bxy,-bz}, {-bxy,-bxy,-bz}, {bxy,-bxy,-bz}};    

    MeVector3 vertexbe[8] = {{bexy,bexy,bez}, {-bexy,bexy,bez}, {-bexy,-bexy,bez}, {bexy,-bexy,bez}, 
    			 {bexy,bexy,-bez}, {-bexy,bexy,-bez}, {-bexy,-bexy,-bez}, {bexy,-bexy,-bez}};     

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

    world = MdtWorldCreate(5, 200);
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    MdtWorldSetAutoDisableVelocityThreshold(world, 0.05f);
    MdtWorldSetAutoDisableAngularVelocityThreshold(world, 0.05f);
    /*
      Create physical bodies.
    */

    /* box */
    box = MdtBodyCreate(world);
    MdtBodyEnable(box);
    MdtBodySetPosition(box, 2, 0,0);
    MdtBodySetMass(box, 1.0f);
    MdtBodySetAngularVelocityDamping(box, 0.3f);
    MdtBodySetLinearVelocityDamping(box, 0.3f);

    /* teapot */
    teapot[0] = MdtBodyCreate(world);
    MdtBodyEnable(teapot[0]);
    MdtBodySetPosition(teapot[0], 2, 0, -2.5);
    MdtBodySetMass(teapot[0], 2.50f);
    MdtBodySetAngularVelocityDamping(teapot[0], 0.3f);
    MdtBodySetLinearVelocityDamping(teapot[0], 0.3f);

    /* teapot */
    teapot[1] = MdtBodyCreate(world);
    MdtBodyEnable(teapot[1]);
    MdtBodySetPosition(teapot[1], 2, 0, 2.5);
    MdtBodySetMass(teapot[1], 2.5f);
    MdtBodySetAngularVelocityDamping(teapot[1], 0.3f);
    MdtBodySetLinearVelocityDamping(teapot[1], 0.3f);

    /* crank */
    crank = MdtBodyCreate(world);
    MdtBodyEnable(crank);
    MdtBodySetPosition(crank, -5, -3, -bz );
    MdtBodySetMass(crank, 1);

    crankI[0][0] = 50;
    crankI[1][1] = 50;
    crankI[2][2] = 50;
    MdtBodySetInertiaTensor(crank, crankI);

    MdtBodySetAngularVelocityDamping(crank, 0.3f);
    MdtBodySetLinearVelocityDamping(crank, 0.3f);

    /* rod */
    rod = MdtBodyCreate(world);
    MdtBodyEnable(rod);
    MdtBodySetPosition(rod, -1.5f, -2, -(bz+0.3f));
    MdtBodySetMass(rod, 1.0f);
    MdtBodySetAngularVelocityDamping(rod, 0.3f);
    MdtBodySetLinearVelocityDamping(rod, 0.3f);

    /*
      Create joints between bodies.
    */
    j_crank_ground = MdtHingeCreate(world);
    MdtHingeSetBodies(j_crank_ground,crank,0);
    MdtHingeSetPosition(j_crank_ground, -5, -3, -bz);
    MdtHingeSetAxis(j_crank_ground, 0, 0, 1);
    MdtHingeEnable(j_crank_ground);

    j_crank_rod = MdtHingeCreate(world);
    MdtHingeSetBodies(j_crank_rod,crank,rod);
    MdtHingeSetPosition(j_crank_rod, -5, -2, -bz);
    MdtHingeSetAxis(j_crank_rod, 0, 0, 1);
    MdtHingeEnable(j_crank_rod);

    j_rod_box = MdtHingeCreate(world);
    MdtHingeSetBodies(j_rod_box,box,rod);
    MdtHingeSetPosition(j_rod_box, 2, -2, -bz);
    MdtHingeSetAxis(j_rod_box, 0, 0, 1);
    MdtHingeEnable(j_rod_box);

    j_box_ground = MdtHingeCreate(world);
    MdtHingeSetBodies(j_box_ground,box,0);
    MdtHingeSetPosition(j_box_ground, 2, 0, 0);
    MdtHingeSetAxis(j_box_ground, 0, 0, 1);
    MdtHingeEnable(j_box_ground);

    /*
      Collision initialization.
    */
    McdInit(1, 100);

    /* McdPrimitivesRegisterTypes(); */
    McdTriangleMeshRegisterType();
    McdTriangleMeshTriangleMeshRegisterInteraction();

    /* McdDtBridgeInit(0); */
    bridge = MstBridgeCreate(10);
    MstSetWorldHandlers(world);

    /*
      Create a collision space.
    */
    space = McdSpaceAxisSortCreate(McdAllAxes, 50, 100, 1);
    /* bridge = McdDtBridgeCreate();  */

    /*
      Create collision models.
    */

    /* box */
    box_geom = McdTriangleMeshCreate(12);

    McdTriangleMeshAddTriangle( box_geom, vertexb[1], vertexb[0], vertexb[2]);
    McdTriangleMeshAddTriangle( box_geom, vertexb[2], vertexb[0], vertexb[3]);
    McdTriangleMeshAddTriangle( box_geom, vertexb[4], vertexb[3], vertexb[0]);
    McdTriangleMeshAddTriangle( box_geom, vertexb[7], vertexb[3], vertexb[4]);

    McdTriangleMeshAddTriangle( box_geom, vertexb[1], vertexb[4], vertexb[0]);
    McdTriangleMeshAddTriangle( box_geom, vertexb[1], vertexb[5], vertexb[4]);
    McdTriangleMeshAddTriangle( box_geom, vertexb[5], vertexb[1], vertexb[2]);
    McdTriangleMeshAddTriangle( box_geom, vertexb[6], vertexb[5], vertexb[2]);

    McdTriangleMeshAddTriangle( box_geom, vertexb[2], vertexb[3], vertexb[6]);
    McdTriangleMeshAddTriangle( box_geom, vertexb[7], vertexb[6], vertexb[3]);
    McdTriangleMeshAddTriangle( box_geom, vertexb[4], vertexb[5], vertexb[6]);
    McdTriangleMeshAddTriangle( box_geom, vertexb[7], vertexb[4], vertexb[6]);
    McdTriangleMeshBuild( box_geom );

    boxCM = McdModelCreate(box_geom);
    McdSpaceInsertModel(space, boxCM);
    McdModelSetBody(boxCM, box);

    /* box */
    boxExt_geom = McdTriangleMeshCreate(12);

    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[1], vertexbe[0], vertexbe[2]);
    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[2], vertexbe[0], vertexbe[3]);
    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[4], vertexbe[3], vertexbe[0]);
    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[7], vertexbe[3], vertexbe[4]);

    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[1], vertexbe[4], vertexbe[0]);
    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[1], vertexbe[5], vertexbe[4]);
    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[5], vertexbe[1], vertexbe[2]);
    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[6], vertexbe[5], vertexbe[2]);

    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[2], vertexbe[3], vertexbe[6]);
    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[7], vertexbe[6], vertexbe[3]);
    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[4], vertexbe[5], vertexbe[6]);
    McdTriangleMeshAddTriangle( boxExt_geom, vertexbe[7], vertexbe[4], vertexbe[6]);

    McdTriangleMeshBuild( boxExt_geom );

    /* teapot */
    for (i=0; i<2; i++) {
      int vertexCount;
      teapot_geom[i] = McduTriangleMeshCreateFromObj( "../Resources/teapot566.obj",
                        0.75, 0, 0, 0.0,&vertexPtr[i], &vertexCount, 0);
      McdTriangleMeshBuild(teapot_geom[i]);
      teapotCM[i] = McdModelCreate(teapot_geom[i]);

      McdSpaceInsertModel(space, teapotCM[i]);
      McdModelSetBody(teapotCM[i], teapot[i]);
    }

    teapotCM[0]->mBody = teapot[0];
    teapotCM[1]->mBody = teapot[1];
    boxCM->mBody = box;

    /*
      Set parameters for contacts.
    */
    McdSpaceBuild(space);

    /* set number of contacts per pair */
    McdGetDefaultRequestPtr()->contactMaxCount = 6;

      /* Set parameters for contacts. */
    params = MstBridgeGetContactParams(bridge,
        MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());
    MdtContactParamsSetType(params, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(params, (MeReal)3.0);
    MdtContactParamsSetRestitution(params, (MeReal)0.3);

    /*
      Rendering.
    */ 
    /* box */ 
    tm = MdtBodyGetTransformPtr(box);     
    boxG = RGraphicTriangleMeshCreate(rc, box_geom, color[0], tm);
    /* McduDrawTriangleMeshSetWFTriangle(box_geom, 10, 0); */
 
    boxExtG = RGraphicTriangleMeshCreate(rc, boxExt_geom, color[0], tm);
    boxG->m_pObject->m_bIsWireFrame = 1;
    boxExtG->m_pObject->m_bIsWireFrame = 1;
    /* McduDrawTriangleMeshSetWFTriangle(boxExt_geom, 10, 0); */

    setTM( tm, tm1 );
    tm1[3][2] -= (bz+0.2f);
    RGraphicCylinderCreate(rc, 0.2f, 0.2f, color[4], tm1);
    setTM( tm, tm2 );
    tm2[3][2] += (bz+0.2f);
    RGraphicCylinderCreate(rc, 0.2f, 0.2f, color[4], tm2);

    /* teapot */
    tm = MdtBodyGetTransformPtr(teapot[0]);
    teapotG[0] = RGraphicTriangleMeshCreate(rc, teapot_geom[0], color[1], tm);
     
    tm = MdtBodyGetTransformPtr(teapot[1]);
    teapotG[1] = RGraphicTriangleMeshCreate(rc, teapot_geom[1], color[2], tm);

    /* */
    tm = MdtBodyGetTransformPtr(rod);
    rodG = RGraphicBoxCreate(rc, 7.50f,0.4f,0.3f, color[3], tm);

    TxV( tm, r3, resl);
    tm3[3][0] = resl[0];
    tm3[3][1] = resl[1];
    tm3[3][2] = resl[2];
    RGraphicCylinderCreate(rc, 0.15f, 0.4f, color[4], tm3);

    TxV( tm, r4, resl);
    tm4[3][0] = resl[0];
    tm4[3][1] = resl[1];
    tm4[3][2] = resl[2];
    RGraphicCylinderCreate(rc, 0.15f, 0.4f, color[4], tm4);

    tm = MdtBodyGetTransformPtr(crank);
    crankG = RGraphicCylinderCreate(rc, 1.5f, 0.3f, color[1], tm);
    RGraphicCylinderCreate(rc, 0.2f, 0.325f, color[4], tm);

    /*
      Set up camera.
    */
    RCameraPanX( rc, 0);
    RCameraPanY( rc, -2);
    RCameraPanZ( rc, 0);
    RCameraRotateAngle( rc, 0 );
    RCameraRotateElevation( rc, 0.5f);
    RCameraZoom( rc, 1.0f);

    RRenderSetActionNCallBack(rc, 5, toggleAutoEvolve, 0);

    RRenderCreateUserHelp(rc,help,1);
    RRenderToggleUserHelp(rc);

    RPerformanceBarCreate(rc);
    RRenderSetWindowTitle(rc,"Mesh4Bars");
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
