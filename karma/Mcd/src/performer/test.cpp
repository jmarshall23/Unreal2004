

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

  MeMatrix4 TM_A, TM_B;

  // initialize Performer
  pfInit();
  pfMultiprocess(0);
  pfConfig();

  // load models
  pfNode *teapotNode = pfdLoadFile_pfb( "../../../Resources/teapot.pfb" );

  // preprocessing: re-scale the cane model
  pfMatrix mFlatten;
  mFlatten.makeScale(5, 5, 5);

  // construct a simple scene
  pfDCS *dcsTeapot = new pfDCS;

  pfScene *scene = new pfScene;
  scene->addChild( dcsTeapot );
  dcsTeapot->addChild( teapotNode );   
  
  McdInit(McdPrimitivesGetTypeCount()+3, 100);

  /* McdPrimitivesRegisterTypes(); */
  McdTriangleMeshRegisterType();
  McdPrimitivesRegisterTypes();
  McdPrimitivesRegisterInteractions();
  McdTriangleMeshTriangleMeshRegisterInteraction();

  printf("building TeapotGeom...\n");
  McdGeometryID teapotGeom;
  teapotGeom = McdTrangleMeshCreateFromPerformer(teapotNode, 0 /* optional pfMatrix */, 0,0,0);
  // teapotGeom = McdSphereCreate( 2);
  
  printf("Building TeapotModel ...\n");  

  McdModelID teapotModelA, teapotModelB;
  teapotModelA = McdModelCreate(teapotGeom);
  teapotModelB = McdModelCreate(teapotGeom);
    
  MeMatrix4TMMakeIdentity(TM_A);
  MeMatrix4TMMakeIdentity(TM_B);  
   
  McdModelSetTransformPtr(teapotModelA, TM_A);
  McdModelSetTransformPtr(teapotModelB, TM_B); 
  
  McdModelPair pair;
  McdModelPairReset(&pair, teapotModelA, teapotModelB);
  McdHello( &pair );      
  
  McdContact contact[100];
  
  McdIntersectResult resl;
  resl.touch = 0;  
  resl.contactCount = 0;  
  resl.contactMaxCount = 100;  
  resl.contacts = contact;
  
  MeBool IsctFnFound = 0;
  int ic = 0;

  printf("Enter the loop ... \n");  
  
  MeReal xb = 5.0;
  
  while (resl.contactCount<1) {
    
    MeMatrix4TMSetPosition(TM_B, xb, 0, 0);
    IsctFnFound = McdIntersect( &pair, &resl);   
    
    printf("\nnum of contatcs: %d, IsctFnFound: %d\n", resl.contactCount, IsctFnFound); 
    printf("loop#: %d, x: %f\n", ic++, xb);
    
    xb -= 0.01;    
  }  
     
  return 0;
}
