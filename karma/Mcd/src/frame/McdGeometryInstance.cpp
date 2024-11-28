/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.15.2.3 $

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

#include <MeMath.h>
#include <MePool.h>
#include <McdCheck.h>
#include <McdGeometry.h>
#include <McdGeometryInstance.h>
#include <McdAggregate.h>

/* create a geometry instance */
static McdGeometryInstanceID MEAPI 
McdGeometryInstanceCreate(McdFramework *frame)
{
    McdGeometryInstance *ins = (McdGeometryInstance*)
        (*MePoolAPI.getStruct)(&frame->instancePool);
    McdGeometryInstanceReset(ins);
    return ins;
}


void MEAPI McdGeometryInstanceReset(McdGeometryInstanceID ins)
{
    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceCreate");
    ins->mMaterial = 0;
    ins->mTM = 0;
    ins->mGeometry = 0;
    MeVector3Set(ins->min,MEINFINITY,MEINFINITY,MEINFINITY);
    MeVector3Set(ins->max,-MEINFINITY,-MEINFINITY,-MEINFINITY);
    ins->next = 0;
    ins->prev = 0;
    ins->parent = 0;
    ins->child = 0;
}

/* destroy a geometry instance and all of its children. When we need to destroy a geometry instance,
it's because we're cleaning up a composite geometry. And in such a case, we always have a framework
pointer to use to put the instance back in its pool. */

static void MEAPI 
McdGeometryInstanceDestroy(McdFrameworkID frame, McdGeometryInstanceID ins)
{
    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceDestroy");
    if(ins->child)
    {   
        McdGeometryInstanceID child = ins->child;
        while(child)
        {
            McdGeometryInstanceID nextChild = child->next;
            McdGeometryInstanceDestroy(frame,child);
            child = nextChild;
        }
    }
    if(ins->mGeometry)
        McdGeometryDecrementReferenceCount(ins->mGeometry);
    (*MePoolAPI.putStruct)(&frame->instancePool, ins);
}

/**
Returns the minimal-volume sphere which completely bounds the volume
occupied by @a m.
*/
extern "C"
void MEAPI
McdGeometryInstanceGetBSphere( McdGeometryInstanceID ins, MeVector3 center, MeReal *radius )
{
    McdGeometryID g = ins->mGeometry;
    MeVector3 c;
    MCD_CHECKGEOMETRY(g, "McdGeometryInstanceGetBSphere");
    MCDGEOMETRY_CHECK_VALID( g, "McdGeometryGetBSphere" );

    McdGeometryGetBSphereFnPtr fn = g->frame->geometryVTableTable[McdGeometryGetType(g)].getBSphere;
    MCD_ASSERT(fn, "McdGeometryInstanceGetBSphere");
    (*fn)(g,c,radius);

    MeMatrix4TMTransform(center,ins->mTM,c);
}

/**
Sets a material ID to be used in MstBridge. This is unchanged in the
Mcd module.
*/
void MEAPI
McdGeometryInstanceSetGeometry(McdGeometryInstanceID ins, McdGeometryID geom)
{
    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceSetGeometry");
    int i;

    if(ins->child)
    {   
        McdFrameworkID frame = McdGeometryGetFramework(ins->mGeometry);
        McdGeometryInstanceID child = ins->child;
        while(child)
        {
            McdGeometryInstanceID nextChild = child->next;
            McdGeometryInstanceDestroy(frame,child);
            child = nextChild;
        }
        ins->child = 0;
    }
    if(ins->mGeometry)
        McdGeometryDecrementReferenceCount(ins->mGeometry);

    ins->mGeometry = geom;

    if(geom)
        McdGeometryIncrementReferenceCount(geom);

    if(!geom || McdGeometryGetTypeId(geom)!=kMcdGeometryTypeAggregate)
        return;

    int max = McdAggregateGetElementCountMax((McdAggregateID)geom);
    if(max==0)
        return;

    McdFramework *frame = McdGeometryGetFramework(geom);
    McdGeometryInstanceID child, next;
    
    child = McdGeometryInstanceCreate(frame);
    child->parent = ins;
    ins->child = child;
    
    for(i=0;i<max-1;i++)
    {
        next = McdGeometryInstanceCreate(frame);
        MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceSetGeometry");
        next->parent = ins;
        next->prev = child;
        child->next = next;
        child = next;
    }
    
    for(i=0, child = ins->child;i<max;i++, child = child->next)
    {
        McdGeometryID g = McdAggregateGetElementGeometry(geom,i);
        McdGeometryInstanceSetGeometry(child, g);
    }        
}

/** Reads the material ID assigned to this model.
The default material ID upon model creation is 0.
@see McdModelSetMaterial.
*/
McdGeometryID MEAPI
McdGeometryInstanceGetGeometry(McdGeometryInstanceID ins)
{
    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceGetGeometry");
    return ins->mGeometry;
}


/**
Returns the axis-aligned bounding box which contains the model's geometry.
*/
extern "C"
void MEAPI
McdGeometryInstanceUpdateAABB( McdGeometryInstanceID ins, MeMatrix4Ptr finalTM, MeBool tight)
{
    McdGeometryID g;
    McdGeometryGetAABBFnPtr fn;

    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceGetAABB");

    g = ins->mGeometry;

    MCD_CHECKGEOMETRY(g, "McdGeometryInstanceGetAABB");

    int type = McdGeometryGetType(g);
    fn = g->frame->geometryVTableTable[type].getAABB;
    MCD_ASSERT(fn, "McdGeometryGetAABB");
    
    (*fn)(ins, finalTM, tight);    
}

/**
returns the extreme point in the direction of @a inDir on the geometry.instance
This is the point of maximum dot product with @a inDir.
@inDir must have a unit normal.
*/
extern "C"
void MEAPI 
McdGeometryInstanceMaximumPoint(McdGeometryInstanceID ins,
                                MeReal * const inDir, 
                                MeReal * const outPoint)
{

    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceMaximumPoint" );

    McdGeometryID g = ins->mGeometry;

    MCD_CHECKGEOMETRY(g, "McdGeometryInstanceGetAABB");

    McdGeometryMaximumPointFnPtr fn = g->frame->geometryVTableTable[McdGeometryGetType(g)].maximumPoint;
        
    MCD_ASSERT(fn, "McdGeometryMaximumPoint");

    (*fn)(ins,inDir,outPoint);
}


/**
Returns the axis-aligned bounding box which contains the model's geometry.
*/
extern "C"
void MEAPI
McdGeometryInstanceGetAABB(McdGeometryInstanceID ins, MeVector3 minCorner, MeVector3 maxCorner)
{
    MeVector3Copy(minCorner, ins->min);
    MeVector3Copy(maxCorner, ins->max);
}

extern "C"
MeMatrix4Ptr MEAPI
McdGeometryInstanceGetTransformPtr(McdGeometryInstanceID ins )
{
    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceGetTransformPtr");
    return ins->mTM;
}

extern "C"
void MEAPI
McdGeometryInstanceSetTransformPtr(const McdGeometryInstanceID ins, const MeMatrix4Ptr tm )
{
    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceSetTransformPtr");
    ins->mTM = tm;
}


/**
Sets a material ID to be used in MstBridge. This is unchanged in the
Mcd module.
*/
void MEAPI
McdGeometryInstanceSetMaterial(McdGeometryInstanceID ins, int material )
{
    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceSetMaterial");
    ins->mMaterial = material;
}

/** Reads the material ID assigned to this model.
The default material ID upon model creation is 0.
@see McdModelSetMaterial.
*/
unsigned int MEAPI
McdGeometryInstanceGetMaterial(McdGeometryInstanceID ins)
{
    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceGetMaterial");
    return ins->mMaterial;
}



/**
returns the i'th child of the geometry instance, or zero if the corresponding
geometry is not a composite.
*/

McdGeometryInstanceID MEAPI McdGeometryInstanceGetChild(McdGeometryInstanceID ins, int num)
{
    McdGeometryInstanceID child = 0;
    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceGetMaterial");
    if(ins->child)
    {
        for(child = ins->child; child && num > 0; child = child->next,num--);
    }
    return child;
}
    

MeBool MEAPI McdGeometryInstanceOverlap(McdGeometryInstanceID ins1, McdGeometryInstanceID ins2)
{
    int result = 1,i;
    MCD_CHECKGEOMETRYINSTANCE(ins1, "McdGeometryInstanceOverlap");
    MCD_CHECKGEOMETRYINSTANCE(ins2, "McdGeometryInstanceOverlap");
    for(i=0;i<3;i++)
        result = result && ins1->min[i] < ins2->max[i] && ins2->min[i] < ins1->max[i];
    return result;
}

McdGeometryType MEAPI McdGeometryInstanceGetGeometryType(McdGeometryInstanceID ins)
{
    MCD_CHECKGEOMETRYINSTANCE(ins, "McdGeometryInstanceOverlap");
    return McdGeometryGetType(ins->mGeometry);
}

