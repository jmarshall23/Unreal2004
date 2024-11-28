/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.26.2.4 $

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

#include <MeApp.h>

/**
 * Create demo support given a universe.
 */
MeApp *MEAPI MeAppCreateFromUniverse(const MstUniverseID u, RRender *rc)
{
    MeApp *app = MeAppCreate(MstUniverseGetWorld(u),MstUniverseGetSpace(u),rc);
    
    if (!app) return 0;
    app->universe = u;

    return app;
}

/**
 * Create demo support given a world and a space.
 */
MeApp *MEAPI MeAppCreate(const MdtWorldID world, const McdSpaceID space,RRender *rc)
{
    MeApp *app = (MeApp*)MeMemoryAPI.createZeroed(sizeof(MeApp));
    AcmeReal zeroVec[3] = {0,0,0};

    /* for mouse spring */
    float color[4];
    color[0] = color[1] = color[2] = color[3] = 1;


    if(!app)
    {
#ifdef _MECHECK
        MeWarning(0, "MeAppDestroy: Could not allocate memory for MeApp.");
#endif
        return 0;
    }

#if defined(WIN32) && defined(_MECHECK)
    do
    {
        short           fpuControl;
        __asm
        {       /* set FPU exceptions on */
            finit
            fstcw   fpuControl
            mov     ax,fpuControl
            and     ax,NOT (1 | 4 | 8)
            mov     fpuControl,ax
            fldcw   fpuControl
            fwait
        }
    }
    while(0);
#endif

    app->world = world;
    app->space = space;
    app->universe = 0;
    app->rc = rc;

    /* mouse picking stuff */
    app->mouseInfo.mouseSpringStiffness = 25;
    app->mouseInfo.mouseSpringDamping = (MeReal)2.8;
    app->mouseInfo.dragBody = 0;
    app->mouseInfo.line = RGraphicLineCreate(rc, zeroVec, zeroVec, color, 0);

    app->contactDrawInfo = 0;
    app->drawContacts = 0;
    return app;
}


/**
 * Destroy demo support structure.
 */
void MEAPI MeAppDestroy(MeApp  *const app)
{
    if (app->contactDrawInfo)
    {
        MeMemoryAPI.destroy(app->contactDrawInfo->contactG);
        MeMemoryAPI.destroy(app->contactDrawInfo);
    }
    MeMemoryAPI.destroy(app);
}

/**
 * Update demo support structures such as mouse picking and contact drawing.
 */
void MEAPI MeAppStep(MeApp *app)
{
    MeAppUpdateMouseSpring(app);

    if (app->drawContacts)
        MeAppDrawContacts(app);
}

/**
 * Returns the MstUniverse.
 */
MstUniverseID MEAPI MeAppGetMstUniverse(MeApp *app)
{
    return app->universe;
}



