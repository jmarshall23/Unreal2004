/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:13 $ - Revision: $Revision: 1.38.6.1 $

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

MdtWorldID    world;
McdSpaceID    space;
MstBridgeID   bridge;
MeApp*        meapp;

const int ringCount = 5;

MdtBodyID b[ringCount];
MdtBodyID ball;

RGraphic* ringGraphics[ringCount];
RGraphic* ballGraphics;
RRender*  rc = 0;

char *help[1] =
{
    "Press $ACTION2 to reset",
};

McdModelID m[ringCount];
McdGeometryID ringGeom;

int autoBenchmark = 0;

MeReal step = (MeReal) 0.05;
MeReal gravity[3] = { 0, -6, 0 };

MeReal *vertexPtr;


int isteps = 0;
int autoEvolve = 1;
int xfdir = -1;


void stepEvolve()
{
  MeReal xf = 10.0f*xfdir;
  MeAppStep(meapp);
  MeProfileStartSection("Collision", 0);
  McdSpaceUpdateAll(space);
  MstBridgeUpdateContacts(bridge, space, world);
  MeProfileEndSection("Collision");

  MeProfileStartSection("Dynamics", 0);
  MdtWorldStep(world, step);
  MeProfileEndSection("Dynamics");

  if (isteps<10) MdtBodyAddForce(b[4], xf, -5, 0);
}


void MEAPI tick(RRender* rc, void* useData)
{
  if (autoEvolve) stepEvolve();
  isteps++;
}

void MEAPI reset(RRender* rc, void* useData)
{
  int i;
  MeVector4 rot;

  isteps = 0;
  xfdir = -xfdir;

  for(i=1;i<ringCount;i++) {
    MdtBodyEnable(b[i]);
    MdtBodySetPosition(b[i], i*0.02f, i*1.8f, 0);
    MeVector3 axis = {0,1,0};
    MeQuaternionMake( rot, axis,  ME_PI/2.0);
    if ((i%2)) MdtBodySetQuaternion(b[i], rot[0],rot[1],rot[2],rot[3]);
    else MdtBodySetQuaternion(b[i],1,0,0,0);

    MdtBodySetAngularVelocity(b[i], 0,0,0);
    MdtBodySetLinearVelocity(b[i], 0,0,0);

    MdtBodySetMass(b[i], 0.90f);
    MdtBodySetAngularVelocityDamping(b[i], 0.3f);
    MdtBodySetLinearVelocityDamping(b[i], 0.3f);
  }
}



void cleanup(void)
{
  int i;

  /* dynamics */
  MdtWorldDestroy(world);

  McdGeometryDestroy(ringGeom);
  MeMemoryAPI.destroy( vertexPtr);

  for (i=0; i<ringCount; i++) {
    McdModelDestroy(m[i]);
  }

  McdSpaceDestroy(space);
  MstBridgeDestroy(bridge);
  McdTerm();

  MeAppDestroy(meapp);

  RRenderContextDestroy(rc);
}

int main(int argc, const char **argv)
{
  MdtContactParamsID params;
  MeReal lookat[3];
  int vertexCount;

  static float color[5][4] =
  { {0,0,0.65f,1},
    {0.65f,0.65f,0,1},
    {0.15f,0.15f,0.15f,1},
    {0,0.65f,0,1},
    {0.65f,0,0,1}
  };

  MeCommandLineOptions* options;

  options = MeCommandLineOptionsCreate(argc, argv);
  rc = RRenderContextCreate(options, 0, !MEFALSE);
  MeCommandLineOptionsDestroy(options);
  if (!rc)
    return 1;

  world = MdtWorldCreate(ringCount+1, 1000);
  MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

  int i;
  b[0] = 0;
  for (i=1; i<ringCount; i++){
    b[i] = MdtBodyCreate(world);
  }
  reset(rc,0);

  McdInit(2, 100);
  McdTriangleMeshRegisterType();
  McdTriangleMeshTriangleMeshRegisterInteraction();

  space = McdSpaceAxisSortCreate(McdAllAxes, 50, 100,1);

  bridge = MstBridgeCreate(1);
  MstSetWorldHandlers(world);

  MeMatrix4 m0TM;
  MeMatrix4TMMakeIdentity(m0TM);
  MeMatrix4TMSetPosition(m0TM,0,0,0);

  ringGeom =  McduTriangleMeshCreateFromObj( "rdonut2.obj",
      5.0, 0, 0, 0.0, &vertexPtr,&vertexCount, McdTriangleMeshOptionNoDistance);
  McdTriangleMeshBuild(ringGeom);

  for (i=0; i< ringCount; i++){
    m[i] = McdModelCreate(ringGeom);

    McdSpaceInsertModel(space, m[i]);
    McdModelSetBody(m[i], b[i]); /* set to 0 for body 0 */
    if (i==0) McdModelSetTransformPtr(m[i], m0TM);
  }
  McdGetDefaultRequestPtr()->contactMaxCount = 6;
  McdSpaceBuild(space);

  params = MstBridgeGetContactParams(bridge,0, 0);
  MdtContactParamsSetType(params, MdtContactTypeFriction2D);
  MdtContactParamsSetFriction(params, 3.0);
  MdtContactParamsSetRestitution(params, (MeReal)0.3);

  meapp = MeAppCreate(world, space, rc);

  for (i=0; i< ringCount; i++){
    ringGraphics[i] = RGraphicTriangleMeshCreate(rc, ringGeom, color[i], McdModelGetTransformPtr(m[i]));
  }

  /* Set up camera. */
  RCameraPanX( rc, 0.0f);
  RCameraPanY( rc, 2.0f);
  RCameraPanZ( rc, -10);

  lookat[0] = 0;
  lookat[1] = -1;
  lookat[2] = 0;
  RCameraSetLookAt(rc, lookat);

  rc->m_backgroundColour[0] = 0;
  rc->m_backgroundColour[1] = 0.4f;
  rc->m_backgroundColour[2] = 0.6f;

  RPerformanceBarCreate(rc);
  RRenderSetWindowTitle(rc, "MeshChain example");
  RRenderCreateUserHelp(rc,help,1);
  RRenderToggleUserHelp(rc);
  RRenderSetActionNCallBack(rc, 2, reset, 0);

#ifndef PS2
  atexit(cleanup);
#endif

  RRun(rc, tick, 0);

  return 0;
}

