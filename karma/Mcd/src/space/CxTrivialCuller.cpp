/*********************************************************
  CxTrivialCuller.cpp
  Written by Bryan Galdrikian
  (c) 1999 MathEngine, inc.
 *********************************************************/

#include "CxTrivialCuller.h"


CxTrivialSpace::CxTrivialSpace(const int inNObjects) :
  mInc(inNObjects), mRepList(inNObjects), mFreeIDs(inNObjects),
  mPairTable(SuperDiagonalSize(inNObjects)), mPairState(SuperDiagonalSize(inNObjects)) {
  mConjointCb = NULL;
  mProcessCb = NULL;
  mDisjointCb = NULL;
  mBSphereUpdateFn = NULL;
  mUserData = NULL;
}

inline void CxTrivialSpace::HandleEvents() {
  const int size = GetSize();
  for (int j = 1; j < size; j++) {
    if (ValidID(j)) {
      for (int i = 0; i < j; i++) {
        if (ValidID(i)) {
          int pairIndex = SuperDiagonalIndex(i,j,size);
          if (BSphereOverlap(mRepList[i]->mBSphere,mRepList[j]->mBSphere)) {
            if (!mPairState[pairIndex]) {
              if (mConjointCb) {
                mConjointCb(GetRepData(i),GetRepData(j),mPairTable.GetArray()+pairIndex);
              }
              mPairState.Set(pairIndex,true);
            }
          } else {
            if (mPairState[pairIndex]) {
              if (mDisjointCb) {
                mDisjointCb(GetRepData(i),GetRepData(j),mPairTable.GetArray()+pairIndex);
              }
              mPairState.Set(pairIndex,false);
            }
          }
        }
      }
    }
  }
}

// Factory method: "constructor" takes a dimensionality and an initial storage size,
CxTrivialSpace *CxTrivialSpace::New(const int inNObjects) {
  CxTrivialSpace *newTrivialSpace = new CxTrivialSpace(inNObjects);
  if (!newTrivialSpace) { return NULL; }
  newTrivialSpace->mFreeIDs.SetSize(inNObjects);
  int i;
  int j;
  for (i = 0; i < inNObjects; i++) {
    newTrivialSpace->mFreeIDs[i] = inNObjects-1-i;
  }
  CxTrivialPair *pair = newTrivialSpace->mPairTable.GetArray();
  for (i = 0; i < inNObjects-1; i++) {
    for (j = i+1; j < inNObjects; j++) {
      *pair++ = NULL;
    }
  }
  return newTrivialSpace;
}

// Get storage size
unsigned int CxTrivialSpace::GetSize() {
  return mRepList.GetSize();
}

unsigned int CxTrivialSpace::GetFree() {
  return mFreeIDs.GetSize();
}


// Add object.
MeI16 CxTrivialSpace::Insert(void * const inData) {
  if (!GetFree()) { return false; }
  const MeI16 id = mFreeIDs.RemoveLast();
  CxTrivialRep *rep = mRepList[id];
  MCD_CHECK_ASSERT_(!ValidID(rep->mID));
  rep->mID = id;
  rep->mData = inData;
  rep->mBSphere[0] = rep->mBSphere[1] = rep->mBSphere[2] = 0;
  rep->mBSphere[3] = MEINFINITY;
  return id;
}

// Remove object.
bool CxTrivialSpace::Remove(const MeI16 inID) {
  if (!ValidID(inID)) {
    return false;
  }
  CxTrivialRep *rep = mRepList[inID];
  rep->mID = -1;
  rep->mData = NULL;
  CxTrivialPair *pair = mPairTable.GetArray()+inID-1;
  MeI16 nColsM1 = (MeI16)GetSize()-1;
  MeI16 index = 0;
  int pairIndex = inID-1;
  while (index++ < inID) {
    *pair = NULL;
    pair += nColsM1-index;
    pairIndex += nColsM1-index;
    mPairState.Set(pairIndex,false);
  }
  while (index++ < nColsM1) {
    ++pair;
    ++pairIndex;
    *pair = NULL;
    mPairState.Set(pairIndex,false);
  }
  return true;
}

// Set user data.
void CxTrivialSpace::SetData(void * const inData) {
  mUserData = inData;
}

// Retrieve user data.
void *CxTrivialSpace::GetData() {
  return mUserData;
}

// Retrieve user data for object
inline void *CxTrivialSpace::GetRepData(const MeI16 inID) {
  MCD_CHECK_ASSERT_(ValidID(inID));
  return mRepList[inID]->mData;
}

// Mark for automatic update (default = true)
bool CxTrivialSpace::SetDynamic(const MeI16 inID, const bool inIsDynamic) {
  return ValidID(inID);
}

// Set axis-aligned bounding box update function
void CxTrivialSpace::SetAABBUpdateFn(BoundsUpdateCb inFn) {
  // do nothing... AABB is not used
}

// Set bounding sphere update function
void CxTrivialSpace::SetBSphereUpdateFn(BoundsUpdateCb inFn) {
  mBSphereUpdateFn = inFn;
}

// Update object (calls appropriate bounds callback)
bool CxTrivialSpace::Update(const MeI16 inID) {
  if (!mBSphereUpdateFn || !ValidID(inID)) {
    return false;
  }

  CxTrivialRep *rep = mRepList[inID];
  const MeReal *bSphere = mBSphereUpdateFn(GetRepData(inID));
  MCD_CHECK_ASSERT_(bSphere != NULL);

  rep->mBSphere[0] = bSphere[0];
  rep->mBSphere[1] = bSphere[1];
  rep->mBSphere[2] = bSphere[2];
  rep->mBSphere[3] = bSphere[3];

  HandleEvents();

  return ProcessPairOverlaps();
}

// Update all objects (calls appropriate bounds callback)
bool CxTrivialSpace::UpdateAll() {
  if (!mBSphereUpdateFn) {
    return false;
  }

  const int size = GetSize();
  for (MeI16 i = 0; i < size; i++) {
    if (ValidID(i)) {
      CxTrivialRep *rep = mRepList[i];
      const MeReal *bSphere = mBSphereUpdateFn(GetRepData(i));
      MCD_CHECK_ASSERT_(bSphere != NULL);
      rep->mBSphere[0] = bSphere[0];
      rep->mBSphere[1] = bSphere[1];
      rep->mBSphere[2] = bSphere[2];
      rep->mBSphere[3] = bSphere[3];
    }
  }
  HandleEvents();
  return ProcessPairOverlaps();
}

// Validate ID #
bool CxTrivialSpace::ValidID(const MeI16 inID) {
  return inID >= 0 && inID < GetSize() && mRepList[inID]->mID == inID;
}

// Request overlap/process/separate callbacks.
bool CxTrivialSpace::SetConjointCallback(SpaceCb inFn) {
  mConjointCb = inFn;
  return true;
}

bool CxTrivialSpace::SetProcessCallback(SpaceCb inFn) {
  mProcessCb = inFn;
  return true;
}

bool CxTrivialSpace::SetDisjointCallback(SpaceCb inFn) {
  mDisjointCb = inFn;
  return true;
}

// Iterate through pair overlaps.
bool CxTrivialSpace::ProcessPairOverlaps() {
  if (!mProcessCb) { return false; }
  const int size = GetSize();
  for (int j = 1; j < size; j++) {
    if (ValidID(j)) {
      for (int i = 0; i < j; i++) {
        if (ValidID(i)) {
          int pairIndex = SuperDiagonalIndex(i,j,size);
          if (mPairState[pairIndex]) {
            mProcessCb(GetRepData(i),GetRepData(j),
                   mPairTable.GetArray()+pairIndex);
          }
        }
      }
    }
  }
  return true;
}

// Return a list of pair overlaps.  (Return value = # returned.)
int CxTrivialSpace::GetPairOverlapList(CxPair * const inPairList, const int inMaxSize) {
  if (!inPairList || inMaxSize <= 0) { return 0; }
  int N = 0;
  CxPair *pOut = inPairList;
  const int size = GetSize();
  for (int j = 1; j < size; j++) {
    if (ValidID(j)) {
      for (int i = 0; i < j; i++) {
        if (ValidID(i)) {
          if (BSphereOverlap(mRepList[i]->mBSphere,mRepList[j]->mBSphere)) {
            int pairIndex = SuperDiagonalIndex(i,j,size);
            pOut->data1 = GetRepData(i);
            pOut->data2 = GetRepData(j);
            pOut->aux = mPairTable.GetArray()+pairIndex;
            pOut++;
            if (++N == inMaxSize) { return N; }
          }
        }
      }
    }
  }

  return N;
}

// Enable/disable pair overlaps
bool CxTrivialSpace::SetPairOverlapEnabled(const MeI16 inID1, const MeI16 inID2,
                      const bool inIsEnabled) {
  return false; // not supported
}
