/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.6.2.7 $

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

#include <McdProfile.h>

#include <McdGjk.h>
#include <McdGjkMaximumPoint.h>
#include <MeSet.h>
#include <McdPlaneIntersect.h>

/****************************************************************************
  This returns the intersection of a plane with a box.

  The output is a set of co-planar points in WRF.
  This function is most efficient when the number of points on the positive
  side of the plane is less than the number of points on the negative side.
*/
void McdBoxGetSlice(McdGeometryInstanceID ins,
                    const MeVector3 normal,
                    MeReal dist, 
                    int maxVert,
                    int *numVert,
                    MeVector3 *outVert)
{
    McdBox *box = (McdBox *) ins->mGeometry;
    MeReal dpp[8];    // distance from point to plane (-999 = not computed)
    MeVector3 v[8], temp, norm;
    int i,j,k;

    MEASSERT(McdGeometryGetType(&box->m_g) == kMcdGeometryTypeBox);

    //  Transform the plane into instance reference frame

    MeMatrix4TMInverseRotate(norm, ins->mTM, normal);
    dist -= MeVector3Dot(normal, ins->mTM[3]);

    //  For efficiency, make norm point to the smaller half of the shape
    if (dist < 0)
    {
        dist = -dist;
        MeVector3Scale(norm, -1);
    }

    //  Pick best starting corner

    for (i=0; i<3; ++i)
        temp[i] = norm[i]<0 ? -box->mR[i] : box->mR[i];

    //  Compute corners

    for (i=0; i<8; ++i)
    {
        MeVector3Set(v[i], i&1?-temp[0]:temp[0], 
                           i&2?-temp[1]:temp[1], 
                           i&4?-temp[2]:temp[2]);
        dpp[i] = -999;       // flag -999 = not computed
    }

    //  Walk the box and test for edges which cross the plane

    *numVert = 0;
    dpp[0] = MeVector3Dot(v[0], norm) - dist;

    //  The "fat" is a parameter I experimented with to see if it
    //  might give more stable resting contact, i.e broader contacts

    MeReal fat = 0;       // -0.01f

    for (i=0; i<7; ++i)
    {
        if (dpp[i] < fat) 
            continue;
        for (j=1; j<8; j<<=1)       // j = { 1, 2, 4 }
        {
            if (i == (k = i|j)) 
                continue;
            if (dpp[k] == -999)
                dpp[k] = MeVector3Dot(v[k], norm) - dist;
            if (dpp[k] < fat)
            {
                MeVector3MultiplyScalar(temp, v[k], dpp[i]);
                MeVector3MultiplyAdd(temp, -dpp[k], v[i]);
                //  This division is safe because dpp[i] >= 0 and dpp[k] < 0
                MeVector3Scale(temp, 1 / (dpp[i] - dpp[k]));
                MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
                if (++*numVert == maxVert)
                    return;                  //  RETURN - NO MORE ROOM
            }
        }
    }
}


/****************************************************************************
  This computes the result of cutting a convex with a plane.
    (norm, dp) locate the plane in LOCAL ref frame.
  If flags & 1 this returns the intersection points of edges with the plane.
  If flags & 2 this returns the vertices on the positive side of the plane.
    tm is used to transform the OUTPUT points into WRF.
  This function is most efficient when the number of points on the positive
  side of the plane is less than the number of points on the negative side.
*/

void McdConvexMeshPlaneCut(McdConvexMesh *conv,
                           const MeVector3 norm,
                           MeReal dp, 
                           int flags,
                           const MeMatrix4 tm,
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

    MEASSERT(conv && numVert && outVert);
    MEASSERT(McdGeometryGetType(&conv->m_g) == kMcdGeometryTypeConvexMesh);

    *numVert = 0;
    if (maxVert < 1)      // RETURN -- NO ROOM
        return;

    //  pick a starting vertex with distance at least dp
    dv = McdConvexMeshMaximumPointLocal(conv, norm, 0, dp, &start);

    if (dv < dp)
        return;            // RETURN - NO INTERSECTION

    //  Get pointer to the polyhedron vertices
    vert = conv->mHull.vertex;
    nv = conv->mHull.numVertex;

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
            MeMatrix4TMTransform(outVert[*numVert], tm, vert[v].position);
            if (++*numVert==maxVert)
                return;               // RETURN -- NO MORE ROOM
        }

        m = McdCnvVertexGetCount(&conv->mHull, v);

        for (i = 0; i < m; ++i)
        {
            u = McdCnvVertexGetNeighbor(&conv->mHull, v, i);

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
                MeMatrix4TMTransform(outVert[*numVert], tm, temp);

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

/****************************************************************************
  This returns the intersection of a plane with a convex mesh.
  The output is a set of co-planar points in WRF.
*/
void McdConvexMeshGetSlice(McdGeometryInstanceID ins,
                           const MeVector3 normal,
                           MeReal dist, 
                           int maxVert,
                           int *numVert,
                           MeVector3 *outVert)
{
    MeVector3 norm;
    MeReal dp;

    McdConvexMesh *conv = (McdConvexMesh *) ins->mGeometry;

    MEASSERT(McdGeometryGetType(&conv->m_g) == kMcdGeometryTypeConvexMesh);

    *numVert = 0;
    if (maxVert < 1)      // RETURN -- NO ROOM
        return;

    //  transform the plane into local reference frame (LRF).
    MeMatrix4TMInverseRotate(norm, ins->mTM, normal);

    //  dp is the signed distance from the LRF origin to the plane along norm.
    dp = dist - MeVector3Dot(normal, ins->mTM[3]);

    //  for efficiency, make norm point to the smaller half of the shape
    if (dp < 0)
    {
        //  NOTE!!!  This code assumes that the LRF origin is inside the convex
        //  mesh.  If not, it might still work but it will be less efficient.

        dp = -dp;
        MeVector3Scale(norm, -1);
    }

    McdConvexMeshPlaneCut(conv, norm, dp, 1, ins->mTM, maxVert, numVert, outVert);
}
    
/****************************************************************************
  This returns the intersection of a plane with a cylinder.

  The output is a set of co-planar points in WRF.
*/
void McdCylinderGetSlice(McdGeometryInstanceID ins,
                         const MeVector3 normal,
                         MeReal dist, 
                         int maxVert,
                         int *numVert,
                         MeVector3 *outVert)
{
    MeVector3 norm;
    MeVector3 temp;
    MeReal dp;

    McdCylinder *cyl = (McdCylinder *) ins->mGeometry;

    MEASSERT(McdGeometryGetType(&cyl->m_g) == kMcdGeometryTypeCylinder);

    *numVert = 0;
    if (maxVert < 1)      // RETURN -- NO ROOM
        return;

    //  dp is the signed distance from the LRF origin to the plane along norm.
    dp = dist - MeVector3Dot(normal, ins->mTM[3]);

    //  Transform the plane into local reference frame (LRF).
    MeMatrix4TMInverseRotate(norm, ins->mTM, normal);

    //  Check if normal is perpendicular to the z-axis, if so special case
    if (MeFabs(norm[2]) < ME_MEDIUM_EPSILON)
    {
        //  Test no intersection
        if (MeFabs(dp) > cyl->mR + ME_SMALL_EPSILON)
            return;                               //  RETURN NO POINTS

        //  Compute 2 contact locations
        MeVector3Copy(temp, ins->mTM[3]);
        MeVector3MultiplyAdd(temp, dp, normal);
        MeVector3ScaleAndAdd(outVert[*numVert], temp, cyl->mRz, ins->mTM[2]);
        if (++*numVert >= maxVert)
            return;
        MeVector3ScaleAndAdd(outVert[*numVert], temp, -cyl->mRz, ins->mTM[2]);
        ++*numVert;
        return;
    }

    //  get elipse minor and major axis vectors

    MeVector3 minor, major;
    MeReal zof, a, b, c, t;

    MeVector3CrossAxis(minor, norm, 2);
    MeVector3Normalize(minor);
    MeVector3Cross(major, norm, minor);
    if (major[2]*norm[2] < 0)
        MeVector3Scale(major, -cyl->mR / norm[2]);
    else
        MeVector3Scale(major, cyl->mR / norm[2]);
    MeVector3Scale(minor, cyl->mR);

    zof = dp / norm[2];        // z-axis intercept of the elipse

    //  Check if normal is parallel to the z-axis, if so special case

    if (major[2] < ME_SMALL_EPSILON)
    {
        if (MeFabs(zof) > cyl->mRz + ME_SMALL_EPSILON)
            return;

        //  add four points

        MeVector3Set(temp, cyl->mR, cyl->mR, zof);
        MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
        if (++*numVert >= maxVert) 
            return;
        MeVector3Set(temp, cyl->mR,-cyl->mR, zof);
        MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
        if (++*numVert >= maxVert) 
            return;
        MeVector3Set(temp,-cyl->mR, cyl->mR, zof);
        MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
        if (++*numVert >= maxVert) 
            return;
        MeVector3Set(temp,-cyl->mR,-cyl->mR, zof);
        MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
        if (++*numVert >= maxVert) 
            return;
    }
    else 
    {
        t = cyl->mRz - zof;
        a = t / major[2];

        if (a >= 1)
        {
            // add one point
            MeVector3Set(temp, major[0], major[1], major[2] + zof);
            MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
            if (++*numVert >= maxVert) 
                return;
        }
        else if (a > -1)    // a = [ -1 .. +1 ]
        {
            // add two points
            b = MeSqrt(1 - a*a);
            MeVector3MultiplyScalar(temp, major, a);
            MeVector3MultiplyAdd(temp, b, minor);
            temp[2] += zof;
            MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
            if (++*numVert >= maxVert) 
                return;
            MeVector3MultiplyAdd(temp, -2*b, minor);
            MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
            if (++*numVert >= maxVert) 
                return;
        }

        t = cyl->mRz + zof;
        c = t / major[2];

        if (c >= 1)
        {
            // add one point
            MeVector3Set(temp, -major[0], -major[1], -major[2] + zof);
            MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
            if (++*numVert >= maxVert) 
                return;
        }
        else if (c > -1)    // c = [ -1 .. +1 ]
        {
            // add two points
            b = MeSqrt(1 - c*c);
            MeVector3MultiplyScalar(temp, major, -c);
            MeVector3MultiplyAdd(temp, b, minor);
            temp[2] += zof;
            MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
            if (++*numVert >= maxVert) 
                return;
            MeVector3MultiplyAdd(temp, -2*b, minor);
            MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
            if (++*numVert >= maxVert) 
                return;
        }

        //  add minor axis end points

        if (a > .2f && c > .2f)
        {
            MeVector3Set(temp, minor[0], minor[1], zof);
            MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
            if (++*numVert >= maxVert) 
                return;
            MeVector3Set(temp, -minor[0], -minor[1], zof);
            MeMatrix4TMTransform(outVert[*numVert], ins->mTM, temp);
            if (++*numVert >= maxVert) 
                return;
        }
    }
}

/****************************************************************************
  This is a virtual function, eventually it should be replaced by a function 
  pointer in the framework (ala McdGeometryInstanceMaximumPoint).
*/
void McdGeometryInstanceGetSlice(McdGeometryInstanceID ins,
                                 const MeVector3 normal,
                                 MeReal dist, 
                                 int maxVert,
                                 int *numVert,
                                 MeVector3 *outVert)
{
    MeReal d2;
    *numVert = 0;
    if (maxVert < 1)
        return;

    switch (McdGeometryGetType(ins->mGeometry))
    {
    case kMcdGeometryTypeBox:
        McdBoxGetSlice(ins, normal, dist, maxVert, numVert, outVert);
        break;
    case kMcdGeometryTypeConvexMesh:
        McdConvexMeshGetSlice(ins, normal, dist, maxVert, numVert, outVert);
        break;
    case kMcdGeometryTypeCylinder:
        McdCylinderGetSlice(ins, normal, dist, maxVert, numVert, outVert);
        break;
    case kMcdGeometryTypeSphere:
    case kMcdGeometryTypePlane:
    case kMcdGeometryTypeAggregate:
        *numVert = 1;
        d2 = dist - MeVector3Dot(ins->mTM[3], normal);
        MeVector3Copy(*outVert, ins->mTM[3]);
        MeVector3MultiplyAdd(*outVert, d2, normal);
        break;
    case kMcdGeometryTypeNull:
    case kMcdGeometryTypeTriangleList:
        MEASSERT(!"NOT IMPLEMENTED - McdGeometryInstanceGetSlice");
        break;
    }
}


/****************************************************************************
  This tests for collision of any geometry with a plane. 
  modplane is a plane geometry, and mod is another non-plane.

  Return 1 if the models are touching, and return 0 if not.  
  Also this sets result-> { touch, normal, contacts, contactCount }.
*/
int McdPlaneIntersectTest(McdModelID modplane, McdModelID mod, 
                          McdIntersectResult *result)
{
    McdGeometryInstanceID plane, ins;
    MeReal dist, sep, fat, dot, slice, pad, eps;
    MeVector3 norm, neg, p;
    MeVector3 vert[MCDCONTACTGEN_VSIZE];
    int numv, i;

    MEASSERT(McdGeometryGetType(modplane->mInstance.mGeometry)==kMcdGeometryTypePlane);
    MEASSERT(McdGeometryGetType(mod->mInstance.mGeometry)!=kMcdGeometryTypePlane);

    //  Get the plane normal and distance from origin

    plane = &modplane->mInstance;
    MeVector3Copy(norm, plane->mTM[2]);
    dist = MeVector3Dot(norm, plane->mTM[3]);
    eps = 0.005f * plane->mGeometry->frame->mScale;

    //  Get the other model maximum point and fatness

    ins = &mod->mInstance;
    MeVector3MultiplyScalar(neg, norm, -1);
    McdGjkMaximumPoint(ins, neg, p);
    fat = McdGjkFatness(ins);
    dot = MeVector3Dot(p, norm);
    pad = mod->mPadding + modplane->mPadding;

    //  dot is the level of the lowest point, dist is the level of the 
    //  floor, thus sep = the distance of separation from the floor.

    sep = dot - fat - dist;

    //  Test for no contact

    result->contactCount = 0;
    MeVector3Copy(result->normal, norm);
    result->touch = (sep <= pad);

    if (! result->touch)
        return 0;              // RETURN -- NO CONTACT

    //------------------------------------------------
    //  This generates contacts by taking a slice in the plane 
    //  and one at the deepest point.

    slice = dist + fat + eps;

    McdGeometryInstanceGetSlice(ins, norm, slice, MCDCONTACTGEN_VSIZE, &numv, vert);

    for (i = 0; i < numv && i < result->contactMaxCount; ++i)
    {
        MeVector3Copy(result->contacts[i].normal, norm);
        MeVector3ScaleAndAdd(result->contacts[i].position, vert[i], -fat, norm);
        result->contacts[i].separation = (sep > 0) ? 0 : sep;
    }

    MeVector3Copy(result->contacts[i].normal, norm);
    MeVector3ScaleAndAdd(result->contacts[i].position, p, -fat, norm);
    result->contacts[i].separation = (sep > 0) ? 0 : sep;

    result->contactCount = i + 1;
    return result->touch;
}

/****************************************************************************
  This is a collision detection function of any geometry vs. plane.
  This uses McdGeometryInstanceGetSlice and McdGjkMaximumPoint
  to generate contacts.
*/
int MEAPI
McdPlaneIntersect(McdModelPair* p, McdIntersectResult *result)
{
    //  See if either model is a plane

    if (McdGeometryGetType(p->model1->mInstance.mGeometry) == kMcdGeometryTypePlane)
    {
        const int result2 = McdPlaneIntersectTest(p->model1, p->model2, result);
        McdProfileEnd("McdPlaneIntersect");
        return result2;
    }
    else if (McdGeometryGetType(p->model2->mInstance.mGeometry) == kMcdGeometryTypePlane)
    {
        const int result2 = McdPlaneIntersectTest(p->model2, p->model1, result);
        McdProfileEnd("McdPlaneIntersect");
        return result2;
    }
    //  Force assertion failure

    MEASSERT(!"neither model is kMcdGeometryTypePlane");

    result->contactCount = 0;
    result->touch = 0;

    McdProfileEnd("McdPlaneIntersect");
    return 0;
}


