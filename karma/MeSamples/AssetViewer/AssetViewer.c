/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.8.2.1 $

   This software and its accompanying manuals have been developed
   by Mathengine PLC ("MathEngine") and the copyright and all other
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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#include <MeApp.h>
#include <MeAssetDB.h>
#include <MeAssetFactory.h>
#include <RMenu.h>

#include <MeAssetDBXMLIO.h>
#include <MeStream.h>

#include "simulation.h"
#include "visualization.h"

MeCommandLineOptions    *options;
MstUniverseID           universe;
MstBridgeID             bridge;
McdSpaceID              space;
McdFrameworkID          frame;
MdtWorldID              world;
MeApp                   *app;
MeAssetDB               *db;
MeAssetFactory          *af;
RRender                 *rc;
RMenu                   *mainMenu;
RMenu                   *visualsMenu;
RMenu                   *simMenu;
RMenu                   *constraintsMenu;
char                    *file;

McdGeometryID           planeGeom;
McdModelID              plane;
RGraphic                *groundG;

MeReal scale = 1;

/* simulation options */
MdtContactParamsID contactparams;
MeReal timestep = (MeReal)0.02;

/* visualization options */
int yIsUp = 0;
int displayHelp = 1;
int isPaused = 0;
int drawcollision = 1;
int drawcontacts = 1;
int showDynamicsOrigins = 1;
int showMassOrigins = 0;
int drawBSJoints = 0;
int drawHinges = 0;
int drawCarwheels = 0;
int drawPrismatics = 0;




void MEAPI_CDECL cleanup(void)
{
    MeCommandLineOptionsDestroy(options);
    RRenderContextDestroy(rc);
    RMenuDestroy(mainMenu);
    RMenuDestroy(visualsMenu);
    RMenuDestroy(simMenu);
    MstUniverseDestroy(universe);
    MeAppDestroy(app);
    MeAssetDBDestroy(db);
}

void InitializeKarma(void)
{
    MstUniverseSizes sizes;
    sizes.collisionModelsMaxCount = 1000;
    sizes.collisionPairsMaxCount = 1200;
    sizes.collisionGeometryInstancesMaxCount = 100;
    sizes.dynamicBodiesMaxCount = 1000;
    sizes.dynamicConstraintsMaxCount = 2000;
    sizes.materialsMaxCount = 10;
    sizes.collisionUserGeometryTypesMaxCount = 0;
    sizes.collisionGroupsMaxCount = 1;

    universe = MstUniverseCreate(&sizes);

    world = MstUniverseGetWorld(universe);
    space = MstUniverseGetSpace(universe);
    bridge = MstUniverseGetBridge(universe);
    frame = MstUniverseGetFramework(universe);

    MdtWorldSetGravity(world, 0, -10, 0);
    app = MeAppCreateFromUniverse(universe,rc);

    /* register all required collision types */
    McdNullRegisterType(frame);
    McdConvexMeshRegisterType(frame);
    McdConvexMeshPrimitivesRegisterInteractions(frame);
    McdAggregateRegisterType(frame);
    McdAggregateRegisterInteractions(frame);

    /* create the asset database which will be filled from XML files */
    db = MeAssetDBCreate();

    af = MeAssetFactoryCreate(frame);

    /* set default contact parameters */

    contactparams = MstBridgeGetContactParams(bridge,
                                      MstBridgeGetDefaultMaterial(),
                                      MstBridgeGetDefaultMaterial());

    MdtContactParamsSetRestitution(contactparams, (MeReal)0.2);
    MdtContactParamsSetType(contactparams, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(contactparams, 1);
}

void InstanceAssets()
{
    MeFAssetIt it;
    MeFAsset *asset;

    MeAssetDBInitAssetIterator(db, &it);
    
    while (asset = MeAssetDBGetAsset(&it))
    {
        MeAssetFactoryInstanceAsset(af, asset, "car", 0, world, space);
    }
}

typedef char input_t[256];

static int tokenize(char *string, ...)
{
    char **tokptr; 
    va_list arglist;
    int tokcount = 0;

    va_start(arglist, string);
    tokptr = va_arg(arglist, char **);
    while (tokptr) {
	while (*string && isspace((unsigned char) *string))
	    string++;
	if (!*string)
	    break;
	*tokptr = string;
	while (*string && !isspace((unsigned char) *string))
	    string++;
	tokptr = va_arg(arglist, char **);
	tokcount++;
	if (!*string)
	    break;
	*string++ = 0;
    }
    va_end(arglist);

    return tokcount;
}

void MEAPI DoCommandLineLoop(RRender *rc, void *userdata)
{
    char *help =
    "l <file>                               load a .ka file\n"
    "i <assetname> <instance name>          instance an asset\n"
    "q                                      return to simulation";

    input_t in;

    char *tok1, *tok2;

    puts(help);
    
    while (1)
    {
        putchar('>');
        fflush(stdout);
        
        if (!fgets(in, sizeof(input_t), stdin))
            break;
        
        if (strncmp(in, "?", 1) == 0)
        {
            puts(help);
        }
            
        else if (strncmp(in, "q", 1) == 0)
        {
            return;
        }

        /* load a whole .ka file */
        else if (strncmp(in, "l", 1) == 0)
        {
            MeStream stream;
            MeAssetDBXMLInput *input;
            if (tokenize(in + 1, &tok1, 0) != 1) {
                puts("what?");
                continue;
            }
            
            stream = MeStreamOpen(tok1, kMeOpenModeRDONLY);
        
            if (!stream)
            {
                puts("file not found");
                continue;
            }

            input = MeAssetDBXMLInputCreate(db);            
            MeAssetDBXMLInputRead(input, stream);
            MeStreamClose(stream);
            
            InstanceAssets();
            ToggleDrawCollisionGeom(1);
        }
        
        else if (strncmp(in, "i", 1) == 0)
        {
            MeFAsset *asset;
            if (tokenize(in + 1, &tok1, &tok2, 0) != 2) {
                puts("what?");
                continue;
            }

            asset = MeAssetDBLookupAsset(db, tok1);
            MeAssetFactoryInstanceAsset(af, asset, tok2, 0, world, space);
            ToggleDrawCollisionGeom(1);
        }
        
        else
        {
            puts("what?");
        }
    }

}

void InitializeRenderer()
{
    MeVector3 g;
    float contactcolor[] = {0,1,0,1};
    char *help[] =
    {
        "$MOUSE - applies force",
        "$ACTION1 - toggle options menu",
        "$ACTION3 - command line mode",
        "$ACTION4 - single step",
     };

    MeAppDrawContactsInit(app,contactcolor,100);
    
    if (drawcontacts)
        MeAppToggleDrawContacts(app,1);

    if (drawcollision)
        ToggleDrawCollisionGeom(1);

    if (showDynamicsOrigins)
        ToggleDynamicsOrigins(1);
    
    if (showMassOrigins)
        ToggleMassOrigins(1);

    if (drawBSJoints)
        ToggleDrawBSJoints(1);

    if (drawHinges)
        ToggleDrawHinges(1);

    if (drawCarwheels)
        ToggleDrawCarWheelJoints(1);
    
    if (drawPrismatics)
        ToggleDrawPrismatics(1);
    
    RCameraZoom(rc,-5);
    RCameraUpdate(rc);
    RRenderSetWindowTitle(rc, "Asset viewer");
    RRenderCreateUserHelp(app->rc,help,sizeof(help)/sizeof(help[0]));

    if (displayHelp)
        RRenderToggleUserHelp(app->rc);

    RRenderSetActionNCallBack(rc,3,DoCommandLineLoop,0);
    RRenderSetActionNCallBack(rc,4,single_step,0);
    RRenderSetMouseCallBack(rc, MeAppMousePickCB,(void*)app);

    mainMenu = RMenuCreate(rc, "Main Menu");
    visualsMenu = RMenuCreate(rc, "Visualization Menu");
    simMenu = RMenuCreate(rc, "Simulation Menu");
    constraintsMenu = RMenuCreate(rc, "Constraints Menu");

    MdtWorldGetGravity(world,g);

    /* main menu */
    RMenuAddSubmenuEntry(mainMenu,"Visualization menu",visualsMenu);
    RMenuAddSubmenuEntry(mainMenu,"Simulation menu",simMenu);

    /* simulation menu */
    RMenuAddValueEntry(simMenu, "Gravity X", SetGravityX, 100, -10, 0.5f, -g[0]);
    RMenuAddValueEntry(simMenu, "Gravity Y", SetGravityY, 100, -10, 0.5f, -g[1]);
    RMenuAddValueEntry(simMenu, "Gravity Z", SetGravityZ, 100, -10, 0.5f, -g[2]);
    RMenuAddValueEntry(simMenu, "Timestep", SetTimestep,0.1f,0.01f,0.01f, timestep);
    RMenuAddValueEntry(simMenu, "Global friction", SetFriction,50,0,1,10);
    RMenuAddSubmenuEntry(simMenu,"Main menu",mainMenu);

    /* visualization menu */
    RMenuAddToggleEntry(visualsMenu, "Display collision geometry", ToggleDrawCollisionGeom, drawcollision);
    RMenuAddToggleEntry(visualsMenu, "Show dynamics origins", ToggleDynamicsOrigins,showDynamicsOrigins);
    RMenuAddToggleEntry(visualsMenu, "Show centres of mass", ToggleMassOrigins,showMassOrigins);
    RMenuAddToggleEntry(visualsMenu, "Draw contacts", ToggleDrawContacts, drawcontacts);
    RMenuAddValueEntry(visualsMenu, "Contact length scale factor", SetContactDrawLength,10,1,0.1f,1);
    RMenuAddSubmenuEntry(visualsMenu,"Constraints menu",constraintsMenu);
    RMenuAddSubmenuEntry(visualsMenu,"Main menu",mainMenu);

    /* constraints menu */
    RMenuAddToggleEntry(constraintsMenu, "Display ball and sockets", ToggleDrawBSJoints, drawBSJoints);
    RMenuAddToggleEntry(constraintsMenu, "Display hinges", ToggleDrawHinges, drawHinges);
    RMenuAddToggleEntry(constraintsMenu, "Display car wheel joints", ToggleDrawCarWheelJoints, drawCarwheels);
    RMenuAddToggleEntry(constraintsMenu, "Display prismatics", ToggleDrawPrismatics, drawPrismatics);
    RMenuAddSubmenuEntry(constraintsMenu,"Visualization menu",visualsMenu);

    RRenderSetDefaultMenu(rc, mainMenu);
    RPerformanceBarCreate(rc);

}


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
    {0, -1.05f,  0, 1}
};

int MEAPI_CDECL main(int argc, const char **argv)
{
#if defined WIN32 && defined _DEBUG && 0
#include <crtdbg.h>
    {
        int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

        debugFlag |= _CRTDBG_ALLOC_MEM_DF;
        debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;
        debugFlag |= _CRTDBG_CHECK_CRT_DF;
        debugFlag |= _CRTDBG_LEAK_CHECK_DF;
        _CrtSetDbgFlag(debugFlag);
    }
#endif

    options = MeCommandLineOptionsCreate(argc, argv);

    rc = RRenderContextCreate(options, 0, !MEFALSE);
    if (!rc)
      return 1;

    isPaused = MeCommandLineOptionsCheckFor(options,"-paused",1);
    displayHelp = !MeCommandLineOptionsCheckFor(options,"-nohelp",1);


    if (isPaused) RRenderSetPause(rc,1);

    InitializeKarma();
    planeGeom = McdPlaneCreate(frame);
    plane = MstFixedModelCreate(universe,planeGeom,groundTM);

    InitializeRenderer();

    {
        float color[4];
        color[0] = color[1] = color[2] = color[3] = 1;
        groundG = RGraphicGroundPlaneCreate(rc, 24, 2, color, -1);
        RGraphicSetTexture(rc, groundG, "checkerboard");
    }
    {
        MeStream stream;
        MeAssetDBXMLInput *input;
        stream = MeStreamOpen("soldier.ka", kMeOpenModeRDONLY);
    
        if (!stream)
        {
            puts("file not found");
        }
    

        input = MeAssetDBXMLInputCreate(db);            
        MeAssetDBXMLInputRead(input, stream);
        MeStreamClose(stream);
        
        InstanceAssets();
        ToggleDrawCollisionGeom(1);
    }

#ifndef PS2
    atexit(cleanup);
#endif
    RRun(rc,tick,0);

    return 0;
}

