/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:36 $ - Revision: $Revision: 1.32.8.3 $

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
   Demonstrates loading functionality, including user data.
 */

#include <MeApp.h>
#include <MeAppLoad.h>

MeXMLError MEAPI UserAppsHandler(MeXMLElement *elem);

MstUniverseID universe;
MeApp *app;
RRender *rc;
MeLoadContext *lc;

void MEAPI tick(RRender* rc, void *userdata)
{
    MeApp *app = (MeApp*)userdata;
    MstUniverseStep(MeAppGetMstUniverse(app),(MeReal)0.02);
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
    MdtBclContactParams *params;
    MeBool success;

    MeCommandLineOptions* options;
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

    sizes = MstUniverseDefaultSizes;
    sizes.dynamicBodiesMaxCount = 100;
    sizes.dynamicConstraintsMaxCount = 100;
    sizes.materialsMaxCount = 1;
    sizes.collisionModelsMaxCount = 100;
    sizes.collisionPairsMaxCount = 50;
    sizes.collisionGeometryInstancesMaxCount = 0;
    sizes.collisionUserGeometryTypesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);

    app = MeAppCreateFromUniverse(universe,rc);

    lc = MeLoadContextCreate();

    MeLoadContextSetUserdataHandler(lc,UserAppsHandler,0);

    /* Load file */

    stream = MeStreamOpenWithSearch("loadtutorial3.me",kMeOpenModeRDONLY);

    if (!stream) {
        MeInfo(0,"Error opening file.");
    }

    success = MeAppLoad(app,stream,lc);

    if (!success) {
        MeInfo(0,"Error loading file.");
    }

    MeStreamClose(stream);

    /* set up contact paramaters */
    params = MstBridgeGetContactParams(MstUniverseGetBridge(universe),
                              MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsSetRestitution(params, (MeReal)0.6);
    MdtContactParamsSetFriction(params, (MeReal)2.0);

    /* set up renderer */
    RCameraRotateElevation(rc,0.5);
    RCameraUpdate(rc);
    RPerformanceBarCreate(rc);
    RRenderSetMouseCallBack(rc, MeAppMousePickCB,(void*)app);
    RRenderSetWindowTitle(rc, "LoadTutorial3 tutorial");

    {
        char *help[] =
        {
            "Demonstrates how to extend .me file format. See loadtutorial3.c",
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

/*
 * The rest of the code handles user data. For details on how to use the
 * MathEngine XML parser, see the Tools And Utilities html reference pages
 * or the loading and saving manual in the docs directory.
 */

typedef struct
{
    int some_int;
    char mdtbody[ME_XML_TAG_BUFFER_SIZE];
} My_XML_element;


MeXMLError MEAPI Handle_My_XML_Element(MeXMLElement *elem)
{
    MeXMLError error;
    My_XML_element e = {0};

    /*
     * If the user data refers to a previously created MathEngine object you
     * can get the hash table and look up the MdtBodyID.
     */

    MeLoadContext *lc = (MeLoadContext*)
        MeXMLInputGetUserData(MeXMLElementGetInput(elem));

    /* shows how to get hold of the userdata passed in when the handler was set */
    void *userdata = MeLoadContextGetUserdata(lc);

    char fullID[300];

    MeHash *hash = MeLoadContextGetHash(lc);

    MeXMLHandler handlers[] = {
        ME_XML_INT_HANDLER("SOME_INT", My_XML_element, some_int, NULL),
        ME_XML_STRING_HANDLER("MDTBODY", My_XML_element, mdtbody,ME_XML_TAG_BUFFER_SIZE, NULL),
        ME_XML_HANDLER_END
    };

    error = MeXMLElementProcess(elem,handlers,&e);

    if (MeXMLHandlerWasCalled(handlers,"MDTBODY")) {
        MdtBodyID b;

        /*
         * This is essential if the .me files are in a hierarchy.
         * The full path has to be obtained.
         */

        MeLoadContextMakeFullID(lc->IDpath,e.mdtbody,fullID);

        b = MdtBodyLookup(fullID,hash);

        if (!b) {
            MeInfo(0,"Body doesn't exist.\n");
        }
    }

    return error;
}

MeXMLError MEAPI UserAppsHandler(MeXMLElement *elem)
{
    MeXMLError error;

    MeXMLHandler handlers[] = {
        ME_XML_ELEMENT_HANDLER("MY_XML_ELEMENT", Handle_My_XML_Element),
        ME_XML_HANDLER_END
    };

    error = MeXMLElementProcess(elem,handlers,0);

    return error;
}
