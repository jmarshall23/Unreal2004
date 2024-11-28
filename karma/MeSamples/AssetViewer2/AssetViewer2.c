/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.7.2.2 $

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

#include "instances.h"
#include "initialization.h"
#include "simulation.h"
#include "visualization.h"
#include "commandline.h"
#include "network.h"

void ResetApp()
{
    DestroyGraphics();
    MeAssetFactoryDestroy(af);    
    MstUniverseDestroy(universe);
    
    {
        int i;
        for(i=0;i<app->contactDrawInfo->maxContacts;i++)
            RGraphicRemoveFromList(app->rc, app->contactDrawInfo->contactG[i], 0);
    }

    MeAppDestroy(app);
    InitializeKarma();
    InitApp();
    InstanceAssets();
}

void FlushAssetDB()
{
    if(db)
    {
        MeAssetDBDestroy(db);
        MeIDPoolDestroy(idPool);
    }
    db = MeAssetDBCreate();
    idPool = MeIDPoolCreate();

    ResetApp();
}

void LoadAssetDB(MeStream stream)
{
    MeAssetDBXMLInput *input;
    
    if(!db)
    {
        db = MeAssetDBCreate();
        idPool = MeIDPoolCreate();
    }
    
    if(!db)
        return;
    
    
    input = MeAssetDBXMLInputCreate(db, idPool);            
    MeAssetDBXMLInputRead(input, stream);    
    
    ResetApp();
}

void SetAssetDB(MeStream stream)
{
    /* this is just like
       FlushAssetDB(); LoadAssetDB(stream);
       but without the extra reset */

    if(db)
    {
        MeAssetDBDestroy(db);
        MeIDPoolDestroy(idPool);
    }

    db = MeAssetDBCreate();
    idPool = MeIDPoolCreate();

    LoadAssetDB(stream);
}

void FlushAll()
{
    RemoveAllInstancesNoReset();
    FlushAssetDB();
}

void MEAPI tick(RRender *rc,void *userdata)
{
    MstUniverseStep(universe,timestep);
    UpdateConstraintGraphics();
    MeAppStep(app);
    if(bNetInitDone)
        NetworkPoll();
}


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
    InitializeRenderer();
    InitApp();

    bNetInitDone = !NetworkInit();
    if(!bNetInitDone)
        MeWarning(0,"Couldn't initialize network.");

    /* create the asset database which will be filled from XML files */
    db = 0;
    FlushAssetDB();
    
#ifndef PS2
    atexit(cleanup);
#endif

    RRun(rc,tick,0);

    return 0;
}

void MEAPI_CDECL cleanup(void)
{
    if(bNetInitDone)
        NetworkCleanup();
    RemoveAllInstancesNoReset();
    FlushAseList();
    MeCommandLineOptionsDestroy(options);
    RRenderContextDestroy(rc);
    RMenuDestroy(mainMenu);
    RMenuDestroy(visualsMenu);
    RMenuDestroy(simMenu);
    MeAssetDBDestroy(db);
    MeAssetFactoryDestroy(af);    
    MstUniverseDestroy(universe);    
    MeAppDestroy(app);
}


