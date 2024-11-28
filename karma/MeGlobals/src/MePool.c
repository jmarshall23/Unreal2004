/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.23.6.4 $

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

#include <MeCall.h>
#include <MeMemory.h>
#include <MeMessage.h>

#include <MePool.h>

#define MePoolDEFAULTFIXED      1
#define MePoolDEBUGPUTGET       0

static void MEAPI       MePoolFixedInit(MePool* pool,
                            int poolSize, int structSize, int alignment);
static void MEAPI       MePoolFixedDestroy(MePool* pool);
static void MEAPI       MePoolFixedReset(MePool* pool);
static void * MEAPI     MePoolFixedGetStruct(MePool* pool);
static void MEAPI       MePoolFixedPutStruct(MePool* pool, void* s);
static int  MEAPI       MePoolFixedGetUsed(MePool* pool);
static int  MEAPI       MePoolFixedGetUnused (MePool* pool);

static void MEAPI       MePoolMallocInit(MePool* pool,
                            int poolSize, int structSize, int alignment);
static void MEAPI       MePoolMallocDestroy(MePool* pool);
static void MEAPI       MePoolMallocReset(MePool* pool);
static void * MEAPI     MePoolMallocGetStruct(MePool* pool);
static void MEAPI       MePoolMallocPutStruct(MePool* pool, void* s);
static int MEAPI        MePoolMallocGetUsed(MePool* pool);
static int MEAPI        MePoolMallocGetUnused (MePool* pool);

struct MePoolAPI        MePoolAPI =
{
#if MePoolDEFAULTFIXED
    &MePoolFixedInit,
    &MePoolFixedDestroy,
    &MePoolFixedReset,
    &MePoolFixedGetStruct,
    &MePoolFixedPutStruct,
    &MePoolFixedGetUsed,
    &MePoolFixedGetUnused
#else
    &MePoolMallocInit,
    &MePoolMallocDestroy,
    &MePoolMallocReset,
    &MePoolMallocGetStruct,
    &MePoolMallocPutStruct,
    &MePoolMallocGetUsed,
    &MePoolMallocGetUnused
#endif
};

struct MePoolAPI        MePoolFixedAPI =
{
    &MePoolFixedInit,
    &MePoolFixedDestroy,
    &MePoolFixedReset,
    &MePoolFixedGetStruct,
    &MePoolFixedPutStruct,
    &MePoolFixedGetUsed,
    &MePoolFixedGetUnused
};

struct MePoolAPI        MePoolMallocAPI =
{
    &MePoolMallocInit,
    &MePoolMallocDestroy,
    &MePoolMallocReset,
    &MePoolMallocGetStruct,
    &MePoolMallocPutStruct,
    &MePoolMallocGetUsed,
    &MePoolMallocGetUnused
};

/**
 *  Reset (empty) the pool.
 *  All pointers to structs in the pool are invalid after this.
 */
static void MEAPI MePoolFixedReset(MePool* u)
{
    struct MePoolFixed *const pool = &u->u.fixed;
    int i;
    void* p = pool->structArray;
    MEASSERT(u->t == MePoolFIXED);

    for (i = 0; i < pool->poolSize; i++)
    {
        pool->freeStructStack[i] = p;
        p = (char*)p + pool->structSize;
    }
}

/**
 * Create a pool of structs (memory allocation done here).
 * 'alignment' species byte multiple for each structure to start on.
 * This is optional and is ignored if '0'.
 */
static void MEAPI MePoolFixedInit(MePool* u,
    int poolSize, int structSize, int alignment)
{
    struct MePoolFixed *const pool = &u->u.fixed;

    int allocateSize = 0;

    if(poolSize > 0)
    {
        /* We don't need to consider alignment. */
        if(alignment == 0)
        {
            allocateSize = structSize;

            pool->structArray = MeMemoryAPI.create(poolSize * allocateSize);
            pool->createdAligned = 0;
        }
        else
        {
            /* If structSize is already multiple of alignment big. */
            if(structSize % alignment == 0)
                allocateSize = structSize;
            else
                allocateSize = structSize
                  - (structSize % alignment) + alignment;

            pool->structArray = MeMemoryAPI.createAligned(
                poolSize * allocateSize, alignment);
            pool->createdAligned = 1;
        }


        if(!pool->structArray)
        {
#ifdef _MECHECK
            MeFatalError(0, "%s",
              "MePoolFixedInit: Could not allocate pool memory.");
#endif
        }

        pool->freeStructStack = (void**)
            MeMemoryAPI.create(poolSize * sizeof(void*));

        if(!pool->freeStructStack)
        {
#ifdef _MECHECK
            MeFatalError(0, "%s",
              "MePoolFixedInit: Could not allocate free struct stack.");
#endif
        }
    }

    pool->structSize = allocateSize;
    pool->poolSize = poolSize;

    u->t = MePoolFIXED;
    MePoolFixedReset(u);

    pool->nextFreeStruct = 0;
}

/**
 * Destroy the pool (memory freed here).
 * Does not free the MePoolFixed struct itself.
 */
static void MEAPI MePoolFixedDestroy(MePool* u)
{
    struct MePoolFixed *const pool = &u->u.fixed;
    MEASSERT(u->t == MePoolFIXED);

    if(pool->poolSize > 0)
    {
        if(pool->createdAligned)
            MeMemoryAPI.destroyAligned(pool->structArray);
        else
            MeMemoryAPI.destroy(pool->structArray);

        MeMemoryAPI.destroy(pool->freeStructStack);
    }
}

/** Get a struct pointer out of the pool. */
static void* MEAPI MePoolFixedGetStruct(MePool* u)
{
    struct MePoolFixed *const pool = &u->u.fixed;
    void *p;
    MEASSERT(u->t == MePoolFIXED);

    if (pool->nextFreeStruct < pool->poolSize)
        p = pool->freeStructStack[(pool->nextFreeStruct)++];
    else
    {
#ifdef _MECHECK
        MeWarning(0, "MePoolFixedGetStruct: Pool 0x%08x (size %d) empty.",
          (long) pool,pool->poolSize);
#endif
        p = 0;
    }

#if MePoolDEBUGPUTGET
    MeDebug(0,"F getStruct: pool 0x%08x, align %2d, used %4d, p 0x%08x\n",
      (long) pool, pool->createdAligned,
      pool->nextFreeStruct, (long) p);
#endif

    return p;
}

/** Put a struct back into the pool. */
static void MEAPI MePoolFixedPutStruct(MePool* u, void* s)
{
    struct MePoolFixed *const pool = &u->u.fixed;
    MEASSERT(u->t == MePoolFIXED);

#if MePoolDEBUGPUTGET
    MeDebug(0,"F putStruct: pool 0x%08x, align %2d, used %4d, p 0x%08x\n",
      (long) pool,  pool->createdAligned,
      pool->nextFreeStruct, (long) s);
#endif

    if (pool->nextFreeStruct > 0)
    {
#ifdef _MECHECK
        /* check if this thing does not belong in this pool. */
        if(((int)s < (int)pool->structArray)
            || ((int)s > (int)pool->structArray + ((int)pool->poolSize - 1)
                *pool->structSize))
        {
            MeWarning(0, "MePoolFixedPutStruct: Structure does not seem to "
                "come from this pool 0x%08x.",(long) pool);
        }
#endif
        pool->freeStructStack[--(pool->nextFreeStruct)] = s;

    }
#ifdef _MECHECK
    else
    {
        MeWarning(0, "MePoolFixedPutStruct: Putting structure back "
            "into full pool 0x%08x.",(long) pool);
    }
#endif
}

/** Return the number of structs currently used in the pool. */
static int MEAPI MePoolFixedGetUsed(MePool* u)
{
    const struct MePoolFixed *const pool = &u->u.fixed;

    return pool->nextFreeStruct;
}

/** Return the amount of space left in the pool. */
static int MEAPI MePoolFixedGetUnused(MePool* u)
{
    const struct MePoolFixed *const pool = &u->u.fixed;

    return pool->poolSize - pool->nextFreeStruct;
}

/**
 *  Reset (empty) the pool.
 *  All pointers to structs in the pool are invalid after this.
 */
static void MEAPI MePoolMallocReset(MePool* u)
{
    struct MePoolMalloc *const pool = &u->u.malloc;

    MeWarning(1,"%s\n","MePoolMallocReset() not implemented");
}

/**
 * Create a pool of structs (memory allocation done here).
 * 'alignment' species byte multiple for each structure to start on.
 * This is optional and is ignored if '0'.
 */
static void MEAPI MePoolMallocInit(MePool* u,
    int poolSize, int structSize, int alignment)
{
    struct MePoolMalloc *const pool = &u->u.malloc;

    u->t = MePoolMALLOC;
    pool->usedStructs = 0;
    pool->poolSize = poolSize;
    pool->structSize = structSize;
    pool->alignment = alignment;
}

/**
 * Destroy the pool (memory freed here).
 * Does not free the MePoolMalloc struct itself.
 */
static void MEAPI MePoolMallocDestroy(MePool* u)
{
    struct MePoolMalloc *const pool = &u->u.malloc;
    MEASSERT(u->t == MePoolMALLOC);

    if (pool->usedStructs != 0)
        MeFatalError(0,"MePoolMallocDestroy(): %d structs still allocated",
            pool->usedStructs);
}

/** Get a struct pointer out of the pool. */
static void* MEAPI MePoolMallocGetStruct(MePool* u)
{
    struct MePoolMalloc *const pool = &u->u.malloc;
    MEASSERT(u->t == MePoolMALLOC);

    if (pool->usedStructs >= pool->poolSize)
    {
#ifdef _MECHECK
        MeWarning(0, "MePoolMallocGetStruct: Pool 0x%08x (size %d) empty.",
            (long) pool,pool->poolSize);
#endif

        return 0;
    }

    {
        void *const p = (pool->alignment == 0)
            ? (*MeMemoryAPI.create)(pool->structSize)
            : (*MeMemoryAPI.createAligned)(pool->structSize,pool->alignment);

        if (p == 0)
            return 0;

        pool->usedStructs++;

#if MePoolDEBUGPUTGET
        MeDebug(0,"M getStruct: pool 0x%08x, align %2d, used %4d, p 0x%08x\n",
          (long) pool, pool->alignment, pool->usedStructs, (long) p);
#endif

        return p;
    }
}

/** Put a struct back into the pool. */
static void MEAPI MePoolMallocPutStruct(MePool* u, void* s)
{
    struct MePoolMalloc *const pool = &u->u.malloc;
    MEASSERT(u->t == MePoolMALLOC);
        
#if MePoolDEBUGPUTGET
    MeDebug(0,"M putStruct: pool 0x%08x, align %2d, used %4d, p 0x%08x\n",
      (long) pool, pool->alignment, pool->usedStructs, (long) s);
#endif

#ifdef _MECHECK
    if (pool->usedStructs == 0)
        MeWarning(0, "%s\n","MePoolMallocPutStruct: Putting structure back "
            "into pool 0x%08x with no allocated structs.",(long) pool);
#endif

    if (s != 0)
    {
        (void)
          (*
            ((pool->alignment == 0)
              ? MeMemoryAPI.destroy
              : MeMemoryAPI.destroyAligned)
          )(s);

        if (pool->usedStructs > 0)
          --pool->usedStructs;
    }
}

/** Return the number of structs currently used in the pool. */
static int MEAPI MePoolMallocGetUsed(MePool* u)
{
    const struct MePoolMalloc *const pool = &u->u.malloc;

    return pool->usedStructs;
}

/** Return the amount of space left in the pool. */
static int MEAPI MePoolMallocGetUnused(MePool* u)
{
    const struct MePoolMalloc *const pool = &u->u.malloc;

    return pool->poolSize - pool->usedStructs;
}
