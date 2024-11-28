/*----------------------------------------------------------------------------*/
/*
  Copyright (c) 1997-2002 MathEngine PLC

    Description:
    Intersection test of a linesegment with all collision models in space.
    Note that no interaction bewteen models.

  $Name: t-stevet-RWSpre-030110 $:  ($Id: cdLineseg.c,v 1.15.10.1 2002/04/04 15:28:51 richardm Exp $)
*/

/*----------------------------------------------------------------------------*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <McdFrame.h>         /* MathEngine Collision Toolkit: prefix Mcd */
#include <McdPrimitives.h>    /* Mcd primitive geometry types             */
#include <McdConvexMesh.h>
#include <McdRGHeightField.h>
#include <McdTriangleMesh.h>
#include <MeMath.h>

#ifdef WIN32
  #include <crtdbg.h>
    #include <windows.h>
#endif

#include <MeViewer.h>         /* MathEngine Mini-Renderer / Viewer        */
#include <McduDrawGrid.h>
#include <McduDrawConvexMesh.h>
#include <McduDrawTriangleMesh.h>
#include <McduTriangleMeshIO.h>


#define   rs 1    /* sphere radius */
#define   bd 2    /* box dimension */
#define   rcn 1   /* cone radius   */
#define   hcn 2   /* cone height   */
#define   MaxObjects 8
#define   MaxPairs 20

int       NObjects = 0;

MeReal    Points[MaxPairs][3];
int       numPts = 0;
#define   MaxElems 10

McdLineSegIntersectResult outList[MaxElems];
McdLineSegIntersectResult isectResl;

float    white[3] = {1,1,1};
float    yellow[3] = {1,1,0};
float    red[3] = {1,0,0};
MeReal   line[2][3] = {{-10,1,0}, {10,1,0}};
float    color[MaxObjects][3] = {
                               { 0.4f, 1, 0 },
                               { 1,0,0 },
                               { 0,0.4f,0.60f },
                               { 0.75f,0.5f,0 },
                               { 0.65f,1.0f,0 },
                               { 1,0.75f,0 },
                               { 1,0.5f,0 },
                               { 1,0.25f,0 }
                               };

MeVector3     *data;
MeReal        *heights;
McdGrid       grid;
int           ixGrid = 40;
int           iyGrid = 40;
MeReal        deltaX = 0.5f;
MeReal        deltaY = 0.5f;
MeReal        zval  = 0.1f;
MeReal        angHz = 1.5f;

static char *help[] =
    {
        " ",
        "The yellow cone determines the line segment direction",
        "Left Mouse Drag - move camera",
        "              i - increase line segment length",
        "              d - decrease line segment length",
        "              f - toggle pick first object / pick all objects"
    };

/*----------------------------------------------------------------------------*/
typedef struct PlBody
{
    /* PlBody() { MeMatrix4TMMakeIdentity(m_TM); } */
    McdModel* m_object;
    MeMatrix4 m_TM;
    RGraphic* m_graphic;
} PlBody;

McdGeometryID geom[MaxObjects];
PlBody        body[MaxObjects];
McdSpaceID    space;
RRender       *rc;
RGraphic      *gDirectCone;

/*----------------------------------------------------------------------------*/
MeReal dcone_r = 0.4f;
#define dcone_h (3.0f)
MeReal dcone_zos = dcone_h*0.25f;
MeReal len_line = 15.0f;
int    pick_first = 0;

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

/*----------------------------------------------------------------------------*/
void SetRotation(MeMatrix4Ptr m, MeReal q[4])
{
    MeQuaternionToTM(m, q);
}


void TogglePickFirst()
{
  pick_first = !pick_first;

  if (pick_first)
    printf("Pick object: ON\n");
  else
    printf("Pick object: OFF -- pick all objects\n");
}

void IncreaseLineSegLen()
{
  len_line += 0.2f;
  printf("Increasing line segment length\n");
}

void DecreaseLineSegLen()
{
  len_line -= 0.2f;
  printf("Decreasing line segment length\n");
}

/*----------------------------------------------------------------------------*/
void EvolveWorld()
{
  int i, numIx;
  MeReal *tmdc = gDirectCone->m_matrixArray;

  MeReal *conezaxis = &tmdc[8];
  MeReal *coneorig  = &tmdc[12];

  for (i=0; i<NObjects; i++) {
    body[i].m_graphic->m_color[2] = 0;
  }

  line[0][0] = coneorig[0] + conezaxis[0]*(dcone_h-dcone_zos);
  line[0][1] = coneorig[1] + conezaxis[1]*(dcone_h-dcone_zos);
  line[0][2] = coneorig[2] + conezaxis[2]*(dcone_h-dcone_zos);

  line[1][0] = line[0][0] + conezaxis[0]*len_line;
  line[1][1] = line[0][1] + conezaxis[1]*len_line;
  line[1][2] = line[0][2] + conezaxis[2]*len_line;

  /*--------------------------------------------------------------------------*/
  /*-------------  Intersection test LineSegment/ModelsInSpace  --------------*/

  if (pick_first) {
  /* return 0 or 1
     pick the model which is closest to the head point of the line segment
  */
    numIx = McdSpaceGetLineSegFirstIntersection(
                            space, line[0], line[1], &outList[0]);
  } else {
    numIx = McdSpaceGetLineSegIntersections(
                            space, line[0], line[1], &outList[0], MaxElems);
  }

  /*--------------------------------------------------------------------------*/

  for (i=0; i<numIx; i++ ) {
    Points[i][0] = outList[i].position[0];
    Points[i][1] = outList[i].position[1];
    Points[i][2] = outList[i].position[2];
    ((PlBody*)McdModelGetUserData(outList[i].model))->m_graphic->m_color[2] = 1;
  }

  if ( numIx > 0 ) {
    if ( !McdLineSegIntersect(outList[0].model, line[0], line[1], &isectResl) )
      printf("Inconsistent results ......\n");
  }

  numPts = numIx;
}

/*----------------------------------------------------------------------------*/
void CreateBox(RRender *rc,int i)
{
  MeReal dims[3];

  dims[0] = dims[1] = dims[2] = bd;
  geom[i] = McdBoxCreate(dims[0], dims[1], dims[2]);
  body[i].m_graphic = RCreateCube(rc, dims[0], dims[1], dims[2], color[i], 0);
    body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object = McdModelCreate(geom[i]);
  MeMatrix4TMMakeIdentity(body[i].m_TM);
}

/*----------------------------------------------------------------------------*/
void CreateSphere(RRender *rc,int i)
{
  geom[i] = McdSphereCreate(rs);
  body[i].m_graphic = RCreateSphere(rc, rs, color[i], 0);
    body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object = McdModelCreate(geom[i]);
  MeMatrix4TMMakeIdentity(body[i].m_TM);
}

/*----------------------------------------------------------------------------*/
void CreateCone(RRender *rc,int i)
{
  geom[i]   = McdConeCreate( hcn, rcn );
  body[i].m_graphic = RCreateCone(rc, rcn, 0, hcn,
                                  McdConeGetZOffset(geom[i]), color[i], 0);
    body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object  = McdModelCreate(geom[i]);
  MeMatrix4TMMakeIdentity(body[i].m_TM);
}

/*----------------------------------------------------------------------------*/
void CreateDirectionCone(RRender *rc,int i)
{
  gDirectCone = RCreateCone(rc, dcone_r, 0, dcone_h, dcone_zos, yellow, 0);
    gDirectCone->m_userData = (void*) i;
}

/*----------------------------------------------------------------------------*/
void CreateCylinder(RRender *rc,int i)
{
  geom[i] = McdCylinderCreate( rcn, hcn );
  body[i].m_graphic = RCreateCylinder(rc, rcn, hcn, color[i], 0);
    body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object  = McdModelCreate(geom[i]);
  MeMatrix4TMMakeIdentity(body[i].m_TM);
}

/*----------------------------------------------------------------------------*/
void CreatePlane(RRender *rc,int i)
{
  geom[i] = McdPlaneCreate();
  body[i].m_graphic = RCreateCube(rc, 20,20.0f,0.1f, color[i], 0);
    body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object = McdModelCreate(geom[i]);
}

MeReal *vertexPtr = 0;

/*----------------------------------------------------------------------------*/
void CreateTriMesh(RRender *rc,int i)
{
  int vertexCount;
  geom[i] = McduTriangleMeshCreateFromObj(
            "../Resources/teapot.obj", 1, 0, 0, 0.0, &vertexPtr, &vertexCount, 0);
  McdTriangleMeshBuild(geom[i]);
  body[i].m_graphic = RCreateTriangleMesh(rc, geom[i], color[i], 0);
  body[i].m_object = McdModelCreate(geom[i]);
  MeMatrix4TMMakeIdentity(body[i].m_TM);
  /* body[i].m_graphic->m_userData = (void*) i; */
}

/*----------------------------------------------------------------------------*/
/* parameters used in constructing convex objects */
  #define ROOT2  (1.414213562f)
  #define r2     (0.45f)
  #define l2     (r2*(ROOT2+1.0f))
  #define offset (0.5f*r2)
  #define r2a    (r2*1.35f)
  #define l2a    (l2*1.35f )


MeMatrix4 gTransform = { 1,0,0,0,
                         0,1,0,0,
                         0,0,1,0,
                         0,0,0,1 };


void
CreateConvex(RRender *rc,int i)
{
  MeReal vertices2[17][3] = { { l2, r2+offset, r2},
                            { r2, r2+offset, l2},
                            {-r2, r2+offset, l2},
                            {-l2, r2+offset, r2},
                            {-l2, r2+offset, -r2},
                            {-r2, r2+offset, -l2},
                            { r2, r2+offset, -l2},
                            { l2, r2+offset, -r2},
                            { l2a, 0+offset, r2a},
                            { r2a, 0+offset, l2a},
                            {-r2a, 0+offset, l2a},
                            {-l2a, 0+offset, r2a},
                            {-l2a, 0+offset, -r2a},
                            {-r2a, 0+offset, -l2a},
                            { r2a, 0+offset, -l2a},
                            { l2a, 0+offset, -r2a},
                            { 0, -4*r2+offset, 0} };


  geom[i] = (McdGeometryID) McdConvexMeshCreateHull(vertices2, 17, 0);
  body[i].m_object = McdModelCreate(geom[i]);
  body[i].m_graphic = RCreateConvexMesh( rc, (McdConvexMeshID)geom[i], color[i], 0 );
}

/*----------------------------------------------------------------------------*/
void CreateRGHF(RRender *rc,int i)
{
  int ix, iy;

  MeReal x0 = -ixGrid*deltaX*0.5f;
  MeReal y0 = -iyGrid*deltaY*0.5f;

  data = (MeVector3*) MeMemoryAPI.create(sizeof(MeVector3)*ixGrid*iyGrid);
  heights = (MeReal*) MeMemoryAPI.create(sizeof(MeReal)*ixGrid*iyGrid);

  /* set up a grid */
  for (iy=0; iy<iyGrid; iy++) {
    for (ix=0; ix<ixGrid; ix++) {
      int id = iy*ixGrid + ix;
      data[id][0] = x0 + deltaX*ix;
      data[id][1] = y0 + deltaY*iy;
      data[id][2] = zval*MeSin(angHz*ix);
      heights[id] = data[id][2];
    }
  }


  grid.vertexArray = data;
  grid.ixDim = ixGrid;
  grid.iyDim = iyGrid;

  body[i].m_graphic = RCreateGrid(rc, &grid, color[i], 0 );

  geom[i]= McdRGHeightFieldCreate( heights, ixGrid, iyGrid,
                                              deltaX, deltaY, x0, y0);

  body[i].m_object = McdModelCreate(geom[i]);
  MeMatrix4TMMakeIdentity(body[i].m_TM);

  /* body[i].m_graphic->m_userData = (void*) i; */
}
/*----------------------------------------------------------------------------*/
void McuDrawLineSeg()
{
  glLineWidth(3);
  glBegin(GL_LINES);
        glVertex3f( line[0][0], line[0][1], line[0][2]);
        glVertex3f( line[1][0], line[1][1], line[1][2]);
  glEnd();
  glLineWidth(1);
}

/*----------------------------------------------------------------------------*/
void McuDrawPoints()
{
  int i;
    for ( i=0; i< numPts; i++) {
        glBegin(GL_POINTS);
            glVertex3f( Points[i][0], Points[i][1], Points[i][2]);
        glEnd();
    }
}

/*----------------------------------------------------------------------------*/
void CreateWorld(RRender *rc)
{
  int i;
  MeReal q[4];
  MeReal *tmdc, *conezaxis, *coneorig;
  MeReal ang, angDelta, x, z, radius = 5.0f;

  McdInit( McdPrimitivesGetTypeCount() + 4, 100 );

  McdPrimitivesRegisterTypes();
  McdRGHeightFieldRegisterType();
  McdTriangleMeshRegisterType();
  McdConvexMeshRegisterType();

  McdPrimitivesRegisterInteractions();
  McdTriangleMeshRegisterInteractions();
  McdRGHeightFieldPrimitivesRegisterInteractions();
  McdConvexMeshPrimitivesRegisterInteractions();

  space = McdSpaceAxisSortCreate(McdAllAxes, MaxObjects, MaxPairs, 1);

  NObjects = 0;

  CreatePlane(rc,NObjects++);
  CreateRGHF(rc,NObjects++);
  CreateBox(rc,NObjects++);
  CreateSphere(rc,NObjects++);
  CreateCone(rc,NObjects++);
  CreateCylinder(rc,NObjects++);
  CreateConvex(rc,NObjects++);
  CreateTriMesh(rc,NObjects++);

  angDelta = ME_PI*2/(MeReal)(NObjects-2);

  for(i = 0; i < NObjects; i++) {

    McdSpaceInsertModel(space, body[i].m_object);
    McdModelSetUserData(body[i].m_object, (void*) &body[i]);
    McdModelSetTransformPtr(body[i].m_object, (MeMatrix4Ptr)body[i].m_graphic->m_matrixArray);

    if (i<2)
      MeMatrix4TMSetPosition((MeMatrix4Ptr)body[i].m_graphic->m_matrixArray, 0, (float)(i-3), 0);
    else {
      ang = angDelta*(i-2);
      x = radius*sin(ang);
      z = radius*cos(ang);
      MeMatrix4TMSetPosition((MeMatrix4Ptr)body[i].m_graphic->m_matrixArray, x, 4, z);
    }

    SetAxisAngle(q, 1, 0, 0, (MeReal)ME_PI/2.0f) ;
    SetRotation( (MeMatrix4Ptr)body[i].m_graphic->m_matrixArray, q );
  }

  // McdSpaceRemoveModel( body[NObjects-2].m_object );

  CreateDirectionCone(rc, NObjects);
  gDirectCone->m_matrixArray[12] = 11;
  gDirectCone->m_matrixArray[13] = 4;
  SetAxisAngle(q, 0, 1, 0, (MeReal)ME_PI/2.0f);
  SetRotation( (MeMatrix4Ptr)gDirectCone->m_matrixArray, q );

  tmdc = gDirectCone->m_matrixArray;
  conezaxis = &tmdc[8];
  coneorig  = &tmdc[12];

  line[0][0] = coneorig[0] + conezaxis[0]*(dcone_h-dcone_zos);
  line[0][1] = coneorig[1] + conezaxis[1]*(dcone_h-dcone_zos);
  line[0][2] = coneorig[2] + conezaxis[2]*(dcone_h-dcone_zos);

  line[1][0] = line[0][0] + conezaxis[0]*len_line;
  line[1][1] = line[0][1] + conezaxis[1]*len_line;
  line[1][2] = line[0][2] + conezaxis[2]*len_line;

  numPts = 0;

  RCreateUserProceduralObject( rc,
                   (RproceduralObjectCallback)McuDrawLineSeg, 0, "seg", 1, white, 0);

  RCreateUserProceduralObject( rc,
                   (RproceduralObjectCallback)McuDrawPoints, 0, "seg", 1, red, 0);
}

/*----------------------------------------------------------------------------*/
void Click(int button, int state, int x, int y, RGraphic* graphic)
{
  if (graphic) {
    /* fprintf(stderr,"\nindex: %d\n", (int) graphic->m_userData); */
  }
}

/*----------------------------------------------------------------------------*/
void Tick(RRender* rc)
{
  EvolveWorld();
}

/*----------------------------------------------------------------------------*/
void
cleanup()
{
  int i;
  /* graphics */
  RDeleteRenderContext(rc);

  for (i=0; i<NObjects; i++) {
    McdGeometryDestroy(geom[i]);
    McdModelDestroy(body[i].m_object);
  }

  McdSpaceDestroy(space);
  McdTerm();

  MeMemoryAPI.destroy(vertexPtr);
  MeMemoryAPI.destroy(data);
  MeMemoryAPI.destroy(heights);

}

/*----------------------------------------------------------------------------*/

int main(int argc, const char **argv)
{
    const RRenderType render = RParseRenderType(&argc,&argv);

  const int helpNum = sizeof (help) / sizeof (help[0]);


#if defined WIN32 && defined _DEBUG && 1
    int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

    debugFlag |= _CRTDBG_ALLOC_MEM_DF;
    debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
    debugFlag |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(debugFlag);
#endif

  rc = RNewRenderContext(render, kRQualitySmooth);

    CreateWorld(rc);

    rc->m_cameraOffset = 20;
    RUpdateCamera();

  RUseKey('i', (RKeyCallback) IncreaseLineSegLen);
  RUseKey('d', (RKeyCallback) DecreaseLineSegLen);
  RUseKey('f', (RKeyCallback) TogglePickFirst);

    RMouseFunc(Click);

  atexit((RKeyCallback) cleanup);

  RCreateUserHelp(help, helpNum);
  rc->m_useDisplayLists = 0;    /* see the color changes */
    RRun( rc, Tick );
    RDeleteRenderContext( rc );

    return 0;
}
/*----------------------------------------------------------------------------*/
