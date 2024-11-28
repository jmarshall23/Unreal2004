/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.4.2.4 $

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

/** @file Level of detail support.
 */

#include <MeMath.h>
#include <MdtCheckMacros.h>
#include <MeProfile.h>
#include <MeMemory.h>
#include <MdtBody.h>
#include <MeVersion.h>
#include "Mdt.h"
#include "MdtUtils.h"
#include "MdtContactGroup.h"
#include "MeHeap.h"

typedef struct MdtLODPartitionData
{
    /* Current size (in rows) of this new partition. */
    int                     rowCount;  

    MdtBody**               bodyArray;
    int                     bodyCount;
    
    MdtBaseConstraint**     conArray;
    int                     conCount;
}MdtLODPartitionData;

/* ************************************************************************* */

/*  Calc importance of this constraint in the initial state (ie no rows).
    Joints are infinity. */
static MeReal ConstraintCalcImportance(MdtBaseConstraint* con, 
                                    MdtPartitionParams* params)
{
    MeReal imp = 0, pen = 0, normVel = 0;
    MdtContactID contact;
    MdtContactGroupID g = MdtConstraintDCastContactGroup(con);
    MdtBodyID b0 = con->head.mdtbody[0];
    MdtBodyID b1 = con->head.mdtbody[1];

    /*  All joints get inifinite importance to make sure 
        they are top of the queue. */
    if(!g)
        return MEINFINITY;

    /* Contact groups with no contacts are pointless! */
    if(g->count == 0)
        return -MEINFINITY;

    /* If contact to world, add bonus. */
    if(g->head.mdtbody[1] == 0)
        imp += params->lodParams.toWorldBonus;

    /* Assume zero rows, so add zero row bonus. */
    imp += params->lodParams.zeroRowBonus;

    /*  Add row count bias. */
    imp += g->head.maxRows * params->lodParams.rowCountBias;


    /*  Add bonus if this is constraint is to a body that was not just turned
        on automatically by the partitioner. */
    if(b0->enabledTime > 0 || !(b0->flags & MdtEntityEnabledByPartitioner))
        imp += params->lodParams.nonAutoBonus;
    else if(b1 && (b1->enabledTime > 0 || !(b1->flags & MdtEntityEnabledByPartitioner)))
        imp += params->lodParams.nonAutoBonus;

    /* Iterate over contacts finding average penetration and impact vel. */
    contact = g->first;
    while(contact)
    {
        MeVector3 vel;
        pen += contact->penetration;
        MdtContactGetRelativeVelocity(contact, vel);
        normVel += MeVector3Dot(vel, contact->normal);

        contact = contact->nextContact;
    }

    /* Add importance based on penetration and contact velocity. */
    imp += params->lodParams.penetrationBias * pen / (MeReal)g->count;
    imp += params->lodParams.normVelBias * normVel / (MeReal)g->count;

    return imp;
}

/*  Add body to partition currently indicated by its LODpartIndex.
    Does NOT update body->partitionIndex, body->arrayIdWorld or 
    body->arrayIdPartition.*/
static void AddBody(MdtLODPartitionData* pdataArray, 
                    int LODpartIx,
                    MdtBody* body)
{
    MdtLODPartitionData* pData = &(pdataArray[LODpartIx]);

    /* Assert body is not already in a partition. */
    MEASSERT(body->LODpartIndex == -1);

    pData->bodyArray = (MdtBody**)MeMemoryAPI.resize(
        pData->bodyArray, (pData->bodyCount+1) * sizeof(MdtBody*));

    body->LODpartIndex = LODpartIx;
    pData->bodyArray[pData->bodyCount] = body;
    (pData->bodyCount)++;
}

/* Add constraint to partition currently indicated by its LODpartIndex */
static void AddConstraint(MdtLODPartitionData* pdataArray, 
                          int LODpartIx,
                          MdtBaseConstraint* con)
{
    MdtLODPartitionData* pData = &(pdataArray[LODpartIx]);

    /* Assert constraint is not already in a partition. */
    MEASSERT(con->head.LODpartIndex == -1);

    pData->conArray = (MdtBaseConstraint**)MeMemoryAPI.resize(
        pData->conArray, (pData->conCount+1) * sizeof(MdtBaseConstraint*));

    con->head.LODpartIndex = LODpartIx;
    pData->conArray[pData->conCount] = con;
    (pData->conCount)++;
}

/* Return index of new, empty partition. */
static int NewPartition(MdtLODPartitionData* pData, int* pCount)
{
    pData[*pCount].rowCount = 0;
    
    pData[*pCount].bodyCount = 0;
    pData[*pCount].bodyArray = 0;
    
    pData[*pCount].conCount = 0;
    pData[*pCount].conArray = 0;

    (*pCount)++;

    return (*pCount) - 1;
}


/* Compare importance of constraints. Used by priority queue (heap) */
static int CompareImportance(const void* elem1, const void* elem2)
{
    MdtBaseConstraint* c1 = (MdtBaseConstraint*)elem1;
    MdtBaseConstraint* c2 = (MdtBaseConstraint*)elem2;

    return (c1->head.importance > c2->head.importance);
}


/* qsort compare function */
int ComparePenetration(const void* contact1, const void* contact2)
{
    MdtContactID c1 = *(MdtContactID*)contact1;
    MdtContactID c2 = *(MdtContactID*)contact2;

    if(c1->penetration < c2->penetration)
        return -1;
    else if(c1->penetration > c2->penetration)
        return 1;
    else
        return 0;
}

static void ResizeConstraint(MdtBaseConstraint* con, MdtPartitionParams* params)
{
    MdtContactGroupID group = MdtConstraintDCastContactGroup(con);

    if(group)
    {
        int i, initialNContacts;
        int nContacts, nFrictionContacts, nRows;
        MdtContactID contact;
        MdtContactID *byPenetration;
        MeBool *removed;

        /* Array for holding contacts sorted by penetration. */
        initialNContacts = group->count;

        byPenetration =  MeMemoryALLOCA(initialNContacts * sizeof(MdtContactID));
        removed = MeMemoryALLOCA(initialNContacts * sizeof(MeBool));

        MEASSERT(initialNContacts > 0);

        nFrictionContacts = 0;
        for(contact = group->first, i=0; contact; contact=contact->nextContact, i++)
        {
            if(contact->params.type != MdtContactTypeFrictionZero)
                nFrictionContacts++;

            byPenetration[i] = contact;
            removed[i] = 0;
        }
        
        /*  Quicksort contacts by penetration depth (smallest pen first). 
            Probably ineffecient for such small numbers!*/
        qsort((void*)byPenetration, initialNContacts, sizeof(MdtContactID), ComparePenetration);

        nContacts = initialNContacts;
        nRows = group->head.maxRows;

        MEASSERT(params->lodParams.frictionRatio >= 0 && params->lodParams.frictionRatio <= 1);

        /* Keep removing stuff until we get down to our row budget. */
        while(nRows > group->head.rowBudget)
        {
            if((MeReal)nFrictionContacts/nContacts < params->lodParams.frictionRatio ||
                nFrictionContacts == 0)
            {
                /*  Remove a frictionless contact. 
                    There must be at least 1 frictionless contact! */

                i=0;
                while(byPenetration[i]->params.type != MdtContactTypeFrictionZero || 
                    removed[i] == 1)
                {
                    i++;
                    MEASSERT(i < initialNContacts);
                }

                MdtContactGroupDestroyContact(group, byPenetration[i]);
                removed[i] = 1;
                nContacts--;
                nRows--;
            }
            else
            {
                /*  Remove friction from a contact. */

                i=0;
                while(byPenetration[i]->params.type == MdtContactTypeFrictionZero || 
                    removed[i] == 1)
                {
                    i++;
                    MEASSERT(i < initialNContacts);
                }

                if(byPenetration[i]->params.type == MdtContactTypeFriction1D)
                    nRows -= 1;
                else
                    nRows -= 2;

                byPenetration[i]->params.type = MdtContactTypeFrictionZero;
                nFrictionContacts--;
            }
        }

        MeMemoryFREEA(byPenetration);
        MeMemoryFREEA(removed);
    }
    else
    {
        /* There shouldn't be any joints with less than the full row budget. */
        MEASSERT(con->head.rowBudget == con->head.maxRows);
    }

}

/* ************************************************************************* */

/*  Move all bodies and constraints from p2Data into p1Data, 
    updating their LODpartIx. */
static void MergePartitions(MdtLODPartitionData* pdataArray, 
                            int p1Ix, int p2Ix)
{
    int i, newNBod, newNCon;
    MdtLODPartitionData *p1Data = &(pdataArray[p1Ix]);
    MdtLODPartitionData *p2Data = &(pdataArray[p2Ix]);

    newNBod = p1Data->bodyCount + p2Data->bodyCount;
    newNCon = p1Data->conCount + p2Data->conCount;

    /* Resize body and constraint array. */
    p1Data->bodyArray = (MdtBody**)MeMemoryAPI.resize(
        p1Data->bodyArray, 
        newNBod * sizeof(MdtBody*));

    p1Data->conArray = (MdtBaseConstraint**)MeMemoryAPI.resize(
        p1Data->conArray, 
        newNCon * sizeof(MdtBaseConstraint*));

    /* Copy across bodies and constraints (updating each's LODpartIx) */
    for(i=0; i<p2Data->bodyCount; i++)
    {
        p1Data->bodyArray[p1Data->bodyCount] = p2Data->bodyArray[i];
        p1Data->bodyArray[p1Data->bodyCount]->LODpartIndex = p1Ix;
        (p1Data->bodyCount)++;
    }
    MEASSERT(p1Data->bodyCount = newNBod);

    for(i=0; i<p2Data->conCount; i++)
    {
        p1Data->conArray[p1Data->conCount] = p2Data->conArray[i];
        p1Data->conArray[p1Data->conCount]->head.LODpartIndex = p1Ix;
        (p1Data->conCount)++;
    }
    MEASSERT(p1Data->conCount = newNCon);

    /* Add row counts together. */
    p1Data->rowCount = p1Data->rowCount + p2Data->rowCount;

    /* Then zero old p2Data */
    if(p2Data->bodyArray)
    {
        MeMemoryAPI.destroy(p2Data->bodyArray);
        p2Data->bodyArray = 0;
    }
    p2Data->bodyCount = 0;

    if(p2Data->conArray)
    {
        MeMemoryAPI.destroy(p2Data->conArray);
        p2Data->conArray = 0;
    }
    p2Data->conCount = 0;

    p2Data->rowCount = 0;
}

/* ************************************************************************* */

/*  If it can, increase row budget for this constraint and put it back
    into the priority queue. If it can't increase row budget, leaves it out
    of queue. Returns whether or not it could increase budget. */
static MeBool IncrementConstraintRowBudget(MdtBaseConstraint* con, 
                                           MdtLODPartitionData* pdataArray,
                                           int LODpartIx,
                                           MdtPartitionParams* params,
                                           MeHeap* q)
{
    MdtContactGroupID group;
    MdtLODPartitionData* pData = &(pdataArray[LODpartIx]);

    MEASSERT(LODpartIx != -1);
    MEASSERT(con->head.rowBudget < con->head.maxRows);

    group = MdtConstraintDCastContactGroup(con);

    if(!group) /* Its a joint, can only have none or all rows. */
    {
        MEASSERT(con->head.rowBudget == 0);
        con->head.rowBudget = con->head.maxRows;
        pData->rowCount += con->head.maxRows;

        /*  Don't put back in queue - 
            it has all the rows it needs, nothing more to do. */
    }
    /* Its a contact group, increase by one if room left in partition .*/
    else if(pData->rowCount < params->maxMatrixSize)
    {
        /* If this is the first row, remove zero row bonus. */
        if(con->head.rowBudget == 0)
            con->head.importance -= params->lodParams.zeroRowBonus;

        con->head.rowBudget += 1;
        con->head.importance -= params->lodParams.rowCountBias;
        pData->rowCount += 1;

        /* If this contact group isn't done yet, put back into queue. */
        if(con->head.rowBudget < con->head.maxRows)
            MeHeapPush(q, con);
    }
    else
    {
        /*  Can't add any more rows to this partition, 
            don't put back into queue. */
        return 0;
    }

    /*  We added some rows. 
        If constraint is not already in the partition, add it here. */
    if(con->head.LODpartIndex == -1)
    {
        AddConstraint(pdataArray, LODpartIx, con);
    }
    else
    {
        MEASSERT(con->head.LODpartIndex == LODpartIx);
    }

    return 1;
}


/* ************************************************************************* */

/** 
 *  Take a partition, and break it down into 1 or more partitions of no larger 
 *  than maxMatrixSize.
 */
void MEAPI MdtLODLastPartition(MdtPartitionOutput* po, 
                               MdtPartitionParams* params)
{
    int i, j;
    int p = po->nPartitions - 1; /* Partition we are working on - the last one in po. */
    MdtPartitionInfo* info = &(po->info[p]);

    /* Temporary storage. */
    int nBod = po->bodiesSize[p];
    MdtBody** blist = &(po->bodies[po->bodiesStart[p]]);

    int nCon = po->constraintsSize[p];
    MdtBaseConstraint** clist = &(po->constraints[po->constraintsStart[p]]);

    int partitionCount = 0; /* Partition count. */

    MdtLODPartitionData* pdataArray;
    MdtBaseConstraint* currentCon;

    /* Priority queue */
    MeHeap q;
    void **qMem = (void**)MeMemoryALLOCA((1 + nCon) * sizeof(void*));
    MeHeapInit(&q, qMem, 1 + nCon, CompareImportance);

    /*  Arrays for temporary data. 
        Max partitions == number of bodies. */
    pdataArray =  MeMemoryALLOCA(nBod * sizeof(MdtLODPartitionData));

    for(i=0; i<nBod; i++)
    {
        blist[i]->LODpartIndex = -1;
    }

    for(i=0; i<nCon; i++)
    {
        MdtContactGroupID group;
        clist[i]->head.LODpartIndex = -1;
        clist[i]->head.rowBudget = 0;

        /*  For the case of groups, update the maxRows to be pretty accurate
            (ie. taking into account friction type). 
            NOTE: For contact group, this is not currently used by pack partitions. */
        group = MdtConstraintDCastContactGroup(clist[i]);
        if(group)
            clist[i]->head.maxRows = MdtConstraintGetRowCount(clist[i]);

        /*  Set initial importance of contact group.
            This will change as its rowBudget increases. */
        clist[i]->head.importance = ConstraintCalcImportance(clist[i], params);

        /*  Add constraint to priority queue, if its a joint or group with at 
            least 1 row to add. (ie. don't even bother putting contact-less
            groups into the queue. */
        if(!group || group->head.maxRows > 0)
            MeHeapPush(&q, clist[i]);
    }

    /* Get constraint with max importance. */
    currentCon = (MdtBaseConstraint*)MeHeapPop(&q);

    /* While there are still constraints in the heap. */
    while(currentCon)
    {
        MdtBody* b0 = currentCon->head.mdtbody[0];
        MdtBody* b1 = currentCon->head.mdtbody[1];
        int b0PIx = b0->LODpartIndex;
                
        if(b1)
        {
            /* This is a constraint between 2 bodies. */
            int b1PIx = b1->LODpartIndex;
            
            if(b0PIx == -1)
            {
                if(b1PIx == -1)
                {
                    /*  Neither body is in a partition, */
                    /*  Create new partition and add constraint and both bodies. */
                    int newPIx = NewPartition(pdataArray, &partitionCount);
                    if(IncrementConstraintRowBudget(currentCon, pdataArray, newPIx, params, &q))
                    {
                        AddBody(pdataArray, newPIx, b0);
                        AddBody(pdataArray, newPIx, b1);
                    }
                }
                else
                {
                    /* b1 is in a partition already, but b0 is not. */
                    /* Try to add constraint and b0 to b1's partition. */
                    if(IncrementConstraintRowBudget(currentCon, pdataArray, b1PIx, params, &q))
                    {
                        AddBody(pdataArray, b1PIx, b0);
                    }
                }
            }
            else
            {
                if(b1PIx == -1)
                {
                    /* b0 is in a partition already, but b1 is not. */
                    /* Try to add constraint and b1 to b0's partition. */
                    if(IncrementConstraintRowBudget(currentCon, pdataArray, b0PIx, params, &q))
                    {
                        AddBody(pdataArray, b0PIx, b1);
                    }
                }
                else
                {
                    /* Both bodies are already in a partition. */
                    if(b0PIx == b1PIx)
                    {
                        /* Both are in the same partition */
                        /* Try to add constraint to same partition as b0. */
                        IncrementConstraintRowBudget(currentCon, pdataArray, b0PIx, params, &q);
                    }
                    else
                    {
                        /*  Bodies are in different partitions. */
                        /*  Merge partitions and add constraint if the result will not be too big,
							or constraint is a joint, in which case we will always merge partitions,
                            otherwise, do not put constraint back into queue. */
                        int newRowCount = pdataArray[b0PIx].rowCount + pdataArray[b1PIx].rowCount;

						MdtContactGroupID group = MdtConstraintDCastContactGroup(currentCon);

                        if(newRowCount + 1 < params->maxMatrixSize || !group)
                        {
                            MergePartitions(pdataArray, b0PIx, b1PIx);
                            IncrementConstraintRowBudget(currentCon, pdataArray, b0PIx, params, &q);
                        }
                    }
                }
            }
        }
        else
        {
            /* This is a constraint to the world. */
            if(b0PIx == -1)
            {
                /* b0 is not in a partition. */
                /* Create new partition and add constraint and b0. */
                int newPIx = NewPartition(pdataArray, &partitionCount);
                if(IncrementConstraintRowBudget(currentCon, pdataArray, newPIx, params, &q))
                {
                    AddBody(pdataArray, newPIx, b0);
                }
            }
            else
            {
                /* b0 is already in a partition. */
                /* Try to add constraint to same partition as b0. */
                IncrementConstraintRowBudget(currentCon, pdataArray, b0PIx, params, &q);
            }
        }

        currentCon = MeHeapPop(&q);
    } /* while(currentCon) */

    /* Rebuild partition output from this data. */
    
    /* First, rewind partition output to start of this partition. */
    po->nPartitions--;
    po->totalBodies -= po->bodiesSize[p];
    po->totalConstraints -= po->constraintsSize[p];
    
    po->overallInfo.rowCount -= po->info[p].rowCount;
    po->overallInfo.jointCount -= po->info[p].jointCount;
    po->overallInfo.contactCount -= po->info[p].contactCount;

    /* Then go add a 'proper' partition for each of the ones we just made. */
    for(i=0; i<partitionCount; i++)
    {
        /* If there is anything in this partition... */
        if(pdataArray[i].bodyCount > 0)
        {
            /* Reset number of bodies and constraints in the current partition. */
            po->bodiesSize[po->nPartitions] = 0;
            po->constraintsSize[po->nPartitions] = 0;
            
            MdtPartInfoReset(&po->info[po->nPartitions]);
            
            po->bodiesStart[po->nPartitions] = po->totalBodies;
            po->constraintsStart[po->nPartitions] = po->totalConstraints;

            /*  Copy bodies from this partition into main partition output. 
                We need to update partitionIndex, arrayIDWorld & arrayIDPartition. */
            for(j=0; j<pdataArray[i].bodyCount; j++)
            {
                MdtBody* bp = pdataArray[i].bodyArray[j];
                bp->arrayIdPartition = po->bodiesSize[po->nPartitions];
                bp->arrayIdWorld = po->bodiesStart[po->nPartitions] + po->bodiesSize[po->nPartitions];
                bp->partitionIndex = po->nPartitions;
                po->bodies[po->totalBodies] = bp;
                
                (po->totalBodies)++;
                (po->bodiesSize[po->nPartitions])++;
            }

            /* Copy constraints into partition output. */
            for(j=0; j<pdataArray[i].conCount; j++)
            {
                MdtBaseConstraint* cp = pdataArray[i].conArray[j];

                /* Fiddle the constraint so its within its rowBudget. */
                ResizeConstraint(cp, params);

                po->constraints[po->totalConstraints] = cp;
                
                (po->totalConstraints)++;
                (po->constraintsSize[po->nPartitions])++;

                MdtPartInfoAddConstraint(&po->info[po->nPartitions], cp);
            }
            
            /* Number of rows in a partition always rounded to nearest 4. */
            po->info[po->nPartitions].rowCount = MeMathCEIL4(po->info[po->nPartitions].rowCount);
            
            /* Keep track of overall stats (for all partitions). */
            po->overallInfo.rowCount += po->info[po->nPartitions].rowCount;        
            po->overallInfo.jointCount += po->info[po->nPartitions].jointCount;        
            po->overallInfo.contactCount += po->info[po->nPartitions].contactCount;

            (po->nPartitions)++;

            /*  Auto-disable this partition if its come to rest! 
                We allow immediate disable, in case we want to turn something
                off that has just been turned on by MdtUpdatePartitions.
				Only do this if we have actually shrunk partition.
                This should NOT come back into MdtLODLastPartition!!! */
			if(po->info[po->nPartitions-1].rowCount <= params->maxMatrixSize)
				MdtAutoDisableLastPartition(po, params);
        }
    }

    /* Free temporary data. */
    for(i=0; i<partitionCount; i++)
    {
        if(pdataArray[i].bodyArray)
            MeMemoryAPI.destroy(pdataArray[i].bodyArray);

        if(pdataArray[i].conArray)
            MeMemoryAPI.destroy(pdataArray[i].conArray);
    }

    MeMemoryFREEA(pdataArray);
    MeMemoryFREEA(qMem);
}
