/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/26 10:04:54 $ - Revision: $Revision: 1.32.2.18 $

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

#include <lsVec3.h>
#include <MeAssert.h>
#include <McdQHullTypes.h>
#include <McdModel.h>
#include <McdGeometryTypes.h>
#include <McdConvexMesh.h>
#include <MeMath.h>
#include <McdModelPair.h>
#include <McdCheck.h>
#include <McdContact.h>
#include <ConvexGeomUtils.h>
#include <McdPolygonIntersection.h>
#include <MeSet.h>

#if 0
    #include <GjkMe.h>
    #include <CxTriangleNE.h>
    #include <ConvexConvex.h>
    #include <IxConvexConvex.h>
    #include <cnv.h>
#endif

#define EPSILON (1e-4f)



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

void McdConvexHullPlaneCut(McdConvexHull *hull,
                           const MeVector3 norm,
                           MeReal dp, 
                           int flags,
                           int maxVert,
                           int *numVert,
                           MeVector3 *outVert)
{
    McdCnvVertex *vert;
    MeVector3 temp;
    MeReal du, dv;
    int nv, i, start, u, v, m;
    MeSet set;
    MeDictNode nodemem[200];

    MEASSERT(hull && numVert && outVert);

    *numVert = 0;
    if (maxVert < 1)      // RETURN -- NO ROOM
        return;

    //  pick a starting vertex with distance at least dp
    dv = McdConvexHullMaximumPoint(hull, norm, 0, MEINFINITY, &start);

    if (dv < dp)
        return;            // RETURN - NO INTERSECTION

    //  Get pointer to the polyhedron vertices
    vert = hull->vertex;
    nv = hull->numVertex;

    //  Create a set and add first vertex
    MeSetInit(&set, nodemem, sizeof nodemem / sizeof *nodemem, 0);
    MeSetAdd(&set, (void*) start);

    //  Iterate over every vertex in the set in old-to-new order

    while (set.next)       // next==0 means iterator is finished
    {
        v = (int) MeSetIteratorNext(&set);

        dv = MeVector3Dot(norm, vert[v].position);

        MEASSERT(dv >= dp);

        if (flags & 2)
        {
            MeVector3Copy(outVert[*numVert], vert[v].position);
            if (++*numVert==maxVert)
                return;               // RETURN -- NO MORE ROOM
        }

        m = McdCnvVertexGetCount(hull, v);

        for (i = 0; i < m; ++i)
        {
            u = McdCnvVertexGetNeighbor(hull, v, i);

            //  If the point is on the positive half of the plane then
            //  add it to the set, otherwise compute where the edge crosses
            //  the plane.

            du = MeVector3Dot(norm, vert[u].position);
            if (du >= dp)
            {
                MeSetAdd(&set, (void*) u);
            }
            else if (flags & 1)
            {
                MEASSERT(du < dp && dv >= dp);

                //  Given points u and v with distances du<dp and dv>=dp
                //  compute the intersection point, r, of the edge (u,v) with
                //  the plane p.  r = u*(dv-dp)/(dv-du) + v*(dp-du)/(dv-du).

                MeVector3MultiplyScalar(temp, vert[u].position, (dv-dp)/(dv-du));
                MeVector3MultiplyAdd(temp, (dp-du)/(dv-du), vert[v].position);
                MeVector3Copy(outVert[*numVert], temp);

                if (!*numVert ||
                    !ME_ARE_EQUAL_3VEC(outVert[*numVert], outVert[*numVert - 1]))
                {
                    if (++*numVert==maxVert)
                        return;               // RETURN -- NO MORE ROOM
                }
            }
        }
    }
}


extern void
AccumulateSphylContacts(MeVector3 cp, MeVector3 normal,
                          MeReal sep, MeI16 segDim, MeI16 otherDim,
                            MeVector3Ptr axis, MeReal ds, MeMatrix4Ptr tm,
                              McdIntersectResult *result);

static void
GenerateTriangleContact(McdUserTriangle * tri, McdConvexHull * hull,
                         const MeReal fatness, MeMatrix4 tm, MeReal eps,
                                              McdIntersectResult *result)
{
    // Calculate triangle edges
    MeVector3 e[3];
    MeVector3Subtract(e[0], *tri->vertices[1], *tri->vertices[0]);
    MeVector3Subtract(e[1], *tri->vertices[2], *tri->vertices[1]);
    MeVector3Subtract(e[2], *tri->vertices[0], *tri->vertices[2]);

    // Triangle plane displacement
    MeReal dp = MeVector3Dot(*tri->vertices[0], *tri->normal) - fatness;

    int maxVert;
    MeReal zeroDisp = - McdConvexHullMaximumPoint(hull, *tri->normal, -1,
                                                     MEINFINITY, &maxVert);

    // Find separating plane
    MeReal sep = dp + zeroDisp;

    if (sep >= eps)
    {   // No contacts to be generated
        return;
    }

    MeVector3 n;
    MeVector3MultiplyScalar(n, *tri->normal, -1);

    // Find edge which generates better separating plane, if any
    MeI32 ei = -1;
    for (MeI32 i = 0; i < 3; ++i)
    {
        MeVector3 ni;
        VoronoiRegionType regionType;
        MeReal si;
        MeReal sepi = SegmentConvexHullSep(*tri->vertices[i], e[i], 0, 1,
                                                 hull, ni, &si, &regionType)
                      - fatness;

        MeReal width;
        if (si > 0 || si < 1)
        {   /* Only need to check the other vertex to see if the triangle
             *  penetrates the convex hull farther
             */
            const MeI32 nextEdge = NextMod3(i);
            width = MeVector3Dot(e[nextEdge], ni);
        }
        else
        {
            width = 0;
            MeReal wi;
            if ((wi = MeVector3Dot(e[i], ni)) < 0)
            {
                wi = MeVector3Dot(e[NextMod3(i)], ni);
                if (wi < 0)
                {
                    width = wi;
                }
            }
            else
            {
                wi = -MeVector3Dot(e[PrevMod3(i)], ni);
                if (wi < 0)
                {
                    width = wi;
                }
            }
        }
        if (width < 0)
        {
            sepi += width;
        }
        const MeReal dsep = sepi - sep;
        if (dsep > PARALLEL_THRESH_EPS)
        {
            if (sepi >= eps)
            {   // No contacts to be generated
                return;
            }
            sep = sepi;
            ei = i;
            MeVector3MultiplyScalar(n, ni, -1);
            MeVector3 sp;
            MeVector3ScaleAndAdd(sp, *tri->vertices[i], si, e[i]);
            zeroDisp = sepi + MeVector3Dot(sp, n);
        }
    }

    /* Separating plane is found, and the separation is less than eps.
     * Generate contacts.
     */
    const int count = result->contactCount;

    {   // Face test
        // Find deepest feature of convex hull
        MeI32 featureIndex;
        const VoronoiRegionType featureType =
            ConvexHullMaximumFeature(hull, *tri->normal, &featureIndex, eps);

        // Push a lot of space for the hull's vertices.  Far too much.
        MeVector3 *cnvV = (MeVector3*)MeMemoryALLOCA(
                                         hull->numVertex*sizeof(MeVector3)
                                      );
        int cnvVCount = 0;

        McdConvexHullPlaneCut(hull, *tri->normal, dp, 1, hull->numVertex,
                                                           &cnvVCount, cnvV);

        if (cnvVCount > 0)
        {
            // Generate contacts
            MeVector3 cOff;
            MeVector3MultiplyScalar(cOff, n, -fatness);
            for (MeI32 i = 0; i < cnvVCount; ++i)
            {
                MeI32 j;
                for (j = 0; j < 3; ++j)
                {   // Interior of triangle only
                    MeVector3 d;
                    MeVector3Subtract(d, cnvV[i], *tri->vertices[j]);
                    MeVector3 c;
                    MeVector3Cross(c, d, e[j]);
                    MeReal dist = MeVector3Dot(c, *tri->normal);
                    if (dist > 0 || dist*dist < eps*eps*MeVector3Dot(e[j],
                                                                        e[j]))
                    {   // point is within eps of edge
                        break;
                    }
                }
                if (j == 3)
                {
                    MeVector3Add(cnvV[i], cnvV[i], cOff);
                    AccumulateSphylContacts(cnvV[i], n, sep, 2, featureType, 
                                                         NULL, 0, tm, result);
                }
            }
        }
    }

    {   // Edge tests
        for (MeI32 i = 0; i < 3; ++i)
        {
            if (((tri->flags>>(2+i)) & 1) != 0)
            {
                // Edge is turned on
                // Intersect edge with hull
                MeVector3 cp;
                MeVector3 ep;
                VoronoiRegionType regionType;
                MeVector3 d;
                MeVector3Copy(d, e[i]);
                 /* ? Change ConvexHullNSegment -> ConvexHullSegment, so that
                  *   we don't need to normalize
                  */
                const MeReal len = MeVector3Normalize(d);
                MeReal es;
                MeReal ds = ConvexHullNSegment(hull, *tri->vertices[i], d, 0,
                                                    len, cp, &es, &regionType);
                const MeI16 segDim = (MeI16)(es > 0 && es < len);
                MeVector3ScaleAndAdd(ep, *tri->vertices[i], es, d);
                MeVector3 disp;
                MeVector3Subtract(disp, cp, ep);
                if (MeVector3Dot(disp, disp) < fatness*fatness+eps)
                {
                    if (es == 0)
                    {
                        MeVector3ScaleAndAdd(ep, ep, ds, d);
                        ds = 0;
                    }
                    MeReal sepi = zeroDisp - MeVector3Dot(ep, n);
                    if (sepi < eps)
                    {
                        // Generate contacts
                        AccumulateSphylContacts(ep, n, sepi, segDim,
                                        regionType, NULL, 0, tm, result);
                    }
                    if (ds > 0)
                    {
                        sepi -= ds*MeVector3Dot(d, n);
                        if (sepi < eps)
                        {
                            // Generate contacts
                            MeVector3ScaleAndAdd(ep, ep, ds, d);
                            AccumulateSphylContacts(ep, n, sepi, segDim,
                                            regionType, NULL, 0, tm, result);
                        }
                    }
                }
            }
        }
    }

    // Transfer triangle user data into contacts
    for (int c = count; c < result->contactCount; ++c)
    {
        result->contacts[c].element2.ptr = tri->triangleData.ptr;
    }
}

MeBool MEAPI
McdConvexMeshTriangleListIntersect(McdModelPair *p, McdIntersectResult *result)
{
    MeMatrix4Ptr tm1 = McdModelGetTransformPtr(p->model1);
    MeMatrix4Ptr tm2 = McdModelGetTransformPtr(p->model2);
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    McdConvexMesh *convexMesh = (McdConvexMesh*)McdModelGetGeometry(p->model1);
    McdConvexHull *convex = &convexMesh->mHull;

    McdTriangleListID trilistGeom = (McdTriangleListID)McdModelGetGeometry( p->model2 );
    McdTriangleList *triList = (McdTriangleList*)trilistGeom;

    result->contactCount = 0;
    result->touch = 0;

    // relTM maps from the tri list's frame into the convex hull's frame
    MeMatrix4 relTM;
    MeMatrix4TMInverseRotate(relTM[0], tm1, tm2[0]);
    MeMatrix4TMInverseRotate(relTM[1], tm1, tm2[1]);
    MeMatrix4TMInverseRotate(relTM[2], tm1, tm2[2]);
    MeMatrix4TMInverseTransform(relTM[3], tm1, tm2[3]);

    MeVector3 convexCenter;
    MeReal convexRadius;
    McdConvexMeshGetBSphere((McdGeometryID)convexMesh, convexCenter,
                                                           &convexRadius);
    MeVector3 convexCenterRelative;
    MeMatrix4TMInverseTransform(convexCenterRelative, relTM, convexCenter);
    triList->list = (McdUserTriangle *)MeMemoryALLOCA(
                                          triList->triangleMaxCount *
                                          sizeof(McdUserTriangle)
                                        );

    MeI32 count = (*triList->triangleListGenerator)(p, triList->list,
                                  convexCenterRelative, convexRadius,
                                            triList->triangleMaxCount);

    if (!count)
    {
        return 0;
    }

    MeVector3Set(result->normal, 0, 0, 0);

    for (MeI32 i = 0; i < count; ++i)
    {
        McdUserTriangle tri;
        MeVector3 vectors[4];
        MeI32 flags =  triList->list[i].flags;
        tri.triangleData = triList->list[i].triangleData;
        tri.normal = vectors;
        tri.vertices[0] = vectors+1;
        tri.vertices[1] = vectors+2;
        tri.vertices[2] = vectors+3;
        MeMatrix4TMRotate(vectors[0], relTM, *triList->list[i].normal);
        MeMatrix4TMTransform(vectors[1], relTM, *triList->list[i].vertices[0]);
        MeVector3 disp;
        MeVector3Subtract(disp, convexCenter, vectors[1]);
        // We will use the inward normal of the triangle, always pointing away from the convex mesh
        if (!(flags & kMcdTriangleTwoSided) || MeVector3Dot(disp, vectors[0]) >= 0)
        {   // Reverse order of vertices
            MeMatrix4TMTransform(vectors[3], relTM, *triList->list[i].vertices[1]);
            MeMatrix4TMTransform(vectors[2], relTM, *triList->list[i].vertices[2]);
            MeVector3Scale(vectors[0], -1);
            // Reverse flags!
            flags &= ~(kMcdTriangleUseEdge0|kMcdTriangleUseEdge2);
            flags |= (triList->list[i].flags&kMcdTriangleUseEdge0)<<2;
            flags |= (triList->list[i].flags&kMcdTriangleUseEdge2)>>2;
        } else
        {
            MeMatrix4TMTransform(vectors[2], relTM, *triList->list[i].vertices[1]);
            MeMatrix4TMTransform(vectors[3], relTM, *triList->list[i].vertices[2]);
        }
        tri.flags = (McdTriangleFlags)flags;
        GenerateTriangleContact(&tri, convex, convexMesh->mFatness, tm1, eps, result);
    }

    const MeReal n2 = MeVector3Dot(result->normal, result->normal);
    if (n2 > ME_MIN_EPSILON*ME_MIN_EPSILON)
    {
        MeVector3Scale(result->normal, MeRecipSqrt(n2));
    }

    return result->touch;
}

static void McdConvexMeshTriangleListTermAction(McdFrameworkID frame)
{
    //cnvFreeHull((McdConvexHull *)frame->cnvTriangle);
}


MeBool MEAPI
McdConvexMeshTriangleListRegisterInteraction(McdFramework *frame)
{
    MeReal v[3][3] = {{0,0,0}, {0,0,1}, {0,1,0}};
    MeReal n[3] = {1,0,0};
    
    McdInteractions interactions;
//    if(!frame->cnvTriangle)
//        frame->cnvTriangle = cnvCreateTriangle(v[0], v[1], v[2], n);
    
    interactions.helloFn = 0;
    interactions.goodbyeFn = 0;
    interactions.intersectFn = McdConvexMeshTriangleListIntersect;
    interactions.safetimeFn = 0;
    interactions.swap = 0;
    interactions.cull = 1;
    interactions.warned = 0;
    
    McdFrameworkSetInteractions(frame,
        kMcdGeometryTypeConvexMesh,
        kMcdGeometryTypeTriangleList,
        &interactions);

    McdFrameworkRegisterTermAction(frame,McdConvexMeshTriangleListTermAction);
    return 1;
}


