/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:12 $ - Revision: $Revision: 1.1.6.1 $

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

/* Global declarations */
#define COLOR1 {0.0 , 0.73, 0.73, 1}
#define COLOR2 {0.0 , 0.4 , 1.0 , 1}
#define COLOR3 {1.0 , 0.0 , 0.5 , 1}
#define COLOR4 {1.0 , 0.6 , 0.0 , 1}
#define COLOR5 {1.0,  0.4 , 0.0 , 1}
#define COLOR6 {0.6 , 0.4 , 1.0 , 1}


char *help[6] =
{
    "$ACTION1 - toggle options menu",
    "$ACTION2 - reset",
    "$ACTION3 - shoot",
    "$ACTION4 - disable all blocks",
    "$ACTION5 - enable all blocks",
    "$MOUSE - mouse force"
};




/* World for the Dynamics Toolkit simulation */
MstUniverseID u;

/* Physics representations */
McdGeometryID gyroGeom;
McdModelID gyroM;
McdGeometryID spindleGeom;
McdModelID spindleM;

MdtHingeID hinge;
MdtBSJointID BandS;

/* Graphical representations */
RGraphic *spindleG;
RGraphic *gyroG;
RGraphic *floorG;


/* Render context */
RRender *rc;
RMenu* menu;

MeApp* meapp;


MeReal step = (MeReal)(0.03);


MeMatrix4 groundRenderTM =
{
    {1,     0,  0, 0},
    {0,     0, -1, 0},
    {0,     1,  0, 0},
    {0, 0,  0, 1}
};




/*
  Tick() is a callback function called from the renderer's main loop
  to evolve the world by 'step' seconds
*/
void MEAPI tick(RRender * rc,void *userdata)
{
    MeAppStep(meapp);
    MstUniverseStep(u, step);
}

/* Reset boxes and balls to initial positions */
void MEAPI reset(RRender* rc, void* userData)
{
	MdtBodySetTransform(McdModelGetBody(spindleM),groundRenderTM);
	MdtBodySetLinearVelocity(McdModelGetBody(spindleM),0,0,0);
	MdtBodySetAngularVelocity(McdModelGetBody(spindleM),0,0,0);

	MdtBodySetTransform(McdModelGetBody(gyroM),groundRenderTM);
	MdtBodySetLinearVelocity(McdModelGetBody(gyroM),0,0,0);
	MdtBodySetAngularVelocity(McdModelGetBody(gyroM),0,10,0);
}

void MEAPI_CDECL cleanup(void)
{

    puts("Cleaning up");
    MeAppDestroy(meapp);

    MstModelAndBodyDestroy(gyroM);
    McdGeometryDestroy(gyroGeom);


    MstUniverseDestroy(u);
    RRenderContextDestroy(rc);
    RMenuDestroy(menu);
}

void MEAPI togglehelp(RRender* rc, void* userdata) {
    RRenderToggleUserHelp(rc);
}


/* Main Routine */
int MEAPI_CDECL main(int argc, const char * argv[])
{
    float color[4];
    MstUniverseSizes sizes=MstUniverseDefaultSizes;
    MeCommandLineOptions* options;

//	MeVector3 cen={0,0,0};
//	MeReal *rad=(MeReal*)MeMemoryAPI.create(sizeof(MeReal));


    u = MstUniverseCreate(&sizes);

	MdtWorldSetEpsilon(u->world,0.0001);

    MdtWorldSetGravity(u->world, 0, -10, 0);

    /* GROUND PLANE */
    gyroGeom = McdCylinderCreate(u->frame,5,1);
    gyroM = MstModelAndBodyCreate(u, gyroGeom, 0.1);
	MdtBodySetTransform(McdModelGetBody(gyroM),groundRenderTM);
	MdtBodySetAngularVelocityDamping(McdModelGetBody(gyroM),5);

    spindleGeom = McdCylinderCreate(u->frame,1,5);
	spindleM = MstModelAndBodyCreate(u,spindleGeom,.1);
	MdtBodySetTransform(McdModelGetBody(spindleM),groundRenderTM);

	hinge = MdtHingeCreate(u->world);
	MdtHingeSetBodies(hinge,McdModelGetBody(spindleM),McdModelGetBody(gyroM));
	MdtHingeSetAxis(hinge,0,1,0);
	MdtHingeEnable(hinge);


	BandS = MdtBSJointCreate(u->world);
	MdtBSJointSetBodies(BandS,McdModelGetBody(spindleM),0);
	MdtBSJointSetPosition(BandS,0,-2.5,0);
	MdtBSJointEnable(BandS);

	McdSpaceDisablePair(gyroM,spindleM);

	

    reset(rc, 0);
    /* ***************** END PHYSICS ***************** */

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
//    floorG = RGraphicGroundPlaneCreate(rc, 24, 2, color, -1);
//    RGraphicSetTexture(rc, floorG, "checkerboard");

    color[0] = color[1] = color[2] = color[3] = .75;
    gyroG = RGraphicCylinderCreate(rc, 5,1,color, McdModelGetTransformPtr(gyroM));
    color[0] = color[1] = color[2] = color[3] = .5;
    spindleG = RGraphicCylinderCreate(rc, 1,5,color, MdtBodyGetTransformPtr(McdModelGetBody(spindleM)));

    meapp = MeAppCreate(u->world, u->space, rc);

    RRenderSkydomeCreate(rc, "skydome", 2, 1);

    /* CONTROLS: */
    RRenderSetActionNCallBack(rc, 2, reset, 0);
//    RRenderSetActionNCallBack(rc, 3, shoot, 0);
//    RRenderSetActionNCallBack(rc, 4, killAll, 0);
//    RRenderSetActionNCallBack(rc, 5, wakeAll, 0);

    RRenderSetWindowTitle(rc, "Topple example");
    RRenderCreateUserHelp(rc,help,6);
    RRenderToggleUserHelp(rc);

    RRenderSetMouseCallBack(rc, MeAppMousePickCB, (void*)meapp);

    menu = RMenuCreate(rc, "Options Menu");
//    RMenuAddToggleEntry(menu, "Gravity", ToggleGravity, 1);

//    RMenuAddToggleEntry(menu, "High quality friction", ToggleHighQualityFriction, DEFAULT_FRICTION);
    
    RRenderSetDefaultMenu(rc, menu);

    /* Cleanup after simulation. */
    atexit(cleanup);

    /* Run the Simulation. */
    RRun(rc, tick,0);

    return 0;
}
