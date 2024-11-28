/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.38.2.4 $

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

/* Some notes:
   1) This assumes all constraint flags are zero when this function is called.
   2) All enabled bodies and no disabled bodies will end up in a partition.
*/

#include <stdio.h>
#ifndef PS2
#ifndef __MWERKS__
#include <memory.h>
#endif
#endif

#include "MdtUtils.h"
#include <MeProfile.h>
#include <MdtBody.h>
#include <MeMemory.h>
#include <MeMessage.h>
#include "MeMath.h"
#include "MdtAlignment.h"
#include "MdtConstraint.h"
#include "MdtContactGroup.h"


/* Finds next un-ADDED body in list from supplied body. */
MeDictNode *FindNextUnadded(MeDict *dict, MeDictNode * node)
{
    while (node)
    {
        MdtBody *b = (MdtBodyID)MeDictNodeGet(node);
        if (!(b->flags & MdtEntityAddedFlag))
        {
            MEASSERT(b->flags & MdtEntityEnabledFlag);
            return node;
        }
        node = MeDictNext(dict,node);
    }
    return 0;
}

/** 
 *  Construct empty MdtPartitionOutput structure of the required size
 *  from the supplied MeChunk temporary memory resource. 
 */
MdtPartitionOutput* MEAPI MdtPartOutCreateFromChunk(MeChunk* chunk,
                                                    int maxBodies,
                                                    int maxConstraints)
{
    MdtPartitionOutput* po;
    int maxPartitions = maxBodies;

    /* Calculate the total amount of memory we need for this partitionOutput. */
    int structSize = sizeof(MdtPartitionOutput);

    int bStartSize = maxPartitions * sizeof(int);
    int bSizeSize = maxPartitions * sizeof(int);
    int bSize = maxBodies * sizeof(MdtBody*);

    int cStartSize = maxPartitions * sizeof(int);
    int cSizeSize = maxPartitions * sizeof(int);
    int cSize = maxConstraints * sizeof(MdtBaseConstraint*);

    int pInfoSize = maxPartitions * sizeof(MdtPartitionInfo);    

    /*  Because 'bodies' can also act as input to Kea, we need to ensure its 
    alignment and allocate a bit more. */
    int totalSize = structSize +
                    bStartSize + 
                    bSizeSize + 
                    bSize + 
                    cStartSize + 
                    cSizeSize + 
                    pInfoSize + 
                    cSize;

    po = (MdtPartitionOutput*)MeChunkGetMem(chunk, totalSize);

    po->bodiesStart = (int*)((char*)po + structSize);
    po->bodiesSize = (int*)((char*)(po->bodiesStart) + bStartSize);
    po->bodies = (MdtBody**)((char*)(po->bodiesSize) + bSizeSize);

    po->constraintsStart = (int*)((char*)(po->bodies) + bSize);
    po->constraintsSize = (int*)((char*)(po->constraintsStart) + cStartSize);
    po->info = (MdtPartitionInfo*)((char*)(po->constraintsSize) + cSizeSize);    
    po->constraints = (MdtBaseConstraint**)((char*)(po->info) + pInfoSize);
    
#ifdef _MECHECK
    {
        char* endPointer = (char*)(po->constraints) + cSize;
        if(endPointer > (char*)po + totalSize)
            MeFatalError(1, "MdtPartOutCreateFromChunk: Error.");
    }
#endif

    po->maxPartitions = maxPartitions;
    po->maxBodies = maxBodies;
    po->maxConstraints = maxConstraints;

    po->nPartitions = 0;
    po->totalBodies = 0;
    po->totalConstraints = 0;

    return po;
}

void MEAPI MdtPartInfoReset(MdtPartitionInfo* info)
{
    info->contactCount = 0;
    info->jointCount = 0;
    info->rowCount = 0;
}

void MEAPI MdtPartInfoAddConstraint(MdtPartitionInfo* info, MdtBaseConstraint* c)
{
    int rows = 0, padding = 0;
    MdtContactGroupID group = MdtConstraintDCastContactGroup(c);

    info->rowCount += MdtConstraintGetRowCount(c);

    if(group)
        info->contactCount += group->count;
    else
        info->jointCount += 1;
}

/*
 * Divide the MdtWorld into seperate partitions that can be solved
 * independently.
 */
void MEAPI MdtUpdatePartitions(MeDict *enabledBodyDict,
                MdtPartitionOutput* po, const MdtPartitionEndCB pcb,
                void* pcbdata)
{
    /* Body to start next island exploration from. */
    MdtBody *rootBody;
    MeDictNode *rootNode;
    int i;

    MdtBaseConstraint *cp;
    MdtBody *bp;

    /*
       Arrays to keep track of all bodies and constraints added. This is
       necessary because the bodies and constraints may be removed during
       the end-of-partition callback.
    */
    MdtBody** addedBodies =
        (MdtBody**)MeMemoryALLOCA(po->maxBodies *
        sizeof(MdtBody*));
    int numAddedBodies = 0;

    MdtBaseConstraint** addedConstraints =
        (MdtBaseConstraint**)MeMemoryALLOCA(po->maxConstraints *
        sizeof(MdtBaseConstraint*));
    int numAddedConstraints = 0;

    po->nPartitions = 0;
    po->totalBodies = 0;
    po->totalConstraints = 0;

    po->overallInfo.contactCount = 0;
    po->overallInfo.jointCount = 0;
    po->overallInfo.rowCount = 0;

#ifdef PROFILE_MDT
    MeProfileStartSection("Mdt Partition", 1);
#endif

    rootNode = MeDictFirst(enabledBodyDict);
    /* LOOP OVER EACH PARTITION */
    while(rootNode != 0)
    {
        int exploreNext, addNext;

        rootBody = (MdtBodyID)MeDictNodeGet(rootNode);
        
        /* Reset number of bodies and constraints in the current partition. */
        po->bodiesSize[po->nPartitions] = 0;
        po->constraintsSize[po->nPartitions] = 0;

        MdtPartInfoReset(&po->info[po->nPartitions]);

        po->bodiesStart[po->nPartitions] = po->totalBodies;
        po->constraintsStart[po->nPartitions] = po->totalConstraints;

        /* Initialise to start of partition. */
        addNext = exploreNext = po->bodiesStart[po->nPartitions];

        /* Add nextRoot to partitionBodies array. */
        po->bodies[addNext++] = rootBody;
        addedBodies[numAddedBodies++] = rootBody;

        rootBody->arrayIdPartition = po->bodiesSize[po->nPartitions];
        rootBody->arrayIdWorld = po->bodiesStart[po->nPartitions] + po->bodiesSize[po->nPartitions];
        rootBody->partitionIndex = po->nPartitions;
        rootBody->flags |= MdtEntityAddedFlag;

        (po->totalBodies)++;
        (po->bodiesSize[po->nPartitions])++;


        /* LOOP OVER EACH PARTITION BODY */
        while (exploreNext < po->bodiesStart[po->nPartitions] + po->bodiesSize[po->nPartitions])
        {
            MeDict *dict;
            MeDictNode *node;

            MdtBody *currentBody = po->bodies[exploreNext++];
            dict = &currentBody->constraintDict;
            /* Loop over all constraints affecting this body. */

            /* LOOP OVER CURRENT BODY CONSTRAINTS */
            for(node = MeDictFirst(dict); node != 0; node = MeDictNext(dict,node))
            {
                cp = (MdtBaseConstraint *)MeDictNodeGet(node);

                /* Follow this constraint if it has not been ADDED yet. */
                if (!(cp->head.flags & MdtEntityAddedFlag))
                {
#ifdef _MECHECK
                    if (po->totalConstraints >= po->maxConstraints)
                        MeFatalError(1, "MdtUpdatePartitions: "
                            "Adding too many constraints.");
#endif

                    /* Add constraint pointer to partition constraints array
                       and mark as added. */
                    po->constraints[po->totalConstraints] = cp;
                    addedConstraints[numAddedConstraints++] = cp;
    
                    (po->totalConstraints)++;
                    (po->constraintsSize[po->nPartitions])++;

                    /* Keep track of other information about this partition. */
                    MdtPartInfoAddConstraint(&po->info[po->nPartitions], cp);

                    cp->head.flags |= MdtEntityAddedFlag;

                    /* Find bodies affected by this constraint */
                    for (i = 0; i < MdtKeaMAXBODYCONSTRAINT; i++)
                    {
                        bp = cp->head.mdtbody[i];

#ifdef _MECHECK
                        if ((bp) && (bp->keaBody.tag != MdtKeaBODYVER1))
                        {
                            MeFatalError(0,"MdtUpdatePartitions: "
                                "Constraint found with invalid or "
                                "destroyed MdtBody.");
                        }
#endif

                        /* If a body exists AND we are not coming from it.. */
                        if ((bp) && (bp != currentBody) &&
                            /* ..AND it has not yet been added. */
                            !(bp->flags & MdtEntityAddedFlag))
                        {
#ifdef _MECHECK
                            if (po->totalBodies >= po->maxBodies)
                                MeFatalError(1, "MdtUpdatePartitions: "
                                "Adding too many constraints.");
#endif

                            /* Add body to partitionBodies array! */
                            po->bodies[addNext++] = bp;
                            addedBodies[numAddedBodies++] = bp;

                            bp->arrayIdPartition = po->bodiesSize[po->nPartitions];
                            bp->arrayIdWorld = po->bodiesStart[po->nPartitions] + po->bodiesSize[po->nPartitions];
                            bp->partitionIndex = po->nPartitions;
                            bp->flags |= MdtEntityAddedFlag;

                            (po->totalBodies)++;
                            (po->bodiesSize[po->nPartitions])++;
                        }
                    }
                }
            }

            /* If this body should be enabled, do so here. */
            if (!MdtBodyIsEnabled(currentBody))
            {
                MdtBodyEnable(currentBody);
                currentBody->flags |= MdtEntityEnabledByPartitioner;
            }
            else
                currentBody->flags &= ~MdtEntityEnabledByPartitioner;
        }

        /* FINISHED A PARTITION */

        /* Number of rows in a partition always rounded to nearest 4. */
        po->info[po->nPartitions].rowCount = MeMathCEIL4(po->info[po->nPartitions].rowCount);

        /* Keep track of overall stats (for all partitions). */
        po->overallInfo.rowCount += po->info[po->nPartitions].rowCount;        
        po->overallInfo.jointCount += po->info[po->nPartitions].jointCount;        
        po->overallInfo.contactCount += po->info[po->nPartitions].contactCount;

        /* Finally increase number of partitions! */
#ifdef _MECHECK
        if (po->nPartitions >= po->maxPartitions)
            MeFatalError(1, "MdtUpdatePartitions: "
            "Adding too many partitions.");
#endif

        (po->nPartitions)++;

        /* do this here in case the callback removes the node from the enabled list! */

        rootNode = FindNextUnadded(enabledBodyDict,rootNode);

        /* If we have a end-of-partition callback - call it. */
        if(pcb) pcb(po, pcbdata);
    }

    /* FINISHED ALL PARTITIONS */

    /* Mark constraints that were added as 'un-ADDED' again */
    for(i=0; i < numAddedConstraints; i++)
        addedConstraints[i]->head.flags &= ~MdtEntityAddedFlag;

    /* Mark bodies that were added as as 'un-ADDED' again */
    for(i=0; i < numAddedBodies; i++)
        addedBodies[i]->flags &= ~MdtEntityAddedFlag;

    MeMemoryFREEA(addedConstraints);
    MeMemoryFREEA(addedBodies);
}





/* ..playground tactics, no rabbit-in-a-hat tricks.. */
