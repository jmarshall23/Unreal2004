/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   $Date: 2002/04/04 15:29:14 $ $Revision: 1.9.8.1 $

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
#include <MeMath.h>
#include <MeViewer.h>
#include <MeApp.h>

#include "trebuchet.h"

/* help text */

char *help[6] =
{
    "$ACTION1 - toggle options menu",
    "$ACTION2 - reset",
    "$ACTION3 - shoot",
//    "$ACTION4 - disable all blocks",
//    "$ACTION5 - enable all blocks",
    "$MOUSE - mouse force"
};

// Global constructs
McdFrameworkID framework;
MstUniverseID universe;
McdSpaceID space;
MdtWorldID world;
MstBridgeID bridge;
MeApp* meapp;

// Setup Floor
McdGeometryID	groundGeom;
McdModelID		groundModel;
//MdtBodyID		groundBody;
RGraphic *		groundG;
float		groundColor[3] = {0.0, 1.0, 0.0};
MeMatrix4 groundTM =
{
    {1,  0,  0, 0},
    {0,  0, -1, 0},
    {0,  1,  0, 0},
    {0,  0,  0, 1}
};

/* World for the Dynamics Toolkit simulation */
MdtWorldID world;

MeReal gravity[3] = { 0, -7, 0 };

/* Render context */
RRender *rc;
RMenu* menu;

MeReal step = (MeReal)(0.03);

TrebuchetProto* trebProto;
Trebuchet* treb;


/*************/
/* CallBacks */
/*************/
void MEAPI tick(RRender * rc,void *userdata)
{
    MeAppStep(meapp);
    MstUniverseStep(universe, step);

	// check to see whether to let go of bucket
	Trebuchet_releaseBucket(treb);
}

/* Reset trebuchet to initial positions */
void MEAPI reset(RRender* rc, void* userData)
{

	Trebuchet_reset(treb);

}

void MEAPI shoot(RRender* rc, void* userData)
{
	Trebuchet_fire(treb);
}


void MEAPI_CDECL cleanup(void)
{
    puts("Cleaning up");

    MeAppDestroy(meapp);

	// Destroy the ground plane
    MstFixedModelDestroy(groundModel);
    McdGeometryDestroy(groundGeom);

	Trebuchet_destroy(treb);

	TrebuchetProto_destroy(trebProto);

    MstUniverseDestroy(universe);
    RRenderContextDestroy(rc);

}

void MEAPI togglehelp(RRender* rc, void* userdata) {
    RRenderToggleUserHelp(rc);
}


/****************/
/* Main Routine */
/****************/
int MEAPI_CDECL main(int argc, const char * argv[])
{
    MstUniverseSizes sizes;
    MeCommandLineOptions* options;
    MdtBclContactParams *p;

    /* ************* MAKE PHYSICS *************** */

    sizes = MstUniverseDefaultSizes;
    sizes.dynamicBodiesMaxCount = TrebuchetProto_getNumBodies()+TrebuchetProto_getNumModels()+1;
    sizes.dynamicConstraintsMaxCount = TrebuchetProto_getNumJoints()+300;
    sizes.materialsMaxCount = 2;
    sizes.collisionModelsMaxCount = 200;
    sizes.collisionPairsMaxCount = 1000;
    sizes.collisionGeometryInstancesMaxCount = 10;
    sizes.collisionUserGeometryTypesMaxCount = 0;

    universe = MstUniverseCreate(&sizes);
    framework = MstUniverseGetFramework(universe);
    space = MstUniverseGetSpace(universe);
    world = MstUniverseGetWorld(universe);
    bridge = MstUniverseGetBridge(universe);
	// register collisions with Aggregates
    McdAggregateRegisterType(framework);
	McdAggregateRegisterInteractions(framework);

	// Set the gravity
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);


	/* Sort out the contacts */
    p = MstBridgeGetContactParams(bridge,
            MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());
    MdtContactParamsSetType(p, MdtContactTypeFriction2D);
    MdtContactParamsSetRestitution(p, 0.3);
	MdtContactParamsSetFriction(p, 100.0);

    /* Initialise rendering attributes, eating command line parameters we
       recognize. */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 1;

	/****************/
	/* Create Stuff */
	/****************/
	// Create the only trebuchet prototype
	trebProto = TrebuchetProto_create(universe);
	treb = Trebuchet_create(universe,rc,trebProto);
	Trebuchet_reset(treb);
    /* GROUND PLANE */
    groundGeom	= McdPlaneCreate(framework);
    groundModel	= MstFixedModelCreate(universe,groundGeom,groundTM);

    meapp = MeAppCreate(world, space, rc);

    RPerformanceBarCreate(rc);


    groundG = RGraphicGroundPlaneCreate(rc, 24, 2, groundColor, 0);
    RGraphicSetTexture(rc, groundG, "checkerboard");

    RRenderSkydomeCreate(rc, "skydome", 2, 1);

    /* CONTROLS: */
    RRenderSetActionNCallBack(rc, 2, reset, 0);
    RRenderSetActionNCallBack(rc, 3, shoot, 0);

	RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);


    RRenderSetWindowTitle(rc, "Seige example");
    RRenderCreateUserHelp(rc,help,4);
    RRenderToggleUserHelp(rc);

    /* Cleanup after simulation. */
    atexit(cleanup);

    /* Run the Simulation. */
    RRun(rc, tick,0);

    return 0;
}


/*
void MEAPI ToggleHighQualityFriction(MeBool on)
{
    MdtContactParamsID boxboxprops = MstBridgeGetContactParams(bridge,
        MstBridgeGetDefaultMaterial(), MstBridgeGetDefaultMaterial());

    MdtContactParamsID boxfloorprops = MstBridgeGetContactParams(bridge,
        floorMaterial, MstBridgeGetDefaultMaterial());

    if (on)
    {

		// High quality friction on

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

		// High quality friction off
        // This uses a cheaper way of doing friction. Slip means contacts only 
        // have dynamic friction, and the adhesive force can make things
        // behave funny, but go lots faster.

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
*/
/*
void MEAPI ToggleGravity(MeBool on)
{
    wakeAll(rc, 0);
    if(on)
        MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);
    else
        MdtWorldSetGravity(world, 0, 0, 0);

}
*/
