/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

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

#include <MePrecision.h>
#include <MeMath.h>
#include <McdCheck.h>

#include <McdModel.h>
#include <McdContact.h>
#include <McdInteractionTable.h>
#include <McdPrimitives.h>
#include <ConvexGeomUtils.h>

extern void
AccumulateSphylContacts(MeVector3 cp, MeVector3 normal, MeReal sep, MeI16 segDim, MeI16 otherDim,
                        MeVector3Ptr axis, MeReal ds, MeMatrix4Ptr tm,
                        McdIntersectResult *result);

MeBool MEAPI
McdSphylConvexMeshIntersect(McdModelPair * p, McdIntersectResult *result)
{
    MeMatrix4Ptr tm1 = McdModelGetTransformPtr(p->model1);
    MeMatrix4Ptr tm2 = McdModelGetTransformPtr(p->model2);
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    McdSphyl* sphyl = (McdSphyl*)McdModelGetGeometry( p->model1 );
    McdConvexMesh* convexMesh = (McdConvexMesh*)McdModelGetGeometry( p->model2 );
    McdConvexHull *convex = &convexMesh->mHull;

    result->contactCount = 0;
    result->touch = 0;

    // Work in convex hull's reference frame.
    MeVector3 axis;
    MeVector3 pos;
    MeMatrix4TMInverseRotate(axis, tm2, tm1[2]);
    MeMatrix4TMInverseTransform(pos, tm2, tm1[3]);

    // Find closest point(s) of approach between convex and sphyl
    MeVector3 cp;
    MeVector3 sp;
    VoronoiRegionType regionType;
    MeReal s;
    const MeReal ds = ConvexHullNSegment(convex, pos, axis, -sphyl->mHalfHeight, sphyl->mHalfHeight,
                                         cp, &s, &regionType);
    MeVector3ScaleAndAdd(sp, pos, s, axis);

    // Normal is difference between closest points, unless the segment intersects the hull
    MeVector3 n;
    MeVector3Subtract(n, sp, cp);
    const MeReal n2 = MeVector3Dot(n, n);
    MeReal sep;
    if (n2 > eps*eps)
    {
        // We have a good normal
        const MeReal nInv = MeRecipSqrt(n2);
        sep = n2*nInv;
        MeVector3Scale(n, nInv);
    } else
    {
        // The line segment intersected the hull.  This will be rare if contacts are generated.
        // If contacts are not generated, we should consider not returning a normal in result->normal.
        // Find normal by separating plane.
        sep = SegmentConvexHullSep(pos, axis, -sphyl->mHalfHeight, sphyl->mHalfHeight, convex, n, &s, &regionType);
    }

    sep -= convexMesh->mFatness + sphyl->mRadius;

    if (sep < eps)
    {
        MeVector3ScaleAndAdd(cp, cp, convexMesh->mFatness, n);

        MeVector3Set(result->normal, 0, 0, 0);

        AccumulateSphylContacts(cp, n, sep, 3, regionType, tm1[2], ds, tm2, result);
    }

    return result->touch;
}
