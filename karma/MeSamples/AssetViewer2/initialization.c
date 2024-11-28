/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.5.2.1 $

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

#include "AssetViewer2.h"
#include "commandline.h"
#include "initialization.h"
#include "visualization.h"
#include "simulation.h"

MeReal scale = 1;
MeBool bNetInitDone = 0;

/* simulation options */
MdtContactParamsID contactparams;
MeReal timestep = (MeReal)0.02;

/* visualization options */
int yIsUp = 0;
int displayHelp = 1;
int isPaused = 0;
int drawmeshes = 1;
int drawcollision = 1;
int drawcontacts = 1;
int showDynamicsOrigins = 1;
int showMassOrigins = 0;
int drawBSJoints = 0;
int drawHinges = 0;
int drawCarwheels = 0;
int drawPrismatics = 0;

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


void InitializeKarma(void)
{
    MstUniverseSizes sizes;
    sizes = MstUniverseDefaultSizes;
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
    McdConvexMeshRegisterType(frame);
    McdConvexMeshPrimitivesRegisterInteractions(frame);
    
    af = MeAssetFactoryCreate(world, space, frame);
    
    /* set default contact parameters */
    
    contactparams = MstBridgeGetContactParams(bridge,
        MstBridgeGetDefaultMaterial(),
        MstBridgeGetDefaultMaterial());
    
    MdtContactParamsSetRestitution(contactparams, (MeReal)0.2);
    MdtContactParamsSetType(contactparams, MdtContactTypeFriction2D);
    MdtContactParamsSetFriction(contactparams, 1);
}

void InitializeRenderer()
{
    MeVector3 g;
    char *help[] =
    {
        "$MOUSE - applies force",
        "$ACTION1 - toggle options menu",
        "$ACTION3 - command line mode",
        "$ACTION4 - single step",
     };

   
    RCameraZoom(rc,-5);
    RCameraUpdate(rc);
    RRenderSetWindowTitle(rc, "Asset viewer");
    RRenderCreateUserHelp(rc,help,sizeof(help)/sizeof(help[0]));

    if (displayHelp)
        RRenderToggleUserHelp(rc);

    RRenderSetActionNCallBack(rc,3,DoCommandLineLoop,0);
    RRenderSetActionNCallBack(rc,4,single_step,0);

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
    RMenuAddToggleEntry(visualsMenu, "Display graphic meshes", ToggleDrawMeshes, drawmeshes);    
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
    
    {
        float color[4];
        color[0] = color[1] = color[2] = color[3] = 1;
        groundG = RGraphicGroundPlaneCreate(rc, 24, 2, color, -1);
        RGraphicSetTexture(rc, groundG, "checkerboard");
    }    
    
    rc->m_DirectLight1.m_bUseLight = 0;
    rc->m_DirectLight2.m_bUseLight = 0;
    rc->m_PointLight.m_bUseLight = 1;
    rc->m_PointLight.m_Position[0] = 0;
    rc->m_PointLight.m_Position[1] = 10;
    rc->m_PointLight.m_Position[2] = 0;
    rc->m_PointLight.m_rgbAmbient[0] = rc->m_PointLight.m_rgbDiffuse[0] = 0.4f;
    rc->m_PointLight.m_rgbAmbient[1] = rc->m_PointLight.m_rgbDiffuse[1] = 0.4f;
    rc->m_PointLight.m_rgbAmbient[2] = rc->m_PointLight.m_rgbDiffuse[2] = 0.4f;
    rc->m_bForceDirectLight1Update = 1;
    rc->m_bForceDirectLight2Update = 1;
    rc->m_bForcePointLightUpdate = 1;    
    
}

void InitApp()
{    
    float contactcolor[] = {0,1,0,1};
    
    if(!app)
        return;
    
    MeAppDrawContactsInit(app,contactcolor,100);
    
    RRenderSetMouseCallBack(rc, MeAppMousePickCB,(void*)app);    
    
    planeGeom = McdPlaneCreate(frame);
    plane = MstFixedModelCreate(universe,planeGeom,groundTM);                
    
    RebuildGraphics();    
}
