/* -*- mode: c -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/26 02:29:28 $ - Revision: $Revision: 1.12.2.12 $

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

#if (WIN32 && _DEBUG)
#include <crtdbg.h>
#endif
#include <MeApp.h>
#include <McdTriangleList.h>
#include <RConvex.h>

#define TRI_LIST_SIZE 8
#define GEOMETRY_RADIUS 1.0f

/* undefine this, and watch what happens to the cube */

/* #define USING_SHARP_EDGES  */

MstBridgeID bridge;
McdFrameworkID framework;
McdSpaceID space;
MdtWorldID world;
MeApp *app;
RRender *rc;
RMenu *menu;

int sharpEdges = 1, faceNormals = 0, twoSided = 0, autoDisable = 1, autoEvolve = 1, singleStep = 0;

MeALIGNDATA(MeMatrix4, triListMatrix,16) =
{
    {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {2,-0,0,1}
};

char *help[5] =
{
    "$ACTION1 - toggle options menu",
    "$ACTION2 - reset",
    "$ACTION5 - toggle dynamics pause",
    "$ACTION3 - single step dynamics",

    "$MOUSE - mouse force"
};

typedef struct
{
    McdGeometryID geometry;
    McdModelID model;
    MdtBodyID body;
    RGraphic *graphic;
} Primitive;

Primitive prim[5];

MeVector3 gravity = {0,(MeReal)-9.81,0};

int mat_terrain, mat_ball;
MdtBclContactParams *params;

float red[4] = {1.f, 0.f, 0, 1};
float yellow[4] = {1.f, 1.f, 0, 1};
float green[4] = {0,1, 0, 1};

typedef struct
{
    MeVector3 vertex[3];
    MeVector3 normal;
} TerrainPoly;

TerrainPoly terrain[] =
{
    { { {-10, 0.5f, -5},{-10, 0.5f, 5},	{-5, 0, -5}     } },
    { { {-5, 0, -5},	{-10, 0.5f, 5},	{-5, 0, 5}      } },

    { { {-5, 0, -5},	{-5, 0, 5},	{0, -1, -5}     } },
    { { {0, -1, -5},	{-5, 0, 5},	{0, -1, 5}      } },

    { { {0, -1, -5},	{0, -1, 5},	{5, -2.5f, -5}  } },
    { { {5, -2.5f, -5},	{0, -1, 5},     {5, -2.5f, 5}   } },

    { { {5, -2.5f, -5},	{5, -2.5f, 5},	{8, -3.0f, -5}  } },
    { { {8, -3.0f, -5}, {5, -2.5f, 5},  {8, -3.0f, 5}   } }
};

#define r1     (0.45f)
#define r2     (0.35f)
#define l2     (r2*(ROOT2+1.0f))
#define offset (0.5f*r2)
#define r2a    (r2*1.35f)
#define l2a    (l2*1.35f )
#define ROOT2  (1.414213562f)

/* a couple of different convex objects with quite different behaviours */

#ifdef USE_DIAMOND
const int convexVertexCount = 17;
const MeVector3 convexVertices[17] =
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
    { 0, -4*r2+offset, 0} };
#else
const int convexVertexCount = 12;
const MeVector3 convexVertices[12] =
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
#endif

void cleanup(void)
{
    MeAppDestroy(app);
    
    MdtWorldDestroy(world);
    MstBridgeDestroy(bridge);
    McdSpaceDestroy(space);
    McdFrameworkDestroyAllModelsAndGeometries(framework);
    McdTerm(framework);

    RMenuDestroy(menu);
    RRenderContextDestroy(rc);
}

void MEAPI tick(RRender *rc, void *userdata)
{
    MeAppStep(app);
    MeProfileStartSection("Collision", 0);
    McdSpaceUpdateAll(space);
    MstBridgeUpdateContacts(bridge, space, world);
    MeProfileEndSection("Collision");

    MeProfileStartSection("Dynamics", 0);
    if (autoEvolve || singleStep)
    {
        MdtWorldStep(world, 0.02f);
    }
    singleStep = 0;
    MeProfileEndSection("Dynamics");

    MeAppDrawContacts(app);
}

void MakeRObjectVertex(RObjectVertex* vertex,
    MeVector3 vert, MeVector3 norm, MeReal u, MeReal v)
{
    vertex->m_X = vert[0];
    vertex->m_Y = vert[1];
    vertex->m_Z = vert[2];
    vertex->m_NX = norm[0];
    vertex->m_NY = norm[1];
    vertex->m_NZ = norm[2];
    vertex->m_U = u;
    vertex->m_V = v;
}


int isSharp(MeVector3Ptr v0, MeVector3Ptr v1)
{
#if 1
    return (v0[0] == -10 && v1[0]==-10)    /* left edge  */
         || (v0[0] == 8 && v1[0]== 8)     /* right edge */
         || (v0[2] == v1[2]
         || (v0[1] == v1[1] && v0[2] > v1[2]));     
                                           /* side edges */
#else
    return (v0[0] == v1[0] || v0[2] == v1[2]);
#endif    
}

int MEAPI TriListGeneratorCB(McdModelPair *mp,
    McdUserTriangle *list, MeVector3 pos, MeReal rad,int max)
{
  McdUserTriangle *tri;
  int i;

  for(i=0; i<TRI_LIST_SIZE; i++)
  {
    tri = &list[i];

    /* Vertices */
    tri->vertices[0] = &terrain[i].vertex[0];
    tri->vertices[1] = &terrain[i].vertex[1];
    tri->vertices[2] = &terrain[i].vertex[2];

    /* Normal -- Must be related to vertices / edges using RH rule  */
    tri->normal = &terrain[i].normal;
    tri->flags = 0;

    if(sharpEdges)
    {
        if(isSharp(terrain[i].vertex[0], terrain[i].vertex[1]))
            tri->flags |= kMcdTriangleUseEdge0;
        if(isSharp(terrain[i].vertex[1], terrain[i].vertex[2]))
            tri->flags |= kMcdTriangleUseEdge1;
        if(isSharp(terrain[i].vertex[2], terrain[i].vertex[0]))
            tri->flags |= kMcdTriangleUseEdge2;
    }

    if(!faceNormals)
        tri->flags |= kMcdTriangleUseSmallestPenetration;

    if(twoSided)
        tri->flags |= kMcdTriangleTwoSided;
  }

  return TRI_LIST_SIZE;
}


void MEAPI toggleTwoSided(MeBool on)
{
    twoSided = on;
}

void MEAPI toggleFaceNormals(MeBool on)
{
    faceNormals = on;
}

void MEAPI toggleSharpEdges(MeBool on)
{
    sharpEdges = on;
}

void MEAPI toggleAutoDisable(MeBool on)
{
    int i;

    autoDisable = on;
    MdtWorldSetAutoDisable(world,on);
    if(!on)
    {
        for(i=1;i<=4;i++)
            MdtBodyEnable(prim[i].body);
    }
}

/* Reset boxes and balls to initial positions */
void MEAPI reset(RRender* rc, void* userData)
{
    int i;
    for(i=1;i<=4;i++)
    {
        MdtBodyEnable(prim[i].body);
        MdtBodySetLinearVelocity(prim[i].body, 0, 0, 0);
        MdtBodySetAngularVelocity(prim[i].body, 0, 0, 0);
        MdtBodySetQuaternion(prim[i].body, 1, 0, 0, 0);
    }
    MdtBodySetPosition(prim[2].body, -1.5f, 2.5f, 0);
    MdtBodySetPosition(prim[1].body, -8.0f, 14.5f, 0);
    MdtBodySetPosition(prim[3].body, -4.0f, 8.5f, 0);
    MdtBodySetPosition(prim[4].body,  2.0f, 6.5f, 0);
}

void MEAPI toggleAutoEvolve(RRender* rc, void* userData)
{
    autoEvolve = !autoEvolve;
    singleStep = 0;
    if (autoEvolve)
    {
        int i;
        for(i=1;i<=4;i++)
            MdtBodyEnable(prim[i].body);
    }
}

void MEAPI triggerSingleStep(RRender* rc, void* userData)
{
    if (!autoEvolve)
    {
        int i;
        singleStep = 1;
        for(i=1;i<=4;i++)
            MdtBodyEnable(prim[i].body);
    }
}

int MEAPI_CDECL main(int argc, const char *argv[])
{
    int i, j;
    MdtBclContactParams *params;
    MeCommandLineOptions *options;
    MeVector3 min = {-200,-200,-200}, max = {200,200,200};

#if defined WIN32 && defined _DEBUG
  {
    int debugFlag;
    debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

    debugFlag |= _CRTDBG_ALLOC_MEM_DF;
    debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
    debugFlag |= _CRTDBG_LEAK_CHECK_DF;

    _CrtSetDbgFlag(debugFlag);
  }
#endif

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);


    /* setup */

    framework = McdInit(0, 5, 0, 1);
    world = MdtWorldCreate(10, 30, 1, 1);
    space = McdSpaceAxisSortCreate(framework,McdAllAxes, 10, 20);
    bridge = MstBridgeCreate(framework, 4);
    app = MeAppCreate(world,space,rc);

    McdPrimitivesRegisterTypes(framework);
    McdPrimitivesRegisterInteractions(framework);
    McdConvexMeshRegisterType(framework);
    McdConvexMeshPrimitivesRegisterInteractions(framework);

    MstSetWorldHandlers(world);

    MdtWorldSetGravity(world,gravity[0],gravity[1],gravity[2]);
    MdtWorldSetAutoDisable(world,1);

    /* create primitives */

    prim[1].geometry = McdSphereCreate(framework,GEOMETRY_RADIUS);
    prim[1].graphic = RGraphicSphereCreate(rc,GEOMETRY_RADIUS,red,0);

    prim[2].geometry = McdBoxCreate(framework,
        GEOMETRY_RADIUS*2,GEOMETRY_RADIUS*2,GEOMETRY_RADIUS*2);
    prim[2].graphic = RGraphicBoxCreate(rc,
        GEOMETRY_RADIUS*2,GEOMETRY_RADIUS*2,GEOMETRY_RADIUS*2,red,0);

    prim[3].geometry = McdCylinderCreate(framework,
        GEOMETRY_RADIUS,2*GEOMETRY_RADIUS);
    prim[3].graphic = RGraphicCylinderCreate(rc,
        GEOMETRY_RADIUS,2*GEOMETRY_RADIUS,red,0);

    prim[4].geometry = McdConvexMeshCreateHull(framework,
        convexVertices, convexVertexCount, 0);
    prim[4].graphic =  RGraphicConvexCreate(rc,
        prim[4].geometry, red, 0 );

    for(i=1;i<=4;i++)
    {
        prim[i].body = MdtBodyCreate(world);
        prim[i].model = McdModelCreate(prim[i].geometry);
        McdModelSetBody(prim[i].model,prim[i].body);
        McdSpaceInsertModel(space,prim[i].model);
        RGraphicSetTransformPtr(prim[i].graphic,
            MdtBodyGetTransformPtr(prim[i].body));
        RGraphicSetWireframe(prim[i].graphic,1);
    }

    reset(rc, 0);

    /* triangle list */

    MeAppDrawContactsInit(app, green,10);

    prim[0].geometry = McdTriangleListCreate(framework,min,max,
        TRI_LIST_SIZE, TriListGeneratorCB);

    ((McdTriangleList*)prim[0].geometry)->triangleMaxCount = TRI_LIST_SIZE;

    prim[0].model = McdModelCreate(prim[0].geometry);
    McdModelSetBody(prim[0].model, NULL);
    McdModelSetTransformPtr(prim[0].model, triListMatrix);
    McdSpaceInsertModel(space, prim[0].model);
    McdSpaceUpdateModel(prim[0].model);
    McdSpaceFreezeModel(prim[0].model); /* world doesn't move */

    prim[0].graphic = RGraphicCreateEmpty(TRI_LIST_SIZE * 3);

    for(i=0; i<TRI_LIST_SIZE; i++)
    {
        MeVector3 edge1, edge2;

        /* Edges - Not needed if you are storing the poly Normals */
        MeVector3Subtract(edge1, terrain[i].vertex[1], terrain[i].vertex[0]);
        MeVector3Subtract(edge2, terrain[i].vertex[2], terrain[i].vertex[1]);

        /* Normal -- Must be related to vertices / edges using RH rule  */
        MeVector3Cross(terrain[i].normal, edge1, edge2);
        MeVector3Normalize(terrain[i].normal);

        for(j=0; j<3; j++)
        {
            MakeRObjectVertex(prim[0].graphic->m_pVertices+(i*3)+j,
                terrain[i].vertex[j], terrain[i].normal, 0, 0);
        }
    }

    prim[0].graphic->m_pObject->m_nTextureID = -1; /* no texture */
    RGraphicSetColor(prim[0].graphic,yellow);
    prim[0].graphic->m_pObject->m_bIsWireFrame = 0;
    prim[0].graphic->m_pLWMatrix = NULL;

    RGraphicSetTransformPtr(prim[0].graphic,triListMatrix);
    RGraphicAddToList(rc, prim[0].graphic, 0);

    McdSpaceBuild(space);

    /* materials */

    mat_ball  = MstBridgeGetDefaultMaterial();
    mat_terrain = MstBridgeGetNewMaterial(bridge);

    params = MstBridgeGetContactParams(bridge,mat_ball,mat_terrain);
    MdtContactParamsSetType(params,MdtContactTypeFriction2D);

    params->softness = 0.001f;
    params->options |= MdtBclContactOptionSoft;

    McdModelSetMaterial(prim[0].model, mat_terrain);
    for(i=1;i<3;i++)
    {
        McdModelSetMaterial(prim[i].model, mat_ball);
    }


    /* camera and rendering things */

    RCameraSetView(rc,20.0f,0.3f,0.3f);

    /* Lighting */
    rc->m_DirectLight1.m_bUseLight = 1;

    rc->m_DirectLight1.m_Direction[0] = -1;
    rc->m_DirectLight1.m_Direction[1] = 0;
    rc->m_DirectLight1.m_Direction[2] = 0;

    rc->m_DirectLight1.m_rgbAmbient[0] = 0;
    rc->m_DirectLight1.m_rgbAmbient[1] = 0;
    rc->m_DirectLight1.m_rgbAmbient[2] = 0;

    rc->m_DirectLight2.m_bUseLight = 0;

    RPerformanceBarCreate(rc);

    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 5, toggleAutoEvolve, 0);
    RRenderSetActionNCallBack(rc, 3, triggerSingleStep, 0);
    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)app);

    RRenderSetWindowTitle(rc, "TriangleList example");
    RRenderCreateUserHelp(rc,help,5);
    RRenderToggleUserHelp(rc);

    menu = RMenuCreate(rc, "Options Menu");
    RMenuAddToggleEntry(menu, "Sharp Edges", toggleSharpEdges, sharpEdges);
    RMenuAddToggleEntry(menu, "TwoSided", toggleTwoSided, twoSided);
    RMenuAddToggleEntry(menu, "Always Use Tri Normals",
        toggleFaceNormals, faceNormals);
    RMenuAddToggleEntry(menu, "AutoDisable", toggleAutoDisable, 1);
    RRenderSetDefaultMenu(rc, menu);

    RRun(rc, tick, 0);

    cleanup();

    return 0;
}
