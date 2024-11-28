/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:41 $ - Revision: $Revision: 1.30.2.3 $

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

/** @file
 * Mst Bridge.
 * Provides the data required to transfer contact geometry information
 * from the MathEngine Collition Toolkit to the MathEngine Dynamics Toolkit.
 * This consists of a 'material table', used to set the dynamics properties
 * of contacts (friction, restitution etc.), and buffers used during contact
 * processing.
 */

#include "McdProfile.h"

#include <Mst.h>
#include <MeMath.h>
#include <McdModelPairContainer.h>

/**
 * Create an collision<->dynamics MstBridge.
 * This is used during MdtStep to pass contact geometry information from
 * the MathEngine Collision Toolkit to the MathEngine Dynamics Toolkit.
 *
 * @param maxMaterials      Size of the material-pair properties table.
 */
MstBridgeID MEAPI MstBridgeCreate(McdFrameworkID frame, const unsigned int maxMaterials)
{
    unsigned int nElements, i;
    MstMaterialPair* p;
    MstBridgeID b = (MstBridgeID)MeMemoryAPI.create(sizeof(MstBridge));

    if(!b)
    {
#ifdef _MECHECK
        MeWarning(0, "MstBridgeCreate: Could not allocate memory for "
            "bridge.");
#endif
        return b;
    }

    /* Initialise material pool */
    b->maxMaterials = maxMaterials;
    nElements = MeUpperDiagonalSize(b->maxMaterials);
    b->materialPairArray = (MstMaterialPair *)
        MeMemoryAPI.create(nElements * sizeof (MstMaterialPair));

    /* Set all material pairs to the default contact params. */
    p = b->materialPairArray;
    for(i=0; i<nElements; i++)
    {
        MdtContactParamsReset(&(p->cp));

        /* Set contact callback to null. */
        p->contactCB = NULL;
        p->intersectCB = NULL;
        p->pairCB = NULL;
        p++;
    }

    /* Material '0' is used as default */
    b->usedMaterials = 1;

    /* Allocate ModelPair buffer. */
    b->pairs = McdModelPairContainerCreate(50);
    b->context = McdBatchContextCreate(frame);

    /* Allocate McdContact buffer. */
    b->contactsMaxCount = 50;
    b->contacts = (McdContact*)
        MeMemoryAPI.create(b->contactsMaxCount * sizeof(McdContact));

    /* Set callbacks for body enable/disable. */
    return b;
}



/**
 * Destroy an MstBridge. It cannot be used after this.
 */
void MEAPI MstBridgeDestroy(const MstBridgeID b)
{
    McdModelPairContainerDestroy(b->pairs);
    McdBatchContextDestroy(b->context);
    MeMemoryAPI.destroy(b->contacts);
    MeMemoryAPI.destroy(b->materialPairArray);
    MeMemoryAPI.destroy(b);
}

/* *************************************************************** */
/* ACTION                                                          */
/* *************************************************************** */

/**
 * Handle 'Hello' and 'Goodbye' farfield pairs.
 * This removes any contacts and data for 'Goodbye' pairs, and initialises
 * 'Hello' pairs. Use after McdSpaceRemoveModel before detroying the model.
 * Calls McdSpaceUpdate, McdSpaceGetPairs and MstHandleTransitions.
 **/
void MEAPI MstBridgeUpdateTransitions(const MstBridgeID b,
                                      const McdSpaceID s,
                                      const MdtWorldID w)
{
    MeBool pairOverflow;
    McdSpacePairIterator spaceIter;

    McdSpaceEndChanges(s);

    /* Initialise iterator for this space. */
    McdSpacePairIteratorBegin(s, &spaceIter);

    /* Keep getting pairs from farfield until we're done (ie. no overflow). */
    do
    {
        McdModelPairContainerReset(b->pairs);
        pairOverflow = McdSpaceGetPairs(s, &spaceIter, b->pairs);

        MstHandleTransitions(b->pairs, s, w, b);
    }
    while(pairOverflow);
    McdSpaceBeginChanges(s);
}

/**
 * Take all the McdModelPairs from the McdSpace, and handle them.
 * Call this after McdSpaceUpdate to update the contact geometry information
 * in the MdtWorld.
 */
void MEAPI MstBridgeUpdateContacts(const MstBridgeID b,
                                   const McdSpaceID s,
                                   const MdtWorldID w)
{
    MeBool pairOverflow;
    McdSpacePairIterator spaceIter;

    McdProfileStart("MstBridgeUpdateContacts");

    /* end state-modification mode, ready for state queries */
    McdSpaceEndChanges(s);

    /* Initialise iterator for this space. */
    McdSpacePairIteratorBegin(s, &spaceIter);

    /* Keep getting pairs from farfield until we're done (ie. no overflow). */
    do
    {
        McdProfileStart("MstBUC>McdSpaceGetPairs");
        McdModelPairContainerReset(b->pairs);
        pairOverflow = McdSpaceGetPairs(s, &spaceIter, b->pairs);
        McdProfileEnd  ("MstBUC>McdSpaceGetPairs");

        McdProfileStart("MstBUC>MstHandleTransitions");
        /* Initialises 'Hello' pairs and clears 'Goodbye' pairs. */
        MstHandleTransitions(b->pairs, s, w, b);
        McdProfileEnd  ("MstBUC>MstHandleTransitions");

        /* Generate collision information and pass to dynamics. */
        McdProfileStart("MstBUC>MstHandleCollisions");
        MstHandleCollisions(b->pairs, s, w, b);
        McdProfileEnd  ("MstBUC>MstHandleCollisions");
    }
    while (pairOverflow);

    /* end of state-query mode, ready for state modifications. */
    McdSpaceBeginChanges(s);

    McdProfileEnd("MstBridgeUpdateContacts");
}

/* *************************************************************** */
/* ACCESSORS                                                       */
/* *************************************************************** */


/**
 * Get the current dynamics contact parameters for contacts between the
 * given pair of materials. You can then use the MdtContactParams interface
 * to modify friction, restitution etc. for this pair of materials.
 */
MdtContactParamsID MEAPI MstBridgeGetContactParams(const MstBridgeID b,
                              const MstMaterialID m1, const MstMaterialID m2)
{
    int index;

    if (m1 > b->maxMaterials || m2 > b->maxMaterials) {
#ifdef _MECHECK
        MeWarning(0,"MstMaterialID greater than max number of materials.");
#endif
        return 0;
    }

    index = MeSymUpperDiagonalIndex(m1, m2, b->maxMaterials);
    return (MdtContactParamsID)&(b->materialPairArray[index].cp);
}

/**
 * Get the current, optional per-pair callback executed for each colliding
 * pair of models with the given materials.
 */
MstPerPairCBPtr MEAPI MstBridgeGetPerPairCB(const MstBridgeID b,
                       const MstMaterialID m1, const MstMaterialID m2)
{
    int index;
    if (m1 > b->maxMaterials || m2 > b->maxMaterials)
    {
#ifdef _MECHECK
        MeWarning(0,"MstMaterialID greater than max number of materials.");
#endif
        return 0;
    }

    index = MeSymUpperDiagonalIndex(m1, m2, b->maxMaterials);
    return b->materialPairArray[index].pairCB;
}

/**
 * Get the current, optional per-pair callback executed for each contact
 * between models with the given materials.
 */
MstPerContactCBPtr MEAPI MstBridgeGetPerContactCB(const MstBridgeID b,
                       const MstMaterialID m1, const MstMaterialID m2)
{
    int index;
    if (m1 > b->maxMaterials || m2 > b->maxMaterials)
    {
#ifdef _MECHECK
        MeWarning(0,"MstMaterialID greater than max number of materials.");
#endif
        return 0;
    }

    index = MeSymUpperDiagonalIndex(m1, m2, b->maxMaterials);
    return b->materialPairArray[index].contactCB;
}

/**
 * Get the current, optional per-pair callback executed for each intersect.
 */
MstIntersectCBPtr MEAPI MstBridgeGetIntersectCB(const MstBridgeID b,
                       const MstMaterialID m1, const MstMaterialID m2)
{
    int index;
    if (m1 > b->maxMaterials || m2 > b->maxMaterials)
    {
#ifdef _MECHECK
        MeWarning(0,"MstMaterialID greater than max number of materials.");
#endif
        return 0;
    }

    index = MeSymUpperDiagonalIndex(m1, m2, b->maxMaterials);
    return b->materialPairArray[index].intersectCB;
}

/**
 * Get a new, unused material from the material table to assign to a model.
 */
MstMaterialID MEAPI MstBridgeGetNewMaterial(const MstBridgeID b)
{
    int newm;

#ifdef _MECHECK
    /* if we don't have any unused materials */
    if(b->usedMaterials >= b->maxMaterials)
    {
        MeWarning(0, "MstBridgeGetNewMaterial: No remaining unused "
            "materials!");
    }
#endif

    newm = b->usedMaterials;
    b->usedMaterials++;
    return newm;
}

/* *************************************************************** */
/* MUTATORS                                                        */
/* *************************************************************** */

/**
 * Set the optional per-pair user callback for the given pair of materials.
 * This will be executed once for each pair of colliding models with the given
 * materials, with the McdIntersectResult and the set of dynamic contacts.
 * It allows, for example, further culling of dynamics contacts based on the
 * entire set of contact values. If the callback returns 0, the set of contacts will
 * be deleted.
 * @see MstPerPairCB
 */
void MEAPI MstBridgeSetPerPairCB(const MstBridgeID b, const MstMaterialID m1,
               const MstMaterialID m2, const MstPerPairCBPtr cb)
{
    int index;

    if (m1 > b->maxMaterials || m2 > b->maxMaterials) {
#ifdef _MECHECK
        MeWarning(0,"MstMaterialID greater than max number of materials.");
#endif
        return;
    }

    index = MeSymUpperDiagonalIndex(m1,m2,b->maxMaterials);
    b->materialPairArray[index].pairCB = cb;
}

/**
 * Set the optional per-contact user callback for the given pair of materials.
 * This will be executed once for each contact between models with the given
 * materials, with the McdIntersectResult and the Mcd and Mdt contacts. It allows
 * control of parameters in the dynamics contact based on data contained in the
 * collision contact. If the callback returns 0, the Mdt contact will be deleted.
 * @see MstPerContactCBPtr
 */
void MEAPI MstBridgeSetPerContactCB(const MstBridgeID b, const MstMaterialID m1,
               const MstMaterialID m2, const MstPerContactCBPtr cb)
{
    int index;

    if (m1 > b->maxMaterials || m2 > b->maxMaterials) {
#ifdef _MECHECK
        MeWarning(0,"MstMaterialID greater than max number of materials.");
#endif
        return;
    }

    index = MeSymUpperDiagonalIndex(m1,m2,b->maxMaterials);
    b->materialPairArray[index].contactCB = cb;
}

/**
 * Set the optional per-intersection user callback for the given pair of material.
 * This will be executed once for each pair of colliding models with the given
 * materials, with the McdIntersectResult and the set of collision contacts.
 * It allows control over McdContacts before they are converted to MdtContacts.
 * @see MstIntersectCBPtr
 */

void MEAPI MstBridgeSetIntersectCB(const MstBridgeID b, const MstMaterialID m1,
               const MstMaterialID m2, const MstIntersectCBPtr cb)
{
    int index;

    if (m1 > b->maxMaterials || m2 > b->maxMaterials) {
#ifdef _MECHECK
        MeWarning(0,"MstMaterialID greater than max number of materials.");
#endif
        return;
    }

    index = MeSymUpperDiagonalIndex(m1,m2,b->maxMaterials);
    b->materialPairArray[index].intersectCB = cb;
}

/**
 * Manually resize the ModelPair buffer used during Step.
 * This will allocate memory.
 */
void MEAPI MstBridgeSetModelPairBufferSize(const MstBridgeID b,
                                const unsigned int s)
{
    McdModelPairContainerDestroy(b->pairs);
    b->pairs = McdModelPairContainerCreate(s);
}

/**
 * Manually resize contact buffer used during Step.
 * This will allocate memory.
 */
void MEAPI MstBridgeSetContactBufferSize(const MstBridgeID b,
                                const unsigned int s)
{
    MeMemoryAPI.destroy(b->contacts);
    b->contactsMaxCount = s;
    b->contacts = (McdContact*) MeMemoryAPI.create(b->contactsMaxCount * sizeof(McdContact));
}
