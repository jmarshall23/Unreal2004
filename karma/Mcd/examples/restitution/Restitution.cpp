/*
  Copyright (c) 1997-2002 MathEngine PLC

  $Name: t-stevet-RWSpre-030110 $

  $Id: Restitution.cpp,v 1.24.10.1 2002/04/04 15:28:51 richardm Exp $
*/

/*
  This example shows the collision detection, Dynamics Event Manager
  and Kea Dynamics working on a complicated environment.


  You can 'kill' all the objects in the scene using 'k'. They will be
  re-awakened if you hit them with a ball, or you can press 'w' to wake
  everything.

  Pressing F8 to disable draw-lists in the renderer lets you see which
  objects are currently active (they go dark).
*/


#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <Mdt.h>

#include <McdFrame.h>
#include <McdPrimitives.h>
#include <Mst.h>

#include <MeViewer.h>
#include <MeMath.h>
#include <McduDrawMcdContacts.h>
#include <MePrecision.h>
#include <MeMessage.h>
#include <MeProfile.h>
#include <MdtTypes.h>

#include <McduDrawConvexMesh.h>
#include <McdCylinder.h>
#include <McdConvexMesh.h>
#include <GL/gl.h>

#define WITH_OPENGL
#pragma warning( disable : 4305 )

/*
  Global declarations
*/
#define COLOR1 {0.0 , 0.73, 0.73}
#define COLOR2 {0.0 , 0.4 , 1.0 }
#define COLOR3 {1.0 , 0.0 , 0.5 }
#define COLOR4 {1.0 , 0.6 , 0.0 }
#define COLOR5 {1.0,  0.4 , 0.0 }
#define COLOR6 {0.6 , 0.4 , 1.0 }

#define NBoxes      1

MeReal boxDensity = (MeReal)(1.);

/*
  Radius of each box.
*/
#define BOX
#ifdef BOX
MeReal boxDim[NBoxes][3] = { {0.05, 1.5, 0.05} };
#else
MeReal boxDim[NBoxes][3] = { {1.0, 1.0, 1.0} };
#endif


/*
  Position of each box.
*/
MeReal boxPos[NBoxes][3] = { {0.1, 1.35, -2.0} };


/*
  Color of each box.
*/
MeReal boxColor[NBoxes][3] = { COLOR5 };

MdtWorldID world;
MstBridge *bridge;
McdSpace *space;

/*
  Physics representations.
*/
MdtBodyID box[NBoxes];

/*
  Graphical representations.
*/
RGraphic *groundG;
RGraphic *boxG[NBoxes];

/*
  Collision reps.
*/
McdModelID groundCM;
McdModelID boxCM[NBoxes];

MeVector3 gravity = { 0, -10.0f, 0 };

RRender *rc;

const int gMaxPairs = 10;
const int gMaxContacts = 50;

unsigned int fireDelay = 40;
unsigned int fireFrames = 0;

int autoEvolve = 1;
int enableFriction = 1;
int preEvolve = 0;

MeReal step = (MeReal)(0.01);
MeReal gStep = (MeReal)(0.01);

McdContact    gContacts[50];
McdModelPair *gPairs[10];
MeReal        gEpsilonForPenetration = 0.15;


MeMatrix4 groundTransform = { 1, 0, 0, 0,
                  0, 0,-1, 0,
                  0, 1, 0, 0,
                  0,-1, 0, 1};

MeMatrix4 groundRenderTransform = { 1, 0,    0, 0,
                    0, 0,   -1, 0,
                    0, 1,    0, 0,
                    0,-1.05, 0, 1};

void stepEvolve(void);

void Normalize(MeReal *v)
{
    MeReal norm = MeRecipSqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    v[0] *= norm;
    v[1] *= norm;
    v[2] *= norm;
}

void toggleAutoEvolve(void)
{
    autoEvolve = !autoEvolve;
}

MeReal drand48()
{
  return 14.0f;
}

void shoot(void)
{
  MeReal w[3];
  MeReal e[4];
  MeReal n=0.0;
  MeReal z = 0.0;
  int i;
  for(i=0;i<3;++i) { w[i] = 5*drand48(); }
  for(i=0;i<4;++i) { e[i] = 5*drand48(); n+=e[i]*e[i]; }

  n = MeRecipSqrt(n);
  for(i=0;i<4;++i) { e[i] *= n;}

  MdtBodySetPosition(box[0], (MeReal)0, (MeReal)10, (MeReal)5);
  #ifdef BOX

  MdtBodySetAngularVelocity(box[0], w[0], w[1], w[2]);
  MdtBodySetQuaternion(box[0], 1, 0, 0, 0);
  #else
  MdtBodySetAngularVelocity(box[0], w[0], w[1], w[2]);
  MdtBodySetLinearVelocity(box[0], z, -5, z);
  MdtBodySetQuaternion(box[0], 1.0, z, z, z);
  #endif


  MdtBodySetLinearVelocity(box[0], z, z, z);

}


MeReal leastSeparation;

void MEAPI checkSeparation(McdIntersectResult *ir) {
  int i;
  for(i=0;i<ir->contactCount;i++)
  {
    if(ir->contacts[i].separation < leastSeparation)
      leastSeparation = ir->contacts[i].separation;
    printf(" \n separation = %f \n", ir->contacts[i].separation);

  }
}

void stepEvolve(void)
{
  leastSeparation = 0.0;
  McdSpaceUpdateAll(space);
  MstHandleContacts(world,space,bridge);
  if(leastSeparation >= -gEpsilonForPenetration ) {
    MdtWorldStep(world,step);
  }
}

/*
  tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds.
*/
void tick(RRender * rc)
{
    if (autoEvolve)
        stepEvolve();

    if (fireFrames == 50)
        shoot();

    fireFrames++;
}

/*
  Main routine.
*/

int main(int argc, const char **argv)
{
    const RRenderType render = RParseRenderType(&argc, &argv);

    int i;
    float color[3];
    MeReal mass;
    MeMatrix3 I[NBoxes];
    MeMatrix4 TMIdent;

    McdGeometry *plane_prim, *box_prim;
    MdtBclContactParams *props;

    static char *help[] =
    {
        "Left Mouse Drag - move camera",
        "          Space - fire!",
        "              a - toggle auto-evolve",
        "              s - step evolve",
        "              k - kill all objects",
        "              w - wake all objects",
        "              f - toggle friction",
        "              F8- toggles greying of inactive bodies"
    };
    const int helpNum = sizeof (help) / sizeof (help[0]);

    MeMatrix4TMMakeIdentity(TMIdent);

/**************************************************/
      /*
        Initialise dynamics.
      */
    world = MdtWorldCreate(NBoxes, 100);

    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    /*
      BOXES:
    */

    for (i = 0; i < NBoxes; i++)
    {
        box[i] = MdtBodyCreate(world);
#ifdef BOX
        mass = boxDensity * 2 * boxDim[i][0] * 2 * boxDim[i][1] * 2 *
            boxDim[i][2];

        MdtMakeInertiaTensorBox(mass, 2 * boxDim[i][0],
                                      2 * boxDim[i][1],
                      2 * boxDim[i][2], I[i]);
        MdtBodySetMass(box[i], mass);

        MdtBodySetInertiaTensor(box[i], I[i]);

        MdtBodySetPosition(box[i], boxPos[i][0], boxPos[i][1], boxPos[i][2]);
#else
        mass = boxDensity * boxDim[i][0] * boxDim[i][1] * boxDim[i][2]*ME_PI/3;
    MdtMakeInertiaTensorSphere(mass, boxDim[i][0], I[i]);
        MdtBodySetMass(box[i], mass);
        MdtBodySetInertiaTensor(box[i], I[i]);
        MdtBodySetPosition(box[i], boxPos[i][0], boxPos[i][1], boxPos[i][2]);
#endif
    }

    for (i = 0; i < NBoxes; i++)
        MdtBodyEnable(box[i]);

/**************************************************/
      /*
        Collision detection.
      */

    McdInit(McdPrimitivesGetTypeCount(), 100);

    McdPrimitivesRegisterTypes();
    McdPrimitivesRegisterInteractions();

    /* max objects and pairs */
    space = McdSpaceAxisSortCreate(McdAllAxes, 50, 150, 1);

    bridge = MstBridgeCreate(10);
    MstSetWorldHandlers(world);

    MstBridgeSetIntersectCB(bridge,MstBridgeGetDefaultMaterial(),
        MstBridgeGetDefaultMaterial(), checkSeparation);

    /*
      Set parameters for contacts.
    */
    props = MstBridgeGetContactParams(bridge,
        MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsSetType(props, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(props, (MeReal)2.0);
    MdtContactParamsSetRestitution(props, (MeReal)0.9);
    MdtContactParamsSetSoftness(props, (MeReal)0.0001);

    /*
      Only use 3 contacts per pair.
    */
    McdGetDefaultRequestPtr()->contactMaxCount = 4;

    plane_prim = McdPlaneCreate();
    groundCM = McdModelCreate(plane_prim);
    McdSpaceInsertModel(space, groundCM);

    McdModelSetTransformPtr(groundCM, groundTransform);

    McdSpaceUpdateAll(space);
    McdSpaceFreezeModel(groundCM);

    for (i = 0; i < NBoxes; i++) {
#ifdef BOX
      box_prim = McdBoxCreate(2*boxDim[i][0], 2*boxDim[i][1], 2*boxDim[i][2]);
#else
      box_prim = McdSphereCreate(2*boxDim[i][0]);
#endif
        boxCM[i] = McdModelCreate(box_prim);
        McdModelSetBody(boxCM[i], box[i]);
        McdSpaceInsertModel(space, boxCM[i]);

        mass = boxDensity * 2 * boxDim[i][0] * 2 * boxDim[i][1] * 2 *
            boxDim[i][2];

        MdtMakeInertiaTensorBox(mass, 2 * boxDim[i][0],
                                      2 * boxDim[i][1],
                      2 * boxDim[i][2], I[i]);

        MdtBodySetMass(box[i], mass);

        MdtBodySetInertiaTensor(box[i], I[i]);

    }



/**************************************************/
      /*
        Initialise rendering attributes.
      */
    rc = RNewRenderContext(render, kRQualitySmooth);
    rc->m_cameraOffset = 15;
    rc->m_showTexture = 0; // 1/0 on/off initially

    RUpdateCamera();

    /*
      GROUND:
    */

    color[0] = 0.0f;
    color[1] = 0.75f;
    color[2] = 0.1f;

    groundG = RCreateCube(rc, 24.0f, 24.0f, 0.1f, color, (AcmeReal*)groundRenderTransform);

    RSetTexture(groundG, "checkerboard");

    for (i = 0; i < NBoxes; i++)
    {
        color[0] = boxColor[i][0];
        color[1] = boxColor[i][1];
        color[2] = boxColor[i][2];
    #ifdef BOX
        boxG[i] = RCreateCube(rc, 2 * boxDim[i][0], 2 * boxDim[i][1],
            2 * boxDim[i][2], color, (AcmeReal*)MdtBodyGetTransformPtr(box[i]));
    #else
        boxG[i] = RCreateSphere(rc, boxDim[i][0], color, (AcmeReal*)MdtBodyGetTransformPtr(box[i]));
    #endif
    }


    /*
      KEYS:
    */

    /* contact drawing */
#if 1
    MstBridgeSetPerPairCB(bridge,MstBridgeGetDefaultMaterial(),
        MstBridgeGetDefaultMaterial(), McduCollectContactPairs);
    McduMcdContactGraphics = McduCreateMcdContactGraphics(rc,100);
    McduToggleMcdContactDrawing();
#endif


#ifndef PS2
    RUseKey('s', stepEvolve);
    RUseKey(' ', shoot);
    RUseKey('a', toggleAutoEvolve);
#else

    RUsePadKey(PADRup, stepEvolve);


#endif

    /* selects a fixed-width font */
    RSetOGLFont(3);

    RCreateUserHelp(help, helpNum);

    for (i = 0; i < preEvolve; i++)
        stepEvolve();

/**************************************************/
      /*
        Run the Simulation
      */

    /*
      RRun() executes the main loop.

      Pseudocode: while no exit-request { Handle user input call Tick() to
      evolve the simulation and update graphic transforms Draw graphics }
    */

    RRun(rc, tick);


    /*
      Exit and cleanup
    */

    MstBridgeDestroy(bridge);
    McdTerm();

    MdtWorldDestroy(world);


    RDeleteRenderContext(rc);

    return 0;
}
