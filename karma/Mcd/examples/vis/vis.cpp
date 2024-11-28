/*
   file: vis.cpp
   Object interaction visualization with manual manipulation of objects.
   Incarnation now for ConvexMesh and ParticleSystem geometry types.

   NOTE::ParticleSystem Use
   Particle System requires the libMcdParticleSystem made in Mcd/src/particle lib.
   Particle System requires the libMps made in Mps modules ~metk/> cvs co Mps and build
   Particle System requires the MeViewer built by using variable WITH_MPS defined:

   MeViewer/src>setenv WITH_MPS 1
   MeViewer/src>make <release|debug|check_release>

   All the Particle System stuff will be #ifdef'd for now.  This is all non-standard
   development stuff that you can probably do without.
To build with particle system:
   > setenv WITH_MPS 1
   > rebuild MeViewer
   > build Mps
   > build Mcd/src/particle
   > build vis.cpp

   -scott

   Copyright (c) 1997-2002 MathEngine PLC
*/

#include <stdlib.h>    //  malloc
#include <stdio.h>
#include <math.h>

#include <McdFrame.h>       /* collision: prefix "Mcd" */
#include <McdPrimitives.h>  /* Mcd primitive geometry types */
#include <MeViewer.h>
#include <McduDrawMcdContacts.h>
#include <MeMemory.h>

#include <McduDrawConvexMesh.h>
#include <McdCylinder.h>
#include <McdConvexMesh.h>
#include <McdRGHeightField.h>
#include <GL/gl.h>

#include <McdCompositeModel.h>
#include <MeMath.h>

#ifdef WITH_MPS
#include <McdParticleSystem.h>
#include "McdParticleIntersect.h"
#include "MeViewerMps.h"
extern "C" {
#include <Mps.h>
}
#include "MpsSystem.h"
#endif // ParticleSystem Includes

// #define FREEZETEST

#ifdef WIN32
#include <windows.h>
#endif              /* WIN32 */

#define rs 1            // sphere radius
#define bd 2            // box dimension

#define rcn 1           // cone radius
#define hcn 2           // cone height
#define M_PI 3.14159265358979323846

const int MaxObjects = 20;
const int MaxPairs = 30;
const int MaxContacts = 500;

float color[MaxObjects][3] = {
  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f},
  {0.0f, 1.0f, 1.0f}, {0.0f, 0.4f, 0.6f},
  {0.0f, 0.8f, 0.4f}, {0.0f, 0.5f, 0.5f},
  {0.2f, 1.0f, 1.0f}, {0.0f, 0.4f, 0.6f},
  {0.2f, 0.8f, 0.4f}, {0.0f, 0.5f, 0.5f},
  {0.4f, 1.0f, 1.0f}, {0.0f, 0.4f, 0.6f},
  {0.4f, 0.8f, 0.4f}, {0.0f, 0.5f, 0.5f}
};

float white[3] = { 1, 1, 1 };
float orange[3] = {1, 0.5, 0.5};

int NObjects = 0;
int NPairs = MaxPairs;

McdModelPairContainer *Pairs;
McdContact Contacts[MaxContacts];
McdParticleContact PSContacts[MaxContacts];

MeMatrix4 mg;           // ground

extern MeReal McdGjkPt1[3];
extern MeReal McdGjkPt2[3];
MeReal AA[3] = { 0, 0, 0 };
RGraphic *segG;

/*----------------------------------------------------------------------------*/
inline void SetIdentity(MeMatrix4Ptr m) {
    m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
    m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
    m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
    m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}

/*----------------------------------------------------------------------------*/
struct PlBody {
  PlBody() {
    SetIdentity(m_TM);
  } McdModelID m_object;
  MeMatrix4 m_TM;
  RGraphic *m_graphic;
};

PlBody body[MaxObjects];
PlBody Comp[10];

McdCompositeID composite;
int CompObj = -1;

McdSpaceID space;

#ifdef WITH_MPS
/*----------------------------------------------------------------------------*/
/* Global Particle Definitions  */

MeReal Time = 0.0f;
MeReal TimeStep = 0.01f;
int    TimeStepsPerFrame = 4;
int    NumParticles = 500;
int    PSBody = 0;
MeReal PtclesPerSec = 500.0f;

MpsManager*     PM;
MpsSystem*      Explosion;
MpsEmitter*     ExplosionSource;

RParticleSystemRender*   PR1;
RParticleSystemGraphic*  ExplosionGraphic;
MeReal ExplosionCtctCol[]  = {0.83f, 0.86f, 0.80f,
                 0.75f, 0.60f, 0.75f,
                 0.45f, 0.65f, 0.85f,
                 0.75f, 0.40f, 0.85f,
                 0.25f, 0.76f, 0.90f,
                 0.20f, 0.87f, 0.38f,
                 0.15f, 0.99f, 0.42f,
                 0.10f, 0.45f, 0.52f};
MeReal ExplosionColors[] = {1-0.13f, 1-0.86f, 1-0.8f,
                 1-0.35f, 1-0.60f, 1-0.75f,
                 1-0.25f, 1-0.15f, 1-0.35f,
                 1-0.35f, 1-0.40f, 1-0.85f,
                 1-0.25f, 1-0.76f, 1-0.90f,
                 1-0.20f, 1-0.87f, 1-0.38f,
                 1-0.15f, 1-0.99f, 1-0.42f,
                 1-0.10f, 1-0.45f, 1-0.52f};
MeReal ExplosionAgeLimits[] = {0.0f, 1.0f};

/*----------------------------------------------------------------------------*/
void SanePSSettings() {
  MpsSystemSetDecayMethod(Explosion,MpsLifetime);
  MpsSystemSetDecayMean(Explosion,0.75f);
  MpsSystemSetDecayStDev(Explosion,0.25f);
  MpsSystemSetMassMean(Explosion,1.0f);
  MpsSystemSetMassStDev(Explosion,0.0f);
  MpsSystemSetFractionToSpawn(Explosion,0.0f);
  MpsSystemSetNumberToSpawn(Explosion,0);
  MpsSystemSetNumberOfSpawns(Explosion,0);
  MpsSystemSetSpawnSpeedMean(Explosion,0.5f);
  MpsSystemSetSpawnSpeedStDev(Explosion,0.0f);
  MpsSystemSetSpawnSpread(Explosion,3.142f/2.0f);

  MpsEmitterSetNormal(ExplosionSource,0.0f,1.0f,0.0f);
  MpsEmitterSetRadius(ExplosionSource,0.01f);

  MpsSystemAddEmitter(Explosion,ExplosionSource);
  MpsSystemSetRate(Explosion,PtclesPerSec);
  MpsSystemSetSpeedMean(Explosion,1.0f);
  MpsSystemSetSpeedStDev(Explosion,0.05f);
  MpsSystemSetVelAngSpread(Explosion,3.142f);
}

#endif // MPS settings

/*----------------------------------------------------------------------------*/
/* Global AABB Definitions  */

struct BndBox {
  AcmeReal bb_edges[8][3];
  RGraphic *bb_seg[12];
};

BndBox AABB[MaxObjects];
int ActiveAABBs[MaxObjects] = {0};
AcmeReal a[3],b[3],c[3],d[3],e[3],f[3],g[3],h[3]; // 8 box corners
RGraphic *A,*B,*C,*D,*E,*F,*G,*H,*I,*J,*K,*L;
float BBoxColor[3] = {0,1,1};

/*----------------------------------------------------------------------------*/
void
SetBBSeg(int object, int i, AcmeReal orig[3], AcmeReal end[3]) {

  AABB[object].bb_seg[i]->m_origin[0] = orig[0];
  AABB[object].bb_seg[i]->m_origin[1] = orig[1];
  AABB[object].bb_seg[i]->m_origin[2] = orig[2];
  AABB[object].bb_seg[i]->m_end[0] = end[0];
  AABB[object].bb_seg[i]->m_end[1] = end[1];
  AABB[object].bb_seg[i]->m_end[2] = end[2];
}

void ResetBBEdges(int object) {
  MeVector3 minCorner, maxCorner;
  McdModelGetAABB( body[object].m_object, minCorner, maxCorner);

  AABB[object].bb_edges[0][0] = minCorner[0];
  AABB[object].bb_edges[0][1] = minCorner[1];
  AABB[object].bb_edges[0][2] = minCorner[2];
  AABB[object].bb_edges[1][0] = maxCorner[0];
  AABB[object].bb_edges[1][1] = minCorner[1];
  AABB[object].bb_edges[1][2] = minCorner[2];
  AABB[object].bb_edges[2][0] = maxCorner[0];
  AABB[object].bb_edges[2][1] = maxCorner[1];
  AABB[object].bb_edges[2][2] = minCorner[2];
  AABB[object].bb_edges[3][0] = minCorner[0];
  AABB[object].bb_edges[3][1] = maxCorner[1];
  AABB[object].bb_edges[3][2] = minCorner[2];
  AABB[object].bb_edges[4][0] = minCorner[0];
  AABB[object].bb_edges[4][1] = minCorner[1];
  AABB[object].bb_edges[4][2] = maxCorner[2];
  AABB[object].bb_edges[5][0] = maxCorner[0];
  AABB[object].bb_edges[5][1] = minCorner[1];
  AABB[object].bb_edges[5][2] = maxCorner[2];
  AABB[object].bb_edges[6][0] = maxCorner[0];
  AABB[object].bb_edges[6][1] = maxCorner[1];
  AABB[object].bb_edges[6][2] = maxCorner[2];
  AABB[object].bb_edges[7][0] = minCorner[0];
  AABB[object].bb_edges[7][1] = maxCorner[1];
  AABB[object].bb_edges[7][2] = maxCorner[2];
}

/*----------------------------------------------------------------------------*/
void UpdateAABB() {
  int num_objects = ActiveAABBs[0];
  int reg_idx;

  for(int object=1; object<=num_objects; object++) {
    reg_idx = ActiveAABBs[object];
    ResetBBEdges(reg_idx);
    SetBBSeg(reg_idx, 0, AABB[reg_idx].bb_edges[0], AABB[reg_idx].bb_edges[3]);
    SetBBSeg(reg_idx, 1, AABB[reg_idx].bb_edges[4], AABB[reg_idx].bb_edges[7]);
    SetBBSeg(reg_idx, 2, AABB[reg_idx].bb_edges[5], AABB[reg_idx].bb_edges[6]);
    SetBBSeg(reg_idx, 3, AABB[reg_idx].bb_edges[1], AABB[reg_idx].bb_edges[2]);
    SetBBSeg(reg_idx, 4, AABB[reg_idx].bb_edges[3], AABB[reg_idx].bb_edges[2]);
    SetBBSeg(reg_idx, 5, AABB[reg_idx].bb_edges[0], AABB[reg_idx].bb_edges[1]);
    SetBBSeg(reg_idx, 6, AABB[reg_idx].bb_edges[4], AABB[reg_idx].bb_edges[5]);
    SetBBSeg(reg_idx, 7, AABB[reg_idx].bb_edges[7], AABB[reg_idx].bb_edges[6]);
    SetBBSeg(reg_idx, 8, AABB[reg_idx].bb_edges[3], AABB[reg_idx].bb_edges[7]);
    SetBBSeg(reg_idx, 9, AABB[reg_idx].bb_edges[2], AABB[reg_idx].bb_edges[6]);
    SetBBSeg(reg_idx, 10, AABB[reg_idx].bb_edges[1], AABB[reg_idx].bb_edges[5]);
    SetBBSeg(reg_idx, 11, AABB[reg_idx].bb_edges[0], AABB[reg_idx].bb_edges[4]);
  }
}

void InitAABB(RRender *rc, int object) {
  int i;
  ResetBBEdges(object);
  ActiveAABBs[0]++;
  ActiveAABBs[ActiveAABBs[0]] = object; // <g> [0] entry is the length

  AABB[object].bb_seg[0] =
    RCreateLine(rc, AABB[object].bb_edges[0],AABB[object].bb_edges[3], BBoxColor, 0);
  AABB[object].bb_seg[1] =
    RCreateLine(rc, AABB[object].bb_edges[4],AABB[object].bb_edges[7], BBoxColor, 0);
  AABB[object].bb_seg[2] =
    RCreateLine(rc, AABB[object].bb_edges[5],AABB[object].bb_edges[6], BBoxColor, 0);
  AABB[object].bb_seg[3] =
    RCreateLine(rc, AABB[object].bb_edges[1],AABB[object].bb_edges[2], BBoxColor, 0);
  AABB[object].bb_seg[4] =
    RCreateLine(rc, AABB[object].bb_edges[3],AABB[object].bb_edges[2], BBoxColor, 0);
  AABB[object].bb_seg[5] =
    RCreateLine(rc, AABB[object].bb_edges[0],AABB[object].bb_edges[1], BBoxColor, 0);
  AABB[object].bb_seg[6] =
    RCreateLine(rc, AABB[object].bb_edges[4],AABB[object].bb_edges[5], BBoxColor, 0);
  AABB[object].bb_seg[7] =
    RCreateLine(rc, AABB[object].bb_edges[7],AABB[object].bb_edges[6], BBoxColor, 0);
  AABB[object].bb_seg[8] =
    RCreateLine(rc, AABB[object].bb_edges[3],AABB[object].bb_edges[7], BBoxColor, 0);
  AABB[object].bb_seg[9] =
    RCreateLine(rc, AABB[object].bb_edges[2],AABB[object].bb_edges[6], BBoxColor, 0);
  AABB[object].bb_seg[10]=
    RCreateLine(rc, AABB[object].bb_edges[1],AABB[object].bb_edges[5], BBoxColor, 0);
  AABB[object].bb_seg[11]=
    RCreateLine(rc, AABB[object].bb_edges[0],AABB[object].bb_edges[4], BBoxColor, 0);

  for (i=0; i<12; i++) RMakeUnpickable(AABB[object].bb_seg[i]);
}

/*----------------------------------------------------------------------------*/
//  Function Definitions

/*----------------------------------------------------------------------------*/
inline void SetPosition(MeMatrix4Ptr _m, MeReal x, MeReal y, MeReal z) {
    MeVector4* m = (MeVector4*)_m;
    (m)[3][0] = x; (m)[3][1] = y; (m)[3][2] = z;
}

/*-----------------------------------------------------------------------------*/
inline void QuaternionToMatrix44(MeMatrix4Ptr m, const MeReal * quaternion)
{
  const MeReal q0 = quaternion[0];
  const MeReal q1 = quaternion[1];
  const MeReal q2 = quaternion[2];
  const MeReal q3 = quaternion[3];

  MeReal q0sq = q0 * q0;
  MeReal q1sq = q1 * q1;
  MeReal q2sq = q2 * q2;
  MeReal q3sq = q3 * q3;
  MeReal q0q1 = 2 * q0 * q1;
  MeReal q0q2 = 2 * q0 * q2;
  MeReal q0q3 = 2 * q0 * q3;
  MeReal q1q2 = 2 * q1 * q2;
  MeReal q1q3 = 2 * q1 * q3;
  MeReal q2q3 = 2 * q2 * q3;

    // rotation matrix in terms of quaternion
  m[0][0] = q0sq + q1sq - q2sq - q3sq;
  m[1][0] = q1q2 + q0q3;
  m[2][0] = q1q3 - q0q2;

  m[0][1] = q1q2 - q0q3;
  m[1][1] = q0sq + q2sq - q1sq - q3sq;
  m[2][1] = q2q3 + q0q1;

  m[0][2] = q1q3 + q0q2;
  m[1][2] = q2q3 - q0q1;
  m[2][2] = q0sq + q3sq - q1sq - q2sq;
}

/*----------------------------------------------------------------------------*/
void Normalize(MeReal * v) {
  MeReal norm = 1.0f / (MeReal) sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  v[0] *= norm;
  v[1] *= norm;
  v[2] *= norm;
}

/*-----------------------------------------------------------------------------*/
void SetAxisAngle(MeReal * q, const MeReal nX, const MeReal nY,
          const MeReal nZ, MeReal angle)
{
  MeReal s_2 = -(MeReal) sin(0.5f * angle);
  q[1] = nX;
  q[2] = nY;
  q[3] = nZ;
  Normalize(q + 1);
  q[0] = (MeReal) cos(0.5f * angle);
  q[1] *= s_2;
  q[2] *= s_2;
  q[3] *= s_2;
}

/*----------------------------------------------------------------------------*/
void set(const MeReal * mgl, MeMatrix4Ptr mcl) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      mcl[i][j] = mgl[i * 4 + j];
    }
  }
}

void
checkPairContainer( McdSpaceID space )
{
  McdModelPairContainer pairs;
  McdModelPair **array;
  int size;

  McdSpacePairIterator it;
//    int overflow;
  int numHello,numStaying,numGoodbye;

  size = 10;
  array = (McdModelPair**)malloc( size * sizeof( McdModelPair* ) );
  McdModelPairContainerInit( &pairs, array, size );

  McdSpacePairIteratorBegin( space, &it );

  numHello = numStaying = numGoodbye = 0;
//    overflow = 0;
//    overflow = McdSpaceGetPairs( space, &it, &pairs );
    //McdModelPairContainerReset( &pairs );
  numGoodbye = McdModelPairContainerGetGoodbyeCount( &pairs );
  numStaying = McdModelPairContainerGetStayingCount( &pairs );
  numHello = McdModelPairContainerGetHelloCount( &pairs );

  printf( "\n");
  printf( "num hello: %i ",numHello );
  printf( "num stay: %i ",numStaying );
  printf( "num goodbye: %i ",numGoodbye );
  //printf( "\n");
}


/*----------------------------------------------------------------------------*/
void EvolveWorld() {
  McdIntersectResult result;
#ifdef WITH_MPS
  McdParticleIntersectResult PSresult;
  int nPSContacts;
#endif
  int i, nContacts;
  int touch;
  McdSpacePairIterator spaceIter;
  McdModelPair *pair;
  MeBool pairOverflow;
  for (i = 0; i < NObjects; i++) {
    body[i].m_graphic->m_color[0] = color[i][0];
    body[i].m_graphic->m_color[1] = color[i][1];
    body[i].m_graphic->m_color[2] = color[i][2];
  }
#ifdef WITH_MPS
  SetParticleSystemGraphicColors(ExplosionGraphic, (RenderReal *)ExplosionColors);
#endif

  McdSpaceUpdateAll(space);
  McdSpaceEndChanges(space);
  McdSpacePairIteratorBegin(space, &spaceIter);
  result.contacts = &(Contacts[0]);
  result.contactMaxCount = MaxContacts;

#ifdef WITH_MPS
  PSresult.contacts = &(PSContacts[0]);
  PSresult.contactMaxCount = MaxContacts;
  nPSContacts = 0;
#endif

  do {
    McdModelPairContainerReset(Pairs);
    pairOverflow = McdSpaceGetPairs(space, &spaceIter, Pairs);
    McdGoodbyeEach(Pairs);
    McdHelloEach(Pairs);
    nContacts = 0;
    for (i = Pairs->helloFirst; i < Pairs->stayingEnd; i++) {
      pair = Pairs->array[i];
#ifdef WITH_MPS
      if (!(McdModelGetGeometry(Pairs->array[i]->model1) == McdModelGetGeometry(body[PSBody].m_object)) &&
      !(McdModelGetGeometry(Pairs->array[i]->model2) == McdModelGetGeometry(body[PSBody].m_object)))
#endif
      {
    McdIntersect(pair, &result);
    touch = result.touch;
    result.contacts += result.contactCount;
    result.contactMaxCount -= result.contactCount;
    nContacts += result.contactCount;
    if (touch) {
        // Model 1 orange
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model1))->m_graphic->m_color[0] = orange[0];
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model1))->m_graphic->m_color[1] = orange[1];
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model1))->m_graphic->m_color[2] = orange[2];
        // Model 2 orange
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model2))->m_graphic->m_color[0] = orange[0];
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model2))->m_graphic->m_color[1] = orange[1];
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model2))->m_graphic->m_color[2] = orange[2];
    }
      }
#ifdef WITH_MPS
      else
      {
    McdParticleIntersect(Pairs->array[i], &PSresult);
    touch = PSresult.touch;
    PSresult.contacts += PSresult.contactCount;
    PSresult.contactMaxCount -= PSresult.contactCount;
    nPSContacts += PSresult.contactCount;
    if (touch) {
      SetParticleSystemGraphicColors(ExplosionGraphic, (RenderReal *)ExplosionCtctCol);
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model1))->m_graphic->m_color[0] = orange[0];
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model1))->m_graphic->m_color[1] = orange[1];
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model1))->m_graphic->m_color[2] = orange[2];
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model2))->m_graphic->m_color[0] = orange[0];
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model2))->m_graphic->m_color[1] = orange[1];
      ((PlBody *)McdModelGetUserData(Pairs->array[i]->model2))->m_graphic->m_color[2] = orange[2];
    }
      }
#endif
    }
#ifdef WITH_MPS /*-- This is all Particle System Managment -- Could be streamlined, n'est-ce pas? --*/
    if ( MpsSystemGetTotalParticlesCreated(Explosion) >
     MpsSystemGetMaxParticles(Explosion)) {
      MpsSystemSetCurrentEmitter(Explosion,ExplosionSource);
      MpsSystemSetRate(Explosion,0.0f);
    }
    if (MpsSystemGetNumParticles(Explosion) == 0)  {
      MpsSystemSetCurrentEmitter(Explosion,ExplosionSource);
      MpsSystemSetRate(Explosion,PtclesPerSec);
      MpsSystemSetTotalParticlesCreated(Explosion,0);
      MpsUpdateAllSystems(PM,TimeStep*4);
    }
    Time += TimeStep;
    MpsUpdateAllSystems(PM,TimeStep*4);
    ExplosionGraphic->m_ParticleNumber = MpsSystemGetNumParticles(Explosion);
    for(i=0 ; i<ExplosionGraphic->m_ParticleNumber ; i++) {
      MeVector3 pPos;
      MpsSystemGetParticlePosition(Explosion,i,pPos);
      ExplosionGraphic->m_ParticlePositions[3*i]   = pPos[0];
      ExplosionGraphic->m_ParticlePositions[3*i+1] = pPos[1];
      ExplosionGraphic->m_ParticlePositions[3*i+2] = pPos[2];
      ExplosionGraphic->m_ParticleRelativeAges[i] = MpsSystemGetParticleRelativeAge(Explosion,i);
    } //  End Particle System Management
#endif
  }while(pairOverflow);

  McduSetContactArray(Contacts, nContacts);
  UpdateAABB();
  McdSpaceBeginChanges(space);
}

/*----------------------------------------------------------------------------*/
void CreateBox(RRender * rc, int i, MeReal dimx, MeReal dimy, MeReal dimz)
{
    MeReal dims[3];
    McdBoxID box;
    dims[0] = dimx; dims[1] = dimy; dims[2] = dimz;
    box = McdBoxCreate(dims[0], dims[1], dims[2]);
    body[i].m_graphic =
    RCreateCube(rc, dims[0], dims[1], dims[2], color[i], 0);
    body[i].m_graphic->m_userData = (void *) i;
    body[i].m_object = McdModelCreate(box);
}

/*----------------------------------------------------------------------------*/
void CreateSphere(RRender * rc, int i) {
  McdSphereID geom = McdSphereCreate(rs);
  body[i].m_graphic =   RCreateSphere(rc, rs, color[i], 0);
  body[i].m_graphic->m_userData = (void *) i;
  body[i].m_object = McdModelCreate(geom);
}

/*----------------------------------------------------------------------------*/
void CreateCone(RRender * rc, int i) {
    McdConeID geom = McdConeCreate(hcn, rcn);
    body[i].m_graphic =
    RCreateCone(rc, rcn, 0, hcn, McdConeGetZOffset(geom), color[i], 0);
    body[i].m_graphic->m_userData = (void *) i;
    body[i].m_object = McdModelCreate(geom);
}

/*----------------------------------------------------------------------------*/
void CreateCylinder(RRender * rc, int i) {
  McdCylinderID geom = McdCylinderCreate(rcn, hcn);
  body[i].m_graphic = RCreateCylinder(rc, rcn, hcn, color[i], 0);
  body[i].m_graphic->m_userData = (void *) i;
  body[i].m_object = McdModelCreate(geom);
}

/*----------------------------------------------------------------------------*/
void CreateConvex(RRender * rc, int i) {
  McdConvexMeshID geometry;
  const MeReal Fatty = 0;
  MeReal cnvxbd[3];
  cnvxbd[0] = (rs - Fatty)/2;
  cnvxbd[1] = (bd - Fatty)/2;
  cnvxbd[2] = (bd - Fatty)/2;
  MeReal vertices[9][3] = {{ cnvxbd[0], cnvxbd[1], cnvxbd[2]},
               {-cnvxbd[0], cnvxbd[1], cnvxbd[2]},
               { cnvxbd[0],-cnvxbd[1], cnvxbd[2]},
               {-cnvxbd[0],-cnvxbd[1], cnvxbd[2]},
               { cnvxbd[0], cnvxbd[1],-cnvxbd[2]},
               {-cnvxbd[0], cnvxbd[1],-cnvxbd[2]},
               { cnvxbd[0],-cnvxbd[1],-cnvxbd[2]},
               {-cnvxbd[0],-cnvxbd[1],-cnvxbd[2]},
               { 0, 1.5f*cnvxbd[1], 0}};
  geometry = (McdGeometryID) McdConvexMeshCreateHull(vertices, 8, Fatty);
  body[i].m_graphic = RCreateConvexMesh(rc, (McdConvexMeshID) geometry,
                    color[i], 0);
  body[i].m_graphic->m_userData = (void *) i;
  body[i].m_object = McdModelCreate(geometry);
}

/*----------------------------------------------------------------------------*/
void CreatePlane(RRender * rc, int i) {
  MeReal q[4];
  SetAxisAngle(q, 1, 0, 0, -(MeReal) M_PI / 2.0f);
  SetIdentity(mg);
  QuaternionToMatrix44(mg, q);
  SetPosition(mg, 0, -2, 0);
  McdPlaneID geom = McdPlaneCreate();
  body[i].m_graphic =
    RCreateCube(rc, 20, 20.0f, 0.1f, color[i], (MeReal *) mg);
  body[i].m_graphic->m_userData = (void *) i;
  body[i].m_object = McdModelCreate(geom);
}

/*----------------------------------------------------------------------------*/
void CreateCompositeNoSpheres(RRender * rc, int i) {
  McdBoxID box;
  McdSphereID sphere;
  MeReal dimx = (MeReal)bd*3, dimy = (MeReal)bd/3.0, dimz = (MeReal)bd/3.0;
  MeMatrix4 relXform;
  SetIdentity(relXform);
  CompObj = i;
  sphere = McdSphereCreate(rs);
  box = McdBoxCreate(dimx, dimy, dimz);

  body[i].m_graphic = RCreateCube(rc, dimx, dimy, dimz, color[i], 0);
  body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object = McdCompositeModelCreate(10);
  SetPosition(body[i].m_TM, 0, 0, 0);
  McdModelSetTransformPtr( body[i].m_object, (MeMatrix4Ptr)&body[i].m_graphic->m_matrixArray);

  Comp[0].m_graphic =  body[i].m_graphic;
  Comp[0].m_graphic->m_userData = (void*) i;
  Comp[0].m_object = McdModelCreate(box);
  SetPosition(Comp[0].m_TM, 0, 0, 0);
  McdModelSetTransformPtr( Comp[0].m_object, (MeMatrix4Ptr)&Comp[0].m_graphic->m_matrixArray);
  McdCompositeModelAddElement(body[i].m_object, Comp[0].m_object,  Comp[0].m_TM);
  McdCompositeModelBuild(body[i].m_object);
}

/*----------------------------------------------------------------------------*/
void CreateComposite(RRender * rc, int i) {
  McdBoxID box;
  McdSphereID sphere;
  MeReal dimx = (MeReal)bd*3, dimy = (MeReal)bd/3.0, dimz = (MeReal)bd/3.0;
  MeMatrix4 relXform;
  SetIdentity(relXform);
  CompObj = i;
  sphere = McdSphereCreate(rs);
  box = McdBoxCreate(dimx, dimy, dimz);

// Composite Model
  body[i].m_graphic = RCreateCube(rc, dimx, dimy, dimz, color[i], 0);
//    body[i].m_graphic = RCreateSphere(rc, rs, color[i], 0);
  body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object = McdCompositeModelCreate(10);
  SetPosition(body[i].m_TM, 0, 0, 0);
  McdModelSetTransformPtr( body[i].m_object, (MeMatrix4Ptr)&body[i].m_graphic->m_matrixArray);

//  So here the (McdModel Xform)Comp->m_object->mGlobalTM points to the render xform
//  Composite Geometries
// BAR
  Comp[0].m_graphic =  body[i].m_graphic;
//    Comp[0].m_graphic =  RCreateCube(rc, dimx, dimy, dimz, color[i], 0);
  Comp[0].m_graphic->m_userData = (void*) i;
  Comp[0].m_object = McdModelCreate(box);
  SetPosition(Comp[0].m_TM, 0, 0, 0);
  McdModelSetTransformPtr( Comp[0].m_object, (MeMatrix4Ptr)&Comp[0].m_graphic->m_matrixArray);
  McdCompositeModelAddElement(body[i].m_object, Comp[0].m_object,  Comp[0].m_TM);

// BEL 1
  Comp[1].m_graphic = RCreateSphere(rc, rs, color[i], 0);
  Comp[1].m_graphic->m_userData = (void*) i;
  Comp[1].m_object = McdModelCreate(sphere);
  SetPosition(Comp[1].m_TM, 3*bd/2, 0, 0);
  McdModelSetTransformPtr( Comp[1].m_object, (MeMatrix4Ptr)&Comp[1].m_graphic->m_matrixArray);
  McdCompositeModelAddElement(body[i].m_object, Comp[1].m_object, Comp[1].m_TM);

// BEL 2
  Comp[2].m_graphic = RCreateSphere(rc, rs, color[i], 0);
  Comp[2].m_graphic->m_userData = (void*) i;
  Comp[2].m_object = McdModelCreate(sphere);
  SetPosition(Comp[2].m_TM, -3*bd/2, 0, 0);
  McdModelSetTransformPtr( Comp[2].m_object, (MeMatrix4Ptr)&Comp[2].m_graphic->m_matrixArray);
  McdCompositeModelAddElement(body[i].m_object, Comp[2].m_object, Comp[2].m_TM);

//  Solidify the model after additions
  McdCompositeModelBuild(body[i].m_object);
}

#ifdef WITH_MPS
/*----------------------------------------------------------------------------*/
void CreateParticle(RRender *rc, int i) {
  SanePSSettings(); // All global declarations initialized
//    int pMax = MpsSystemGetMaxParticles(Explosion);
//    int pVerts = MpsSystemGetNumParticles(Explosion);
  MeReal pRadius = 10.2;
  MeVector3 pPosition = {0.5,2.0,0.2};

  McdParticleSystemID geom;
  geom = McdParticleSystemCreate(&(Explosion->m_NumberOfActiveParticles),
                 Explosion->m_Size,
                 pPosition,
                 pRadius,
                 Explosion->m_ParticlePosition, sizeof(MeVector3));
  ExplosionGraphic = RCreateParticleSystemGraphic(PR1);
  ExplosionGraphic->m_RMethod        = Blobs;
  ExplosionGraphic->m_BlobSize       = 0.2f;
  ExplosionGraphic->m_ColorVariation = LinearInterpolation;
  ExplosionGraphic->m_NumberOfColors = 4;
  SetParticleSystemGraphicColors(ExplosionGraphic, (RenderReal *)ExplosionCtctCol);
  SetParticleSystemGraphicAgeLimits(ExplosionGraphic, (RenderReal *)ExplosionAgeLimits);
  SetPSGMaximumNumberOfParticles(ExplosionGraphic,
                                 MpsSystemGetMaxParticles(Explosion));
  body[i].m_graphic = (RGraphic*)ExplosionGraphic;
  body[i].m_graphic->m_userData = (void*) i;
  body[i].m_object = McdModelCreate(geom);
}
#endif

/*----------------------------------------------------------------------------*/
void McuDrawLineSeg(void *cn)
{
    glBegin(GL_LINES);
    glVertex3f(McdGjkPt1[0], McdGjkPt1[1], McdGjkPt1[2]);
    glVertex3f(McdGjkPt2[0], McdGjkPt2[1], McdGjkPt2[2]);
    glEnd();
    glBegin(GL_POINTS);
    glVertex3f(McdGjkPt1[0], McdGjkPt1[1], McdGjkPt1[2]);
    glVertex3f(McdGjkPt2[0], McdGjkPt2[1], McdGjkPt2[2]);
    glEnd();
}

/*----------------------------------------------------------------------------*/
void CreateWorld(RRender * rc)
{
  int i;
  McdInit(McdPrimitivesGetTypeCount()+4, 100);
  McdPrimitivesRegisterTypes();
  McdConvexMeshRegisterType();
  McdPrimitivesRegisterInteractions();
//  McdConvexMeshRegisterInteractions();
#ifdef WITH_MPS
  McdParticleSystemRegisterType();
  McdParticleSystemRegisterInteractions();
/*  Create Particle System Managers */
  PM = MpsNewManager();
  Explosion = MpsCreateSystem(PM,NumParticles);
  ExplosionSource = MpsCreateEmitter(PM,MpsSphere);
#endif
  McdCompositeRegisterType();
  McdCompositeGenericRegisterInteractions();

  space = McdSpaceAxisSortCreate(McdAllAxes, MaxObjects, MaxPairs,1);
  Pairs = McdModelPairContainerCreate(MaxPairs);

  CreatePlane(rc, NObjects++); // Leave the plane as object '0'
  CreateBox(rc, NObjects++, bd,bd,bd);
//  CreateBox(rc, NObjects++, (MeReal)bd/3.0, (MeReal)bd/3.0, (MeReal)4.0*bd);
  CreateSphere(rc, NObjects++);
  CreateCone(rc, NObjects++);
  CreateCylinder(rc, NObjects++);
//    CreateConvex(rc, NObjects++);
  CreateComposite(rc, NObjects++);


  for (i = 0; i < NObjects; i++) {
    McdModelSetUserData(body[i].m_object, (void *) &body[i]);
    McdModelSetTransformPtr( body[i].m_object, (MeMatrix4Ptr)&body[i].m_graphic->m_matrixArray);
    McdSpaceInsertModel(space, body[i].m_object);
    if (i != 0) {       // plane should be the first "Create" above
    SetPosition((MeMatrix4Ptr) body[i].m_graphic->m_matrixArray, (i-2) * 2.5f, 0, 0);
    }
  }
  if (CompObj > 0)
    SetPosition((MeMatrix4Ptr) body[CompObj].m_graphic->m_matrixArray, 0 ,  5.0f, 0);

#ifdef WITH_MPS
  PSBody = NObjects++;
  CreateParticle(rc,PSBody);
  SetIdentity((MeMatrix4Ptr)body[PSBody].m_graphic->m_matrixArray);

  McdSpaceInsertModel(space, body[PSBody].m_object);
  McdModelSetUserData(body[PSBody].m_object, (void*) &body[PSBody]);
  McdModelSetTransformPtr(body[PSBody].m_object, (MeVector4*)&body[PSBody].m_graphic->m_matrixArray);

  SetPosition((MeMatrix4Ptr)body[PSBody].m_graphic->m_matrixArray, 0.5f, 2.5f, 0.2f);
  SetPosition(McdModelGetTransformPtr(body[PSBody].m_object),      0.5f, 2.5f, 0.2f);
  MpsEmitterSetOrigin(ExplosionSource,                             0.5f, 2.5f, 0.2f);
#endif

  segG = RCreateUserProceduralObject(rc, (RproceduralObjectCallback)McuDrawLineSeg,
                     0, "seg", 1, white, 0);
  McduCreateMcdContactGraphics(rc, 0);
  McdSpaceBuild(space);

//    if (NObjects >= CompObj && CompObj > 0)  InitAABB(rc, CompObj);
//    if (NObjects >= 6)  InitAABB(rc, 6);
#ifdef WITH_MPS
  InitAABB(rc, NObjects-1);
#endif
}

/*----------------------------------------------------------------------------*/
void Click(int button, int state, int x, int y, RGraphic * graphic) {
#if FREEZETEST
  if (graphic && button==1 && state==0) {
      // fprintf(stderr, "index: %d, button: %d state: %d\n", (int) graphic->m_userData, button, state);
    int index = (int) graphic->m_userData;
    McdModelID m = body[index].m_object;

    if (McdSpaceModelIsFrozen(m)) {

      fprintf(stderr, "UNfreezing %d\n", index);
      McdSpaceUnfreezeModel(m);
    }
    else
    {
      fprintf(stderr, "freezing %d\n", index);
      McdSpaceFreezeModel(m);
    }
  }
#endif
}

/*----------------------------------------------------------------------------*/
void Tick(RRender * rc) {
  EvolveWorld();
}

/*----------------------------------------------------------------------------*/
int main(int argc, const char **argv)
{
  const RRenderType render = RParseRenderType(&argc, &argv);
  RRender *rc = RNewRenderContext(render, kRQualitySmooth);

#ifdef WITH_MPS
  PR1 = RNewParticleSystemRenderContext();
  RExternalRender(RenderAllParticleSystems);
#endif

  CreateWorld(rc);

  rc->m_cameraOffset = 20;
  RUpdateCamera();

  RUseKey('s', EvolveWorld);
  RUseKey('c', McduToggleMcdContactDrawing);
  RMouseFunc(Click);
  rc->m_useDisplayLists = 0;    /* see the color changes */

  McduToggleMcdContactDrawing();

  McdGetDefaultRequestPtr()->contactMaxCount = 10;

  RRun(rc, Tick);
  RDeleteRenderContext(rc);

#ifdef WITH_MPS
  RDeleteParticleSystemRenderContext(PR1);
  MpsDeleteManager(PM);
#endif

  return 0;
}

