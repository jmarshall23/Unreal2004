/*
  Copyright (c) 1997-2002 MathEngine PLC
  $Name: t-stevet-RWSpre-030110 $ $RCSfile: nfBench.cpp,v $
*/
/*
  @file nfBench.cpp

  Nearfield Benchmark. No physics is used.

Invoque for example as

nfBench -box -cylinder -depth 0.1 -visual -rep 10 -angle 10
to see the behaviour

and
nfBench -box -cylinder -depth 0.1
for benchmark

mesh example:

nfBench -i -trimesh -box -trimesh -sphere -angle 10

-trimesh ../Resources/teapot.obj -trimesh ../Resources/teapot.obj -angle 20 -rep 20 -depth  -1


User specifies two objects of any type (currently: box, sphere, cylinder).

First the second object is moved into position so that its depth
is the depth specified (relative to size of object2) using a binary
search approach. The first object remains at the origin.

Next, the nearfield is tested without moving -rep times.

Finally, the first object is rotated by -angle degrees.

It should be possible to time either the binary search approach
(-approach)
or the static NF test repetition (default).

Next step: add timing functions for both and optionally remove graphics.

*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <assert.h>

#if defined(WIN32)
#include <malloc.h>
#elif !defined(PS2)
#include <alloca.h>
#endif

#include <McdFrame.h>       /* MathEngine Collision Toolkit: prefix Mcd */
#include <McdPrimitives.h>  /* Mcd primitive geometry types             */
#include <McdConvexMesh.h>
#include <McdTriangleMesh.h>
#include <McdRGHeightField.h>
#include <MeViewer.h>
#include "MeViewerTimer.h"
#include <MeMath.h>
#include "McduMeshSphere.h"

#include <McduTriangleMeshIO.h>

#include <McduDrawTriangleMesh.h>
#include "McdDistanceResult.h"

int benchDistance = 0;

const int NObjects = 2;

float color[10][3] = { {0.4f, 1, 0}, {0,1,1},{0,0.4f,0.60f},{1,0,1} };
float white[3] = {1,1,1};

struct Body
{
  Body() {
    // MeMatrix4TMMakeIdentity(&m_TM);
    // m_position = &m_TM[3][0];
  }

  McdModel* m_object;
  MeMatrix4Ptr m_TM;
  MeReal* m_position;
  MeReal m_velocity[3];
  RGraphic* m_graphic;
};

Body body[NObjects];
McdSpaceID space;

inline MeReal Rand(const MeReal min, const MeReal max) {
  return min + (max-min) * rand() / RAND_MAX;
}

const MeReal DEG_TO_RAD = ME_PI / 180;

inline void GenerateRandomOrientationQ(MeReal* o) {
  do {
    o[1] = Rand(-1, 1);
    o[2] = Rand(-1, 1);
    o[3] = Rand(-1, 1);

    o[0] = o[1]*o[1] + o[2]*o[2] + o[3]*o[3];
  } while (o[0] > 1);

  MeReal s = MeSqrt(o[0]*o[0] + o[0]);
  int i;
  for(i = 0; i < 4; i++) o[i] /= s;
}

inline void QuaternionToMatrix44(MeMatrix4Ptr matrix, const MeReal* quaternion) {
  // only changes Matrix33 part of Matrix44

  // copied from MathUtils.cpp
  //   Dla::ToMatrix(CMeMatrix44* mout, const CMeQuaternion& orient, const CMeVector3& pos)
  const MeReal qz = quaternion[0];
  const MeReal q1 = quaternion[1];
  const MeReal q2 = quaternion[2];
  const MeReal q3 = quaternion[3];

  (matrix)[0][0] = 2 * (qz*qz + q1*q1) - 1;
  (matrix)[1][1] = 2 * (qz*qz + q2*q2) - 1;
  (matrix)[2][2] = 2 * (qz*qz + q3*q3) - 1;
  (matrix)[0][1] = 2 * (q1*q2 - qz*q3);
  (matrix)[1][0] = 2 * (q1*q2 + qz*q3);
  (matrix)[0][2] = 2 * (q1*q3 + qz*q2);
  (matrix)[2][0] = 2 * (q1*q3 - qz*q2);
  (matrix)[1][2] = 2 * (q2*q3 - qz*q1);
  (matrix)[2][1] = 2 * (q2*q3 + qz*q1);
}

inline void SetRandomOrientation(MeMatrix4Ptr matrix44) {
  MeReal quaternion[4];
  GenerateRandomOrientationQ(quaternion);
  QuaternionToMatrix44(matrix44, quaternion);
}

inline void SetZero(MeReal* v, const int n) {
  for(int i = 0; i < n; i++) v[i] = 0;
}

inline MeReal Dot(const MeReal* a, const MeReal* b) {
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

inline void SubtractVec(MeReal* a, const MeReal* b, const MeReal* c) {
  for(int i = 0; i < 3; i++) a[i] = b[i] - c[i];
}

inline void MultiplyAddVec(MeReal* a, const MeReal* b, const MeReal x, const MeReal* c) {
  for(int i = 0; i < 3; i++) a[i] = b[i] + x * c[i];
}

/*----------------------------------------------------------------------------*/
inline void SetPosition(MeMatrix4Ptr m, MeReal x, MeReal y, MeReal z)
{
    (m)[3][0] = x; (m)[3][1] = y; (m)[3][2] = z;
}

/*----------------------------------------------------------------------------*/
void Normalize(MeReal *v) {
    MeReal norm = 1.0f/(MeReal)sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    v[0] *= norm;
    v[1] *= norm;
    v[2] *= norm;
}

/*-----------------------------------------------------------------------------*/
void SetAxisAngle(MeReal *q, const MeReal nX, const MeReal nY,
                  const MeReal nZ, MeReal angle)
{
    MeReal s_2 = -(MeReal)sin(0.5f*angle);
    q[1] = nX;
    q[2] = nY;
    q[3] = nZ;
    Normalize(q+1);
    q[0] = (MeReal)cos(0.5f*angle);
    q[1] *= s_2;
    q[2] *= s_2;
    q[3] *= s_2;
}

/*----------------------------------------------------------------------------*/
void CreateBox(RRender *rc,int i, MeReal* dims)
{
  // MeReal dims[3];
  McdBoxID box;

  // dims[0] = dims[1] = dims[2] = 1;
  box = McdBoxCreate(dims[0], dims[1], dims[2]);
  body[i].m_graphic = RCreateCube(rc, dims[0], dims[1], dims[2], color[i], 0);
  body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object = McdModelCreate(box);
  body[i].m_TM = (MeVector4*) &body[i].m_graphic->m_matrixArray;

  McdModelSetTransformPtr( body[i].m_object,body[i].m_TM );

}

/*----------------------------------------------------------------------------*/
void CreateSphere(RRender *rc,int i, MeReal rs)
{
  McdSphereID geom = McdSphereCreate(rs);
  body[i].m_graphic = RCreateSphere(rc, rs, color[i], 0);
  body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object = McdModelCreate(geom);
  body[i].m_TM = (MeVector4*) &body[i].m_graphic->m_matrixArray;
  McdModelSetTransformPtr( body[i].m_object,body[i].m_TM );
}

/*----------------------------------------------------------------------------*/
void CreateCone(RRender *rc,int i, MeReal hcn, MeReal rcn)
{
  McdConeID geom    = McdConeCreate( hcn, rcn );
  body[i].m_graphic = RCreateCone(rc, rcn, 0, hcn, McdConeGetZOffset(geom), color[i], 0);
    body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object  = McdModelCreate(geom);
  body[i].m_TM = (MeVector4*) &body[i].m_graphic->m_matrixArray;
  McdModelSetTransformPtr( body[i].m_object,body[i].m_TM );
}

/*----------------------------------------------------------------------------*/
void CreateCylinder(RRender *rc,int i, MeReal hcn, MeReal rcn)
{
  McdCylinderID geom    = McdCylinderCreate(rcn, hcn);
  body[i].m_graphic = RCreateCylinder(rc, rcn, hcn, color[i], 0);
    body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object  = McdModelCreate(geom);
  body[i].m_TM = (MeVector4*) &body[i].m_graphic->m_matrixArray;
  McdModelSetTransformPtr( body[i].m_object,body[i].m_TM );
}

/*----------------------------------------------------------------------------*/
void CreatePlane(RRender *rc,int i)
{
  MeReal q[4];
  SetAxisAngle(q, 1, 0, 0, -(MeReal)ME_PI/2.0f) ;
  MeMatrix4Ptr mg = (MeMatrix4Ptr) malloc( sizeof(MeMatrix4) );
  MeMatrix4TMMakeIdentity( mg );
  QuaternionToMatrix44( mg, q );
  SetPosition( mg, 0, -2, 0);

  McdPlaneID geom = McdPlaneCreate();
  body[i].m_graphic = RCreateCube(rc, 20,20.0f,0.1f, color[i], (MeReal*)mg);
  body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object = McdModelCreate(geom);
  body[i].m_TM = (MeVector4*) &body[i].m_graphic->m_matrixArray;
  McdModelSetTransformPtr( body[i].m_object,body[i].m_TM );
}

const MeReal bdim = 1.0f;

MeVector3 vertex[8] = {{bdim,bdim,-bdim}, {-bdim,bdim,-bdim},
               {-bdim,bdim,bdim}, {bdim,bdim,bdim},
               {bdim,-bdim,-bdim}, {-bdim,-bdim,-bdim},
               {-bdim,-bdim,bdim}, {bdim,-bdim,bdim} };

MeVector3 svertex[8];

/*--------------------------------------------------------------------------*/
void CreateTriMesh(RRender *rc,int i, const char* filename, MeReal s)
{
  MeReal *vertexPtr;
  int vertexCount = 0;

  McdGeometryID geom = 0;
  if (strcmp(filename,"-box")==0) {
    geom = McdTriangleMeshCreate(12);

    McdTriangleMeshAddTriangle( geom, vertex[0], vertex[1], vertex[2]);
    McdTriangleMeshAddTriangle( geom, vertex[0], vertex[2], vertex[3]);
    McdTriangleMeshAddTriangle( geom, vertex[1], vertex[6], vertex[2]);
    McdTriangleMeshAddTriangle( geom, vertex[1], vertex[5], vertex[6]);
    McdTriangleMeshAddTriangle( geom, vertex[5], vertex[7], vertex[6]);
    McdTriangleMeshAddTriangle( geom, vertex[5], vertex[4], vertex[7]);
    McdTriangleMeshAddTriangle( geom, vertex[4], vertex[3], vertex[7]);
    McdTriangleMeshAddTriangle( geom, vertex[4], vertex[0], vertex[3]);
    McdTriangleMeshAddTriangle( geom, vertex[1], vertex[0], vertex[4]);
    McdTriangleMeshAddTriangle( geom, vertex[1], vertex[4], vertex[5]);
    McdTriangleMeshAddTriangle( geom, vertex[2], vertex[7], vertex[3]);
    McdTriangleMeshAddTriangle( geom, vertex[2], vertex[6], vertex[7]);
  }
  else if (strcmp(filename,"-sphere")==0)
    {
      void *shpereData = 0;
      geom = makeMeshSphere(s, 3, 1, &shpereData, McdTriangleMeshOptionDistanceAllowed );
    }
  else
    {
	  printf("loading %s\n", filename);
      geom =  McduTriangleMeshCreateFromObj( filename, 
					     s   /* scale */, 
					     0   /* recenter */, 
					     1   /*merge*/, 
					     1e-7 /*eps*/, 
					     &vertexPtr,&vertexCount, 
					     benchDistance?McdTriangleMeshOptionDistanceAllowed:McdTriangleMeshOptionNoDistance);
    }
  McdTriangleMeshBuild(geom);
  body[i].m_object  = McdModelCreate(geom);

  // RGraphic       *MEAPI MeAppCreateTriangleMeshGraphicFromModel(MeApp
                                      // *app,McdModelID model,float color[4]);
  body[i].m_graphic =
    RCreateTriangleMesh(rc, geom, color[i], 0);

  // body[i].m_graphic->m_userData = (void*) i;
  body[i].m_TM = (MeVector4*) &body[i].m_graphic->m_matrixArray;
  McdModelSetTransformPtr( body[i].m_object,body[i].m_TM );
}


/*--------------------------------------------------------------------------*/

int autoEvolve = 1;

void toggleAutoEvolve() // (...)
{
  autoEvolve = !autoEvolve;
}

int stepPos_binSearch();
int stepPos_translate();

//---------------------------------------------------------------------

// Simulation / timing parameters
// model[1] will be translated to correct position
// model[0] will be rotated by stepAngle, and stays at origin

McdModelID model[3];

// change at sim time:
MeReal startX,endX;
MeReal stepX;
MeReal angle = 0;
MeReal quat[4] = {0,1,0,0}; // quat for model[0] rotation

// parameters
int repCount = 100;
McdModelPair p;
const MeReal Xeps = 1e-3; // stop binary search

MeReal depth = 0;
MeReal stepAngle = 1.0; // deg
int stepAngleCount = 360; // num of times to step
MeReal radius[2]; // radii of BS of models



int visual = 0;
int approach = 0;
int colliding = 0;

void collideInit()
{
  //McdModelPairInit(body[0],body[1]);
//    p.model1 = body[0].m_object;
//    p.model2 = body[1].m_object;
//    p.pairData = 0;
  McdModelPairReset(&p, body[0].m_object, body[1].m_object);
  McdHello(&p);
}

inline void position(MeReal x)
{
  SetPosition(body[1].m_TM, x, 0, 0);
}

inline MeReal getPos()
{
  return (body[1].m_TM)[3][0];
}

int collide()
{
  if (benchDistance) {
    McdDistanceResult r;
    McdTriangleMeshTriangleMeshDistance(&p,&r);
    return colliding = (r.distanceLB<=0.0);
  }
  else 
  {
    McdContact contacts[10];
    McdIntersectResult r;
    r.contactMaxCount = 10;
    r.contacts = contacts;
    
    bool ok = McdIntersect(&p,&r);
    assert(ok);
    return colliding = r.touch;
  }
}

// prepare for binary search
void initPos()
{
  MeVector3 center;
  MeMatrix4TMMakeIdentity(body[0].m_TM);
  MeMatrix4TMMakeIdentity(body[1].m_TM);

  McdModelGetBSphere( body[0].m_object, center, &radius[0] );
  McdModelGetBSphere( body[1].m_object, center, &radius[1] );

  fprintf(stderr, "Rad = %g,  %g\n", (float)radius[0], (float)radius[1]);

  startX = (MeReal)(0.0);
  endX = radius[0] + radius[1];
  endX *= 1.01;
  stepX = ( endX - startX ) / (repCount);

  depth = radius[1] * depth; // convert depth to relative depth

  collideInit();
  position(startX);
  // assert( collide() );
  position(endX);
  assert( !collide() );
}

void startPos()
{
  startX = (MeReal)(0.0);
  endX = radius[0] + radius[1];
  endX *= 1.01;
  stepX = ( endX - startX ) / (repCount);

  position(endX);

}

void stepRotate()
{
  angle += (stepAngle*DEG_TO_RAD);
  SetAxisAngle(quat, 0,1,0, angle);
  QuaternionToMatrix44(body[0].m_TM,quat);
}

int stepStatic()
{
  static int step = 0;
  collide();
  step++;
  if (step == repCount) { step = 0; return 0;}
  return 1;
}

void lastStep()
{
    position(getPos()- depth);
}

int stepPos_binSearch()
{
  if ( MeFabs(startX - endX) < Xeps )
    {
      lastStep();
      return 0; // stop
    }
  MeReal midX = ( startX + endX ) / 2.;
  position(midX);
  if (collide()) { startX = midX; }
  else { endX = midX; }
  return 1; // continue
}

int stepPos_translate()
{
  endX -= stepX;
  position(endX);
  if (collide())
    {
      lastStep();
      return 0;
    }

  return 1;
}


void parseArgs(RRender *rc, int argc, const char **argv)
{

  char Usage[] =
"Usage: bench_nf [geometry1] [geometry2] [options]\nOptions:\n"
"-sphere -box -cylinder of size 1\n"
"-rep number_of_repetitions_to_time(200)\n"
"-depth relative_depth at which approach terminates (0)\n"
"-angle rotation_angle_degrees (1deg)\n"
"-arep number_of_times_to_rotate_by_angle (360/angle)\n"
"-approach  : time binary search approach, otherwise static position at depth\n\n"
"Sample runs:\n"
" nfBench -box -cylinder -depth 0.1\n"
" nfBench -box -cylinder -depth 0.1 -visual -step 10 -angle 10   (for graphics)\n";

  // Parse Command Args
  double real;
  MeReal scale = 1;
  MeReal dims[3] = {2,2,2};

  int i = 0;
  int obj =0;
  if (argc == 1) {
    printf("%s: %s\n",argv[0],Usage);
  } else {
    while (++i < argc) {
      
      if (strcmp(argv[i],"-s")==0 || strcmp(argv[i],"-scale")==0 ) 
	{ sscanf(argv[++i],"%lf",&real); scale = real; 
	}

      else if (strcmp(argv[i],"-sphere")==0) { CreateSphere(rc,obj++,scale*dims[0]/2); scale = 1.0f; }
      else if (strcmp(argv[i],"-box")==0) { 
	MeReal dims2[3]; 
	dims2[0] = dims[0]*scale; dims2[1] = dims[1]*scale; dims2[2] = dims[2]*scale; 
	CreateBox(rc,obj++,dims2); scale = 1;  }

      else if (strcmp(argv[i],"-cylinder")==0) { CreateCylinder(rc,obj++,dims[0],dims[1]/2); scale = 1.0f; }


      /*
      else if (strcmp(argv[i],"-convexBox")==0) 
	{ 
		CreateBox(rc,obj++,dims); 
		// register GJK replacement for box-box, etc
		// McdConvexMeshRegisterBoxAndSphereFns();
		scale = 1.0f;
	}
      */


      else if (strcmp(argv[i],"-trimesh")==0) { 
	 CreateTriMesh(rc,obj++, argv[i+1],scale);
	 i++;
	 scale = 1.0f;
      }

      else if (strcmp(argv[i],"-nofit")==0) { 
	extern int gMcdTriangleMeshFitTighter;
	 gMcdTriangleMeshFitTighter = 0;
      }

      else if (strcmp(argv[i],"-fit")==0) { 
	extern int gMcdTriangleMeshFitTighter;
	 gMcdTriangleMeshFitTighter = 1;
      }

      else if (strcmp(argv[i],"-distance")==0) { 
	benchDistance = 1;
      }

      else if (strcmp(argv[i],"-convex")==0) { 
	//	 CreateConvexMesh(rc,obj++, argv[i+1],scale);
	 i++;
	 scale = 1.0f;
      }


      else if (strcmp(argv[i],"-visual")==0) { visual = 1; }
      else if (strcmp(argv[i],"-i")==0) { visual = 1; }
      else if (strcmp(argv[i],"-approach")==0) { approach = 1; }

      else if (strcmp(argv[i],"-depth")==0) 
	{ sscanf(argv[++i],"%lf",&real); depth = real; 
	  printf("Depth = %lg\n", real);
	}

      else if (strcmp(argv[i],"-angle")==0) 
	{ sscanf(argv[++i],"%lf",&real); stepAngle = real; 
	  printf("stepAngle = %lg\n", real);
	  stepAngleCount = (int) floor(360.0 / stepAngle);
	}

      else if (strcmp(argv[i],"-arep")==0)
        { sscanf(argv[++i],"%d",&stepAngleCount);
          //printf("stepAngleCount = %d\n", stepAngleCount);
        }

      else if (strcmp(argv[i],"-rep")==0) 
	{ 
	  sscanf(argv[++i],"%d",&repCount);
	  printf("Repetitions = %d\n", repCount);
	}


      if (obj>2) { printf("Only 2 objects allowed.\n"); exit(1);}
    }
  }

  printf("Number of rotation steps = %d\n", stepAngleCount);

  if (obj < 2)
    CreateBox(rc, obj++, dims);
  if (obj < 2)
    CreateSphere(rc, obj++, 1);

}

///////////////////////////////////////////////////////////////////////

int test = 0;
int phase = 0; // 0 = bin search approach, 1 = static

void Tick(RRender* rc)
{
  if(autoEvolve==1) {
    test++;
    // time binary search
    if (0==phase && !stepPos_binSearch()) {phase = 1;}
    // end time binary search

    // time static test
    if (1==phase && !stepStatic()) {phase = 0; stepRotate(); startPos(); }
    // end time static test
  }

  // to be commented out:
  // autoEvolve--; if (autoEvolve == 0) autoEvolve = 10;
  if (colliding)
    body[0].m_graphic->m_color[0] = 1.0; else
    body[0].m_graphic->m_color[0] = 0.0;
}


void run_bench_binSearch(int iter)
{

    int i,j;
        Timer *t = CreateTimer();
    int test = 0;

        StartTimer(t);
        for (i = 0; i < iter; i++)
        {
       for (j = 0; j< repCount; j++) {
        if (!stepPos_binSearch() ) { startPos(); }
        test++;
       }
       stepRotate(); startPos();
    }

    StopTimer(t);

        printf("%4.6f ms\n", 1000.0*DeltaTime(t)/ test );

        DestroyTimer(t);
}

void run_bench_static(int iter)
{

    int i,j;
    int test =0;
        Timer *t = CreateTimer();
    double totalTime = 0;

        for (i = 0; i < iter; i++)
        {
           while (stepPos_binSearch());

           StartTimer(t);
       for (j = 0; j < repCount; j++) {
        collide();
        test++;
       }
           StopTimer(t);
       totalTime += DeltaTime(t);

       stepRotate();
           startPos();
        }

        printf("%4.6f ms\n", 1000.0*totalTime/ (iter*repCount) );

        DestroyTimer(t);
}



int main(int argc, const char **argv)
{

  const RRenderType render = RParseRenderType(&argc,&argv);

  // srand(Seed);

  RRender* rc = RNewRenderContext(render, kRQualityWireframe);

  McdInit( McdPrimitivesGetTypeCount()+10, 100 );

  McdPrimitivesRegisterTypes();
  McdConvexMeshRegisterType();
  McdRGHeightFieldRegisterType();

  McdPrimitivesRegisterInteractions();

  McdConvexMeshPrimitivesRegisterInteractions();
  McdRGHeightFieldPrimitivesRegisterInteractions();
  McdConvexMeshRGHeightFieldRegisterInteraction();

  McdTriangleMeshRegisterType();
  McdTriangleMeshTriangleMeshRegisterInteraction();

  rc->m_cameraOffset = 20;
  RUpdateCamera();

  parseArgs(rc,argc,argv);
  MeMatrix4TMMakeIdentity(body[0].m_TM);
  MeMatrix4TMMakeIdentity(body[1].m_TM);
  initPos();

  if (!visual)
    {
    printf("Number of iterations: %d\n",stepAngleCount);
    if (approach)
        run_bench_binSearch(stepAngleCount);
    else
        run_bench_static(stepAngleCount);
      exit(0);
    }

  RUseKey('a', (RKeyCallback)toggleAutoEvolve);
  RRun( rc, Tick );
  RDeleteRenderContext( rc );

  // Term ...
  return 0;
}
