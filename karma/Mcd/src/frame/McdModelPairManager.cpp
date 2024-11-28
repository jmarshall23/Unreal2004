/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.2.2.1 $

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

#include <MePool.h>
#include <McdCheck.h>
#include <McdCTypes.h>
#include <MeMath.h>
#include <McdModelPair.h>
#include <McdModelPairManager.h>
#include <MeMemory.h>

/*---------------------------------------------------------------
* McdModelPairManager implementation
*----------------------------------------------------------------
*/


MeINLINE void McdModelPairManagerLinkPhaseInsert(McdModelPairManagerLink *list, McdModelPairManagerLink *element) 
{
    element->phaseNext = list->phaseNext;
    element->phasePrev = list;
    element->phaseNext->phasePrev = element;
    element->phasePrev->phaseNext = element;
}
        
MeINLINE void McdModelPairManagerLinkPhaseRemove(McdModelPairManagerLink *link)
{
    link->phaseNext->phasePrev = link->phasePrev;
    link->phasePrev->phaseNext = link->phaseNext;
}


McdModelPairManagerHash *McdModelPairManagerHashCreate(int size, int buckets) 
{
    McdModelPairManagerHash *hash = (McdModelPairManagerHash *)MeMemoryAPI.create(sizeof(McdModelPairManagerHash));
    int i;
    hash->size = size;
    
    for(hash->bucketCount = 1; buckets>1; hash->bucketCount<<=1, buckets>>=1);
    
    hash->bucket = (McdModelPairManagerLink **)MeMemoryAPI.create(hash->bucketCount * sizeof(McdModelPairManagerLink*));
    for(i=0;i<hash->bucketCount;i++) hash->bucket[i]=0;
    return hash;
}

void McdModelPairManagerHashDestroy(McdModelPairManagerHash *hash)
{
    MeMemoryAPI.destroy(hash->bucket);
    MeMemoryAPI.destroy(hash);
}

MeINLINE int McdModelPairManagerHashKeyToBucket(McdModelPairManagerHash *hash, int key)  
{ 
    return ((key>>16)^(key&0xffff)) & (hash->bucketCount-1); 
}

MeINLINE void McdModelPairManagerHashInsert(McdModelPairManagerHash *hash, McdModelPairManagerLink *link)
{
    int bucket = McdModelPairManagerHashKeyToBucket(hash,link->key);
    link->next = hash->bucket[bucket];        
    hash->bucket[bucket] = link;
}

MeINLINE McdModelPairManagerLink *McdModelPairManagerHashRemove(McdModelPairManagerHash *hash, int key)
{
    int bucket = McdModelPairManagerHashKeyToBucket(hash,key);
    McdModelPairManagerLink **linkptr = hash->bucket + bucket, *link;
    while(*linkptr!=0) 
    {
        if((*linkptr)->key == key)
        {
            link = *linkptr;
            *linkptr = link->next;
            return link;
        }
        linkptr = &(*linkptr)->next;
    }
    return 0;
}

MeINLINE McdModelPairManagerLink *McdModelPairManagerHashAccess(McdModelPairManagerHash *hash, int key)
{
    McdModelPairManagerLink *link = hash->bucket[McdModelPairManagerHashKeyToBucket(hash,key)];
    while(link) 
    {
        if(link->key == key)
            return link;
        link = link->next;
    }
    return 0;
}


MeINLINE int McdModelPairManagerHashSize(McdModelPairManagerHash *hash) 
{
    return hash->size;
}

MeINLINE McdModelPairManagerLink *McdModelPairManagerHashGetNext(McdModelPairManagerHash *hash, McdSpacePairIterator *i)
{
    McdModelPairManagerLink *link = (McdModelPairManagerLink *)i->ptr;
    
    if(!link)
    {
        for(i->count++; i->count<hash->bucketCount && !hash->bucket[i->count];i->count++);
        
        if(i->count == hash->bucketCount)
            return 0;
        
        link = hash->bucket[i->count];
    }    
    
    i->ptr = link->next;
    return link;
}            


#ifdef _MECHECK
void McdModelPairManagerHashDump(McdModelPairManagerHash *hash)
{
    int i;
    McdModelPairManagerLink *link;
    for(i=0;i<hash->bucketCount;i++)
    {
        for(link = hash->bucket[i];link;link = link->next)
        {
            printf("(%d, %d)\n", link->key>>16,link->key&0xffff);
        }
    }
    puts("------------------");
}
#endif




McdModelPairManagerID MEAPI McdModelPairManagerCreate(int count)
{
    McdModelPairManagerID manager = (McdModelPairManagerID )MeMemoryAPI.create(sizeof(McdModelPairManager));

    manager->linkPool = (MePool *)MeMemoryAPI.create(sizeof(MePool));
    MePoolAPI.init(manager->linkPool,count, sizeof(McdModelPairManagerLink),0);

    manager->pairPool = (MePool *)MeMemoryAPI.create(sizeof(MePool));
    MePoolAPI.init(manager->pairPool,count, sizeof(McdModelPair),0);

    manager->hash = McdModelPairManagerHashCreate(count,count/10);
    manager->goodbyeList.phaseNext = manager->goodbyeList.phasePrev = &manager->goodbyeList;
    manager->helloList.phaseNext = manager->helloList.phasePrev = &manager->helloList;
    return manager;

}

void MEAPI McdModelPairManagerDestroy(McdModelPairManagerID manager)
{
    McdModelPairManagerHashDestroy(manager->hash);
    (*MePoolAPI.destroy)(manager->linkPool);
    MeMemoryAPI.destroy(manager->linkPool);
    MePoolAPI.destroy(manager->pairPool);
    MeMemoryAPI.destroy(manager->pairPool);
    MeMemoryAPI.destroy(manager);
}


MeBool MEAPI
McdModelPairManagerGetTransitions(McdModelPairManagerID manager, McdSpacePairIterator * iter, McdModelPairContainer* a)
{
    int i=0;
    McdModelPairManagerLink *link = (McdModelPairManagerLink *)iter->ptr;
    a->goodbyeFirst = 0;
    a->helloFirst = 0;

    MCD_CHECK_ASSERT_(iter->count<0, "CxSmallSort::getTransitions");

    switch(iter->count) // these are really meant to drop through!
    {

    case -1:
        iter->ptr = manager->goodbyeList.phaseNext;
        iter->count = -2;

    case -2:
        while(iter->ptr!=&manager->goodbyeList && i<a->size)
        {
            a->array[i++]=((McdModelPairManagerLink *)iter->ptr)->pair;
            iter->ptr = ((McdModelPairManagerLink *)iter->ptr)->phaseNext;
        }
        a->goodbyeEnd = i;
        if(iter->ptr==&manager->goodbyeList)
        {
            iter->count =-3;
            iter->ptr = manager->helloList.phaseNext;
            a->helloFirst = i;
        }
    
    case -3:            
        while(iter->ptr!=&manager->helloList && i<a->size)
        {
            a->array[i++]=((McdModelPairManagerLink *)iter->ptr)->pair;
            iter->ptr = ((McdModelPairManagerLink *)iter->ptr)->phaseNext;
        }
        if(iter->ptr==&manager->helloList)
            iter->count =-4;
    }

    a->helloEndStayingFirst = i;
    a->stayingEnd = a->helloEndStayingFirst;

    return i==a->size && iter->count>-4;
}

MeBool MEAPI
McdModelPairManagerGetPairs(McdModelPairManagerID manager, McdSpacePairIterator * iter, McdModelPairContainer* a)
{
    McdModelPairManagerLink *link;
    McdModelPair *pair;
    int i;

    MCD_CHECK_ASSERT_(iter->count>=-1, "CxSmallSort::getPairs");

    a->goodbyeFirst = 0;
    a->goodbyeEnd = 0;
    a->helloFirst = a->size;
    a->stayingEnd = a->size;
    
    for(i=0; i < a->size; i++)
    {
        link = McdModelPairManagerHashGetNext(manager->hash,iter);
        if(link==0) break;

        if(link->pair->phase==kMcdFFStateGoodbye)
            a->array[a->goodbyeEnd++] = link->pair;
        else
            a->array[--a->helloFirst] = link->pair;
    }

    int result = link?1:0;

    // sort the hello pairs below the staying pairs

    int hesf = a->size;
    for(i=a->helloFirst;i<hesf;i++)
    {
        if(a->array[i]->phase==kMcdFFStateStaying)
        {
            while(i<hesf && a->array[--hesf]->phase != kMcdFFStateHello);

            pair = a->array[hesf];
            a->array[hesf] = a->array[i];
            a->array[i] = pair;
        }
    }

    a->helloEndStayingFirst = hesf;

    return result;
}


inline MeU32 CxSmallSortKey(MeI32 inID1, MeI32 inID2) {
  return (((MeU32)inID1)<<16)|((MeU32)inID2);
}

/* works for unsigned 32-bit integer variables */

#define ORDER(_a,_b)                      \
    do {                                  \
	const MeU32 d = ((id2-id1)>>31)-1;    \
	const MeU32 o = ~d;                   \
	const MeU32 l = d&_a|o&_b;            \
    _b = o&id1|d&id2;                     \
	_a = l;                               \
    } while(0);                           

McdModelPairID MEAPI McdModelPairManagerGetPair(McdModelPairManagerID manager, McdModelID m1, McdModelID m2)
{

    MeU32 id1 = m1->mSpaceID;
    MeU32 id2 = m2->mSpaceID;

    ORDER(id1,id2)
    MCD_CHECK_ASSERT_(id1<id2,"CxSmallSort::activate");
    int key = CxSmallSortKey(id1,id2);
    McdModelPairManagerLink *link = McdModelPairManagerHashAccess(manager->hash,key);

    return link?link->pair:0;
}

MeBool MEAPI McdModelPairManagerActivate(McdModelPairManagerID manager, McdModelID m1, McdModelID m2)
{

    MeU32 id1 = m1->mSpaceID;
    MeU32 id2 = m2->mSpaceID;

    ORDER(id1,id2);

    MCD_CHECK_ASSERT_(id1<id2,"CxSmallSort::activate");
    int key = CxSmallSortKey(id1,id2);
    McdModelPairManagerLink *link = McdModelPairManagerHashAccess(manager->hash,key);


    if(link) 
    {
        if(link->pair->phase!=kMcdFFStateGoodbye)
            return 0;
        link->pair->phase = kMcdFFStateStaying;
        McdModelPairManagerLinkPhaseRemove(link);
    }
    else     
    {
        link = (McdModelPairManagerLink *)MePoolAPI.getStruct(manager->linkPool);
        if(link)
        {
            link->key = key;
            link->pair = (McdModelPairID)MePoolAPI.getStruct(manager->pairPool);
            McdModelPairReset(link->pair,m1,m2);
            link->pair->phase = kMcdFFStateHello;
            McdModelPairManagerHashInsert(manager->hash,link);
            McdModelPairManagerLinkPhaseInsert(&manager->helloList,link);
        }
        else
             (*manager->poolFullHandler)(m1,m2);
    }
    
    return 1;
}





MeBool MEAPI McdModelPairManagerDeactivate(McdModelPairManagerID manager, McdModelID m1, McdModelID m2)
{
    MeU32 id1 = m1->mSpaceID;
    MeU32 id2 = m2->mSpaceID;

    ORDER(id1,id2);
   
    MCD_CHECK_ASSERT_(id1<id2,"CxSmallSort::deactivate");
    int key = CxSmallSortKey(id1,id2);
    McdModelPairManagerLink *link = McdModelPairManagerHashAccess(manager->hash,key);

    if(link)
    {
        if(link->pair->phase!=kMcdFFStateHello && link->pair->phase!=kMcdFFStateStaying)
            return 0;

        if(link->pair->phase==kMcdFFStateHello) 
        {
            McdModelPairManagerHashRemove(manager->hash,key);
            McdModelPairManagerLinkPhaseRemove(link);
            MePoolAPI.putStruct(manager->pairPool,link->pair);
            MePoolAPI.putStruct(manager->linkPool,link);
        }
        else
        {
            McdModelPairManagerLinkPhaseInsert(&manager->goodbyeList,link);
            link->pair->phase = kMcdFFStateGoodbye;
        }
    }
    return 1;
}

void MEAPI McdModelPairManagerFlush(McdModelPairManagerID manager) 
{
    McdModelPairManagerLink *link, *nextLink;

    for(link = manager->goodbyeList.phaseNext; link!=&manager->goodbyeList; link = nextLink)
    {
        nextLink = link->phaseNext;
        McdModelPairManagerHashRemove(manager->hash,link->key);
        MePoolAPI.putStruct(manager->pairPool,link->pair);
        MePoolAPI.putStruct(manager->linkPool,link);
    }

    manager->goodbyeList.phaseNext = &manager->goodbyeList;
    manager->goodbyeList.phasePrev = &manager->goodbyeList;

    for(link = manager->helloList.phaseNext; link!=&manager->helloList; link = link->phaseNext)
        link->pair->phase = kMcdFFStateStaying;

    manager->helloList.phaseNext = &manager->helloList;
    manager->helloList.phasePrev = &manager->helloList;
}

MEPUBLIC int MEAPI McdModelPairManagerGetSize(McdModelPairManagerID manager)
{
    return MePoolAPI.getUsed(manager->pairPool) + MePoolAPI.getUnused(manager->pairPool);
}

void MEAPI McdModelPairManagerSetPoolFullHandler(McdModelPairManagerID manager,McdSpacePoolErrorFnPtr handler)
{
    manager->poolFullHandler = handler;
}


McdSpacePoolErrorFnPtr MEAPI 
McdModelPairManagerGetPoolFullHandler(McdModelPairManagerID manager)
{
    return manager->poolFullHandler;
}

