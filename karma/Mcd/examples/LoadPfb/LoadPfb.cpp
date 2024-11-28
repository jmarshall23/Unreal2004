/* 
  $Name: t-stevet-RWSpre-030110 $

  $Id: LoadPfb.cpp,v 1.4 2001/12/10 19:39:27 dilips Exp $
*/

#include <Performer/pf.h>
#include <Performer/pr.h>
#include <Performer/pfdb/pfpfb.h>
#include <Performer/pf/pfScene.h>
#include <Performer/pf/pfSCS.h>
#include <Performer/pf/pfDCS.h>
#include <McdFrame.h>
#include <MeMath.h>
#include <McdPerformer.h>
#include <McdTriangleMesh.h>
#include <McdPrimitives.h>

int main()
{
  // initialize Performer
  pfInit();
  pfMultiprocess(0);
  pfConfig();

  // load pfb file
  pfNode *teapotNode = pfdLoadFile_pfb( "../../../../Resources/teapot.pfb" );
   
  // intialize collision detection framework 
  McdInit(1, 100);
  
  // register McdTriangleMesh type and interactions
  McdTriangleMeshRegisterType(); 
  McdTriangleMeshTriangleMeshRegisterInteraction();

  printf("building TeapotGeom...\n");  
  McdGeometryID teapotGeom;      
  // build a collision geometry from a pfNode
  teapotGeom = McdTrangleMeshCreateFromPerformer(teapotNode, 0 /* optional pfMatrix */); 
  
  printf("Building TeapotModels ...\n");  
  McdModelID teapotModelA, teapotModelB;
  // build collision models from collision geometry
  teapotModelA = McdModelCreate(teapotGeom);
  teapotModelB = McdModelCreate(teapotGeom);

  MeMatrix4 TM_A, TM_B;    
  MeMatrix4TMMakeIdentity(TM_A);
  MeMatrix4TMMakeIdentity(TM_B);  
   
  // set transformation matrix to collision models 
  McdModelSetTransformPtr(teapotModelA, TM_A);
  McdModelSetTransformPtr(teapotModelB, TM_B); 
  
  // prepare a pair of models for intersections
  McdModelPair pair;
  McdModelPairReset(&pair, teapotModelA, teapotModelB);
  McdHello( &pair );      
  
  // prepare to collect intersection results
  McdIntersectResult resl;
  McdContact contact[100];  
  resl.contacts = contact;
  resl.touch = 0;  
  resl.contactCount = 0;  
  resl.contactMaxCount = 100;  
  
  MeBool IsctFnFound = 0;
  int ic = 0;

  MeReal xb = 8.0;  
  printf("Enter the loop ... \n");    
  while (resl.contactCount<1) {
  
    // set the posiiton of collision model B to (xb, 0, 0)
    MeMatrix4TMSetPosition(TM_B, xb, 0, 0);
    
    // ***************************************    
    // call the intersection function 
    IsctFnFound = McdIntersect( &pair, &resl);       
    
    printf("\nnum of contatcs: %d, IsctFnFound: %d\n", resl.contactCount, IsctFnFound); 
    printf("loop#: %d, x: %f\n", ic++, xb);    
    xb -= 0.01;    
  }  
  
  printf("\nCleanup ... \n");      
  // release memory
  McdGeometryDestroy( teapotGeom );
  McdModelDestroy( teapotModelA );
  McdModelDestroy( teapotModelB );       
  McdTerm();
  pfExit();
  
  return 0;
}
