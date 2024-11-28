/*********************************************************
    CxAxisSort.cpp
    Written by Bryan Galdrikian
    (c) 1999-2001 MathEngine, Inc.
 *********************************************************/

#include "CxAxisSort.h"

#include <math.h>


CxAxisSort::CxAxisSort(const int inDim, const int inNObjects) :
    mInc(inNObjects), mRepList(inNObjects), mDynamicList(inNObjects), mFreeIDs(inNObjects),
    mPairTable(SuperDiagonalSize(inNObjects)) {
    mConjointCb = NULL;
    mProcessCb = NULL;
    mDisjointCb = NULL;
    mAABBUpdateFn = NULL;
    mUserData = NULL;
    mSpaceBuilt = false;
}

CxAxisSort::~CxAxisSort() {
}


cxAxisSort::HandleEvent(const int 


CxAxisSort *CxAxisSort::New(const int inDim, const int inNObjects) {
    if (inDim < 1 || inDim > 3 || inNObjects <= 0) { return NULL; }

    CxAxisSort *newAxisSort = new CxAxisSort(inDim,inNObjects);

    if (!newAxisSort) { return NULL; }

    const MeU32 allAxes = ((1<<inDim)-1);
    newAxisSort->mDisjointEvent = allAxes<<(allAxes<<2);
    newAxisSort->mConjointEvent = 0L;
    MeU32 axisMask = 1<<inDim;
    while ((axisMask>>=1) != 0L) {
        newAxisSort->mConjointEvent |= (axisMask<<((allAxes-axisMask)<<2));
    }
    newAxisSort->mNAxes = inDim;
    int i;
    for (i = 0; i < inDim; i++) {
        newAxisSort->mMinBound[i].mOrdinate = -MEINFINITY;
        newAxisSort->mMaxBound[i].mOrdinate = MEINFINITY;
        newAxisSort->mMinBound[i].mType = 0x80;
        newAxisSort->mMaxBound[i].mType = 0xFF;
        newAxisSort->mSort[i].InsertHead(newAxisSort->mMinBound+i);
        newAxisSort->mSort[i].InsertTail(newAxisSort->mMaxBound+i);
    }
    for (i = 0; i < inNObjects; i++) {
        newAxisSort->mFreeIDs[i] = inNObjects-1-i;
    }
    int j;
    CxAxisSortPair *pair = newAxisSort->mPairTable.GetArray();
    for (i = 0; i < inNObjects-1; i++) {
        for (j = i+1; j < inNObjects; j++) {
            (pair++)->Init(i,j);
        }
    }
    newAxisSort->mDynamicList.PadZero();

    return newAxisSort;
}

unsigned int CxAxisSort::GetSize() {
    return mRepList.GetSize();
}

unsigned int CxAxisSort::GetFree() {
    return mFreeIDs.GetSize();
}

MeI16 CxAxisSort::Insert(void * const inData) {
    if (!GetFree()) {
    }

    const MeI16 id = mFreeIDs.RemoveLast();

    CxAxisSortRep *rep = mRepList[id];

    MCD_CHECK_ASSERT_(!ValidID(rep->mID));

    rep->mID = id;
    rep->mData = inData;

    CxAxisSortMarker *marker = rep->mAABBMarkers;
    for (int i = 0; i < mNAxes; i++) {
        marker->mOrdinate = MEINFINITY;
        marker->mID = id;
        marker->mType = 0;
        marker->InsertBefore(mSort[i].Tail());
        marker++;
        marker->mOrdinate = MEINFINITY;
        marker->mID = id;
        marker->mType = 1<<i;
        marker->InsertBefore(mSort[i].Tail());
        marker++;
    }

    mDynamicList.Set(id,true);

    return id;
}

bool CxAxisSort::Remove(const MeI16 inID) {
    if (!ValidID(inID)) {
        return true;
    }

    CxAxisSortRep *rep = mRepList[inID];

    const MeU32 allAxes = ((1<<mNAxes)-1);

    CxAxisSortMarker *marker = rep->mAABBMarkers;
    for (int i = 0; i < mNAxes; i++) {
        (marker++)->Remove();
        (marker++)->Remove();
    }

    CxAxisSortPair *pair = mPairTable.GetArray()+inID-1;
    MeI16 nColsM1 = (MeI16)GetSize()-1;
    MeI16 index = 0;
    while (index++ < inID) {
        pair->Remove();
        pair->Clear();
        pair += nColsM1-index;
    }
    while (index++ < nColsM1) {
        ++pair;
        pair->Remove();
        pair->Clear();
    }

    mDynamicList.Set(inID,false);

    rep->mID = -1;
    rep->mData = NULL;

    return true;
}

bool CxAxisSort::SetDynamic(const MeI16 inID, const bool inIsDynamic) {
    if (!ValidID(inID)) {
        return false;
    }
    mDynamicList.Set(inID,inIsDynamic);
    return true;
}

bool CxAxisSort::GetDynamic(const MeI16 inID) {
    if (!ValidID(inID)) {
        return false;
    }
    return mDynamicList[inID];
}

void CxAxisSort::SetData(void * const inData) {
    mUserData = inData;
}

void *CxAxisSort::GetData() {
    return mUserData;
}

bool CxAxisSort::ValidID(const MeI16 inID) {
    return inID >= 0 && (MeU16)inID < GetSize() && mRepList[inID]->mID == inID;
}

bool CxAxisSort::SetConjointCallback(SpaceCb inFn) {
    mConjointCb = inFn;
    return true;
}

bool CxAxisSort::SetProcessCallback(SpaceCb inFn) {
    mProcessCb = inFn;
    return true;
}

bool CxAxisSort::SetDisjointCallback(SpaceCb inFn) {
    mDisjointCb = inFn;
    return true;
}

bool CxAxisSort::ProcessPairOverlaps() {
    if (!mProcessCb) { return false; }

    for (CxAxisSortPair *p = (CxAxisSortPair*)mActiveList.IterHead();
         mActiveList.Valid(p); p = p->Next()) {
        mProcessCb(GetRepData(p->GetID1()),GetRepData(p->GetID2()),&p->mAux);
    }

    return true;
}

int CxAxisSort::GetPairOverlapList(CxPair * const inPairList, const int inMaxSize) {
    if (!inPairList) { return 0; }

    int N = 0;
    CxPair *pOut = inPairList;
    CxAxisSortPair *pIn = (CxAxisSortPair*)mActiveList.IterHead();
    while (N < inMaxSize && mActiveList.Valid(pIn)) {
        pOut->data1 = GetRepData(pIn->GetID1());
        pOut->data2 = GetRepData(pIn->GetID2());
        pOut->aux = &pIn->mAux;
        pOut++;
        N++;
        pIn = pIn->Next();
    }

    return N;
}

/**************************************************
    Update functions
 **************************************************/

inline void CxAxisSort::HandleEvents() {
    // Handle new active events
    if (mConjointCb) {
        while (!mNewActiveList.Empty()) {
            CxAxisSortPair *p = (CxAxisSortPair*)mNewActiveList.UNSAFE_RemoveHead();
            mActiveList.InsertTail(p);
            p->ResetStatusChange();
            mConjointCb(mRepList[p->GetID1()]->mData,mRepList[p->GetID2()]->mData,&p->mAux);
        }
    } else {
        while (!mNewActiveList.Empty()) {
            CxAxisSortPair *p = (CxAxisSortPair*)mNewActiveList.UNSAFE_RemoveHead();
            mActiveList.InsertTail(p);
            p->ResetStatusChange();
        }
    }
    // Handle new inactive events
    if (mDisjointCb) {
        while (!mNewInactiveList.Empty()) {
            CxAxisSortPair *p = (CxAxisSortPair*)mNewInactiveList.UNSAFE_RemoveHead();
            p->ResetStatusChange();
            mDisjointCb(mRepList[p->GetID1()]->mData,mRepList[p->GetID2()]->mData,&p->mAux);
        }
    } else {
        while (!mNewInactiveList.Empty()) {
            CxAxisSortPair *p = (CxAxisSortPair*)mNewInactiveList.UNSAFE_RemoveHead();
            p->ResetStatusChange();
        }
    }
}

inline void CxAxisSort::OverlapUpdate(CxAxisSortMarker * const inMarker,
                                      CxAxisSortMarker * const inPassingMarker) {
    if (inPassingMarker->mID != inMarker->mID) {
        CxAxisSortPair *pair = mPairTable.GetArray()+
            SymSuperDiagonalIndex(inMarker->mID,inPassingMarker->mID,mRepList.GetSize());
        const MeU8 state = pair->GetState();
        const MeU8 delta = inMarker->mType^inPassingMarker->mType;
        pair->SetState(state^delta);
        MeI32 event = ((MeI32)delta)<<(state<<2);
        if (event&mDisjointEvent) {
            // if it was fully active, put it in newly inactive state
            pair->Remove();
            if (!pair->GetStatusChange()) { mNewInactiveList.InsertTail(pair); }
            pair->ToggleStatusChange();
        } else
        if (event&mConjointEvent) {
            // if it was fully inactive, put it in newly active state
            if (!pair->GetStatusChange()) { mNewActiveList.InsertTail(pair); }
            else { pair->Remove(); mActiveList.InsertTail(pair); }
            pair->ToggleStatusChange();
        }
    }
}

inline void CxAxisSort::MoveMarkerDown(CxAxisSortMarker * const inMarker,
                                       const MeReal inNewOrdinate) {
    CxAxisSortMarker *marker = inMarker->Prev();
    if (marker->mOrdinate > inNewOrdinate) {
        do {
            OverlapUpdate(inMarker,marker);
            marker = marker->Prev();
        } while (marker->mOrdinate > inNewOrdinate);
        inMarker->Remove();
        inMarker->InsertAfter(marker);
    }
    inMarker->mOrdinate = inNewOrdinate;
}

inline void CxAxisSort::MoveMarkerUp(CxAxisSortMarker * const inMarker,
                                     const MeReal inNewOrdinate) {
    CxAxisSortMarker *marker = inMarker->Next();
    if (marker->mOrdinate < inNewOrdinate) {
        do {
            OverlapUpdate(inMarker,marker);
            marker = marker->Next();
        } while (marker->mOrdinate < inNewOrdinate);
        inMarker->Remove();
        inMarker->InsertBefore(marker);
    }
    inMarker->mOrdinate = inNewOrdinate;
}

inline bool CxAxisSort::_Update(const MeI16 inID, MeReal * const inAABB) {
    MCD_CHECK_ASSERT_(ValidID(inID));

    CxAxisSortRep *rep = mRepList[inID];

    MCD_CHECK_ASSERT_(rep->mID == inID);

    // Update Markers:
    MeReal newOrdinate;
    CxAxisSortMarker *marker = rep->mAABBMarkers;
    MeReal *aabb = inAABB;
    for (int i = 0; i < mNAxes; i++) {
        newOrdinate = *aabb++;
        if (newOrdinate < marker->mOrdinate) {
            MoveMarkerDown(marker,newOrdinate);
        } else if (newOrdinate > marker->mOrdinate) {
            MoveMarkerUp(marker,newOrdinate);
        }
        marker++;
        newOrdinate = *aabb++;
        if (newOrdinate < marker->mOrdinate) {
            MoveMarkerDown(marker,newOrdinate);
        } else if (newOrdinate > marker->mOrdinate) {
            MoveMarkerUp(marker,newOrdinate);
        }
        marker++;
    }

    return true;
}

void CxAxisSort::SetAABBUpdateFn(BoundsUpdateCb inFn) {
    mAABBUpdateFn = inFn;
}

void CxAxisSort::SetBSphereUpdateFn(BoundsUpdateCb inFn) {
    // do nothing... bounding sphere is not used
}

bool CxAxisSort::Update(const MeI16 inID) {
    MCD_CHECK_ASSERT_(ValidID(inID));

    if (!mAABBUpdateFn) {
        return false;
    }

    bool result = _Update(inID,mAABBUpdateFn(GetRepData(inID)));
    HandleEvents();
    return result && ProcessPairOverlaps();
}

bool CxAxisSort::UpdateAll() {
    if (!mAABBUpdateFn) {
        return false;
    }

    // We've padded our bit array with zero, so we can safely consider all words in the array.
    MeU32 *wordPtr = mDynamicList.GetArray();
    MeU32 *stop = wordPtr+mDynamicList.GetWordSize();

    MeI16 wordID = 0;
    while (wordPtr < stop) {
        MeU32 word = *wordPtr++;
        MeI16 id = wordID;
        MCD_CHECK_ASSERT_(ValidID(id));
        while (word) {
            if (word&1) { _Update(id,mAABBUpdateFn(GetRepData(id))); }
            word >>= 1;
            id++;
        }
        wordID += 32;
    }

    HandleEvents();

    return ProcessPairOverlaps();
}

// Enable/disable pair overlaps
bool CxAxisSort::SetPairOverlapEnabled(const MeI16 inID1, const MeI16 inID2,
                                       const bool inIsEnabled) {
    return false; // not supported
}
