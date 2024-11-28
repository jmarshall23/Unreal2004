/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:54 $ - Revision: $Revision: 1.44.2.2 $

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

#include <MeMemory.h>
#include <MeMessage.h>
#include <MeMemory.h>
#include <McdCheck.h>
#include <McdMessage.h>
#include <McdFrame.h>
#include <McdGeometry.h>
#include <McdGeometryInstance.h>

/*----------------------------------------------------------------
 * McdGeometry implementation
 *----------------------------------------------------------------
 */


/** @internal */
extern "C"
MeBool MEAPI
McdGeometryIsValid( McdGeometryID g)
{
    if( !g )
    {
        return 0;
    }
    return McdFrameworkTypeIsValid(g->frame, McdGeometryGetType(g) );
}


#include <stdio.h>
/** @internal */
extern "C"
void MEAPI
McdGeometryInit(McdGeometry* g, McdFramework *frame, MeI16 typeId )
{
    // CHECK typeid > reigsterednum
    g->frame = frame;
    g->mRefCtAndID = typeId;
    

    if(!frame->firstGeometry)
    {
        frame->firstGeometry = g;
        g->next = g->prev = g;
    }
    else
    {
        g->prev = frame->firstGeometry->prev;
        g->next = frame->firstGeometry;

        g->prev->next = g;
        g->next->prev = g;
    }
    frame->geometryCount++;
}

/* internal */
extern "C" 
void MEAPI
McdGeometryDeinit(McdGeometry *g)
{
    McdFramework *frame = g->frame;

    if(frame->firstGeometry==g)
        frame->firstGeometry = g->prev==g ? 0 : g->next;

    /* harmless if this is the only link */
    g->next->prev = g->prev;
    g->prev->next = g->next;

    MeMemoryAPI.destroyAligned(g);
    frame->geometryCount--;
}


/** Determines whether a given geometry type has been registered with the
Mcd system.
@arg typeId: the typeId for a concrete geometry type, such as
McdSphereGetTypeId()
*/


/** @internal */
extern "C"
const char* MEAPI
McdGeometryGetTypeName( McdGeometryID g )
{
    MCDGEOMETRY_CHECK_VALID( g, "McdGeometryGetTypeName" );

    return McdFrameworkGetTypeName(g->frame, McdGeometryGetType(g) );
}


/** @internal */
extern "C"
int MEAPI
McdGeometryGetReferenceCount( McdGeometryID g )
{
    MCDGEOMETRY_CHECK_VALID( g, "McdGeometryGetReferenceCount" );

    return g->mRefCtAndID>>8;
}

/**
 *  Increase 'reference count' of geometry by one.
 *  This allows a user to manually increase the internal reference count of
 *  this geometry. McdModel's will automatically increase the reference count.
 *
 *  @see McdGeometryDecrementReferenceCount
 */
extern "C"
void MEAPI McdGeometryIncrementReferenceCount( McdGeometryID g )
{
    MCDGEOMETRY_CHECK_VALID( g, "McdGeometryIncrementReferenceCount" );

    MCD_ASSERT(McdGeometryGetReferenceCount(g) < (1<<24), "McdGeometryDecrementReferenceCount");
    g->mRefCtAndID+=256;
}

/**
 *  Decrese 'reference count' of geometry by one.
 *  This allows a user to manually decrease the internal reference count of
 *  this geometry. McdModel's will automatically decrease the reference count.
 *
 *  @see McdGeometryIncrementReferenceCount
 */
extern "C"
void MEAPI McdGeometryDecrementReferenceCount( McdGeometryID g )
{
    MCDGEOMETRY_CHECK_VALID( g, "McdGeometryDecrementReferenceCount" );

    MCD_ASSERT(McdGeometryGetReferenceCount(g) > 0, "McdGeometryDecrementReferenceCount");
    (g->mRefCtAndID)-=256;
}

extern "C"
void MEAPI McdGeometrySetReferenceCount( McdGeometryID g, int refCount )
{
    MCDGEOMETRY_CHECK_VALID( g, "McdGeometrySetReferenceCount" );
    MCD_ASSERT(refCount >= 0, "McdGeometrySetReferenceCount");
    MCD_ASSERT(refCount < (1<<24), "McdGeometrySetReferenceCount");

    g->mRefCtAndID = (refCount<<8)&(g->mRefCtAndID & 255);
}

/** Destroy a McdGeometry object. @a g is no longer valid after this call.
*/
extern "C"
void MEAPI
McdGeometryDestroy(McdGeometryID g)
{
    MCDGEOMETRY_CHECK_VALID(g, "McdGeometryDestroy");

    if((g->mRefCtAndID>>8)!=0)
        MeWarning(0,"Destroying geometry with non-zero reference count!");

    McdGeometryDestroyFnPtr fn = g->frame->geometryVTableTable[McdGeometryGetType(g)].destroy;

    MCD_ASSERT(fn, "McdGeometryDestroy");


    /* the destroyFn responsible for freeing g */
    (*fn)(g);
}

extern "C"
McdFrameworkID MEAPI
McdGeometryGetFramework(McdGeometryID g)
{
    return g->frame;
}



/** get the line segment intersect fn associated with a geometry
*/

McdLineSegIntersectFnPtr MEAPI
McdGeometryGetLineSegIntersectFnPtr(McdGeometryID g)
{
    MCDGEOMETRY_CHECK_VALID(g, "McdGeometryDestroy");
    return g->frame->geometryVTableTable[McdGeometryGetType(g)].lineSegIntersect;
}

/**
returns the axis aligned bounding box corresponding to the
position of the geometry as given by the transform @tm.
@minCorner (resp. @maxCorner) is the corner of the box
with minimum (resp. maximum) x, y and z coordinates.

  \deprecated.
*/
extern "C"
void MEAPI
McdGeometryGetAABB(McdGeometryID g, MeMatrix4 tm,
                   MeVector3 minCorner, MeVector3 maxCorner )
{

    McdGeometryInstance ins;
    MCDGEOMETRY_CHECK_VALID(g, "McdGeometryGetAABB");

//    McdGeometryGetAABBFnPtr fn = 
//        g->frame->geometryVTableTable[McdGeometryGetType(g)].getAABB;

    MEASSERT(!"McdGeometryGetAABB is deprecated; use McdGeometryInstanceGetAABB instead.");

    ins.mTM = tm;
    ins.mGeometry = g;
//    (*fn)(&ins,0,0);
    McdGeometryInstanceGetAABB(&ins,minCorner,maxCorner);
}

/**
returns the bounding sphere of the geometry in its own coordinate system.

  \deprecated.
*/
extern "C"
void MEAPI
McdGeometryGetBSphere(McdGeometryID g,
                      MeVector3 center, MeReal *radius )
{
    MCDGEOMETRY_CHECK_VALID( g, "McdGeometryGetBSphere" );

    McdGeometryGetBSphereFnPtr fn = 
        g->frame->geometryVTableTable[McdGeometryGetType(g)].getBSphere;

    MCD_ASSERT(fn, "McdGeometryGetBSphere");

    (*fn)(g,center,radius);
}

/**
returns the extreme point in the direction of @a inDir on the geometry.
This is the point of maximum dot product with @a inDir.
@inDir must have a unit normal.

\deprecated

*/
extern "C"
void MEAPI
McdGeometryMaximumPoint(McdGeometryID g, MeMatrix4 tm,
                        MeReal * const inDir,
                        MeReal * const outPoint)
{
    McdGeometryInstance ins;
    MCDGEOMETRY_CHECK_VALID( g, "McdGeometryMaximumPoint" );

    McdGeometryMaximumPointFnPtr fn = 
        g->frame->geometryVTableTable[McdGeometryGetType(g)].maximumPoint;

    MCD_ASSERT(fn, "McdGeometryMaximumPoint");
    ins.mTM = tm;
    ins.mGeometry = g;

    (*fn)(&ins,inDir,outPoint);
}

/** Computes @a g 's inertia matrix and volume, writing the results into @a
m and @a volume, respectively. The
inertia matrix is computed relative to @a relTM, whose translational
component always holds the position of the center of mass, and whose
orientation is either an identity matrix or an orientation with respect
to which @a m is diagonal. A return value of zero indicates that @a
relTM is diagonal, in which case @a m has been calculated with respect
to the original local coordinate system of @a g.

  Note: when using this function for calculating the mass-properties of
  McdAggregate geometry types, any area of overlap will be counted multiple
  times. This has the effect of making those regions particularly dense. To
  obtain accurate inertia tensors, try to avoid overlapping aggregate elements.
*/
extern "C"
MeI16 MEAPI
McdGeometryGetMassProperties(McdGeometryID g,  MeMatrix4 relTM, MeMatrix3
                             m, MeReal *volume )
{
    MCDGEOMETRY_CHECK_VALID( g, "McdGeometryGetMassProperties" );

    McdGeometryGetMassPropertiesFnPtr fn = 
        g->frame->geometryVTableTable[McdGeometryGetType(g)].getMassProperties;

    MCD_ASSERT(fn, "McdGeometryGetMassProperties");

    return (*fn)(g, relTM,m, volume);
}


void MEAPI McdGeometryDebugDraw(const McdGeometryID g, const MeMatrix4Ptr tm, const MeReal colour[3])
{
    MCDGEOMETRY_CHECK_VALID( g, "McdGeometryDebugDraw" );
    McdGeometryDebugDrawFnPtr fn = 
        g->frame->geometryVTableTable[McdGeometryGetType(g)].debugDraw;
    (*fn)(g,tm,colour);
}   
