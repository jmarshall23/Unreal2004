/*
Copyright (c) 1997-2002 MathEngine PLC
$Name: t-stevet-RWSpre-030110 $ $RCSfile: cdAlone.cpp,v $
*/
/*
@file CollisionAlone.cpp
Example of cubes reflecting off the boundary of a large cube.
No physics is used.
*/

#include <stdlib.h>
#include <math.h>

#if defined(WIN32)
#include <malloc.h>
#elif !defined(PS2)
#include <alloca.h>
#endif

#include <McdFrame.h>       /* MathEngine Collision Toolkit: prefix Mcd */
#include <McdPrimitives.h>  /* Mcd primitive geometry types             */
#include <MeViewer.h>
#include <MeMemory.h>
#include <MeMath.h>

#ifdef PS2

/* Profiling Stuff */
#include <MeProfile.h>
int fr;
#define FRAMES (1000)
const MePHWTimerMode hwtmode = {
  2,
    MEPHWT_CACHE
};
#endif
#include <stdio.h>

#define JAGGEDNESS 0.2f
#define SETTLETIME 15

/* End of Profiling Stuff */


const unsigned int Seed = 1;

const int NFreeObjects = 60;
const int NObjects = 66; // 6 + NFreeObjects;
const int SomeNumber = 4;
const int NPairs = 1000;

const MeReal CubeDimMin[3] = {0.5f, 0.5f, 0.5f};
const MeReal CubeDimMax[3] = {1.f, 1.f, 1.f};
const MeReal PosMax[3] = {3, 3, 3};
const MeReal VelMax[3] = {5, 5, 5};
const MeReal K = 2;
const MeReal WorldLimits[3] = {PosMax[0] + K, PosMax[1] + K, PosMax[2] + K};

short cubeType;
short planeType;

struct PlBody
{
  PlBody() {
    MeMatrix4TMMakeIdentity(m_TM);
    m_position = &m_TM[3][0];
  }

  McdModel* m_object;
  MeMatrix4 m_TM;
  MeReal* m_position;
  MeReal m_velocity[3];
};

PlBody body[NObjects];
McdSpaceID space;
McdModelPairContainer *pairs;

inline MeReal Rand(const MeReal min, const MeReal max) {
  return min + (max-min) * rand() / RAND_MAX;
}

inline void SetRandomVelocity(MeReal* v) {
  v[0] = Rand(-VelMax[0], VelMax[0]);
  v[1] = Rand(-VelMax[1], VelMax[1]);
  v[2] = Rand(-VelMax[2], VelMax[2]);
}

inline void SetRandomDimensions(MeReal* d) {
  d[0] = Rand(CubeDimMin[0], CubeDimMax[0]);
  d[1] = Rand(CubeDimMin[1], CubeDimMax[1]);
  d[2] = Rand(CubeDimMin[2], CubeDimMax[2]);
}

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

inline void SetRandomOrientation(MeMatrix4Ptr matrix44) {
  MeReal quaternion[4];
  GenerateRandomOrientationQ(quaternion);
  MeQuaternionToTM( matrix44, quaternion);
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

int HandleCubeCubeCollision(McdModelPair *pair) {
  PlBody* b1 = (PlBody*) McdModelGetUserData(pair->model1);
  PlBody* b2 = (PlBody*) McdModelGetUserData(pair->model2);

  MeReal* v1 = b1->m_velocity;
  MeReal* v2 = b2->m_velocity;

  MeReal dirn[3];
  MeVector3Subtract( dirn, v1,  v2);
  MeVector3Normalize(dirn);

  MeReal k = -2 * MeVector3Dot(v1, dirn);
  MultiplyAddVec(v1, v1, k, dirn);

  k = -2 * MeVector3Dot(v2, dirn);
  MultiplyAddVec(v2, v2, k, dirn);

  return 1; // what does this mean?
}

int HandleCubePlaneCollision(McdModelPair *pair) {
  PlBody* b1 = (PlBody*) McdModelGetUserData(pair->model1);
  PlBody* b2 = (PlBody*) McdModelGetUserData(pair->model2);

  MeReal normal[3] = {-b2->m_position[0], -b2->m_position[1], -b2->m_position[2]};
  MeVector3Normalize(normal);

  MeReal* v = b1->m_velocity;
  MeReal k = -2 * MeVector3Dot(v, normal);
  if(k > 0) {
    MultiplyAddVec(v, v, k, normal);
  }

  return 1;
}

void EvolveWorld()
{
  McdModelPair *p;
  MeI16 t1, t2;
  const MeReal step = 0.0167f;
  int i;
  MeBool pairOverflow;
  McdIntersectResult interact;
  McdContact contact[20];
  McdSpacePairIterator spaceIter;

    //  MeProfileStartFrame();

  for(i = 0; i < NObjects; i++)
    MultiplyAddVec(body[i].m_position, body[i].m_position, step, body[i].m_velocity);

  McdSpaceUpdateAll(space);
  McdSpacePairIteratorBegin(space, &spaceIter);
  interact.contacts = contact;
  interact.contactMaxCount = 20;

  McdSpaceEndChanges(space);
  do {
    McdModelPairContainerReset(pairs);
    pairOverflow = McdSpaceGetPairs(space, &spaceIter, pairs);
    McdGoodbyeEach(pairs);
    McdHelloEach(pairs);

    for( i = pairs->helloFirst ; i < pairs->stayingEnd ; ++i )
    {
      p = pairs->array[i];

      if(!McdSpaceModelIsFrozen(p->model1) || !McdSpaceModelIsFrozen(p->model2)) {

        McdIntersect(p, &interact);
        if(interact.touch) {
          t1 = McdGeometryGetTypeId(McdModelGetGeometry(p->model1));
          t2 = McdGeometryGetTypeId(McdModelGetGeometry(p->model2));
          if (t1!=t2)
            HandleCubePlaneCollision(pairs->array[i]);
          else if(t1 == kMcdGeometryTypeBox)
            HandleCubeCubeCollision(p);
        }
      }
    }
  } while(pairOverflow);
  McdSpaceBeginChanges(space);

//  MeProfileEndFrame();

#ifdef PS2
  if(fr++==FRAMES) {
    const MePOutputStyle os = MEPOS_DUMP ;
    const MePPrintStyle ps = MEPPS_NORMAL;
    const MePDumpStyle ds = MEPDS_GNUPLOT;
    //  MeProfileOutputResults(os, ps, ds, JAGGEDNESS, SETTLETIME);
    //  MeProfileStopTiming();

    exit(0);
  }
#endif
}

void CreateWorld()
{
  McdInit( McdPrimitivesGetTypeCount(), NObjects+10 );

  McdPrimitivesRegisterTypes();
  McdPrimitivesRegisterInteractions();

  space = McdSpaceAxisSortCreate(McdAllAxes, NObjects, NPairs, 1);
  pairs = McdModelPairContainerCreate(100);

  int i;

  for(i = 0; i < NObjects - 6; i++) { // boxes
    MeReal dim[3];
    SetRandomDimensions(dim);
    McdGeometry* box = McdBoxCreate(dim[0], dim[1], dim[2]);
    body[i].m_object = McdModelCreate(box);
  }
  for( ; i < NObjects; i++) { // boundaries
    McdGeometry* plane = McdPlaneCreate();
    body[i].m_object = McdModelCreate(plane);
  }

  for(i = 0; i < NObjects; i++) {
    McdModelSetUserData(body[i].m_object, (void*) &body[i]);
    McdModelSetTransformPtr(body[i].m_object, body[i].m_TM); // dodgy cast?
    McdSpaceInsertModel(space, body[i].m_object);
  }

  for(i = 0; i < NObjects - 6; i++) { // boxes
    // need a better way of generating an initial (non-colliding) position
    body[i].m_position[0] = -PosMax[0] + PosMax[0] * 2 * (i % SomeNumber) / SomeNumber;
    body[i].m_position[1] = -PosMax[1] + PosMax[1] * 2 * (i/SomeNumber % SomeNumber) / SomeNumber;
    body[i].m_position[2] = -PosMax[2] + PosMax[2] * 2 * (i/(SomeNumber*SomeNumber)) / SomeNumber;

    SetRandomOrientation(body[i].m_TM);
    SetRandomVelocity(body[i].m_velocity);
  }
  for( ; i < NObjects; i++) { // boundaries
    MeVectorSetZero(body[i].m_velocity, 3);
  }

  // setup plane matrices:

  // {1,0,0,0,  0,0,-1,0, 0,1,0,0,  0,+-WorldLimits[1],0,1}
  // {1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,+-WorldLimits[2],1}
  // {0,0,1,0,  0,1,0,0,  1,0,0,0,  +-WorldLimits[0],0,0,1}

  // {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,-WorldLimits[2],1}
  body[NObjects - 6].m_TM[3][2] = -WorldLimits[2];

  // {-1,0,0,0, 0,1,0,0, 0,0,-1,0, 0,0,WorldLimits[2],1}
  body[NObjects - 5].m_TM[0][0] = -1;
  body[NObjects - 5].m_TM[2][2] = -1;
  body[NObjects - 5].m_TM[3][2] = WorldLimits[2];

  // {1,0,0,0, 0,0,-1,0, 0,1,0,0, 0,-WorldLimits[1],0,1}
  body[NObjects - 4].m_TM[1][1] = 0;
  body[NObjects - 4].m_TM[1][2] = -1;
  body[NObjects - 4].m_TM[2][1] = 1;
  body[NObjects - 4].m_TM[2][2] = 0;
  body[NObjects - 4].m_TM[3][1] = -WorldLimits[1];

  // {1,0,0,0, 0,0,1,0, 0,-1,0,0, 0,WorldLimits[1],0,1}
  body[NObjects - 3].m_TM[1][1] = 0;
  body[NObjects - 3].m_TM[1][2] = 1;
  body[NObjects - 3].m_TM[2][1] = -1;
  body[NObjects - 3].m_TM[2][2] = 0;
  body[NObjects - 3].m_TM[3][1] = WorldLimits[1];

  // {0,0,1,0, 0,1,0,0, 1,0,0,0, -WorldLimits[0],0,0,1}
  body[NObjects - 2].m_TM[0][0] = 0;
  body[NObjects - 2].m_TM[0][2] = 1;
  body[NObjects - 2].m_TM[2][0] = 1;
  body[NObjects - 2].m_TM[2][2] = 0;
  body[NObjects - 2].m_TM[3][0] = -WorldLimits[0];

  // {0,0,-1,0, 0,1,0,0, -1,0,0,0, WorldLimits[0],0,0,1}
  body[NObjects - 1].m_TM[0][0] = 0;
  body[NObjects - 1].m_TM[0][2] = -1;
  body[NObjects - 1].m_TM[2][0] = -1;
  body[NObjects - 1].m_TM[2][2] = 0;
  body[NObjects - 1].m_TM[3][0] = WorldLimits[0];

  for(i = NObjects - 6 ; i < NObjects; i++) {
    McdSpaceUpdateAll(space);
    McdSpaceFreezeModel(body[i].m_object);
  }

  McdSpaceBuild(space);
}

int autoEvolve = 1;

void toggleAutoEvolve() // (...)
{
  autoEvolve = !autoEvolve;
}

void Tick(RRender* rc)
{
#ifdef WIN32
  static int frameCount = 0;
  static int tm = GetTickCount();

  frameCount++;
  if(GetTickCount()-tm>5000) {
    printf("%d frames\n",frameCount);
    exit(0);
  }
#endif
  if(autoEvolve) EvolveWorld();
}

int main(int argc, const char **argv)
{
  const RRenderType render = RParseRenderType(&argc,&argv);

  AcmeReal origin[3], end[3];

  srand(Seed);

  //     MeProfileStartTiming(hwtmode);

  CreateWorld();

  RRender* rc = RNewRenderContext(render, kRQualityWireframe);

  rc->m_cameraOffset = 20;
  RUpdateCamera();

  float color[3] = {1, 0, 0};

  int i;
  for(i = 0; i < NObjects - 6; i++) {
    McdGeometry* box = McdModelGetGeometry(body[i].m_object);
    MeReal dim[3];
    McdBoxGetDimensions(box, &dim[0], &dim[1], &dim[2]);
    MeMatrix4Ptr m = McdModelGetTransformPtr(body[i].m_object);
    RCreateCube(rc, dim[0], dim[1], dim[2], color, (AcmeReal*) m);
  }
  //   RCreateCube(rc, 2*WorldLimits[0], 2*WorldLimits[1], 2*WorldLimits[2], color, 0);

  float linecolour[3] = {0, 1, 0};

  /* 1-3 */

  origin[0] = -WorldLimits[0]; origin[1] = -WorldLimits[1]; origin[2] = -WorldLimits[2];
  end[0] = -WorldLimits[0]; end[1] =  -WorldLimits[1]; end[2] = WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  origin[0] = -WorldLimits[0]; origin[1] = -WorldLimits[1]; origin[2] = -WorldLimits[2];
  end[0] = -WorldLimits[0]; end[1] =  WorldLimits[1]; end[2] = -WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  origin[0] = -WorldLimits[0]; origin[1] = -WorldLimits[1]; origin[2] = -WorldLimits[2];
  end[0] = WorldLimits[0]; end[1] =  -WorldLimits[1]; end[2] = -WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  /* 4,5 */

  origin[0] = -WorldLimits[0]; origin[1] = -WorldLimits[1]; origin[2] = WorldLimits[2];
  end[0] = -WorldLimits[0]; end[1] =  WorldLimits[1]; end[2] = WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  origin[0] = -WorldLimits[0]; origin[1] = -WorldLimits[1]; origin[2] = WorldLimits[2];
  end[0] = +WorldLimits[0]; end[1] =  -WorldLimits[1]; end[2] = WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  /* 6,7 */

  origin[0] = -WorldLimits[0]; origin[1] = WorldLimits[1]; origin[2] = -WorldLimits[2];
  end[0] = -WorldLimits[0]; end[1] =  WorldLimits[1]; end[2] = WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  origin[0] = -WorldLimits[0]; origin[1] = WorldLimits[1]; origin[2] = -WorldLimits[2];
  end[0] = WorldLimits[0]; end[1] =  WorldLimits[1]; end[2] = -WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  /* 8*/

  origin[0] = -WorldLimits[0]; origin[1] = WorldLimits[1]; origin[2] = WorldLimits[2];
  end[0] = WorldLimits[0]; end[1] =  WorldLimits[1]; end[2] = WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  /* 9, 10*/

  origin[0] = WorldLimits[0]; origin[1] = -WorldLimits[1]; origin[2] = -WorldLimits[2];
  end[0] = WorldLimits[0]; end[1] =  -WorldLimits[1]; end[2] = WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  origin[0] = WorldLimits[0]; origin[1] = -WorldLimits[1]; origin[2] = -WorldLimits[2];
  end[0] = WorldLimits[0]; end[1] =  WorldLimits[1]; end[2] = -WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  /* 11 */

  origin[0] = WorldLimits[0]; origin[1] = -WorldLimits[1]; origin[2] = WorldLimits[2];
  end[0] = WorldLimits[0]; end[1] =  WorldLimits[1]; end[2] = WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  /* 12 */

  origin[0] = WorldLimits[0]; origin[1] = WorldLimits[1]; origin[2] = -WorldLimits[2];
  end[0] = WorldLimits[0]; end[1] =  WorldLimits[1]; end[2] = WorldLimits[2];
  RCreateLine(rc, origin, end, linecolour, 0);

  /* HURRAY */

  RUseKey('s', EvolveWorld);
  RUseKey('a', toggleAutoEvolve);

  RRun( rc, Tick );
  RDeleteRenderContext( rc );

  // Term ...
  return 0;
}
