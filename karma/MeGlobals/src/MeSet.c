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

#include <MeMemory.h>
#include <MeSet.h>

/****************************************************************************
  This is the default comparison function.
  Return -1 if item1 < item2, 0 if equal, 1 if item1 > item2.
*/
static int MeSetDefaultCompare(const void *item1, const void *item2)
{
    return (char*)item1 - (char*)item2;
}

/****************************************************************************
  This initializes the set.  You must allocate both s and node memory.

  The comparison function is OPTIONAL.  If cmp is zero (null pointer) then
  the record pointers are compared.  This is usually acceptable.
*/
void MeSetInit(MeSet *s, MeDictNode *nodemem, int maxnode, MeDictCompareFn cmp)
{
    //  init the pool and dict and then tie them together.
    MePoolxInit(&s->nodepool, nodemem, sizeof *nodemem, maxnode);
    MeDictInit(&s->dict, maxnode, cmp ? cmp : MeSetDefaultCompare);
    MePoolxUseWithDict(&s->nodepool, &s->dict);
    s->next = s->last = 0;
}

/****************************************************************************
  This returns the number of (unique) elements in the set.
*/
int MeSetSize(MeSet *s)
{
    return (int) MeDictCount(&s->dict);
}

/****************************************************************************
  This return true if set contains item.
*/
int MeSetContains(MeSet *s, void *item)
{
    return 0 != MeDictLookup(&s->dict, item);
}

/****************************************************************************
  This adds item to the set if it is not already in the set.
  Return true if item was added.
*/
int MeSetAdd(MeSet *s, void *item)
{
    MeDictNode *node;

    if (MeSetContains(s, item))
        return 0;

    node = s->dict.allocnode(s->dict.context);
    if (!node) 
        return 0;

    MEASSERT(!(s->last && s->last->data));
    MEASSERT(!(s->next && !s->last));

    if (!s->next)
        s->next = node;
    if (s->last)
        s->last->data = node;
    s->last = node;

    MeDictNodeInit(node, 0);
    MeDictInsert(&s->dict, node, item);
    return 1;
}
 
/****************************************************************************
  This removes item from set if it is in the set.
  Return true if item was removed.
*/
int MeSetRemove(MeSet *s, void *item)
{
    MeDictNode *dn = MeDictLookup(&s->dict, item);
    if (!dn)
        return 0;
    MeDictDeleteFree(&s->dict, dn);
    return 1;
}

/****************************************************************************
  PRIVATE - This function is used internally by PopFirst and Pop.
*/
static void *MeSetPopNode(MeSet *s, MeDictNode *dn)
{
    void *item;
    if (!dn) 
        return 0;
    item = (void*) dn->key;      // cast off const
    MeDictDeleteFree(&s->dict, dn);
    return item;
}

/****************************************************************************
  This removes the first item from set.
  Return the item, or NULL if set is empty.
*/
void *MeSetPopFirst(MeSet *s)
{
    return MeSetPopNode(s, MeDictFirst(&s->dict));
}

/****************************************************************************
  This removes the any item from set.  The key of this item is likely to
  be near the median, but there are no guarantees.
  Return the item, or NULL if set is empty.
*/
void *MeSetPop(MeSet *s)
{
    return MeSetPopNode(s, s->dict.nilnode.left);   // this is the root of the tree
}

/****************************************************************************
  Returns each item in the set in the order they were added.
  WARNING!  You must not do any Remove or Pop because it will mess up
  the linked list.  The data pointers are used to superimpose a linked
  list on the dictionary from oldest to newest.
*/
void *MeSetIteratorNext(MeSet *s)
{
    void *k;
    if (!s->next)       // end of list
        return 0;
    k = (void *) s->next->key;   // cast off const
    s->next = (MeDictNode*) s->next->data;
    return k;
}

/****************************************************************************
  This returns true if set is full.  Note, if the set dictionary does not
  use a fixed size pool, this will never return true.
*/
int MeSetIsFull(MeSet *s)
{
    return MeDictIsFull(&s->dict);
}

/****************************************************************************
  This is a convenience function to make an unlimited size set which 
  will use MeMemoryAPI to malloc nodes.  For performance, MeSetInit is
  better than MeSetCreate.
*/
MeSet *MeSetCreate(MeDictCompareFn cmp)
{
    MeSet *s = (MeSet *) MeMemoryAPI.createZeroed(sizeof *s);
    MeDictInit(&s->dict, INT_MAX, cmp ? cmp : MeSetDefaultCompare);
    s->nodepool.numrec = -999;
    return s;
}

/****************************************************************************
  This is a convenience function to destroy a set created by MeSetCreate.
*/
void MeSetDestroy(MeSet *s)
{
    MEASSERT(s->nodepool.numrec == -999);   // ensure MeSetCreate was called.

    if (s->nodepool.numrec == -999)
    {
        MeDictFreeNodes(&s->dict);
        MeMemoryAPI.destroy(s);
    }
}


/****************************************************************************
  This is a test case for set.

  Change the #if 0 to 1 if you want to run it, and call MeSetTest_main.
*/
#if 0

#include <stdlib.h>
#include <stdio.h>

#define SZ 5

void MeSetTest_main()
{
    MeSet s1, *s = &s1;
    MeDictNode m[SZ];
    int i;

#ifndef _MECHECK
    printf("\n ERROR --> you must define _MECHECK to run this test case.\n");
#endif

//    MEASSERT(!"If you see this, MEASSERT is working correctly.");

    printf("\ntry init...\n");

    MeSetInit(s, m, SZ, 0);    // initialize set

    i = MeSetIsFull(s);    MEASSERT(i == 0);
    i = MeSetSize(s);    MEASSERT(i == 0);

    i = MeSetAdd(s, (void*) 3);    MEASSERT(i == 1);
    i = MeSetAdd(s, (void*) 4);    MEASSERT(i == 1);
    i = MeSetAdd(s, (void*) 2);    MEASSERT(i == 1);
    i = MeSetIsFull(s);    MEASSERT(i == 0);
    i = MeSetAdd(s, (void*) 5);    MEASSERT(i == 1);
    i = MeSetAdd(s, (void*) 1);    MEASSERT(i == 1);

    i = MeSetAdd(s, (void*) 3);    MEASSERT(i == 0);
    i = MeSetAdd(s, (void*) 4);    MEASSERT(i == 0);

    i = MeSetIsFull(s);    MEASSERT(i == 1);
    i = MeSetSize(s);    MEASSERT(i == 5);

    i = MeSetContains(s, (void*) 2);    MEASSERT(i == 1);
    i = MeSetContains(s, (void*) 6);    MEASSERT(i == 0);
    i = MeSetContains(s, (void*) 4);    MEASSERT(i == 1);
    i = MeSetContains(s, (void*) 6);    MEASSERT(i == 0);

    i = MeSetRemove(s, (void*) 6);    MEASSERT(i == 0);
    i = MeSetRemove(s, (void*) 4);    MEASSERT(i == 1);

    i = MeSetSize(s);    MEASSERT(i == 4);

    i = MeSetRemove(s, (void*) 1);    MEASSERT(i == 1);
    i = MeSetRemove(s, (void*) 2);    MEASSERT(i == 1);
    i = MeSetRemove(s, (void*) 3);    MEASSERT(i == 1);
    i = MeSetRemove(s, (void*) 5);    MEASSERT(i == 1);

    i = MeSetSize(s);    MEASSERT(i == 0);

    //--------------------------------------------
    //  repeat using create

    printf("\ntry create...\n");

    s = MeSetCreate(0);    // initialize set

    i = MeSetIsFull(s);    MEASSERT(i == 0);
    i = MeSetSize(s);    MEASSERT(i == 0);

    i = MeSetAdd(s, (void*) 3);    MEASSERT(i == 1);
    i = MeSetAdd(s, (void*) 4);    MEASSERT(i == 1);
    i = MeSetAdd(s, (void*) 2);    MEASSERT(i == 1);
    i = MeSetIsFull(s);    MEASSERT(i == 0);
    i = MeSetAdd(s, (void*) 5);    MEASSERT(i == 1);
    i = MeSetAdd(s, (void*) 1);    MEASSERT(i == 1);

    i = MeSetAdd(s, (void*) 3);    MEASSERT(i == 0);
    i = MeSetAdd(s, (void*) 4);    MEASSERT(i == 0);

    i = MeSetIsFull(s);    MEASSERT(i == 0);
    i = MeSetSize(s);    MEASSERT(i == 5);

    i = MeSetContains(s, (void*) 2);    MEASSERT(i == 1);
    i = MeSetContains(s, (void*) 6);    MEASSERT(i == 0);
    i = MeSetContains(s, (void*) 4);    MEASSERT(i == 1);
    i = MeSetContains(s, (void*) 6);    MEASSERT(i == 0);

    i = MeSetRemove(s, (void*) 6);    MEASSERT(i == 0);
    i = MeSetRemove(s, (void*) 4);    MEASSERT(i == 1);

    i = MeSetSize(s);    MEASSERT(i == 4);

    i = MeSetRemove(s, (void*) 1);    MEASSERT(i == 1);
    i = MeSetRemove(s, (void*) 2);    MEASSERT(i == 1);
    i = MeSetRemove(s, (void*) 3);    MEASSERT(i == 1);
    i = MeSetRemove(s, (void*) 5);    MEASSERT(i == 1);

    i = MeSetSize(s);    MEASSERT(i == 0);

    MeSetDestroy(s);

    //  Test iterator

    s = MeSetCreate(0);    // initialize set

    for (i=0; i<10; ++i)
    {
        MeSetAdd(s, (void*) i);
        if (~i & 1)
            MEASSERT(MeSetIteratorNext(s) == (void*)(i>>1));
    }
    for (i=5; i<10; ++i)
        MEASSERT(i == (int) MeSetIteratorNext(s));

    MEASSERT(0 == MeSetIteratorNext(s));
    MEASSERT(0 == MeSetIteratorNext(s));
    MEASSERT(0 == MeSetIteratorNext(s));
    
    MeSetDestroy(s);

    printf("\ndone\n");
}

#endif