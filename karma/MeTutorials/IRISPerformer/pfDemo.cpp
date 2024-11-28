/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:34 $ - Revision: $Revision: 1.16.8.1 $

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

//-----------------------------------------------------------------------------
/*
  This example illustrates how to use MathEngine Collison Toolkit
  with IRIS Perfomer
*/
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <Performer/pf/pfChannel.h>
#include <Performer/pf/pfLightSource.h>
#include <Performer/pf/pfNode.h>
#include <Performer/pfutil.h>
#include <Performer/pfdu.h>
#include <Performer/pf.h>
#include <Performer/pr.h>
#include <Performer/pfdb/pfpfb.h>
#include <Performer/pf/pfScene.h>
#include <Performer/pf/pfSCS.h>
#include <Performer/pf/pfDCS.h>
#include <Performer/pfui.h>
#include <McdFrame.h>
#include <MeMath.h>
#include <McdPerformer.h>
#include <McdTriangleMesh.h>
#include <McdPrimitives.h>
//-----------------------------------------------------------------------------

#if 1
  #define USE_COLLISION_SPACE
#else
  #undef USE_COLLISION_SPACE
#endif

#ifdef USE_COLLISION_SPACE
  McdSpaceID space;
  McdModelPairContainer *pairs;
#endif

// Global variables
MeMatrix4           TM_A;
MeMatrix4           TM_B;
McdContact          contact[100];
McdGeometryID       teapotGeomA;
McdGeometryID       teapotGeomB;
McdModelID          teapotModelA;
McdModelID          teapotModelB;
McdIntersectResult  resl;
McdModelPair        pair;

pfScene             *scene;
pfNode              *teapotNodeA;
pfNode              *teapotNodeB;
pfChannel           *chan;
pfDCS               *dcsTA;
pfDCS               *dcsTB;

pfuEventStream      events;
char                *progName;
int                 exitFlag = 0;
pfiTDFXformer       *xformer;

MeReal              hv=0, pv=0, rv=0;
int                 iMoveObjectA = 1;
int                 iMoveToRight = 1;
MeReal              rot = 0.0f;
MeReal              XL = -11;
MeReal              XU = 11;

//-----------------------------------------------------------------------------
//
//      Usage() -- print usage advice and exit. This
//      procedure is executed in the application process.
//
static void
Usage (void)
{
  printf("     Usage:   pfDemo run_duration (seconds) \n");
  printf("     Example: pfDemo 20 \n");
  exit(1);
}

//-----------------------------------------------------------------------------
// return a random number in the range [a, b]
MeReal random(MeReal a, MeReal b)
{
  return (a + ((MeReal)rand())/((MeReal)RAND_MAX)*b);
}

//-----------------------------------------------------------------------------
MeReal mepfMoveObject( pfDCS *dcs, MeReal xinc)
{
  MeReal x = (*(dcs->getMatPtr()))[3][0]+xinc;
  MeReal y = (*(dcs->getMatPtr()))[3][1];
  MeReal z = (*(dcs->getMatPtr()))[3][2];
  dcs->setTrans(x,y,z);
  return x;
}

//-----------------------------------------------------------------------------
void HandleCollision(McdModelPairContainer *pairs)
{
  int i;
  for( i = pairs->helloFirst ; i < pairs->stayingEnd ; ++i )
  {
    McdModelPairID pairFromSpace = pairs->array[i];
    McdIntersect(pairFromSpace, &resl);
  }
}

//-----------------------------------------------------------------------------
void Dance()
{
  MeReal x = 0;
  MeBool IsctFnFound = 0;

  if (resl.contactCount>0) {
    for (int j=0; j<75; j++) {
      pfFrame();
    }
    iMoveToRight = !iMoveToRight;
  }

  if (iMoveObjectA) {
    if (iMoveToRight) {
      rot += 1;
      x = mepfMoveObject( dcsTA, +0.1);
    }
    else {
      rot -= 1;
      x = mepfMoveObject( dcsTA, -0.1);
    }

    dcsTA->setRot(hv+rot, pv+rot, rv+rot);

    if (x < XL) {
      hv = random(0,360);
      pv = random(0,360);
      rv = random(0,360);
      iMoveObjectA = 0;
    }

    dcsTB->setTrans(XU,0,0);

  } else {

    dcsTA->setTrans(XL,0,0);

    if (!iMoveToRight) {
      rot -= 1;
      x = mepfMoveObject( dcsTB, -0.1);
    }
    else {
      rot += 1;
      x = mepfMoveObject( dcsTB, 0.1);
    }

    dcsTB->setRot(hv+rot, pv+rot, rv+rot);

    if (x > XU) {
      hv = random(0,360);
      pv = random(0,360);
      rv = random(0,360);
      iMoveObjectA = 1;
    }
  }

#ifdef USE_COLLISION_SPACE
  MeBool pairOverflow;
  McdSpacePairIterator spaceIter;

  McdSpaceUpdateAll(space);
  McdSpaceEndChanges(space);

  /* Initialise iterator for this space, after updating space. */
  McdSpacePairIteratorBegin(space, &spaceIter);

  do
  {
    McdModelPairContainerReset(pairs);
    pairOverflow = McdSpaceGetPairs(space, &spaceIter, pairs);
    MEASSERT(!pairOverflow);
    McdGoodbyeEach(pairs);
    McdHelloEach(pairs);
    HandleCollision(pairs);
  } while(pairOverflow);

  McdSpaceBeginChanges(space);

#else
  McdCopyTransformFromPerformerMatrix(TM_A, dcsTA->getMatPtr());
  McdCopyTransformFromPerformerMatrix(TM_B, dcsTB->getMatPtr());

  // call intersection function
  IsctFnFound = McdIntersect( &pair, &resl);
#endif
}

//-----------------------------------------------------------------------------
void HandleEvents(void)
{
  int             i, j;
  int             key, dev, val, numDevs;
  pfuEventStream  *pEvents = &events;

  pfuGetEvents(&events);
  numDevs = pEvents->numDevs;

  for ( j=0; j < numDevs; ++j) {
    dev = pEvents->devQ[j];
    val = pEvents->devVal[j];

    if ( pEvents->devCount[dev] > 0 ) {
      switch ( dev ) {

        case PFUDEV_REDRAW:
          pEvents->devCount[dev] = 0;
          break;

        case PFUDEV_WINQUIT:
          exitFlag = 1;
          pEvents->devCount[dev] = 0;
          break;

        case PFUDEV_KEYBD:
          for ( i=0; i < pEvents->numKeys; ++i ) {

             key = pEvents->keyQ[i];
             if ( pEvents->keyCount[key] ) {

                switch ( key ) {
                  case 27:                   /* ESC key. Exits prog */
                    exitFlag = 1;
                    break;

                  case 'h':
                    // printHelp(progName);
                    break;

                  case 'r':
                    // do nothing
                  break;

                  default:
                    break;
                }
             }
          }

          pEvents->devCount[dev] = 0;
          break;

        default:
          break;
      }
    }
  }

  pEvents->numKeys = 0;
  pEvents->numDevs = 0;
}

//-----------------------------------------------------------------------------
// Configure and open GL window
void WindowSetup( char *title)
{
  pfPipe *p = pfGetPipe(0);
  pfPipeWindow *pw = new pfPipeWindow(p);
  pw->setWinType(PFPWIN_TYPE_X);
  pw->setName("IRIS Performer");
  pw->setOriginSize(200,200,750,500);
  pfuInitInput(pw, PFUINPUT_X);
  pw->open();
}

//-----------------------------------------------------------------------------
void SceneSetup(void)
{
  // Attach loaded file to a new pfScene
  scene = new pfScene;
  dcsTA = new pfDCS;
  dcsTB = new pfDCS;

  pfMatrix ma, mb;
  ma.makeIdent();
  mb.makeIdent();
  dcsTA->setMat(ma);
  dcsTB->setMat(mb);

  scene->addChild(dcsTA);
  scene->addChild(dcsTB);
  dcsTA->addChild(teapotNodeA);
  dcsTB->addChild(teapotNodeB);
  dcsTA->setTrans(XL,0,0);
  dcsTB->setTrans(XU,0,0);

  // Create a pfLightSource and attach it to scene
  scene->addChild(new pfLightSource);
}


//-----------------------------------------------------------------------------
// Initialize Performer
void InitPerformer( void )
{
  pfInit();
  pfuInitUtil();
  pfiInit();

  // Use default multiprocessing mode based on number of
  // processors.
  //
  pfMultiprocess( PFMP_DEFAULT );

  pfFilePath("../../../Resources");

  // Load all loader DSO's before pfConfig() forks
  pfdInitConverter("teapot.flt");

  // initiate multi-processing mode set in pfMultiprocess call
  // FORKs for Performer processes,  CULL and DRAW, etc. happen here.
  //
  pfConfig();
}


//-----------------------------------------------------------------------------
// Create and configure a pfChannel.
void ChannelSetup( void )
{
  pfCoord   view;
  float     s, c;

  pfPipe *p = pfGetPipe(0);;
  chan = new pfChannel(p);
  chan->setScene(scene);
  chan->setFOV(45.0f, 0.0f);

  // determine extent of scene's geometry
  pfSphere bsphere;
  teapotNodeA->getBound(&bsphere);
  chan->setNearFar(1.0f, 20.0f * bsphere.radius);

  // set view
  pfSinCos(0, &s, &c);
  view.hpr.set(0, -5.0f, 0);
  view.xyz.set(6.0f * bsphere.radius * s,
         -6.0f * bsphere.radius *c,
          0.5* bsphere.radius);
  chan->setView(view.xyz, view.hpr);
}

//-----------------------------------------------------------------------------
// import
void LoadpfModels( void )
{
  teapotNodeA = pfdLoadFile("teapot566.pfb");
  teapotNodeB = pfdLoadFile("teapot566.flt");
  if (teapotNodeA == NULL || teapotNodeB == NULL)
  {
    pfExit();
    exit(-1);
  }
}

//-----------------------------------------------------------------------------
//
void Cleanup( void )
{
  printf("\nCleanup ... \n");
  // release memory
  McdGeometryDestroy( teapotGeomA );
  McdGeometryDestroy( teapotGeomB );
  McdModelDestroy( teapotModelA );
  McdModelDestroy( teapotModelB );
  McdTerm();

  // Terminate parallel processes and exit
  pfuExitUtil();
  pfuExitInput();
  pfExit();
}

//-----------------------------------------------------------------------------
//
void InitCollision( void )
{
   // intialize collision detection framework
  McdInit(1, 100);

#ifdef USE_COLLISION_SPACE
  space = McdSpaceAxisSortCreate(McdAllAxes, 2, 10,1);
  pairs = McdModelPairContainerCreate(10);
#endif

  // register McdTriangleMesh type and interactions
  McdTriangleMeshRegisterType();
  McdTriangleMeshTriangleMeshRegisterInteraction();
}

//-----------------------------------------------------------------------------
//
void CollisionSetup( void )
{
  printf("building TeapotGeom...\n");
  // build a collision geometry from a pfNode
  teapotGeomA = McdTrangleMeshCreateFromPerformer(teapotNodeA, 0, 0, 0, 0);
  teapotGeomB = McdTrangleMeshCreateFromPerformer(teapotNodeB, 0, 0, 0, 0);

  printf("Building TeapotModels ...\n");
  // build collision models from collision geometry
  teapotModelA = McdModelCreate(teapotGeomA);
  teapotModelB = McdModelCreate(teapotGeomB);

#ifdef USE_COLLISION_SPACE
  McdSpaceInsertModel(space, teapotModelA);
  McdSpaceInsertModel(space, teapotModelB);
#endif

  MeMatrix4TMMakeIdentity(TM_A);
  MeMatrix4TMMakeIdentity(TM_B);

  // set transformation matrix to collision models
  McdModelSetTransformPtr(teapotModelA, TM_A);
  McdModelSetTransformPtr(teapotModelB, TM_B);

#if !defined(USE_COLLISION_SPACE)
  // prepare a pair of models for intersections
  McdModelPairReset(&pair, teapotModelA, teapotModelB);
  McdHello( &pair );
#endif

  // prepare to collect intersection results
  resl.contacts = contact;
  resl.touch = 0;
  resl.contactCount = 0;
  resl.contactMaxCount = 100;

#ifdef USE_COLLISION_SPACE
  // connect model transform to pfDCS
  McdModelSetPerformerDCSPtr( teapotModelA, dcsTA );
  McdModelSetPerformerDCSPtr( teapotModelB, dcsTB );

  // set update callback functions
  McdModelSetUpdateCallback(teapotModelA, McdModelUpdateTransformFromPerformerDCS);
  McdModelSetUpdateCallback(teapotModelB, McdModelUpdateTransformFromPerformerDCS);
#endif
}

//-----------------------------------------------------------------------------
int
main (int argc, char *argv[])
{
  float duration;
  if (argc < 2) Usage();
  duration = atof(argv[1]);

  InitPerformer();
  WindowSetup("Collision Toolkit");
  LoadpfModels();
  SceneSetup();
  ChannelSetup();

  InitCollision();
  CollisionSetup();

  float t = 0;

  // running for 'duration' seconds
  while (t<duration && !exitFlag)
  {
    t = pfGetTime();
    pfFrame();
    Dance();
    HandleEvents();
  }

  Cleanup();

  return 0;
}
