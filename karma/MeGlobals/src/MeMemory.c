/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/17 16:21:55 $ - Revision: $Revision: 1.22.2.9 $

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

#include <stdlib.h>

#include <MeMemory.h>
#include <MeMessage.h>

#if (defined NGC)
#   include <dolphin/os.h>

    static unsigned initNGC = 1;
    static void *arenaLo, *arenaHi;
    static OSHeapHandle theHeap;

    extern void MeMemoryInitNGC(void)
    {
        void *arenaHiMinus;

        OSInit();

#if 0
        OSReport("MeMemoryInitNGC() started\n");
#endif

        arenaLo = OSGetArenaLo();
        arenaHi = OSGetArenaHi();
        arenaHiMinus = (void *) ((char *) arenaHi - (1<<20));

#if 0
        OSReport("initially: arenaLo 0x%08x, arenaHi 0x%08x\n",
            arenaLo,arenaHi);
#endif

        arenaLo = OSInitAlloc(arenaLo,arenaHiMinus,1);
        OSSetArenaLo(arenaLo);

#if 0
        OSReport("after OSInitAlloc: arenaLo 0x%08x, arenaHi 0x%08x\n",
            arenaLo,arenaHi);
#endif

        {
            void *const roundedLo = (void *) OSRoundUp32B(arenaLo);
            void *const roundedHi = (void *) OSRoundDown32B(arenaHiMinus);

#if 0
            OSReport("before OSCreateHeap:"
                " roundedLo 0x%08x, roundedHi 0x%08x\n",roundedLo,roundedHi);
#endif

            theHeap = OSCreateHeap(roundedLo,roundedHi);
        }

        OSSetCurrentHeap(theHeap);

        arenaLo = arenaHiMinus;
        OSSetArenaLo(arenaLo);

        OSReport("after OSCreateHeap: arenaLo 0x%08x, arenaHi 0x%08x\n",
            arenaLo,arenaHiMinus);

        initNGC = 0;
    }
#endif

/* To ensure MallocDestroyAligned is used to free memory allocated with
   MallocCreateAligned, the following word is used. */
#ifdef _MECHECK
#  define ALIGNED_MAGIC_NUMBER (0xCAD111AC)
#  define UNALIGNED_MAGIC_NUMBER (0x60D5111A)
#endif

/* ************************************************************************* */

/*
    There is a bit of a story here. C guarantees that any memory
    returned by 'malloc()' is going to be aligned to the natural
    alignment of the most strictly aligned builtin type. This usually is
    'double', thus 'malloc()'ed memory is aligned to 8 bytes on most
    platforms (with the exception of Visual C on x86, where it is 4 byte
    aligned, which works but unduly slowly if there is a double there).

    If we get an 8 byte aligned memory block and we prefix it with a
    single 'unsigned' of 4 bytes, then it becomes 4 aligned, and this
    can cause problems.
*/

#if (defined PS2)
#   define PADDING      (8)
#elif (defined _MSC_VER && defined WIN32)
#   define PADDING      (sizeof (unsigned))
#else
#   define PADDING      (sizeof (double))
#endif

/* Simple malloc-wrapping function for allocating memory. */
static void* MEAPI MallocCreate(size_t bytes)
{
#if (defined NGC)
    if (initNGC)
        MeMemoryInitNGC();

    return OSAlloc(OSRoundUp32B(bytes));
#elif (!defined _MECHECK)
    return malloc(bytes);
#else
    void *allocation, *mem = 0;

    if(bytes == 0)
        MeWarning(0, "MallocCreate: Allocating zero sized memory block.");

    allocation = malloc(PADDING + bytes);

	if (allocation)
	{
		/* We store an extra number to indicate this is an unaligned create. */
		*(unsigned int*)allocation = (unsigned int)UNALIGNED_MAGIC_NUMBER;

		/* Then move point on to actual memory to use and return. */
		mem = (char*)allocation + PADDING;
	}

    return mem;
#endif
}

/* ************************************************************************* */

/* Simple calloc-wrapping function for allocating and clearing memory. */
static void* MEAPI Calloc(size_t bytes)
{
#if (defined NGC)
    if (initNGC)
        MeMemoryInitNGC();

    {
        const size_t n = OSRoundUp32B(bytes);
        void *const b = OSAlloc(n);

        (void) memset(b,0,n);

        return b;
    }
#elif (!defined _MECHECK)
    return calloc(1,bytes);
#else
    void *allocation, *mem = 0;

    if(bytes == 0)
        MeWarning(0, "Calloc: Allocating zero sized memory block.");

    allocation = calloc(1,PADDING + bytes);

	if (allocation)
	{
		/* We store an extra number to indicate this is an unaligned create. */
		*(unsigned int*)allocation = (unsigned int)UNALIGNED_MAGIC_NUMBER;

		/* Then move point on to actual memory to use and return. */
		mem = (char*)allocation + PADDING;
	}
	
    return mem;
#endif
}

/* ************************************************************************* */

/*
    Simple free-wrapping function for freeing memory.
    Should NOT be called for memory created with MallocCreateAligned,
    use MallocDestroyAligned instead.
*/
static void  MEAPI MallocDestroy(void *const block)
{
#if (defined NGC)
    OSFree(block);
#elif (!defined _MECHECK)
    free(block);
#else
    void *allocation;

    if(!block)
    {
        MeWarning(0, "MallocDestroy: Trying to free null.");
        return;
    }

    allocation = (char*)block - PADDING;

    /* Check 'aligned' marker. */
    {
        unsigned int test = *(unsigned int*)allocation;
        if(test != (unsigned int)UNALIGNED_MAGIC_NUMBER)
        {
            if(test == (unsigned int)ALIGNED_MAGIC_NUMBER)
            {
                MeFatalError(0, "MallocDestroy: Freeing memory that "
                    "was allocated with MallocCreateAligned. "
                    "Use MallocDestroyAligned instead.");
            }
            else
            {
                MeFatalError(0, "MallocDestroy: Freeing memory that "
                    "was not allocated using MallocCreate or "
                    "MallocCreateAligned.");
            }
        }
    }
    
    free(allocation);
#endif
}

/* ************************************************************************* */

/* Simple realloc-wrapping function for resizing memory. */
static void* MEAPI MallocResize(void *const block, size_t bytes)
{
#if (defined NGC)
    /*
        We cannot resize because we can only resize by allocating a new
        block and copying into it the old block, but we don't know
        the size of the old block.
    */
    return 0;
#elif (!defined _MECHECK)
    return realloc(block, bytes);
#else
    void *allocation, *newAllocation, *mem;

    if(block == 0)
        return MallocCreate(bytes);

    allocation = (char*)block - PADDING;

    /* Check 'aligned' marker. */
    {
        unsigned int test = *(unsigned int*)allocation;
        if(test != (unsigned int)UNALIGNED_MAGIC_NUMBER)
        {
            if(test == (unsigned int)ALIGNED_MAGIC_NUMBER)
            {
                MeFatalError(0, "MallocResize: Resizing memory that "
                    "was allocated with MallocCreateAligned. "
                    "Use MallocResizeAligned instead.");
            }
            else
            {
                MeFatalError(0, "MallocResize: Freeing memory that "
                    "was not allocated using MallocCreate or "
                    "MallocCreateAligned.");
            }
        }
    }
    
    /* Call realloc on the 'original' pointer. */
    newAllocation = realloc(allocation, PADDING + bytes);
    mem = (char*)newAllocation + PADDING;
    return mem;
#endif
}

/* ************************************************************************* */

/*
   Function that allows user to specify desired alignment of returned
   memory. This will allocate slightly more space than 'bytes'. You
   MUST use MallocDestroyAligned to free memory allocated using this
   function. 'alignment' must be a multiple of sizeof(unsigned int).

   It works by allocating more space than is needed. The pointer is then
   moved up to the next desired aligned boundary. Then, the amount forward
   that the pointer was moved is stored in the word before this pointer
*/
static void* MEAPI MallocCreateAligned(size_t bytes, unsigned int alignment)
{
#if (defined NGC)
    if (initNGC)
        MeMemoryInitNGC();

    (void) alignment;

    return OSAlloc(OSRoundUp32B(bytes));
#else
    void* offsetStore;
#if (!defined _MECHECK)
    const int extra = 1;
#else
    const int extra = 2;
#endif
    unsigned int offset;

    /* Allow space for moving to aligned boundary and storing offset. */
    void* block = malloc(bytes + alignment + extra*sizeof(unsigned int));
    if(!block)
        return 0;

    /* Distance to next aligned boundary. */
    offset = alignment - (unsigned int)block % alignment;

    if (offset == alignment) offset = 0;

    if(alignment % sizeof(unsigned int) != 0)
    {
        MeFatalError(0, "MallocCreateAligned: Alignment must be a multiple "
            "of sizeof(unsigned int).");
    }

    /* If this wouldn't give us enough room to store the offset, wind on to
       next aligned boundary after that. */
    while(offset < extra*sizeof(unsigned int))
    {
        offset += alignment;
    }

    block = (char*)block + offset;

#if (defined _MECHECK)
    /* We store an extra number to indicate this is an aligned create. */
    offsetStore = (char*)block - sizeof(unsigned int);
    *(unsigned int*)offsetStore = (unsigned int)ALIGNED_MAGIC_NUMBER;
#endif

    offsetStore = (char*)block - extra*sizeof(unsigned int);
    *(unsigned int*)offsetStore = offset;

    return block;
#endif
}

/* ************************************************************************* */

/* */
static void MEAPI MallocDestroyAligned(void *const block)
{
#if (defined NGC)
    OSFree(block);
#else
    void *offsetStore, *start;
    unsigned int offset;
#if (!defined _MECHECK)
    const int extra = 1;
#else
    const int extra = 2;
#endif

#if (defined _MECHECK)
    if(!block)
    {
        MeWarning(0, "MallocDestroy: Trying to free null.");
        return;
    }

    /* Check 'aligned' marker. */
    offsetStore = (char*)block - sizeof(unsigned int);
    {
        unsigned int test = *(unsigned int*)offsetStore;
        if(test != (unsigned int)ALIGNED_MAGIC_NUMBER)
        {
            if(test == (unsigned int)UNALIGNED_MAGIC_NUMBER)
            {
                MeFatalError(0, "MallocDestroyAligned: Freeing memory that "
                    "was allocated with MallocCreate. "
                    "Use MallocDestroy instead.");
            }
            else
            {
                MeFatalError(0, "MallocDestroyAligned: Freeing memory that "
                    "was not allocated using MallocCreate or "
                    "MallocCreateAligned.");
            }
        }
    }
#endif

    offsetStore = (char*)block - extra*sizeof(unsigned int);
    offset = *(unsigned int*)offsetStore;
    start = (char*)block - offset;

    free(start);
#endif
}

/* ************************************************************************* */

struct MeMemoryAPIStruct MeMemoryAPI =
{
    &MallocCreate,
    &Calloc,
    &MallocCreateAligned,
    &MallocDestroy,
    &MallocDestroyAligned,
    &MallocResize
};
