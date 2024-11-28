/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/17 13:44:35 $ - Revision: $Revision: 1.4.2.3 $

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
#include <MeMemory.h>
#include <McdSphyl.h>
#include "McdCheck.h"
#include <McdGeometryInstance.h>
#include <McdMessage.h>
#include "lsTransform.h"
#include <MeMath.h>
#include <MeDebugDraw.h>
#include "MeMessage.h"

MCD_IMPLEMENT_GEOMETRY_TYPE( McdSphyl, "McdSphyl", Sphyl );

#define PARALLEL_THRESH_EPS ((MeReal)0.0001)

/** 
 *  Create Sphyl (round-ended cylinder) with supplied radius & height. 
 *  Note, height is length of cylinderical section, and does not include radius of each rounded end.
 *  Origin of Sphyl is its centre, and it is aligned along the z-axis.
 */
McdSphylID MEAPI McdSphylCreate(McdFramework *frame,  MeReal r, MeReal h )
{
    McdSphylID s;
    
    s = (McdSphylID)
        MeMemoryAPI.createAligned(sizeof(McdSphyl),MeALIGNTO);

    if (s != 0)
    {
        McdGeometryInit((McdGeometryID)s, frame, kMcdGeometryTypeSphyl);
        McdSphylSetRadius(s,r);
        McdSphylSetHeight(s,h);
    }
    
    return s;
}

/* type-specific functions */

/** Set a sphyls's radius to the value @param r */
void MEAPI McdSphylSetRadius( McdSphylID s, MeReal r )
{
    McdSphyl* sphyl = (McdSphyl*)s;
    
    if(r < 0)
    {
#ifdef _MECHECK
        MeWarning(0, "McdSphylSetRadius: Radius cannot be negative.");
#endif
        r = 0;
    }

    sphyl->mRadius = r;
}

/** Return a sphyl's radius */
MeReal MEAPI McdSphylGetRadius( McdSphylID s )
{
    return ((McdSphyl*)s)->mRadius;
}

/**
 *  Set a sphyls's height to the value @param r.
 *  Note, this is the length of the cylindrical section 
 *  ie. does not include radius of each rounded end.
 */
void MEAPI McdSphylSetHeight( McdSphylID s, MeReal h )
{
    McdSphyl* sphyl = (McdSphyl*)s;
    
    if(h < 0)
    {
#ifdef _MECHECK
        MeWarning(0, "McdSphylSetRadius: Height cannot be negative.");
#endif
        h = 0;
    }

    sphyl->mHalfHeight = (MeReal)0.5 * h;
}

/**
 * Return a sphyl's total height
 * Note, this is the length of the cylindrical section 
 * ie. does not include radius of each rounded end.
 */
MeReal MEAPI
McdSphylGetHeight( McdSphylID s )
{
    return 2 * ((McdSphyl*)s)->mHalfHeight;
}


/* polymorphic functions */

/** Destroy this Sphyl geometry. */
void MEAPI McdSphylDestroy( McdSphylID s)
{
    MCD_CHECKGEOMETRY(s, "McdSphylDestroy");
    McdGeometryDeinit(s);
}

void MEAPI McdSphylUpdateAABB(McdGeometryInstanceID ins, MeMatrix4Ptr finalTM, MeBool tight)
{
    MeVector3 absAx;
    McdSphyl* s = (McdSphyl*)McdGeometryInstanceGetGeometry(ins);
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);

    /* Fabs Z-axis + maxmised radius vector.*/
    absAx[0] = s->mHalfHeight * MeFabs(tm[2][0]) + s->mRadius;
    absAx[1] = s->mHalfHeight * MeFabs(tm[2][1]) + s->mRadius;
    absAx[2] = s->mHalfHeight * MeFabs(tm[2][2]) + s->mRadius;

    /* Calculate current counding box */
    ins->min[0] = tm[3][0] - absAx[0]; 
    ins->min[1] = tm[3][1] - absAx[1]; 
    ins->min[2] = tm[3][2] - absAx[2]; 

    ins->max[0] = tm[3][0] + absAx[0]; 
    ins->max[1] = tm[3][1] + absAx[1]; 
    ins->max[2] = tm[3][2] + absAx[2]; 

    /*  If there is a 'finalTTM' (ie. this is a swept test),
        work out final AABB and combine. */
    if(finalTM)
    {
        MeVector3 fMin, fMax;
        absAx[0] = s->mHalfHeight * MeFabs(finalTM[2][0]) + s->mRadius;
        absAx[1] = s->mHalfHeight * MeFabs(finalTM[2][1]) + s->mRadius;
        absAx[2] = s->mHalfHeight * MeFabs(finalTM[2][2]) + s->mRadius;
        
        fMin[0] = finalTM[3][0] - absAx[0]; 
        fMin[1] = finalTM[3][1] - absAx[1]; 
        fMin[2] = finalTM[3][2] - absAx[2]; 
        
        fMax[0] = finalTM[3][0] + absAx[0]; 
        fMax[1] = finalTM[3][1] + absAx[1]; 
        fMax[2] = finalTM[3][2] + absAx[2];

        MeVector3Min(ins->min,ins->min, fMin);
        MeVector3Max(ins->max,ins->max, fMax);
    }
}

void MEAPI McdSphylGetBSphere( McdSphylID s, MeVector3 center, MeReal *radius )
{
    McdSphyl* sphyl = (McdSphyl*)s;
    
    center[0] = center[1] = center[2] = (MeReal)(0.);
    *radius = sphyl->mHalfHeight + sphyl->mRadius;
}

void MEAPI McdSphylMaximumPoint(McdGeometryInstanceID ins, MeReal * const inDir, MeReal * const outPoint)
{
    MeReal nZ;
    McdSphyl* s = (McdSphyl*)McdGeometryInstanceGetGeometry(ins);
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);

    /* First, set outPoint to the nearest line-seg end of sphyl. */
    nZ = MeVector3Dot(tm[2], inDir);    
    MeVector3Copy(outPoint, tm[3]);
    if (nZ > PARALLEL_THRESH_EPS)
    {
        MeVector3MultiplyAdd(outPoint, s->mHalfHeight, tm[2]);
    } else
    if (nZ < -PARALLEL_THRESH_EPS)
    {
        MeVector3MultiplyAdd(outPoint, -s->mHalfHeight, tm[2]);
    }

    /* And add radius in direction of inDir. */
    MeVector3MultiplyAdd(outPoint, s->mRadius, inDir);
}

/**
 * Get mass properties for this sphyl.
 * This should be identical to a cylinder whos ends have been extended by 3/(2*sqrt(5)) * radius.
 */
MeI16 MEAPI
McdSphylGetMassProperties( McdSphylID s,
                           MeMatrix4 relTM,
                           MeMatrix3 m,
                           MeReal *volume)
{
    McdSphyl *sphyl = (McdSphyl*)s;
    MeReal r = sphyl->mRadius;
    MeReal l = 2 * (sphyl->mHalfHeight + (sphyl->mRadius*(MeReal)0.67082));
    MeReal mass = 1.0;
    
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            m[i][j] = 0.0;
        
    m[0][0] = (MeReal) (((1/4.0) * mass * r * r) + ((1/12.0) * mass * l * l));
    m[1][1] = m[0][0];
    m[2][2] = (MeReal) ((1/2.0) * mass * r * r);
    
    MeMatrix4TMMakeIdentity( relTM );
    
    *volume = ME_PI * r * r * l;
    
    return 0;
}



/** 
 *  Draw a sphere for debug purposes. 
 *  Assumes MeDebugDraw line function has been set up 
 */

void MEAPI McdSphylDebugDraw(const McdGeometryID geom, const MeMatrix4 tm, const MeReal colour[3])
{
    /* TODO */
}
