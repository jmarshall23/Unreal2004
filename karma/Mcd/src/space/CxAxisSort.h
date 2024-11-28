/*********************************************************
    CxAxisSort.h
    Written by Bryan Galdrikian
    (c) 1999-2001 MathEngine, Inc.
 *********************************************************/

#ifndef _CxAxisSort_h_
#define _CxAxisSort_h_

#include "mesflist.h"
#include "mesfarray.h"

#include "CxSpace.h"

class CxAxisSortMarker : public Link {
public:
//  Fields:
    MeReal mOrdinate;
    MeI16 mID;
    MeU8 mType;
    MeU8 mPad; // brings this up to a nice multiple of four bytes

//  Functions:
    CxAxisSortMarker() : mOrdinate((MeReal)(0.0)), mID(-1), mType('\0') {}

    /*********************************************************
    Automatic typecasting for derivations of Link:
 *********************************************************/

    CxAxisSortMarker *Next() const { return (CxAxisSortMarker*)Link::Next(); } 
    CxAxisSortMarker *Prev() const { return (CxAxisSortMarker*)Link::Prev(); }
};

class CxAxisSortPair : public Link {
//  Fields:
private:
    // mucho compression!
    // first three bits is state, next bit is "status change" bit,
    // next 14 bits give ID #1, next 14 bits give ID #(MeReal)(2.)
    MeU32 mData;

public:
    // for external use as, e.g. a function or object pointer.
    void *mAux;

//  Functions:
public:
    ~CxAxisSortPair() { Remove(); }

    // clears state & aux
    void Init(const MeI16 inID1, const MeI16 inID2) {
        mData = (((MeU32)inID2)<<18)|(((MeU32)inID1)<<4);
        mAux = NULL;
    }

    void ToggleStatusChange() { mData ^= 8; }
    void SetStatusChange() { mData |= 8; }
    void ResetStatusChange() { mData &= ~(8L); }
    MeU8 GetStatusChange() { return (MeI8)((mData&8)>>3); }

    MeU8 GetState() { return (MeU8)(mData&0x00000007); }
    void SetState(const MeU8 inState) { mData &= 0xFFFFFFF8; mData |= inState; }
    void Clear() {
        mData &= 0xFFFFFFF0;
        mAux = NULL;
    }

    MeI16 GetID1() { return (MeI16)(mData>>4)&0x00003FFF; }
    MeI16 GetID2() { return (MeI16)(mData>>18)&0x00003FFF; }

    CxAxisSortPair *Next() const { return (CxAxisSortPair*)Link::Next(); } 
    CxAxisSortPair *Prev() const { return (CxAxisSortPair*)Link::Prev(); }
};

DefineArray(CxAxisSortPairArray,CxAxisSortPair)

class CxAxisSortRep {
public:
// Fields:
    MeI16 mID;
    void *mData;

    CxAxisSortMarker mAABBMarkers[6];

// Functions:
    CxAxisSortRep() : mID(-1), mData(NULL) {}
};

DefinePtrArray(CxAxisSortRepPtrArray,CxAxisSortRep)

class CxAxisSort : public CxSpace {
  private:
// Fields:
  MeI16 mNAxes;
  unsigned int mInc;

  MeU32 mConjointEvent;
  MeU32 mDisjointEvent;

  CxAxisSortRepPtrArray mRepList;

  BitArray mDynamicList;
  
  ShortArray mFreeIDs;

  CxAxisSortPairArray mPairTable;

  CxAxisSortMarker mMinBound[3];
  CxAxisSortMarker mMaxBound[3];

  LinkList mSort[3];

  LinkList mNewActiveList;
  LinkList mActiveList;
  LinkList mNewInactiveList;

  SpaceCb mConjointCb;
  SpaceCb mProcessCb;
  SpaceCb mDisjointCb;

  BoundsUpdateCb mAABBUpdateFn;
/*  #ifdef MCDCHECK */
/*    static bool mSpaceBuilt; */
/*  #endif */

  void *mUserData;

// Functions:
    // Standard constructor, non-public to enforce usage of New()
  CxAxisSort(const int inDim, const int inNObjects);

    // List sorting functions:
  inline void MoveMarkerDown(CxAxisSortMarker * const inMarker, const MeReal inNewOrdinate);
  inline void MoveMarkerUp(CxAxisSortMarker * const inMarker, const MeReal inNewOrdinate);
  inline void OverlapUpdate(CxAxisSortMarker * const inMarker,
                            CxAxisSortMarker * const inPassingMarker);
  inline bool _Update(const MeI16 inID, MeReal * const inAABB);
  inline void HandleEvents();

  inline void *GetRepData(const MeI16 inID) { return mRepList[inID]->mData; }

  public:
    // Factory method: "constructor" takes a dimensionality and an initial storage size.
  static CxAxisSort *New(const int inDim, const int inNObjects);

    // Destructor
  ~CxAxisSort();

    // Get storage size
  unsigned int GetSize();

    // Get number of free IDs
  unsigned int GetFree();

    // Add object.
  MeI16 Insert(void * const inData);

    // Remove object.
  bool Remove(const MeI16 inID);

    // Set user data.
  void SetData(void * const inData);

    // Retrieve user data.
  void *GetData();

    // Mark for automatic update (default = true)
  bool SetDynamic(const MeI16 inID, const bool inIsDynamic);

    // Look at mark for automatic update (default = true)
  bool GetDynamic(const MeI16 inID);

    // Set axis-aligned bounding box update function
  void SetAABBUpdateFn(BoundsUpdateCb inFn);

    // Set bounding sphere update function
  void SetBSphereUpdateFn(BoundsUpdateCb inFn);

    // Update object (calls appropriate bounds callback)
  bool Update(const MeI16 inID);

    // Update all objects (calls appropriate bounds callback)
  bool UpdateAll();

    // Validate ID #
  bool ValidID(const MeI16 inID);

    // Request overlap/process/separate callbacks.
  bool SetConjointCallback(SpaceCb inFn);
  bool SetProcessCallback(SpaceCb inFn);
  bool SetDisjointCallback(SpaceCb inFn);

    // Iterate through pair overlaps.
  bool ProcessPairOverlaps();

    // Return a list of pair overlaps.  (Return value = # returned.)
  int GetPairOverlapList(CxPair * const inPairList, const int inMaxSize);

    // Enable/disable pair overlaps
  bool SetPairOverlapEnabled(const MeI16 inID1, const MeI16 inID2, const bool inIsEnabled);
};

#endif // ifndef _CxAxisSort_h_
