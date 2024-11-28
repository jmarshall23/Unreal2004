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

#include <ConvexGeomUtils.h>
#include <McdQHullTypes.h>


/*
    Copied from McdGjkMaximumPoint.cpp, an made to take a McdConvexHull instead of an McdConvexMesh.
 */
static MeReal
McdConvexHullMaximumPoint(const McdConvexHull *hull,
                          const MeVector3 inDir, 
                          int hint,
                          MeReal minDist,
                          int *outIndex)
{
    MEASSERT(hull && outIndex);

    int i, j, k, t, m, next, prev;
    int n = hull->numVertex;
    McdCnvVertex *vert = hull->vertex;
    MeReal d1, d2;

    next = (hint < 0 || hint >= n) ? 0 : hint;
    d1 = MeVector3Dot(inDir, vert[next].position);
    i = -1;

    //  Walk over the convex edges always going to the best neighbor vertex.
    for (t = -2; t < n && i != next; ++t)
    {
        //  find any vertex with distance greater than minDist
        if (d1 > minDist)
            break;

        //  Look at all the vertices around "i", if any are better, set "next".
        prev = i;
        i = next;
        m = McdCnvVertexGetCount(hull, i);

        for (j = 0; j < m; ++j)
        {
            k = McdCnvVertexGetNeighbor(hull, i, j);

            if (k == prev) continue;    // don't go back where we came from

            d2 = MeVector3Dot(inDir, vert[k].position);

            if (d2 > d1)
            {
                d1 = d2;
                next = k;
            }
        }
    }
    MEASSERT(t < n);      // check exceeded max iterations

    *outIndex = next;
    return d1;
}

MeReal
SegmentConvexHullSep(const MeVector3 p, const MeVector3 d, const MeReal smin, const MeReal smax,
                     const McdConvexHull * const convex, MeVector3 n,
                     MeReal * s, VoronoiRegionType * regionType)
{
    MeI32 i;
    VoronoiRegionType closestRegionType;

    // Start with faces
    closestRegionType = kFaceRegion;
    MeI32 face = 0;
    const McdCnvFace * f = convex->face;
    const McdCnvEdge * e = convex->edge + f->firstEdge;
    const McdCnvVertex * v = convex->vertex + e->fromVert;
    MeReal sep = SegmentPlaneSep(p, d, smin, smax, v->position, f->normal, s);
    MeVector3Copy(n, f->normal);
    for (i = 1; i < convex->numFace; ++i)
    {
        ++f;
        const McdCnvEdge * e = convex->edge + f->firstEdge;
        const McdCnvVertex * v = convex->vertex + e->fromVert;
        MeReal si;
        const MeReal faceSep = SegmentPlaneSep(p, d, smin, smax, v->position, f->normal, &si);
        if (faceSep > sep)
        {
            sep = faceSep;
            *s = si;;
            face = i;
        }
    }

    f = convex->face + face;
    MeVector3Copy(n, f->normal);

    // Look at edges
//    for (i = f->firstEdge; i < (f+1)->firstEdge; ++i)
//    {
//        const McdCnvEdge * const ei = convex->edge + i;
    for (i = 0; i < convex->numEdge; ++i)
    {
        const McdCnvEdge * const ei = convex->edge + i;
        if (ei->leftFace < ei->rightFace)
        {
            continue;
        }

        // Get edge start point and direction
        const McdCnvVertex * const v0i = convex->vertex + ei->fromVert;
        const McdCnvVertex * const v1i = convex->vertex + ei->toVert;
        MeVector3 edgeDir;
        MeVector3Subtract(edgeDir, v1i->position, v0i->position);
        const MeReal edgeLen2 = MeVector3Dot(edgeDir, edgeDir);
        MeVector3Scale(edgeDir, ei->invLength);
        const MeReal edgeLen = edgeLen2*ei->invLength;

        // Calculate closest points between edge and segment
        MeReal s0;
        MeReal s1;
        MeReal ds0;
        const MeBool askew = NSegmentSegment(v0i->position, edgeDir, 0, edgeLen, p, d, smin, smax, &s0, &s1, &ds0);

        MeVector3 ni;
        MeVector3ScaleAndAdd(ni, p, s1, d);
        MeVector3Subtract(ni, ni, v0i->position);
        MeVector3ScaleAndAdd(ni, ni, -s0, edgeDir);
        MeReal ni2 = MeVector3Dot(ni, ni);

        // Keep "edge normal" for reference
        MeVector3 edgeNormal;
        const McdCnvFace * const leftFace = convex->face + ei->leftFace;
        const McdCnvFace * const rightFace = convex->face + ei->rightFace;
        MeVector3Add(edgeNormal, leftFace->normal, rightFace->normal);

        if (ni2 > PARALLEL_THRESH_EPS)
        {   // n is a good normal
            MeVector3Scale(ni, MeRecipSqrt(ni2));
            if (MeVector3Dot(ni, edgeNormal) < 0)
            {
                MeVector3Scale(ni, -1);
            }
        } else
        {
            if (askew)
            {   // may find a good normal by crossing lines
                MeVector3Cross(ni, edgeDir, d);
                MeVector3Scale(ni, MeRecipSqrt(ni2));
                if (MeVector3Dot(ni, edgeNormal) < 0)
                {
                    MeVector3Scale(ni, -1);
                }
            } else
            {   // Lines coincide.  "Edge normal" (the average of the normals
                // of the two adjoining faces) will be used.
                MeVector3Copy(ni, edgeNormal);
            }
            MeVector3Normalize(ni);
        }

        MeReal di = MeVector3Dot(d, ni);
        di *= (di < 0) ? smax : smin;
        const MeReal ci = MeVector3Dot(p, ni) + di;
        int maxPoint;
        const MeReal maxP = McdConvexHullMaximumPoint(convex, ni, -1, MEINFINITY, &maxPoint);
        const MeReal sepi = ci-maxP;

        if (sepi > sep-PARALLEL_THRESH_EPS)
        {   // Best separation, use this
            sep = sepi;
            MeVector3Copy(n, ni);
            closestRegionType =  (s0 <= 0 || s0 >= edgeLen) ? kVertexRegion : kEdgeRegion;
            *s = s1;
        }
    }

    *regionType = closestRegionType;
    return sep;
}

VoronoiRegionType
ConvexHullMaximumFeature(const McdConvexHull * const convex, const MeVector3 d, MeI32 *regionIndex, const MeReal eps)
{
    // Start with an arbitrary edge
    MeI32 edgeIndex = convex->face->firstEdge;
    MeReal maxDH;
    do
    {
        // Get next vertex
        const McdCnvVertex * v = convex->vertex + convex->edge[edgeIndex].toVert;

        // Check each adjoining edge
        {   // Initialize with first edge
            edgeIndex = convex->edgeIndex[v->firstEdgeIndex];
            const McdCnvEdge * const e = convex->edge + edgeIndex;
            MeVector3 edgeDisp;
            MeVector3Subtract(edgeDisp, convex->vertex[e->toVert].position, v->position);
            maxDH = MeVector3Dot(edgeDisp, d);
        }
        for (MeI32 i = v->firstEdgeIndex+1; i < (v+1)->firstEdgeIndex; ++i)
        {
            const MeI32 index = convex->edgeIndex[i];
            const McdCnvEdge * const e = convex->edge + index;
            MeVector3 edgeDisp;
            MeVector3Subtract(edgeDisp, convex->vertex[e->toVert].position, v->position);
            const MeReal dh = MeVector3Dot(edgeDisp, d);
            if (dh > maxDH-eps)
            {   // We will always keep the last edge close to max.  This way, if there are two
                // maximum edges (within epsilon), the coresponding maximum face will be to the right
                // of the kept edge.
                edgeIndex = index;
                maxDH = dh;
            }
        }
    } while (maxDH > eps);

    if (maxDH < -eps)
    {   // The vertex is the maximum feature
        *regionIndex = convex->edge[edgeIndex].fromVert;
        return kVertexRegion;
    }

    const MeI32 rightFace = convex->edge[edgeIndex].rightFace;

    if (MeVector3Dot(d, convex->face[rightFace].normal) < 1-eps)
    {   // The edge is the maximum feature
        *regionIndex = edgeIndex;
        return kEdgeRegion;
    }

    // The face is the maximum feature
    *regionIndex = rightFace;
    return kFaceRegion;
}

VoronoiRegionType
ConvexHullVoronoiRegion(const McdConvexHull * const convex, const MeVector3 pos, MeI32 * regionIndex)
{
    MeReal h;
    MeI32 i;

    // Start by finding face we are farthest outside
    MeI32 faceIndex;
    const McdCnvFace * face = convex->face;
    {
        h = MeVector3Dot(pos, face->normal);
        const MeI32 vIndex = convex->edge[face->firstEdge].fromVert;
        h -= MeVector3Dot(convex->vertex[vIndex].position, face->normal);
        faceIndex = 0;
    }
    for (i = 1; i < convex->numFace; ++i)
    {
        ++face;
        MeReal hi = MeVector3Dot(pos, face->normal);
        const MeI32 vIndex = convex->edge[face->firstEdge].fromVert;
        hi -= MeVector3Dot(convex->vertex[vIndex].position, face->normal);
        if (hi > h)
        {
            h = hi;
            faceIndex = i;
        }
    }

    if (h < 0)
    {
        *regionIndex = 0;
        return kInteriorRegion;
    }

    // See which edge we are farthest outside
    MeI32 edgeIndex;
    face = convex->face+faceIndex;
    const McdCnvEdge * edge = convex->edge+face->firstEdge;
    {
        MeVector3 edgeDir;
        MeVector3Subtract(edgeDir, convex->vertex[edge->toVert].position,
                                   convex->vertex[edge->fromVert].position);
        MeVector3Scale(edgeDir, edge->invLength);
        MeVector3 en;
        MeVector3Cross(en, edgeDir, face->normal);
        MeVector3 r;
        MeVector3Subtract(r, pos, convex->vertex[edge->fromVert].position);
        h = MeVector3Dot(r, en);
        edgeIndex = face->firstEdge;
    }
    for (i = face->firstEdge+1; i < (face+1)->firstEdge; ++i)
    {
        ++edge;
        MeVector3 edgeDir;
        MeVector3Subtract(edgeDir, convex->vertex[edge->toVert].position,
                                   convex->vertex[edge->fromVert].position);
        MeVector3Scale(edgeDir, edge->invLength);
        MeVector3 en;
        MeVector3Cross(en, edgeDir, face->normal);
        MeVector3 r;
        MeVector3Subtract(r, pos, convex->vertex[edge->fromVert].position);
        const MeReal hi = MeVector3Dot(r, en);
        if (hi > h)
        {
            h = hi;
            edgeIndex = i;
        }
    }

    if (h < 0)
    {
        *regionIndex = faceIndex;
        return kFaceRegion;
    }

    // See which vertex we are farthest outside
    edge = convex->edge+edgeIndex;
    MeVector3 edgeDisp;
    MeVector3Subtract(edgeDisp, convex->vertex[edge->toVert].position,
                                convex->vertex[edge->fromVert].position);
    MeVector3 r;
    MeVector3Subtract(r, pos, convex->vertex[edge->fromVert].position);
    h = MeVector3Dot(r, edgeDisp);

    if (h <= 0)
    {
        *regionIndex = edge->fromVert;
        return kVertexRegion;
    }

    const MeReal edgeDisp2 = MeVector3Dot(edgeDisp, edgeDisp);

    if (h >= edgeDisp2)
    {
        *regionIndex = edge->toVert;
        return kVertexRegion;
    }

    *regionIndex = edgeIndex;
    return kEdgeRegion;
}

/*
    Support functions for ConvexHullNSegment:

    ClosestInVertexRegion
    ClosestInEdgeRegion
    ClosestInFaceRegion
    ClosestInInteriorRegion

    For each function:
        If the closest point(s) of approach is found within the present region, then:
            Sets cp to closest point on convex hull
            Sets *s to min. parameter value of closest approach.
            Sets *ds to difference between max. parameter of closest approach and *s.
            Returns true.
        Otherwise:
            Sets *s to parameter where line segment leaves the present region.
            Sets *regionType and *regionIndex to proper values for the next region.
            Returns false.

    Note: upon return, *s will never be less than its original value, nor will it exceed smax.

    N.B. The direction vector d must be normalized.
 */
static MeBool
ClosestInVertexRegion(const McdConvexHull * const convex,
                      const MeVector3 p, const MeVector3 d, const MeReal smax,
                      MeVector3 cp, MeReal * const s, MeReal * const ds,
                      VoronoiRegionType * regionType, MeI32 * regionIndex)
{
    McdCnvVertex *v = convex->vertex + *regionIndex;

    // Segment-vertex displacement
    MeVector3 r;
    MeVector3Subtract(r, v->position, p);

    // Find parameter of closest approach
    // (p+s*d-v).d = 0  =>  -r.d+s = 0  =>  s = r.d
    MeReal sClosestNum = MeVector3Dot(r, d);

    // Here, we will implicitly assume that the denomenator of sClosest is 1.

    // Done if we're already past the closest point
    const MeReal smin = *s;
    if (smin > sClosestNum+PARALLEL_THRESH_EPS)
    {
        MeVector3Copy(cp, v->position);
        *ds = 0;
        return true;
    }

    // Clamp closest point parameter to smax
    if (smax < sClosestNum)
    {
        sClosestNum = smax;
    }

    // From now on, the denomenator of sClosest may not be 1.
    MeReal sClosestDen = 1;

    // Check each adjoining edge
    MeI32 nextRegionIndex = -1;
    for (MeI32 i = v->firstEdgeIndex; i < (v+1)->firstEdgeIndex; ++i)
    {
        const MeI32 index = convex->edgeIndex[i];
        const McdCnvEdge * const e = convex->edge + index;

        // Find parameter of edge end-plane crossing
        // (p+s*d-v).e = 0 => -r.e+s*(d.e) = 0 => s = r.e/d.e
        MeVector3 edgeDisp;
        MeVector3Subtract(edgeDisp, convex->vertex[e->toVert].position, v->position);
        const MeReal sExitDen = MeVector3Dot(d, edgeDisp);
        if (sExitDen < PARALLEL_THRESH_EPS)
        {   // Ignore if line direction points away from (or parallel to) plane
            continue;
        }
        const MeReal sExitNum = MeVector3Dot(r, edgeDisp);
//        if (smin*sExitDen >= sExitNum)
//        {   // Ignore if crossing is before our present line parameter
//            continue;
//        }
        if ((sExitNum-PARALLEL_THRESH_EPS)*sClosestDen < sClosestNum*sExitDen)
        {   // sExit < sClosest, so we have found a new closest point (within
            // the present region).  This means we need to go on to the next region.
            sClosestNum = sExitNum;
            sClosestDen = sExitDen;
            nextRegionIndex = index;
        }
    }

    // The closest point within this region
    *s = sClosestNum/sClosestDen;

    if (nextRegionIndex < 0)
    {   // Closest point is properly within this region
        MeVector3Copy(cp, v->position);
        *ds = 0;
        return true;
    }

    // On to the next region
    *regionIndex = nextRegionIndex;
    *regionType = kEdgeRegion;
    return false;
}

static MeBool
ClosestInEdgeRegion(const McdConvexHull * const convex,
                    const MeVector3 p, const MeVector3 d, const MeReal smax,
                    MeVector3 cp, MeReal * const s, MeReal * const ds,
                    VoronoiRegionType * regionType, MeI32 * regionIndex)
{
    McdCnvEdge *e = convex->edge + *regionIndex;
    McdCnvVertex *v = convex->vertex + e->fromVert;

    // Edge length squared and normalized direction
    MeVector3 edgeDir;
    MeVector3Subtract(edgeDir, convex->vertex[e->toVert].position, convex->vertex[e->fromVert].position);
    const MeReal edgeLength2 = MeVector3Dot(edgeDir, edgeDir);
    MeVector3Scale(edgeDir, e->invLength);

    // Segment-edge displacement
    MeVector3 r;
    MeVector3Subtract(r, v->position, p);

    // Necessary dot-products
    const MeReal de = MeVector3Dot(d, edgeDir);
    const MeReal det = 1 - de*de;
    const MeReal er = MeVector3Dot(edgeDir, r);

    // Determines whether we can have multiple solutions
    const MeBool parallel = det <= PARALLEL_THRESH_EPS;

    // Find parameters of closest approach
    MeReal sClosestNum;
    MeReal sClosestDen = 1;
    MeI32 nextRegionIndex = -1;
    VoronoiRegionType nextRegionType;
    const MeReal smin = *s;
    if (!parallel)
    {   // Segment and edge are askew.
        sClosestNum = (MeVector3Dot(d, r)-de*er)/det;

        // Here, we will implicitly assume that the denomenator of sClosest is 1.

        // Done if we're already past the closest point
        if (smin > sClosestNum+PARALLEL_THRESH_EPS)
        {
            MeVector3ScaleAndAdd(cp, v->position, smin*de - er, edgeDir);
            *ds = 0;
            return true;
        }

        // Clamp closest point parameter to smax
        if (smax < sClosestNum)
        {
            sClosestNum = smax;
        }

        // From now on, the denomenator of sClosest may not be 1.

        // Check adjoining faces
        {   // Right face
            const McdCnvFace * const f = convex->face + e->rightFace;
            // Find border-plane normal
            MeVector3 n;
            MeVector3Cross(n, edgeDir, f->normal);
            const MeReal sExitDen = MeVector3Dot(d, n);
            if (sExitDen >= PARALLEL_THRESH_EPS)
            {   // Line direction points into plane
                const MeReal sExitNum = MeVector3Dot(r, n);
//                if (smin*sExitDen < sExitNum)
//                {   // Line crossing is after our present parameter
                    if ((sExitNum-PARALLEL_THRESH_EPS)*sClosestDen < sClosestNum*sExitDen)
                    {   // sExit < sClosest, so we have found a new closest point (within
                        // the present region).  This means we need to go on to the next region.
                        sClosestNum = sExitNum;
                        sClosestDen = sExitDen;
                        nextRegionIndex = e->rightFace;
                        nextRegionType = kFaceRegion;
                    }
//                }
            }
        }
        {   // Left face
            const McdCnvFace * const f = convex->face + e->leftFace;
            // Find border-plane normal
            MeVector3 n;
            MeVector3Cross(n, f->normal, edgeDir);
            const MeReal sExitDen = MeVector3Dot(d, n);
            if (sExitDen >= PARALLEL_THRESH_EPS)
            {   // Line direction points into plane
                const MeReal sExitNum = MeVector3Dot(r, n);
//                if (smin*sExitDen < sExitNum+PARALLEL_THRESH_EPS)
//                {   // Line crossing is after our present parameter
                    if ((sExitNum-PARALLEL_THRESH_EPS)*sClosestDen < sClosestNum*sExitDen)
                    {   // sExit < sClosest, so we have found a new closest point (within
                        // the present region).  This means we need to go on to the next region.
                        sClosestNum = sExitNum;
                        sClosestDen = sExitDen;
                        nextRegionIndex = e->leftFace;
                        nextRegionType = kFaceRegion;
                    }
//                }
            }
        }
    } else
    {   // Segment is parallel with the edge.  Choose endpoint.
        sClosestNum = smax;
    }

    // Check edge end-planes
    if (de > PARALLEL_THRESH_EPS)
    {   // Segment points towards edge max-point (n = edge/|edge|)
        // (p + s*d - v).n = |edge|  =>  -r.n + s*d.n = |edge|  => s = (r.n+|edge|)/d.n = (er+|edge|)/de
        const MeReal edgeLength = edgeLength2*e->invLength;
        const MeReal sExitNum = er+edgeLength;
        if (sExitNum*sClosestDen < sClosestNum*de)
        {   // sExit < sClosest, so we have found a new closest point (within
            // the present region).  This means we need to go on to the next region.
            sClosestNum = sExitNum;
            sClosestDen = de;
            nextRegionIndex = e->toVert;
            nextRegionType = kVertexRegion;
        }
    } else
    if (de < -PARALLEL_THRESH_EPS)
    {   // Segment points towards edge min-point (n = -edge/|edge|)
        // (p + s*d - v).n = 0  =>  -r.n + s*d.n = 0  => s = r.n/d.n = er/de
        // Flip inequality since de < 0
        if (er*sClosestDen > sClosestNum*de)
        {   // sExit < sClosest, so we have found a new closest point (within
            // the present region).  This means we need to go on to the next region.
            sClosestNum = -er;
            sClosestDen = -de;
            nextRegionIndex = e->fromVert;
            nextRegionType = kVertexRegion;
        }
    }

    // The closest point within this region
    const MeReal sClosest = sClosestNum/sClosestDen;

    if (!parallel && nextRegionIndex >= 0)
    {
        // On to the next region
        *s = sClosest;
        *regionIndex = nextRegionIndex;
        *regionType = nextRegionType;
        return false;
    }

    if (parallel)
    {
        *ds = sClosest - smin;
    } else
    {
        *s = sClosest;
        *ds = 0;
    }

    MeVector3ScaleAndAdd(cp, v->position, (*s)*de - er, edgeDir);
    return true;
}

static MeBool
ClosestInFaceRegion(const McdConvexHull * const convex,
                    const MeVector3 p, const MeVector3 d, const MeReal smax,
                    MeVector3 cp, MeReal * const s, MeReal * const ds,
                    VoronoiRegionType * regionType, MeI32 * regionIndex)
{
    McdCnvFace *f = convex->face + *regionIndex;
    McdCnvVertex *v = convex->vertex + convex->edge[f->firstEdge].fromVert;

    // Segment-face displacement
    MeVector3 r;
    MeVector3Subtract(r, v->position, p);

    // Necessary dot-products
    const MeReal dn = MeVector3Dot(d, f->normal);
    const MeReal rn = MeVector3Dot(r, f->normal);

    // Determine whether or not the segment is parallel with the face
    MeBool parallel;
    const MeReal smin = *s;
    VoronoiRegionType nextRegionType;
    MeI32 nextRegionIndex;
    MeReal sClosestNum;
    MeReal sClosestDen;
    if (dn > PARALLEL_THRESH_EPS)
    {   // Line segment points away from the face
        MeVector3ScaleAndAdd(cp, p, smin, d);
        MeVector3ScaleAndAdd(cp, cp, rn-smin*dn, f->normal);
        *ds = 0;
        return true;
    } else
    {
        parallel = dn >= -PARALLEL_THRESH_EPS;
        if (!parallel && smax*dn <= rn)
        {   // Line segment intersects face plane
            // Start with parameter of plane crossing, clamped to smax
            // (p+s*d-v).n = 0  =>  -r.n+s*(d.n) = 0  =>  s = r.n/d.n = -r.n/-d.n
            nextRegionType = kInteriorRegion;
            nextRegionIndex = 0; // Not used (yet) for interior region
            sClosestNum = -rn;
            sClosestDen = -dn;
        } else
        {   // Line segment will not reach face
            nextRegionIndex = -1;
            sClosestNum = smax;
            sClosestDen = 1;
        }
    }

    // Check each adjoining edge
    for (MeI32 index = f->firstEdge; index < (f+1)->firstEdge; ++index)
    {
        const McdCnvEdge * const e = convex->edge + index;

        // Get edge start point and displacement
        const McdCnvVertex * const vi = convex->vertex + e->fromVert;
        MeVector3 edgeDisp;
        MeVector3Subtract(edgeDisp, convex->vertex[e->toVert].position, vi->position);

        // Find border-plane normal (unnormalized)
        MeVector3 n;
        MeVector3Cross(n, edgeDisp, f->normal);
        const MeReal sExitDen = MeVector3Dot(d, n);
        if (sExitDen < PARALLEL_THRESH_EPS)
        {   // Ignore if line direction points away from (or parallel to) plane
            continue;
        }
        // Displacement to this plane
        MeVector3 ri;
        MeVector3Subtract(ri, vi->position, p);
        const MeReal sExitNum = MeVector3Dot(ri, n);
//        if (smin*sExitDen >= sExitNum)
//        {   // Ignore if crossing is before our present line parameter
//            continue;
//        }
        if (sExitNum*sClosestDen < sClosestNum*sExitDen)
        {   // sExit < sClosest, so we have found a new closest point (within
            // the present region).  This means we need to go on to the next region.
            sClosestNum = sExitNum;
            sClosestDen = sExitDen;
            nextRegionIndex = index;
            nextRegionType = kEdgeRegion;
        }
    }

    // The closest point within this region
    const MeReal sClosest = sClosestNum/sClosestDen;

    if (!parallel && nextRegionIndex >= 0)
    {
        // On to the next region
        *s = sClosest;
        *regionIndex = nextRegionIndex;
        *regionType = nextRegionType;
        return false;
    }

    if (parallel)
    {
        *ds = sClosest - smin;
    } else
    {
        *s = sClosest;
        *ds = 0;
    }

    MeVector3ScaleAndAdd(cp, p, (*s), d);
    MeVector3ScaleAndAdd(cp, cp, rn-(*s)*dn, f->normal);
    return true;
}

static MeBool
ClosestInInteriorRegion(const McdConvexHull * const convex,
                        const MeVector3 p, const MeVector3 d, const MeReal smax,
                        MeVector3 cp, MeReal * const s, MeReal * const ds,
                        VoronoiRegionType * regionType, MeI32 * regionIndex)
{
    // We will not look for the next region here.  We will define all interior
    // points to have zero distance from the convex shape, so that this region
    // will be the last one traversed if we come to it.

    // Check each face
    const MeReal smin = *s;
    MeReal sClosestNum = smax;
    MeReal sClosestDen = 1;
    for (MeI32 index = 0; index < convex->numFace; ++index)
    {
        const McdCnvFace * const f = convex->face + index;

        // Normal dot product
        const MeReal dn = MeVector3Dot(d, f->normal);

        if (dn < PARALLEL_THRESH_EPS)
        {   // Ignore if line direction points away from (or parallel to) plane
            continue;
        }

        const McdCnvEdge * const e = convex->edge + f->firstEdge;
        const McdCnvVertex * const v = convex->vertex + e->fromVert;

        // Displacement to plane
        MeVector3 r;
        MeVector3Subtract(r, v->position, p);
        const MeReal rn = MeVector3Dot(r, f->normal);

        // Find parameter of plane crossing
        // (p+s*d-v).n = 0 => -r.n+s*(d.n) = 0 => s = r.n/d.n
//        if (smin*dn >= rn)
//        {   // Ignore if crossing is before our present line parameter
//            continue;
//        }

        if (rn*sClosestDen < sClosestNum*dn)
        {   // sExit < sClosest, so we have found a new closest point (within
            // the present region).  This means we need to go on to the next region.
            sClosestNum = rn;
            sClosestDen = dn;
        }
    }

    MeVector3ScaleAndAdd(cp, p, smin, d);
    *ds = sClosestNum/sClosestDen-smin;
    return true;
}

MeReal
ConvexHullNSegment(const McdConvexHull * const convex,
                   const MeVector3 p, const MeVector3 d, const MeReal smin, const MeReal smax,
                   MeVector3 cp, MeReal * s, VoronoiRegionType * closestRegionType)
{
    // Find starting region
    MeVector3 sp;
    MeVector3ScaleAndAdd(sp, p, smin, d);
    MeI32 regionIndex;
    VoronoiRegionType regionType = ConvexHullVoronoiRegion(convex, sp, &regionIndex);

    // Start at one end of the segment and walk the regions; stop when we've found the closest approach
    MeReal ds;
    *s = smin;
    while
    (
        (regionType & 2) ?
        (
            (regionType & 1) ?
                !ClosestInInteriorRegion(convex, p, d, smax, cp, s, &ds, &regionType, &regionIndex) :
                !ClosestInFaceRegion(convex, p, d, smax, cp, s, &ds, &regionType, &regionIndex)
        ) :
        (
            (regionType & 1) ?
                !ClosestInEdgeRegion(convex, p, d, smax, cp, s, &ds, &regionType, &regionIndex) :
                !ClosestInVertexRegion(convex, p, d, smax, cp, s, &ds, &regionType, &regionIndex)
        )
    ) {}

    *closestRegionType = regionType;

    return ds;
}
