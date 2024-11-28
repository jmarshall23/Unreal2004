/* cdBoxOnBox.c */


#include <Mdt.h>           /* MathEngine Dynamics Toolkit - Kea solver */
#include <McdFrame.h>          /* MathEngine Collision Toolkit: prefix Mcd */
#include <McdPrimitives.h>     /* Mcd primitive geometrical types (prefix Mcd) */
#include <Mst.h>

#include <MeViewer.h>           /* MathEngine Mini-Renderer / Viewer */
/*#include <McduDrawContacts.h>*/

#define N_BODIES (2)
#define NContacts   20

MstBridge   *bridge;
MdtWorldID      world;
McdSpaceID      space;

MdtBodyID       body[N_BODIES];
McdModelID      model[N_BODIES];
McdModelID      fixed;

RRender         *rc;

RGraphicsDescription *boxG[N_BODIES];
RGraphicsDescription *fixedG;

MeMatrix4 fixedTransform = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

/* const int FramesPerEvolve = 32; */
const int FramesPerEvolve = 1;
MeReal step = 0.0167777f;

MeReal BoxDims[3] = {1, 0.5f, 1};
MeReal SphereRadius = 1;

void Reset(void) {
    int i;
    for(i = 0; i < N_BODIES; i++) {
        MdtBodySetPosition(body[i], 0, 3.0f * (i + 1), 0);
        MdtBodySetQuaternion(body[i], 0, 1, 0, 0);
        MdtBodySetLinearVelocity(body[i], 0, 0, 0);
        MdtBodySetAngularVelocity(body[i], 0, 0, 0);
    }

    MdtBodySetAngularVelocity(body[0], 5, 5, 0);
}


void Tick(RRender *rc) {
    static int count = 0;
    if((count++ % FramesPerEvolve) == 0) {
        McdSpaceUpdateAll(space);
        MstBridgeUpdateContacts(bridge,space,world);
        MdtWorldStep(world, step);
/*        McduDisplayContacts(rc);*/
    }
}

int main(int argc, const char **argv)
{
  const RRenderType render = RParseRenderType(&argc,&argv);

  int i;
  float color[3] = {1, 0, 0};
  McdGeometryID geometry;

  world = MdtWorldCreate(N_BODIES ,NContacts);

  MdtWorldSetGravity(world, 0, -9.81f, 0);

  McdInit( McdPrimitivesGetTypeCount(), 100 );
  McdPrimitivesRegisterTypes();
  McdPrimitivesRegisterInteractions();

  bridge = MstBridgeCreate(1);


  space = McdSpaceAxisSortCreate(McdAllAxes, 50, 100, 1);

  rc = RNewRenderContext(render, kRQualitySmooth);

  geometry = McdBoxCreate(BoxDims[0], BoxDims[1], BoxDims[2]);
  fixed = McdModelCreate(geometry);
  McdSpaceInsertModel(space, fixed);
  McdModelSetTransformPtr(fixed, fixedTransform);
  McdSpaceUpdateAll(space);
  McdSpaceFreezeModel(fixed);
  McdModelSetBody(fixed,0);

  fixedG = RCreateCube(rc, BoxDims[0], BoxDims[1], BoxDims[2],
                       color, 0);
    
  for(i = 0; i < N_BODIES; i++) {
    body[i] = MdtBodyCreate( world );
    MdtBodyEnable(body[i]);

    if(0) {
      geometry = McdBoxCreate(BoxDims[0], BoxDims[1], BoxDims[2]);

      boxG[i] = RCreateCube(rc, BoxDims[0], BoxDims[1], BoxDims[2],
                            color, (AcmeReal*)MdtBodyGetTransformPtr(body[i]));
    }
    else {
      geometry = McdSphereCreate(SphereRadius);

      boxG[i] = RCreateSphere(rc, SphereRadius,
                              color, (AcmeReal*)MdtBodyGetTransformPtr(body[i]));
    }
        
    model[i] = McdModelCreate(geometry);
    McdSpaceInsertModel(space, model[i]);

    McdModelSetBody(model[i], body[i]);
  }  /* contact drawing */
#if 0
  material = McdGetDefaultMaterialID();
  McdDtBridgeSetContactCB(material, material, McduCollectContacts);
  McduContactGraphics = McduCreateContactGraphics(rc);
  McduToggleContactDrawing();
#endif
  McdSpaceBuild(space);

  Reset();

  RUseKey('\r', Reset);

  RRun(rc, Tick);

  RDeleteRenderContext( rc );
  MstBridgeDestroy(bridge);
  McdTerm();

/* MdtTerm()?
   delete MdtBodies etc. */
    MdtWorldDestroy(world);

  return 0;
}
          
