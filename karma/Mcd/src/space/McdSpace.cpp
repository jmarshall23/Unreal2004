/* -*-c++-*-
 *===============================================================
 * File:        McdSpace.c
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.19.2.2.4.1 $
 * $Date: 2002/05/17 09:23:10 $
 *
 *================================================================
 */


#include <stdio.h>
#include <MeMessage.h>
#include <MeProfile.h>
#include <McdCheck.h>
#include <McdMessage.h>
#include <McdCTypes.h>
#include <McdSpace.h>
#include <McdModel.h>
#include <CxSmallSort.h>
#include <McdGeometryTypes.h>

/*----------------------------------------------------------------
 * McdSpace implementation
 *----------------------------------------------------------------
 */

#ifdef MCDCHECK

#define    MCD_CHECKSPACECHANGING(space, fnName)\
if (!McdSpaceIsChanging((McdSpaceID)space))\
McdCoreError( kMcdErrorNum_SpaceNotChanging, "", fnName, __FILE__, __LINE__ );

#define    MCD_CHECKSPACENOTCHANGING(space, fnName)\
if (McdSpaceIsChanging((McdSpaceID)space))\
McdCoreError( kMcdErrorNum_SpaceIsChanging, "", fnName, __FILE__, __LINE__ );

#else

#define    MCD_CHECKSPACENOTCHANGING(space, fnName)
#define    MCD_CHECKSPACECHANGING(space, fnName)

#endif

static void MEAPI defaultPoolFullHandler(McdModelID model1, McdModelID model2)
{
#ifdef MCDCHECK
    McdCoreError(kMcdErrorNum_Mem_SpaceModelPairPool,"","defaultPoolFullHandler", __FILE__, __LINE__);
#endif
}


/**
    Creates a new McdSpace object whose models are sorted by x, y, and z axes.

    @arg axes is a bit field, where the bits McdXAxis, McdYAxis,
      and McdZAxis may be used in combination to specify which axes
      to test for model proximity.  Axes left unspecified will cause all pairs
      to be treated as if they always overlap along those axes.  Note:
      McdAllAxes is a constant equal to McdXAxis+McdYAxis+McdZAxis.
    @arg objectCount is the maximum number of objects the space may hold.
    @arg pairCount is the maximum number of overlapping pairs the space
      will handle.

      McdSpace creation functions implicitly call McdSpaceBeginChanges(),
      so that McdSpaceIsChanging() is true directly after creation,
      allowing the new McdSpace object be populated immediately via McdSpaceInsertModel().

    @warning Performs memory allocation.

    @see McdXAxis McdYAxis McdZAxis McdAllAxes

    BeginChanges() called.
*/

McdSpaceID MEAPI
McdSpaceAxisSortCreate(McdFramework *fwk, int axes, int objectCount, int pairCount)
{
  CxSmallSort* s = CxSmallSort::New(fwk,(MeU8)axes,objectCount,pairCount);

  s->SetAABBUpdateFn(McdModelUpdatePathCompatible);
  s->SetBSphereUpdateFn(McdModelGetBSphere);
  s->SetPoolFullHandler(defaultPoolFullHandler);
  
  s->UpdateBegin();

  return (McdSpaceID)s;
}

/** Destroys @a s.
 Does not destroy the McdModel objects present in @a s. They
 can be destroyed manually using McdSpaceModelIterator, McdSpaceRemove and
 McdModelDestroy. It is not required to remove all models from @a s before
 calling this function.
*/

void MEAPI
McdSpaceDestroy( McdSpaceID s )
{
    MCD_CHECKSPACE( s, "McdSpaceDestroy" );
    ((CxSmallSort*)s)->Delete();
}

/**
    Indicates that most models have been inserted. In certain
    implementations, this is the point at which internal parameters values
    will be chosen, and internal data structures optimized. It may also
    involve memory allocation.

    This call is performed implicitly the first time that
    McdSpaceEndChanges() is called, unless called explicitly before this
    point.

    Calling McdSpaceBuild explicitly does not prevent subsequent
    insertions or deletions before the first call to McdSpaceEndChanges(),
    but if considerable changes are applied between McdSpaceBuild() and
    McdSpaceEndChanges(), the running time might be slightly
    affected, depending on the implementation.

    Currently, in the only available implementation (McdAxisSort),
    this call has no effect.

*/

void MEAPI
McdSpaceBuild( McdSpaceID s )
{
  MCD_CHECKSPACE(s, "McdSpaceBuild");
#ifdef MCDCHECK
  ((CxSmallSort*)s)->SetSpaceBuilt();
#endif // MCDCHECK
}

/**
    Queries the state of @a s. Returns true between
    calls to McdSpaceBeginChanges and McdSpaceEndChanges.

 Indicates the current mode, either inside (true) or outside (false) a
 "changes" block. A "changes" block is delimited by McdSpaceBeginChanges()
 and McdSpaceEndChanges().

 All state-modification operations must occur inside a "changes" block,
 and most state-query operations can occur only outside of such blocks.

 McdSpace functions applicable
 only inside a "changes" block:
 (i.e. when McdSpaceIsChanging() is true):

 McdSpaceInsertModel()
 McdSpaceRemoveModel()
 McdSpaceUpdateModel()
 McdSpaceEnablePair()
 McdSpaceDisablePair()
 McdSpaceEndChanges()

 McdSpace functions applicable
 only outside of a "changes" block:
 (i.e. when McdSpaceIsChanging() is false):

 McdSpaceGetPairs()
 McdSpaceGetLineSegIntersections()
 McdSpaceGetLineSegFirstIntersection()
 McdSpaceBeginChanges()

 All other McdSpace functions have no restrictions due to the value of
 McdSpaceIsChanging(), i.e. they are allowed both inside and outside of
 "changes" blocks:

 McdSpaceIsChanging()
 McdSpaceFreezeModel()
 McdSpaceUnfreezeModel()
 McdSpaceModelIsFrozen()
 McdSpacePairIsEnabled()
 McdSpaceGetModelCount()
 McdSpaceModelIteratorBegin()
 McdSpaceGetModel()
 McdSpaceSetUserData()
 McdSpaceGetUserData()
 McdSpaceDestroy()

*/

MeBool MEAPI
McdSpaceIsChanging( McdSpaceID s )
{
  return ((CxSmallSort*)s)->IsChanging();
}

/**
 Inserts @a m into @a s. @a s will now keep track of the volume occupied by @a
 m and detect any close proximities with other models present in @a s.
 @m's bounding volume will be computed via McdModelGetAABB.
 Note that McdSpaceInsertModel does not imply McdSpaceUpdateModel. You must
 call the latter function explicitly ( or via McdSpaceUpdateAll ).
 Once @m is updated, the next call to McdSpaceGetPairs will report any
 close proximities between @a m and other McdModel objects present in @a
 s. They will initially appear as "hello" pairs.

 Calling McdSpaceRemoveModel() on the same model before
 McdSpaceEndChanges() is called will remove all effects of the original
 McdSpaceInsertModel() call.

 In the current version, a McdModel object can only be present in one
 McdSpace at a time. This restriction will be lifted in the next version.

*/

/* if already present in s, insert operation will be ignored,
   in debug, a warning will also be issued.
   zero return value indicates failure.. ? what kind ?, is failure
   checked for in release ?
*/


MeBool MEAPI
McdSpaceInsertModel( McdSpaceID s, McdModelID cm )
{
    MCD_CHECKSPACE(s, "McdSpaceInsertModel");
    MCD_CHECKSPACECHANGING(s,"McdSpaceInsertModel");
    MCD_CHECKMODEL(cm, "McdSpaceInsertModel");

	if (McdGeometryGetType(McdModelGetGeometry(cm))==kMcdGeometryTypeNull)
		return 0;

    if (cm->mSpace) {
        MCD_CHECK_ASSERT_(((CxSmallSort*)s)->ValidID(cm->mSpaceID), "McdSpaceInsertModel");
        return cm->mSpace == s;
    }

    unsigned int result = ((CxSmallSort *)s)->Insert(cm,0,0,0);

    if(((CxSmallSort*)s)->ValidID(result)) 
    {
      cm->mSpace = s;
      cm->mSpaceID = result;
      return 1;
    }

    return 0;
}

/** if already present in s, insert operation will be ignored,
   in debug, a warning will also be issued.
   zero return value indicates failure.. ? what kind ?, is failure
   checked for in release ?

  two models are culled immediately from the list of AABB-intersecting pairs if
  they have the same culling table and culling ID, and the bit indexed by their
  two cullingIndex values in the table is set.
*/

MeBool MEAPI
McdSpaceInsertModelWithCulling(McdSpaceID s, McdModelID cm, 
                               McdCullingTable *table, int cullingIndex,int cullingID)
{
    MCD_CHECKSPACE(s, "McdSpaceInsertModelWithCulling");
    MCD_CHECKSPACECHANGING(s,"McdSpaceInsertModelWithCulling");
    MCD_CHECKMODEL(cm, "McdSpaceInsertModelWithCulling");


    if (cm->mSpace) {
        MCD_CHECK_ASSERT_(((CxSmallSort*)s)->ValidID(cm->mSpaceID), "McdSpaceInsertModel");
        return cm->mSpace == s;
    }

    unsigned int result = ((CxSmallSort *)s)->Insert(cm,table,cullingIndex,cullingID);

    if(((CxSmallSort*)s)->ValidID(result)) 
    {
      cm->mSpace = s;
      cm->mSpaceID = result;

      return 1;
    }

    return 0;
}

/** 
    sets the culling parameters for the model. See McdSpaceInsertModelWithCulling 
*/

void MEAPI 
McdSpaceSetModelCullingParameters(McdSpaceID s, 
                                  McdModelID cm, 
                                  McdCullingTable *table, 
                                  int cullingIndex,                               
                                  int cullingID)
{
    MCD_CHECKSPACE(s, "McdSpaceSetModelCullingParameters");
    MCD_CHECKMODEL(cm, "McdSpaceSetModelCullingParameters");
    MCD_CHECKSPACECHANGING(s,"McdSpaceSetModelCullingParameters");

    ((CxSmallSort*)s)->SetCullingParameters(cm->mSpaceID,table,cullingIndex,cullingID);
}

/**
    Computes overlaps with this model in its space.
*/

void MEAPI
McdSpaceUpdateModel( McdModelID cm )
{
    MCD_CHECKMODEL(cm, "McdSpaceUpdateModel");
    MCD_CHECKSPACE(((CxSmallSort *)cm->mSpace), "McdSpaceUpdateModel");
    MCD_CHECKSPACECHANGING(cm->mSpace,"McdSpaceUpdateModel");

    ((CxSmallSort *)(cm->mSpace))->UpdateSingle(cm->mSpaceID,0);
}

/**
 Freeze the current McdSpace bounding volume data associated with @a m.
 Subsequent calls to McdSpaceUpdateAllModels(),
 will ignore @a m. Subsequent direct calls to McdSpaceUpdateModel()
 using @a m as an argument will have no effect, and a warning will be
 issued in debug mode.

 Note that McdSpaceFreezeModel() does not imply a call to
 McdSpaceUpdateModel(). If @a m's transform has changed since the last time
 McdSpaceUpdateModel() was called on it, you may want to update @a m before
 freezing it.

 If @a m is a newly inserted model, be sure that it has been explicitly updated
( it is not implicitly updated via McdSpaceInsertModel() ) before freezing
it.

 McdSpaceFreezeModel() can only be called when McdSpaceIsChanging() is true.

 Frozen models also introduces the phenomenon of frozen pairs.
 A frozen McdPair object is one in which both McdModel objects in the pair
 are frozen.
Such frozen pairs are suppressed from the
 McdSpaceGetPairs() output, and reappear only when the pair becomes unfrozen, ie when one of the models is unfrozen via McdSpaceUnfreezeModel().

 This happens when a pair, after the last "changes" block, is in either the "hello" or "staying" state, typically with one model (but not the other one)
 frozen. In the new "changes" block, the other model of the pair becomes frozen
 via McdSpaceFreezeModel(). Frozen pairs are always "staying" pairs, and
 any information computed from them, such as via McdIntersect() will be
 identical to the information previously obtained when it was last
 unfrozen. This information can be reused without recomputation, until
 the pair becomes unfrozen again.

After calling McdSpaceFreezeModel(m), subsequent calls to
McdSpaceModelIsFrozen(m) will return true. @a m's "frozen status" can be removed via McdSpaceUnFreezeModel().

@see McdSpaceUnFreezeModel, McdSpaceUpdateModel, McdSpaceModelIsFrozen,
McdSpaceGetPairs
*/


MeBool MEAPI
McdSpaceFreezeModel( McdModelID m )
{
    MCD_CHECKMODEL(m, "McdSpaceFreezeModel");
    MCD_CHECKSPACE(((CxSmallSort *)m->mSpace), "McdSpaceFreezeModel");

    return ((CxSmallSort *)m->mSpace)->SetDynamic(m->mSpaceID, false);
}

/**

 Reverses effect of McdSpaceFreezeModel().
 Subsequent calls to McdSpaceIsFrozen() using @a m will return false.
 McdSpaceUpdateAllModels() will now update @a m as well.


 If you are not using McdSpaceUpdateAllModels() in
 your "changes" block, then call McdSpaceUpdateModel() explicitly, if this
 operation is desired. ( McdSpaceUnfreezeModel() does not imply a call to
 McdSpaceUpdateModel() )

 Any frozen pairs involving @a m, will become unfrozen, and reappear in the
 output of McdSpaceGetPairs(), either as "staying" pairs or "goodbye"
 pairs, depending on the new proximity conditions.

 Subsequent calls to McdSpaceModelIsFrozen with @a m as argument will
 return false.

 McdSpaceUnfreezeModel() can only be called when McdSpaceIsChanging() is true.

 @see McdSpaceFreezeModel, McdSpaceUpdateAll, McdSpaceUpdateModel
*/

MeBool MEAPI
McdSpaceUnfreezeModel( McdModelID cm )
{
    MCD_CHECKMODEL(cm, "McdSpaceUnfreezeModel");
    MCD_CHECKSPACE(((CxSmallSort *)cm->mSpace), "McdSpaceUnfreezeModel");
    return ((CxSmallSort *)cm->mSpace)->SetDynamic(cm->mSpaceID, true);
}

/**
   Returns @a cm 's "frozen" status.
   @see McdSpaceFreezeModel, McdSpaceUnfreezeModel
*/

MeBool MEAPI
McdSpaceModelIsFrozen( McdModelID m )
{
    MCD_CHECKMODEL(m, "McdSpaceModelIsFrozen");
    MCD_CHECKSPACE(((CxSmallSort *)m->mSpace), "McdSpaceModelIsFrozen");
    return !((CxSmallSort *)m->mSpace)->GetDynamic(m->mSpaceID);
}

/**
   Update all unfrozen McdModel objects in @a s.
   Can only be called inside a "changes" block, i.e.
   when McdSpaceIsChanging() is true.
   Calls McdSpaceUpdateModel() on all non-frozen models in @a s.
*/

void MEAPI
McdSpaceUpdateAll( McdSpaceID s )
{

    MCD_CHECKSPACE(s, "McdSpaceUpdateAll");
    MCD_CHECKSPACECHANGING(s,"McdSpaceUpdateAll");

    ((CxSmallSort*)s)->UpdateAll(0);
}

#if 0
/*
   Updates new/current/old nearby pair lists only for those models.
   Every nearby pair generated involves at least one model from the list,
   but may involve one model in Space, but not on the list as well.
   Models not updated need to have been updated at least once
   so that their bounding volumes are known to Space.
   @see McdSpaceUpdate or @see McdModelUpdateInSpace
 */

void MEAPI
McdSpaceUpdateModels( McdSpaceID s, McdModelID *models, int modelCount )
{

  while (modelCount--)
    ((CxSmallSort*)s)->UpdateSingle((*models++)->mSpaceID,0);
}
#endif

/**
   Same effect as McdSpaceUpdateAll(), except that the usual AABB bounding
   volumes for each McdModel object in @a s are replaced by PathAABB
   bounding volumes.

   This function is subject to change.

   @see McdModelGetPathAABB, McdSpaceUpdateAll
*/

void MEAPI
McdSpacePathUpdateAll( McdSpaceID s, MeReal duration )
{
    MCD_CHECKSPACE(s, "McdSpacePathUpdateAll");
    MCD_CHECKSPACECHANGING(s,"McdSpacePathUpdateAll");

    MeProfileStartSection("McdSpacePathUpdate",0);

    ((CxSmallSort*)s)->SetAABBUpdateFn(McdModelUpdatePath);
    ((CxSmallSort*)s)->UpdateAll(duration);
    ((CxSmallSort*)s)->SetAABBUpdateFn(McdModelUpdatePathCompatible);
    MeProfileEndSection("McdSpacePathUpdate");
}

#if 0
/*
   Updates hello, staying, goodbye nearby pair lists only for those models
   using path swept volumes.


   @see McdSpaceUpdateModels
 */

void MEAPI
McdSpacePathUpdateModels( McdSpaceID s, McdModelID *models, int modelCount, MeReal duration )
{
  MCD_CHECKSPACECHANGING(s);
  ((CxSmallSort*)s)->SetAABBUpdateFn((BoundsUpdateCb)McdModelUpdatePathAABB);

  while (modelCount--)
    ((CxSmallSort*)s)->UpdateSingle((*models++)->mSpaceID,duration);

  //((CxSmallSort*)s)->UpdateEnd();
  ((CxSmallSort*)s)->SetAABBUpdateFn((BoundsUpdateCb)McdModelUpdateAABB_PathCompatible);
}
#endif



void MEAPI
McdSpaceSetAABBFn( McdSpaceID s, McdSpaceUpdateAABBFnPtr updateAABBFn)
{
    ((CxSmallSort*)s)->SetAABBUpdateFn(updateAABBFn);
}

/**
    Set user void* attached to space.
*/

void MEAPI
McdSpaceSetUserData( McdSpaceID s, void *data )
{
    MCD_CHECKSPACE( s, "McdSpaceSetUserData" );
    ((CxSmallSort*)s)->SetData(data);
}

/**
    Get user void* attached to space.
*/

void * MEAPI
McdSpaceGetUserData( McdSpaceID s )
{
    MCD_CHECKSPACE( s, "McdSpaceGetUserData" );
    return ((CxSmallSort*)s)->GetData();
}


/** iter not valid until this is called. This must be called before using
    McdSpaceGetPairs(). Must be called after McdSpaceUpdateAll() is called.
*/

void MEAPI
McdSpacePairIteratorBegin( McdSpaceID s, McdSpacePairIterator* iter )
{
    MCD_CHECKSPACE( s, "McdSpacePairIteratorBegin" );
    MCD_CHECKSPACENOTCHANGING(s,"McdSpacePairIteratorBegin");

    iter->count = -1;
    iter->ptr = 0;
}

/* in the case of overflow,
best to fill up maximally on goodbye's first, as it will free up
memory further down the control chain.
After goodbye filled, hello and stay are filled independently.
(only goodbye and hello share the same array block )

goodbye pair objects in output remain valid only until
next SpaceBeginChanges().
*/

/** get all pair-events since the last call to McdSpaceUpdateAll(). By
    default, this includes "goodbye" events due to models being
    removed from s via McdSpaceRemoveModel().  return value
    indicates overflow condition: call again to get remaining pairs.
*/

int               
MEAPI McdSpaceGetPairs( McdSpaceID s, McdSpacePairIterator*iter,
                            McdModelPairContainer*a )
{
    return ((CxSmallSort *)s)->getPairs(iter,a);
}

int              
MEAPI McdSpaceGetTransitions( McdSpaceID s, McdSpacePairIterator* iter,
                                McdModelPairContainer* a)
{
    return ((CxSmallSort *)s)->getTransitions(iter,a);
}

/**
    Intersects an oriented line segment with all models in the space.
    @arg space the collision space
    @arg inOrig pointer to an MeVector3 representing the first point on the line segment.
    @arg inDest pointer to an MeVector3 representing the second point on the line segment.
    @arg outList structure containing the line segment intersection data.
    @arg inMaxListSize the maximum number of intersection which will be reported.
    @see McdLineSegIntersect
*/

int MEAPI
McdSpaceGetLineSegIntersections( McdSpaceID space, MeReal* inOrig, MeReal* inDest,
                 McdLineSegIntersectResult *outList, int inMaxListSize )
{
    MCD_CHECKSPACE( space, "McdSpaceGetLineSegIntersections" );
    MCD_CHECK_ASSERT_(inOrig != NULL && inDest != NULL && outList != NULL, "McdSpaceGetLineSegIntersections");
    
    int id = 0, iHit = 0;
    
    CxSmallSort *sp = (CxSmallSort *)space;
    
    int iSpaceSize = sp->GetSize();
    int iNumOfModelsInSpace = sp->GetSize() - sp->GetFree();
    int iValidModelCount = 0;
    
    McdModelID cm = NULL;
    McdLineSegIntersectFnPtr theFn = NULL;
    
    while (iHit < inMaxListSize && id < iSpaceSize &&
        iValidModelCount < iNumOfModelsInSpace) 
    {
        if (! (sp->ValidID(id) )) { ++id; continue; }
        cm = sp->GetRepData(id);
        if ( cm ) 
        {
            if (cm->mSpaceID == -1 || cm->mSpace != space) 
            { // includes NULL case
                ++id;
                continue;
            }
            iValidModelCount++;
            theFn = McdGeometryGetLineSegIntersectFnPtr(McdModelGetGeometry(cm));
            if (theFn && (*theFn)(cm, inOrig, inDest, &outList[iHit])) 
            {
                outList[iHit].model = cm;
                ++iHit;
            }
        }
        ++id;
    }
    
    return iHit;
}


/**
    Finds first intersection of an oriented line segment with all models in the space.
    @arg space the collision space
    @arg inOrig pointer to an MeVector3 representing the first point on the line segment.
    @arg inDest pointer to an MeVector3 representing the second point on the line segment.
    @arg outList structure containing the line segment intersection data.
    @see McdLineSegIntersect
*/

int MEAPI
McdSpaceGetLineSegFirstIntersection( McdSpaceID space, MeReal* inOrig, MeReal* inDest,
                 McdLineSegIntersectResult *outResult )
{
    MCD_CHECKSPACE( space, "McdSpaceGetLineSegFirstIntersection" );
    MCD_CHECK_ASSERT_(inOrig != NULL && inDest != NULL, "McdSpaceGetLineSegFirstIntersection");
    
    CxSmallSort *sp = (CxSmallSort *)space;
    
    int iSpaceSize = sp->GetSize();
    int iNumOfModelsInSpace = sp->GetSize() - sp->GetFree();
    int iValidModelCount = 0;
    
    McdModelID cm = NULL;
    McdLineSegIntersectFnPtr theFn = NULL;
    
    McdLineSegIntersectResult tmp;
    outResult->distance = (MeReal)1.0E25;
    
    for (int i=0; i<iSpaceSize && iValidModelCount < iNumOfModelsInSpace; i++) {
        if (! (sp->ValidID(i) )) { continue; }
        cm = sp->GetRepData(i);
        if ( cm ) {
            
            if ( cm->mSpaceID == -1 || cm->mSpace != space) {
                continue;
            }
            
            iValidModelCount++;
            
            theFn = McdGeometryGetLineSegIntersectFnPtr(McdModelGetGeometry(cm));
            
            if (theFn && (*theFn)(cm, inOrig, inDest, &tmp)) {
                if (tmp.distance < outResult->distance ) {
                    outResult->distance = tmp.distance;
                    outResult->model = cm;
                    outResult->normal[0] = tmp.normal[0];
                    outResult->normal[1] = tmp.normal[1];
                    outResult->normal[2] = tmp.normal[2];
                    outResult->position[0] = tmp.position[0];
                    outResult->position[1] = tmp.position[1];
                    outResult->position[2] = tmp.position[2];
                }
            }
        }
    }
    
    return (outResult->distance<(MeReal)1.0E25);
}


/**
    Finds first intersection of an oriented line segment with all enabled models in
    the space.
    @arg space the collision space
    @arg inOrig pointer to an MeVector3 representing the first point on the line segment.
    @arg inDest pointer to an MeVector3 representing the second point on the line segment.
    @arg outList structure containing the line segment intersection data.
    @see McdLineSegIntersect
*/


int MEAPI
McdSpaceGetLineSegFirstEnabledIntersection( McdSpaceID space, MeReal* inOrig, MeReal* inDest,
                                McdLineSegIntersectEnableCallback filterCB, void * filterData,
                                McdLineSegIntersectResult *outResult )
{
    MCD_CHECKSPACE( space, "McdSpaceGetLineSegFirstEnabledIntersection" );
    MCD_CHECK_ASSERT_(inOrig != NULL && inDest != NULL, "McdSpaceGetLineSegFirstEnabledIntersection");
    
    CxSmallSort *sp = (CxSmallSort *)space;
    
    int iSpaceSize = sp->GetSize();
    int iNumOfModelsInSpace = sp->GetSize() - sp->GetFree();
    int iValidModelCount = 0;
    
    McdModelID cm = NULL;
    McdLineSegIntersectFnPtr theFn = NULL;
    
    McdLineSegIntersectResult tmp;
    outResult->distance = (MeReal)1.0E25;
    
    for (int i=0; i<iSpaceSize && iValidModelCount < iNumOfModelsInSpace; i++) {
        if (! (sp->ValidID(i) )) { continue; }
        cm = sp->GetRepData(i);
        if ( cm ) {
            
            if ( cm->mSpaceID == -1 || cm->mSpace != space) {
                continue;
            }
            
            iValidModelCount++;
            
            if ( !filterCB(cm, filterData ) ) continue;
            
            theFn = McdGeometryGetLineSegIntersectFnPtr(McdModelGetGeometry(cm));
            
            if (theFn && (*theFn)(cm, inOrig, inDest, &tmp)) {
                if (tmp.distance < outResult->distance ) {
                    outResult->distance = tmp.distance;
                    outResult->model = cm;
                    outResult->normal[0] = tmp.normal[0];
                    outResult->normal[1] = tmp.normal[1];
                    outResult->normal[2] = tmp.normal[2];
                    outResult->position[0] = tmp.position[0];
                    outResult->position[1] = tmp.position[1];
                    outResult->position[2] = tmp.position[2];
                }
            }
        }
    }
    
    return (outResult->distance<(MeReal)1.0E25);
}


/**
    Takes model out of its space.
    @return 1 if successful, 0 if not.

    Any associated "staying" pairs in that space will become "goodbye" pairs.

    By default, removed pairs are treated the same as other "goodbye" pairs after
    McdSpaceUpdateAll() is called, and appear as goodbye events via McdSpaceGetPairs().

    @see McdSpaceUpdateAll()

*/

MeBool MEAPI
McdSpaceRemoveModel( McdModelID cm )
{
    MCD_CHECKMODEL(cm, "McdSpaceRemoveModel");
    if (cm->mSpace == NULL) {
        return false;
    }

    MCD_CHECKSPACECHANGING(cm->mSpace,"McdSpaceRemoveModel");


    if (!((CxSmallSort*)cm->mSpace)->Remove(cm->mSpaceID)) {
        return false;
    }

    cm->mSpace = NULL;
    cm->mSpaceID = -1;

    return true;
}

/**
 Resume tracking of proximity between m1 and m2.
 If m1 and m2 come into close proximity after this point,
 or are in close proximity at the time that the call is made,
 a new McdPair object will be assigned to this pair, and it will appear as a "hello" pair in the next call to McdSpaceGetPairs().

 This behaviour is different from the one occuring when a frozen McdPair
 object becomes unfrozen via McdSpaceUnfreezeModel(). In this case, the
 original pair reappears either as a "staying" or "goodbye" event,
 depending on whether the pair is still in close proximity or not.

 McdSpaceEnablePair() can only be called when McdSpaceIsChanging() is true.
 Subsequent calls to McdSpacePairIsEnabled() with @a m1 and @a m2 as
 arguments will return true.


*/

MeBool MEAPI
McdSpaceEnablePair( McdModelID cm1, McdModelID cm2 )
{
    MCD_CHECKMODEL(cm1, "McdSpaceEnablePair");
    MCD_CHECKMODEL(cm2, "McdSpaceEnablePair");
    MCD_CHECKSPACE(((CxSmallSort *)cm1->mSpace), "McdSpaceEnablePair");
    MCD_CHECKSPACE(((CxSmallSort *)cm2->mSpace), "McdSpaceEnablePair");
    MCD_CHECK_ASSERT_(cm1->mSpace == cm2->mSpace, "McdSpaceEnablePair");
    MCD_CHECKSPACECHANGING(cm1->mSpace, "McdSpaceEnablePair");
    return (unsigned int)
      ((CxSmallSort *)cm1->mSpace)->SetPairOverlapEnabled(cm1->mSpaceID,cm2->mSpaceID, true);
}

/**
 Cease to track proximity between m1 and m2.
 If m1 and m2 come into close proximity after this point,
 no McdPair object will be assigned to this pair, and
 nothing will be reported about the pair in the next call to
 McdSpaceGetPairs().
 If, in the last time step, McdSpaceGetPairs() contained a "hello" or
 "staying" pair involving m1 and m2, then that pair will appear as a
 "goodbye" pair in the next call to McdSpaceGetPairs(). After that point,
 no more references to that pair will appear, regardless of whether they
 are in close proximity to each other. If McdSpaceEnablePair() is
 subsequently called on
 the same pair of models, the pair will reappear as a new "hello" McdPair
 object, if the pair is in close proximity to each other. This holds true even if McdSpaceEnablePair() is called within the
 same "changes" block that the original McdSpaceDisablePair() is called.

 This behaviour is different than the one caused by a pair becoming frozen
 via McdSpaceFreezeModel(). In that case, the frozen pair does not appear as a
 "goodbye" pair, but simply ceases to be reported until unfrozen
 again. When it does become unfrozen, the original McdPair object reappears
 as a "staying" pair, and from that point on follows the usual semantics for
 regular non-frozen pairs.

 McdSpaceDisablePair() can only be called when McdSpaceIsChanging() is true.
 Subsequent calls to McdSpacePairIsEnabled() with @a m1 and @a m2 as
 arguments will return false.
*/

MeBool MEAPI
McdSpaceDisablePair( McdModelID m1, McdModelID m2)
{
    MCD_CHECKMODEL(m1, "McdSpaceDisablePair");
    MCD_CHECKMODEL(m2, "McdSpaceDisablePair");
    MCD_CHECKSPACE(((CxSmallSort *)m1->mSpace), "McdSpaceDisablePair");
    MCD_CHECKSPACE(((CxSmallSort *)m2->mSpace), "McdSpaceDisablePair");
    MCD_CHECK_ASSERT_(m1->mSpace == m2->mSpace, "McdSpaceDisablePair");
    MCD_CHECKSPACECHANGING(m1->mSpace, "McdSpaceDisablePair");
    return (unsigned int)
      ((CxSmallSort *)m1->mSpace)->SetPairOverlapEnabled(m1->mSpaceID,m2->mSpaceID, false);
}

/**
 Returns the "enabled" status for the pair @a m1, @a m2.
 By default, pairs are enabled unless McdSpaceDisablePair() has been called.
 See McdSpaceEnablePair(), McdSpaceDisablePair().

*/

MeBool MEAPI
McdSpacePairIsEnabled( McdModelID m1, McdModelID m2 )
{
    MCD_CHECKMODEL(m1, "McdSpacePairIsEnabled");
    MCD_CHECKMODEL(m2, "McdSpacePairIsEnabled");
    MCD_CHECKSPACE(((CxSmallSort *)m1->mSpace),"McdSpacePairIsEnabled");
    MCD_CHECKSPACE(((CxSmallSort *)m2->mSpace),"McdSpacePairIsEnabled");
    MCD_CHECK_ASSERT_(m1->mSpace == m2->mSpace, "McdSpacePairIsEnabled");

    return (MeBool)
      ((CxSmallSort *)m1->mSpace)->GetPairOverlapEnabled(m1->mSpaceID,m2->mSpaceID);
}

/**

 Indicates that a new set of state-modification operations will be applied to
 @a s. State-modifying operations can only be applied inside the
 McdSpaceBeginChanges() / McdSpaceEndChanges() delimiters (the "changes"
 block), and state-query
 operations, such as McdSpaceGetPairs(), can only be performed outside
 the "changes" block, i.e. after McdSpaceEndChanges() has been called.

 McdSpaceIsChanging() can be used to determine which mode is currently in
 effect. It is an error to
 call McdSpaceBeginChanges() inside the "changes" block,
or to call
 McdSpaceEndChanges() outside the "changes" block.
 See McdSpaceIsChanging() for a complete list of restrictions on the use of
 McdSpace functions.

 The "changes" block also indicate a new step in the life-cycle of McdSpace pair-events:
 "goodbye" pairs reported in the previous call to McdSpaceGetPairs() are no
 longer valid after McdSpaceBeginChanges(), and will not appear
 in the next call to McdSpaceGetPairs();
 "hello" pairs from the previous step
 will reappear either as "staying" or "goodbye" pairs; "staying"
 pairs can reappear again as "staying" pairs or become "goodbye" pairs, if
 they are no longer in close proximity.

 @see McdSpaceGetPairs()
 @see McdSpaceEndChanges()


*/
extern "C"
void MEAPI
McdSpaceBeginChanges( McdSpaceID space )
{
    MCD_CHECKSPACE(((CxSmallSort *)space), "McdSpaceBeginChanges");
    MCD_CHECKSPACENOTCHANGING(space, "McdSpaceBeginChanges");

    ((CxSmallSort*)space)->UpdateBegin();
}



/**

 Indicates that no more state-modification operations will be applied to
 @a s, i.e. the end of the "changes" block. The user is now free to call state-query operations such as
 McdSpaceGetPairs().

 @see McdSpaceBeginChanges(), McdSpaceIsChanging()
*/

extern "C"
void MEAPI
McdSpaceEndChanges( McdSpaceID space )
{
    MCD_CHECKSPACE(((CxSmallSort *)space), "McdSpaceEndChanges");
    MCD_CHECKSPACECHANGING(space, "McdSpaceEndChanges");

    ((CxSmallSort*)space)->UpdateEnd();
}

/**
    Iteration over all models in space.
    Use McdSpaceGetModel to get successive models.
*/


extern "C"
void MEAPI
McdSpaceModelIteratorBegin( McdSpaceID, McdSpaceModelIterator *it)
{
    it->it = 0;
    it->count = 0;
}

/**
   Get the model pointed to by iterator it, and advance iterator.
   Returns 1 if the returned model is valid, 0 otherwise.
*/

extern "C"
MeBool MEAPI
McdSpaceGetModel( McdSpaceID space, McdSpaceModelIterator *it, McdModelID* model )
{
    CxSmallSort *s = (CxSmallSort *)space;
    MCD_CHECKSPACE(s, "McdSpaceEndChanges");
    *model = 0;
    int size = (int)s->GetSize();
    int freeCount = (int)s->GetFree();
    // count counts the number of models in space
    // it is the index into the space internal rep array

    if ( it->count >= size - freeCount)
       return 0;

    while ( !s->ValidID(it->it) && it->it < size)
        (it->it)++; 

    if (it->it >= size)  
        return 0;

    *model = s->GetRepData( it->it );
    it->count++;
    it->it++;
    return 1;
}

/**
   Returns the number of models in Space
*/

extern "C"
int MEAPI
McdSpaceGetModelCount( McdSpaceID space )
{
  MCD_CHECKSPACE(((CxSmallSort *)space), "McdSpaceEndChanges");
  return
    (int)(((CxSmallSort*) space)->GetSize()) - (int)(((CxSmallSort*) space)->GetFree());
}

extern "C"
void MEAPI McdSpaceSetPoolFullHandler(McdSpaceID space, McdSpacePoolErrorFnPtr handler)
{
    MCD_CHECKSPACE(((CxSmallSort *)space), "McdSpaceEndChanges");
    if(!handler)
        handler = defaultPoolFullHandler;

    ((CxSmallSort*) space)->SetPoolFullHandler(handler);
}

extern "C"
McdSpacePoolErrorFnPtr MEAPI McdSpaceGetPoolFullHandler(McdSpaceID space)
{
    MCD_CHECKSPACE(((CxSmallSort *)space), "McdSpaceEndChanges");
    return ((CxSmallSort*) space)->GetPoolFullHandler();
}


/*----------------------------------------------------------------
* constants
*---------------------------------------------------------------
*/

int McdXAxis            = X_axis;
int McdYAxis            = Y_axis;
int McdZAxis            = Z_axis;
int McdAllAxes          = All_axes;
