/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:35 $ - Revision: $Revision: 1.26.8.3 $

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
   Demonstrates basic loading functionality.
   Loads dynamics, collision detection and graphics.
   A file is loaded and a handle to a dynamics body is obtained.
   For more details see cradle.me.
*/

#include <MeApp.h>
#include <MeAppLoad.h>

MeApp *app;
MstUniverseID universe;
RRender *rc;
MeLoadContext *lc;

void MEAPI tick(RRender* rc, void *userdata)
{
    MeApp *app = (MeApp*)userdata;
    MstUniverseStep(MeAppGetMstUniverse(app),(MeReal)0.02);

    /* update demo support stuff such as mouse picking */
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
    MeHash *hash;
    MeStream stream;
    MeBool success;
    MdtBclContactParams *params;
    MdtBodyID ball = 0;

    MeCommandLineOptions* options;
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
        return 1;

    /*
     * You must make sure that the universe is large enough to cope with all the
     * items specfied in the file.
     */

    sizes = MstUniverseDefaultSizes;
    sizes.dynamicBodiesMaxCount = 100;
    sizes.dynamicConstraintsMaxCount = 100;
    sizes.materialsMaxCount = 1;
    sizes.collisionModelsMaxCount = 100;
    sizes.collisionPairsMaxCount = 50;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.collisionUserGeometryTypesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);

    /*
     * MeApp provides some useful application support functions such as creating
     * graphics based on collision geometry, mouse picking, visual debug output
     * and loading complete scenes with geometry, dynamics and graphics.
     */

    app = MeAppCreateFromUniverse(universe,rc);

    /*
     * This hash table will contain the contents of cradle.me. Once loaded, handles
     * to the items in the file can be obtained by using the contents of the <ID>
     * field in a hash lookup function.
     */

    lc = MeLoadContextCreate();

    stream = MeStreamOpenWithSearch("loadtutorial1.me",kMeOpenModeRDONLY);

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

    hash = MeLoadContextGetHash(lc);

    /* get handles to objects we want to manipulate */

    ball = MdtBodyLookup("BALL0",hash);

    /*
     * For constraints use MdtConstraintLookup(), for geometries use
     * McdGeometryLookup, for models use McdModelLookup() and for
     * graphics use RGraphicLookup().
     */

    if (ball)
        MdtBodyAddImpulse(ball,-50,0,0);

    /* set up contact paramaters */

    params = MstBridgeGetContactParams(MstUniverseGetBridge(universe),
                   MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsSetRestitution(params, (MeReal)0.6);

#ifndef PS2
  atexit(cleanup);
#endif

    RRenderSetWindowTitle(rc, "LoadTutorial1 tutorial");
    RPerformanceBarCreate(rc);
    RRenderSetMouseCallBack(rc, MeAppMousePickCB,(void*)app);

    {
        char *help[] =
        {
            "This scene has been loaded from loadtutorial1.me",
            "$MOUSE applies force",
        };
        RRenderCreateUserHelp(app->rc,help,sizeof(help)/sizeof(help[0]));
        RRenderToggleUserHelp(app->rc);
    }

    RRun(rc,tick,(void*)app);

    return 0;
}
