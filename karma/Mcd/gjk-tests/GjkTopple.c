/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:52 $ - Revision: $Revision: 1.5.2.4 $

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

#define MAX_BOXES  15
#define USE_NEW_GJK 0
#define USE_TALL    1   /* make very tall stacks */
#define USE_CONVEX  1   /* use convex mesh */
#define USE_CYLINDERS 0
#define DRAW_CONTACTS 1

#define AUTO_DISABLE 0
#define GAMMA 0.2f           // default = 0.2
#define PADDING 0.005
#define FATNESS 0
#define GRAVITY -20

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <MeMath.h>
#include <MeViewer.h>
#include <MeApp.h>

#include <McdGjk.h>

/* Global declarations */
#define COLOR1 {0.0 , 0.73, 0.73, 1}
#define COLOR2 {0.0 , 0.4 , 1.0 , 1}
#define COLOR3 {1.0 , 0.0 , 0.5 , 1}
#define COLOR4 {1.0 , 0.6 , 0.0 , 1}
#define COLOR5 {1.0,  0.4 , 0.0 , 1}
#define COLOR6 {0.6 , 0.4 , 1.0 , 1}

float green[] = {0 , 1 , 0 , 1};

#if USE_TALL
#  define NBoxes 48
#else
#  define NBoxes 30
#endif
#define NBalls      4

/* help text */

char *help[6] =
{
    "$ACTION1 - toggle options menu",
    "$ACTION2 - reset",
    "$ACTION3 - shoot",
    "$ACTION4 - disable all blocks",
    "$ACTION5 - enable all blocks",
    "$MOUSE - mouse force"
};

int NumBoxes = NBoxes > MAX_BOXES ? MAX_BOXES : NBoxes;

MeReal boxDensity = (MeReal)(2.7);

MeReal shootVelocity = (MeReal)(16.5);

/* Radius of each box. */
MeReal boxRadii[NBoxes][3] =
{
    {1.5, 0.3, 0.5}, {0.3, 1.0, 0.5}, {0.3, 1.0, 0.5},
    {0.4, 0.3, 0.4}, {0.5, 0.15, 0.5}, {0.4, 0.3, 0.4},

#if USE_TALL
    {0.45, 0.3, 0.4}, {0.5, 0.15, 0.5}, {0.4, 0.3, 0.4},
    {0.45, 0.3, 0.4}, {0.5, 0.15, 0.5}, {0.4, 0.3, 0.4},
    {0.45, 0.3, 0.4}, {0.5, 0.15, 0.5}, {0.4, 0.3, 0.4},
#endif

    {1.5, 0.3, 0.5}, {0.3, 1.0, 0.5}, {0.3, 1.0, 0.5},
    {1.0, 0.25, 0.5}, {0.2, 0.6, 0.3}, {0.2, 0.6, 0.3},

#if USE_TALL
    {1.0, 0.25, 0.5}, {0.2, 0.6, 0.3}, {0.2, 0.6, 0.3},
    {1.0, 0.25, 0.5}, {0.2, 0.6, 0.3}, {0.2, 0.6, 0.3},
    {1.0, 0.25, 0.5}, {0.2, 0.6, 0.3}, {0.2, 0.6, 0.3},
#endif

    {1.5, 0.3, 0.5}, {0.3, 1.0, 0.5}, {0.3, 1.0, 0.5},
    {0.5, 0.5, 0.5}, {0.3, 0.3, 0.3},

    {0.25, 1.5, 0.25}, {1.0, 0.25, 0.25}, {0.25, 0.7, 0.25},

    {0.25, 0.8, 0.25}, {0.25, 0.8, 0.25}, {0.25, 0.8, 0.25},
    {0.25, 0.8, 0.25}, {1.0, 0.2, 1.0}, {0.4, 2.0, 0.4},

    {0.4, 2.0, 0.4}, {0.4, 2.0, 0.4}, {0.4, 2.0, 0.4},
    {0.4, 2.0, 0.4}
};

/* Position of each box */
MeReal boxPos[NBoxes][3] =
{
    /* middle pile */
    {0.0, 1.35, -2.0}, {-1.0, 0.01, -2.0}, {1.0, 0.01, -2.0},
    {0.0, 1.98, -2.0}, {0.0, 2.45, -2.0}, {0.0, 2.9, -2.0},

#if USE_TALL
    {0.0, 3.5, -2.0}, {0.0, 3.95, -2.0}, {0.0, 4.4, -2.0},
    {0.0, 5.0, -2.0}, {0.0, 5.45, -2.0}, {0.0, 5.9, -2.0},
    {0.0, 6.5, -2.0}, {0.0, 6.95, -2.0}, {0.0, 7.4, -2.0},
#endif
    /* front pile */
    {0.0, 1.35, -8.0}, {-1.0, 0.01, -8.0}, {1.0, 0.01, -8.0},
    {0.0, 3.1, -8.0}, {-0.6, 2.20, -8.0}, {0.6, 2.20, -8.0},

#if USE_TALL
    {0.0, 4.8, -8.0}, {-0.6, 3.90, -8.0}, {0.6, 3.90, -8.0},
    {0.0, 6.5, -8.0}, {-0.6, 5.60, -8.0}, {0.6, 5.60, -8.0},
    {0.0, 8.2, -8.0}, {-0.6, 7.30, -8.0}, {0.6, 7.30, -8.0},
#endif
    /* back pile */
    {0.0, 1.3, 6.0}, {-1.0, 0.01, 6.0}, {1.0, 0.01, 6.0},
    {0.0, 2.1, 6.0}, {0.0, 2.95, 6.0},

    /* cross */
    {6.0, 0.5, 0.0}, {6.0, 2.25, 0.0}, {6.0, 3.3, 0.0},

    /* table */
    {-7.6, -0.15, -0.7}, {-7.6, -0.15, 0.7}, {-6.4, -0.15, -0.7},
    {-6.4, -0.15, 0.7}, {-7.0, 0.8, 0.0},

    /* single pillars */
    {-6.0, 1.0, -6.0}, {-6.0, 1.0, 6.0}, {6.0, 1.0, -6.0},
    {6.0, 1.0, 6.0}, {0.0, 1.0, 12.0}
};

/* Color of each box */
float boxColor[NBoxes][4] =
{
    COLOR1, COLOR1, COLOR1, 
    COLOR1, COLOR1, COLOR1,
#if USE_TALL
    COLOR1, COLOR1, COLOR1,
    COLOR1, COLOR1, COLOR1,
    COLOR1, COLOR1, COLOR1,
    COLOR2, COLOR2, COLOR2, 
    COLOR2, COLOR2, COLOR2,
    COLOR2, COLOR2, COLOR2, 
#endif
    COLOR2, COLOR2, COLOR2,
    COLOR2, COLOR2, COLOR2, 
    COLOR3, COLOR3, COLOR3, COLOR3, COLOR3,
    COLOR4, COLOR4, COLOR4,
    COLOR5, COLOR5, COLOR5, COLOR5, COLOR5,
    COLOR6, COLOR6, COLOR6, COLOR6, COLOR6
};

/* World for the Dynamics Toolkit simulation */
McdFrameworkID framework;
MstUniverseID universe;
McdSpaceID space;
MdtWorldID world;
MstBridgeID bridge;

/* Physics representations */
McdGeometryID boxGeom[NBoxes];
McdModelID box[NBoxes];

/* all balls are the same shape - so we only need 1 model! */
McdGeometryID ballGeom;
McdModelID ball[NBalls+1];

McdGeometryID planeGeom;
McdModelID plane;

/*
   Material for floor. 
   If static/dynamic friction is turned off, 
   floor is given infinite friction
*/
MstMaterialID floorMaterial;

/* Graphical representations */
RGraphic *groundG;
RGraphic *boxG[NBoxes];
RGraphic *ballG[NBalls+1];

MeReal gravity[3] = { 0, GRAVITY, 0 };

/* Render context */
RRender *rc;
RMenu* menu;

MeApp* meapp;

unsigned int fireDelay = 40;
unsigned int fireFrames = 0;
int nextBall = 0;

/* 
   Static/dynamic friction calculation on PS2 can be quite slow
   when the dynamics matrix is large (as in this demo).
   whereas infinite friction and zero friction are quite fast.
   So default on PS2 is zero friction between boxes, and
   infinite friction between boxes and ground.
   Luckily, situations where you really need static/dynamic friction
   (like cars) tend to have small dynamics matrices, so are ok on PS2.
*/

#ifdef PS2
#  define DEFAULT_FRICTION    (0)
#else
#  define DEFAULT_FRICTION    (1)
#endif

int enableGravity = 1;

MeReal step = (MeReal)(0.03);

MeMatrix4 groundTM =
{
    {1,  0,  0, 0},
    {0,  0, -1, 0},
    {0,  1,  0, 0},
    {0, -1,  0, 1}
};

MeMatrix4 groundRenderTM =
{
    {1,     0,  0, 0},
    {0,     0, -1, 0},
    {0,     1,  0, 0},
    {0, -1.05,  0, 1}
};


MeMatrix4 cylTM = /* cyls are oriented with height on Z-axis, so flip to Y */
{
    {-1, 0, 0, 0},
    { 0, 0, 1, 0},
    { 0, 1, 0, 0},
    { 0, 0, 0, 1}
};


void MEAPI killAll(RRender* rc, void* userData)
{
    int i;
    for (i = 0; i < NBalls; i++) McdModelDynamicsDisable(ball[i]);
    for (i = 0; i < NumBoxes; i++) McdModelDynamicsDisable(box[i]);
}

void MEAPI wakeAll(RRender* rc, void* userData)
{
    int i;
    for (i = 0; i < NBalls; i++) McdModelDynamicsEnable(ball[i]);
    for (i = 0; i < NumBoxes; i++) McdModelDynamicsEnable(box[i]);
}

void MEAPI ToggleHighQualityFriction(MeBool on)
{
    MdtContactParamsID boxboxprops = MstBridgeGetContactParams(bridge,
        MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsID boxfloorprops = MstBridgeGetContactParams(bridge,
        floorMaterial, MstBridgeGetDefaultMaterial());

    if (on)
    {
        /*
	 * High quality friction on
	 */

        MdtContactParamsSetType(boxboxprops, MdtContactTypeFriction2D);
        MdtContactParamsSetFriction(boxboxprops, 2.0f);
        MdtContactParamsSetPrimarySlip(boxboxprops, 0.0f);
        MdtContactParamsSetSecondarySlip(boxboxprops, 0.0f);
        MdtContactParamsSetRestitution(boxboxprops, 0.3f);
        MdtContactParamsSetMaxAdhesiveForce(boxboxprops, 0.0f);

        MdtContactParamsSetType(boxfloorprops, MdtContactTypeFriction2D);
        MdtContactParamsSetFriction(boxfloorprops, 2.0f);
        MdtContactParamsSetPrimarySlip(boxfloorprops, 0.0f);        
        MdtContactParamsSetSecondarySlip(boxfloorprops, 0.0f);        
        MdtContactParamsSetRestitution(boxfloorprops, 0.3f);
        MdtContactParamsSetMaxAdhesiveForce(boxfloorprops, 0.0f);
    }
    else
    {
	/* 
	 * High quality friction off
         * This uses a cheaper way of doing friction. Slip means contacts only 
         * have dynamic friction, and the adhesive force can make things
         * behave funny, but go lots faster.
	 */

        MdtContactParamsSetType(boxboxprops, MdtContactTypeFriction2D);
        MdtContactParamsSetFriction(boxfloorprops, MEINFINITY);
        MdtContactParamsSetPrimarySlip(boxboxprops, 0.3f);
        MdtContactParamsSetSecondarySlip(boxboxprops, 0.3f);
        MdtContactParamsSetRestitution(boxboxprops, 0.3f);
        MdtContactParamsSetMaxAdhesiveForce(boxboxprops, MEINFINITY);

        MdtContactParamsSetType(boxfloorprops, MdtContactTypeFriction2D);
        MdtContactParamsSetFriction(boxfloorprops, MEINFINITY);
        MdtContactParamsSetPrimarySlip(boxfloorprops, 0.3f);
        MdtContactParamsSetSecondarySlip(boxfloorprops, 0.3f);
        MdtContactParamsSetRestitution(boxfloorprops, 0.3f);
        MdtContactParamsSetMaxAdhesiveForce(boxfloorprops, MEINFINITY);
    }
}

void MEAPI ToggleGravity(MeBool on)
{
    wakeAll(rc, 0);
    if(on)
        MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
    else
        MdtWorldSetGravity(world, 0, 0, 0);

}

void MEAPI shoot(RRender* rc, void* userData)
{
    MeVector3 v, up, cam_pos;

    if (fireFrames < fireDelay) return;

    fireFrames = 0;

    McdModelDynamicsReset(ball[nextBall]);

    RCameraGetPosition(rc, cam_pos);

    RCameraGetUp(rc, up);
    MeVector3Scale(up, (MeReal)0.5);
    MeVector3Subtract(cam_pos, cam_pos, up);
    
    McdModelDynamicsSetPosition(ball[nextBall], 
        cam_pos[0], cam_pos[1], cam_pos[2]);

    RCameraGetLookDir(rc, v);
    MeVector3Scale(v, shootVelocity);

    McdModelDynamicsSetLinearVelocity(ball[nextBall], v[0], v[1], v[2]);

    if(!McdModelDynamicsIsEnabled(ball[nextBall]))
    {
        McdModelDynamicsEnable(ball[nextBall]);
    }

    nextBall = (nextBall + 1) % NBalls;
}

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender * rc,void *userdata)
{
    MeAppStep(meapp);
    MeAppDrawContacts(meapp);
    MstUniverseStep(universe, step);
    fireFrames++;
}

/* Reset boxes and balls to initial positions */
void MEAPI reset(RRender* rc, void* userData)
{
    int i;

    for (i = 0; i < NumBoxes; i++)
    {
        McdModelDynamicsReset(box[i]);
#if USE_CYLINDERS
        McdModelDynamicsSetTransform(box[i],cylTM);
#endif
        McdModelDynamicsSetPosition(box[i], boxPos[i][0], boxPos[i][1], boxPos[i][2]);
        McdModelDynamicsEnable(box[i]);
    }

    for (i = 0; i < NBalls; i++)
    {
        McdModelDynamicsReset(ball[i]);
        McdModelDynamicsSetPosition(ball[i], 4, -0.5, 4 + ((MeReal)(2.0) * i));
        McdModelDynamicsEnable(ball[i]);
    }

    nextBall = 0;
}

void MEAPI_CDECL cleanup(void)
{
    MeAppDestroy(meapp);
    McdFrameworkDestroyAllModelsAndGeometries(framework);
    MstUniverseDestroy(universe);
    RRenderContextDestroy(rc);
    RMenuDestroy(menu);
}

void MEAPI togglehelp(RRender* rc, void* userdata) {
    RRenderToggleUserHelp(rc);
}

#if USE_CONVEX
//  This routine creates a box using a convex mesh.
//  it's for testing convex mesh.

McdGeometryID MEAPI ConvexBoxCreate(McdFramework *fw, MeReal x, MeReal y, MeReal z)
{
    MeReal s=1;//e3;
    static int isreg = 0;
    static MeVector3 vert[8];
    if (!isreg++)
    {
        McdConvexMeshRegisterType(fw);
        McdConvexMeshPrimitivesRegisterInteractions(fw);
    }
    x/=2*s; y/=2*s; z/=2*s;
    MeVector3Set(vert[0],x,y,z);
    MeVector3Set(vert[1],x,y,-z);
    MeVector3Set(vert[2],x,-y,z);
    MeVector3Set(vert[3],x,-y,-z);
    MeVector3Set(vert[4],-x,y,z);
    MeVector3Set(vert[5],-x,y,-z);
    MeVector3Set(vert[6],-x,-y,z);
    MeVector3Set(vert[7],-x,-y,-z);
    return McdConvexMeshCreateHull(fw, vert, 8, FATNESS/s);
}
#endif


//-------------------------------------------------------------
//  This sets interaction to GJK, this uses global var fwk.
//  see McdInteractionTableSetElement

#define fwk framework
int McdGjkMain(McdCache *c, McdGjkSimplex *s);

void SetGJK(int geoType1, int geoType2)
{
    McdInteractions *e = &fwk->interactionTable[geoType1 + 
              fwk->geometryRegisteredCountMax * geoType2 ];

    e->intersectFn = McdGjkCgIntersect;
    e->helloFn = McdCacheHello;
    e->goodbyeFn = McdCacheGoodbye;

    if (geoType1 != geoType2)
    {
        e = &fwk->interactionTable[geoType2 + 
              fwk->geometryRegisteredCountMax * geoType1 ];
        e->intersectFn = McdGjkCgIntersect;
        e->helloFn = McdCacheHello;
        e->goodbyeFn = McdCacheGoodbye;
    }
}

#define SET_GJK(a,b) SetGJK(kMcdGeometryType##a,kMcdGeometryType##b)


/* Main Routine */
int MEAPI_CDECL main(int argc, const char * argv[])
{
    int i;
    float color[4];
    MstUniverseSizes sizes;
    MeCommandLineOptions* options;
    MdtBclContactParams *p;

    /* ************* MAKE PHYSICS *************** */

    sizes = MstUniverseDefaultSizes;
    sizes.dynamicBodiesMaxCount = NBalls + NumBoxes;
    sizes.dynamicConstraintsMaxCount = 600;
    sizes.materialsMaxCount = 2;
    sizes.collisionModelsMaxCount = 200;
    sizes.collisionPairsMaxCount = 1000;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.collisionUserGeometryTypesMaxCount = 0;


    universe = MstUniverseCreate(&sizes);
    framework = MstUniverseGetFramework(universe);
    space = MstUniverseGetSpace(universe);
    world = MstUniverseGetWorld(universe);
    bridge = MstUniverseGetBridge(universe);

    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
    MdtWorldSetAutoDisable(world,AUTO_DISABLE);
    McdFrameworkSetDefaultContactTolerance(framework, PADDING);
    MdtWorldSetGamma(world, GAMMA);   // default = 0.2

    /* GROUND PLANE */
    planeGeom = McdPlaneCreate(framework);
    plane = MstFixedModelCreate(universe,planeGeom,groundTM);
    floorMaterial = MstBridgeGetNewMaterial(bridge);
    McdModelSetMaterial(plane, floorMaterial);

    /* BALLS */
    ballGeom = McdSphereCreate(framework,0.5);

    for (i = 0; i < NBalls; i++)
    {
        ball[i] = MstModelAndBodyCreate(universe, ballGeom, 1.5);
        McdModelDynamicsSetDamping(ball[i], 0.2, 0.1);
    }

    /* BOXES */
    for (i = 0; i < NumBoxes; i++)
    {
#if USE_CYLINDERS
        boxGeom[i] = McdCylinderCreate(framework,
            MeSqrt(boxRadii[i][0]*boxRadii[i][2]), 2 * boxRadii[i][1]);
#elif USE_CONVEX
        boxGeom[i] = ConvexBoxCreate(framework,
            2 * boxRadii[i][0], 2 * boxRadii[i][1], 2 * boxRadii[i][2]);
#else
        boxGeom[i] = McdBoxCreate(framework,2 * boxRadii[i][0], 2 * boxRadii[i][1],
            2 * boxRadii[i][2]);
#endif
        box[i] = MstModelAndBodyCreate(universe, boxGeom[i], boxDensity);

#if USE_CONVEX
        {
            //  repair a bug in convex inertia tensors
            MeMatrix3 I;
            MdtBodyID body = McdModelGetBody(box[i]);
            MdtBodyGetInertiaTensor(body, I);
            MeMatrix3MakeIdentity(I);
            I[0][0] = boxRadii[i][1]*boxRadii[i][1] + boxRadii[i][2]*boxRadii[i][2];
            I[1][1] = boxRadii[i][2]*boxRadii[i][2] + boxRadii[i][0]*boxRadii[i][0];
            I[2][2] = boxRadii[i][0]*boxRadii[i][0] + boxRadii[i][1]*boxRadii[i][1];
            MeMatrix3Scale(I, MdtBodyGetMass(body)/3);
            MdtBodySetInertiaTensor(body, I);
        }
#endif

        McdModelDynamicsSetDamping(box[i], 0.2, 0.1);
    }
    /* Set up contact parameters for collisions between boxes */

    p = MstBridgeGetContactParams(bridge,
            MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsSetType(p, MdtContactTypeFrictionZero);
    MdtContactParamsSetRestitution(p, 0.3);

    /* Set up contact parameters for collisions between boxes and the floor */

    /* Set up contact parameters */
    ToggleHighQualityFriction(DEFAULT_FRICTION);
    reset(rc, 0);
    /* ***************** END PHYSICS ***************** */


#if USE_NEW_GJK
    //--------------------------------------------------------------
    //  Set interaction table pointers to the new GJK code
    //--------------------------------------------------------------
    SET_GJK(Sphere, Sphere);
    SET_GJK(Sphere, Box);
//    SET_GJK(Sphere, Plane);
    SET_GJK(Sphere, Cylinder);
    SET_GJK(Sphere, ConvexMesh);
    SET_GJK(Box  , Box);
    SET_GJK(Box  , Plane);
    SET_GJK(Box  , Cylinder);
    SET_GJK(Box  , ConvexMesh);
    SET_GJK(Plane , Plane);
    SET_GJK(Plane , Cylinder);
    SET_GJK(Plane , ConvexMesh);
    SET_GJK(Cylinder, Cylinder);
    SET_GJK(Cylinder, ConvexMesh);
    SET_GJK(ConvexMesh, ConvexMesh);
    //--------------------------------------------------------------
#endif


    /* Initialise rendering attributes, eating command line parameters we
       recognize. */

    
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

    RPerformanceBarCreate(rc);

    /* Make the graphics */
    color[0] = color[1] = color[2] = color[3] = 1;
    groundG = RGraphicGroundPlaneCreate(rc, 24, 2, color, -1);
    RGraphicSetTexture(rc, groundG, "checkerboard");

    for (i = 0; i < NumBoxes; i++)
    {
#if USE_CYLINDERS
        boxG[i] =
            RGraphicCylinderCreate(rc, MeSqrt(boxRadii[i][0]*boxRadii[i][2]), 2 * boxRadii[i][1],
                boxColor[i], McdModelGetTransformPtr(box[i]));
#else
        boxG[i] =
            RGraphicBoxCreate(rc, 2 * boxRadii[i][0], 2 * boxRadii[i][1],
                2 * boxRadii[i][2], boxColor[i], McdModelGetTransformPtr(box[i]));
#endif
    }

    color[0] = 0.85f; color[1] = 0.85f; color[2] = 0.0f; color[3] = 1.0f;
    for (i = 0; i < NBalls; i++)
    {
        ballG[i] = RGraphicSphereCreate(rc, 0.5, color, McdModelGetTransformPtr(ball[i]));
    }

    meapp = MeAppCreate(world, space, rc);

#if DRAW_CONTACTS
    MeAppDrawContactsInit(meapp, green, 300);
    MeAppSetContactDrawLength(meapp, 0.1f);   // 0.02 is actual 1:1
#else
    RRenderSkydomeCreate(rc, "skydome", 2, 1);
#endif

    /* CONTROLS: */
    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, shoot, 0);
    RRenderSetActionNCallBack(rc, 4, killAll, 0);
    RRenderSetActionNCallBack(rc, 5, wakeAll, 0);

    RRenderSetWindowTitle(rc, "Topple example");
    RRenderCreateUserHelp(rc,help,6);
    RRenderToggleUserHelp(rc);

    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);

    menu = RMenuCreate(rc, "Options Menu");
    RMenuAddToggleEntry(menu, "Gravity", ToggleGravity, 1);

    RMenuAddToggleEntry(menu, "High quality friction", ToggleHighQualityFriction, DEFAULT_FRICTION);
    
    RRenderSetDefaultMenu(rc, menu);

    /* Cleanup after simulation. */
    atexit(cleanup);

    /* Run the Simulation. */
    RRun(rc, tick,0);

    return 0;
}

