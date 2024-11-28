/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/05/17 09:23:10 $ - Revision: $Revision: 1.88.2.6.4.1 $

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

#include <McdModel.h>
#include <MePool.h>
#include <McdCheck.h>
#include <MeMath.h>
#include <McdGeometry.h>
#include <McdInteractionTable.h>
#include <McdGeometryTypes.h>
#include <McdGeometryInstance.h>


/*---------------------------------------------------------------
* McdModel implementation
*----------------------------------------------------------------
*/

/**
Create a new collision model with geometry @a g.
You must explicitly allocate a transformation matrix for the newly created
object before it can be used. This is set using either
McdModelSetTransformPtr() or McdModelSetRelativeTransformPtrs().
*/

extern "C"
McdModelID MEAPI
McdModelCreate(McdGeometryID g )
{
#ifdef MCDCHECK
    if( !McdGeometryIsValid( g ) )
    {
        McdCoreError(kMcdErrorNum_GenericFatal, "attempt to create an McdModel from an invalid McdGeometry object. ( Use of unregistered geometry type?)", "McdModelCreate", __FILE__, __LINE__ );
        return 0;
    }
#endif
    McdFramework *frame = McdGeometryGetFramework(g);    
	McdModel *obj = (McdModel *)MePoolAPI.getStruct(&frame->modelPool);
    
    MCDCHECK_ALLOC(obj, "McdModel", "McdModelCreate"); // doesn't seem to produce a warning!

    if(obj)
    {
        frame->modelCount++;
        McdModelReset(obj);
        obj->mPadding = frame->mDefaultPadding;
        obj->frame = frame;
        McdGeometryInstanceSetGeometry(&obj->mInstance,g);
        
        if(!frame->firstModel)
        {
            obj->next = obj->prev = obj;
        }
        else
        {
            obj->prev = frame->firstModel->prev;
            obj->next = frame->firstModel;

            obj->prev->next = obj;
            obj->next->prev = obj;
        }

        frame->firstModel = obj;
    }
    
    return (McdModelID)obj;
}

/**
Destroy @a m. @a m's associated McdGeometry object is not automatically
destroyed, since it might be shared by other McdModel objects. It must be
destroyed explicitly, if desired.
*/
extern "C"
void MEAPI
McdModelDestroy( McdModelID cm )
{
    McdFramework *frame;

    MCD_CHECKMODEL(cm, "McdModelDestroy");

    if(cm->flags & kMcdModelOwnRelativeTransforms)
    {
        MEASSERT(cm->mRelTM);
        MeMemoryAPI.destroyAligned(cm->mRelTM);
        MeMemoryAPI.destroyAligned(McdGeometryInstanceGetTransformPtr(&cm->mInstance));
        cm->flags &= ~kMcdModelOwnRelativeTransforms;
    }

    McdGeometryInstanceSetGeometry(&cm->mInstance,0);

    frame = cm->frame;

    if(frame->firstModel==cm)
        frame->firstModel = cm->next==cm ? 0 : cm->next;

    /* harmless if this is the only link */
    cm->next->prev = cm->prev;
    cm->prev->next = cm->next;

	MePoolAPI.putStruct(&cm->frame->modelPool, cm);
    frame->modelCount--;
}


extern "C"
void MEAPI
McdModelReset(McdModelID obj)
{
    obj->mRelTM = 0;
    obj->mRefTM = 0;
    obj->mBody = 0;
    obj->mRequestID = 0;
    obj->mSpace = 0;
    obj->mSpaceID = -1;
    obj->mData = 0;
    obj->linearVelocity = 0;
    obj->angularVelocity = 0;
    obj->mTransformUpdateFn = 0;
    obj->sortKey = 0;
    obj->mIntersectFn = 0;
    obj->flags = 0;

    McdGeometryInstanceReset(&obj->mInstance);
}

/**
Returns model's geometry type.
*/

extern "C"
McdGeometryType MEAPI
McdModelGetGeometryType( McdModelID cm)
{
    MCD_CHECKMODEL(cm, "McdModelGetGeometryType");
    MCD_CHECKGEOMETRY(cm, "McdModelGetGeometryType");

    return McdGeometryGetType(cm->mInstance.mGeometry);
}

/**
Returns model's collision space. NULL if model is not in a space.
*/

extern "C"
McdSpaceID MEAPI
McdModelGetSpace(McdModelID cm)
{
    MCD_CHECKMODEL(cm, "McdModelSetContactTolerance");
	return cm->mSpace;
}

/**
Sets @a m's contact tolerance to @a tol. Used for geometric intersection tests.
*/
extern "C"
void MEAPI
McdModelSetContactTolerance( McdModelID m , MeReal tol )
{
    MCD_CHECKMODEL(m, "McdModelSetContactTolerance");
    m->mPadding = tol;
}

/**
Returns @a m's contact tolerance.
*/
extern "C"
MeReal MEAPI
McdModelGetContactTolerance( McdModelID m )
{
    MCD_CHECKMODEL(m, "McdModelGetContactTolerance");
    return m->mPadding;
}

/**
Returns the minimal-volume sphere which completely bounds the volume
occupied by @a m.
*/
extern "C"
void MEAPI
McdModelGetBSphere( McdModelID m, MeVector3 center, MeReal *radius )
{
    MCD_CHECKMODEL(m, "McdModelGetBSphere");
    MCD_CHECKGEOMETRY(m, "McdModelGetBSphere");
    McdGeometryInstanceGetBSphere(&m->mInstance,center,radius);
}

/**
Indicates that the transformation, velocities, etc... have changed.
Called automatically by McdSpaceUpdate.
*/
extern "C"
void MEAPI
McdModelUpdate( McdModelID cm )
{
    MCD_CHECKMODEL(cm, "McdModelUpdate");
    MCD_CHECKGEOMETRY(cm, "McdModelUpdate");
    MCD_CHECKTRANSFORM(cm->mInstance.mTM,"McdModelUpdate");

    // update transformation matrix if it exists

    if (cm->mTransformUpdateFn) {
        (*(cm->mTransformUpdateFn))(cm);
    }

    if( cm->mRelTM )
        McdModelCompoundTransforms(cm);

    McdGeometryInstanceUpdateAABB(&cm->mInstance,0,0);

    cm->mInstance.min[0] -= cm->mPadding;
    cm->mInstance.min[1] -= cm->mPadding;
    cm->mInstance.min[2] -= cm->mPadding;
    cm->mInstance.max[0] += cm->mPadding;
    cm->mInstance.max[1] += cm->mPadding;
    cm->mInstance.max[2] += cm->mPadding;
}


/**
Computes and caches the axis-aligned bounding box which contains the model's swept
volume as it moves kinematically along the path given by the linear
and angular velocities starting from the position given by the transform.

  This function is subject to change.
*/

extern "C"
void MEAPI
McdModelUpdatePath( McdModelID cm, MeReal motionDuration )
{
    MCD_CHECKMODEL(cm, "McdModelUpdate");
    MCD_CHECKGEOMETRY(cm, "McdModelUpdate");
    MCD_CHECKTRANSFORM(cm->mInstance.mTM,"McdModelUpdate");
    
    // update transformation matrix if it exists

    if (cm->mTransformUpdateFn) 
        (*(cm->mTransformUpdateFn))(cm);

    if( cm->mRelTM )
        McdModelCompoundTransforms(cm);

    MeMatrix4 tmEnd;
    MeMatrix4Ptr tmEndPtr = 0;

    if (cm->linearVelocity && cm->angularVelocity)
    {
        const MeReal EPS = (MeReal)1e-4;

        MeMatrix4TMUpdateFromVelocities(tmEnd, EPS, motionDuration,
            (const MeReal *)cm->linearVelocity, (const MeReal *)cm->angularVelocity, cm->mInstance.mTM);

        tmEndPtr = tmEnd;

    }
    McdGeometryInstanceUpdateAABB(&cm->mInstance, tmEndPtr, 0);
}


/** @internal */

extern "C"
void MEAPI
McdModelUpdatePathCompatible( McdModelID cm, MeReal )
{
    McdModelUpdate(cm);
}

/**
Returns the cached axis-aligned bounding box which contains the model's geometry.
*/
extern "C"
void MEAPI
McdModelGetAABB( McdModelID cm, MeVector3 minCorner, MeVector3 maxCorner )
{
    MCD_CHECKMODEL(cm, "McdModelGetAABB");
    McdGeometryInstanceGetAABB(&cm->mInstance, minCorner,maxCorner);
}


/**
Returns the pointer to the model's GeometryInstance structure
*/

McdGeometryInstanceID
                  MEAPI McdModelGetGeometryInstance(McdModelID cm)
{
    MCD_CHECKMODEL(cm, "McdModelGetGeometryInstance");
    return &cm->mInstance;
}


/**
Returns the pointer to the geometry's 4X4 transformation matrix.
This transform determines the global coordinates of the
model's geometry, which is specified locally.

  If cm has been used in McdDtBridgeSetBody(..), then its transform
  pointer will be pointed to the MdtBody's transform.

    The returned address is intended for reading only, not writing. The
    validity of the return value is guaranteed only during the current time
    step.
*/
extern "C"
MeMatrix4Ptr MEAPI
McdModelGetTransformPtr( McdModelID cm )
{
    MCD_CHECKMODEL(cm, "McdModelGetTransformPtr");
    return McdGeometryInstanceGetTransformPtr(&cm->mInstance);
}

/**
Sets the geometry's 4X4 transformation matrix.
User must allocate his/her own transform for model's geometry,
and pass a pointer in with this function. This transform determines
the global coordinates of the model's geometry, which is specified
locally.

  If cm has been used in McdModelSetBody(..), then its transform
  pointer will be pointed to the MdtBody's transform. If
  McdModelSetTransformPtr() is called after this point, a warning will be
  issued.
*/


extern "C"
void MEAPI
McdModelSetTransformPtr( const McdModelID cm, const MeMatrix4Ptr geometryTM )
{
    MCD_CHECKMODEL(cm, "McdModelSetTransformPtr");
    MEASSERTALIGNED(geometryTM,MeALIGNTO);
    McdGeometryInstanceSetTransformPtr(&cm->mInstance, geometryTM);
}

/**
Set a pointer to the models linear velocity vector.
This is used in McdModelGetPathAABB and in McdSafeTime.
*/
extern "C"
void MEAPI
McdModelSetLinearVelocityPtr( McdModelID cm, MeReal * linVelocity )
{
    MCD_CHECKMODEL(cm, "McdModelSetLinearVelocityPtr");
    cm->linearVelocity = linVelocity;
}

/**
Get the pointer to the models linear velocity vector.
See @McdModelSetLinearVelocityPtr
*/
extern "C"
MeReal* MEAPI
McdModelGetLinearVelocityPtr( McdModelID cm )
{
    MCD_CHECKMODEL(cm, "McdModelGetLinearVelocityPtr");
    return cm->linearVelocity;
}

/**
Set a pointer to the models angular velocity vector.
This is used in McdModelGetPathAABB and in McdSafeTime.
*/
extern "C"
void MEAPI
McdModelSetAngularVelocityPtr( McdModelID cm, MeReal * angVelocity)
{
    MCD_CHECKMODEL(cm, "McdModelSetAngularVelocityPtr");
    cm->angularVelocity = angVelocity;
}

/**
Get the pointer to the models angular velocity vector.
See @McdModelSetAngularVelocityPtr
*/
extern "C"
MeReal* MEAPI
McdModelGetAngularVelocityPtr( McdModelID cm )
{
    MCD_CHECKMODEL(cm, "McdModelGetAngularVelocityPtr");
    return cm->angularVelocity;
}


/**
Sets the user data to be attached to the model.
*/
extern "C"
void MEAPI
McdModelSetUserData( McdModelID cm, void *data )
{
    MCD_CHECKMODEL(cm, "McdModelSetUserData");
    cm->mData = data;
}

/**
Gets the user data attached to the model.
*/
extern "C"
void * MEAPI
McdModelGetUserData( McdModelID cm )
{
    MCD_CHECKMODEL(cm, "McdModelGetUserData");
    return cm->mData;
}

/**
Re-sets a model's geometry to a new one.
This might allocate memory if the geometry needs precomputation
for collision optimization.
*/
extern "C"
void MEAPI
McdModelSetGeometry( McdModelID cm, McdGeometryID g )
{
    MCD_CHECKMODEL(cm, "McdModelSetGeometry");
	if ((McdGeometryGetType(g)==kMcdGeometryTypeNull)&&(McdModelGetSpace(cm)!=(McdSpaceID)0))
	{
		MeFatalError(MeFatalErrorLevel,"Please Remove your model from the space before changinge geometry to Null");
	}

    McdGeometryInstanceSetGeometry(&cm->mInstance,g);
}

/**
Returns a model's geometry.
*/
extern "C"
McdGeometryID MEAPI
McdModelGetGeometry( McdModelID cm)
{
    MCD_CHECKMODEL(cm, "McdModelGetGeometry" );
    return McdGeometryInstanceGetGeometry(&cm->mInstance);
}


/** @internal */
void* MEAPI
McdModelGetBodyData( McdModelID cm)
{
    MCD_CHECKMODEL(cm, "McdModelGetBodyData" );
    return (void*) cm->mBody;
}

/** @internal */
void MEAPI McdModelSetBodyData( McdModelID cm, void * body)
{
    MCD_CHECKMODEL(cm, "McdModelSetBodyData" );
    cm->mBody = body;
}


/**
Defines a transform offset between the coordinate system of \a cm and
another reference coordinate system.
McdModel always uses the transform set in McdModelSetTransformPtr
for collision calculations.
To define an offset between an external reference transform and
the collision transform, requires both the allocation of \a
relativeTM, a relative transform describing the offset value, and  \a
compoundTM.
Each time step, the referenceTM is compounded
by \a relativeTM, and the result is written to the transform
pointed to by McdModelSetTransformPtr. If \a relativeTM
is NULL, then no compounding occurs.

IMPORTANT: This function must be called AFTER McdModelSetBody().

  @param relativeTM : the relative transform between the two coordinate
  systems. Allocated by user.
  @param referenceTM :  The transform representing the external
  reference transform.
  @param compoundTM :  The transform representing the compounded
  reference transform.
  @param own : if true, the transforms will be deleted if a new relative
  transform is set. In this case the transform should be allocated using
  MeMemoryAPI.createAligned
    @see McdModelSetTransformPtr
*/

void MEAPI
McdModelSetRelativeTransformPtrs(McdModelID cm, 
                                 MeMatrix4 relativeTM, 
                                 MeMatrix4 referenceTM, 
                                 MeMatrix4 compoundTM,
                                 MeBool own)
{
    MCD_CHECKMODEL(cm, "McdModelSetRelativeTransformPtrs");
    
    MEASSERTALIGNED(relativeTM,MeALIGNTO);
    MEASSERTALIGNED(compoundTM,MeALIGNTO);

    if(cm->flags & kMcdModelOwnRelativeTransforms)
    {
        MEASSERT(cm->mRelTM);
        MeMemoryAPI.destroyAligned(cm->mRelTM);
        MeMemoryAPI.destroyAligned(McdGeometryInstanceGetTransformPtr(&cm->mInstance));
        cm->flags &= ~kMcdModelOwnRelativeTransforms;
    }

    cm->mRelTM = relativeTM;
    cm->mRefTM = referenceTM;

    if(relativeTM)
    {
        McdGeometryInstanceSetTransformPtr(&cm->mInstance,compoundTM);
        if(own)
            cm->flags |= kMcdModelOwnRelativeTransforms;
        else
            cm->flags &= ~kMcdModelOwnRelativeTransforms;
    }

    else
        McdGeometryInstanceSetTransformPtr(&cm->mInstance,referenceTM);

}

/**
Set the pointer to the relative transform and to the reference transform.
The model's transform (as set by McdModelSetTransformPtr) is updated
(in McdModelUpdate) to be the compound of the relative and reference
transforms.
*/
void MEAPI
McdModelGetRelativeTransformPtrs( McdModelID cm, MeMatrix4Ptr *relativeTM, MeMatrix4Ptr *referenceTM)
{
    MCD_CHECKMODEL(cm, "McdModelGetRelativeTransformPtrs");
    *relativeTM = cm->mRelTM;
    *referenceTM = cm->mRefTM;
}

/**
Copies the model's relative transform, allocates space for a compound transform, and then calls
McdModelSetRelativeTransformPtrs. The relative and compound matrices are owned by the model and
will be deleted when the relative transform pointers are overwritten or when the model is
destroyed

@see McdModelSetRelativeTransformPtrs.
*/

MEPUBLIC 
void MEAPI McdModelSetRelativeTransform(McdModelID cm,
                                        MeMatrix4Ptr relTM,
                                        MeMatrix4Ptr refTM)
{
    MCD_CHECKMODEL(cm, "McdModelSetRelativeTransform");

    if(relTM)
    {
        MeMatrix4Ptr compound = (MeMatrix4Ptr)
            MeMemoryAPI.createAligned(sizeof(MeMatrix4),MeALIGNTO);
        MeMatrix4Ptr relative = (MeMatrix4Ptr)
            MeMemoryAPI.createAligned(sizeof(MeMatrix4),MeALIGNTO);

        MeMatrix4Copy(relative,relTM);
        McdModelSetRelativeTransformPtrs(cm,relative,refTM,compound,1);
    }
    else
    {
        McdModelSetRelativeTransformPtrs(cm,0,refTM,0,0);
    }
}

/**
Returns a pointer to the model's relative transform 
@see McdModelGetRelativeTransformPtrs.
*/

MeMatrix4Ptr MEAPI McdModelGetRelativeTransform(McdModelID cm)
{
    MCD_CHECKMODEL(cm, "McdModelGetRelativeTransform");
    return cm->mRelTM;
}


/**
Explicitely updates the model's transform from it's relative and
reference transform.
@see McdModelGetRelativeTransformPtrs.
*/
void MEAPI
McdModelCompoundTransforms( McdModelID cm )
{
    MCD_ASSERT(cm->mRefTM != NULL && cm->mRelTM != NULL && cm->mInstance.mTM != NULL, "McdModelCompoundTransforms");
    MeMatrix4TMCompound(cm->mInstance.mTM, cm->mRelTM, cm->mRefTM);
}

/**
Sets a material ID to be used in MstBridge. This is unchanged in the
Mcd module.
*/
void MEAPI
McdModelSetMaterial( McdModelID cm, int material )
{
    MCD_CHECKMODEL(cm, "McdModelSetMaterial");
    McdGeometryInstanceSetMaterial(&cm->mInstance, material);
}

/** Reads the material ID assigned to this model.
The default material ID upon model creation is 0.
@see McdModelSetMaterial.
*/
unsigned int MEAPI
McdModelGetMaterial( McdModelID cm )
{
    MCD_CHECKMODEL(cm, "McdModelGetMaterial");
    return McdGeometryInstanceGetMaterial(&cm->mInstance);
}


/**
Sets the request ID that can be used by the McduRequestTable
to match a request to a given pair of models by their request IDs.
This can be used to establish group objects and a correspondence
between a pair of groups and the number of contacts to be generated.
@see McduRequestTableInit
*/
void MEAPI
McdModelSetRequestID( McdModelID cm, int requestId )
{
    MCD_CHECKMODEL(cm, "McdModelSetRequestID");
    cm->mRequestID = requestId;
}

/**
Returns model's request ID.
@see McdModelSetRequestID
*/
int MEAPI
McdModelGetRequestID( McdModelID cm )
{
    MCD_CHECKMODEL(cm, "McdModelGetRequestID");
    return cm->mRequestID;
}


/** Set the model's update callback, which gets called as the first thing by
McdModelUpdate calls. */

void MEAPI McdModelSetUpdateCallback( McdModelID cm, McdModelUpdateFnPtr f)
{
    MCD_CHECKMODEL(cm, "McdModelSetUpdateCallback");
    cm->mTransformUpdateFn = f;
}


/** Get the model's update callback, which gets called as the first thing by
    McdModelUpdate calls. */

McdModelUpdateFnPtr MEAPI McdModelGetUpdateCallback( McdModelID cm )
{
    MCD_CHECKMODEL(cm, "McdModelGetUpdateCallback");
    return cm->mTransformUpdateFn;
}

MEPUBLIC  
void MEAPI McdModelSetIntersectCallback(McdModelID cm, McdModelIntersectFnPtr fn)
{
    MCD_CHECKMODEL(cm, "McdModelSetIntersectCallback");
    cm->mIntersectFn = fn;
}


MEPUBLIC           
McdModelIntersectFnPtr 
                  MEAPI McdModelGetIntersectCallback(McdModelID cm)
{    
    MCD_CHECKMODEL(cm, "McdModelGetIntersectCallback");
    return cm->mIntersectFn;
}


  /** Read the model's sort key. Used in conjunction with McduRequestTable. */
MeI16 MEAPI McdModelGetSortKey(McdModelID cm)
{
    MCD_CHECKMODEL(cm, "McdModelGetSortKey");
    return cm->sortKey;
}

  /** Set the model's sort key. Should be between 0 and (2^15)-1 */
void MEAPI McdModelSetSortKey(McdModelID cm, MeI16 key)
{
    MCD_CHECKMODEL(cm, "McdModelSetSortKey");

#ifdef MCDCHECK
    if(cm->mSpaceID!=-1)
        McdCoreError(kMcdErrorNum_GenericFatal,
        "Attempt to use set a sort key on an object which was already in a far field","McdModelSetSortKey",
        __FILE__,__LINE__);
#endif

    cm->sortKey = key;
}


