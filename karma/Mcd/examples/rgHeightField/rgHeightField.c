/*
  Copyright (c) 1997-2002 MathEngine PLC

    Intereaction McdRGHeightField/McdSphere
    ( McdRGHeightField -- regular grid height field)

    Libs required (as of 11/May/2000)
        * mcd
        * mcddtbridge
        * mdt
        * mdtkea
        * render
        * MeGlobals
        * MdtConstraints
*/
/*----------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>

#include <Mdt.h>            /* MathEngine Dynamics  Toolkit - Kea solver      */
#include <McdFrame.h>
#include <McdPrimitives.h>
#include <McdRGHeightField.h>
#include <McduRGHeightFieldIO.h>
#include <McdConvexMesh.h>
#include <McduRequestTable.h>
#include <Mst.h>    /* Cd to Dynamics Bridge: prefix McdDtBridge      */

#include <MeViewer.h>         /* MathEngine Mini-Renderer / Viewer              */
#include <McduDrawGrid.h>
#include <McduDrawConvexMesh.h>
#include <McduDrawMcdContacts.h>

/* #define CONVEX_RGHF  */

/* #define CONVEX_LOAD */

/* #define LOAD_GRID */   /* load from bmp file */

#ifdef CONVEX_LOAD
#include <McduConvexMeshIO.h>
#endif

/* #define CONVEX_RGHF */
/* #define USE_PLANE */

MdtWorldID world;     /* World for the Dynamics Toolkit simulation */
McdSpaceID space;     /* Space for the Collision Toolkit           */
MstBridgeID bridge; /* Connection between dynamics and collision */
int geoTypeCount;

/* Physics representations */
MdtBodyID conv;       /* dynamics bodies (balls) ID               */
MdtBodyID ball;       /* dynamics bodies (balls) ID               */
MdtBodyID box;        /* dynamics bodies (balls) ID               */
MdtBodyID cyl1;       /* dynamics bodies (balls) ID               */
MdtBodyID cyl2;       /* dynamics bodies (balls) ID               */
MdtBodyID cyl3;       /* dynamics bodies (balls) ID               */

const int NBodies = 10;
const int NContacts = 200;


/* Graphics representations */
RGraphicsDescription* convG;
RGraphicsDescription* ballG;
RGraphicsDescription* boxG;
RGraphicsDescription* cyl1G;
RGraphicsDescription* cyl2G;
RGraphicsDescription* cyl3G;
RGraphicsDescription* gridG;
RRender*              rc = 0;

/* Collision representations */
McdModelID boxCM;
McdModelID convCM;
McdModelID ballCM;
McdModelID cyl1CM;
McdModelID cyl2CM;
McdModelID cyl3CM;
McdModelID gridCM;

McdGeometryID conv_prim;     /* Collision geometry ID (ball)      */
McdGeometryID ball_prim;     /* Collision geometry ID (ball)      */
McdGeometryID box_prim;      /* Collision geometry ID (ball)      */
McdGeometryID cyl1_prim;
McdGeometryID cyl2_prim;
McdGeometryID cyl3_prim;
McdGeometryID grid_prim;


MeReal  gravity[3] = {0, -9.8f, 0};
int     autoBenchmark = 0;
MeReal  step = (MeReal) 0.02;

int         autoEvolve = 1;
int         autoDeform_left  = 0;
int         autoDeform_right = 0;

MeVector3 *data;
MeReal *heights;
McdGrid   grid;

int ixGrid = 40;
int iyGrid = 40;
MeReal deltaX = 0.5f;
MeReal deltaY = 0.5f;

MeReal zval  = 0.1f;

MeReal angHz = 1.5f;

int stopped = 0;

#define r0 1
#define r1 0.5f
#define r2 1

/* press F1 to get the help text */
char *help[] = {"'a' to toggle pause",
                                "'b' to toggle benchmark",
                "'q' to toggle deforming terrain (left)",
                "'w' to toggle deforming terrain (right)" };


/*-------------------------------------------------------------------------*/
void toggleBenchmark(void)
{
    autoBenchmark = !autoBenchmark;
  printf("%i\n", autoBenchmark);
}

/*-------------------------------------------------------------------------*/
void toggleStopped(void)
{
    stopped = !stopped;
}

/*-------------------------------------------------------------------------*/
void toggleAutoDeform_Left(void)
{
  autoDeform_right = 0;
    autoDeform_left = !autoDeform_left;
}

/*-------------------------------------------------------------------------*/
void toggleAutoDeform_Right(void)
{
  autoDeform_left  = 0;
    autoDeform_right = !autoDeform_right;
}


/*-------------------------------------------------------------------------*/
void Normalize(MeReal *v) {
    MeReal norm = 1.0f/(MeReal)sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    v[0] *= norm;
    v[1] *= norm;
    v[2] *= norm;
}

/*-------------------------------------------------------------------------*/
void Shoot(void)
{
  int i;
  MeReal v[3];
  MdtBodyEnable( ball );
  MdtBodySetPosition(ball,  rc->m_cameraPos[0],
                            rc->m_cameraPos[1], rc->m_cameraPos[2]);
  for (i=0;i<3;i++) { v[i] = rc->m_cameraLookAt[i]-rc->m_cameraPos[i]; }
  Normalize(v);
  for (i=0;i<3;i++) { v[i] *= 20.0; }
  MdtBodySetLinearVelocity(ball, v[0], v[1], v[2]);
  MdtBodySetAngularVelocity(ball, 0, 0, 0);
}

void
checkPairContainer( McdSpaceID space )
{
  McdModelPairContainer pairs;
  McdModelPair **array;
  int size;

  McdSpacePairIterator it;
  int overflow;
  int numHello,numStaying,numGoodbye;

  size = 100;
  array = (McdModelPair**)malloc( size * sizeof( McdModelPair* ) );
  McdModelPairContainerInit( &pairs, array, size );

  McdSpacePairIteratorBegin( space, &it );

  numHello = numStaying = numGoodbye = 0;
  overflow = 0;
  overflow = McdSpaceGetPairs( space, &it, &pairs );
    numGoodbye = McdModelPairContainerGetGoodbyeCount( &pairs );
    numStaying = McdModelPairContainerGetStayingCount( &pairs );
    numHello = McdModelPairContainerGetHelloCount( &pairs );

#if 0
  while( !overflow ) {
    overflow = McdSpaceGetPairs( space, &it, &pairs );
      /* McdModelPairContainerReset( &pairs ); */
    numGoodbye += McdModelPairContainerGetGoodbyeCount( &pairs );
    numStaying += McdModelPairContainerGetStayingCount( &pairs );
    numHello += McdModelPairContainerGetHelloCount( &pairs );
  }
#endif

  printf( "\n");
  printf( "num hello: %i ",numHello );
  printf( "num stay: %i ",numStaying );
  printf( "num goodbye: %i ",numGoodbye );
  printf( "\n");
}

/*-------------------------------------------------------------------------*/
void stepEvolve()
{
    if(stopped) {
        MdtBodySetTransform(conv, (MeMatrix4Ptr) &convG->m_matrixArray);
        MdtBodySetTransform(ball, (MeMatrix4Ptr) &ballG->m_matrixArray);
        MdtBodySetTransform(box, (MeMatrix4Ptr) &boxG->m_matrixArray);
        MdtBodySetTransform(cyl1, (MeMatrix4Ptr) &cyl1G->m_matrixArray);
        MdtBodySetTransform(cyl2, (MeMatrix4Ptr) &cyl2G->m_matrixArray);
        MdtBodySetTransform(cyl3, (MeMatrix4Ptr) &cyl3G->m_matrixArray);
    }

    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge,space,world);
    MdtWorldStep(world,step);
}

/*-------------------------------------------------------------------------*/
void toggleAutoEvolve()
{
    autoEvolve = !autoEvolve;
}

/*-------------------------------------------------------------------------*/
MeReal offset = 0;

void tick(RRender*rc)
{
    int ix=0, iy=0, id=0;
    MeVector3 p;

    if ( autoEvolve )   {

        MdtBodyGetPosition(ball, p);

        if (p[1] < -30 ) {
            MdtBodySetPosition(ball, 5.2f, 5.0f, 0.01f);
            MdtBodySetLinearVelocity(ball, 0, 0, 0);
            MdtBodySetAngularVelocity(ball, 0, 0, 0);
        }

    /* if ( !MdtBodyIsEnabled(ball) ) MdtBodyEnable(ball);  */
    /* if ( !MdtBodyIsEnabled(box) ) MdtBodyEnable(box);  */
    /* if ( !MdtBodyIsEnabled(cyl) ) MdtBodyEnable(cyl);  */

        if (autoDeform_left || autoDeform_right) {

            /* deform the grid */
      if ( autoDeform_left )
        offset += 0.075f;
      else if ( autoDeform_right )
        offset -= 0.075f;

            for (iy=0; iy<iyGrid; iy++) {
                for (ix=0; ix<ixGrid; ix++) {
          id = iy*ixGrid + ix;
          data[id][2] = zval*MeSin(angHz*ix+offset);
          heights[id] = data[id][2];
                }
            }
        }

        stepEvolve();
    }
}

/*-------------------------------------------------------------------------*/
int main( int argc, const char **argv )
{
  int ix, iy, id;

  MeReal damping = 0.4f;

  MeReal x0 = -ixGrid*deltaX*0.5f;
  MeReal y0 = -iyGrid*deltaY*0.5f;

  MeMatrix4 tm_grid = {1,0,0,0, 0,0,-1,0, 0,1,0,0, 0,-2,0,1};
  MeReal radius = 0.500f;

  MeReal rcyl1 = 1.25f;
  MeReal h1   = 2*0.15f;

  MeReal rcyl2 = 0.5f;
  MeReal h2   = 2*0.5f;

  MeReal rcyl3 = 0.2f;
  MeReal h3   = 2*0.4f;

  float color[3][3] = {{1,1,0},{0,0.4f,0.6f},{0,1,1}};
  const RRenderType render = RParseRenderType(&argc,&argv);


  MeReal vertices[9][3] = {{ r0, r1, r2},
               {-r0, r1, r2},
               { r0,-r1, r2},
               {-r0,-r1, r2},
               { r0, r1,-r2},
               {-r0, r1,-r2},
               { r0,-r1,-r2},
               {-r0,-r1,-r2},
               { 0, 1.5f*r1, 0}};

  MdtBclContactParams *props;


  /* set up a grid */
#ifndef LOAD_GRID
  heights = (MeReal*) malloc(sizeof(MeReal)*ixGrid*iyGrid);
  data = (MeVector3*) malloc(sizeof(MeVector3)*ixGrid*iyGrid);


  for (iy=0; iy<iyGrid; iy++) {
    for (ix=0; ix<ixGrid; ix++) {
      id = iy*ixGrid + ix;
      data[id][0] = x0 + deltaX*ix;
      data[id][1] = y0 + deltaY*iy;
      data[id][2] = /*zval*sin(angHz*ix) + */
/*  (MeSqrt(MeSqr((ixGrid/2-ix)/ixGrid)+MeSqr((iyGrid/2-iy))/iyGrid)); */
    (MeSqrt(MeSqr(5*(ixGrid/2.0-ix)/ixGrid)+MeSqr(5*(iyGrid/2.0-iy)/iyGrid)));
      heights[id] = data[id][2];
    }
  }

#else

    McduRGHeighFieldCreateHeightMatrixFromBmp("terrain.bmp",
        0.33, 0.33, 0.33, /* z0 = */0.0,
        &heights, &ixGrid, &iyGrid);

    x0 = -ixGrid*deltaX*0.5f;
    y0 = -iyGrid*deltaY*0.5f;


/*
    grid_prim = McduRGHeighFieldCreateFromBmp(
                                "terrain2.bmp",
                                -3,  -3, 6, 6,
                                0.33, 0.33, 0.33, 0.0);
    McdRGHeightFieldGetParameters(terrainPrim,&heights,&ixGrid,&iyGrid,&deltaX,&deltaY,&xOrigin,&yOrigin);
*/
  data = (MeVector3*) malloc(sizeof(MeVector3)*ixGrid*iyGrid);
  for (iy=0; iy<iyGrid; iy++) {
    for (ix=0; ix<ixGrid; ix++) {
      id = iy*ixGrid + ix;
      data[id][0] = x0 + deltaX*ix;
      data[id][1] = y0 + deltaY*iy;
      data[id][2] = heights[id];
    }
  }
#endif

  /***********************
   * Dynamics
   ***********************/

  /* create and initialize a dynamics world  */

  world = MdtWorldCreate(NBodies ,NContacts);

  MdtWorldSetGravity(world, gravity[0],gravity[1],gravity[2]);

#ifdef CONVEX_RGHF
  conv = MdtBodyCreate(world);
  MdtBodyEnable(conv);
  MdtBodySetLinearVelocityDamping(conv, damping);
    /* MdtBodySetAngularVelocityDamping(conv, damping); */
  MdtBodySetPosition(conv, 5.2f, radius+4.1f, 0.01f);
  MdtBodySetMass(conv,1.0f);
  MdtBodySetAngularVelocity(conv, 0,0,5);
#endif

  ball = MdtBodyCreate(world);
  MdtBodyEnable(ball);
  MdtBodySetLinearVelocityDamping(ball, damping);
  MdtBodySetAngularVelocityDamping(ball, damping);
  MdtBodySetPosition(ball, 5.2f, radius+4.1f, 0.01f);
  MdtBodySetMass(ball,1.0f);

  box = MdtBodyCreate(world);
  MdtBodyEnable(box);
  MdtBodySetLinearVelocityDamping(box, damping);
  MdtBodySetAngularVelocityDamping(box, damping);
  MdtBodySetPosition(box, 2.0f, radius+4.1f, 0.01f);
  MdtBodySetMass(box, 1.0f);

  cyl1 = MdtBodyCreate(world);
  MdtBodyEnable(cyl1);
  MdtBodySetLinearVelocityDamping(cyl1, damping);
  MdtBodySetAngularVelocityDamping(cyl1, damping);
  MdtBodySetAngularVelocity(cyl1, 1,1,0);
  MdtBodySetPosition(cyl1, -1.0f, radius+4.1f, 0.01f);
  MdtBodySetMass(cyl1, 1.0f);

  cyl2 = MdtBodyCreate(world);
  MdtBodyEnable(cyl2);
  MdtBodySetLinearVelocityDamping(cyl2, damping);
  MdtBodySetAngularVelocityDamping(cyl2, damping);
  MdtBodySetAngularVelocity(cyl2, 1,1,0);
  MdtBodySetPosition(cyl2, -3.0f, radius+4.1f, 0.01f);
  MdtBodySetMass(cyl2, 1.0f);

  cyl3 = MdtBodyCreate(world);
  MdtBodyEnable(cyl3);
  MdtBodySetLinearVelocityDamping(cyl3, damping);
  MdtBodySetAngularVelocityDamping(cyl3, damping);
  MdtBodySetAngularVelocity(cyl3, 1,1,0);
  MdtBodySetPosition(cyl3, -1.0f, radius+4.1f, 3.0f);
  MdtBodySetMass(cyl3, 1.0f);

  /***********************
   * Collision Detection
   ***********************/

  /* collision initialization */

    /* ntypes = #primitives + heightfield + convex */
  geoTypeCount = McdPrimitivesGetTypeCount() + 2;
  McdInit(geoTypeCount, 100);

  McdPrimitivesRegisterTypes();
  McdConvexMeshRegisterType();
  McdRGHeightFieldRegisterType();

  McdPrimitivesRegisterInteractions();
  McdConvexMeshPrimitivesRegisterInteractions();
  McdRGHeightFieldPrimitivesRegisterInteractions();
  McdConvexMeshRGHeightFieldRegisterInteraction();

  /* create a collision space */
  space = McdSpaceAxisSortCreate(McdAllAxes,50,100, 1);
  bridge = MstBridgeCreate(10);
  MstSetWorldHandlers(world);

  /* set parameters for contacts */
  props = MstBridgeGetContactParams(bridge,
      MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

  props->type = MdtContactTypeFriction2D;
  props->friction1 = (MeReal)(0.50);
  props->friction2 = (MeReal)(0.50);
  props->restitution = (MeReal)(0.3);
  props->softness = (MeReal)(0.0001);
  props->options |= MdtBclContactOptionBounce;
  props->options |= MdtBclContactOptionSoft;

#ifdef CONVEX_RGHF
#ifndef CONVEX_LOAD
  conv_prim = (McdGeometryID) McdConvexMeshCreateHull(vertices, 6, 0.1f);
#else
  conv_prim = (McdGeometryID) McduConvexMeshCreateHullFromObj("zono.obj", 0.3, 0);
#endif
  convCM = McdModelCreate( conv_prim );
  McdSpaceInsertModel(space,convCM);
  McdModelSetBody(convCM, conv);

#endif

  ball_prim = McdSphereCreate(radius);
  ballCM = McdModelCreate( ball_prim );
  McdModelSetBody(ballCM, ball );
  McdSpaceInsertModel(space,ballCM);

  box_prim = McdBoxCreate(2*radius, 2*radius, 2*radius);
  boxCM = McdModelCreate( box_prim );
  McdModelSetBody(boxCM, box);
  McdSpaceInsertModel(space, boxCM);

  cyl1_prim = McdCylinderCreate(rcyl1, h1);
  cyl1CM = McdModelCreate( cyl1_prim );
  McdModelSetBody(cyl1CM, cyl1);
  McdModelSetRequestID( cyl1CM, 1 );
  McdSpaceInsertModel(space, cyl1CM);

  cyl2_prim = McdCylinderCreate(rcyl2, h2);
  cyl2CM = McdModelCreate( cyl2_prim );
  McdModelSetBody(cyl2CM, cyl2);
  McdModelSetRequestID( cyl2CM, 1 );
  McdSpaceInsertModel(space, cyl2CM);

  cyl3_prim = McdCylinderCreate(rcyl3, h3);
  cyl3CM = McdModelCreate( cyl3_prim );
  McdModelSetBody(cyl3CM, cyl3);
  McdModelSetRequestID( cyl3CM, 1 );
  McdSpaceInsertModel(space, cyl3CM);

#ifndef USE_PLANE
  grid_prim = McdRGHeightFieldCreate( heights, ixGrid, iyGrid,
                      deltaX, deltaY, x0, y0);
  gridCM = McdModelCreate( grid_prim );
  McdModelSetTransformPtr( gridCM, tm_grid);
  McdSpaceInsertModel(space, gridCM);
#else
  grid_prim = McdPlaneCreate();
  gridCM = McdModelCreate(grid_prim);
  McdModelSetTransformPtr( gridCM, tm_grid);
  McdUpdateModel(space,gridCM);
  McdSpaceFreezeModel(gridCM);
  McdSpaceInsertModel(space, gridCM);
#endif

  /***********************
   * Rendering
   ***********************/
  /* create a render context */
  /* rc = RNewRenderContext(render,kRQualityWireframe);   */
  rc = RNewRenderContext(render,kRQualitySmooth );

#ifdef CONVEX_RGHF
  convG = RCreateConvexMesh(rc, (McdConvexMeshID)conv_prim, color[1],
                (AcmeReal*) MdtBodyGetTransformPtr(conv));
#endif

  ballG = RCreateSphere(rc,radius,color[1], (AcmeReal*) MdtBodyGetTransformPtr(ball));
  boxG = RCreateCube(rc, 2*radius, 2*radius, 2*radius, color[1],
             (AcmeReal*) MdtBodyGetTransformPtr(box));
  cyl1G = RCreateCylinder(rc, rcyl1, h1, color[1], (AcmeReal*) MdtBodyGetTransformPtr(cyl1));
  cyl2G = RCreateCylinder(rc, rcyl2, h2, color[1], (AcmeReal*) MdtBodyGetTransformPtr(cyl2));
  cyl3G = RCreateCylinder(rc, rcyl3, h3, color[1], (AcmeReal*) MdtBodyGetTransformPtr(cyl3));

#ifndef USE_PLANE
  grid.vertexArray = data;
  grid.ixDim = ixGrid;
  grid.iyDim = iyGrid;
  gridG = RCreateGrid(rc, &grid, color[0], (MeReal*)tm_grid );
#else
  gridG = RCreateCube(rc, 40, 40,0.05f, color[0], (MeReal*)tm_grid );
#endif

  McdGetDefaultRequestPtr()->contactMaxCount = 4;
/* new version: */

  McduRequestTableInit(2, McdGetDefaultRequestPtr());

  /* start using request table */
  McdSetHelloCallback(McduHelloRequestTableCB);

  McduRequestTableGetRequestPtr(1,1)->contactMaxCount = 3;
  McduRequestTableGetRequestPtr(1,0)->contactMaxCount = 3;
  McduRequestTableGetRequestPtr(0,0)->contactMaxCount = 4;

  /* contact drawing */
#if 1
/*  McdDtBridgeSetContactCB(material, material, McduCollectMcdContacts); */
  MstBridgeSetPerPairCB(bridge, MstBridgeGetDefaultMaterial(),
      MstBridgeGetDefaultMaterial(), McduCollectContactPairs);
  McduMcdContactGraphics = McduCreateMcdContactGraphics(rc,100);
  McduToggleMcdContactDrawing();
#endif

  McdSpaceBuild(space);

  RUseKey('a', (RKeyCallback)toggleAutoEvolve);
  RUseKey('b', (RKeyCallback)toggleBenchmark);
  RUseKey('q', (RKeyCallback)toggleAutoDeform_Left);
  RUseKey('w', (RKeyCallback)toggleAutoDeform_Right);
  RUseKey('s', (RKeyCallback)toggleStopped);

  RCreateUserHelp(help, 4);

  RUseKey(' ', Shoot);

  /* set up camera */
  rc->m_cameraLookAt [0] = 0;
  rc->m_cameraLookAt [1] = -0.4f;
  rc->m_cameraLookAt [2] = -7 ;
  rc->m_cameraPos[0] = 0;
  rc->m_cameraPos[1] = 4.4f;
  rc->m_cameraPos[2] = -16;
  RUpdateCamera();

  RRun( rc, tick );
  RDeleteRenderContext( rc );

  /***********************
   * Exit and cleanup
   ***********************/

  McdTerm();

  MstBridgeDestroy(bridge);
  MdtWorldDestroy(world);

  return 0;
}


