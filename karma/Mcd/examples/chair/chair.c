/*
  Copyright (c) 1997-2002 MathEngine PLC
  www.mathengine.com

  $Name: t-stevet-RWSpre-030110 $

  $Id: chair.c,v 1.12.2.1 2002/04/04 15:28:50 richardm Exp $
*/

/*
  Overview:

  This program demonstrates how to create a geometrical composite
  object (chair) using a set of primitives available in the collision
  toolkit.
*/

#include <stdlib.h>
#include <stdio.h>

#include <MeMath.h>
#include <MeViewer.h>
#include <MeApp.h>
#include <McdCompositeModel.h>

/* number of box parts in the chair */
#define NBoxes        8
/* number of  shooting balls */
#define NBalls        2
/* number of  Crosses */
#define NCrosses      40
/* maximum number of contacts */
#define NContacts   200


MdtWorldID world;
McdSpaceID space;
MstBridgeID bridge;

/*
  Physics representations.
*/

MdtBodyID chair;
MdtBodyID ball[NBalls];
MdtBodyID crosses[NCrosses];
MdtBodyID sticks[NBoxes];

/*
  Graphics representations.
*/

RGraphic  *boxG[NBoxes];
RGraphic  *ballG[NBalls];
RGraphic  *planeG;
RRender   *rc = 0;
static float color[3] = { 1, 1, 1 };

MeApp     *meapp;

/*
  Collision representations.
*/

McdModelID chairCM;
McdModelID crossCM[NCrosses];
McdModelID ballCM[NBalls];
McdModelID planeCM;
McdModelID legCM[NBoxes];
int ImAlreadySmashed = 0;
MeMatrix4 tmRel[NBoxes];
MeMatrix4 tmSpace[NBoxes];

int ballHitsChair = 0;

/* Collision geometry ID (box) */
McdGeometryID box_prim[NBoxes];
McdGeometryID cross_prim[NCrosses];
/* Collision geometry ID (ball) */
McdGeometryID ball_prim = 0;
McdGeometryID plane_prim = 0;

MeReal step = (MeReal) 0.02;
MeReal gravity[3] = { 0, -6, 0 };

MeMatrix4 tmPlane = {
  {  1,  0,  0,  0},
  {  0,  0, -1,  0},
  {  0,  1,  0,  0},
  {  0,  0,  0,  1}
};


char *help[] = { "Aim with camera. Press $ACTION3  to shoot. \nPress $ACTION2 to shoot wrecking ball. Press $ACTION4 to break." };

void MEAPI Shoot(RRender* rc, void* userData) {
    int i;
    MeReal v[3];
    MeVector3 cam_pos, cam_lookat;
    int id = (int)userData;

    RCameraGetPosition(rc, cam_pos);
    RCameraGetLookAt(rc, cam_lookat);

    MdtBodyEnable(ball[id]);

    MdtBodySetPosition(ball[id],
        cam_pos[0], cam_pos[1] + 1.0f, cam_pos[2]);

    for (i = 0; i < 3; i++)
        v[i] = cam_lookat[i] - cam_pos[i];

    MeVector3Normalize(v);

    for (i = 0; i < 3; i++)
        v[i] *= 12.0;

    MdtBodySetLinearVelocity(ball[id], v[0], v[1], v[2]);
    MdtBodySetAngularVelocity(ball[id], 0, 0, 0);
}

void MEAPI SmashCallback(McdIntersectResult* r) {
  if ((r->pair->model1 == chairCM && r->pair->model2 == ballCM[1]) ||
      (r->pair->model2 == chairCM && r->pair->model1 == ballCM[1]))
  {
    ballHitsChair++;
  }
}

void MEAPI SmashComposite(RRender* rc, void* userData) {
/*    McdModelID model = (McdModelID)userData; */
  int i, numPieces = McdCompositeModelGetElementCount(chairCM);
  MeMatrix4Ptr modelXform;
  MeVector3 linearVel, angularVel;

  if(!ImAlreadySmashed) {
    MdtBodyGetLinearVelocity(chair, linearVel);
    MdtBodyGetAngularVelocity(chair, angularVel);

    McdCompositeModelDecompose(chairCM);

    for(i=0; i < numPieces; i++) {
    /* sticks[i] = MdtBodyCreate(world); */
      MdtBodyEnable(sticks[i]);
      modelXform = McdModelGetTransformPtr(legCM[i]);
      MdtBodySetTransform(sticks[i], modelXform);
      modelXform = MdtBodyGetTransformPtr(sticks[i]);
      RGraphicSetTransformPtr(boxG[i], modelXform);
      MdtBodySetAngularVelocity(sticks[i], angularVel[0], angularVel[1], angularVel[2]);
      McdModelSetBody(legCM[i], sticks[i]);
      McdSpaceInsertModel(space, legCM[i]);
    }
    McdSpaceRemoveModel(chairCM);
    ImAlreadySmashed = 1;
  }
}

void MEAPI CreateChair(RRender* rc, McdModelID model) {
  int i;
  static MeReal transl[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
  MeMatrix4Ptr tm;
    /*
      Initialze useful transformation matrices
    */
  for (i=0; i<NBoxes; i++) {
    MeMatrix4TMMakeIdentity(tmRel[i]);
    MeMatrix4TMMakeIdentity(tmSpace[i]);
  }

    /*  The Seat */
  box_prim[0] = McdBoxCreate(2.4f, 0.3f, 2.4f);
  legCM[0] = McdModelCreate(box_prim[0]);
  MeMatrix4TMSetPosition(tmRel[0], 0.f, 0.5f, 0.f);
  McdModelSetTransformPtr( legCM[0], tmSpace[0]);
  McdCompositeModelAddElement(model, legCM[0], tmRel[0]);

    /*  The Back */
  box_prim[1] = McdBoxCreate(0.4f, 1.6f, 0.2f);
  legCM[1] = McdModelCreate(box_prim[1]);
  MeMatrix4TMSetPosition(tmRel[1], -1.0f, 1.4f, 1.1f);
  McdModelSetTransformPtr( legCM[1], tmSpace[1]);
  McdCompositeModelAddElement(model, legCM[1], tmRel[1]);

  box_prim[2] = McdBoxCreate(0.4f, 1.6f, 0.2f);
  legCM[2] = McdModelCreate(box_prim[2]);
  MeMatrix4TMSetPosition(tmRel[2], 1.0f, 1.4f, 1.1f);
  McdModelSetTransformPtr( legCM[2], tmSpace[2]);
  McdCompositeModelAddElement(model, legCM[2], tmRel[2]);

  box_prim[3] = McdBoxCreate(1.6f, 0.2f, 0.2f);
  legCM[3] = McdModelCreate(box_prim[3]);
  MeMatrix4TMSetPosition(tmRel[3], 0.f, 2.1f, 1.1f);
  McdModelSetTransformPtr( legCM[3], tmSpace[3]);
  McdCompositeModelAddElement(model, legCM[3], tmRel[3]);

    /* The Four legs */
  for (i = 4; i < NBoxes; i++) { /* Relative to the origin of the Chair Model */
    box_prim[i] = McdBoxCreate(0.4f, 2.0f, 0.4f);
    legCM[i] = McdModelCreate(box_prim[i]);
    MeMatrix4TMSetPosition(tmRel[i], transl[i - 4][0], -0.5f, transl[i - 4][1]);
    McdModelSetTransformPtr(legCM[i], tmSpace[i]);
    McdCompositeModelAddElement(model, legCM[i], tmRel[i]);
  }

    /*
      Graphics for each object.
    */
    /*  the seat */
  tm = McdModelGetTransformPtr(legCM[0]);
  boxG[0] = RGraphicBoxCreate(rc, 2.4f, 0.3f, 2.4f, color, tm);
  RGraphicSetTexture(rc, boxG[0], "wood1");

    /*  the back */
  tm = McdModelGetTransformPtr(legCM[1]);
  boxG[1] = RGraphicBoxCreate(rc, 0.4f, 1.6f, 0.2f, color, tm);
  RGraphicSetTexture(rc, boxG[1], "wood1");

  tm = McdModelGetTransformPtr(legCM[2]);
  boxG[2] = RGraphicBoxCreate(rc, 0.4f, 1.6f, 0.2f, color, tm);
  RGraphicSetTexture(rc, boxG[2], "wood1");

  tm = McdModelGetTransformPtr(legCM[3]);
  boxG[3] = RGraphicBoxCreate(rc, 1.6f, 0.2f, 0.2f, color, tm);
  RGraphicSetTexture(rc, boxG[3], "wood1");

    /*  the legs */
  for (i = 4; i < NBoxes; i++) {
    tm = McdModelGetTransformPtr(legCM[i]);
    boxG[i] = RGraphicBoxCreate(rc, 0.4f, 2, 0.4f, color, tm);
    RGraphicSetTexture(rc, boxG[i], "wood1");
  }
}

void MEAPI CreateCross(RRender* rc, McdModelID model) {
  int i;
  MeMatrix4Ptr tm;
  McdBoxID* xBoxGeo;
  McdModelID* xCM;
  MeMatrix4* xOrient;
  MeMatrix4* xTM;
  RGraphic** xGraphic;

  xBoxGeo  = (McdBoxID*)   malloc(3 * sizeof(McdBoxID));
  xCM      = (McdModelID*) malloc(3 * sizeof(McdModelID));
  xOrient  = (MeMatrix4*)  malloc(3 * sizeof(MeMatrix4));
  xTM      = (MeMatrix4*)  malloc(3 * sizeof(MeMatrix4));
  xGraphic = (RGraphic**)  malloc(3 * sizeof(RGraphic*));

  for (i=0; i < 3 ; i++) {
    MeMatrix4TMMakeIdentity(xOrient[i]);
    MeMatrix4TMMakeIdentity(xTM[i]);
  }

  xBoxGeo[0] = McdBoxCreate(2.4f, 0.3f, 0.3f);
  xCM[0] = McdModelCreate(xBoxGeo[0]);
  MeMatrix4TMSetPosition(xOrient[0], 0.f, 0.0f, 0.f);
  McdModelSetTransformPtr( xCM[0], xTM[0]);
  McdCompositeModelAddElement(model, xCM[0], xOrient[0]);

  xBoxGeo[1] = McdBoxCreate(0.3f, 2.4f, 0.3f);
  xCM[1] = McdModelCreate(xBoxGeo[1]);
  MeMatrix4TMSetPosition(xOrient[1], 0.f, 0.f, 0.f);
  McdModelSetTransformPtr( xCM[1], xTM[1]);
  McdCompositeModelAddElement(model, xCM[1], xOrient[1]);

  xBoxGeo[2] = McdBoxCreate(0.3f, 0.3f, 2.4f);
  xCM[2] = McdModelCreate(xBoxGeo[2]);
  MeMatrix4TMSetPosition(xOrient[2], 0.f, 0.f, 0.f);
  McdModelSetTransformPtr( xCM[2], xTM[2]);
  McdCompositeModelAddElement(model, xCM[2], xOrient[2]);

    /*
      Graphics for each object.
    */
  tm = McdModelGetTransformPtr(xCM[0]);
  xGraphic[0] = RGraphicBoxCreate(rc, 2.4f, 0.3f, 0.3f, color, tm);
  RGraphicSetTexture(rc, xGraphic[0], "wood1");

  tm = McdModelGetTransformPtr(xCM[1]);
  xGraphic[1] = RGraphicBoxCreate(rc, 0.3f, 2.4f, 0.3f, color, tm);
  RGraphicSetTexture(rc, xGraphic[1], "wood");

  tm = McdModelGetTransformPtr(xCM[2]);
  xGraphic[2] = RGraphicBoxCreate(rc, 0.3f, 0.3f, 2.4f, color, tm);
  RGraphicSetTexture(rc, xGraphic[2], "wood");
}

void stepEvolve() {
  McdSpaceUpdateAll(space);
  MstBridgeUpdateContacts(bridge,space,world);
  MdtWorldStep(world,step);
  if (ballHitsChair == 1) { SmashComposite(0,0); ballHitsChair++; }
}

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender * rc,void *userdata) {
    stepEvolve();
}

void cleanup(void) {
  int i, numPieces = McdCompositeModelGetElementCount(chairCM);

  MeAppDestroy(meapp);
    /* graphics */
  RRenderContextDestroy(rc);

    /* dynamics */
  MdtWorldDestroy(world);

    /* collision */
  for (i = 0; i < numPieces; i++) {
    McdGeometryDestroy(box_prim[i]);
    McdModelDestroy(legCM[i]);
  }
  McdGeometryDestroy(ball_prim);
  for (i = 0; i < NBalls; i++) McdModelDestroy(ballCM[i]);
  McdGeometryDestroy(plane_prim);
  McdModelDestroy(planeCM);
  McdModelDestroy(chairCM);

  McdSpaceDestroy(space);
  MstBridgeDestroy(bridge);
  McdTerm();
}


int main(int argc, const char **argv) {
  int i;
  MeMatrix4Ptr tm;
  MdtContactParamsID params;

  MeCommandLineOptions options;
  RParseRenderType(&argc, &argv, &options);
  rc = RRenderContextCreate(options.m_argc, kRQualitySmooth);
  if (!rc)
    return 0;

    /*
      Create and initialize a dynamics world.
    */
  world = MdtWorldCreate(1 + NBoxes + NBalls + NCrosses, 200);
  MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    /*
      Create physical bodies.
    */
  chair = MdtBodyCreate(world);
  MdtBodyEnable(chair);
  MdtBodySetPosition(chair, 0, 4, 0);

  for(i=0; i < NCrosses; i++) {
    crosses[i] = MdtBodyCreate(world);
    MdtBodyEnable(crosses[i]);
    MdtBodySetPosition(crosses[i], 0, 7+3*i, 0);
    MdtBodySetAngularVelocity(crosses[i], (i-NCrosses/2), -1*(i-NCrosses/2), 0);
  }

  for (i=0; i < NBalls; i++) {
    ball[i] = MdtBodyCreate(world);
    MdtBodyEnable(ball[i]);
    MdtBodySetPosition(ball[i], -6 - i*3.0f, 3, 0);
    MdtBodySetMass(ball[i], (i+1)*1.0f);
  }

  for(i=0; i < NBoxes; i++) {
    sticks[i] = MdtBodyCreate(world);
  }

    /*
      Collision initialization.
    */
  McdInit(McdPrimitivesGetTypeCount()+1, 200,1);
  McdPrimitivesRegisterTypes();
  McdPrimitivesRegisterInteractions();
  McdCompositeRegisterType();
  McdCompositeGenericRegisterInteractions();

    /*
      Create a collision space.
    */
  space = McdSpaceAxisSortCreate(McdAllAxes, 50, 100, 1);
  bridge = MstBridgeCreate(10);
  MstSetWorldHandlers(world);
  MstBridgeSetIntersectCB(bridge,0,0,SmashCallback);

    /*
      Create the Composite Models
    */
  chairCM = McdCompositeModelCreate(NBoxes*2);
  McdModelSetBody(chairCM, chair);
  McdSpaceInsertModel(space, chairCM);

  CreateChair(rc, chairCM);

  for(i=0; i < NCrosses; i++) {
    crossCM[i] = McdCompositeModelCreate(NCrosses+1);
    McdModelSetBody(crossCM[i], crosses[i]);
    McdSpaceInsertModel(space, crossCM[i]);
    CreateCross(rc, crossCM[i]);
  }
    /*
      Create collision objects for the balls
    */
  ball_prim = McdSphereCreate(0.5f);

  for (i=0; i< NBalls; i++) {
    ballCM[i] = McdModelCreate(ball_prim);
    McdSpaceInsertModel(space, ballCM[i]);
    McdModelSetBody(ballCM[i], ball[i]);
  }

  plane_prim = McdPlaneCreate();

  planeCM = McdModelCreate(plane_prim);
  McdModelSetTransformPtr(planeCM, tmPlane);
  McdSpaceInsertModel(space, planeCM);
  McdSpaceUpdateModel(planeCM);
  McdSpaceFreezeModel(planeCM);

    /*
      Set parameters for contacts.
    */
  params = MstBridgeGetContactParams(bridge,
                     MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

  MdtContactParamsSetType(params, MdtContactTypeFriction2D);
  MdtContactParamsSetFriction(params, 3.0);
  MdtContactParamsSetRestitution(params, (MeReal)0.3);

    /* render the firing spheres */
  for (i = 0; i < NBalls; i++) {
    tm = MdtBodyGetTransformPtr(ball[i]);
    ballG[i] = RGraphicSphereCreate(rc, 0.5, color, tm);
    if (i==1) RGraphicSetTexture(rc, ballG[i], "rock");
    else RGraphicSetTexture(rc, ballG[i], "ME_ball3");
  }

  planeG = RGraphicGroundPlaneCreate(rc, 20, 2, color, 0);
  RGraphicSetTexture(rc, planeG, "checkerboard");

    /*
      Set up camera.
    */
  RCameraRotateAngle( rc, 0);
  RCameraRotateElevation( rc, 0.5f);

  RRenderCreateUserHelp(rc, help, 1);
  RRenderToggleUserHelp(rc);
  RRenderSetWindowTitle(rc,"Chair tutorial");

  RRenderSetActionNCallBack(rc, 3, Shoot, (void*)0);
  RRenderSetActionNCallBack(rc, 2, Shoot, (void*)1);
  RRenderSetActionNCallBack(rc, 4, SmashComposite, 0);

  RPerformanceBarCreate(rc);
  meapp = MeAppCreate(world, space, rc);

  RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);

    /*
      Cleanup after simulation.
    */
#ifndef PS2
  atexit(cleanup);
#endif
    /*
      Run the Simulation.
    */
  RRun(rc, tick, 0);

  return 0;
}
