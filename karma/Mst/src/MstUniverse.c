/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:41 $ - Revision: $Revision: 1.80.2.4 $

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

#define ME_NEW_MCD_API
#include <Mcd.h>
#include <Mst.h>
#include <MeMath.h>
#include <MeProfile.h>

/* *************************************************************** */
/* CONSTRUCTORS / DESTRUCTORS                                      */
/* *************************************************************** */

/**
 * Some default sizes. This is so that every time we change the parameter set
 * for an Mst universe, we don't have to change every demo which uses it.
 */

const MstUniverseSizes MstUniverseDefaultSizes = 
{
    100,      /*    dynamicBodiesMaxCount        */
    500,      /*    dynamicConstraintsMaxCount   */
    0,        /*    collisionUserGeometryTypesMaxCount */
    100,      /*    collisionModelsMaxCount      */
    500,      /*    collisionPairsMaxCount       */
    100,      /*    collisionGeometryInstancesMaxCount */
    10,       /*    materialsMaxCount            */
    1,        /*    lengthScale                  */
    1         /*    massScale                    */
};

/**
 * Allocate memory for an MstUniverse simulation container.
 * An MstUniverse is a useful container for a collision McdSpace farfield,
 * a dynamics MdtWorld, an MstMaterialTable, and containers used for moving
 * contact information between collision and dynamics.
 */
MstUniverseID MEAPI MstUniverseCreate(const MstUniverseSizes * const sizes)
{
    MstUniverse* u;

    u = (MstUniverse *)MeMemoryAPI.create(sizeof (MstUniverse));

    if(!u)
    {
#ifdef _MECHECK
        MeWarning(0, "MstUniverseCreate: Could not allocate memory "
            "for MstUniverse.");
#endif
        return u;
    }

    u->sizes = *sizes;

    /* Start Physics */
    u->world = MdtWorldCreate(sizes->dynamicBodiesMaxCount,
        sizes->dynamicConstraintsMaxCount, sizes->lengthScale, sizes->massScale);

    MdtWorldSetAutoDisable(u->world, 1);

    /* Start Collision */
    u->frame = McdInit(sizes->collisionUserGeometryTypesMaxCount, sizes->collisionModelsMaxCount, 
        sizes->collisionGeometryInstancesMaxCount, sizes->lengthScale);
    McdPrimitivesRegisterTypes(u->frame);
    McdPrimitivesRegisterInteractions(u->frame);
    McdAggregateRegisterType(u->frame);
    McdAggregateRegisterInteractions(u->frame);
    McdNullRegisterType(u->frame);

    /* Collision Farfield Space */
    u->space = McdSpaceAxisSortCreate(u->frame,McdAllAxes,
        sizes->collisionModelsMaxCount, 
        sizes->collisionPairsMaxCount);

    /* Collision Bridge */

    /* Set default max contacts per pair to 4 */
    /* McdRequestGetDefaultPtr()->contactMaxCount = 4; */

    /* Make material pair parameters table */
    u->bridge = MstBridgeCreate(u->frame, sizes->materialsMaxCount);
    MstBridgeSetModelPairBufferSize(u->bridge, u->sizes.collisionPairsMaxCount);

    MstSetWorldHandlers(u->world);

    return u;
}

/**
 * De-allocate memory and destroy an MstUniverse simulation container.
 * All Models/Bodies/Constraints created from this universe will be
 * invalid after this call.
 */
void MEAPI MstUniverseDestroy(const MstUniverseID u)
{
    /* End Physics */
    MdtWorldDestroy(u->world);

    /* End Collision */
    McdSpaceDestroy(u->space);
    McdFrameworkDestroyAllModelsAndGeometries(u->frame);
    McdTerm(u->frame);

    MstBridgeDestroy(u->bridge);

    MeMemoryAPI.destroy(u);
}

/* *************************************************************** */
/* ACTION                                                          */
/* *************************************************************** */

/**
 * Dynamics and Collision 'main loop', using MstUniverse container.
 * @see MdtStep
 */
void MEAPI MstUniverseStep(const MstUniverseID u, const MeReal stepSize)
{
    MeProfileStartSection("Collision", 0);
    McdSpaceUpdateAll(u->space);

    MstBridgeUpdateContacts(u->bridge, u->space, u->world);
    MeProfileEndSection("Collision");

    MeProfileStartSection("Dynamics", 0);
    MdtWorldStep(u->world,stepSize);
    
    MeProfileEndSection("Dynamics");
}

/* *************************************************************** */
/* MUTATORS                                                        */
/* *************************************************************** */


/* *************************************************************** */
/* ACCESSORS                                                       */
/* *************************************************************** */

/** Get the dynamics world (part of the MstUniverse). */
MdtWorldID MEAPI MstUniverseGetWorld(const MstUniverseID u)
{
    return u->world;
}

/** Get the collision space (part of the MstUniverse). */
McdSpaceID MEAPI MstUniverseGetSpace(const MstUniverseID u)
{
    return u->space;
}

/** Get the Mst collision<->dynamics bridge (part of the MstUniverse). */
MstBridgeID MEAPI MstUniverseGetBridge(const MstUniverseID u)
{
    return u->bridge;
}

/** Get the collision framework */
McdFrameworkID MEAPI MstUniverseGetFramework(const MstUniverseID u)
{
    return u->frame;
}


/* "...there is no spoon." */
