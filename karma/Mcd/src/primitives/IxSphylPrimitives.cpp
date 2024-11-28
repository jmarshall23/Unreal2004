/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/05/30 12:38:53 $ - Revision: $Revision: 1.20.2.11.4.1 $

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

#include <McdSphere.h>
#include <McdModel.h>
#include <McdContact.h>
#include <McdInteractionTable.h>
#include <McdPrimitives.h>
#include <Polynomial.h>
#include <GeomUtils.h>

inline void
OrderIndices(MeI32 *iA, MeI32 *iB, MeReal a[])
{
    const MeI32 o = -(MeI32)(a[*iA] > a[*iB]);
    const MeI32 d = ~o;
    const MeI32 l = d&(*iA)|o&(*iB);
    *iB = o&(*iA)|d&(*iB);
    *iA = l;
}

#if 0 // using AccumulateSphylContacts, below
static void
GenerateSphereContact(MeVector3 p0, MeReal r0, MeVector3 p1, MeReal r1, MeReal eps, McdIntersectResult *result)
{
    MeVector3 norm;
    
    MeVector3Subtract(norm, p0, p1);
    MeReal mag = MeVector3Normalize(norm);
    MeReal rSum = r0 + r1;
    MeReal sep = mag - rSum;
    
    if(sep < eps)
    {
        MCD_CHECK_ASSERT((result->contactMaxCount > result->contactCount), 
            kMcdErrorNum_contactBufferOverrun, "", "GenerateSphereContact");

        // Contact position is weighted average, based on relative radii.
        MeReal invRSum = 1/rSum;
        MeReal w0 = r1 * invRSum;
        MeReal w1 = r0 * invRSum;
        result->contacts[result->contactCount].position[0] = w0 * p0[0] + w1 * p1[0];
        result->contacts[result->contactCount].position[1] = w0 * p0[1] + w1 * p1[1];
        result->contacts[result->contactCount].position[2] = w0 * p0[2] + w1 * p1[2];

        MeVector3Copy(result->contacts[result->contactCount].normal, norm);
        result->contacts[result->contactCount].separation = sep;
        result->contacts[result->contactCount].dims = ((r1>0?3:0)<<8)|(r0>0?3:0);

        MeVector3Copy(result->normal, norm);

        result->contactCount++;
        result->touch = 1;
    }
}
#endif

void
AccumulateSphylContacts(MeVector3 cp, MeVector3 normal, MeReal sep, MeI16 segDim, MeI16 otherDim,
                        MeVector3Ptr axis, MeReal ds, MeMatrix4Ptr tm,
                        McdIntersectResult *result)
{
    MeI16 dims = (otherDim<<8)|segDim;

    result->touch = 1;

    MeVector3 globalNormal;

    if (tm)
    {
        MeMatrix4TMRotate(globalNormal, tm, normal);
    } else
    {
        MeVector3Copy(globalNormal, normal);
    }
    MeVector3Add(result->normal, result->normal, globalNormal);

    if (result->contactCount >= result->contactMaxCount)
    {
        MCD_CHECK_ASSERT((0), kMcdErrorNum_contactBufferOverrun, "", "AccumulateSphylContacts");
        return;
    }

    if (tm)
    {
        MeMatrix4TMTransform(result->contacts[result->contactCount].position, tm, cp);
    } else
    {
        MeVector3Copy(result->contacts[result->contactCount].position, cp);
    }
    MeVector3Copy(result->contacts[result->contactCount].normal,globalNormal);
    result->contacts[result->contactCount].separation = sep;
    result->contacts[result->contactCount].dims = dims;
    result->contactCount++;

    if (ds == 0)
    {
        return;
    }

    if (axis == NULL)
    {
        MCD_CHECK_ASSERT((0), kMcdErrorNum_GenericWarning,
            "Cannot generate second contact without axis", "AccumulateSphylContacts");
        return;
    }

    // Another contact
    if (result->contactCount >= result->contactMaxCount)
    {
        MCD_CHECK_ASSERT((0), kMcdErrorNum_GenericWarning,
            "Cannot generate second contact without axis", "AccumulateSphylContacts");
        return;
    }

    MeVector3ScaleAndAdd(result->contacts[result->contactCount].position,
                         result->contacts[result->contactCount-1].position, ds, axis);
    MeVector3Copy(result->contacts[result->contactCount].normal,
                  result->contacts[result->contactCount-1].normal);
    result->contacts[result->contactCount].separation = sep;
    result->contacts[result->contactCount].dims = dims;
    result->contactCount++;
}

static void
GenerateTriangleContact(McdUserTriangle *tri, MeVector3 center, MeVector3 axis,
                        MeReal hh, MeReal R, MeMatrix4 tm, MeReal eps, McdIntersectResult *result)
{
    // Calculate the edges, the face normal, and the edge normals.
    // These vectors will not be normalized.
    MeVector3 edge[3];
    MeVector3Subtract(edge[0], *tri->vertices[1], *tri->vertices[0]);
    MeVector3Subtract(edge[1], *tri->vertices[2], *tri->vertices[1]);
    MeVector3Subtract(edge[2], *tri->vertices[0], *tri->vertices[2]);
    MeVector3 faceN;
    MeVector3Cross(faceN, edge[0], edge[1]);
    MeReal faceN2 = MeVector3Dot(faceN, faceN);
    MeVector3 eN[3];
    MeVector3Cross(eN[0], edge[0], faceN);
    MeVector3Cross(eN[1], edge[1], faceN);
    MeVector3Cross(eN[2], edge[2], faceN);
    MeReal eN2[3] =
    {
        MeVector3Dot(eN[0], eN[0]),
        MeVector3Dot(eN[1], eN[1]),
        MeVector3Dot(eN[2], eN[2])
    };

    // Find distance-to-plane equation, as a function of line parameter s, for each plane.
    MeReal b[3] =
    {
        MeVector3Dot(center, eN[0]) - MeVector3Dot(*tri->vertices[0], eN[0]),
        MeVector3Dot(center, eN[1]) - MeVector3Dot(*tri->vertices[1], eN[1]),
        MeVector3Dot(center, eN[2]) - MeVector3Dot(*tri->vertices[2], eN[2])
    };
    MeReal bFace = MeVector3Dot(center, faceN) - MeVector3Dot(*tri->vertices[0], faceN);
    MeReal m[3] = 
    {
        MeVector3Dot(axis, eN[0]),
        MeVector3Dot(axis, eN[1]),
        MeVector3Dot(axis, eN[2])
    };
    MeReal mFace = MeVector3Dot(axis, faceN);

    MeBool centerUnderFace = !(tri->flags & kMcdTriangleTwoSided) && bFace < 0;

    // This will be the contact normal
    MeVector3 n = {0, 0, 0};

    // This will be the contact position
    MeVector3 p;

    // This will be the line parameter of the first (or only) contact.
    MeReal s0 = 0;

    // This will be an offset to the 2nd contact, if any.
    MeReal ds0 = 0;

    // Find the distances between the sphyl segment and each edge
    MeReal r2min;

    // This will be the relevant edge (if any)
    MeI32 e = -1;

    // The dimensionality of the triangle contact
    MeI16 triDim = 2;

    // See if either endpoint projects onto the triangle face.
    // Try to calculate these conditions with no conditionals:
    const MeReal hhm[3] = {hh*m[0], hh*m[1], hh*m[2]};
    const MeI32 minInside =
        ((MeI32)(b[0]<=hhm[0]))&((MeI32)(b[1]<=hhm[1]))&((MeI32)(b[2]<=hhm[2]));
    const MeI32 maxInside =
        ((MeI32)(b[0]<=-hhm[0]))&((MeI32)(b[1]<=-hhm[1]))&((MeI32)(b[2]<=-hhm[2]));
    const MeI32 someInside = minInside|maxInside;
    const MeI32 bothInside = minInside&maxInside;

    // Seed distance calculation with endpoint distances
    if (someInside)
    {
        if (bothInside)
        {
            // Both endpoints project onto the triangle face
            const MeReal hhmFace = hh*mFace;
            if (mFace*mFace < PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS*faceN2)
            {
                // sphyl is parallel with triangle face
                if (bFace*bFace >= hhmFace*hhmFace)
                {
                    // sphyl segment does not penetrate face
                    s0 = -hh;
                    ds0 = 2*hh;
                }
            } else
            {
                const MeReal faceDMin = bFace-hhmFace;
                const MeReal faceDMax = bFace+hhmFace;
                s0 = (faceDMin*faceDMin < faceDMax*faceDMax) ? -hh : hh;
            }
        } else
        {
            s0 = minInside ? -hh : hh;
        }
        // Find contact point and normal
        MeVector3ScaleAndAdd(p, center, s0, axis);
        MeVector3Subtract(n, p, *tri->vertices[0]);
        const MeReal d = MeVector3Dot(n, faceN);
        const MeReal recipFaceN2 = MeRecip(faceN2);
        const MeReal d_N2 = d*recipFaceN2;
        MeVector3MultiplyScalar(n, faceN, d_N2);
        MeVector3Subtract(p, p, n);
        r2min = d*d_N2;
    } else
    {
        r2min = MEINFINITY;
    }

    // Determine if the sphyl axis is parallel with the triangle face
    const MeReal mFace2 = mFace*mFace;
    const MeBool parallel = mFace2 < PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS || bFace*bFace > hh*hh*mFace2;

    MeBool askew = false;

    // Now compare edge distances, if at least one endpoint did not project onto the face
    if (!bothInside)
    {
        for (MeI32 i = 0; i < 3; ++i)
        {
            MeReal s0i, s1i;
            MeReal ds0i;
            MeBool askewi = NSegmentSegment(center, axis, -hh, hh,
                                            *tri->vertices[i], edge[i], 0, 1,
                                            &s0i, &s1i, &ds0i);
            MeVector3 r, r1;
            MeVector3ScaleAndAdd(r, center, s0i, axis);
            MeVector3ScaleAndAdd(r1, *tri->vertices[i], s1i, edge[i]);
            MeVector3Subtract(r, r, r1);
            const MeReal r2 = MeVector3Dot(r, r);
            // See if ||r|-closestD| < eps*r2
            const MeReal sqDiff = r2-r2min;
            const MeBool withinTolerance = sqDiff*sqDiff < 4*PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS*r2*r2;
            if ((sqDiff < 0 && !withinTolerance) || (withinTolerance && ds0i != 0) || r2min == MEINFINITY)
            {
                // A new minimum distance.
                MeVector3Copy(p, r1);
                MeVector3Copy(n, r);
                r2min = r2;
                s0 = s0i;
                ds0 = ds0i;
                e = i;
                askew = askewi;
                triDim = (MeI16)(s1i*s1i != s1i || ds0 != 0);
            } else
            if (withinTolerance && ds0 == 0 && parallel)
            {
                // Within tolerance, and we only have one contact point.
                ds0 = s0i-s0;
                if (ds0*ds0 > PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
                {
                    // This contact point is well-separated and appropriate for a second contact.
                    triDim = 2;
                    e = -1;
                }
            }
        }
    }

    if (e >= 0 && ((tri->flags>>(2+e)) & 1) == 0)
    {
        // Contacting edge is turned off
        return;
    }

    // Normalize the normal
    MeReal segmentSep;
    const MeReal n2 = MeVector3Dot(n, n);
    if (n2 > PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
    {
        // We have a good normal
        const MeReal invN = MeRecipSqrt(n2);
        segmentSep = n2*invN;
        MeVector3Scale(n, invN);

        // If the sphyl's segment intersects the triangle, we must reverse the normal direction.
        if (!parallel)
        {
            const MeReal nsFace = bFace/mFace;
            const MeI32 segmentIntersect =
                ((MeI32)(b[0]<=nsFace*m[0]))&((MeI32)(b[1]<=nsFace*m[1]))&((MeI32)(b[2]<=nsFace*m[2]));
            if (segmentIntersect)
            {
                MeVector3Scale(n, -1);
                segmentSep *= -1;
            }
        }

        if (centerUnderFace && e < 0)
        {
            // The triangle is one-sided, and we have picked the wrong face-normal
            if (segmentSep-R < eps)
            {
                // Only change the separation if we are in contact with the triangle.
                MeVector3Scale(n, -1);
                segmentSep = -segmentSep-2*hh*MeFabs(mFace);
            }
        }
    } else
    {
        segmentSep = 0;
        // Line segment glanced the triangle.  We don't have a good normal.
        if (e < 0)
        {
            // We have a face contact.
            MeVector3Copy(n, faceN);
            if (bFace < 0)
            {
                MeVector3Scale(n, -1);
            }
        } else
        {
            // We have an edge contact
            if (askew)
            {
                MeVector3Cross(n, edge[e], axis);
                if (mFace < 0)
                {
                    MeVector3Scale(n, -1);
                }
            } else
            {
                MeVector3Copy(n, eN[e]);
            }
        }
        MeVector3Normalize(n);
    }

    // Calculate separation
    const MeReal sep = segmentSep-R;

    // Generate the contact(s)
    if (sep < eps)
    {
        MeVector3 transformedAxis;
        MeMatrix4TMRotate(transformedAxis, tm, axis);
        const int count = result->contactCount;
        AccumulateSphylContacts(p, n, sep, 3, triDim, transformedAxis, ds0, tm, result);
        if (ds0 == 0 && bothInside)
        {
            // We only generated one contact.  See if we can improve the contact footprint a bit
#if 0 // old version
            MeVector3 b, para;
            MeVector3Cross(para, axis, n);
            MeVector3Cross(b, axis, para);
            MeReal b2 = MeVector3Dot(b, b);
            if (b2 > PARALLEL_THRESH_EPS)
            {
                MeVector3Scale(b, MeRecipSqrt(b2));
                MeVector3Scale(b, R);
                MeVector3Add(b, b, center);
                // Solve for (c+b+s*a).n = p.n
                // => s = (p.n-(c+b).n)/a.n
                MeReal s = (MeVector3Dot(*tri->vertices[0], n) - MeVector3Dot(b, n)) / MeVector3Dot(axis, n);
                if (s <= hh && s >= -hh)
                {
                    MeVector3 pos2;
                    MeVector3ScaleAndAdd(pos2, b, s, transformedAxis);
                    AccumulateSphylContacts(pos2, n, sep, 3, 2, NULL, 0, tm, result);
                }
            }
#else
            // Solve for (C-R*n+s*a-p).n = 0
            // => s = (R-(C-p).n)/a.n
            MeReal s;
            MeReal num = R-(MeVector3Dot(center, n) - MeVector3Dot(*tri->vertices[0], n));
            MeReal den = MeVector3Dot(axis, n);
            if (den > PARALLEL_THRESH_EPS)
            {
                if (num >= den*hh)
                { // s >= hh
                    s = hh; // clamp at hh
                } else
                if (num-den*s0 < hh*den*den)
                { // 2nd contact on same hemisphere as first
                    s = s0; // throw this one out
                } else
                {
                    s = num/den;
                }
            } else
            if (den < PARALLEL_THRESH_EPS)
            {
                if (num >= -den*hh)
                { // s <= hh
                    s = hh; // clamp at -hh
                } else
                if (num-den*s0 < hh*den*den)
                { // 2nd contact on same hemisphere as first
                    s = s0; // throw this one out
                } else
                {
                    s = num/den;
                }
            }
            if (s != s0)
            {
                MeVector3 pos2;
                MeVector3ScaleAndAdd(pos2, center, -R, n);
                MeVector3MultiplyAdd(pos2, s, transformedAxis);
                AccumulateSphylContacts(pos2, n, sep, 3, 2, NULL, 0, tm, result);
            }
#endif 
        }
        // Transfer triangle user data into contacts
        for (int i = count; i < result->contactCount; ++i)
        {
            result->contacts[i].element2.ptr = tri->triangleData.ptr;
        }
    }
}

/* ************************************************************************* */
/* ***** SPHYL - PLANE ***** */
int MEAPI McdSphylPlaneIntersect(McdModelPair* p, McdIntersectResult *result)
{   
    MeMatrix4Ptr tm1 = McdModelGetTransformPtr(p->model1);
    MeMatrix4Ptr tm2 = McdModelGetTransformPtr(p->model2);
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    const MeReal delta = (MeReal)0.001; // scale independent small number
    
    McdSphyl* sphyl = (McdSphyl*)McdModelGetGeometry( p->model1 );
    McdPlane* plane = (McdPlane*)McdModelGetGeometry( p->model2 );

    result->contactCount = 0;
    result->touch = 0;

    MeVector3 end, disp;

    // point-plane distance
    MeReal zz = MeVector3Dot(tm1[2], tm2[2]);
    MeReal ds = 0;
    if (zz < -delta)
    {
        MeVector3ScaleAndAdd(end, tm1[3], sphyl->mHalfHeight, tm1[2]);
    } else
    {
        MeVector3ScaleAndAdd(end, tm1[3], -sphyl->mHalfHeight, tm1[2]);
        if (zz < delta)
        {
            ds = 2*sphyl->mHalfHeight;
        }
    }

    MeVector3Subtract(disp, end, tm2[3]);
    MeReal sep = MeVector3Dot(disp, tm2[2]) - sphyl->mRadius; // contact normal is plane z axis (tm2[2])

    if (sep < eps) // generate contact
    {
        MeVector3 pos;
        MeVector3ScaleAndAdd(pos, end, -sphyl->mRadius-sep, tm2[2]);

        MeVector3Set(result->normal, 0, 0, 0);

        AccumulateSphylContacts(pos, tm2[2], sep, 3, 2, tm1[2], ds, NULL, result);

        if (ds == 0)
        {
#if 0 // old version
            // We only generated one contact.  See if we can improve the contact footprint a bit
            MeVector3 b, para;
            MeVector3Cross(para, tm1[2], tm2[2]);
            MeVector3Cross(b, tm1[2], para);
            MeReal b2 = MeVector3Dot(b, b);
            if (b2 > PARALLEL_THRESH_EPS)
            {
                MeVector3Scale(b, MeRecipSqrt(b2));
                MeVector3Scale(b, sphyl->mRadius);
                MeVector3Add(b, b, tm1[3]);
                // Solve for (c+b+s*a).n = p.n
                // => s = (p.n-(c+b).n)/a.n
                MeReal s = (MeVector3Dot(tm2[3], tm2[2]) - MeVector3Dot(b, tm2[2])) / zz;
                if (s <= sphyl->mHalfHeight && s >= -sphyl->mHalfHeight)
                {
                    MeVector3 pos2;
                    MeVector3ScaleAndAdd(pos2, b, s, tm1[2]);
                    MeVector3Set(result->normal, 0, 0, 0);
                    AccumulateSphylContacts(pos2, tm2[2], sep, 3, 2, NULL, 0, NULL, result);
                }
            }
#else
            // Solve for (C-R*n+s*a-p).n = 0
            // => s = (R-(C-p).n)/a.n
            MeReal s;
            MeReal num = sphyl->mRadius-(MeVector3Dot(tm1[3], tm2[2]) - MeVector3Dot(tm2[3], tm2[2]));
            MeReal s0 = zz > 0 ? -sphyl->mHalfHeight : sphyl->mHalfHeight;
            if (zz > PARALLEL_THRESH_EPS)
            {
                if (num >= zz*sphyl->mHalfHeight)
                { // s >= sphyl->mHalfHeight
                    s = sphyl->mHalfHeight; // clamp at sphyl->mHalfHeight
                } else
                if (num-s0*zz < sphyl->mRadius*zz*zz)
                { // 2nd contact on same hemisphere as first
                    s = s0; // throw this one out
                } else
                {
                    s = num/zz;
                }
            } else
            if (zz < PARALLEL_THRESH_EPS)
            {
                if (num >= -zz*sphyl->mHalfHeight)
                { // s <= sphyl->mHalfHeight
                    s = sphyl->mHalfHeight; // clamp at -sphyl->mHalfHeight
                } else
                if (num-s0*zz < sphyl->mRadius*zz*zz)
                { // 2nd contact on same hemisphere as first
                    s = s0; // throw this one out
                } else
                {
                    s = num/zz;
                }
            }
            if (s != s0)
            {
                MeVector3 pos2;
                MeVector3ScaleAndAdd(pos2, tm1[3], -sphyl->mRadius, tm2[2]);
                MeVector3MultiplyAdd(pos2, s, tm1[2]);
                MeVector3Set(result->normal, 0, 0, 0);
                AccumulateSphylContacts(pos2, tm2[2], sep, 3, 2, NULL, 0, NULL, result);
            }
#endif 
        }
    }

    return result->touch;
}

MCD_IMPLEMENT_INTERSECT_REGISTRATION(Sphyl, Plane, 1)



/* ************************************************************************* */
/* ***** SPHYL - SPHERE ***** */
int MEAPI McdSphylSphereIntersect(McdModelPair* p, McdIntersectResult *result)
{   
    MeMatrix4Ptr tm1 = McdModelGetTransformPtr(p->model1);
    MeMatrix4Ptr tm2 = McdModelGetTransformPtr(p->model2);
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    McdSphyl* sphyl = (McdSphyl*)McdModelGetGeometry( p->model1 );
    McdSphere* sphere = (McdSphere*)McdModelGetGeometry( p->model2 );

    McdFramework *fwk = p->model1->frame;

    result->contactCount = 0;
    result->touch = 0;

    // First, calculate closest point on sphyl axis (local z-axis).
    MeVector3 cp;
    PointSegmentClosestPoint(tm1[3], tm1[2], sphyl->mHalfHeight, tm2[3], cp);

    MeVector3 n;

    MeVector3Subtract(n, cp, tm2[3]);

    MeReal n2 = MeVector3Dot(n, n);
    MeReal sep;
    if (n2 > PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS * fwk->mScale * fwk->mScale)
    {
        MeReal invN = MeRecipSqrt(n2);
        sep = invN*n2;
        MeVector3Scale(n, invN);
    } else
    {
        MeVector3 aa = {tm1[2][0]*tm1[2][0], tm1[2][1]*tm1[2][1], tm1[2][2]*tm1[2][2]};
        MeI32 i = MinIndex(aa);
        aa[0] = 0;
        aa[1] = 0;
        aa[2] = 0;
        aa[i] = 1;
        MeVector3Cross(n, tm1[2], aa);
        MeVector3Normalize(n);
        sep = 0;
    }

    sep -= sphyl->mRadius + sphere->mRadius;

    if (sep < eps)
    {
        MeVector3 pos;
        MeVector3ScaleAndAdd(pos, cp, -sphyl->mRadius-sep, n);

        MeVector3Set(result->normal, 0, 0, 0);

        AccumulateSphylContacts(pos, n, sep, 3, 3, tm1[2], 0, NULL, result);
    }

    return result->touch;
}

MCD_IMPLEMENT_INTERSECT_REGISTRATION(Sphyl, Sphere, 1)



/* ************************************************************************* */
/* ***** SPHYL - SPHYL ***** */
int MEAPI McdSphylSphylIntersect(McdModelPair* p, McdIntersectResult *result)
{   
    MeMatrix4Ptr tm1 = McdModelGetTransformPtr(p->model1);
    MeMatrix4Ptr tm2 = McdModelGetTransformPtr(p->model2);
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    McdSphyl* sphyl1 = (McdSphyl*)McdModelGetGeometry( p->model1 );
    McdSphyl* sphyl2 = (McdSphyl*)McdModelGetGeometry( p->model2 );

    result->contactCount = 0;
    result->touch = 0;

    MeReal s, t, ds;
    MeVector3 cp1, cp2;

    // Find closest point on each segment
    MeBool askew = NSegmentNSegment(tm1[3], tm1[2], -sphyl1->mHalfHeight, sphyl1->mHalfHeight,
                                    tm2[3], tm2[2], -sphyl2->mHalfHeight, sphyl2->mHalfHeight,
                                    &s, &t, &ds);

    MeVector3ScaleAndAdd(cp1, tm1[3], s, tm1[2]);
    MeVector3ScaleAndAdd(cp2, tm2[3], t, tm2[2]);

    MeVector3 n;
    MeVector3Subtract(n, cp1, cp2);
    MeReal n2 = MeVector3Dot(n, n);
    MeReal sep;
    if (n2 > PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
    {
        // The line segments don't touch, so the normal comes from the difference
        // between the closest points
        MeReal nInv = MeRecipSqrt(n2);
        sep = n2*nInv;
        MeVector3Scale(n, nInv);
    } else
    {
        // The line segments do touch.
        sep = 0;
        if (askew)
        {
            // Luckily, the lines are askew, so the normal can be obtained from the
            // cross product of the two direction vectors.
            MeVector3Cross(n, tm1[2], tm2[2]);
        } else
        {
            // The worst case.  The segments are parallel and overlap.  We just have
            // to pick a perpendicular and call that the normal.
            MeVector3 aa = {tm1[2][0]*tm1[2][0], tm1[2][1]*tm1[2][1], tm1[2][2]*tm1[2][2]};
            MeI32 i = MinIndex(aa);
            aa[0] = 0;
            aa[1] = 0;
            aa[2] = 0;
            aa[i] = 1;
            MeVector3Cross(n, tm1[2], aa);
        }
        MeVector3Normalize(n);
    }

    sep -= sphyl1->mRadius+sphyl2->mRadius;

    if (sep < eps)
    {
        MeVector3 p;
        MeVector3ScaleAndAdd(p, cp2, sphyl2->mRadius, n);

        MeVector3Set(result->normal, 0, 0, 0);

        AccumulateSphylContacts(p, n, sep, 3, 3, tm1[2], ds, NULL, result);
    }

    return result->touch;
}

MCD_IMPLEMENT_INTERSECT_REGISTRATION(Sphyl, Sphyl, 1)



/* ************************************************************************* */
/* ***** SPHYL - BOX ***** */
int MEAPI McdSphylBoxIntersect(McdModelPair* p, McdIntersectResult *result)
{
    int i, j; // iteration variables

    MeMatrix4Ptr tm1 = McdModelGetTransformPtr(p->model1);
    MeMatrix4Ptr tm2 = McdModelGetTransformPtr(p->model2);
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );

    McdSphyl* sphyl = (McdSphyl*)McdModelGetGeometry( p->model1 );
    McdBox* box = (McdBox*)McdModelGetGeometry( p->model2 );

    McdFramework *fwk = p->model1->frame;

    result->contactCount = 0;
    result->touch = 0;

    // Work in the box's reference frame.
    MeVector3 relAxis;
    MeVector3 relPos;
    MeMatrix4TMInverseRotate(relAxis, tm2, tm1[2]);
    MeMatrix4TMInverseTransform(relPos, tm2, tm1[3]);

    // We will traverse the box's Voronoi regions.  There are four region types:
    // (0) "corner" regions, where the closest point is a corner
    // (1) "wing" regions, where the closest point lies on an edge
    // (2) "side" regions, where the closest point lies on a face
    // (3) inside the box
    MeI32 regionType = 0;

    // Distinguishing axis, if any
    MeI32 axis = 0;

    // Examine the boundary-crossing history of the line.  There will be 6 crossings, in general.
    // We will label the crossings 0-5, given by 2*a+b, where a = the axis #, and b = 0 or 1,
    // corresponding to min and max along the axis.
    MeReal si[7]; // the line parameter at each plane crossing

    // First, determine the starting region of the line.
    MeI32 signN = 0;
    MeVector3 boxP; // position signs (initialized in following loop)
    MeVector3 pos;
    MeVector3ScaleAndAdd(pos, relPos, -sphyl->mHalfHeight, relAxis);
    for (i = 0; i < 3; ++i)
    {
        // Find plane intersect time
        if (MeFabs(relAxis[i]) > PARALLEL_THRESH_EPS)
        {
            MeReal recipN = MeRecip(relAxis[i]);
            si[i<<1] = (-box->mR[i]-relPos[i])*recipN;
            si[(i<<1)+1] = (box->mR[i]-relPos[i])*recipN;
            if (relAxis[i] < 0)
            {
                signN |= 1<<i;
            }
        } else
        {
            si[i<<1] = -MEINFINITY;
            si[(i<<1)+1] = MEINFINITY;
        }

        // Update starting region
        if (pos[i] < -box->mR[i])
        {
            boxP[i] = -box->mR[i];
        } else
        if (pos[i] > box->mR[i])
        {
            boxP[i] = box->mR[i];
        } else
        {
            ++regionType;
            if (regionType == 1)
            {
                axis = i;
            } else
            if (regionType == 2)
            {
                axis = 3-(i+axis); // the other axis, besides i and the present axis
            }
            boxP[i] = 0;
        }
    }

    MeI32 sort[7]; // we will sort the plane crossings in order of line parameter
    MeI32 aSigns = 0; // keep track of the axes for which we have already recorded a crossing
    MeReal s = -MEINFINITY;

    MeI32 jMin = 0;
    MeI32 jStop;

    for (j = 0; j < 6; ++j)
    {
        MeReal minS = MEINFINITY; // smallest line parameter
        MeI32 nextIndex = 0;
        for (i = 3; i--;)
        {
            // if we are pointing in the + direction, check min first
            MeI32 b = ((signN^aSigns)>>i)&1;
            MeI32 index = (i<<1)+b;
            if (si[index] <= minS && si[index] >= s)
            {
                nextIndex = index;
                minS = si[index];
            }
        }
        s = minS;
        sort[j] = nextIndex;
        aSigns ^= 1<<(nextIndex>>1);
        if (s <= -sphyl->mHalfHeight)
        {
            ++jMin;
        }
    }
    jStop = j+1;

    // Padding
    sort[6] = 6;
    si[6] = MEINFINITY;

    // Now traverse line, and find closest point(s) of approach
    MeVector3 n = {0,0,0}; // collision normal will come out of this loop
    MeI16 boxDim = 0;
    MeReal ds1 = 0; // if this is not zero, it gives the offset to another intersect point
    s = -sphyl->mHalfHeight;
    MeReal s0 = s;
    for (j = jMin; j < jStop; ++j)
    {
        MeI32 index = sort[j];
        MeVector3 d;
        MeVector3Subtract(d, relPos, boxP);

        // Find closest point in this region
        if (!(regionType&2))
        {
            MeReal dl = MeVector3Dot(d, relAxis);
            if (!regionType)
            {
                // regionType == 0, corner
                s0 = MeCLAMP(-dl, -sphyl->mHalfHeight, sphyl->mHalfHeight);
                if (s0 < si[index]-PARALLEL_THRESH_EPS)
                {
                    MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                    MeVector3Subtract(n, n, boxP);
                    boxDim = 0;
                    break;
                }
            } else
            {
                // regionType == 1, wing
                MeReal det = 1-relAxis[axis]*relAxis[axis];
                if (det > PARALLEL_THRESH_EPS)
                {
                    MeReal invDet = MeRecip(det);
                    s0 = (relAxis[axis]*d[axis]-dl)*invDet;
                    s0 = MeCLAMP(s0, -sphyl->mHalfHeight, sphyl->mHalfHeight);
                    if (s0 < si[index]-PARALLEL_THRESH_EPS)
                    {
                        MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                        boxP[axis] = n[axis];
                        MeVector3Subtract(n, n, boxP);
                        n[axis] = 0;
                        boxDim = 1;
                        break;
                    }
                } else
                {
                    s0 = s;
                    MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                    boxP[axis] = n[axis];
                    MeVector3Subtract(n, n, boxP);
                    n[axis] = 0;
                    ds1 = MeMIN(si[index], sphyl->mHalfHeight)-s0;
                    boxDim = 1;
                    break;
                }
            }
        } else
        {
            if (!(regionType&1))
            {
                // regionType == 2. face
                if (MeFabs(relAxis[axis]*boxP[axis]) < PARALLEL_THRESH_EPS)
                {
                    // sphyl is parallel with the face
                    s0 = s;
                    MeVector3Set(n, 0, 0, 0);
                    n[axis] = relPos[axis]-boxP[axis];
                    MeVector3ScaleAndAdd(boxP, relPos, s0, relAxis);
                    MeVector3Subtract(boxP, boxP, n);
                    ds1 = MeMIN(si[index], sphyl->mHalfHeight)-s0;
                    boxDim = 2;
                    break;
                } else
                {
                    if (relAxis[axis]*boxP[axis] > 0)
                    {
                        s0 = s;
                        MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                        MeReal ba = boxP[axis];
                        MeVector3Copy(boxP, n);
                        boxP[axis] = ba;
                        MeVector3Subtract(n, n, boxP);
                        boxDim = 2;
                        break;
                    } else
                    {
                        if (sphyl->mHalfHeight < si[index])
                        {
                            s0 = sphyl->mHalfHeight;
                            MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                            MeReal ba = boxP[axis];
                            MeVector3Copy(boxP, n);
                            boxP[axis] = ba;
                            MeVector3Subtract(n, n, boxP);
                            boxDim = 2;
                            break;
                        }
                    }
                }
            } else
            {
                // regionType == 3, inside
                s0 = s;
                MeVector3ScaleAndAdd(boxP, relPos, s0, relAxis);
                ds1 = MeMIN(si[index], sphyl->mHalfHeight)-s0;
                // Normal will be (0,0,0), triggering special-case code
                break;
            }
        }

        s = si[index];

        // Find next region
        MeI32 plane = index>>1;
        MeI32 side = index&1;
        MeBool entrance = (side == ((signN>>plane)&1));
        if (entrance)
        {
            ++regionType;
            boxP[plane] = 0;
            if (regionType == 1)
            {
                axis = plane;
            } else
            if (regionType == 2)
            {
                axis = 3-(plane+axis); // the other axis, besides i and the present axis
            }
        } else
        {
            --regionType;
            boxP[plane] = box->mR[plane]*(MeReal)(2*side-1);
            if (regionType == 1)
            {
                axis = 3-(plane+axis); // the other axis, besides i and the present axis
            } else
            if (regionType == 2)
            {
                axis = plane;
            }
        }
    }

    MCD_CHECK_ASSERT((j < jStop), 
        kMcdErrorNum_GenericWarning, "Unhandled configuration", "McdSphylBoxIntersect");
    if (j >= jStop)
    {
        // This should not be hit
        return result->touch;
    }

    MeReal n2 = MeVector3Dot(n, n);
    MeReal sep;
    if (n2 > PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
    {
        // We have a valid normal
        MeReal invN = MeRecipSqrt(n2);
        MeVector3Scale(n, invN);
        sep = invN*n2 - sphyl->mRadius;
    } else
    {
        // Normal is not valid, so use separating plane to find normal
        MeI32 sepIndex = 0;
        MeReal sepSign = 1;
        MeReal maxSep = -MEINFINITY;
        // Box faces
        for (i = 0; i < 3; ++i)
        {
            MeReal sumR = MeFabs(relAxis[i])*sphyl->mHalfHeight + box->mR[i];
            MeReal sepI = MeFabs(relPos[i])-sumR;
            if (sepI > maxSep)
            {
                maxSep = sepI;
                sepIndex = i;
                sepSign = relPos[i] > 0 ? (MeReal)1 : (MeReal)-1;
            }
        }
        // Sphyl axis crossed with box axes
        for (i = 0; i < 3; ++i)
        {
            MeVector3 a;
            MeVector3CrossAxis(a, relAxis, i);
            MeReal a2 = MeVector3Dot(a, a);
            if (a2 > PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
            {
                MeReal invA = MeRecipSqrt(a2);
                MeVector3Scale(a, invA);
                MeReal sumR = MeFabs(a[0])*box->mR[0]+MeFabs(a[1])*box->mR[1]+MeFabs(a[2])*box->mR[2];
                MeReal disp = MeVector3Dot(relPos, a);
                MeReal sepI = MeFabs(disp) - sumR;
                if (sepI > maxSep)
                {
                    maxSep = sepI;
                    sepIndex = i+3; // denotes we are writing the normal now
                    MeVector3Copy(n, a);
                    if (disp < 0)
                    {
                        MeVector3Scale(n, -1);
                    }
                }
            }
        }
        if (sepIndex < 3)
        {
            n[sepIndex] = sepSign;
        }
        // Calculate proper separation
        sep = maxSep-sphyl->mRadius;
    }
    
    if (sep < eps)
    {
        MeVector3Set(result->normal, 0, 0, 0);

        AccumulateSphylContacts(boxP, n, sep, 3, boxDim, tm1[2], ds1, tm2, result);
    }

    return result->touch;
}

MCD_IMPLEMENT_INTERSECT_REGISTRATION(Sphyl, Box, 1)



/* ************************************************************************* */
/* ***** SPHYL - CYLINDER ***** */
int MEAPI McdSphylCylinderIntersect(McdModelPair* p, McdIntersectResult *result)
{   
    int i, j; // iteration variables

    MeMatrix4Ptr tm1 = McdModelGetTransformPtr(p->model1);
    MeMatrix4Ptr tm2 = McdModelGetTransformPtr(p->model2);
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );

    McdSphyl* sphyl = (McdSphyl*)McdModelGetGeometry( p->model1 );
    McdCylinder* cyl = (McdCylinder*)McdModelGetGeometry( p->model2 );

    result->contactCount = 0;
    result->touch = 0;

    // Work in the cylinder's reference frame.
    MeVector3 relAxis;
    MeVector3 relPos;
    MeMatrix4TMInverseRotate(relAxis, tm2, tm1[2]);
    MeMatrix4TMInverseTransform(relPos, tm2, tm1[3]);

    // We will traverse the cylinders's Voronoi regions.  There are four region types:
    // (0) "rim" regions, where the closest point is on one of the cylinder's rims
    // (1) "wall" regions, where the closest point lies on the cylinder's wall
    // (2) "end" regions, where the closest point lies on one of the cylinder's end-caps
    // (3) inside the cylinder
    MeI32 regionType = 0;

    // Examine the boundary-crossing history of the line.  There will be 4 crossings, max.
    // We will label the crossings 0-3:
    // 0 = min-Z crossing
    // 1 = max-Z crossing
    // 2 = min-R crossing
    // 3 = max-R crossing
    MeReal si[5]; // the line parameter at each crossing

    // First, determine the intersect times and the starting region of the line.
    MeI32 signZ = relAxis[2] >= 0 ? 1 : -1;
    MeVector3 pos;
    MeVector3ScaleAndAdd(pos, relPos, -sphyl->mHalfHeight, relAxis);

    // Find plane intersect time
    if (MeFabs(relAxis[2]) > PARALLEL_THRESH_EPS)
    {
        MeReal recipN = MeRecip(relAxis[2]);
        MeReal szrz = signZ*cyl->mRz;
        si[0] = (-szrz-relPos[2])*recipN;
        si[1] = (szrz-relPos[2])*recipN;
    } else
    {
        si[0] = -MEINFINITY;
        si[1] = MEINFINITY;
    }

    // Update starting region
    MeReal cylZ;
    if (pos[2] < -cyl->mRz)
    {
        cylZ = -cyl->mRz;
    } else
    if (pos[2] > cyl->mRz)
    {
        cylZ = cyl->mRz;
    } else
    {
        cylZ = 0;
        regionType |= 1;
    }

    // Find the circle intersect time
    MeReal aXY2 = relAxis[0]*relAxis[0] + relAxis[1]*relAxis[1];
    if (aXY2 > PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
    {
        MeReal cXY2 = relPos[0]*relPos[0] + relPos[1]*relPos[1];
        MeReal acXY = relAxis[0]*relPos[0] + relAxis[1]*relPos[1];
        MeReal arg = acXY*acXY - aXY2*(cXY2 - cyl->mR*cyl->mR);
        if (arg >= 0)
        {
            MeReal rad = MeSqrt(arg);
            MeReal den = MeRecip(aXY2);
            si[2] = -(acXY+rad)*den;
            si[3] = -(acXY-rad)*den;
        } else
        {
            si[2] = MEINFINITY;
            si[3] = MEINFINITY;
        }
    } else
    {
        si[2] = MEINFINITY;
        si[3] = MEINFINITY;
    }

    // Update starting region
    if (pos[0]*pos[0]+pos[1]*pos[1] <= cyl->mR*cyl->mR)
    {
        regionType |= 2;
    }

    // Now sort the crossings
    MeI32 sort[5]; // we will sort the  crossings in order of line parameter
    sort[0] = 0;
    sort[2] = 2;
    OrderIndices(sort, sort+2, si);
    sort[1] = sort[0]+1;
    sort[3] = sort[2]+1;
    OrderIndices(sort+1, sort+2, si);
    OrderIndices(sort+2, sort+3, si);

    // Padding
    sort[4] = 4;
    si[4] = MEINFINITY;

    // Now traverse line, and find closest point(s) of approach
    MeVector3 n = {0,0,0}; // collision normal will come out of this loop
    MeVector3 cylP; // collision point will come out of this loop
    MeI16 cylDim = 0;
    MeReal ds1 = 0; // if this is not zero, it gives the offset to another intersect point
    MeReal s = -sphyl->mHalfHeight;
    MeReal s0 = s;

    MeI32 jMin = 0;
    MeI32 jMax = 5;
    for (j = 0; j < 4; ++j)
    {
        MeI32 index = sort[j];
        if (si[index] < -sphyl->mHalfHeight-PARALLEL_THRESH_EPS)
        {
            continue;
        }
        MeI32 crossingMask = (index&2)?2:1;
        MeBool inside = (regionType&crossingMask) != 0;
        MeBool exiting = (index&1) != 0;
        if (inside == exiting)
        {
            break;
        }
    }
    jMin = j;

    for (j = jMin; j < jMax; ++j)
    {
        MeI32 index = sort[j];

        // Find closest point in this region
        if (!(regionType&2))
        {
            if (!regionType)
            {
                // regionType == 0, rim
                if (MeFabs(relAxis[2]) > PARALLEL_THRESH_EPS)
                {
                    MeReal c = relAxis[0]*relAxis[0]+relAxis[1]*relAxis[1];
                    if (c > PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
                    {
                        // need to solve a quartic, in order to find the closest point
                        MeReal a = cyl->mR*cyl->mR;
                        MeReal b = relPos[0]*relAxis[0]+relPos[1]*relAxis[1];
                        MeReal d = b + (relPos[2]-cylZ)*relAxis[2];
                        MeReal e = relPos[0]*relPos[0]+relPos[1]*relPos[1];
                        MeReal coef[20]; // 20 = (4+1)*4, as needed for temporary storage by root finder
                        coef[0] = e*d*d-a*b*b;
                        coef[1] = 2*(d*e+d*d*b-a*b*c);
                        coef[2] = e+4*d*b+d*d*c-a*c*c;
                        coef[3] = 2*(b+c*d);
                        coef[4] = c;
                        MeReal roots[6]; // save room for an extra "roots"
                        MeReal maxS = MeMIN(si[index], sphyl->mHalfHeight);
                        MeI32 rootCount = PolynomialRoots(roots, s-PARALLEL_THRESH_EPS, maxS+PARALLEL_THRESH_EPS, 4, coef);
                        roots[rootCount] = maxS;
                        MeI32 rootN = rootCount++;
                        roots[rootCount++] = s;
                        // Pick root which gives closest point.
                        MeReal rootDist2 = MEINFINITY;
                        for (i = 0; i < rootCount; ++i)
                        {
                            MeVector3 p;
                            MeVector3ScaleAndAdd(p, relPos, roots[i], relAxis);
                            MeReal dR = MeSqrt(p[0]*p[0]+p[1]*p[1])-cyl->mR;
                            MeReal dZ = p[2]-cylZ;
                            MeReal d2 = dR*dR+dZ*dZ;
                            if (d2 < rootDist2)
                            {
                                rootDist2 = d2;
                                rootN = i;
                            }
                        }
                        s0 = MeCLAMP(roots[rootN], -sphyl->mHalfHeight, sphyl->mHalfHeight);
                        if (s0 < si[index]-PARALLEL_THRESH_EPS)
                        {
                            MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                            cylP[0] = n[0];
                            cylP[1] = n[1];
                            cylP[2] = 0;
                            MeVector3Normalize(cylP);
                            MeVector3Scale(cylP, cyl->mR);
                            cylP[2] = cylZ;
                            MeVector3Subtract(n, n, cylP);
                            cylDim = 1;
                            break;
                        }
                    } else
                    {
                        // Axes are parallel - only need to solve a linear equation
                        s0 = (cyl->mRz-relPos[2])*relAxis[2];
                        s0 = MeCLAMP(s0, -sphyl->mHalfHeight, sphyl->mHalfHeight);
                        if (s0 < si[index]-PARALLEL_THRESH_EPS)
                        {
                            MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                            cylP[0] = n[0];
                            cylP[1] = n[1];
                            cylP[2] = 0;
                            MeVector3Normalize(cylP);
                            MeVector3Scale(cylP, cyl->mR);
                            cylP[2] = cylZ;
                            MeVector3Subtract(n, n, cylP);
                            cylDim = 1;
                            break;
                        }
                    }
                } else
                {
                    // Axes are perpendicular.
                    MeReal dl = MeVector3Dot(relPos, relAxis);
                    s0 = MeMIN(-dl, si[index]);
                    s0 = MeCLAMP(s0, -sphyl->mHalfHeight, sphyl->mHalfHeight);
                    if (s0 < si[index]-PARALLEL_THRESH_EPS)
                    {
                        MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                        cylP[0] = n[0];
                        cylP[1] = n[1];
                        cylP[2] = 0;
                        MeVector3Normalize(cylP);
                        MeVector3Scale(cylP, cyl->mR);
                        cylP[2] = cylZ;
                        MeVector3Subtract(n, n, cylP);
                        cylDim = 1;
                        break;
                    }
                }
            } else
            {
                // regionType == 1, wall
                // Find the closest point on the sphyl axis to the cyl axis
                MeReal det = 1-relAxis[2]*relAxis[2];
                MeReal dl = MeVector3Dot(relPos, relAxis);
                if (det > PARALLEL_THRESH_EPS)
                {
                    MeReal invDet = MeRecip(det);
                    s0 = (relAxis[2]*relPos[2]-dl)*invDet;
                    s0 = MeCLAMP(s0, -sphyl->mHalfHeight, sphyl->mHalfHeight);
                    if (s0 < si[index]-PARALLEL_THRESH_EPS)
                    {
                        // Project contact point onto infinite cylinder wall
                        MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                        cylP[0] = n[0];
                        cylP[1] = n[1];
                        cylP[2] = 0;
                        MeVector3Normalize(cylP);
                        MeVector3Scale(cylP, cyl->mR);
                        cylP[2] = n[2];
                        n[0] -= cylP[0];
                        n[1] -= cylP[1];
                        n[2] = 0;
                        cylDim = 3;
                        break;
                    }
                } else
                {
                    s0 = s;
                    MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                    cylP[0] = n[0];
                    cylP[1] = n[1];
                    cylP[2] = 0;
                    MeVector3Normalize(cylP);
                    MeVector3Scale(cylP, cyl->mR);
                    cylP[2] = n[2];
                    n[0] -= cylP[0];
                    n[1] -= cylP[1];
                    n[2] = 0;
                    ds1 = MeMIN(si[index], sphyl->mHalfHeight)-s0;
                    cylDim = 3;
                    break;
                }
            }
        } else
        {
            if (!(regionType&1))
            {
                // regionType == 2. end
                if (MeFabs(relAxis[2]) < PARALLEL_THRESH_EPS)
                {
                    // sphyl is parallel with the face
                    s0 = s;
                    MeVector3Set(n, 0, 0, relPos[2]-cylZ);
                    MeVector3ScaleAndAdd(cylP, relPos, s0, relAxis);
                    MeVector3Subtract(cylP, cylP, n);
                    ds1 = MeMIN(si[index], sphyl->mHalfHeight)-s0;
                    cylDim = 2;
                    break;
                } else
                {
                    if (relAxis[2]*cylZ > 0)
                    {
                        s0 = s;
                        MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                        MeVector3Copy(cylP, n);
                        cylP[2] = cylZ;
                        MeVector3Subtract(n, n, cylP);
                        cylDim = 2;
                        break;
                    } else
                    {
                        if (sphyl->mHalfHeight < si[index])
                        {
                            s0 = sphyl->mHalfHeight;
                            MeVector3ScaleAndAdd(n, relPos, s0, relAxis);
                            MeVector3Copy(cylP, n);
                            cylP[2] = cylZ;
                            MeVector3Subtract(n, n, cylP);
                            cylDim = 2;
                            break;
                        }
                    }
                }
            } else
            {
                // regionType == 3, inside
                s0 = s;
                MeVector3ScaleAndAdd(cylP, relPos, s0, relAxis);
                ds1 = MeMIN(si[index], sphyl->mHalfHeight)-s0;
                // Normal will be (0,0,0), triggering special-case code
                break;
            }
        }

        s = si[index];

        // Find next region
        MeI32 boundaryType = index>>1; // 0 = end, 1 = wall
        MeI32 side = index&1;
        MeBool entrance = !side;
        MeI32 boundaryBit = 1<<boundaryType;
        if (entrance)
        {
            regionType |= boundaryBit;
            if (!boundaryType)
            {
                cylZ = 0;
            }
        } else
        {
            regionType &= ~boundaryBit;
            if (!boundaryType)
            {
                cylZ = signZ*cyl->mRz;
            }
        }
    }

    MCD_CHECK_ASSERT((j < jMax), 
        kMcdErrorNum_GenericWarning, "Unhandled configuration", "McdSphylCylinderIntersect");
    if (j >= jMax)
    {
        // This should not be hit
        return result->touch;
    }

    MeReal n2 = MeVector3Dot(n, n);
    MeReal sep;
    if (n2 > PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
    {
        // We have a valid normal
        MeReal invN = MeRecipSqrt(n2);
        MeVector3Scale(n, invN);
        sep = invN*n2 - sphyl->mRadius;
    } else
    {
        // Normal is not valid, so use a separating plane to find normal
        // Cylinder ends
        MeReal maxSep = MeFabs(relPos[2])-(MeFabs(relAxis[2])*sphyl->mHalfHeight + cyl->mRz);
        n[2] = relPos[2] > 0 ? (MeReal)1 : (MeReal)-1;
        // Sphyl axis crossed with cylinder axis
        MeVector3 a = {relAxis[1], -relAxis[0], 0};
        MeReal a2 = MeVector3Dot(a, a);
        if (a2 < PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
        {
            MeVector3Copy(a, relPos);
            a[2] = 0;
            a2 = MeVector3Dot(a, a);
            if (a2 < PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
            {
                MeVector3Set(a, 1, 0, 0);
                a2 = 1;
            }
        }
        MeReal invA = MeRecipSqrt(a2);
        MeVector3Scale(a, invA);
        MeReal disp = MeVector3Dot(relPos, a);
        MeReal sepI = MeFabs(disp) - cyl->mR;
        if (sepI > maxSep)
        {
            maxSep = sepI;
            MeVector3Copy(n, a);
            if (disp < 0)
            {
                MeVector3Scale(n, -1);
            }
        }
        // Calculate proper separation
        sep = maxSep-sphyl->mRadius;
    }
    
    if (sep < eps)
    {
        MeVector3Set(result->normal, 0, 0, 0);

        AccumulateSphylContacts(cylP, n, sep, 3, cylDim, tm1[2], ds1, tm2, result);
    }

    return result->touch;
}

MCD_IMPLEMENT_INTERSECT_REGISTRATION(Sphyl, Cylinder, 1)

/* ************************************************************************* */
/* ***** SPHYL - TRIANGLELIST ***** */
int MEAPI McdSphylTriangleListIntersect(McdModelPair* p, McdIntersectResult *result)
{
    MeMatrix4Ptr tm1 = McdModelGetTransformPtr(p->model1);
    MeMatrix4Ptr tm2 = McdModelGetTransformPtr(p->model2);
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    McdSphylID sphylGeom = (McdSphylID)McdModelGetGeometry( p->model1 );
    McdTriangleListID trilistGeom = (McdTriangleListID)McdModelGetGeometry( p->model2 );

    result->contactCount = 0;
    result->touch = 0;

    McdSphyl *sphyl = (McdSphyl*)sphylGeom;
    McdTriangleList *triList = (McdTriangleList*)trilistGeom;

    // Convert sphyl to triangle list's frame
    MeVector3 relAxis;
    MeVector3 relPos;
    MeMatrix4TMInverseRotate(relAxis, tm2, tm1[2]);
    MeMatrix4TMInverseTransform(relPos, tm2, tm1[3]);

    triList->list =
        (McdUserTriangle*)MeMemoryALLOCA(triList->triangleMaxCount 
            * sizeof(McdUserTriangle));

    MeI32 count = (*triList->triangleListGenerator)(p, triList->list, relPos, 
        sphyl->mHalfHeight+sphyl->mRadius+eps, triList->triangleMaxCount);

    if (!count)
        return 0;

    MeVector3Set(result->normal, 0, 0, 0);

    for (MeI32 i = 0; i < count; ++i)
    {
        GenerateTriangleContact(triList->list+i, relPos, relAxis, 
            sphyl->mHalfHeight, sphyl->mRadius, tm2, eps, result);
    }

    const MeReal n2 = MeVector3Dot(result->normal, result->normal);

    if (n2 > ME_MIN_EPSILON*ME_MIN_EPSILON)
        MeVector3Scale(result->normal, MeRecipSqrt(n2));

    return result->touch;
}

MCD_IMPLEMENT_INTERSECT_REGISTRATION(Sphyl, TriangleList, 1)

/* ************************************************************************* */
/* ***** SPHYL - LINE SEGMENT ***** */

int LineSphere(MeVector3 sp, MeReal r, MeVector3 inOrig, MeVector3 dir, MeReal mag, McdLineSegIntersectResult * info)
{
    MeReal r2 = r*r;
    MeVector3 origToCentre;

    // Now its basically as with sphere.
    MeVector3Subtract(origToCentre, sp, inOrig); // vector from line start to sphere centre
    MeReal L2 = MeVector3Dot(origToCentre, origToCentre); // distance squared from orig to centre
    MeReal D = MeVector3Dot(origToCentre, dir); // distance to centre proj onto ray

    // if L2 < R2, we are inside sphere, so do as per manual.
    if(L2 < r2)
    {
        info->distance = 0;
        MeVector3Copy(info->position, inOrig);
        return 1;
    }

    // If D is negative, sphere is behind us. If L2 > R2 we are not inside it.
    if(D < 0 && L2 > r2)
        return 0;

    // Now use pythag to work out distance between centre and [orig + (D+dir)]
    MeReal D2 = D*D;
    MeReal M2 = L2 - D2;

    // if M2 is bigger than R2, we are outside the sphere so we miss.
    if(M2 > r2)
        return 0;

    MeReal DminusMag = D - mag;
    // we don't reach the sphere
    if(DminusMag > 0 && DminusMag*DminusMag > r2 - M2)
        return 0;

    // Use pythag again to calculate amount to subtract from D to reach intersect point.
    MeReal t = D - MeSqrt(r2 - M2);

    info->distance = t;
    MeVector3ScaleAndAdd(info->position, inOrig, t, dir);

    // Calculate normal based on vector between intersect point and centre.
    MeReal invR = MeRecip(r);
    info->normal[0] = invR * (info->position[0] - sp[0]);    
    info->normal[1] = invR * (info->position[1] - sp[1]);
    info->normal[2] = invR * (info->position[2] - sp[2]);

    return 1;
}

int MEAPI IxSphylLineSegment(const McdModelID model, 
                             MeReal* const inOrig, MeReal* const inDest, 
                             McdLineSegIntersectResult * info )
{
    McdSphyl* sphyl = (McdSphyl*)McdModelGetGeometry(model);
    MCD_CHECK_ASSERT_(inOrig != NULL && inDest != NULL && sphyl != NULL, "IxSphylLineSegment");
    MCD_CHECK_ASSERT_(McdGeometryGetType(&sphyl->m_g) == kMcdGeometryTypeSphyl, "IxSphylLineSegment");

    MeMatrix4Ptr tm = McdModelGetTransformPtr(model);

    // Transform start and end points into local ref frame.
    MeVector3 lOrig, lDest;
    MeMatrix4TMInverseTransform(lOrig, tm, inOrig);
    MeMatrix4TMInverseTransform(lDest, tm, inDest);

    // bools indicate if line enters different areas of sphyl
    MeBool doTest[3] = {0, 0, 0};
    if(lOrig[2] >= sphyl->mHalfHeight) // start in top
    {
        doTest[0] = 1;

        if(lDest[2] < sphyl->mHalfHeight)
        {
            doTest[1] = 1;

            if(lDest[2] < -sphyl->mHalfHeight)
                doTest[2] = 1;
        }
    }
    else if(lOrig[2] >= -sphyl->mHalfHeight) // start in middle
    {
        doTest[1] = 1;

        if(lDest[2] >= sphyl->mHalfHeight)
            doTest[0] = 1;

        if(lDest[2] < -sphyl->mHalfHeight)
            doTest[2] = 1;
    }
    else // start at bottom
    {
        doTest[2] = 1;

        if(lDest[2] >= -sphyl->mHalfHeight)
        {
            doTest[1] = 1;

            if(lDest[2] >= sphyl->mHalfHeight)
                doTest[0] = 1;
        }
    }


    // find line in terms of point and unit direction vector
    MeReal mag;
    MeVector3 sp = {0, 0, 0}, dir;
    MeVector3Subtract(dir, lDest, lOrig);
    mag = MeVector3Normalize(dir);

    // Check line against each sphere, then the cylinder Because shape
    // is convex, once we hit something, we dont have to check any more.
    MeBool hit = 0;
    McdLineSegIntersectResult tmpInfo;

    if(doTest[0])
    {
        sp[2] = sphyl->mHalfHeight;
        hit = LineSphere(sp, sphyl->mRadius, lOrig, dir, mag, &tmpInfo);

        // Discard hit if in cylindrical region.
        if(hit && tmpInfo.position[2] < sphyl->mHalfHeight)
            hit = 0;
    }

    if(doTest[2] && !hit)
    {
        sp[2] = -sphyl->mHalfHeight;
        hit = LineSphere(sp, sphyl->mRadius, lOrig, dir, mag, &tmpInfo);
        if(hit && tmpInfo.position[2] > -sphyl->mHalfHeight)
            hit = 0;
    }
    
    if(doTest[1] && !hit)
    {
        const MeReal dx = dir[0];
        const MeReal dy = dir[1];
        const MeReal dz = dir[2];
        const MeReal x0 = lOrig[0];
        const MeReal y0 = lOrig[1];
        const MeReal z0 = lOrig[2];
        
        // First, check if start is inside cylinder.
        if(x0*x0 + y0*y0 <= sphyl->mRadius * sphyl->mRadius && 
            z0 <= sphyl->mHalfHeight && z0 >= -sphyl->mHalfHeight)
        {
            MeVector3Copy(tmpInfo.position, lOrig);
            hit = 1;
        }
        else // if not.. solve quadratic
        {
            
            MeReal A = dx*dx + dy*dy;
            MeReal B = 2*(x0*dx + y0*dy);
            MeReal C = x0*x0 + y0*y0 - sphyl->mRadius*sphyl->mRadius;
            MeReal root = B*B - 4*A*C;
            
            if (root >= 0 && MeFabs(A) > ME_SMALL_EPSILON*ME_SMALL_EPSILON) 
            {
                root = MeSqrt(root);
                MeReal s = (-B-root)/(2*A);
                MeReal z1 = lOrig[2] + s * dir[2];
                
                // if its not before the start of the line, or past its end, 
                // or behyong the end of the cylinder, we have a hit!
                if(s > 0 && s < mag && z1 <= sphyl->mHalfHeight
                  && z1 >= -sphyl->mHalfHeight)
                {
                    tmpInfo.distance = s;
                    tmpInfo.position[0] = lOrig[0] + s * dir[0];
                    tmpInfo.position[1] = lOrig[1] + s * dir[1];
                    tmpInfo.position[2] = z1;

                    MeVector3ScaleAndAdd(tmpInfo.position,
                        lOrig, tmpInfo.distance, dir);

                    tmpInfo.normal[0] = tmpInfo.position[0];
                    tmpInfo.normal[1] = tmpInfo.position[1];
                    tmpInfo.normal[2] = 0;
                    MeVector3Normalize(tmpInfo.normal);
                    hit = 1;
                }
            }
        }
    }

    // If we didn't hit anything - return;
    if(!hit)
        return 0;
    
    // finally, transform contact info back into world frame.
    MeMatrix4TMTransform(info->position, tm, tmpInfo.position);
    MeMatrix4TMRotate(info->normal, tm, tmpInfo.normal);
    info->distance = tmpInfo.distance;

    return hit;
}

MCD_IMPLEMENT_LINESEG_REGISTRATION(Sphyl);
