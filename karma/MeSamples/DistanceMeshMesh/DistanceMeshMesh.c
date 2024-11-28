/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:11 $ - Revision: $Revision: 1.13.6.1 $

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <MeAssert.h>
#include <Mcd.h>
#include <MeMath.h>
#include <MeViewer.h>
#include <MeMemory.h>

#include <McdTriangleMesh.h>
#include "McdDistanceResult.h"
#include <McduTriangleMeshIO.h>

#include <RTriangleMesh.h>

/*
  Globals
*/
#define MAX_BODIES 10
#define MAX_PAIRS 100

/*
  render context
*/
RRender *rc;

/*
  Worlds and Spaces
*/
McdSpaceID space;
MeReal     *vertexPtr;


/*
  Colors
*/
float white[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };
float color[2][4] = {
  { 0.0f, 0.598f,0.797f, 1.0f},
  { 1.0f, 0.4f,  0.0f,   1.0f}
};

/*
  help text
*/
char *help[1] =
{
    "$ACTION2: toggleAutoEvolve",
};

/*
  Structure that holds information about an object
*/
typedef struct MyObject
{
    McdGeometryID m_geom;
    McdModelID    m_coll;
    MeMatrix4     m_TM;
    RGraphic*     m_graphic;
} MyObject;

/*
  Object instances
*/
MyObject mobj[2];
RGraphic *lineG;


enum { X, Y, Z, W };
MeReal           line[2][3];
McdModelPair    pair;
MeReal          ang = 0;
int             autoEvolve = 1;


/**
 *
 */
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

/**
 *
 */
void SetRotation(MeMatrix4Ptr m, MeReal q[4])
{
    MeQuaternionToTM(m, q);
}


/**
 *
 */
void autoRotateObj( int iObj, unsigned int iAxis, MeReal angle )
{
  MeReal q[4];
  const MeReal SQRT3 = 1.0f/(MeReal)sqrt(3);

  if ( iAxis == X )
    SetAxisAngle(q, 1, 0, 0, angle);
  else if ( iAxis == Y )
    SetAxisAngle(q, 0, 1, 0, angle);
  else if (iAxis == Z )
    SetAxisAngle(q, 0, 0, 1, angle);
  else
    SetAxisAngle(q, SQRT3, SQRT3, SQRT3, angle);

  SetRotation( mobj[iObj].m_TM, q );
}


/**
 *
 */
void EvolveWorld()
{
  McdDistanceResult resdist;

  McdTriangleMeshTriangleMeshDistance( &pair, &resdist);

  /* printf("%f \n", resdist.distanceLB ); */

  if (resdist.distanceLB > 0) {
    line[0][0] = resdist.point1[0];
    line[0][1] = resdist.point1[1];
    line[0][2] = resdist.point1[2];

    line[1][0] = resdist.point2[0];
    line[1][1] = resdist.point2[1];
    line[1][2] = resdist.point2[2];
  } else {
    line[0][0] = 0;
    line[0][1] = 0;
    line[0][2] = 0;

    line[1][0] = 0;
    line[1][1] = 0;
    line[1][2] = 0;
  }
}


/*
  This function is called every frame by the renderer.
*/
void MEAPI tick(RRender * rc, void *userData)
{
  if (autoEvolve) {
    ang += (MeReal)0.075;

    if( ang < 10 ) {
      autoRotateObj( 0, X, ang);
      autoRotateObj( 1, Z, ang);
    } else if( ang > 10 && ang < 20 ) {
      autoRotateObj( 0, W, ang);
      autoRotateObj( 1, X, ang);
    } else if( ang > 20 && ang < 30 ) {
      autoRotateObj( 0, Z, ang);
      autoRotateObj( 1, Y, ang);
    } else if( ang > 30 && ang < 40 ) {
      autoRotateObj( 0, Z, ang);
      autoRotateObj( 1, W, ang);
    } if( ang > 40 && ang < 50 ) {
      autoRotateObj( 0, W, ang);
      autoRotateObj( 1, W, ang);
    }

    if (ang >50) ang = 0;

  }

  EvolveWorld();
  RGraphicLineMoveEnds(lineG, line[0], line[1]);
}

/**
 *
 */
void MEAPI_CDECL cleanup(void)
{
  /*
    memory release. Collision objects must be explicitly deleted
    by the user.
  */
  RRenderContextDestroy(rc);

  McdGeometryDestroy(mobj[0].m_geom);
  McdModelDestroy(mobj[0].m_coll);
  McdModelDestroy(mobj[1].m_coll);

  MeMemoryAPI.destroy(vertexPtr);

  McdSpaceDestroy(space);
  McdTerm();
}

/**
 *
 */
void MEAPI toggleAutoEvolve(RRender * rc, void *userData)
{
  autoEvolve = !autoEvolve;
}


/*
  Main Routine
*/
int MEAPI_CDECL main(int argc, const char * argv[])
{
  int i, vertexCount;
  MeCommandLineOptions* options;

  /* Capture the command line */
  options = MeCommandLineOptionsCreate(argc, argv);
  /*
    Initialize renderer
  */
  rc = RRenderContextCreate(options, 0, !MEFALSE);
  MeCommandLineOptionsDestroy(options);
  if (!rc)
    return 1;

  /*
    Initialize toolkit
  */
  McdInit(1, 100);

  McdTriangleMeshRegisterType();
  McdTriangleMeshRegisterInteractions();

  space = McdSpaceAxisSortCreate(McdAllAxes, MAX_BODIES, 2 * MAX_BODIES,1);

  RCameraSetView(rc,(float)15,(float)-0.1,(float)0.5);

  mobj[0].m_geom = McduTriangleMeshCreateFromObj("../Resources/teapot566.obj",
              1, 0, 0, 0.0, &vertexPtr, &vertexCount, 1);
  McdTriangleMeshBuild(mobj[0].m_geom);

  mobj[1].m_geom = mobj[0].m_geom;

  for (i=0; i<2; i++) {
    MeMatrix4TMMakeIdentity(mobj[i].m_TM);
    mobj[i].m_TM[3][0] = (MeReal) (10*(i-0.5));
    mobj[i].m_TM[3][1] = 0;
    mobj[i].m_TM[3][2] = 0;

    mobj[i].m_coll= McdModelCreate(mobj[i].m_geom);

    McdModelSetTransformPtr(mobj[i].m_coll, mobj[i].m_TM);
    McdSpaceInsertModel(space, mobj[i].m_coll);
    mobj[i].m_graphic = RGraphicTriangleMeshCreate(rc, mobj[i].m_geom, color[i], mobj[i].m_TM);
  }

  pair.model1 = mobj[0].m_coll;
  pair.model2 = mobj[1].m_coll;

  lineG = RGraphicLineCreate(rc, line[0], line[2], white, 0);

  /*
    Build Space
  */
  McdSpaceBuild(space);

  /*
    Keyboard callbacks
  */
  RRenderSetActionNCallBack(rc, 2, toggleAutoEvolve, 0);

  /*
    On screen help text
  */
  RRenderSetWindowTitle(rc,"Mesh/Mesh Distance");
  RRenderCreateUserHelp(rc, help, 1);
  RRenderToggleUserHelp(rc);

  /*
    cleanup after simulation
  */
  atexit(cleanup);

  /*
    run the simulation loop
  */
  RRun(rc, tick, 0);

  return 0;
}
