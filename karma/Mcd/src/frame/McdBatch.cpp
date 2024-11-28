/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/18 09:34:35 $ - Revision: $Revision: 1.18.2.5 $

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

#include <McdProfile.h>

#include <McdInteractions.h>
#include <McdFrame.h>
#include <McdGeometry.h>
#include <McdGeometryInstance.h>
#include <McdCheck.h>
#include <McdModel.h>
#include <McdModelPair.h>
#include <MeMath.h>
#include <McdAggregate.h>
#include <McdSpace.h>
#include <McdBatch.h>
#include <McdContact.h>
#include <MeMessage.h>

/* It works like this:
   There are three distinct phases:
   flattening/batching: where aggregates are flattened and all tests batched
                        by geometry types to minimise i-cache misses during the
                        tests
   intersection
   unflattening/culling, where contacts are culled and written to the output array

  The process can run out of resources at any stage, but if they do the remaining
  phases still complete. If the context state is non-empty on startup, execution
  resumes at the latest stage which the pipeline stalled. Stalls happen if
  a) we are batching, and run out of McdBatchEntry structs
  b) we are intersecting, and run out of space in the atomic array (aggregates are
     assumed to have generated enough contacts if they run out of space in their
     contact arrays, and further collisions generate no contacts. This is a deliberate
     acceptance of a _bad_ case, which is presumed to be rare).
  c) we are culling, and run out of space in the output contact array
*/

/* default pipeline buffer sizes and resource chunks. These can all be made small
 * except CONTACT_BUFFER_SIZE which should be at least 256
 */

#define TM_STACK_SIZE 1000
#define MAX_ENTRY_COUNT 1000
#define CONTACT_BUFFER_SIZE 1000
#define MAX_PAIR_COUNT 1000
#define AGGREGATE_ARRAY_COUNT 16
#define AGGREGATE_ARRAY_SIZE 128
#define ATOMIC_ARRAY_SIZE 1024



typedef enum 
{
    kMcdBatchIsFlipped = 1,
    kMcdBatchIns1IsAggregated = 2,
    kMcdBatchIns2IsAggregated = 4
} McdBatchEntryAggregateFlags;


inline void McdBatchBuildGeometryData(McdBatchGeometryData *data,
                                      int type, McdGeometryInstance *ins, MeReal epsilon)
{
    data->type = type;
    data->eps = epsilon;
    data->min = ins->min;
    data->max = ins->max;
    data->geometry=ins->mGeometry;
    data->tm=ins->mTM;
}



MeBool McdBatchFlattenAggregate(McdBatchContext *context,
                                     int flags,
                                     McdGeometryInstanceID ins1, McdGeometryInstanceID ins2, 
                                     MeReal eps1, MeReal eps2)
{
    McdProfileStart("McdFlattenAggregate");

    int i;
    McdAggregate *a = (McdAggregate *)McdGeometryInstanceGetGeometry(ins1);
    McdGeometryInstanceID elementIns = ins1->child;
    MeVector3 min1, max1, min2, max2;

    McdGeometryInstanceGetAABB(ins2,min2,max2);

    int type1, type2 = McdGeometryInstanceGetGeometryType(ins2);

    for(i=0;i<a->elementCountMax;i++, elementIns = elementIns->next)
    {
        if(!a->elementTable[i].mGeometry)
            continue;

        McdGeometryInstanceGetAABB(elementIns,min1,max1);
        
        if(min1[0] > max2[0] || min1[1] > max2[1] || min1[2] > max2[2]
        || min2[0] > max1[0] || min2[1] > max1[1] || min2[2] > max1[2])
            continue;
        
        if(!elementIns->mTM)
        {
            elementIns->mTM = context->tmArray[context->nextTM];
            context->tmTrack[context->nextTM] = &elementIns->mTM;
            context->nextTM++;
            MEASSERT(context->nextTM<context->tmMaxCount);
            MEASSERT(MeMatrix4IsTM(a->elementTable[i].mRelTM,(MeReal)0.001));
            MEASSERT(MeMatrix4IsTM(ins1->mTM,(MeReal)0.001));
            MeMatrix4MultiplyMatrix(elementIns->mTM,a->elementTable[i].mRelTM,ins1->mTM);
        }
    
        type1 = McdGeometryGetType(a->elementTable[i].mGeometry);

        if(type1==kMcdGeometryTypeAggregate)
        {
            if(McdBatchFlattenAggregate(context,flags,elementIns,ins2,eps1,eps2)==0)
            {
                McdProfileEnd("McdFlattenAggregate");
                return 0;
            }
        }
                
        else if(type2==kMcdGeometryTypeAggregate)
        {
            if(McdBatchFlattenAggregate(context, flags | kMcdBatchIsFlipped | kMcdBatchIns2IsAggregated,
                ins2,elementIns,eps2,eps1)==0)
            {
                McdProfileEnd("McdFlattenAggregate");
                return 0;
            }
        }
        else 
        {
            int index;
            McdBatchEntry *entry;

            if(context->nextEntry>=context->maxEntryCount)
            {
                McdProfileEnd("McdFlattenAggregate");
                return 0;
            }

            entry = context->entryArray+context->nextEntry;

            if(!McdFrameworkGetInteractions(context->frame,type1,type2)->swap)
            {
                McdBatchBuildGeometryData(&entry->geometryData1,type1,elementIns,eps1);
                McdBatchBuildGeometryData(&entry->geometryData2,type2,ins2,eps2);
                entry->ins1 = elementIns;
                entry->ins2 = ins2;
                entry->flags = flags | kMcdBatchIns1IsAggregated;
            }
            else
            {
                McdBatchBuildGeometryData(&entry->geometryData1,type2,ins2,eps2);
                McdBatchBuildGeometryData(&entry->geometryData2,type1,elementIns,eps1);                
                entry->ins1 = ins2;
                entry->ins2 = elementIns;
                entry->flags = (flags | kMcdBatchIns2IsAggregated 
                                      | ((flags>>1)&kMcdBatchIns1IsAggregated))
                                      ^ kMcdBatchIsFlipped;
            }
            
            index = entry->geometryData1.type * context->typeCount + entry->geometryData2.type;
            entry->next = context->table[index];
            context->table[index] = entry;
            context->nextEntry++;
        }
    }

    McdProfileEnd("McdFlattenAggregate");
    return 1;
}


MeBool McdBatchFlatten(McdBatchContext *context, McdModelPairContainer *pairs)
{
    McdProfileStart("McdBatchFlatten");

    int i;

    context->nextEntry = 0;
    context->nextTM = 0;
    context->nextPool = 1;
    context->nextPairData = 0;

    for(i=0;i<context->typeCount * context->typeCount;i++) 
        context->table[i]=0;
    
    for(i=0;i<=AGGREGATE_ARRAY_COUNT;i++)
        context->pools[i].contactCount = 0;                
                                
    for(context->nextFlattenPair;
        context->nextFlattenPair < pairs->stayingEnd;
        context->nextFlattenPair++,context->nextPairData++)
    {
        if(context->nextPairData>=context->pairDataMaxCount)
        {
            McdProfileEnd("McdBatchFlatten");
            return 0;
        }

        McdBatchPairData *pd = context->pairData + context->nextPairData;
        pd->pair = pairs->array[context->nextFlattenPair];

        McdModelID model1 = pd->pair->model1;
        McdModelID model2 = pd->pair->model2;
        int type1, type2;
        McdGeometryInstanceID ins1,ins2;

        /* Don't go on if both models are frozen. */
        if(McdSpaceModelIsFrozen(model1) && McdSpaceModelIsFrozen(model2))
        {
            pd->status = kMcdBatchPairFrozen;
            continue;
        }

        ins1 = McdModelGetGeometryInstance(model1);
        ins2 = McdModelGetGeometryInstance(model2);
        type1 = McdGeometryGetType(ins1->mGeometry);
        type2 = McdGeometryGetType(ins2->mGeometry);

        MEASSERT(MeMatrix4IsTM(ins1->mTM,(MeReal)0.001));
        MEASSERT(MeMatrix4IsTM(ins2->mTM,(MeReal)0.001));        

        if(type1!=kMcdGeometryTypeAggregate)
        {
            McdBatchEntry *entry;
            int index = type1 * context->typeCount + type2;

            MEASSERT(type2!=kMcdGeometryTypeAggregate);

            if(context->nextEntry >= context->maxEntryCount)
            {
                McdProfileEnd("McdBatchFlatten");
                return 0;
            }

            entry = context->entryArray+context->nextEntry++;

            McdBatchBuildGeometryData(&entry->geometryData1,type1,ins1,
                McdModelGetContactTolerance(model1));
            McdBatchBuildGeometryData(&entry->geometryData2,type2,ins2,
                McdModelGetContactTolerance(model2));

            entry->ins1 = ins1;
            entry->ins2 = ins2;

            entry->pool = 0;
            entry->pairData = pd;
            entry->flags = 0;
            
            entry->next = context->table[index];
            context->table[index] = entry;

            pd->start = entry;
            pd->entries = 1;
            pd->done = 0;
            pd->status = kMcdBatchPairFlattened;
        }    
        else
        {
            int j, pool;
            MeReal eps1 = McdModelGetContactTolerance(model1);
            MeReal eps2 = McdModelGetContactTolerance(model2);
            int saveNextEntry = context->nextEntry;
            int saveNextFlattenPair = context->nextFlattenPair;
            int saveNextTM = context->nextTM;

            pd->start = context->entryArray + context->nextEntry;
 
            if(!McdBatchFlattenAggregate(context,0,ins1, ins2, eps1, eps2) ||
                context->nextEntry > saveNextEntry + 1 && 
                context->nextPool >= context->poolMaxCount)
            {
                /* unlink any aggregate entries from the batching table and rewind the state */
                for(j=context->nextEntry-1;j>=saveNextEntry;j--)
                {
                    McdBatchEntry *e = context->entryArray+j;
                    MEASSERT(context->table[e->geometryData1.type
                                 * context->typeCount + e->geometryData2.type]==e);
                    context->table[e->geometryData1.type
                        * context->typeCount + e->geometryData2.type] = e->next;
                }

                context->nextEntry = saveNextEntry;

                for(j=saveNextTM;j<context->nextTM;j++)
                    *context->tmTrack[j]=0;

                context->nextTM = saveNextTM;
                context->nextFlattenPair = saveNextFlattenPair;

                McdProfileEnd("McdBatchFlatten");
                return 0;
            }
 
            pd->entries = context->nextEntry - saveNextEntry;
            pd->done = 0;
            pd->status = kMcdBatchPairAggregate | (pd->entries ? kMcdBatchPairFlattened
                                                : kMcdBatchPairTested);
            pool = pd->entries > 1 ? context->nextPool++ : 0;

            for(j=0;j<pd->entries;j++)
            {
                pd->start[j].pairData = pd;
                pd->start[j].pool = pool;
            }
        }    
    }

    McdProfileEnd("McdBatchFlatten");
    return 1;
}


MeBool McdBatchIntersectBucket(McdBatchContext *context, int type1, int type2)
{
    McdProfileStart("McdBatchIntersectBucket");

    McdBatchEntry *entry;
    McdModelPair p;
    McdIntersectResult r;
    McdModel m1,m2;
    int index = type1*context->typeCount+type2;
    int ct = 0;

    McdIntersectFn fn = McdFrameworkGetInteractions(context->frame,
        type1, type2)->intersectFn;

    MEASSERT (context->table[index] != 0);

    if (fn == 0
        && !McdFrameworkGetInteractionsWarned(context->frame,type1,type2))
    {
        MeWarning(1,"No test registered between %s and %s",
            McdFrameworkGetTypeName(context->frame,type1),
            McdFrameworkGetTypeName(context->frame,type2)
        );

        McdFrameworkSetInteractionsWarned(context->frame,type1,type2,1);
    }

    while (entry = context->table[index])
    {
        McdBatchContactPool *pool = context->pools + entry->pool;
        McdBatchPairData *pd = entry->pairData;

        /* we don't stall on aggregates, just on singletons */

        if (entry->pool == 0 && pool->contactMaxCount < pool->contactCount+128)
        {
            McdProfileEnd("McdBatchIntersectBucket");
            return 0;
        }

        /* make sure everything we'll need is 16-byte aligned */
        m1.mInstance.mTM = entry->geometryData1.tm;
        m1.mInstance.mGeometry = entry->geometryData1.geometry;
        m1.mPadding = entry->geometryData1.eps;
		m1.mData = pd->pair->model1->mData;

        m2.mInstance.mTM = entry->geometryData2.tm;
        m2.mInstance.mGeometry = entry->geometryData2.geometry;
        m2.mPadding = entry->geometryData2.eps;
		m2.mData = pd->pair->model2->mData;

        m1.frame = m2.frame = context->frame;

        MEASSERTALIGNED(m1.mInstance.mTM,MeALIGNTO);
        MEASSERTALIGNED(m2.mInstance.mTM,MeALIGNTO);
        MEASSERTALIGNED(m1.mInstance.mGeometry,MeALIGNTO);
        MEASSERTALIGNED(m2.mInstance.mGeometry,MeALIGNTO);

        p.model1 = &m1;
        p.model2 = &m2;
        p.request = pd->pair->request;
        p.userData = pd->pair->userData;
        p.m_cachedData = pd->status & kMcdBatchPairAggregate
            ? 0 : pd->pair->m_cachedData;

        r.contacts = pool->contacts + pool->contactCount;
        r.contactMaxCount = pool->contactMaxCount - pool->contactCount;
        r.pair = &p;

        MEASSERT(MeMatrix4IsTM(m1.mInstance.mTM,(MeReal)0.001));
        MEASSERT(MeMatrix4IsTM(m2.mInstance.mTM,(MeReal)0.001));

        MEASSERT(McdGeometryGetType(m1.mInstance.mGeometry)==type1);
        MEASSERT(McdGeometryGetType(m2.mInstance.mGeometry)==type2);

        /* If there is a function here, call it. */
        if (fn != 0)
        {
            McdProfileStart("McdBatchIntersectBucket-fn");
            (*fn)(&p, &r);
            McdProfileEnd("McdBatchIntersectBucket-fn");
        }
        else
        {
            r.touch = 0;
            r.contactCount = 0;
        }

        pd->pair->m_cachedData = p.m_cachedData;
        entry->contacts = pool->contacts + pool->contactCount;
        entry->touch = r.touch;
        entry->contactCount = 0;
        
        if(++pd->done == pd->entries)
            pd->status = (pd->status & kMcdBatchPairAggregate)
                | kMcdBatchPairTested;

        if(entry->touch)
        {
            entry->contactCount = r.contactCount;
            pool->contactCount += r.contactCount;
            MeVector3Copy(entry->normal,r.normal);
        }

        context->table[index] = entry->next;
        ct++;
    }

    McdProfileEnd("McdBatchIntersectBucket");
    return 1;
}

static MeBool McdBatchTest(McdBatchContext *context)
{
    unsigned type1, type2;

    context->pools[0].contactCount = 0;

    for (type1 = 0; type1 < (unsigned)context->typeCount; type1++)
    {
        for (type2 = 0; type2 < (unsigned)context->typeCount; type2++)
        {
            const unsigned index = type1*context->typeCount+type2;

            if (context->table[index])
                if (McdBatchIntersectBucket(context, type1, type2) == 0)
                    return 0;
        }
    }

    return 1;
}


void McdBatchUnflattenAggregate(McdBatchContext *context,
                                       McdIntersectResult *ir,
                                       McdBatchPairData * pd)
{
    static int ct = 0;

    McdBatchEntry *entry;
    int j,contactCount;
    McdContact *contacts;

    ir->touch = 0;
    ir->contactCount = 0;
    MeVector3Set(ir->normal,0,0,0);
   
    if(pd->entries==1)
    {
        contacts = pd->start->contacts;
        contactCount = pd->start->contactCount;
    }
    else
    {
        contacts = context->pools[pd->start->pool].contacts;
        contactCount = context->pools[pd->start->pool].contactCount;
    }

    for(entry=pd->start;entry<pd->start+pd->entries;entry++)
    {
        if(entry->touch)
        {
            ir->touch = 1;
            
            for(j=0;j<entry->contactCount;j++)
            {
                // TODO: fix this for triangle lists
                if(entry->flags&kMcdBatchIsFlipped)
                {
                    MeVector3Scale(entry->contacts[j].normal,-1);
                    if(entry->flags&kMcdBatchIns1IsAggregated)
                        entry->contacts[j].element2.ptr = entry->ins1;
                    if(entry->flags&kMcdBatchIns2IsAggregated)
                        entry->contacts[j].element1.ptr = entry->ins2;
                    
                }
                else
                {
                    if(entry->flags&kMcdBatchIns1IsAggregated)
                        entry->contacts[j].element1.ptr = entry->ins1;
                    if(entry->flags&kMcdBatchIns2IsAggregated)
                        entry->contacts[j].element2.ptr = entry->ins2;
                }
            }            
            MeVector3Add(ir->normal,ir->normal,entry->normal);
        }
    }
    
    if(contactCount>0)
    {
        MeVector3Normalize(ir->normal);        
        ir->contactCount = McdContactSimplify(ir->normal,                                              
                                              contacts, contactCount, 
                                              ir->contacts, ir->pair->request->contactMaxCount,
                                              ir->pair->request->faceNormalsFirst,
                                              context->frame->mScale);
    }

}



MeBool McdBatchUnflatten(McdBatchContext *context,
                              McdIntersectResult *resultArray, int * resultCount, int resultMaxCount,
                              McdContact *contactArray, int *contactCount, int contactMaxCount)
{
    McdProfileStart("McdBatchUnflatten");

    /* first the non-aggregates */

    for(context->nextSingleUnflattenPair;
        context->nextSingleUnflattenPair < context->nextPairData;
        context->nextSingleUnflattenPair++)
    {
        McdBatchPairData *pd = context->pairData+context->nextSingleUnflattenPair;

        if(pd->status==kMcdBatchPairTested) 
        {
            McdBatchEntry *entry = pd->start;
            McdRequestID request = pd->pair->request;
            McdIntersectResult *ir;

            if(*resultCount >= resultMaxCount
                || request->contactMaxCount + *contactCount >= contactMaxCount)
            {
                McdProfileEnd("McdBatchUnflatten");
                return 0;
            }
            
            ir = resultArray+(*resultCount)++;
            ir->pair = pd->pair;
            ir->touch = 0;
            ir->contactCount = 0;
            ir->contacts = contactArray+(*contactCount);
            
            if(entry->touch)
            {
                ir->touch = 1;
                MeVector3Copy(ir->normal,entry->normal);
                
                if(McdFrameworkGetInteractions(context->frame,entry->geometryData1.type,
                                                              entry->geometryData2.type)->cull)
                {
                    ir->contactCount = McdContactSimplify(ir->normal,
                        entry->contacts, entry->contactCount, 
                        ir->contacts, request->contactMaxCount,
                        request->faceNormalsFirst, context->frame->mScale);
                }
                else
                {
                    ir->contactCount = entry->contactCount;
                    memcpy(ir->contacts,entry->contacts,ir->contactCount*sizeof(McdContact));
                }
                
                *contactCount += ir->contactCount;
            }
            pd->status = kMcdBatchPairUnflattened;
        }
    }

    /* now the aggregates */

    for(context->nextAggregateUnflattenPair;
        context->nextAggregateUnflattenPair < context->nextPairData;
        context->nextAggregateUnflattenPair++)
    {
        McdBatchPairData *pd = context->pairData+context->nextAggregateUnflattenPair;

        if(pd->status == (kMcdBatchPairTested|kMcdBatchPairAggregate))
        {
            McdRequestID request = pd->pair->request;
            McdIntersectResult *ir;
            
            if(*resultCount == resultMaxCount
                || request->contactMaxCount + *contactCount >= contactMaxCount)
            {
                McdProfileEnd("McdBatchUnflatten");
                return 0;
            }
            
            ir = resultArray+(*resultCount)++;
            ir->pair = pd->pair; /* for zero-entry aggregates */
            ir->touch = 0;
            ir->contactCount = 0;
            ir->contacts = contactArray+(*contactCount);
            
            if(pd->entries)
            {
                McdBatchUnflattenAggregate(context,ir,pd);
                *contactCount += ir->contactCount;
            }
            
            pd->status = kMcdBatchPairUnflattened|kMcdBatchPairAggregate;
        }   
    }

    McdProfileEnd("McdBatchUnflatten");
    return 1;
}


 
McdBatchContext *MEAPI
McdBatchContextCreate(McdFramework *frame)
{
    int i;

    McdBatchContext *context;
    
    context = (McdBatchContext *)MeMemoryAPI.create(sizeof(McdBatchContext));
    context->frame = frame;
    context->typeCount = context->frame->geometryRegisteredCountMax;

    context->tmArray = (MeMatrix4 *)
        MeMemoryAPI.createAligned(sizeof(MeMatrix4)*TM_STACK_SIZE,MeALIGNTO);
    context->tmTrack = (MeMatrix4Ptr **)
        MeMemoryAPI.create(sizeof(MeMatrix4Ptr *)*TM_STACK_SIZE);
    context->tmMaxCount = TM_STACK_SIZE;

    context->table = (McdBatchEntry **)MeMemoryAPI.create(sizeof(McdBatchEntry *) * 
        context->typeCount * context->typeCount); 

    context->entryArray = (McdBatchEntry *)MeMemoryAPI.create(sizeof(McdBatchEntry) * MAX_ENTRY_COUNT);
    context->maxEntryCount = MAX_ENTRY_COUNT;

    context->pairData = (McdBatchPairData *)MeMemoryAPI.create(sizeof(McdBatchPairData) * MAX_PAIR_COUNT);
    context->pairDataMaxCount = MAX_PAIR_COUNT;

    context->pools = (McdBatchContactPool *)MeMemoryAPI.create((AGGREGATE_ARRAY_COUNT+1)*sizeof(McdBatchContactPool));
    context->poolMaxCount = AGGREGATE_ARRAY_COUNT+1;
    context->pools[0].contacts = (McdContact *)MeMemoryAPI.create(ATOMIC_ARRAY_SIZE * sizeof(McdContact));
    context->pools[0].contactMaxCount = ATOMIC_ARRAY_SIZE;

    for(i=1;i<=AGGREGATE_ARRAY_COUNT;i++)
    {
        context->pools[i].contacts = (McdContact *)MeMemoryAPI.create(AGGREGATE_ARRAY_SIZE * sizeof(McdContact));
        context->pools[i].contactMaxCount = AGGREGATE_ARRAY_SIZE;
    }
    return context;
}

void MEAPI
McdBatchContextReset(McdBatchContext *context)
{
    context->state = 0;
}


void MEAPI
McdBatchContextDestroy(McdBatchContext * context)
{
    int i;
    MeMemoryAPI.destroy(context->table);
    MeMemoryAPI.destroyAligned(context->tmArray);
    MeMemoryAPI.destroy(context->tmTrack);
    MeMemoryAPI.destroy(context->entryArray);
    MeMemoryAPI.destroy(context->pairData);

    for(i=0;i<=AGGREGATE_ARRAY_COUNT;i++)
        MeMemoryAPI.destroy(context->pools[i].contacts);

    MeMemoryAPI.destroy(context->pools);
    MeMemoryAPI.destroy(context);
}

typedef enum
{
    kMcdBatchFlattenPending = 1,
    kMcdBatchTestsPending = 2,
    kMcdBatchUnflattenPending = 4,
} McdBatchStateFlags;



extern "C"
MeBool MEAPI
McdBatchIntersectEach(McdBatchContext *context,
                      McdModelPairContainer *pairs,
                      McdIntersectResult* resultArray, 
                      int *resultCount,
                      int resultMaxCount,
                      McdContact* contactArray,
                      int *contactCount,
                      int contactMaxCount)
{
    int i;

    McdProfileStart("McdBatchIntersectEach");

    *resultCount = 0;
    *contactCount = 0;

    if(!(context->state & kMcdBatchUnflattenPending))
    {
        if(!(context->state & (kMcdBatchTestsPending|kMcdBatchUnflattenPending)))
        {
            if(!context->state)
                context->nextFlattenPair = pairs->helloFirst;
        
            if(McdBatchFlatten(context,pairs))
                context->state &= ~kMcdBatchFlattenPending;
            else
                context->state |= kMcdBatchFlattenPending;
        }

        if(McdBatchTest(context))
            context->state &= ~kMcdBatchTestsPending;
        else
            context->state |= kMcdBatchTestsPending;

        context->nextSingleUnflattenPair = 0;
        context->nextAggregateUnflattenPair = 0;
    }
    
    if(McdBatchUnflatten(context,resultArray,resultCount,resultMaxCount,
        contactArray,contactCount,contactMaxCount))
        context->state &= ~kMcdBatchUnflattenPending;
    else
        context->state |= kMcdBatchUnflattenPending;


    if(!(context->state & (kMcdBatchTestsPending|kMcdBatchUnflattenPending)))
    {
        for(i=0;i<context->nextTM;i++)
                *(context->tmTrack[i])=0;
        context->nextTM = 0;

        if(!context->state)
        {
            for(i=0;i<context->nextPairData;i++)
            {
                MEASSERT(context->pairData[i].status == kMcdBatchPairFrozen ||
                    context->pairData[i].status == kMcdBatchPairUnflattened ||
                    context->pairData[i].status == (kMcdBatchPairUnflattened|kMcdBatchPairAggregate)
                    );
            }
            
            McdProfileEnd("McdBatchIntersectEach");
            return 1;
        }
    }

    McdProfileEnd("McdBatchIntersectEach");
    return 0;
}
