/*********************************************************
CxSmallSort.h
Written by Bryan Galdrikian
(c) 1999-2001 MathEngine, Inc.
*********************************************************/

#ifndef _CxSmallSort_h_
#define _CxSmallSort_h_

#include <MeMath.h>
#include <McdModelPair.h>
#include <CxSpace.h>
#include <CxSmallSortUtils.h>
#include <McdSpace.h>
#include <McdModelPairManager.h>
#include <McdCullingTable.h>

/*********************************************************
	This file contains basic global functions and macros
 *********************************************************/

enum {
    X_axis      = 0x1,
    Y_axis      = 0x2,
    Z_axis      = 0x4,
    All_axes    = X_axis+Y_axis+Z_axis
};


class CxSmallSortMarker : public Link {
public:
    MeReal mOrdinate;             // value on axis
    class CxSmallSortRep *mRep;   // back pointer to rep
    MeU8 mType;                   // encodes low / hi
    
    CxSmallSortMarker() : mOrdinate((MeReal)(0.0)), mRep(NULL), mType('\0') {}

    CxSmallSortMarker *Next() const { return (CxSmallSortMarker*)Link::Next(); }
    CxSmallSortMarker *Prev() const { return (CxSmallSortMarker*)Link::Prev(); }
};



/* this is in a list of all reps belonging to a group */

class CxSmallSortRep: public Link 
{
public:
    MeI32 mID;
    McdModelID mModel;    // back pointer - to collision model
    bool mDynamic;
    const McdCullingTable *mCullingTable;
    MeU32          mCullingID;
    MeU32          mCullingIndex;
    
    CxSmallSortMarker mAABBMarkers[6];

    CxSmallSortRep *Next() const { return (CxSmallSortRep*)Link::Next(); }
    CxSmallSortRep *Prev() const { return (CxSmallSortRep*)Link::Prev(); }
};


class CxSmallSort
{
private:
    MeU8 mNAxes;
    MeU8 mAxes;
    McdFramework *mFramework;
    
    
    CxSmallSortRep *mRepList;
    MeI32 *mFreeIDs;         // SmallSortRepId's
    MeI32 *mReleasedIDs;     // SmallSortRepId's
    int mModelMax;           // max # of objects in space
    int mFreeIDCount;        // # of available object slots
    int mReleasedIDCount;    // # of free objects
            
    NibbleArray mPairState; // Overlap state
    
    McdModelPairManagerID mManager;

    // store +-infinity at both ends of each array
    CxSmallSortMarker mMinBound[3];
    CxSmallSortMarker mMaxBound[3];
    
    LinkList mSort[3];      // the three axis lists
    LinkList mActiveList;   // list of active rep objects
    
    McdUpdateAABBFnPtr mAABBUpdateFn;
#ifdef MCDCHECK
    bool mSpaceBuilt;
#endif
    int mChanging;
    void *mUserData;
            
    // private functions:
    // Standard constructor, non-public to enforce usage of New()
    CxSmallSort(const int inNObjects, const int inNPairs);
    
    // List sorting functions:
    void MoveStartMarkerUp(CxSmallSortMarker * const inMarker);
    void MoveStartMarkerDown(CxSmallSortMarker * const inMarker);
    void MoveEndMarkerUp(CxSmallSortMarker * const inMarker);
    void MoveEndMarkerDown(CxSmallSortMarker * const inMarker);
    
    void OverlapUpdate(CxSmallSortRep * const inRep1,
        CxSmallSortRep * const inRep2, const MeU8 delta);
    void _Update(const MeI32 inID, const MeVector3Ptr min,
        const MeVector3Ptr max);
    

public:
    // Factory methods: "constructor" takes a field of axes and
    // initial storage sizes.
    static CxSmallSort *New(McdFramework *fwk, const MeU8 inAxes,
        const int inNObjects, const int inNPairs);
    
    // Destructor
    virtual ~CxSmallSort();
    
    // Self-destruct
    void Delete();
    
    // Get storage size
    unsigned int GetSize()
    {
        return mModelMax;
    }

    unsigned int GetFree() 
    {
        return mFreeIDCount;
    }

    unsigned int GetMaxPairCount() 
    {
        return McdModelPairManagerGetSize(mManager);
    }

    // Add object with static culling parameters (this is much
    // faster than inserting and then doing a SetCullingParameters
    MeI32 Insert(const McdModelID inData, const McdCullingTable *table,
        MeU32 cullingIndex, MeU32 cullingID);
    
    // Change static culling parameters. This is an expensive operation

    void SetCullingParameters(const MeI32 inID, const McdCullingTable *table,
        const MeU32 cullingIndex, const MeU32 cullingID);

    // Remove object.
    bool Remove(const MeI32 inID);
    
    // Get object back pointer
    McdModelID GetRepData(const MeI32 inID) { return mRepList[inID].mModel;  }
    
    // Set user data.
    void SetData(void * const inData);
    
    // Retrieve user data.
    void *GetData();
    
    // Mark for automatic update (default = true)
    bool SetDynamic(const MeI32 inID, const bool inIsDynamic);
    
    // Look at mark for automatic update (default = true)
    bool GetDynamic(const MeI32 inID);
    
    void SetAABBUpdateFn(McdUpdateAABBFnPtr inFn)
    {
        mAABBUpdateFn = inFn;
    }

    void SetBSphereUpdateFn(McdUpdateSphereFnPtr inFn) {}
        
    // Update object (calls appropriate bounds callback)
    bool Update(const MeI32 inID, MeReal duration);
    
    // Update sequence: begin single end
    bool UpdateBegin();
    bool UpdateEnd();
    bool UpdateSingle(const MeI32 inID, MeReal duration);
    
    // Update all objects (calls appropriate bounds callback)
    bool UpdateAll(MeReal duration);
    
    // Validate ID #
    bool ValidID(const MeI32 inID);
    
    // Enable/disable pair overlaps
    bool SetPairOverlapEnabled(const MeI32 inID1, const MeI32 inID2, const bool inIsEnabled);
    
    // Get pair overlap enabled status
    bool GetPairOverlapEnabled(const MeI32 inID1, const MeI32 inID2);
    
    inline bool IsChanging() { return mChanging !=0; };
    // the inline keyword seems necessary for VC++, otherwise this
    // function cannot be called as a virtual function
    
    // set handler for model pair pool being full
    void SetPoolFullHandler(McdSpacePoolErrorFnPtr handler)
    {
        McdModelPairManagerSetPoolFullHandler(mManager,handler);
    }

    McdSpacePoolErrorFnPtr GetPoolFullHandler() 
    {
        return McdModelPairManagerGetPoolFullHandler(mManager);
    }

    int MEAPI getPairs(McdSpacePairIterator * iter, McdModelPairContainer* a);
    int MEAPI getTransitions(McdSpacePairIterator * iter, McdModelPairContainer* a);

#ifdef MCDCHECK
    // Set when necessary build routines run
    bool SetSpaceBuilt();
    // Check to make sure that space has run necessary build routine
    bool IsSpaceBuilt();
    
    void dumplist(int axis);
#endif // MCDCHECK
};

#endif
