/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.1.2.1 $

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

#include <string.h>          // for memset
#include <MeMemory.h>
#include <MePoolx.h>
#include <MeDict.h>          // for MeDictSetAllocator

/****************************************************************************
  This initializes the pool.  You must allocate both r and memory.

  recsize must be a multiple of sizeof(int) and it must be
  at least 2*sizeof(int).  If it isn't, an assertion will fail.

  The pool uses a lazy memory management scheme.  Basically, when you 
  call init, it is assumed that all the records are free.  The ifree is set
  to the first record and -1 is stored there.  As you get/put records, a
  linked list of free records is maintained, starting at the ifree index
  and the last item in the list is always the top of the "never been used"
  chunk of the memory.  The last item is always marked with a -1.

  If you put a record that is already free, this will cause the free
  list to become corrupt which can have disasterous effects.  Therefore
  I employ a heuristic to mark any free records with a 0xBadC0de int
  and then check that this int is not found in the put.
*/
void MePoolxInit(MePoolx *p, void *memory, int recsize, int numrec)
{
    MEASSERT(p && memory && numrec);
    MEASSERT(recsize >= 2*sizeof(int));
    MEASSERT(recsize % sizeof(int) == 0);

    p->mem = (int*)memory;
    p->isize = recsize / sizeof(int);
    p->numfree = p->numrec = numrec;
    p->ifree = 0;
    p->mem[0] = -1;
#ifdef _MECHECK
    p->mem[1] = 0xBadC0de;
#endif
}    

/****************************************************************************
  This gets a record from the memory.  Return 0 if no more available.
*/
void *MePoolxGet(MePoolx *p)
{
    int *result;
    MEASSERT(p);
    if (!p->numfree)
        return 0;
    result = p->mem + p->ifree;
    if (--p->numfree)
        if (result[0] == -1)
        {
            p->ifree += p->isize;
            p->mem[p->ifree] = -1;
#ifdef _MECHECK
            p->mem[p->ifree + 1] = 0xBadC0de;
#endif
        }
        else
        {
            p->ifree = result[0];
            MEASSERT(p->mem[p->ifree + 1] == 0xBadC0de);
        }
#ifdef _MECHECK
    result[1] = -1;    // erase the 0xBadC0de
#endif
    return result;
}

/****************************************************************************
  This gets a record from the memory and clears it.  
  Return 0 if no more available.
*/
void *MePoolxGetZeroed(MePoolx *p)
{
    void *r = MePoolxGet(p);
    if (r) 
        memset(r, 0, p->isize * sizeof(int));
    return r;
}

/****************************************************************************
  This puts a record back (marks it free).
*/
void MePoolxPut(MePoolx *p, void *rec)
{
    int i;
    MEASSERT(p && rec);
    i = (int*)rec - p->mem;

    //  Check if rec is a valid pointer
    MEASSERT(i >= 0 && i < p->numrec*p->isize && i % p->isize == 0);

    //  Check if rec is already free
    MEASSERT(p->mem[i + 1] != 0xBadC0de);

    p->mem[i] = p->ifree;
#ifdef _MECHECK
    p->mem[i + 1] = 0xBadC0de;
#endif
    p->ifree = i;
    ++p->numfree;
}

/****************************************************************************
  This gets one node from the pool.
  This is a convenience function to interface MePoolx with MeDict
  A node is the thing used to build the red/black tree in the dictionary.
*/
static MeDictNode *MePoolxDictNodeAllocate(void *pool)
{
    return (MeDictNode *) MePoolxGet((MePoolx *)pool);
}

/****************************************************************************
  This puts one node back in the pool.
  This is a convenience function to interface MePoolx with MeDict
*/
static void MePoolxDictNodeDeallocate(MeDictNode *node, void *pool)
{
    MePoolxPut((MePoolx *)pool, node);
}


/****************************************************************************
  This sets the allocator/deallocator in the dictionary to use the poolx
  This is a convenience function to interface MePoolx with MeDict
*/
void MePoolxUseWithDict(MePoolx *p, struct MeDict *d)
{ 
    MeDictSetAllocator(d, MePoolxDictNodeAllocate, MePoolxDictNodeDeallocate, p);
}

/****************************************************************************
  This is a test case for pool
*/

#if 0

#include <stdio.h>
#include <time.h>  // for clock

#define SZ 1557

typedef struct xx
{
    int a,b,c;
    char d;
} MePoolxTest_rec;

void MePoolxTest_main()
{
    MePoolxTest_rec mem[SZ];
    MePoolx r;

    MePoolxInit(&r, mem, sizeof *mem, SZ);

    MePoolxTest_rec *p[SZ];
    int i,j,k;
    void *w;
    
    clock_t t = clock();

    for (i=0; i<SZ; ++i)
    {
        //  get several
        for (j=0; j<=i; ++j)
            p[j] = (MePoolxTest_rec*) MePoolxGet(&r);

        if (p[i]==0)
            printf("Error, unable to allocate %d records\n",i);

        if (i==SZ - 1) w = MePoolxGet(&r);  // this should fail

        //  put half back randomly
        for (k=0; k<i/2; ++k)
        {
            j = rand() % (i+1);
            if (p[j]) MePoolxPut(&r, p[j]);
            p[j] = 0;
        }

        //  put back the other half
        for (j=0; j<=i; ++j)
            if (p[j])
                MePoolxPut(&r, p[j]);
    }

    t = clock() - t;

    if (!w) printf("The MePoolx test is successful.\n");

    printf("time for %d get/put is %g\n",(SZ+1)*SZ/2,t/1000.0);

    //  Repeat the same test using malloc for performance comparison

    t = clock();

    for (i=0; i<SZ; ++i)
    {
        //  get several
        for (j=0; j<=i; ++j)
            p[j] = (MePoolxTest_rec*) MeMemoryAPI.create(sizeof(MePoolxTest_rec*));

        if (p[i]==0)
            printf("Error, unable to allocate %d records\n",i);

        //  put half back randomly
        for (k=0; k<i/2; ++k)
        {
            j = rand() % (i+1);
            if (p[j]) MeMemoryAPI.destroy(p[j]);
            p[j] = 0;
        }

        //  put back the other half
        for (j=0; j<=i; ++j)
            if (p[j])
                MeMemoryAPI.destroy(p[j]);
    }

    t = clock() - t;
    printf("time for %d malloc/free is %g\n",(SZ+1)*SZ/2,t/1000.0);

}

#endif