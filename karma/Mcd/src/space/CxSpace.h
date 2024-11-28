/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:58 $ - Revision: $Revision: 1.2.2.1 $

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

#ifndef _CXSPACE_H
#define _CXSPACE_H


#include <MePrecision.h>
#include <McdGeometry.h>
#include <McdModelPair.h>
#include <lsVec3.h>
#include <MeCall.h>

// Callback fn. for space
typedef int (*SpaceCb)(McdModelPair *pair);

// Bounding volume update fn.
typedef void (MEAPI *McdUpdateAABBFnPtr) (McdModelID model, MeReal duration);
typedef void (MEAPI *McdUpdateSphereFnPtr) (McdModelID model, MeVector3 min, MeVector3 max);


/*****************************************************
 CxSpace (culling mechanism) - abstract base class
 *****************************************************/

class CxSpace {
public:
  // Self-destruct
  virtual void Delete() = 0;

  // Get storage size
  virtual unsigned int GetSize() = 0;

  // Get pair storage
  virtual unsigned int GetMaxPairCount() = 0;

  // Get number of free IDs
  virtual unsigned int GetFree() = 0;

  // Add object.
  virtual MeI32 Insert(const McdModelID inData) = 0;

  // Remove object.
  virtual bool Remove(const MeI32 inID) = 0;

  // Set user data.
  virtual void SetData(void * const inData) = 0;

  // Get object back pointer
  virtual McdModelID GetRepData(const MeI32 inID) = 0;

  // Retrieve user data.
  virtual void *GetData() = 0;

  // Mark for automatic update (default = true)
  virtual bool SetDynamic(const MeI32 inID, const bool inIsDynamic) = 0;

  // Look at mark for automatic update (default = true)
  virtual bool GetDynamic(const MeI32 inID) = 0;

  // Set axis-aligned bounding box update function
  virtual void SetAABBUpdateFn(McdUpdateAABBFnPtr inFn) = 0;

  // Set bounding sphere update function
  virtual void SetBSphereUpdateFn(McdUpdateSphereFnPtr inFn) = 0;

  // Update sequence: begin single end
  virtual bool UpdateBegin() = 0;
  virtual bool UpdateEnd() = 0;
  virtual bool UpdateSingle(const MeI32 inID, MeReal duration) = 0;

  // Update all objects (calls appropriate bounds callback)
  virtual bool UpdateAll(MeReal duration) = 0;

#ifdef MCDCHECK
  // Update all objects (calls appropriate bounds callback)
  virtual bool SetSpaceBuilt() = 0;

  // Update all objects (calls appropriate bounds callback)
  virtual bool IsSpaceBuilt() = 0;
#endif // MCDCHECK
  // Validate ID #
  virtual bool ValidID(const MeI32 inID) = 0;

  // Enable/disable pair overlaps
  virtual bool SetPairOverlapEnabled(const MeI32 inID1, const MeI32 inID2,
    const bool inIsEnabled) = 0;

  // Get pair overlap enabled status
  virtual bool GetPairOverlapEnabled(const MeI32 inID1, const MeI32 inID2) = 0;

  // Line-segment query (returns # in outList)
  // virtual int GetLineSegmentOverlapList(MeReal * const inOrig, MeReal * const inDest,
  //   McdLineSegIntersectResult *outList, const int inMaxListSize) = 0;

  virtual bool IsChanging() = 0;

};

typedef CxSpace *CxSpacePtr;

#endif // _CXSPACE_H
