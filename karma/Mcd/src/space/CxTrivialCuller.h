/*********************************************************
  CxTrivialCuller.h
  Written by Bryan Galdrikian
  (c) 1999 MathEngine, inc.
 *********************************************************/

#ifndef _CxTrivialCuller_h_
#define _CxTrivialCuller_h_

#include "CxSpace.h"

/*****************************************************
  TrivialSpace
 *****************************************************/

class CxTrivialRep {
public:
  MeI16 mID;
  void *mData;
  MeReal mBSphere[4];
  CxTrivialRep() : mID(-1), mData(NULL) {}
};

DefinePtrArray(CxTrivialRepPtrArray,CxTrivialRep);

typedef void *CxTrivialPair;

DefineArray(CxTrivialPairArray,CxTrivialPair)

class CxTrivialSpace : public CxSpace {
// Enums:
private:
  enum {
    kOverlap =  0x01,
    kChange = 0x02,
  };

// Fields:
private:
  unsigned int mInc;

  CxTrivialRepPtrArray mRepList;

  MeI16 *mFreeIDs;

  SpaceCb mConjointCb;
  SpaceCb mProcessCb;
  SpaceCb mDisjointCb;

  CxTrivialPairArray mPairTable;

  BitArray mPairState;

  BoundsUpdateCb mBSphereUpdateFn;

  void *mUserData;

// Functions:
  CxTrivialSpace(const int inNObjects);

  inline void HandleEvents();

  inline void *GetRepData(const MeI16 inID);

public:
  // Factory method: "constructor" takes a dimensionality and an initial storage size.
  static CxTrivialSpace *New(const int inNObjects);

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

#endif // ifndef _CxTrivialCuller_h_
