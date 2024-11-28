
/*
  cdDominoes.c
  Copyright (c) 1997-2002 MathEngine PLC
*/

/*
  This example shows a set of long boxes standing on a flat surface.
  Press 'a' start and stop physics.  With physics stopped, 's' single-steps
  the physics.  One box can be toppled by pressing 'g' momentarily.  Balls
  can also be shot at the boxes by pressing the space bar.

  This example uses MathEngine Dynamics and Collision Toolkits
*/


#ifdef _DEBUG
#   define _MECHECK
#else
#   define _MERELEASE
#endif

#include <math.h>
#include <stdlib.h>           /* included for malloc() */
#include <stdio.h>           /* included for malloc() */

#include <Mdt.h>           /* MathEngine Dynamics Toolkit - Kea solver */
#include <McdFrame.h>          /* MathEngine Collision Toolkit: prefix Mcd */
#include <McdPrimitives.h>     /* Mcd primitive geometrical types (prefix Mcd) */
#include <McduDtBridge.h>       /* Collision to Dynamics Bridge: prefix McdMdt */

/* #define USE_CYLINDER */

#define WITH_OPENGL
#include <MeViewer.h>           /* MathEngine Mini-Renderer / Viewer */
#include <McduDrawMcdContacts.h>

/* Global declarations */
int geomCount = 1;
#define NBoxes  10
#define NBalls  3
MeReal BoxCircleRadius = 3.0;

MdtWorldID              world;      /* World for the Dynamics Toolkit */
McdSpace                *space;     /* Space for the Collision Tookit */
McduDtBridge             *bridge; /* Connection between collision and
                                       dynamics */

#define NContacts   200

/* Physics representations */
MdtBodyID               box[NBoxes];
MdtBodyID               ball[NBalls];

/* Graphics representations */
RGraphicsDescription    *groundG;
RGraphicsDescription    *boxG[NBoxes];
RGraphicsDescription    *ballG[NBalls];

/* Collision representations */
McdModelID              groundCM;
McdModelID              boxCM[NBoxes];
McdModelID              ballCM[NBalls];

MeReal gravity[3] =     {0, -3, 0};

RRender* rc;

const MeReal kPi =      3.141592653589793238f;

unsigned int            fireDelay = 30;
unsigned int            fireFrames = 0;

int                     autoEvolve = 1;

#ifndef PS2
int                     nSteps = 2;
MeReal                  step=0.01f;
#else
int                     nSteps = 1;
MeReal                  step=0.016f;
#endif

int                     nextBall = 0;

MeMatrix4 groundTransform = {1,0,0,0, 0,0,-1,0, 0,1,0,0, 0,-1,0,1};
MeMatrix4 groundRenderTransform = {1,0,0,0, 0,0,-1,0, 0,1,0,0, 0,-1.05f,0,1};

#ifdef USE_CYLINDER
MeReal cylinderHeight = (MeReal)(3);
MeReal cylinderRadius = (MeReal)(.4);
#endif

/* #define GRAPHICS_YCYLINDER */

#ifdef GRAPHICS_YCYLINDER
MeMatrix4 cylinderTransform[NBoxes];
MeMatrix4 cylinderRelativeTransform = {1,0,0,0, 0,0,-1,0, 0,1,0,0, 0,0,0,1};
#endif


MeReal boxDims[3] = { 1.0, 3, 0.5 };

/*-------------------------------------------------------------------*/
void Normalize(MeReal *v) {
    MeReal norm = 1.0f/(MeReal)sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    v[0] *= norm;
    v[1] *= norm;
    v[2] *= norm;
}

/*-------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------*/
void toggleAutoEvolve(void)
{
    autoEvolve = !autoEvolve;
}

/*-------------------------------------------------------------------*/

void handleContacts(void)
{
  int pairArraySize;
  int overflow;
  McdModelPairContainer pairs;
  McdSpacePairIterator iter;

  McdModelPair **array;

  pairArraySize = 400;
  array = (McdModelPair**)alloca( pairArraySize * sizeof( McdModelPair* ) );
  overflow = 1;

  McdModelPairContainerInit( &pairs, array, pairArraySize );
  McdSpacePairIteratorBegin( space,&iter );

#ifdef MCDCHECK
  printf("handleContacts\n\n");
#endif

  while( overflow )
    {
      McdModelPairContainerReset( &pairs );
      overflow = McdSpaceGetPairs( space,&iter,&pairs );

#ifdef MCDCHECK
      McdModelPairContainerPrintStats( &pairs );
#endif

      McduDtBridgeProcessPairs( bridge, &pairs );

    }
}

/* evolution */
void stepEvolve(void)
{
  int i;
  for (i = nSteps; i--;)
    {
      MdtWorldStep(world,step);
      McdSpaceUpdateAll(space);
      handleContacts();
  }
}

/*-------------------------------------------------------------------*/
void push(void)
{
    MeReal pos[3];
    MdtBodyGetPosition(box[0],pos);
    MdtBodyAddForceAtPosition(box[0], 0,0,-10, pos[0],pos[1]+1.5f,pos[2]);

    autoEvolve = 1;
}

/*-------------------------------------------------------------------*/
void shoot(void)
{
    int i;
    MeReal v[3];

    if (fireFrames < fireDelay) {
        return;
    }
    fireFrames = 0;

    MdtBodySetPosition(ball[nextBall], rc->m_cameraPos[0], rc->m_cameraPos[1], rc->m_cameraPos[2]);
    for (i=0;i<3;i++) { v[i] = rc->m_cameraLookAt[i]-rc->m_cameraPos[i]; }
    Normalize(v);
    for (i=0;i<3;i++) { v[i] *= 10.0; }
    MdtBodySetLinearVelocity(ball[nextBall], v[0], v[1], v[2]);
    MdtBodySetAngularVelocity(ball[nextBall], 0, 0, 0);

    MdtBodyEnable(ball[nextBall] );

    if (++nextBall >= NBalls) { nextBall = 0; }
    MdtBodyDisable(ball[nextBall]);

    MdtBodySetPosition(ball[nextBall], 0,1000,0);
    MdtBodySetLinearVelocity(ball[nextBall], 0,0,0);
    MdtBodySetAngularVelocity(ball[nextBall], 0,0,0);
}

/*-------------------------------------------------------------------*/
void tick(RRender*rc)
{
    if (autoEvolve) {
        stepEvolve();
    }

    MdtBodyResetForces(box[0]);

    fireFrames++;
}


void
testGeoNames()
{
#ifdef MCDCHECK
  McdGeometryID p;
  McdGeometryID b;
  McdGeometryID s;

  McdGeometryShowTypes();

  p = McdPlaneCreate();
  s = McdSphereCreate(1);
  b = McdBoxCreate(1,2,3);

  printf(McdGeometryGetTypeName(p));
  printf(McdGeometryGetTypeName(s));
  printf(McdGeometryGetTypeName(b));
#endif
}

void
testNewIntersect()
{
  McdIntersectResult result;
  McdModelPairID p;
  McdModel *m1 = McdModelCreate( McdBoxCreate(1,2,3) );
  McdModel *m2 = McdModelCreate( McdSphereCreate(1) );

  p = McdModelPairCreate(m1,m2);

  printf("\n order box-sphere (unswapped) \n");
  McdHello( p );
  McdIntersect( p, &result );
  McdGoodbye( p );

  printf("\n order sphere-box (swapped) \n");
  McdModelPairReset( p, m2, m1 );
  McdHello( p );
  McdIntersect( p, &result );
  McdGoodbye( p );
}

/*-------------------------------------------------------------------*/
int main(int argc, const char **argv)
{
    const RRenderType render = RParseRenderType(&argc,&argv);
    MdtBclContactParams *params;

    char *help[] = {"'s' single step evolution",
                    "'a' toggle automatic evolution",
                    "'g' push on one of the dominoes",
                    "'Space' fires a ball"};

    int i;
    float color[3];
    MeReal rot[4];
    MeReal pos[3];
    MeReal theta;

    /* Collision geometry */
    McdGeometry *plane_prim, *box_prim, *ball_prim;


    /*-----------------------
      Dynamics initialization
      -----------------------*/
    world = MdtWorldCreate(NBoxes+NBalls ,NContacts);

    MdtWorldSetGravity(world, gravity[0],gravity[1],gravity[2]);
    /*MdtWorldSetEpsilon(world, 10e-6); */


    /*------------------------
      Collision initialization
      ------------------------*/

    McdInit( McdPrimitivesGetTypeCount(), 100 );
    McdPrimitivesRegisterTypes();
    McdPrimitivesRegisterInteractions();

    space = McdSpaceAxisSortCreate(McdAllAxes,50,100, 1);

    bridge = McduDtBridgeCreate( world );

    /* testModelIndex(); */
    /* testNewIntersect(); */
    testGeoNames();

    /*------------------------------------
      Setup rendering and dynamical bodies
      ------------------------------------*/
    rc = RNewRenderContext(render,kRQualitySmooth);

    /* GROUND: */
    color[0] = 0.0f;
    color[1] = 0.75f;
    color[2] = 0.1f;

    groundG = RCreateCube(rc,10.0f,10.0f,0.05f,color,(AcmeReal*)groundRenderTransform);

    /* BOXES: */
    color[0] = 0.5f;
    color[1] = 0.25f;
    color[2] = 0.0f;

    for (i = 0; i < NBoxes; i++) {
        box[i] = MdtBodyCreate( world );
        MdtBodyEnable(box[i]);

        theta = -((MeReal)(i))/((MeReal)NBoxes)*2.0f*kPi;

#ifdef USE_CYLINDER
  #if !defined(GRAPHICS_YCYLINDER )
        SetAxisAngle(rot,1,0,0,0.5f*kPi);
  #endif
#else
        SetAxisAngle(rot,0,1,0,theta);
#endif

        MdtBodySetQuaternion(box[i], rot[0],rot[1],rot[2],rot[3]);

        pos[0] = BoxCircleRadius * (MeReal)cos(theta);
        pos[1] = (MeReal)0.8;
        pos[2] = BoxCircleRadius * (MeReal)sin(theta);

        MdtBodySetPosition(box[i], pos[0],pos[1],pos[2]);
#ifdef USE_CYLINDER
        boxG[i] = RCreateCylinder(rc,
                  cylinderRadius, cylinderHeight,
                  color,(AcmeReal*)MdtBodyGetTransformPtr(box[i]));
#else
        boxG[i] = RCreateCube(rc,
                  boxDims[0],boxDims[1],boxDims[2],
                  color,(AcmeReal*)MdtBodyGetTransformPtr(box[i]));
#endif
    }

    /* BALLS: */
    color[0] = 0.85f;
    color[1] = 0.85f;
    color[2] = 0.0f;

    for (i = 0; i < NBalls-1; i++) {
        ball[i] = MdtBodyCreate( world );
        MdtBodyEnable(ball[i]);
        theta = -((MeReal)(i))/((MeReal)(NBalls-1))*2.0f*kPi;
        pos[0] = 1.8f*(MeReal)cos(theta);
        pos[1] = 1.5f;
        pos[2] = 1.8f*(MeReal)sin(theta);
        MdtBodySetPosition(ball[i], pos[0],pos[1],pos[2]);
        ballG[i] = RCreateSphere(rc,0.5,color,(AcmeReal*)MdtBodyGetTransformPtr(ball[i]));
    }

    ball[i] = MdtBodyCreate( world );
    MdtBodyEnable(ball[i]);
    pos[0] = 0;
    pos[1] = 1000;
    pos[2] = 0;
    MdtBodySetPosition(ball[i], pos[0],pos[1],pos[2]);
    MdtBodyDisable(ball[i]);

    ballG[i] = RCreateSphere(rc,0.5,color,(AcmeReal*)MdtBodyGetTransformPtr(ball[i]));
    nextBall = i;

    /*------------------------------------------------------------
      Create collision models and establish their correspondence
      with physical bodies
      ------------------------------------------------------------*/

    /* ground */
    plane_prim = McdPlaneCreate();
    groundCM = McdModelCreate( plane_prim );
    McdSpaceInsertModel(space,groundCM);

    McdSpacePairIteratorBegin(space,0);
    McdSpaceGetPairs(space,0,0);

    McdSpaceEndChanges(space);
    McdSpaceDisablePair(groundCM,groundCM);
    McdSpaceUpdateModel(groundCM);
    McdSpaceFreezeModel(groundCM);

    McduDtBridgeSetBody(bridge, groundCM, 0 );
    McdModelSetTransformPtr( groundCM, groundTransform );

    /* indicate that object will not move */
    McdSpaceUpdateModel(groundCM);
    McdSpaceFreezeModel(groundCM);

    /* boxes */
    for ( i = 0; i < NBoxes; i++ )
    {
#ifdef USE_CYLINDER
        box_prim = McdCylinderCreate(cylinderRadius, cylinderHeight);
#else
        box_prim = McdBoxCreate(boxDims[0],boxDims[1],boxDims[2]);
#endif
        boxCM[i] = McdModelCreate( box_prim );
        McdSpaceInsertModel(space,boxCM[i]);
        McduDtBridgeSetBody(bridge, boxCM[i], box[i] );
#ifdef USE_CYLINDER
  #ifdef GRAPHICS_YCYLINDER
        McdDtBridgeSetRelativeTransformToBodyPtr(boxCM[i],cylinderRelativeTransform,
                       cylinderTransform[i]);
  #endif
#endif
    }

    /*--------------------------------------------------------------
      Setup contact properties for contact generation between models
      --------------------------------------------------------------*/
    params = &bridge->defaultContactParams;
/*    params->bouncyness = 1.0; */
    params->type = MdtContactTypeFriction2D;
    params->friction1  = 1.0f;
    params->friction2  = 1.0f;
    /* params->options = MdtContactOptionBouncy; */

    for ( i = 0; i < NBalls; i++ )
    {
        ball_prim = McdSphereCreate(0.5);
        ballCM[i] = McdModelCreate( ball_prim );
        McdSpaceInsertModel(space,ballCM[i]);
        McduDtBridgeSetBody(bridge, ballCM[i], ball[i] );
    }

    McdSpaceBuild(space); /* prepare space for use */

    /*--------------------
      Setup renderer
      --------------------*/

    /* Keys: */

    #ifndef PS2
    /*McdDtBridgeSetActiveTouchCB(0, 0, McduCollectContactPairs);
    McduDtBridgeSetCallbackFn(McduCollectContactPairs);
     */
    McduMcdContactGraphics = McduCreateMcdContactGraphics(rc,100);

    RUseKey('c', McduToggleMcdContactDrawing);
    RUseKey('s', stepEvolve);
    RUseKey('a', toggleAutoEvolve);
    RUseKey('g', push);
    RUseKey(' ', shoot);
    #else
    RUsePadKey(PADtriangle, stepEvolve);
    RUsePadKey(PADcross, toggleAutoEvolve);
    RUsePadKey(PADsquare, push);
    RUsePadKey(PADcircle, shoot);
    #endif

    RCreateUserHelp(help,4);

    /* McduToggleMcdContactDrawing(); */

    /* run the simulation */
    RRun( rc, tick );

    /*-------
      Cleanup
      -------*/

    /* memory release  */
    McduDtBridgeDestroy(bridge);
    McdTerm();

    MdtWorldDestroy(world);

    return 0;
}
