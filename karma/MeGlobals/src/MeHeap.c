/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.3.2.3 $

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

#include <MeMemory.h>
#include <MeHeap.h>
#include "MeMessage.h"

/****************************************************************************
  This is the default comparison function.
*/
static int MeHeapDefaultCompare(const void *item1, const void *item2)
{
    return *(int*)item1 - *(int*)item2;
}

/**
  This initializes the heap.  You must allocate both h and memory.

  The comparison function is OPTIONAL.  If cmp is zero (null pointer) then
  the records are cast to integers and compared.
*/
void MeHeapInit(MeHeap *h, void **memory, int capacity, MeHeapComparisonFnPtr cmp)
{
    h->capacity = capacity;
    h->cmp = cmp ? cmp : MeHeapDefaultCompare;
    h->mem = memory - 1;    // this makes the mem array be based 1.
    h->used = 0;
}

/**
  Push an item into the heap. 
  Return 1 if success, 0 if heap is full.

  For a cute description of the algorithm see
  http://ciips.ee.uwa.edu.au/~morris/Year2/PLDS210/heaps.html
*/
int MeHeapPush(MeHeap *h, void *item)
{
    void *t;
    int i,p;

    if (h->used >= h->capacity)
    {
#ifdef _MECHECK
        MeWarning(0, "MeHeapPush: Heap already full.");
#endif
        return 0;
    }

    //  Walk up the heap swapping the child with the parent

    i = ++h->used;
    h->mem[i] = item;       // don't forget! mem is based 1.
    for (p = i>>1; p; i=p, p>>=1)
    {
        if (h->cmp(h->mem[i], h->mem[p]))
        {
            t = h->mem[i];              // SWAP i <--> p
            h->mem[i] = h->mem[p];
            h->mem[p] = t;
        }
        else 
            break;
    }
    return 1;
}

/**
  This removes the best (lowest) item from the heap and return it.
  Return NULL if the heap is empty.
*/
void* MeHeapPop(MeHeap *h)
{
    void *result;
    void *t;
    int i,p;

    if (h->used < 1)
    {
        return 0;
    }

    result = h->mem[1];  // don't forget! mem is based 1.

    h->mem[1] = h->mem[h->used--];   // move last to top of heap

    //  Walk down the heap, swapping the best child with the parent.

    for (p=1, i=2; i <= h->used; p=i, i<<=1)
    {
        // if p better than i
        if (h->cmp(h->mem[p], h->mem[i]))  
        {
            // if i+1 not exist or p better than i+1, stop
            if (i+1 > h->used || h->cmp(h->mem[p], h->mem[i+1]))
                break;
            ++i;         // by transitivity, i+1 is better than i
        }
        // if i+1 exists and i+1 better than i
        else if (i+1 <= h->used && h->cmp(h->mem[i+1], h->mem[i]))
            ++i;

        t = h->mem[i];              // SWAP i <--> p
        h->mem[i] = h->mem[p];
        h->mem[p] = t;
    }
    return result;
}

/**
  This is a convenience function to malloc memory and then call MeHeapInit.
*/
MeHeap *MeHeapCreate(int capacity, MeHeapComparisonFnPtr cmp)
{
    MeHeap *h = (MeHeap *) MeMemoryAPI.create(sizeof *h);
    void **m = (void **) MeMemoryAPI.create(capacity * sizeof *m);
    MeHeapInit(h, m, capacity, cmp);
    return h;
}

/**
  This is a convenience function to destroy a heap created by MeHeapCreate.
*/
void MeHeapDestroy(MeHeap *h)
{
    MeMemoryAPI.destroy(h->mem + 1);  // don't forget! mem is based 1.
    MeMemoryAPI.destroy(h);
}


/****************************************************************************
  This is a test case for heap.

  Change the #if 0 to 1 if you want to run it, and call MeHeapTest_main.
*/
#if 0

#include <stdlib.h>
#include <stdio.h>

//  comparison function
int MeHeapTest_cmp(const void *a, const void *b) 
{
    return (int)a < (int)b;
}

#define SZ 313

void MeHeapTest_main()
{
    MeHeap h;
    void *m[SZ];

    //  This test case assumes the following...

    MEASSERT(sizeof(int) <= sizeof(void*));

    MeHeapInit(&h, m, SZ, MeHeapTest_cmp);    // initialize heap

    int i,a,b;
    void *t;

    t = MeHeapPop(&h);
    MEASSERT(!t);          // heap empty

    for (i=0; i<SZ; ++i)
    {
        a = rand();
        b = MeHeapPush(&h, (void*)a);
        MEASSERT(b==1);
    }
    b = MeHeapPush(&h, (void*)a);  // too many
    MEASSERT(b==0);

    for (i=0; i<SZ; ++i)
    {
        t = MeHeapPop(&h);
        a = (int)t;
        MEASSERT(b <= a);
        b = a;
        printf("\t%d",a);
    }

    t = MeHeapPop(&h);
    MEASSERT(!t);          // heap empty

    printf("\ndone\n");
}

#endif