/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:35 $ - Revision: $Revision: 1.25.8.3 $

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

/*
   Demonstrates loading when the .me files are hierarchical.
 */

#include <MeApp.h>
#include <MeAppLoad.h>

MstUniverseID universe;
MeApp *app;
RRender *rc;
MeLoadContext *lc;

void MEAPI tick(RRender* rc, void *userdata)
{
    MeApp *app = (MeApp*)userdata;
    MstUniverseStep(MeAppGetMstUniverse(app),(MeReal)0.025);
    MeAppStep(app);
}

void MEAPI_CDECL cleanup(void)
{
    /* The load context must be destroyed at the end of the app */
    MeLoadContextDestroy(lc);
    MeAppDestroy(app);
    RRenderContextDestroy(rc);
    MstUniverseDestroy(universe);
}

int MEAPI_CDECL main (int argc,const char *argv[])
{
    MstUniverseSizes sizes;
    MeStream stream;
    MeBool success;
    MdtBclContactParams *params;

    MeCommandLineOptions* options;
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

    sizes = MstUniverseDefaultSizes;
    sizes.dynamicBodiesMaxCount = 200;
    sizes.dynamicConstraintsMaxCount = 200;
    sizes.materialsMaxCount = 1;
    sizes.collisionModelsMaxCount = 200;
    sizes.collisionPairsMaxCount = 200;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.collisionUserGeometryTypesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);

    app = MeAppCreateFromUniverse(universe,rc);

    lc = MeLoadContextCreate();

    stream = MeStreamOpenWithSearch("loadtutorial2.me",kMeOpenModeRDONLY);

    if (!stream) {
        MeInfo(0,"Error opening file.");
        exit(1);
    }

    success = MeAppLoad(app,stream,lc);

    if (!success) {
        MeInfo(0,"Error loading file.");
        exit(1);
    }

    MeStreamClose(stream);

    params = MstBridgeGetContactParams(MstUniverseGetBridge(universe),
                MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsSetRestitution(params, (MeReal)0.6);

    RCameraZoom(rc,15);
    RCameraUpdate(rc);
    RRenderSetMouseCallBack(rc, MeAppMousePickCB,(void*)app);
    RPerformanceBarCreate(rc);
    RRenderSetWindowTitle(rc, "LoadTutorial2 tutorial");

    {
        char *help[] =
        {
            "This scene is made up of several .me files",
            "$MOUSE applies force",
        };
        RRenderCreateUserHelp(app->rc,help,sizeof(help)/sizeof(help[0]));
        RRenderToggleUserHelp(app->rc);
    }

#ifndef PS2
    atexit(cleanup);
#endif

    RRun(rc,tick,(void*)app);

    return 0;
}
