/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/25 09:24:01 $ - Revision: $Revision: 1.3.2.2 $

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

/* MathEngine headers   */
#include <Mst.h>
#include <MeApp.h>
#include <MeViewer.h>

/* Application headers  */
#include "controls.h"
#include "vehicle.h"
#include "rider.h"
#include "terrain.h"
#include "utils.h"
#include "rider.h"

/* disable unused variable warnings */
#ifdef _MSC_VER
#pragma warning( disable : 4101 )
#endif

/* Render context */
RRender *rc;

char *help[] =
{
    "$ACTION2 resets the simulation\n"
    "Use the cursor keys to control the Quadbike"

};


MstUniverseID universe=NULL;
/* World for the Dynamics Toolkit simulation */
MdtWorldID world=NULL;
McdSpaceID space=NULL;
McdFrameworkID framework=NULL;
MstBridgeID bridge=NULL;
MeApp           *meApp=NULL;

MstMaterialID groundMat=0, wheelMat=0, chassisMat=0;

int numContacts = 0;

MeReal timeStep = (MeReal)(0.016);
MeVector3 gravity = {0, (MeReal)-20.0, 0};


void MEAPI reset(RRender *rc, void *userData)
{
    ResetVehicle(&quadBike);
    ResetRider(&rider);
}


void MEAPI tick(RRender *rc, void *userData)
{
    MeVector3 pos;

    MeProfileStartSection("Collision", 0);

    /* Add collision update here    */
    /* PASTE_03 */

    MeProfileEndSection("Collision");

#if 0
    MeAppDrawContacts(meApp);
#endif

    MeProfileStartSection("Dynamics", 0);
    
    /* Add dynamics update here */
    /* PASTE_04 */

    MeProfileEndSection("Dynamics");

    UpdateControls();

    if(quadBike.chassisBody)
    {
        MdtBodyGetPosition(quadBike.chassisBody, pos);
        RCameraSetLookAt(rc, pos);
    }

    if(pos[1] < -15)
    {
        reset(rc,0);
    }
}



int MEAPI_CDECL InitialiseRenderer(int argc, const char *(argv[]))
{
    MeCommandLineOptions* options;

    /* Initialize mini-renderer */
    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);
    if (!rc)
      return 0;

    /*
    On screen help text.
    */
    RRenderSetWindowTitle(rc,"Evaluation Application");
    RRenderCreateUserHelp(rc, help, 1);
    RRenderToggleUserHelp(rc);

    RPerformanceBarCreate(rc);

    RRenderSkydomeCreate(rc, "skydome", 2, 1);

    /*
      Initialise View & Camera
    */

    RCameraSetView(rc, 10, 0.1f, 1.4f);
    RCameraSetLookAt(rc, startPos);

    return 1;
}

int MEAPI_CDECL InitialiseMEWorld(void)
{
    MstUniverseSizes sizes;

    /* Add world initialisation here    */
    /* PASTE_01 */

    /* Disable auto-disable. This is only done for clarity and would normally be enabled    */
    if(world) MdtWorldSetAutoDisable(world, 0);

#if 0
    meApp     = MeAppCreateFromUniverse(universe, rc);
    MeAppDrawContactsInit(meApp, green,10);
#endif

    return 1;
}

void MEAPI_CDECL InitialiseMaterials(void)
{
    MdtBclContactParams *params;

    /* Add Material initialisation here */
    /* PASTE_02 */

    /* Set contact callback here    */
    /* PASTE_14 */
}

void MEAPI_CDECL InitialiseControls(void)
{
  /*
  Keyboard callbacks.
  */

    RRenderSetActionNCallBack(rc, 2, reset, 0);
    
    RRenderSetUpCallBack(rc, Accelerate, 0);
    RRenderSetDownCallBack(rc, Brake, 0);
    RRenderSetLeftCallBack(rc, Turn, (void *)-1);
    RRenderSetRightCallBack(rc, Turn, (void *)1);
}

/*
Cleanup and exit
*/
void MEAPI_CDECL cleanup(void)
{
    if(framework) McdFrameworkDestroyAllModelsAndGeometries(framework);
    if(universe) MstUniverseDestroy(universe);
    if(rc) RRenderContextDestroy(rc);
}

/*
Main Routine
*/
int MEAPI_CDECL main(int argc, const char *(argv[]))
{
    /*
        Initialise Simple Renderer
    */
    if (!InitialiseRenderer(argc, argv)) return 1;

    /*
        Initialise MathEngine World
    */
    if (!InitialiseMEWorld()) return 1;

    /*
        Initialise Materials
    */
    InitialiseMaterials();

    /*
        Initialise Controls
    */
    InitialiseControls();   

    /*
        Initialise Terrain
    */
    InitialiseTerrain();

    /*
        Initialise Bike
    */
    InitialiseVehicle(&quadBike);

    /*
        initialise Rider
    */
    InitialiseRider(&rider);

    /*
        Create Environment objects
    */

    /*
        Reset Everything before starting    
    */
#if 0
    reset(rc, 0);
#endif

    atexit(cleanup);
    /*
        Run the Simulation.
    */

    /*
        Here the tick() function is registered as being the function that the
        rendering loop will call continuously until the simulation is exited.

        Pseudocode: while no exit-request { Handle user input call Tick() to
        evolve the simulation and update graphic transforms Draw graphics }
    */

    RRun(rc, tick,0);

    return 0;
}
