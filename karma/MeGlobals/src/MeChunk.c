/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.17.2.3 $

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

#include <MeChunk.h>
#include <MeAssert.h>
#include <MeMemory.h>
#include <MeMessage.h>
#include "MeMath.h"

/* ************************************************************************* */

/**
 *  Initialise this MeChunk to default values. Parameter 'alignment' indicates 
 *  the byte alignment that is required for temporary memory provided by this 
 *  MeChunk.
 *  This must be called before any other MeChunk functions.
 */
void MEAPI MeChunkInit(MeChunk* chunk, int alignment)
{
    /* Alignment must be at least sizeof unsigned int. */
    chunk->alignment = MeMAX(alignment, sizeof(unsigned int));
    chunk->isInUse = 0;
    chunk->maxUsed = 0;
    chunk->memBase = 0;
    chunk->memSize = 0;
    chunk->mode = kMeChunkModeKeepOnPut;    
}
/* ************************************************************************* */

/**
 *  Finish using this MeChunk. Will release any memory still currently held
 *  inside this MeChunk. You must not call this function while the Chunk is
 *  stil 'in use'. Call MeChunkPutMem first.
 *
 *  You should not call any other MeChunk functions (apart from MeChunkInit)
 *  after calling this funciton.
 */
void MEAPI MeChunkTerm(MeChunk* chunk)
{
#ifdef _MECHECK
    if(chunk->isInUse)
    {
        MeFatalError(0, "MeChunkTerm: MeChunk still in use.\n");
    }
#endif
    
    if(chunk->memBase)
    {
        MeMemoryAPI.destroyAligned(chunk->memBase);
        chunk->memBase = 0;
        chunk->memSize = 0;
    }
}
/* ************************************************************************* */

/**
 *  Set the behaviour mode for MeChunk. Determines whether memory is kept 
 *  until next needed, or is freed each time.
 *  @see kMeChunkMode
 */
void MEAPI MeChunkSetMode(MeChunk* chunk, kMeChunkMode mode)
{
#ifdef _MECHECK
    if(mode != kMeChunkModeFreeOnPut && mode != kMeChunkModeKeepOnPut)
    {
        MeFatalError(0, "MeChunkSetMode: Unknown MeChunkMode.\n");
    }
#endif

    /* Check if its different. */
    if(chunk->mode == mode)
        return;

    /*  If we are currently in ModeKeepOnPut, and we are holding onto some 
        memory, free it now. */
    if(!chunk->isInUse && 
        chunk->mode == kMeChunkModeKeepOnPut && 
        chunk->memBase)
    {
        MeMemoryAPI.destroyAligned(chunk->memBase);
    }

    chunk->mode = mode;
}
/* ************************************************************************* */

/**
 *  Returns a pointer to a temporary memory region of size at least 'mem'. 
 *  The alignment of this memory will be that which was specified when calling
 *  MeChunkInit. When finished using the memory, call MeChunkPutMem.
 *  @see MeChunkInit
 *  @see MeChunkPutMem
 */
void* MEAPI MeChunkGetMem(MeChunk* chunk, int size)
{
#ifdef _MECHECK
    if(chunk->isInUse)
    {
        MeFatalError(0, "MeChunkGetMem: MeChunk still in use.");
    }
#endif

    /* If we are not holding onto enough memory already, allocate some. */
    if(size > chunk->memSize)
    {
        /* Free any existing memory. */
        if(chunk->memBase)
        {
            MeMemoryAPI.destroyAligned(chunk->memBase);
            chunk->memBase = 0;
            chunk->memSize = 0;
        }

        /* Allocate a new block. */
        chunk->memBase = MeMemoryAPI.createAligned(size, chunk->alignment);

#ifdef _MECHECK
        if(!chunk->memBase)
        {
            MeFatalError(0, "MeChunkGetMem: MeMemoryAPI.createAligned failed "
                "to allocate %d bytes.", size);
        }
#endif
        chunk->memSize = size;

        /* Keep track of the largest amount this MeChunk ever uses. */
        if(chunk->memSize > chunk->maxUsed)
            chunk->maxUsed = chunk->memSize;
    }

    chunk->isInUse = 1;
    return chunk->memBase;
}

/* ************************************************************************* */

/**
 *  Once finished using the temporary memory region, call this function to
 *  indicate it is no longer needed. You should NOT use 'mem' again after
 *  calling this function.
 *  The behaviour when you do this is defined by the current kMeChunkMode.
 *  @see MeChunkGetMem 
 *  @see MeChunkSetMode
*/
void MEAPI MeChunkPutMem(MeChunk* chunk, void* mem)
{
    if(!chunk->isInUse)
    {
#ifdef _MECHECK
        MeWarning(0, "MeChunkPutMem: MeChunk not in use.");
#endif
        return;
    }

#ifdef _MECHECK
    if(chunk->memBase != mem)
    {
        MeFatalError(0, "MeChunkPutMem: Returning memory not from "
            "this MeChunk.");
    }
#endif

    /* If mode is FreeOnPut, deallocate memory here. */
    if(chunk->mode == kMeChunkModeFreeOnPut)
    {
        MeMemoryAPI.destroyAligned(mem);
        chunk->memBase = 0;
        chunk->memSize = 0;
    }

    chunk->isInUse = 0;
}

/* ************************************************************************* */

/** 
 *  Returns the maximum size of temporary memory ever requested from this 
 *  MeChunk. 
 */
int MEAPI MeChunkGetMaxSize(MeChunk* chunk)
{
    return chunk->maxUsed;
}

/* ************************************************************************* */

/** 
 *  Returns the current size of temporary memory allocated by this MeChunk. 
 */
int MEAPI MeChunkGetCurrentSize(MeChunk* chunk)
{
    return chunk->memSize;
}

/* ************************************************************************* */

/** 
 *  Returns a boolean value indicating whether or not the application is 
 *  currently using temporary memory provided by this MeChunk. 
 */
MeBool MEAPI MeChunkIsInUse(MeChunk* chunk)
{
    return chunk->isInUse;
}

/* ************************************************************************* */

/** 
 *  Returns the byte-alignment used by this MeChunk. All memory returned by
 *  MeChunkGetMem is aligned to a multiple of this many bytes.
 *  @see MeChunkInit
 */
int MEAPI MeChunkGetAlignment(MeChunk* chunk)
{
    return chunk->alignment;
}