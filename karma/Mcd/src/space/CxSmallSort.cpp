/*********************************************************
CxSmallSort.cpp
Written by Bryan Galdrikian
(c) 1999-2001 MathEngine, Inc.
*********************************************************/

#include <CxSmallSort.h>

#include <MeMessage.h>
#include <McdMessage.h>
#include "McdCheck.h"
#include <McdModel.h>
#include <MeMemory.h>
#include <new.h>
#include <math.h>
#include <stdio.h>


#define kOverlapMask      3
#define kPairDisabled     4

/* These encode the behaviour of the far field - at least, the state change tables
   can be built from them. See the accompanying documentation for more information */

    
// Orders two integers without conditionals (swaps if a > b). Actually this isn't guaranteed
// to work right under ANSI, since the result of a right shift of an unsigned integer is undefined.

inline void Order(MeI32 &a, MeI32 &b) {
	const MeI32 o = (b-a)>>31;
	const MeI32 d = ~o;
	const MeI32 l = d&a|o&b;
	b = o&a|d&b;
	a = l;
}

// Orders two integers without conditionals (swaps if a > b). Actually this isn't guaranteed
// to work right under ANSI, since the result of a right shift of an unsigned integer is undefined.

inline void Order(MeI16 &a, MeI16 &b) {
	const MeI16 o = (b-a)>>15;
	const MeI16 d = ~o;
	const MeI16 l = d&a|o&b;
	b = o&a|d&b;
	a = l;
}

// Assumes inID1 < inID2

CxSmallSort::CxSmallSort(const int inNObjects, const int inNPairs) :
    mPairState(MeSuperDiagonalSize(inNObjects))
{
}

CxSmallSort::~CxSmallSort() {
}

void CxSmallSort::Delete() {
    /* TODO: need to goodbye all the model pairs!!! */

    MeMemoryAPI.destroy(mFreeIDs);
    MeMemoryAPI.destroy(mRepList);
    MeMemoryAPI.destroy(mReleasedIDs);
    McdModelPairManagerDestroy(mManager);
    this->~CxSmallSort();
    MeMemoryAPI.destroy(this);
}

CxSmallSort *CxSmallSort::New(McdFramework *fwk,
                              const MeU8 inAxes, 
                              const int inNObjects, 
                              const int inNPairs)
{
    int i;

    if (inAxes <= 0 || inAxes > All_axes || inNObjects <= 0 || inNPairs <= 0) 
        return NULL; 
    
    CxSmallSort *ns = (CxSmallSort *)MeMemoryAPI.create(sizeof(CxSmallSort));
    if (!ns) 
        return NULL; 

    MCD_CHECK_ASSERT_(inNObjects>0,"CxSmallSort::New");
    MCD_CHECK_ASSERT_(inNPairs>0,"CxSmallSort::New");
    MCD_CHECK_ASSERT_(inAxes>0,"CxSmallSort::New");

    new(ns) CxSmallSort(inNObjects, inNPairs);
    
    ns->mModelMax = inNObjects;
    ns->mAABBUpdateFn = NULL;
    ns->mUserData = NULL;
#ifdef MCDCHECK
    ns->mSpaceBuilt = false;
#endif

    ns->mFramework = fwk;    
    ns->mFreeIDs = (MeI32 *)MeMemoryAPI.create(inNObjects*sizeof(MeI32));
    ns->mRepList = (CxSmallSortRep *)MeMemoryAPI.create(inNObjects * sizeof(CxSmallSortRep));

    /* EPIC CHANGE 12/29/2003
     * Valgrind reported access to uninitialized elements in this array, which
     *  caused Postal 2 on Linux to crash. Explicitly initializing fixed the
     *  crashes and valgrind complaints completely.  --ryan.
     */
    memset(ns->mRepList, '\0', inNObjects * sizeof(CxSmallSortRep));

    for(i=0;i<inNObjects;i++) 
    {
        ns->mFreeIDs[i]= ns->mModelMax - 1 - i;
        ns->mRepList[i].mID = -1;
    }

    ns->mFreeIDCount = ns->mModelMax;

    ns->mReleasedIDs = (MeI32 *)MeMemoryAPI.create(inNObjects*sizeof(MeI32));
    ns->mReleasedIDCount = 0;

    ns->mAxes = inAxes;
    ns->mNAxes = 0;

    for(MeU8 axes = inAxes; axes; axes &= axes-1)
        ns->mNAxes++;
    
    for (i = 0; i < 3; i++) 
    {
        ns->mMinBound[i].mOrdinate = -MEINFINITY;
        ns->mMaxBound[i].mOrdinate = MEINFINITY;
        ns->mMinBound[i].mType = 0xFF;
        ns->mMaxBound[i].mType = 0xFF;
        ns->mSort[i].InsertHead(ns->mMinBound+i);
        ns->mSort[i].InsertTail(ns->mMaxBound+i);
    }

    ns->mManager = McdModelPairManagerCreate(inNPairs);

    ns->mChanging = 1;

    return ns;

}


MeI32 CxSmallSort::Insert(const McdModelID inData, const McdCullingTable *cullingTable, 
                          MeU32 cullingIndex, MeU32 cullingID) 
{
    int i;
    
    if(mFreeIDCount==0)
        return -1;

    const MeI32 id = mFreeIDs[--mFreeIDCount];

    CxSmallSortRep *rep = &mRepList[id];

    MCD_CHECK_ASSERT_(!ValidID(rep->mID), "CxSmallSort::Insert");

    rep->mID = id;
    rep->mModel = inData;
    rep->mCullingID = cullingID;
    rep->mCullingIndex = cullingIndex;
    rep->mCullingTable = cullingTable;

    CxSmallSortMarker *marker = rep->mAABBMarkers;

    for (i = 0; i < 3; i++)
    {
        marker->mOrdinate = MEINFINITY;
        marker->mRep = rep;
        marker->mType = 0;
        marker->InsertBefore(mSort[i].Tail());
        marker++;
        marker->mOrdinate = MEINFINITY;
        marker->mRep = rep;
        marker->mType = 1;
        marker->InsertBefore(mSort[i].Tail());
        marker++;
    }

    SetDynamic(id,true);
    
    for (i = 0; i < id; i++)
        mPairState.Set(MeSuperDiagonalIndex(i,id,mModelMax),0);
    
    for (i = id+1; i < mModelMax; i++) 
        mPairState.Set(MeSuperDiagonalIndex(id,i,mModelMax),0);
    
    return id;
}


bool CxSmallSort::Remove(const MeI32 inID) {

    if (!ValidID(inID)) {
        return true;
    }

    SetDynamic(inID,false);

    CxSmallSortRep *rep = &mRepList[inID];
    MCD_CHECK_ASSERT_(rep->mID == inID,"CxSmallSort::Remove");

    int i;

    CxSmallSortMarker *marker = rep->mAABBMarkers;


    for (i = 0; i < 3; i++) {
        (marker++)->Remove();
        (marker++)->Remove();
    }

    for (i = 0; i < inID; i++)
    {
        const MeU32 index = MeSuperDiagonalIndex(i,inID,mModelMax);
        if(mPairState[index]==mNAxes)
            McdModelPairManagerDeactivate(mManager,GetRepData(i),GetRepData(inID));
        mPairState.Set(index,0);
    }
    
    for (i = inID+1; i < mModelMax; i++) 
    {
        const MeU32 index = MeSuperDiagonalIndex(inID,i,mModelMax);
        if(mPairState[index]==mNAxes)
            McdModelPairManagerDeactivate(mManager,GetRepData(inID),GetRepData(i));
        mPairState.Set(index,0);
    }

    if(rep->mDynamic)
        rep->Remove();

    mReleasedIDs[mReleasedIDCount++] = rep->mID;

    rep->mID = -1;
    rep->mModel = NULL;
    
    return true;
}


bool CxSmallSort::SetDynamic(const MeI32 inID, const bool inIsDynamic) 
{
    if (!ValidID(inID)) 
        return false;

    if(inIsDynamic == mRepList[inID].mDynamic)
        return true;

    if(inIsDynamic)
    {
        mRepList[inID].mDynamic = true;
        mActiveList.InsertHead(&mRepList[inID]);
    }
    else
    {
        mRepList[inID].mDynamic = false;
        mRepList[inID].Remove();
    }

    return true;
}

bool CxSmallSort::GetDynamic(const MeI32 inID) 
{
    if (!ValidID(inID)) 
        return false;

    return mRepList[inID].mDynamic;
}

void CxSmallSort::SetData(void * const inData) 
{
    mUserData = inData;
}

void *CxSmallSort::GetData() {
    return mUserData;
}

bool CxSmallSort::ValidID(const MeI32 inID) {
    return inID >= 0 && inID < mModelMax && mRepList[inID].mID == inID;
}



/**************************************************
Update function
**************************************************/




// Get pair overlap enabled status
bool CxSmallSort::GetPairOverlapEnabled(const MeI32 inID1, const MeI32 inID2) {
    
    if(inID1==inID2) 
        return false;

    const MeI32 index = MeSymSuperDiagonalIndex(inID1,inID2,mModelMax);
    return 
        (mPairState[index]&kPairDisabled) == '\0';
}

bool CxSmallSort::SetPairOverlapEnabled(const MeI32 inID1, const MeI32 inID2,
                                        const bool enable) {
    if (inID1 == inID2)
        return false;

    MeI32 i1 = inID1, i2 = inID2;
    Order(i1,i2);
    const MeU32 index = MeSuperDiagonalIndex(i1,i2,mModelMax);
    MeU8 state = mPairState[index];

    if(enable)
    {
        if(state&kPairDisabled)
        {
            mPairState.Set(index,state&=~kPairDisabled);
            if(state==mNAxes)
                McdModelPairManagerActivate(mManager,GetRepData(i1),GetRepData(i2));
        }
    }
    else
    {
        if(!(state&kPairDisabled))
        {
            mPairState.Set(index,state|kPairDisabled);
            if(state==mNAxes)
                McdModelPairManagerDeactivate(mManager,GetRepData(i1),GetRepData(i2));
        }
    }
    return true;
}


int MEAPI
CxSmallSort::getPairs(McdSpacePairIterator * iter, McdModelPairContainer* a)
{
    return McdModelPairManagerGetPairs(mManager,iter,a);
}

int MEAPI
CxSmallSort::getTransitions(McdSpacePairIterator * iter, McdModelPairContainer* a)
{
    return McdModelPairManagerGetTransitions(mManager,iter,a);
}





/*

  the numer of times A and B intersect on the axis is the number of positive events -
  if we're moving A, (A_start moving down over B_end, or A_end moving up over B_start) 
  minus the number of negative events (A_start moving up over B_end, or A_end moving down 
  over B_start). In order not to break the start machine, this number must never go
  negative. To ensure this, it suffices that the start marker for a model never moves
  up over the end marker, or the end marker down over the start marker.

  To ensure this, if the new start marker is greater than the old end marker, 
  we move the new end marker first. Otherwise we move the start marker first.

  Furthermore, we keep move an end marker (but not a start marker) up over another marker 
  of equal ordinate,  and we move a start marker (but not an end marker) down over another 
  marker of equal ordinate. So if we have a bunch of markers of equal ordinate, whatever 
  order they start in and whatever order they move in, start markers we move will end
  up below end markers, and end markers we move will end up below all start markers. So
  as long as we begin with all start markers of equal ordinate below end markers,
  and we always move markers when their values change, we'll preserve that condition.
  */

void CxSmallSort::MoveStartMarkerUp(CxSmallSortMarker * const inMarker)
{
    CxSmallSortMarker *marker = inMarker->Next();
    MeI32 inID = inMarker->mRep->mID;
    const MeReal inOrdinate = inMarker->mOrdinate;
    CxSmallSortRep *inRep = inMarker->mRep;

    while(marker->mOrdinate < inOrdinate && marker->mType != 0xFF)
    {
        MCD_CHECK_ASSERT_(inID != marker->mRep->mID, "SmallSort::MoveStartMarkerUp");
        if(marker->mType==1)
        {
            CxSmallSortRep *rep = marker->mRep;
            MeI32 i1 = inID, i2 = rep->mID;
            {
                Order(i1,i2);
                const MeU32 index = MeSuperDiagonalIndex(i1,i2,mModelMax);
                const MeU8 state = mPairState[index];
                mPairState.Set(index,state-1);
                if(state==mNAxes)
                {
                    if(!inRep->mCullingTable 
                    || inRep->mCullingTable != rep->mCullingTable
                    || inRep->mCullingID != rep->mCullingID 
                    || !McdCullingTableGet(inRep->mCullingTable,inRep->mCullingIndex,rep->mCullingIndex))
                    McdModelPairManagerDeactivate(mManager,GetRepData(i1),GetRepData(i2));
                }
            }
        }
        marker = marker->Next();
    } 
    inMarker->RemoveAndInsertBefore(marker);
}

void CxSmallSort::MoveStartMarkerDown(CxSmallSortMarker * const inMarker)
{
    CxSmallSortMarker *marker = inMarker->Prev();
    MeI32 inID = inMarker->mRep->mID;
    const MeReal inOrdinate = inMarker->mOrdinate;
    CxSmallSortRep *inRep = inMarker->mRep;

    while(marker->mOrdinate >= inOrdinate && marker->mType != 0xFF)
    {
        MCD_CHECK_ASSERT_(inID != marker->mRep->mID, "SmallSort::MoveStartMarkerDown");
        if(marker->mType==1)
        {
            CxSmallSortRep *rep = marker->mRep;
            MeI32 i1 = inID, i2 = rep->mID;
            {
                Order(i1,i2);
                const MeU32 index = MeSuperDiagonalIndex(i1,i2,mModelMax);
                MeU8 state = mPairState[index];
                mPairState.Set(index,++state);
                if(state==mNAxes)
                {
                    if(!inRep->mCullingTable 
                        || inRep->mCullingTable != rep->mCullingTable 
                        || inRep->mCullingID != rep->mCullingID 
                        || !McdCullingTableGet(inRep->mCullingTable,inRep->mCullingIndex,rep->mCullingIndex))
                        
                        McdModelPairManagerActivate(mManager,GetRepData(i1),GetRepData(i2));
                }
            }

        }
        marker = marker->Prev();
    } 
    inMarker->RemoveAndInsertAfter(marker);
}

void CxSmallSort::MoveEndMarkerUp(CxSmallSortMarker * const inMarker)
{
    CxSmallSortMarker *marker = inMarker->Next();
    MeI32 inID = inMarker->mRep->mID;
    const MeReal inOrdinate = inMarker->mOrdinate;
    CxSmallSortRep *inRep = inMarker->mRep;

    while(marker->mOrdinate <= inOrdinate && marker->mType != 0xFF)
    {
        MCD_CHECK_ASSERT_(inID != marker->mRep->mID, "SmallSort::MoveEndMarkerUp");
        if(marker->mType==0)
        {
            CxSmallSortRep *rep = marker->mRep;
            MeI32 i1 = inID, i2 = rep->mID;
            {
                Order(i1,i2);
                const MeU32 index = MeSuperDiagonalIndex(i1,i2,mModelMax);
                MeU8 state = mPairState[index];
                mPairState.Set(index,++state);
                if(state==mNAxes)
                {
                    if(!inRep->mCullingTable 
                        || inRep->mCullingTable != rep->mCullingTable 
                        || inRep->mCullingID != rep->mCullingID 
                        || !McdCullingTableGet(inRep->mCullingTable,inRep->mCullingIndex,rep->mCullingIndex))
                        
                        McdModelPairManagerActivate(mManager,GetRepData(i1),GetRepData(i2));
                }
            }
        }
        marker = marker->Next();
    } 
    inMarker->RemoveAndInsertBefore(marker);
}

void CxSmallSort::MoveEndMarkerDown(CxSmallSortMarker * const inMarker)
{
    CxSmallSortMarker *marker = inMarker->Prev();
    MeI32 inID = inMarker->mRep->mID;
    const MeReal inOrdinate = inMarker->mOrdinate;
    CxSmallSortRep *inRep = inMarker->mRep;

    while(marker->mOrdinate > inOrdinate && marker->mType != 0xFF)
    {
        const MeU8 delta = marker->mType-inMarker->mType;
        MCD_CHECK_ASSERT_(inID != marker->mRep->mID, "SmallSort::MoveEndMarkerDown");
        if(marker->mType==0)
        {
            CxSmallSortRep *rep = marker->mRep;
            MeI32 i1 = inID, i2 = rep->mID;
            {
                Order(i1,i2);
                const MeU32 index = MeSuperDiagonalIndex(i1,i2,mModelMax);
                const MeU8 state = mPairState[index];
                mPairState.Set(index,state-1);
                if(state==mNAxes)
                {
                    if(!inRep->mCullingTable 
                        || inRep->mCullingTable != rep->mCullingTable 
                        || inRep->mCullingID != rep->mCullingID 
                        || !McdCullingTableGet(inRep->mCullingTable,inRep->mCullingIndex,rep->mCullingIndex))
                        
                    McdModelPairManagerDeactivate(mManager,GetRepData(i1),GetRepData(i2));
                }
            }
        }
        marker = marker->Prev();
    } 
    inMarker->RemoveAndInsertAfter(marker);
}


#if 0
void printState(int phase)
{
    switch(phase)
    {
    case Hello: puts("Hello"); break;
    case Goodbye: puts("Goodbye"); break;
    case Staying: puts("Staying"); break;
    }
}

void CxSmallSort::dumplist(int axis)
{
    puts("===================================");
    printf("List %d\n", axis);
    CxSmallSortMarker *marker = (CxSmallSortMarker *)mSort[axis].Head()->Next();
    while(marker!=mSort[axis].Tail())
    {
        printf("model %d (%s), ordinate %12.12f\n", marker->mRep->mID,
            marker->mType==0?"start":"end", marker->mOrdinate);
        marker = marker->Next();
    }
}
#endif

void CxSmallSort::_Update(const MeI32 inID, const MeVector3Ptr min, const MeVector3Ptr max) {
    MCD_CHECK_ASSERT_(ValidID(inID), "CxSmallSort::_Update");

    CxSmallSortRep *rep = &mRepList[inID];

    MCD_CHECK_ASSERT_(rep->mID == inID, "CxSmallSort::_Update");

    // Update Markers:

    for (int i = 0; i < 3; i++)
    {
        if (mAxes&(1<<i)) 
        {
            int maxDone = 0;
            CxSmallSortMarker *startm = &rep->mAABBMarkers[2*i],
                              *endm = &rep->mAABBMarkers[2*i+1];
            MeReal oldmin = startm->mOrdinate,
                   oldmax = endm->mOrdinate;

            startm->mOrdinate = min[i];
            endm->mOrdinate = max[i];

            // this guard ensures we never move the start marker past the end marker
            // by moving the end marker first if that would happen.

            if(min[i]>=oldmax) 
            {
                if (max[i] < oldmax) 
                    MoveEndMarkerDown(endm);
                else if (max[i] > oldmax) 
                    MoveEndMarkerUp(endm);
                maxDone = 1;
            }

            if (min[i] < oldmin) 
                MoveStartMarkerDown(startm);
            else if (min[i] > oldmin) 
                MoveStartMarkerUp(startm);

            if(!maxDone)
            {
                if (max[i] < oldmax) 
                    MoveEndMarkerDown(endm);
                else if (max[i] > oldmax) 
                    MoveEndMarkerUp(endm);
            }
        }
    }
}


bool CxSmallSort::UpdateSingle(const MeI32 inID,MeReal duration)
{
    MCD_CHECK_ASSERT_(ValidID(inID), "CxSmallSort::UpdateSingle");
    if (GetDynamic(inID)) 
    {
        McdModelID model = GetRepData(inID);
        MeVector3 min,max;
        (*mAABBUpdateFn)(model,duration);
        McdModelGetAABB(model,min,max);
        _Update(inID,min,max);
    }

    return true;
}



bool CxSmallSort::UpdateAll(MeReal duration) 
{
    if (!mAABBUpdateFn) 
        return false;

    MeVector3 min,max;
    CxSmallSortRep *rep;

    for(rep=(CxSmallSortRep *)mActiveList.IterHead(); mActiveList.Valid(rep); rep=rep->Next())
    {
        MCD_CHECK_ASSERT_(ValidID(rep->mID), "CxSmallSort::UpdateAll");
        (*mAABBUpdateFn)(rep->mModel,duration);
        McdModelGetAABB(rep->mModel,min,max);
        _Update(rep->mID,min,max);
    }

    return true;
}


// Update sequence: begin single end
bool CxSmallSort::UpdateBegin()
{
    McdModelPairManagerFlush(mManager);
    while(--mReleasedIDCount>=0)
        mFreeIDs[mFreeIDCount++] = mReleasedIDs[mReleasedIDCount];
    
    mReleasedIDCount = 0;
    mChanging = 1;
    return true;
}

bool CxSmallSort::UpdateEnd()
{
    mChanging = 0;
    return true;
}


void CxSmallSort::SetCullingParameters(const MeI32 inID, const McdCullingTable *table, const MeU32 cullingIndex,
                                       const MeU32 cullingID)
{
    const McdCullingTable *oldTable = mRepList[inID].mCullingTable;
    MeU32 oldCullingIndex = mRepList[inID].mCullingIndex;
    MeU32 oldCullingID = mRepList[inID].mCullingID;
    MeI16 i;

    for (i = 0; i < mModelMax; i++)
    {
        CxSmallSortRep *inRep = mRepList+i;

        if(ValidID(i) && i!=inID) 
        {
            MeBool oldCullingState = oldTable 
                && inRep->mCullingTable == oldTable 
                && inRep->mCullingID == oldCullingID 
                && McdCullingTableGet(oldTable,inRep->mCullingIndex,oldCullingIndex);
        
            MeBool newCullingState = table 
                && inRep->mCullingTable == table
                && inRep->mCullingID == cullingID 
                && McdCullingTableGet(table,inRep->mCullingIndex,cullingIndex);
            
            
            MeI32 a = i, b = inID;
            Order(a,b);

            if(newCullingState != oldCullingState 
                && mPairState[MeSuperDiagonalIndex(a,b,mModelMax)]==mNAxes)
            {
                if(oldCullingState)
                    McdModelPairManagerActivate(mManager,GetRepData(a),GetRepData(b));
                else
                    McdModelPairManagerDeactivate(mManager,GetRepData(a),GetRepData(b));
            }
        }
    }
}
        

// Set when necessary build routines run

#ifdef MCDCHECK
bool CxSmallSort::SetSpaceBuilt() 
{
    mSpaceBuilt = true;
    return this->IsSpaceBuilt();
}

// Check to make sure that space has run necessary build routine

bool CxSmallSort::IsSpaceBuilt()
{
    return mSpaceBuilt;
}
#endif // MCDCHECK

