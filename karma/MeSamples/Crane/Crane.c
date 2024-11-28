/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/06/02 16:41:49 $ - Revision: $Revision: 1.12 $

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

#ifdef _MSC_VER
#pragma warning( disable : 4305 )
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <MeApp.h>
#include <MeAssetDB.h>
#include <MeAssetFactory.h>
#include <MeAssetDBXMLIO.h>
#include <MeMath.h>
#include <RGeometry.h>

/* help text */
static char* help[] =
{
    "$UP: Move crane up",
    "$DOWN: Move crane down",
    "$LEFT: Rotate crane left",
    "$RIGHT: Rotate crane right",
    "$ACTION5: Extend Crane",
    "$ACTION4: Retract Crane",
    "$MOUSE - mouse force"
};

/* World for the Dynamics Toolkit simulation */
MstUniverseID   universe;
McdFrameworkID  framework;
McdSpaceID      space;
MdtWorldID      world;
MstBridgeID     bridge;

/* asset stuff */
MeAssetDB       *db;
MeAssetFactory  *factory;
MeFAsset        *crane;
MeAssetInstance *ins;
MeHash          *nameToGraphic;

/* Physics representations */
McdGeometryID   planeGeom;
McdModelID      plane;
McdModelID      baseModel;
McdModelID      armModel;
McdModelID      extenderModel;
MeALIGNDATA(MeMatrix4,groundTM,16) = 
        {
            {1,  0,  0, 0},
            {0,  0, -1, 0},
            {0,  1,  0, 0},
            {0, -1,  0, 1}
        };

/* Base, Arm, Extend */
MdtLimitID      limit[3];
MeReal          desired[3] = {0};
MdtBodyID       mainBody;
MdtHingeID      mainHinge, armHinge;
MdtPrismaticID  armExtend;

/* Graphical representations */
RGraphic *groundG;
RGraphic *baseG;
RGraphic *armG;
RGraphic *extenderG;

MeReal gravity[3] = { 0, -9.8, 0 };

/* Render context */
RRender *rc;
MeApp* meapp;

MeReal step = (MeReal)0.03;

MeReal desiredMainHingePosition = (MeReal)0.0; 
MeReal desiredArmHingePosition = (MeReal)0.0;
MeReal desiredArmPrismaticPosition = (MeReal)0.0;
MeReal positionIncrement = (MeReal)0.02;

/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender * rc,void *userdata)
{
    MeReal clampMargin = 0.1f;
    
    desiredMainHingePosition = MeCLAMP(desiredMainHingePosition, mainHinge->limit.limit[0].stop + clampMargin, 
        mainHinge->limit.limit[1].stop - clampMargin);
    
    desiredArmHingePosition = MeCLAMP(desiredArmHingePosition, armHinge->limit.limit[0].stop + clampMargin, 
        armHinge->limit.limit[1].stop - clampMargin);
    
    desiredArmPrismaticPosition = MeCLAMP(desiredArmPrismaticPosition, armExtend->limit.limit[0].stop + clampMargin, 
        armExtend->limit.limit[1].stop - clampMargin);

    MdtLimitController(MdtHingeGetLimit(mainHinge), desiredMainHingePosition, ME_PI/16, 0.5f, 1000.f);
    MdtLimitController(MdtHingeGetLimit(armHinge), desiredArmHingePosition, ME_PI/16, 0.5f, 10000.f);
    MdtLimitController(MdtPrismaticGetLimit(armExtend), desiredArmPrismaticPosition, 0.5f, 1, 1000.f);

    MeAppStep(meapp);
    MstUniverseStep(universe, step);
}

void MEAPI RotateCranePlus(RRender* rc, void* userData)
{
    MdtBodyEnable(mainBody);   //waky - waky!
    desiredMainHingePosition -= positionIncrement;
}

void MEAPI RotateCraneMinus(RRender* rc, void* userData)
{
    MdtBodyEnable(mainBody);
    desiredMainHingePosition += positionIncrement;
}

void MEAPI MoveCraneUp(RRender* rc, void* userData)
{
    MdtBodyEnable(mainBody);
    desiredArmHingePosition -= positionIncrement;
}

void MEAPI MoveCraneDown(RRender* rc, void* userData)
{
    MdtBodyEnable(mainBody);
    desiredArmHingePosition += positionIncrement;
}

void MEAPI RetractCrane(RRender* rc, void* userData)
{
    MdtBodyEnable(mainBody);
    desiredArmPrismaticPosition += positionIncrement;
}

void MEAPI ExtendCrane(RRender* rc, void* userData)
{
    MdtBodyEnable(mainBody);
    desiredArmPrismaticPosition -= positionIncrement;
}

void cleanup(void)
{
    MeHashDestroy(nameToGraphic);
    MeAssetDBDestroy(db);
    MeAssetInstanceDestroy(ins);
    MeAssetFactoryDestroy(factory);
    MeAppDestroy(meapp);

    MstUniverseDestroy(universe);
    RRenderContextDestroy(rc);
}

void LoadAssetDatabase(MeAssetDB *db, char *filename)
{
    MeAssetDBXMLInput *input;
    MeStream stream;

    input = MeAssetDBXMLInputCreate(db, 0);
    stream = MeStreamOpenWithSearch(filename, kMeOpenModeRDONLY);
    
    if (stream)
    {
        /* load XML and populate asset database */
        MeAssetDBXMLInputRead(input, stream);
        
        MeAssetDBXMLInputDestroy(input);

        MeStreamClose(stream);
    }
}

void CreateCraneGraphics(MeFAsset *asset, MeAssetInstance *ins, MeHash *nameToGraphic)
{
    MeFAssetPartIt it;
    MeFAssetPart *part;

    MeFAssetInitPartIterator(asset, &it);

    while (part = MeFAssetGetPart(&it))
    {
        RGraphic *rg;
        float colour[] = {1, 1, 1, 1};
        McdModelID model = MeAssetInstanceGetModel(ins, MeFAssetPartGetName(part));
        MeVector3 offset;
        MeReal scale;
        char *hint = MeFAssetPartGetGraphicHint(part);

        /* if the asset part has a graphic, use it */
        if (hint)
        {
            MeFAssetPartGetGraphicOffset(part, offset);
            scale = MeFAssetPartGetGraphicScale(part);
        }
        else
        {
            MeFGeometry *geom = MeFAssetPartGetGeometry(part);
        
            /* else if geometry has graphic, use it */
            if (geom)
            {
                hint = MeFGeometryGetGraphicHint(geom);

                if (hint)
                {
                    MeFGeometryGetGraphicOffset(geom, offset);
                    scale = MeFGeometryGetGraphicScale(geom);
                }
            }
        }
        
        if (hint && strstr(hint, ".ase"))
        {
            rg = RGraphicLoadASE(rc, hint, scale, scale, scale, colour, McdModelGetTransformPtr(model));
            if (rg)
                RGraphicTranslateVertices(rc,rg, offset);
        }
        else /* if no graphic hints just create graphic from collision */
        {
            rg = RGraphicCreateFromModel(rc, model, colour);
        }

        if (rg)
        {
            RGraphicSetTexture(rc, rg, "crane");
            MeHashInsert(MeFAssetPartGetName(part), rg, nameToGraphic);
        }
    }
}

int main(int argc, const char * argv[])
{
    float color[4];
    MdtContactParamsID p;
    MeCommandLineOptions* options;

    /* Initialise rendering attributes, eating command line parameters we recognize. */

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    
    if (!rc)
      return 1;

    /* ************* Set up simulation *************** */

    universe = MstUniverseCreate(&MstUniverseDefaultSizes);
    framework = MstUniverseGetFramework(universe);
    space = MstUniverseGetSpace(universe);
    world = MstUniverseGetWorld(universe);
    bridge = MstUniverseGetBridge(universe);

    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
    MdtWorldSetEpsilon(world, 0.001f);

    meapp = MeAppCreateFromUniverse(universe, rc);
    McdConvexMeshRegisterType(framework);
    McdConvexMeshPrimitivesRegisterInteractions(framework);

    /* ground plane */
    planeGeom = McdPlaneCreate(framework);
    plane = MstFixedModelCreate(universe, planeGeom, groundTM);

    p = MstBridgeGetContactParams(bridge,
            MstBridgeGetDefaultMaterial(),
            MstBridgeGetDefaultMaterial());

    MdtContactParamsSetType(p, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(p, 2.0f);
    MdtContactParamsSetRestitution(p, 0.3f);

    /* ************* Create the asset *************** */

    db = MeAssetDBCreate();
    factory = MeAssetFactoryCreate(framework);
    nameToGraphic = MeHashCreate(17);

    LoadAssetDatabase(db, "crane.ka");

    crane = MeAssetDBLookupAssetByName(db, "crane");

    if (crane)
    {
        MeMatrix4 tm;
        MeMatrix4TMMakeIdentity(tm);
        MeMatrix4TMSetPosition(tm, 0, -1.25f, 0);
        
        ins = MeAssetInstanceCreate(factory, crane, tm, 1, world, space);

        if (ins)
        {
            mainHinge = (MdtHingeID)MeAssetInstanceGetJoint(ins, "base");
            armHinge = (MdtHingeID)MeAssetInstanceGetJoint(ins, "j1");
            armExtend = (MdtPrismaticID)MeAssetInstanceGetJoint(ins, "prism");
            baseModel = MeAssetInstanceGetModel(ins, "base");
            armModel = MeAssetInstanceGetModel(ins, "main_arm");
            extenderModel = MeAssetInstanceGetModel(ins, "extender");
            mainBody =  McdModelGetBody(baseModel);

            limit[0] = MdtHingeGetLimit(mainHinge);
            limit[1] = MdtHingeGetLimit(armHinge);
            limit[2] = MdtPrismaticGetLimit(armExtend);

            CreateCraneGraphics(crane, ins, nameToGraphic);
        }
        else
        {
            MeFatalError(0, "Couldn't instance crane");
        }
    }
    else
    {
        MeFatalError(0, "Couldn't load crane");
    }

    /* ***************** Renderer ***************** */

    RPerformanceBarCreate(rc);

    color[0] = color[1] = color[2] = color[3] = 1;

    groundG = RGraphicGroundPlaneCreate(rc, 30.0f, 30, color, groundTM[3][1]);

    RGraphicSetTexture(rc, groundG, "checkerboard");
    RRenderSetWindowTitle(rc, "Crane example");

    /* CONTROLS: */
    RRenderSetUpCallBack(rc, MoveCraneUp, 0);
    RRenderSetDownCallBack(rc, MoveCraneDown, 0);
    RRenderSetLeftCallBack(rc, RotateCraneMinus, 0);
    RRenderSetRightCallBack(rc, RotateCranePlus, 0);
    RRenderSetActionNCallBack(rc, 4, ExtendCrane, 0);
    RRenderSetActionNCallBack(rc, 5, RetractCrane, 0);

    RRenderCreateUserHelp(rc, help, 6);
    RRenderToggleUserHelp(rc);

    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);

    rc->m_bUseAmbientLight = 0;

    {
        MeVector3 lookat = {1, 0, 0};
        RCameraSetLookAt(rc, lookat);
    }

    RCameraSetView(rc, 5, ME_PI/8, ME_PI/8);
    RCameraZoom(rc, 5);

    /* Cleanup after simulation. */
    atexit(cleanup);

    /* Run the Simulation. */
    RRun(rc, tick,0);

    return 0;
}
