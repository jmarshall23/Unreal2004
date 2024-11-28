/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

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

#ifndef _CONVEXGEOMUTILS_H
#define _CONVEXGEOMUTILS_H

#include <GeomUtils.h>

// The Voronoi regions of a convex polyhedron.
// Note that the enumeration value equals the dimension of the
// corresponding feature.
typedef enum
{
    kVertexRegion,
    kEdgeRegion,
    kFaceRegion,
    kInteriorRegion,

    kVoronoiRegionTypeCount
} VoronoiRegionType;


/*
    Find the separation between a segment and a convex hull.
    Returns the normal of the separating plane, n.
    *s = parameter of closest point on segment
    *regionType = dimenion of separating feature on convex hull (0, 1, or 2)
 */
MeReal
SegmentConvexHullSep(const MeVector3 p, const MeVector3 d, const MeReal smin, const MeReal smax,
                     const struct McdConvexHull * const convex, MeVector3 n,
                     MeReal * s, VoronoiRegionType * regionType);


/*
    Determine the maximum feature (vertex, edge or face) relative to a given direction d.
 */
VoronoiRegionType
ConvexHullMaximumFeature(const McdConvexHull * const convex, const MeVector3 d, MeI32 *regionIndex, const MeReal eps);


/*
    Determine which Voronoi region of a convex hull (convex) a point (pos) lies in
 */
VoronoiRegionType
ConvexHullVoronoiRegion(const struct McdConvexHull * const convex, const MeVector3 pos, MeI32 * regionIndex);


/*
    Closest points on  a convex hull and a line segment to each other

    Upon return, cp = position of the closest point (to the line segment) on the convex hull,
                 *s = parameter of closest point (to the convex hull) on the line segment,
                 *closestRegionType = Voronoi region type (dimension) of closest feature.

    If the returned value is zero, then the closest points are unique.  If the function returns
    a nonzero value ds, then the closest points are not unique.  In that case, there is a line segment A
    on the surface of the convex hull, and a corresponding line segment B on (a subset of) the passed-in
    line segment, where the two objects are nearest.  The endpoints of A are cp and cp + ds * d, and the
    endpoints or B are p+(*s)*d and p+(*s + ds)*d.

    N.B. The direction vector d must be normalized.
 */
MeReal
ConvexHullNSegment(const struct McdConvexHull * const convex,
                   const MeVector3 p, const MeVector3 d, const MeReal smin, const MeReal smax,
                   MeVector3 cp, MeReal * s, VoronoiRegionType * closestRegionType);

#endif // #ifndef _CONVEXGEOMUTILS_H
