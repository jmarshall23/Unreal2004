/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.5.2.2 $

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

#include <stdio.h>
#include <string.h>
#include <MePrecision.h>
#include <MeMemory.h>
#include <MeMessage.h>
#include <MeIDPool.h>

#define MEIDPOOL_MAX_IDS        32
#define MEIDPOOL_BLOCK_SIZE     32

/*
    Return the maximum number of blocks required for the given number of IDs.
    A block is defined as 32 bits.
*/
static int getMaxBlocks(int nIDs)
{
    if (nIDs < MEIDPOOL_BLOCK_SIZE)
        return 1;
    return nIDs / MEIDPOOL_BLOCK_SIZE + nIDs % MEIDPOOL_BLOCK_SIZE;
}

/* convert number of blocks to number of bytes */
static int blockToBytes(int blocks)
{
    return blocks * 4;
}

/**
    Allocates and initializes an instance of the MeIDPool structure.
*/
MeIDPool *MEAPI MeIDPoolCreate()
{
    MeIDPool *pool = (MeIDPool*)MeMemoryAPI.create(sizeof(MeIDPool));
    pool->maxIDs = MEIDPOOL_MAX_IDS;
    pool->maxBlocks = getMaxBlocks(pool->maxIDs);
    pool->IDbitfield = MeMemoryAPI.createZeroed(blockToBytes(pool->maxBlocks));
    pool->block = 0;
    pool->assignedIDs = 0;
    return pool;
}

/**
    Copies an MeIDPool.
*/
void MEAPI MeIDPoolCopy(MeIDPool *to, MeIDPool *from)
{
    to->maxIDs = from->maxIDs;
    to->maxBlocks = from->maxBlocks;
    to->block = from->block;
    to->assignedIDs = from->assignedIDs;
    MeMemoryAPI.destroy(to->IDbitfield);
    to->IDbitfield = MeMemoryAPI.createZeroed(blockToBytes(to->maxBlocks));
    memcpy(to->IDbitfield, from->IDbitfield, blockToBytes(to->maxBlocks));
}

/**
    Returns true if the pool is empty.
*/
MeBool MEAPI MeIDPoolIsEmpty(MeIDPool *pool)
{
    char *p;
    int i, empty = 1;
    for (i = 0, p = (char*)pool->IDbitfield; i < blockToBytes(pool->maxBlocks); p++, i++)
    {
        if (*p != '\0')
            empty = 0;
    }
    return pool->assignedIDs == 0 && empty ? 1 : 0;
}

/**
    Returns a unique integer. First resizes pool if it is full.
*/
int MEAPI MeIDPoolRequestID(MeIDPool *pool)
{
    unsigned int i, n, found = 0;
    MeU32 mask;
    unsigned *p;

    if (pool->assignedIDs == pool->maxIDs)
    {
        unsigned int oldMaxBlocks = pool->maxBlocks;
        pool->maxIDs += MEIDPOOL_MAX_IDS;
        pool->maxBlocks = getMaxBlocks(pool->maxIDs);

        if (pool->maxBlocks > oldMaxBlocks)
        {
            MeU32 offset;
            pool->IDbitfield = MeMemoryAPI.resize(pool->IDbitfield, blockToBytes(pool->maxBlocks));

            /* initialize remainder to zero */
            offset = blockToBytes(oldMaxBlocks) / 4;
            memset(pool->IDbitfield + offset, '\0', blockToBytes(1));
            pool->block++;
        }
    }

    while (!found)
    {
        mask = 0x80000000;
        p = pool->IDbitfield + pool->block;
        n = pool->block * MEIDPOOL_BLOCK_SIZE;        
        
        /* go through block until a 0 bit found */
        for (i = 0; i < MEIDPOOL_BLOCK_SIZE && n < pool->maxIDs; i++, n++, mask >>= 1) 
        {
            if (!(*p & mask))
            {
                found = 1;
                break;
            }            
        }

        if (!found || i == MEIDPOOL_BLOCK_SIZE - 1)
            pool->block++;

        if (pool->block >= pool->maxBlocks)
            pool->block = 0;
    }

    *p |= mask;
    
    pool->assignedIDs++;

    return n;
}

/**
    Returns an integer ID to the pool of available IDs.
*/
void MEAPI MeIDPoolReturnID(MeIDPool *pool, int id)
{
    int block = id / MEIDPOOL_BLOCK_SIZE;
    int stride = id % MEIDPOOL_BLOCK_SIZE;
    MeU32 mask = 0x80000000;
    unsigned *p;

    mask >>= stride;
    
    p = pool->IDbitfield + block;

    /* if not already returned to pool, return it */
    if (*p & mask)
    {
        *p &=~mask;
        pool->assignedIDs--;
    }
}

/**
    Clears the ID pool.
*/
void MEAPI MeIDPoolReset(MeIDPool *pool)
{
    memset(pool->IDbitfield, '\0', blockToBytes(pool->maxBlocks));
    pool->block = 0;
    pool->assignedIDs = 0;
}

/**
    Destroys the ID pool.
*/
void MEAPI MeIDPoolDestroy(MeIDPool *pool)
{
    MeMemoryAPI.destroy(pool->IDbitfield);
    MeMemoryAPI.destroy(pool);
}

