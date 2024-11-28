/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:33 $ - Revision: $Revision: 1.36.2.5 $

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

#include <stdio.h>
#include <time.h>

#include <Mst.h>
#include <McdConvexMesh.h>
#include <MeViewer.h>
#include <MeMath.h>
#include <MeApp.h>
#include <RConvex.h>

#define N_BODIES (6)
#define NContacts (200)

MeApp*            meapp;
MstBridgeID       bridge;
MdtWorldID        world;
McdSpaceID        space;

MdtBodyID       dBody[N_BODIES];
McdModelID      cModel[N_BODIES];
McdModelID      cPlane;

RGraphic* gBody[N_BODIES];
RGraphic* gPlane;

RRender         *rc;

McdFramework *frame;

MeMatrix4 cPlaneTransform = {1,0,0,0, 0,0,-1,0, 0,1,0,0, 0,-4,0,1};

char *help[3] =
{
    "Press $ACTION2 to reset",
    "Press $ACTION3 to shoot",
    "$MOUSE - mouse force"
};

/* ------- object dimension ----*/
MeReal dimbox[3] = {1,1,1};

MeReal rball = (MeReal)1;
MeReal rcyl  = (MeReal)1;
MeReal hcyl  = (MeReal)2;

int autoBenchmark = 0;

MeReal step = 0.02f;

void MEAPI shoot(RRender* rc, void* useData)
{
    int iball = 4;
    MeVector3 v, camPos, camLookat;

    RCameraGetPosition(rc, camPos);
    RCameraGetLookAt(rc, camLookat);

    MdtBodySetPosition(dBody[iball],  camPos[0], camPos[1], camPos[2]);

    MeVector3Subtract(v, camLookat, camPos);

    MeVector3Normalize(v);
    MeVector3Scale(v, 20);

    MdtBodySetLinearVelocity(dBody[iball], v[0], v[1], v[2]);
    MdtBodySetAngularVelocity(dBody[iball], 0, 0, 0);

    if (!MdtBodyIsEnabled(dBody[iball]))
    {
        MdtBodyEnable(dBody[iball]);
    }
}

void MEAPI Tick(RRender* rc, void* useData)
{
    MeAppStep(meapp);

    MeProfileStartSection("Collision", 0);
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge, space, world);
    MeProfileEndSection("Collision");

    MeProfileStartSection("Dynamics", 0);
    MdtWorldStep(world, step);
    MeProfileEndSection("Dynamics");
}

#define r1 (0.5f)
#define r2 (0.35f)
#define l2 (0.8449747f) /* r2*((MeReal)sqrt(2.0f) + 1.0f) */
#define r2a (r2*1.35f)
#define l2a (l2*1.35f)
#define offset (0.5f*r2)

void printMassProp(McdGeometryID geometry)
{
    MeMatrix4 relTM;
    MeMatrix3 massP;
    MeReal volume;

    McdConvexMeshGetMassProperties((McdConvexMeshID)geometry, relTM, massP, &volume);
    printf("Volume = %7.3f\n", volume);
    printf("Center of Mass: (%5.1f, %5.1f, %5.1f)\nInertia Matrix:\n",relTM[3][0],relTM[3][1],relTM[3][2]);
    MeMatrixPrint((MeReal*)massP, 3, 3,"%5.1f");
}


void MEAPI reset(RRender *rc, void *userData) {
  int i;


  for(i=0;i<6;i++) {
    MdtBodyEnable(dBody[i]);
    MdtBodySetLinearVelocity(dBody[i], 0, 0, 0);
    MdtBodySetQuaternion(dBody[i], 1, 0, 0, 0);
    MdtBodySetAngularVelocity(dBody[i], 0, 0, 0);
  }

    MdtBodySetPosition(dBody[0], 5.2f, 7.0f, 2);
    MdtBodySetPosition(dBody[1], 5.2f, 5.0f, 2.0f);
    MdtBodySetPosition(dBody[2], 5.2f, 3, 2.0f);
    MdtBodySetPosition(dBody[3], -2, 3, 2.0f);
    MdtBodySetPosition(dBody[4], -2, 0, 2.0f);
    MdtBodySetPosition(dBody[5], 5.2f, 0, 2.0f);
}

const float color[6][4] =
{
    {0,0.4f,0.6f,1},
    {0,1,0,1},
    {1,1,0,1},
    {0,0,1,1},
    {1,1,1,1},
    {0.4f,0.6f,0,1}
};

/*  Here is a nice cube for testing mass Properties */
const MeVector3 vertices0[8] =
{
    {-r1,-r1,-r1},
    { r1,-r1,-r1},
    { r1, r1,-r1},
    {-r1, r1,-r1},
    {-r1,-r1, r1},
    { r1,-r1, r1},
    { r1, r1, r1},
    {-r1, r1, r1}
};

const MeVector3 vertices1[12] =
{
    { r1, 2*r1, r1},
    {-r1, 2*r1, r1},
    {-r1, 2*r1, -r1},
    { r1, 2*r1, -r1},
    { 2*r1, 0, 2*r1},
    {-2*r1, 0, 2*r1},
    {-2*r1, 0, -2*r1},
    { 2*r1, 0, -2*r1},
    { r1, -2*r1, r1},
    {-r1, -2*r1, r1},
    {-r1, -2*r1, -r1},
    { r1, -2*r1, -r1}
};

const MeVector3 vertices2[17] =
{
    { l2, r2+offset, r2},
    { r2, r2+offset, l2},
    {-r2, r2+offset, l2},
    {-l2, r2+offset, r2},
    {-l2, r2+offset, -r2},
    {-r2, r2+offset, -l2},
    { r2, r2+offset, -l2},
    { l2, r2+offset, -r2},
    { l2a, 0+offset, r2a},
    { r2a, 0+offset, l2a},
    {-r2a, 0+offset, l2a},
    {-l2a, 0+offset, r2a},
    {-l2a, 0+offset, -r2a},
    {-r2a, 0+offset, -l2a},
    { r2a, 0+offset, -l2a},
    { l2a, 0+offset, -r2a},
    { 0, -4*r2+offset, 0}
};

const MeVector3 vertices3[16] =
{
    { l2, r2, r2},
    { r2, r2, l2},
    {-r2, r2, l2},
    {-l2, r2, r2},
    {-l2, r2, -r2},
    {-r2, r2, -l2},
    { r2, r2, -l2},
    { l2, r2, -r2},
    { l2a, -r2, r2a},
    { r2a, -r2, l2a},
    {-r2a, -r2, l2a},
    {-l2a, -r2, r2a},
    {-l2a, -r2, -r2a},
    {-r2a, -r2, -l2a},
    { r2a, -r2, -l2a},
    { l2a, -r2, -r2a}
};

int MEAPI_CDECL main(int argc, const char **argv)
{
    MdtBclContactParams *props;

    McdGeometry *geometry;
    McdGeometry *geoCnv[3];

    MeCommandLineOptions* options;

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;

    frame = McdInit(0,100,0,1);
    world = MdtWorldCreate(10, 300, 1, 1);
    space = McdSpaceAxisSortCreate(frame,McdAllAxes, 10, 100);
    bridge = MstBridgeCreate(frame,4);

    McdPrimitivesRegisterTypes(frame);
    McdPrimitivesRegisterInteractions(frame);
    McdConvexMeshRegisterType(frame);
    McdConvexMeshPrimitivesRegisterInteractions(frame);

    MstSetWorldHandlers(world);
    MdtWorldSetGravity(world, 0, -9.81f, 0);

    /* Plane */
    geometry = McdPlaneCreate(frame);
    cPlane = McdModelCreate(geometry);
    McdModelSetTransformPtr(cPlane, cPlaneTransform);
    McdSpaceInsertModel(space, cPlane);
    McdSpaceUpdateModel(cPlane);
    McdSpaceFreezeModel(cPlane);

    /* Convex 1 */
    dBody[0] = MdtBodyCreate(world);
    MdtBodySetMass(dBody[0],1.0f);
    MdtBodySetLinearVelocityDamping(dBody[0], 0.3f);
    MdtBodySetAngularVelocityDamping(dBody[0], 0.3f);

    geoCnv[0] = (McdGeometryID) McdConvexMeshCreateHull(frame,vertices1, 12, 0);
#if 0
    geoCnv[0] = (McdGeometryID) McdConvexMeshCreateHull(vertices0, 8, 0); /* Use the Cube! */
#endif
    cModel[0] = McdModelCreate( geoCnv[0] );
    McdSpaceInsertModel(space,cModel[0]);
    McdModelSetBody(cModel[0], dBody[0]);

    /*printMassProp(geoCnv[0]);*/

    /* Convex 2 */
    dBody[1] = MdtBodyCreate(world);
    MdtBodySetMass(dBody[1],1.0f);
    MdtBodySetLinearVelocityDamping(dBody[1], 0.3f);
    MdtBodySetAngularVelocityDamping(dBody[1], 0.3f);

    geoCnv[1] = (McdGeometryID) McdConvexMeshCreateHull(frame,vertices2, 17, 0);
    cModel[1] = McdModelCreate(geoCnv[1]);
    McdSpaceInsertModel(space,cModel[1]);
    McdModelSetBody(cModel[1], dBody[1]);

    /*printMassProp(geoCnv[1]);*/

    /* Convex 3 */
    dBody[2] = MdtBodyCreate(world);
    MdtBodySetMass(dBody[2],1.0f);
    MdtBodySetLinearVelocityDamping(dBody[2], 0.3f);
    MdtBodySetAngularVelocityDamping(dBody[2], 0.3f);

    geoCnv[2] = (McdGeometryID) McdConvexMeshCreateHull(frame,vertices3, 16, 0);
    cModel[2] = McdModelCreate( geoCnv[2] );
    McdSpaceInsertModel(space,cModel[2]);
    McdModelSetBody(cModel[2], dBody[2]);

    /*printMassProp(geoCnv[2]);*/

    /* Box */
    dBody[3] = MdtBodyCreate(world);
    MdtBodySetMass(dBody[3],1.0f);
    MdtBodySetLinearVelocityDamping(dBody[3], 0.3f);
    MdtBodySetAngularVelocityDamping(dBody[3], 0.3f);

    geometry = (McdGeometryID) McdBoxCreate(frame,dimbox[0], dimbox[1], dimbox[2]);
    cModel[3] = McdModelCreate( geometry );
    McdSpaceInsertModel(space,cModel[3]);
    McdModelSetBody(cModel[3], dBody[3]);

    /* Sphere */
    dBody[4] = MdtBodyCreate(world);
    MdtBodySetMass(dBody[4],1.0f);
    MdtBodySetLinearVelocityDamping(dBody[4], 0.3f);
    MdtBodySetAngularVelocityDamping(dBody[4], 0.3f);

    geometry = (McdGeometryID) McdSphereCreate(frame, rball );
    cModel[4] = McdModelCreate( geometry );
    McdSpaceInsertModel(space,cModel[4]);
    McdModelSetBody(cModel[4], dBody[4]);

    /* Cylinder */
    dBody[5] = MdtBodyCreate(world);
    MdtBodySetMass(dBody[5],1.0f);
    MdtBodySetLinearVelocityDamping(dBody[5], 0.3f);
    MdtBodySetAngularVelocityDamping(dBody[5], 0.3f);

    geometry = (McdGeometryID) McdCylinderCreate(frame, rcyl, hcyl );
    cModel[5] = McdModelCreate( geometry );
    McdSpaceInsertModel(space,cModel[5]);
    McdModelSetBody(cModel[5], dBody[5]);

    reset(rc,0);

    /*    McdSpaceBuild(space); */

    /* set parameters for contacts */

    props = MstBridgeGetContactParams(bridge,
        MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());
    MdtContactParamsSetType(props,MdtContactTypeFriction2D);
    MdtContactParamsSetPrimaryFriction(props, (MeReal)0.50);
    MdtContactParamsSetSecondaryFriction(props, (MeReal)0.50);
    MdtContactParamsSetRestitution(props,(MeReal)0.3);
    MdtContactParamsSetSoftness(props,(MeReal)0.0001);


    /* -------- graphics ------------ */
    gPlane   = RGraphicGroundPlaneCreate(rc, 30.0f,30, color[4], -4);
    RGraphicSetTexture(rc, gPlane, "checkerboard");

    gBody[0] = RGraphicConvexCreate(rc, geoCnv[0], color[1], MdtBodyGetTransformPtr(dBody[0]));
    gBody[1] = RGraphicConvexCreate(rc, geoCnv[1], color[2], MdtBodyGetTransformPtr(dBody[1]));
    gBody[2] = RGraphicConvexCreate(rc, geoCnv[2], color[3], MdtBodyGetTransformPtr(dBody[2]));
    gBody[3] = RGraphicBoxCreate(rc, dimbox[0], dimbox[1], dimbox[2], color[0], MdtBodyGetTransformPtr(dBody[3]));
    gBody[4] = RGraphicSphereCreate(rc, rball, color[5], MdtBodyGetTransformPtr(dBody[4]) );
    gBody[5] = RGraphicCylinderCreate(rc, rcyl, hcyl, color[5], MdtBodyGetTransformPtr(dBody[5]) );

    RPerformanceBarCreate(rc);
    RRenderSetWindowTitle(rc, "ConvexPrims tutorial");

    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, shoot, 0);
    RRenderCreateUserHelp(rc, help, 3);
    RRenderToggleUserHelp(rc);

    meapp = MeAppCreate(world, space, rc);

    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);
    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, shoot, 0);

    RRun(rc, Tick, 0);

    return 0;
}
