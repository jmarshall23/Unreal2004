/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name$

   Date: $Date$ - Revision: $Revision$

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

#include <stdlib.h>
#include <stdio.h>

#define ME_NEW_MCD_API

#include <MeApp.h>
#include <MeAppLoad.h>
#include <RMenu.h>
#include "simulation.h"
#include "visualization.h"

MstUniverseID universe;
MstBridgeID bridge;
McdSpaceID space;
McdFrameworkID frame;
MdtWorldID world;
MeLoadContext *lc;
MeApp *app;
char *file;
RRender *rc;
RMenu *mainMenu;
RMenu *visualsMenu;
RMenu *simMenu;
RMenu *constraintsMenu;


/* loader options */
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
int showDynamicsOrigins = 0;
int showMassOrigins = 0;
int drawBSJoints = 0;
int drawHinges = 0;
int drawCarwheels = 0;
int drawPrismatics = 0;




void MEAPI_CDECL cleanup(void)
{
    MeLoadContextDestroy(lc);
    RRenderContextDestroy(rc);
    RMenuDestroy(mainMenu);
    RMenuDestroy(visualsMenu);
    RMenuDestroy(simMenu);
    MstUniverseDestroy(universe);
    MeAppDestroy(app);
}


void createMeApp(void)
{
    MstUniverseSizes sizes;
    McdFrameworkID frame;

    sizes = MstUniverseDefaultSizes;
    sizes.collisionModelsMaxCount = 1000;
    sizes.collisionPairsMaxCount = 1200;
    sizes.collisionGeometryInstancesMaxCount = 1000;
    sizes.dynamicBodiesMaxCount = 1000;
    sizes.dynamicConstraintsMaxCount = 2000;
    sizes.materialsMaxCount = 10;
    sizes.collisionUserGeometryTypesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);
    frame = MstUniverseGetFramework(universe);

    world = MstUniverseGetWorld(universe);
    space = MstUniverseGetSpace(universe);
    bridge = MstUniverseGetBridge(universe);
    frame = MstUniverseGetFramework(universe);

    app = MeAppCreateFromUniverse(universe,rc);

    McdTriangleMeshRegisterType(frame);
    McdTriangleMeshRegisterInteractions(frame);
    McdConvexMeshRegisterType(frame);
    McdConvexMeshPrimitivesRegisterInteractions(frame);
    McdAggregateRegisterType(frame);
    McdAggregateRegisterInteractions(frame);
}


void loadScene(char *file,int yIsUp)
{
    MeBool rcode;
    MeStream stream;
    char delimiter = 0x00;
    char workingDirectory[200];
    char filename[200];

    strcpy(filename,file);
    if (!strstr(filename,".me"))
        strcat(filename,".me");

    if (strchr(file,'/'))
        delimiter = '/';
    else if (strchr(file,'\\'))
        delimiter = '\\';

    if (delimiter) {
        char *f;
        strcpy(workingDirectory,file);
        f = strrchr(workingDirectory,delimiter) + 1;
        *f = '\0';
        MeLoadContextSetWorkingDirectory(lc,workingDirectory);
    }

    stream = MeStreamOpen(filename,kMeOpenModeRDONLY);

    if (!stream) {
        stream = MeStreamOpenWithSearch(filename,kMeOpenModeRDONLY);
        if (!stream) {
            MeInfo(0,"%s not found.",file);
            exit(1);
        }
    }

    /* transform everything if the stuff in the file/s is Y is up */
    if (!yIsUp) {
        MeMatrix4 offset;
        MeMatrix3 m;
        MeMatrix3MakeRotationX(m,ME_PI/2);
        MeMatrix4TMMakeFromRotationAndPosition(offset,(void *)m,0,0,0);
        MeLoadContextSetTransform(lc,offset);
    }

    MeLoadContextSetScaleFactor(lc,scale);

    rcode = MeAppLoad(app,stream,lc);

    if (rcode == 0)
        MeWarning(0,"Error loading %s.",file);

    MeStreamClose(stream);

}


int MEAPI_CDECL main(int argc, const char **argv)
{
    MeCommandLineOptions* options;
    MeVector3 g;
    float textcolor[] = {1,0,0,1};
    float contactcolor[] = {0,1,0,1};
    char *help[] =
    {
        "$MOUSE - applies force",
        "$ACTION1 - toggle options menu",
        "$ACTION4 - single step",
     };

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

    if (MeCommandLineOptionsCheckFor(options,"--help",MEFALSE) ||
        MeCommandLineOptionsCheckFor(options,"-h",MEFALSE))
    {
        MeInfo(0,"-file FILENAME    where FILENAME is a file with a .me extension");
        MeInfo(0,"-y                Specifies that Y is up. By default Z is up.");
        MeInfo(0,"-paused           Starts simulation paused.");
        MeInfo(0,"-nohelp           Starts simulation with no help.");
        MeInfo(0,"-scale x          Applies scale factor 'x'.");
        return 0;
    }

    rc = RRenderContextCreate(options, 0, !MEFALSE);
    if (!rc)
      return 1;

    file = MeCommandLineOptionsGetString(options,"-file",1);
    yIsUp = MeCommandLineOptionsCheckFor(options,"-y",1);
    isPaused = MeCommandLineOptionsCheckFor(options,"-paused",1);
    displayHelp = !MeCommandLineOptionsCheckFor(options,"-nohelp",1);
    if (MeCommandLineOptionsCheckFor(options,"-scale",0))
        scale = (MeReal)MeCommandLineOptionsGetFloat(options,"-scale",1);

    if (isPaused) RRenderSetPause(rc,1);

    createMeApp();
    lc = MeLoadContextCreate();
    McdTriangleMeshRegisterLoader(lc);
    McdConvexMeshRegisterLoader(lc);

    if (file) {
        loadScene(file,yIsUp);
    }
    else {
        RGraphicTextCreate(rc,"Command line:",150,150,textcolor);
        RGraphicTextCreate(rc,"-file  Name of a .me file (required)",150,180,textcolor);
        RGraphicTextCreate(rc,"-y    Specifies that the .me file expects Y to be up.",150,210,textcolor);
        RGraphicTextCreate(rc,"       By default Z is assumed to be up.",150,240,textcolor);
        RGraphicTextCreate(rc,"-h    For help on other options.",150,270,textcolor);
    }

    MeCommandLineOptionsDestroy(options);

    contactparams = MstBridgeGetContactParams(bridge,
                                      MstBridgeGetDefaultMaterial(),
                                      MstBridgeGetDefaultMaterial());

    MdtContactParamsSetRestitution(contactparams, (MeReal)0.2);

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
    RRenderSetWindowTitle(rc, "SceneLoader");
    RRenderCreateUserHelp(app->rc,help,sizeof(help)/sizeof(help[0]));

    if (displayHelp)
        RRenderToggleUserHelp(app->rc);
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
    RMenuAddValueEntry(simMenu, "Global friction", SetFriction,50,0,1,0);
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

#ifndef PS2
    atexit(cleanup);
#endif

    RRun(rc,tick,0);

    return 0;
}

