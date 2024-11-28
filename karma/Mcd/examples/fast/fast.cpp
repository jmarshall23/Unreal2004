/*
  Copyright (c) 1997-2002 MathEngine PLC

  $Name: t-stevet-RWSpre-030110 $

*/

/*
  This example uses MathEngine collision and dynamics toolkits.

  Fast object hitting another
*/

#include <stdlib.h>
#include <stdio.h>

#include <Mdt.h>
#include <Mst.h>

#include <McdFrame.h>
#include <McdPrimitives.h>

#include <MeViewer.h>
#include <MeMath.h>
#include "GL/gl.h"

#include <McdConvexMesh.h>
#include <McduDrawMcdContacts.h>

#ifdef WITH_GLUI
#include "glui.h"
GLUI *glui = 0;
#endif

/* Whether to boxes or balls, for each object */
enum {projectileIdx=0, targetIdx=1};
const int BoxesNotBalls[2] = {1,1};

/* number of boxes or balls for shooting*/
#define NObjects    2
#define NContacts   200

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;
/* Space for the Collision Toolkit */
McdSpaceID space;
/* Connection between dynamics and collision */
MstBridgeID bridge;

/*
  Physics representations
*/
MdtBodyID target;
MdtBodyID project;

/*
  Graphics representations
*/
RGraphicsDescription *safeBall;
RGraphicsDescription *planeG;
RGraphicsDescription *coneG;
RGraphicsDescription *lineG;
RGraphicsDescription *targetG;
RGraphicsDescription *projectG;
RRender *rc = 0;

/*
  Collision representations
*/
McdModelID targetCM;
McdModelID projectCM;
McdModelID planeCM;

McdModelPairID p, p_plane;

/* Collision geometry ID (box) */
McdGeometryID targetGeom = 0;
/* Collision geometry ID (ball) */
McdGeometryID projectGeom = 0;
McdGeometryID plane_prim = 0;

int autoBenchmark = 0;
int autoEvolve = 1;

MeReal step = (MeReal) 0.01;
MeReal speed = (MeReal) 142.0; /* 12.0; */
MeReal gravity[3] = { 0, -10, 0 };

MeMatrix4 safeXform; // for the SafeTime estimate

MeMatrix4 tmPlane =
{
    {1, 0, 0, 0},
    {0, 0, -1, 0},
    {0, 1, 0, 0},
    {0, 0, 0, 1}
};

MeVector3 ballInitPos = {0, 3, -5};

char *help[] = { "Press <space> key to shoot" };

void toggleBenchmark(void)
{
    autoBenchmark = !autoBenchmark;
    printf("%i\n", autoBenchmark);
}

void toggleAutoEvolve(void)
{
    autoEvolve = !autoEvolve;
}

void Normalize(MeReal *v)
{
    MeReal norm =
        1.0f / (MeReal) sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    v[0] *= norm;
    v[1] *= norm;
    v[2] *= norm;
}


/*----------------------------------------------------------------------------*/
/* Global AABB Definitions  */

const int MaxObjects = 5;
struct BndBox {
  AcmeReal bb_edges[8][3];
  RGraphic *bb_seg[12];
};

BndBox AABB[MaxObjects];
int ActiveAABBCount = 0;
McdModelID ActiveAABBs[MaxObjects];
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
//    McdModelGetPathAABB( ActiveAABBs[object], minCorner, maxCorner, step);
  McdModelGetAABB( ActiveAABBs[object], minCorner, maxCorner);

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
  for(int object=0; object<ActiveAABBCount; object++) {

    ResetBBEdges(object);
    SetBBSeg(object, 0, AABB[object].bb_edges[0], AABB[object].bb_edges[3]);
    SetBBSeg(object, 1, AABB[object].bb_edges[4], AABB[object].bb_edges[7]);
    SetBBSeg(object, 2, AABB[object].bb_edges[5], AABB[object].bb_edges[6]);
    SetBBSeg(object, 3, AABB[object].bb_edges[1], AABB[object].bb_edges[2]);
    SetBBSeg(object, 4, AABB[object].bb_edges[3], AABB[object].bb_edges[2]);
    SetBBSeg(object, 5, AABB[object].bb_edges[0], AABB[object].bb_edges[1]);
    SetBBSeg(object, 6, AABB[object].bb_edges[4], AABB[object].bb_edges[5]);
    SetBBSeg(object, 7, AABB[object].bb_edges[7], AABB[object].bb_edges[6]);
    SetBBSeg(object, 8, AABB[object].bb_edges[3], AABB[object].bb_edges[7]);
    SetBBSeg(object, 9, AABB[object].bb_edges[2], AABB[object].bb_edges[6]);
    SetBBSeg(object, 10, AABB[object].bb_edges[1], AABB[object].bb_edges[5]);
    SetBBSeg(object, 11, AABB[object].bb_edges[0], AABB[object].bb_edges[4]);
  }
}

void InitAABB(RRender *rc, McdModelID objectid) {
  int object = ActiveAABBCount;
  ActiveAABBs[ActiveAABBCount] = objectid;
  ActiveAABBCount++;
  ResetBBEdges(object);

  AABB[object].bb_seg[0] = RCreateLine(rc, AABB[object].bb_edges[0],AABB[object].bb_edges[3], BBoxColor, 0);
  AABB[object].bb_seg[1] = RCreateLine(rc, AABB[object].bb_edges[4],AABB[object].bb_edges[7], BBoxColor, 0);
  AABB[object].bb_seg[2] = RCreateLine(rc, AABB[object].bb_edges[5],AABB[object].bb_edges[6], BBoxColor, 0);
  AABB[object].bb_seg[3] = RCreateLine(rc, AABB[object].bb_edges[1],AABB[object].bb_edges[2], BBoxColor, 0);
  AABB[object].bb_seg[4] = RCreateLine(rc, AABB[object].bb_edges[3],AABB[object].bb_edges[2], BBoxColor, 0);
  AABB[object].bb_seg[5] = RCreateLine(rc, AABB[object].bb_edges[0],AABB[object].bb_edges[1], BBoxColor, 0);
  AABB[object].bb_seg[6] = RCreateLine(rc, AABB[object].bb_edges[4],AABB[object].bb_edges[5], BBoxColor, 0);
  AABB[object].bb_seg[7] = RCreateLine(rc, AABB[object].bb_edges[7],AABB[object].bb_edges[6], BBoxColor, 0);
  AABB[object].bb_seg[8] = RCreateLine(rc, AABB[object].bb_edges[3],AABB[object].bb_edges[7], BBoxColor, 0);
  AABB[object].bb_seg[9] = RCreateLine(rc, AABB[object].bb_edges[2],AABB[object].bb_edges[6], BBoxColor, 0);
  AABB[object].bb_seg[10]= RCreateLine(rc, AABB[object].bb_edges[1],AABB[object].bb_edges[5], BBoxColor, 0);
  AABB[object].bb_seg[11]= RCreateLine(rc, AABB[object].bb_edges[0],AABB[object].bb_edges[4], BBoxColor, 0);
  int i;
  for (i=0; i<12; i++) RMakeUnpickable(AABB[object].bb_seg[i]);
}



/*---------------------------------------------------------------------------*/
typedef struct LineSeg
{
  MeVector3 p1;
  MeVector3 p2;
} LineSeg;

void McduDrawLineSeg(void *cn)
{
    LineSeg *s = (LineSeg*) cn;
    glBegin(GL_LINES);
    glVertex3f(s->p1[0], s->p1[1], s->p1[2]);
    glVertex3f(s->p2[0], s->p2[1], s->p2[2]);
    glEnd();

    glBegin(GL_POINTS);
    glVertex3f(s->p2[0], s->p2[1], s->p2[2]);
    glEnd();
}

LineSeg VelocitySeg1;


/*---------------------------------------------------------------------------*/

void ShootFromCamera(void)
{
    int i;
    MeReal v[3];

    MdtBodySetPosition(project,
        rc->m_cameraPos[0], rc->m_cameraPos[1], rc->m_cameraPos[2]);

    for (i = 0; i < 3; i++)
        v[i] = rc->m_cameraLookAt[i] - rc->m_cameraPos[i];

    Normalize(v);

    for (i = 0; i < 3; i++)
        v[i] *= speed;

    // MdtBodySetLinearVelocity(project, v[0], v[1]/2, v[2]);

    const MeReal *lv = coneG->m_matrix+8;
    MdtBodySetLinearVelocity(project,lv[0]*speed, lv[1]*speed, lv[2]*speed );

    MdtBodySetAngularVelocity(project, 1, 1, 1);
}


void updateFromGraphics()
{
  MdtBodySetTransform(project,(MeMatrix4Ptr)McdModelGetUserData(projectCM));
  MdtBodySetTransform(target,(MeMatrix4Ptr)McdModelGetUserData(targetCM));
}


void updateToGraphics()
{
  MdtBodyGetTransform(project, (MeMatrix4Ptr)McdModelGetUserData(projectCM));
  MdtBodyGetTransform(target, (MeMatrix4Ptr)McdModelGetUserData(targetCM));
}


void Shoot(void)
{

    const MeReal *p = coneG->m_matrix+12;
    MdtBodySetPosition(project, p[0], p[1], p[2]);
    // updateToGraphics:
    //MeVector3Copy( (MeReal*)McdModelGetUserData(projectCM)+12, p);
    updateToGraphics();

    const MeReal *lv = coneG->m_matrix+8;
    MdtBodySetLinearVelocity(project,lv[0]*speed, lv[1]*speed, lv[2]*speed );

    MdtBodySetAngularVelocity(project, 0, 200, 0);

    MdtBodySetQuaternion(project, 1, 0, 0, 0 );
}


void reset(void) {
    MeMatrix4 m;
    MeMatrix4TMMakeIdentity(m);

    MdtBodySetTransform(target,m);
    MdtBodySetPosition(target, 1.5f, (MeReal)5.01, 0);

    // MdtBodySetQuaternion(target, 1, 0, 0, 0 );
    MdtBodySetAngularVelocity(target, 0,0,0);
    MdtBodySetLinearVelocity(target, 0,0,0);

    MdtBodySetPosition(project, 0, 3, -2);
    MdtBodySetQuaternion(project, 1, 0, 0, 0 );
    MdtBodySetAngularVelocity(project, 0,0,0);
    MdtBodySetLinearVelocity(project, 0,0,0);
    updateToGraphics();
}


void stepEvolve(void) {
  McdSpaceUpdateAll(space);
  MstBridgeUpdateContacts(bridge, space, world);

  if (autoEvolve) {

     MdtWorldStep(world, step);

   }

   else {

     updateFromGraphics();

   }

  // MstStep(world,space,bridge,step);

  if (autoEvolve == -1) autoEvolve = 0;
}

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
MeReal oldstep;
int STS = 0;    // SafeTimeSolution

void toggleSafeTimeSolution() {
  STS = !STS;
  if (STS)
    printf("Safe time solution is ON\n");
  else
    printf("Safe time solution is OFF\n");
}

void tick(RRender * rc) {

  MeVector3 v;
  MdtBodyGetLinearVelocity(project, v);
  MeVector3Copy( VelocitySeg1.p1, (MeReal*)MdtBodyGetTransformPtr(project)+12);
  MeVector3Add( VelocitySeg1.p2 , VelocitySeg1.p1,  v);

  McdSafeTimeResult r;
  MeReal t = step;

  oldstep = step;
  McdHello(p);
  int retval = McdSafeTime(p,step,&r);
  McdGoodbye(p);
  if (t > r.time) t = r.time;


  McdSafeTimeResult r_plane;
  McdHello(p_plane);
  McdSafeTime(p_plane,step,&r_plane);
  McdGoodbye(p_plane);

  if (t > r_plane.time) t = r_plane.time;
  if (t<ME_SMALL_EPSILON) t=ME_SMALL_EPSILON;

  if (STS) {
    if (t > ME_SMALL_EPSILON && t<step ) {
      step = t;
      printf("step = %g\n", (float)step);
    }
  }

  MeMatrix4Copy(safeXform, MdtBodyGetTransformPtr(project));
  MeReal* vel_ptr = McdModelGetLinearVelocityPtr(projectCM);
  MeReal* angvel_ptr = McdModelGetAngularVelocityPtr(projectCM);
  MeVector3 vel;
  MeVector3Copy(vel, vel_ptr);
  MeVector3Scale(vel, t);

  /* display contacts at intersection time: */
  McdIntersectResult ir;
  McdContact contacts[10]; ir.contacts = contacts; ir.contactMaxCount = 10;
  McdIntersectAt(p, &ir, t+(MeReal)0.0001);
  McduCollectContactPairs(&ir,0);

  MeMatrix4TMUpdateFromVelocities(safeXform, (MeReal)1e-4, t, vel_ptr,  angvel_ptr, MdtBodyGetTransformPtr(project));

  stepEvolve();
  UpdateAABB();

  /* set back to normal timestep */
  step=oldstep;

#ifdef WITH_GLUI
  glui->sync_live();
#endif
}

void singleStep(void)
{
  autoEvolve = -1;
}

void cleanup(void)
{
    RDeleteRenderContext(rc);
    MdtWorldDestroy(world);
    MstBridgeDestroy(bridge);
    McdTerm();
}

void incStep(void)
{
   step += (MeReal)0.01;
   printf("Step is %g,\t  Speed is %g\n", step, speed);
}

void decStep(void)
{
   step -= (MeReal)0.01;
   printf("Step is %g,\t  Speed is %g\n", step, speed);
}

void incSpeed(void)
{
   speed += 5;
   printf("Step is %g,\t  Speed is %g\n", step, speed);
}

void decSpeed(void)
{
   speed -= 5;
   printf("Step is %g,\t  Speed is %g\n", step, speed);
}

int main(int argc, const char **argv)
{
  MeReal *tm;
  MdtBclContactParams *params;
  static float color[4][3] = { {0, 0.2f, 1}, {0.8f, 0.8f, 0.1f}, {0, 1, 1}, {1, 0, 0}};

  const RRenderType render = RParseRenderType(&argc, &argv);
  rc = RNewRenderContext(render, kRQualitySmooth);

/*
  Dynamics
  create and initialize a dynamics world
*/
  world = MdtWorldCreate(2, 200);

  MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
/*
  create physical bodies
*/

  target = MdtBodyCreate(world);

  MdtBodyEnable(target);
  MdtBodySetAngularVelocityDamping(target, 0.6f);
  MdtBodySetLinearVelocityDamping(target, 0.3f);
    // MdtBodySetPosition(target, 0, 5.01, 0);
  MdtBodySetMass(target, 1.0f);

  project = MdtBodyCreate(world);

  MdtBodyEnable(project);
  MdtBodySetAngularVelocityDamping(project, 0.6f);
  MdtBodySetLinearVelocityDamping(project, 0.3f);
// MdtBodySetPosition(project, 0, 3, -2);
  MdtBodySetMass(project, 1.0f);

/*
  Collision Detection
  Collision initialization.
*/
  McdInit(McdPrimitivesGetTypeCount()+2, 100);
  McdPrimitivesRegisterTypes();
  McdPrimitivesRegisterInteractions();
/* McdConvexMeshRegisterBoxAndSphereFns( ); */ /* HERE */


/*
  Create a collision space.
*/
  space = McdSpaceAxisSortCreate(McdAllAxes, 50, 100, 1);

  bridge = MstBridgeCreate(1);
  MstSetWorldHandlers(world);
  /* use swept volume AABB instead of regular AABB */
  //McdSpaceSetAABBFn(space, McdModelGetPathAABB);

  if (BoxesNotBalls[targetIdx]) {
    targetGeom = McdBoxCreate(0.5f, 5.0f, 0.5f);
    targetCM = McdModelCreate(targetGeom);
  } else {
    targetGeom = McdSphereCreate(0.5f);
    targetCM = McdModelCreate(targetGeom);
  }

  if (BoxesNotBalls[projectileIdx]) {
    projectGeom = McdBoxCreate(5.0f, 0.5f, 0.5f);
    projectCM= McdModelCreate(projectGeom);
  } else {
    projectGeom = McdSphereCreate(0.5f);
    projectCM = McdModelCreate(projectGeom);
  }

  McdSpaceInsertModel(space, targetCM);
  McdModelSetBody(targetCM, target);
  InitAABB(rc,targetCM);

  McdSpaceInsertModel(space, projectCM);
  McdModelSetBody(projectCM, project);
  InitAABB(rc,projectCM);

  plane_prim = McdPlaneCreate();

  planeCM = McdModelCreate(plane_prim);
  McdModelSetTransformPtr(planeCM, tmPlane);
  McdSpaceInsertModel(space, planeCM);

  params = MstBridgeGetContactParams(bridge, 0, 0);
  params->type = MdtContactTypeFriction2D;
  params->friction1 = 1.0f;
  params->friction2 = 1.0f;

  p = McdModelPairCreate(projectCM,targetCM);
  p_plane = McdModelPairCreate( projectCM,planeCM);


/* Rendering */

  if (BoxesNotBalls[targetIdx]) {
    tm = (MeReal *) McdModelGetTransformPtr(targetCM);
    targetG = RCreateCube(rc, 0.5f, 5.0f, 0.5f, color[0], tm);
  } else {
    tm = (MeReal *) McdModelGetTransformPtr(targetCM);
    targetG = RCreateSphere(rc, 0.5, color[0], tm);
  }

  if (BoxesNotBalls[projectileIdx]) {
    tm = (MeReal *) McdModelGetTransformPtr(projectCM);
    projectG = RCreateCube(rc, 5.0f, 0.5f, 0.5f, color[1], tm);
    MeMatrix4TMMakeIdentity(safeXform);
    safeBall = RCreateCube(rc, 5.0f, 0.5f, 0.5f, color[3], (AcmeReal*)safeXform);
  } else {
    tm = (MeReal *) McdModelGetTransformPtr(projectCM);
    projectG = RCreateSphere(rc, 0.5, color[1], tm);
    MeMatrix4TMMakeIdentity(safeXform);
    safeBall = RCreateSphere(rc, 0.5, color[3], (AcmeReal*)safeXform);
  }

  // set user data to point to graphics transform, for updateFromGraphics()
  McdModelSetUserData(targetCM, (void*)&(targetG->m_matrixArray[0]));
  McdModelSetUserData(projectCM, (void*)&(projectG->m_matrixArray[0]));

  reset();

  planeG = RCreateCube(rc, 14, 14, 0.1f, color[2], (MeReal *) tmPlane);
    // RSetTexture(planeG, "checkerboard");

  coneG = RCreateCone(rc, 0.2f, 0.0, 2, 0.0, color[1], 0);
  MeVector3Copy((MeReal*)(coneG)->m_matrix+12, ballInitPos);

  lineG = RCreateUserProceduralObject(rc, McduDrawLineSeg,
                      (void*) &VelocitySeg1,
                      "velocity",1,color[2],0);

/* Set up camera.  */
  rc->m_cameraAngle = 0;
  rc->m_cameraElevation = 0.5f;

  RUpdateCamera();

#ifndef PS2
  MstBridgeSetPerPairCB(bridge, 0,0, McduCollectContactPairs);
  McduMcdContactGraphics = McduCreateMcdContactGraphics(rc,100);
  McduToggleMcdContactDrawing();
  RUseKey('a', toggleSafeTimeSolution);
  RUseKey('b', toggleBenchmark);
  RUseKey('p', (RKeyCallback) toggleAutoEvolve);
  RUseKey('s', (RKeyCallback) singleStep);
  RUseKey('+', incStep);
  RUseKey('-', decStep); RUseKey('=', incStep);
  RUseKey(']', incSpeed);
  RUseKey('[', decSpeed);
  RUseKey(' ', Shoot);
  RUseKey((char)13, reset);
#else
  RUsePadKey(PADcircle, Shoot);
#endif

  RCreateUserHelp(help, 1);
/*
  Cleanup after simulation.
*/
  atexit(cleanup);
/*
  Run the Simulation
*/

#ifdef WITH_GLUI
glui = GLUI_Master.create_glui("Glui");
glui->set_main_gfx_window(rc->m_windowID);
glui->add_checkbox("Autoevolve", &autoEvolve);
glui->add_spinner("Speed", GLUI_SPINNER_FLOAT, (void*)&speed);
//glui->add_rotation("Initial velocity", (float*)coneG->m_matrix);
#endif

  RRun(rc, tick);

  return 0;
}
