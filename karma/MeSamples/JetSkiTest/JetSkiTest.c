/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:12 $ - Revision: $Revision: 1.11.6.1 $

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

#include <MeViewer.h>
#include <Mdt.h>

#include "JetSki.h"


RRender *rc;
RGraphic *groundG;

MdtWorldID world;
JetSki* jetski;

MdtHingeID worldHinge;
MdtPrismaticID worldPrismatic;

MeReal gravity[3] = { 0, -7, 0 };
MeReal step = (MeReal)(0.03);

MeReal jetSkiAngle = 0;
MeReal steeringAngle = 0;

int clickStart[2] = {0, 0};

//#define USE_HINGE

int numModes = 6;
int currentMode = 0;

MeBool useHinge[6] =
{
    0,
    0,
    0,
    1,
    1,
    1,
};

MeVector3 axis[6] =
{
    {0, 0, 1},
    {1, 0, 0},
    {0, 1, 0},
    {0, 1, 0},
    {1, 0, 0},
    {0, 0, 1},
};

char *help[4] =
{
    "$ACTION3 - sit/stand",
    "$ACTION2 - cycle jetski movement mode",
    "$MOUSE Up/Down - move jetski",
    "$MOUSE Left/Right - steer",
};

void MEAPI MouseCB(RRender *rc, int x, int y,
               int modifiers, RMouseButtonWhich which,
               RMouseButtonEvent event, void *userdata)
{
    if(event == kRNewlyPressed)
    {
        clickStart[0] = x;
        clickStart[1] = y;
    }
    else if(event == kRNewlyReleased)
    {
        jetSkiAngle = 0;
        steeringAngle = 0;
    }
    else
    {
        MeReal dx = (MeReal)(x - clickStart[0])/640;
        MeReal dy = (MeReal)(y - clickStart[1])/448;

        if(dx > 1) dx = 1;
        if(dx < -1) dx = -1;
        if(dy > 1) dy = 1;
        if(dy < -1) dy = -1;


        jetSkiAngle = dy * 1.5f;
        steeringAngle = dx * 1.5f;
    }
}

void MEAPI NextPose(RRender * rc, void *userdata)
{
    JetSkiNextPose(jetski);
}

void MEAPI NextMode(RRender * rc, void *userdata)
{
    currentMode = (currentMode + 1)%numModes;
    printf("Control Mode: %d.\n", currentMode);

    MdtHingeSetAxis(worldHinge, axis[currentMode][0], axis[currentMode][1], axis[currentMode][2]);
    MdtPrismaticSetAxis(worldPrismatic, axis[currentMode][0], axis[currentMode][1], axis[currentMode][2]);

    if(useHinge[currentMode])
    {
        if(!MdtHingeIsEnabled(worldHinge))
            MdtHingeEnable(worldHinge);
        if(MdtPrismaticIsEnabled(worldPrismatic))
            MdtPrismaticDisable(worldPrismatic);
    }
    else
    {
        if(!MdtPrismaticIsEnabled(worldPrismatic))
            MdtPrismaticEnable(worldPrismatic);
        if(MdtHingeIsEnabled(worldHinge))
            MdtHingeDisable(worldHinge);
    }
}

void MEAPI tick(RRender * rc,void *userdata)
{
    MdtHingeID steeringHinge = MdtConstraintDCastHinge(jetski->joint[kJetSkiSteering]);

    if(useHinge[currentMode])
        MdtLimitController(MdtHingeGetLimit(worldHinge), jetSkiAngle, 1, 1.5f, 1000);
    else
        MdtLimitController(MdtPrismaticGetLimit(worldPrismatic), jetSkiAngle, 1, 4.5f, 1000);


    MdtLimitController(MdtHingeGetLimit(steeringHinge), steeringAngle, 0.5f, 4.5f, 100);

    MeProfileStartSection("Dynamics", 0);
    MdtWorldStep(world, step);
    MeProfileEndSection("Dynamics");

    JetSkiUpdateMatrices(jetski);
}

void MEAPI_CDECL cleanup(void)
{
    JetSkiDestroy(jetski);

    MdtWorldDestroy(world);
    RRenderContextDestroy(rc);
}


int MEAPI_CDECL main(int argc, const char * argv[])
{
    float color[4];
    MeCommandLineOptions* options;

    options = MeCommandLineOptionsCreate(argc, argv);
    rc = RRenderContextCreate(options, 0, !MEFALSE);
    MeCommandLineOptionsDestroy(options);

    RCameraZoom(rc, -10);
    RCameraRotateElevation(rc, 0.5);

    world = MdtWorldCreate(100, 100);
    MdtWorldSetGravity(world, gravity[0], gravity[1], gravity[2]);

    jetski = JetSkiCreate(world, rc);

    currentMode = 0;

    worldHinge = MdtHingeCreate(world);
    MdtHingeSetBodies(worldHinge, jetski->body[kJetSkiHull], 0);
    MdtHingeSetAxis(worldHinge, axis[currentMode][0], axis[currentMode][1], axis[currentMode][2]);

    worldPrismatic = MdtPrismaticCreate(world);
    MdtPrismaticSetBodies(worldPrismatic, jetski->body[kJetSkiHull], 0);
    MdtPrismaticSetAxis(worldPrismatic, axis[currentMode][0], axis[currentMode][1], axis[currentMode][2]);

    if(useHinge[currentMode])
    {
        if(!MdtHingeIsEnabled(worldHinge))
            MdtHingeEnable(worldHinge);
        if(MdtPrismaticIsEnabled(worldPrismatic))
            MdtPrismaticDisable(worldPrismatic);
    }
    else
    {
        if(!MdtPrismaticIsEnabled(worldPrismatic))
            MdtPrismaticEnable(worldPrismatic);
        if(MdtHingeIsEnabled(worldHinge))
            MdtHingeDisable(worldHinge);
    }

    RRenderSetWindowTitle(rc, "Advanced JetSki Sim");
    RRenderCreateUserHelp(rc,help,4);
    RRenderToggleUserHelp(rc);

    RPerformanceBarCreate(rc);

    RRenderSetMouseCallBack(rc, MouseCB, 0);
    RRenderSetActionNCallBack(rc, 3, NextPose, 0);
    RRenderSetActionNCallBack(rc, 2, NextMode, 0);

    color[0] = 0.1f; color[1] = 0.2f; color[2] = 0.4f; color[3] = 1;
    groundG = RGraphicGroundPlaneCreate(rc, 24, 2, color, -1);

#ifndef PS2
    atexit(cleanup);
#endif

    RRun(rc, tick,0);

    return 0;
}
