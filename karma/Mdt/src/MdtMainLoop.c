/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.30.2.7 $

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
 * Dynamics 'Main Loop' functions.
 * This file contains the main functions called from MdtWorldStep in
 * MdtWorld.c.
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

/**
 * Check the last partition in MdtPartitionOutput and see if it has
 * 'come to rest'. If so - disable the bodies and remove the partition
 * from the MdtPartitionOutput. Used as a callback in MdtUpdatePartitions.
 */
void MEAPI MdtAutoDisableLastPartition(MdtPartitionOutput* po, void* pcbdata)
{
    MeBool partitionIsMoving = 0; /* assume stopped until proven otherwise*/
    int p = po->nPartitions-1; /* partition we are checking (last one) */

    MdtPartitionParams* params = (MdtPartitionParams*)pcbdata;

    /* If we are 'auto-disabling' bodies, see if partition has come to rest. */
    if(params->autoDisable)
    {
        int b;

        /*  Iterate over last partition bodies.
            As soon as we find a moving body, bail out.. */
        for(b = po->bodiesStart[p];
        (b < (po->bodiesStart[p] + po->bodiesSize[p]) 
            && !partitionIsMoving); b++)
        {
            if(MdtBodyIsMovingTest(po->bodies[b], params))
            {
                partitionIsMoving = 1;
            }
        }
    }
    else
        partitionIsMoving = 1;

    /* if partition has come to rest, disable bodies in it and remove
       partition from MdtPartitionOutput. */
    if(!partitionIsMoving)
    {
        int b;

        for(b = po->bodiesStart[p];
        b < (po->bodiesStart[p] + po->bodiesSize[p]);
        b++)
        {
            MdtBodyID body = po->bodies[b];
            MdtBodyDisable(body);
            MdtBodyResetForces(body);
            MdtBodyResetImpulses(body);
        }
        po->nPartitions--;
        po->totalBodies -= po->bodiesSize[p];
        po->totalConstraints -= po->constraintsSize[p];

        po->overallInfo.rowCount -= po->info[p].rowCount;        
        po->overallInfo.jointCount -= po->info[p].jointCount;        
        po->overallInfo.contactCount -= po->info[p].contactCount;
    }
    /*  if its still alive, see how big we think its going to be, and if 
        larger than MaxMatrixSize, take measures to 'cap' it. */
    else
    {
        if(po->info[p].rowCount > params->maxMatrixSize)
        {
#if 0
            MeWarning(0, "MATRIX CAPPING TEMPORARILY DISABLED.\n"
                "Sorry, being re-written.");
#else
            MdtLODLastPartition(po, params);
#endif
        }
    }
}

/** Update forces and torques on body due to damping etc. */
void MdtUpdateBodyForces(MdtBodyID b, MeReal stepSize, MeVector3 g)
{
    MeReal averageInertia;
    /* Mark body as constrained if it is.. */
    if(!MeDictIsEmpty(&b->constraintDict))
        b->keaBody.flags |= MdtKeaBodyFlagIsConstrained;
    else
        b->keaBody.flags &= ~MdtKeaBodyFlagIsConstrained;

    /* Add forces due to damping and gravity */


    averageInertia = (b->keaBody.I0[0] + b->keaBody.I1[1] + b->keaBody.I2[2])/3;
    b->keaBody.torque[0] += -b->angularDamping * b->keaBody.velrot[0] * averageInertia;
    b->keaBody.torque[1] += -b->angularDamping * b->keaBody.velrot[1] * averageInertia;
    b->keaBody.torque[2] += -b->angularDamping * b->keaBody.velrot[2] * averageInertia;

    b->keaBody.force[0] += ((-b->damping * b->keaBody.vel[0]) + g[0]) * b->mass;
    b->keaBody.force[1] += ((-b->damping * b->keaBody.vel[1]) + g[1]) * b->mass;
    b->keaBody.force[2] += ((-b->damping * b->keaBody.vel[2]) + g[2]) * b->mass;

    /* This is used by the auto-disabler to give bodies a minimum number of
       frames enabled. */
    b->enabledTime += stepSize;

    /* Convert impulse into force/torque (using timestep) and accumulate. */
    if(b->impulseAdded)
    {
        int i;
        MeReal invStep = (MeReal)1/stepSize;

        for(i=0; i<4; i++)
        {
            b->keaBody.force[i] += invStep * b->impulseLinear[i];
            b->keaBody.torque[i] += invStep * b->impulseAngular[i];

            b->impulseLinear[i] = 0;
            b->impulseAngular[i] = 0;
        }
        b->impulseAdded = 0;
    }

}

static void MEAPI LogMatrixSize(int partitionRows, MdtWorldParams* params)
{
    int entry = MeMathCEIL4(partitionRows)/4;
    
    /* Ensure 'entry' is not outside log. */
    entry = MeCLAMP(entry, 0, params->matrixSizeLogSize-1);
    
    /* increment this element. */
    (params->matrixSizeLog[entry])++;
}

/**
 *  Set up Kea input from the output of the partitioner. Turns ALL partitions
 *  into ONE set of Kea input. See PackOnePartition for function to simulate 
 *  partitions seperately.
 *
 *  Input to Kea is:
 *      1) Array of pointers to MdtKeaBody structs.
 *      2) Array of MdtKeaTransformation structs.
 *      3) MdtKeaConstraints struct built using MdtBcl.
 *
 *  Note that if there are no constraints in a partition out of the
 *  partitioner, then it will not appear in the MdtKeaConstraints
 *  struct.
 *
 *  Returns the maximum number of rows in any partition.
 */
unsigned int MEAPI MdtPackAllPartitions(const MdtPartitionOutput* po,
                        const MeReal stepSize, MdtWorldParams* params,
                        MdtKeaParameters* keaParams,
                        MdtKeaTransformation* keaTMArray,
                        MdtKeaConstraints* constraints)
{
    int i, partitionindex;
    unsigned int partitionConstraintRows = 0, maxPartitionConstraintRows = 0;
    
    MdtBody** blist;
    MdtKeaBody **keabodyArray;
    MdtBaseConstraint** clist;
    int bodiesSize, bodiesStart, constraintsSize;
    

    MdtBclSolverParameters bclParams;
    bclParams.stepsize = stepSize;
    bclParams.epsilon = keaParams->epsilon;
    bclParams.gamma = keaParams->gamma;

    MdtBclInitConstraintRowList(constraints);
    
    /* Array of pointers to Kea bodies to simulate. */
    keabodyArray = (MdtKeaBody**)po->bodies;

    for(partitionindex=0; partitionindex<po->nPartitions; partitionindex++)
    {
        /* Find number of bodys and constraints in this partition,
           and the start of the body and constraint list. */
        
        blist = &(po->bodies[po->bodiesStart[partitionindex]]);
        bodiesSize = po->bodiesSize[partitionindex];
        bodiesStart = po->bodiesStart[partitionindex];
        
        clist = &(po->constraints[po->constraintsStart[partitionindex]]);
        constraintsSize = po->constraintsSize[partitionindex];

        /* Update all bodies in this partition (forces etc.) */
        for(i = 0; i < bodiesSize; i++)
        {
            MdtUpdateBodyForces(blist[i], stepSize, params->gravity);
            
            /* Copy body data and transformation into arrays */

            GetCOMTransform(blist[i], (MeMatrix4Ptr)&keaTMArray[i + bodiesStart]);
            MEASSERT(keabodyArray[i + bodiesStart] == &blist[i]->keaBody);
#ifdef _MECHECK
            if(!MeMatrix4IsTM((MeMatrix4Ptr)&(keaTMArray[i + bodiesStart]), ME_MEDIUM_EPSILON))
                MeWarning(0, "MdtPackAllPartitions: "
                "Transformation matrix of model is invalid.");
#endif
        }
        
        for (i = 0; i < constraintsSize; i++)
        {
            MdtContactGroup *group = MdtConstraintDCastContactGroup(clist[i]);
            
            /* Set correct body integer offsets in the BCL constraint struct. */
            clist[i]->head.bodyindex[0] = clist[i]->head.mdtbody[0]->arrayIdWorld;
            
            if (clist[i]->head.mdtbody[1] == 0)
                clist[i]->head.bodyindex[1] = MdtBclNO_BODY;
            else
                clist[i]->head.bodyindex[1] = clist[i]->head.mdtbody[1]->arrayIdWorld;
        }
        
        /* Check we don't write off the end of the buffer. */
#ifdef _MECHECK
        if (po->totalConstraints > constraints->max_constraints)
            MeFatalError(1, "Adding too many constraints.");
        if (po->nPartitions > constraints->max_partitions)
            MeFatalError(1, "Adding too many partitions.");
#endif
        
        /*
            Convert each constraint into the constraint rows that represent it by
            calling MdtBcl functions.
        */
        
        if(constraintsSize > 0)
        {
            MdtBclStartPartition(constraints);
            
            for (i = 0; i < constraintsSize; i++)
            {
                clist[i]->head.bclFunction(constraints,
                    (void *) clist[i], keaTMArray, keabodyArray,
                    &bclParams);
            }
            
            MdtBclEndPartition(constraints);    
            
            partitionConstraintRows = 
                constraints->num_rows_exc_padding_partition[constraints->num_partitions-1];
            
            if(params->matrixSizeLog) /* If we are logging matrix sizes, do it here. */
                LogMatrixSize(partitionConstraintRows, params);
        }
        else
        {
            if(params->matrixSizeLog)
                LogMatrixSize(0, params);
        }
        
        if(partitionConstraintRows > maxPartitionConstraintRows)
            maxPartitionConstraintRows = partitionConstraintRows;
    }

    return maxPartitionConstraintRows;
}

/** 
 *  As above, but only packs one partition to simulate. Note you cannot pack
 *  more than one partition into the same set of input using this function, you
 *  can only pack all (using PackAllPartitions) or one (using this function). 
 */
unsigned int MEAPI MdtPackPartition(const MdtPartitionOutput* po,
                        const unsigned int partitionindex,
                        const MeReal stepSize, MdtWorldParams* params,
                        MdtKeaParameters* keaParams,
                        MdtKeaTransformation* keaTMArray,
                        MdtKeaConstraints* constraints)
{
    int i;
    unsigned int partitionConstraintRows = 0;
    
    MdtBody** blist;
    MdtKeaBody **keabodyArray;
    MdtBaseConstraint** clist;
    int bodiesSize, constraintsSize;
    

    MdtBclSolverParameters bclParams;
    bclParams.stepsize = stepSize;
    bclParams.epsilon = keaParams->epsilon;
    bclParams.gamma = keaParams->gamma;

    MdtBclInitConstraintRowList(constraints);

    /* Find number of bodys and constraints in this partition,
    and the start of the body and constraint list. */
    
    blist = &(po->bodies[po->bodiesStart[partitionindex]]);
    bodiesSize = po->bodiesSize[partitionindex];
    
    clist = &(po->constraints[po->constraintsStart[partitionindex]]);
    constraintsSize = po->constraintsSize[partitionindex];
    
    /* Array of pointers to Kea bodies to simulate. */
    keabodyArray = (MdtKeaBody**)blist;

    /* Update all bodies in this partition (forces etc.) */
    for(i = 0; i < bodiesSize; i++)
    {
        MdtUpdateBodyForces(blist[i], stepSize, params->gravity);
        
        /* Copy body data and transformation into arrays */
        memcpy(&keaTMArray[i], &(blist[i]->comTM), sizeof (MeMatrix4));
        MEASSERT(keabodyArray[i] == &blist[i]->keaBody);

#ifdef _MECHECK
            if(!MeMatrix4IsTM((MeMatrix4Ptr)&(keaTMArray[i]), ME_MEDIUM_EPSILON))
                MeWarning(0, "MdtPackAllPartitions: "
                "Transformation matrix of model is invalid.");
#endif

    }
    
    for (i = 0; i < constraintsSize; i++)
    {
        MdtContactGroup *group = MdtConstraintDCastContactGroup(clist[i]);
        
        /* Set correct body integer offsets in the BCL constraint struct. */
        clist[i]->head.bodyindex[0] = clist[i]->head.mdtbody[0]->arrayIdPartition;
        
        if (clist[i]->head.mdtbody[1] == 0)
            clist[i]->head.bodyindex[1] = MdtBclNO_BODY;
        else
            clist[i]->head.bodyindex[1] = clist[i]->head.mdtbody[1]->arrayIdPartition;
    }
    
    /* Check we aren't going to write off the end of the buffer. */
#ifdef _MECHECK
    if (po->constraintsSize[partitionindex] > constraints->max_constraints)
        MeFatalError(1, "Adding too many constraints.");
#endif
    
    /*
        Convert each constraint into the constraint rows that represent it by
        calling MdtBcl functions.
    */

    if(constraintsSize > 0)
    {
        MdtBclStartPartition(constraints);
        
        for (i = 0; i < constraintsSize; i++)
        {
            clist[i]->head.bclFunction(constraints,
                (void *) clist[i], keaTMArray, keabodyArray,
                &bclParams);
        }
        
        MdtBclEndPartition(constraints);    
        
        partitionConstraintRows = 
            constraints->num_rows_exc_padding_partition[constraints->num_partitions-1];

        if(params->matrixSizeLog) /* If we are logging matrix sizes, do it here. */
            LogMatrixSize(partitionConstraintRows, params);
    }
    else
    {
        if(params->matrixSizeLog)
            LogMatrixSize(0, params);
    }

    return partitionConstraintRows;
}

/**
 * Copy bodies and forces back into one partition from PartitionOutput.
 */
void MEAPI MdtUnpackBodies(MdtKeaTransformation* keaTMArray,
               const unsigned int partitionindex, 
			   MdtPartitionOutput* po)
{
    int i;
    MdtBody** blist = &(po->bodies[po->bodiesStart[partitionindex]]);

    /* Unpack all processed bodies */
    for (i = 0; i < po->bodiesSize[partitionindex]; i++)
    {
        MEASSERT(MeMatrix4IsTM((MeMatrix4Ptr)&keaTMArray[i],(MeReal)0.001));
        UpdateBodyTransform(blist[i], (MeMatrix4Ptr)&keaTMArray[i]);
        MEASSERT(MeMatrix4IsTM(blist[i]->bodyTM,(MeReal)0.001));        
    }
}

/**
 *  Copy forces from the output of Kea back to the Mdt constraint strcuts.
 *  Deals with MdtContactGroups which may contain multiple MdtContacts. This
 *  means that the number of constraints in the Kea ouput and the number of
 *  constraints in the partition output may not be the same.
 *  Returns the number of constraints in the Kea output that were unpacked.
 */
unsigned int MEAPI MdtUnpackForces(const MdtKeaForcePair *force,
               const unsigned int partitionindex, 
			   MdtPartitionOutput* po)
{
    int mdtCNum, keaCNum = 0, j;
    const MdtKeaForcePair* forceStart;
    MdtBaseConstraint** clist =
        &(po->constraints[po->constraintsStart[partitionindex]]);

    /* Get the constraint forces out and put them somewhere safe.*/
    for (mdtCNum = 0; mdtCNum < po->constraintsSize[partitionindex]; mdtCNum++)
    {
        MdtContactGroupID group = MdtConstraintDCastContactGroup(clist[mdtCNum]);

        if(group == 0)
        {
            forceStart = force + keaCNum;

            for (j = 0; j < 4; j++)
            {
                clist[mdtCNum]->head.resultForce[0][j] = forceStart->primary_body.force[j];
                clist[mdtCNum]->head.resultTorque[0][j] = forceStart->primary_body.torque[j];

                clist[mdtCNum]->head.resultForce[1][j] = forceStart->secondary_body.force[j];
                clist[mdtCNum]->head.resultTorque[1][j] = forceStart->secondary_body.torque[j];
            }

            keaCNum++;
        }
        else
        {
            MdtContactID contact;

            /* There will be one result force for each contact in a group. */
            contact = group->first;

            for (j = 0; j < 4; j++)
            {
                group->head.resultForce[0][j] = 0;
                group->head.resultTorque[0][j] = 0;
                
                group->head.resultForce[1][j] = 0;
                group->head.resultTorque[1][j] = 0;
            }

            group->normalForce = 0;

            while(contact)
            {
                forceStart = force + keaCNum;

                for (j = 0; j < 3; j++)
                {
                    contact->head.resultForce[0][j] = forceStart->primary_body.force[j];
                    contact->head.resultTorque[0][j] = forceStart->primary_body.torque[j];
                    
                    contact->head.resultForce[1][j] = forceStart->secondary_body.force[j];
                    contact->head.resultTorque[1][j] = forceStart->secondary_body.torque[j];

                    /* group holds total force applied by contacts */

                    group->head.resultForce[0][j] += contact->head.resultForce[0][j];
                    group->head.resultTorque[0][j] += contact->head.resultTorque[0][j];

                    group->head.resultForce[1][j] += contact->head.resultForce[1][j];
                    group->head.resultTorque[1][j] += contact->head.resultTorque[1][j];
                }

                /* Accumulate magnitude of force along normal direction. */
                group->normalForce += MeVector3Dot(
                    contact->head.resultForce[0], contact->normal);

                contact = contact->nextContact;
                keaCNum++;
            }
        }
    }

    return keaCNum;
}

/** Get the lowest body 'safe time' in a partition. */
MeReal MEAPI MdtPartitionGetSafeTime(MdtPartitionOutput* po, int i)
{
    int b;
    MdtBodyID body;
    MeReal safeTime = MEINFINITY;

    for(b = po->bodiesStart[i]; 
    b < po->bodiesStart[i] + po->bodiesSize[i]; 
    b++)
    {
        body = po->bodies[b];

        if(body->safeTime < safeTime)
            safeTime = body->safeTime;
    }

    return safeTime;
}

/** 
 *  Utility for creating an empty, temporary MdtKeaConstraints struct
 *  from the MeChunk provided.
 *
 *  The difference between maxConstraints and maxKeaConstraints is that 
 *  maxConstraints will count a contactgroup as 1, but maxKeaConstraints
 *  will count the number of contacts in the group.
 */
MdtKeaConstraints* MEAPI MdtKeaConstraintsCreateFromChunk(MeChunk* chunk,
                                                          int maxPartitions,
                                                          int maxKeaConstraints,
                                                          int maxRows)
{
    MdtKeaConstraints* c;
    int maxRowsIncPadding, totalSize = 0;
    int jbodySize, jofsSize, jsizeSize, xgammaSize, slipfactorSize, 
        forceSize, lambdaSize, hiSize, loSize, cSize, xiSize, jstoreSize;

    int structSize = sizeof(MdtKeaConstraints);

    /*  Dilip: 11/3/2002
    the following code exercises a compiler bug in Visual C:

    const int numrowsexSize = maxPartitions * sizeof(int);
    const int numrowsincSize = maxPartitions * sizeof(int);
    const int numconsSize = maxPartitions * sizeof(int);
    
    so replaced with this:
    */

    int numrowsexSize = maxPartitions * sizeof(int);
    int numrowsincSize = numrowsexSize;
    int numconsSize = numrowsexSize;

    /*  Kea adds 4 blank rows after each kea constraint in the Jstore, which
        could add another 4 rows per partition due to that being rounded up.*/
    maxRowsIncPadding = maxRows + (maxKeaConstraints*4) + (maxPartitions*4);

    totalSize += (structSize + 
        numrowsexSize + 
        numrowsincSize + 
        numconsSize);
    
    if(maxKeaConstraints > 0)
    {
        /* Calculate the total amount of memory we need for this partitionOutput. */
        jbodySize = MeMathCEIL64(maxKeaConstraints * sizeof(MdtKeaBodyIndexPair)); /* 64 */
        jofsSize = maxKeaConstraints * sizeof (int);
        jsizeSize = maxKeaConstraints * sizeof (int);
        xgammaSize = MeMathCEIL64(maxRows * sizeof (MeReal)); /* 64 */
        slipfactorSize = MeMathCEIL64(maxRows * sizeof (MeReal)); /* 64 */
        forceSize = MeMathCEIL64(maxKeaConstraints * sizeof (MdtKeaForcePair)); /* 64 */
        lambdaSize = MeMathCEIL64(maxRows * sizeof (MeReal)); /* 64 */
        hiSize = MeMathCEIL64(maxRows * sizeof (MeReal)); /* 64 */
        loSize = MeMathCEIL64(maxRows * sizeof (MeReal)); /* 64 */
        cSize = MeMathCEIL64(maxRows * sizeof (MeReal)); /* 64 */
        xiSize = MeMathCEIL64(maxRows * sizeof (MeReal)); /* 64 */
        jstoreSize = MeMathCEIL64(maxRowsIncPadding/4 * sizeof(MdtKeaJBlockPair)); /* 64 */
        
        /* We allow extra space for each array to allow us to align it correctly. */
        
        totalSize += (jbodySize + 64 +
            jofsSize + 
            jsizeSize +
            xgammaSize + 64 +
            slipfactorSize + 64 +
            forceSize + 64 +
            lambdaSize + 64 +
            hiSize + 64 +
            loSize + 64 +
            cSize + 64 +
            xiSize + 64 +
            jstoreSize + 64);
        
    }

    
    c = (MdtKeaConstraints*)MeChunkGetMem(chunk, totalSize);
    
    c->max_partitions = maxPartitions;
    c->max_constraints = maxKeaConstraints;
    
    c->num_rows_exc_padding_partition = (int*)((char*)c + structSize);
    c->num_rows_inc_padding_partition = (int*)((char*)c->num_rows_exc_padding_partition + numrowsexSize);
    c->num_constraints_partition = (int*)((char*)c->num_rows_inc_padding_partition + numrowsincSize);
    
    if(maxKeaConstraints > 0)
    {
        c->max_rows_inc_padding = maxRowsIncPadding;
        c->max_rows_exc_padding = maxRows;

        c->Jbody = (MdtKeaBodyIndexPair*)MeMemory64ALIGN((char*)c->num_constraints_partition + numconsSize);
        c->Jofs = (int*)((char*)(c->Jbody) + jbodySize);
        c->Jsize = (int*)((char*)(c->Jofs) + jofsSize);
        c->xgamma = (MeReal*)MeMemory64ALIGN((char*)(c->Jsize) + jsizeSize);
        c->slipfactor = (MeReal*)MeMemory64ALIGN((char*)(c->xgamma) + xgammaSize);
        c->force = (MdtKeaForcePair*)MeMemory64ALIGN((char*)(c->slipfactor) + slipfactorSize);
        c->lambda = (MeReal*)MeMemory64ALIGN((char*)(c->force) + forceSize);
        c->hi = (MeReal*)MeMemory64ALIGN((char*)(c->lambda) + lambdaSize);
        c->lo = (MeReal*)MeMemory64ALIGN((char*)(c->hi) + hiSize);
        c->c = (MeReal*)MeMemory64ALIGN((char*)(c->lo) + loSize);
        c->xi = (MeReal*)MeMemory64ALIGN((char*)(c->c) + cSize);
        c->Jstore = (MdtKeaJBlockPair*)MeMemory64ALIGN((char*)(c->xi) + xiSize);
        
#ifdef PS2
        /* On the PS2, slipfactor must be accessed in uncached mode */
        c->slipfactor = (MeReal *)((unsigned int)(c->slipfactor)|0x30000000);
#endif

#ifdef _MECHECK
        {
            char* endPointer = (char*)(c->Jstore) + jstoreSize;
            if(endPointer > (char*)c + totalSize)
                MeFatalError(1, "MdtKeaConstraintsCreateFromChunk: Error.");
        }
#endif
    }
    else
    {
        c->max_rows_exc_padding = 0;        
        c->max_rows_inc_padding = 0;        

        c->Jbody = 0;
        c->Jofs = 0;
        c->Jsize = 0;
        c->xgamma = 0;
        c->slipfactor = 0;
        c->force = 0;
        c->lambda = 0;
        c->hi = 0;
        c->lo = 0;
        c->c = 0;
        c->xi = 0;
        c->Jstore = 0;

#ifdef _MECHECK
        {
            char* endPointer = (char*)(c->num_constraints_partition) + numconsSize;
            if(endPointer > (char*)c + totalSize)
                MeFatalError(1, "MdtKeaConstraintsCreateFromChunk: Error.");
        }
#endif
    }

    return c;
}     

/** 
 *  Default function called when sim check fails.
 *  This function safely resets bodies velocities and accelerations, so
 *  bodies will not move, but no FatalErrors will occur.
 *
 *  @see MdtWorldSetSimErrorCB
 */
void MEAPI MdtDefaultSimErrorCallBack(MdtKeaConstraints* kc, 
                                      MdtKeaBody** kb, 
                                      int nBodies, 
                                      void* secbdata)
{
    int i;

    for(i=0; i<nBodies; i++)
    {
        MdtKeaBody* body = kb[i];

        MeVector3Set(body->force, 0, 0, 0);
        MeVector3Set(body->torque, 0, 0, 0);
        MeVector3Set(body->accel, 0, 0, 0);
        MeVector3Set(body->accelrot, 0, 0, 0);
        MeVector3Set(body->vel, 0, 0, 0);
        MeVector3Set(body->velrot, 0, 0, 0);
    }

    for(i=0; i<kc->num_constraints; i++)
    {
        kc->force[i].primary_body.force[0] = 0;
        kc->force[i].primary_body.force[1] = 0;
        kc->force[i].primary_body.force[2] = 0;

        kc->force[i].primary_body.torque[0] = 0;
        kc->force[i].primary_body.torque[1] = 0;
        kc->force[i].primary_body.torque[2] = 0;

        kc->force[i].secondary_body.force[0] = 0;
        kc->force[i].secondary_body.force[1] = 0;
        kc->force[i].secondary_body.force[2] = 0;

        kc->force[i].secondary_body.torque[0] = 0;
        kc->force[i].secondary_body.torque[1] = 0;
        kc->force[i].secondary_body.torque[2] = 0;
    }

    MeWarning(0, "MdtDefaultSimErrorCallBack: Error in simulation.");
}
