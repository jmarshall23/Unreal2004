/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/17 13:44:35 $ - Revision: $Revision: 1.68.2.3 $

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

#include <stdio.h>
#include <McdCheck.h>
#include <McdBox.h>
#include <McdSphere.h>
#include <McdPrimitives.h>
#include <McdModel.h>
#include <McdPlane.h>
#include <lsTransform.h>
#include <MeMath.h>

// const MeReal EPS  = ME_SMALL_EPSILON;

enum
OverlapStatus
{
    DISJOINT,
        INTERSECT,
        INSIDE
};

// A struct to identify a face of a box.
struct FaceId {
    MeU16 axis;
    MeU16 minside;
};

/* not in use: 20/02/2000 */
bool IxCenteredAABBLineSegment( const lsVec3& rbox,
                               const lsVec3& pt1, const lsVec3& pt2 );

OverlapStatus IxRayAABB( MeReal* const inMinB, MeReal* const inMaxB,
                        MeReal* const inOrig, MeReal* const inDir,
                        MeReal* outInsct, MeReal* outLen, FaceId* outFaceId);

                        /*--------------------------------------------------------------------------
                        (1). inOrig and inDest are in world c.s.                                  *
                        (2). box is axis-aligned and origin centered in its local c.s.            *
--------------------------------------------------------------------------*/
int MEAPI
IxBoxLineSegment( const McdModelID model,
                 MeReal* const inOrig, MeReal* const inDest,
                 McdLineSegIntersectResult * info )
{
    McdGeometry *box = McdModelGetGeometry(model);
    MCD_CHECK_ASSERT_(inOrig != NULL && inDest != NULL && box != NULL && info != NULL, "IxBoxLineSegment");
    MCD_CHECK_ASSERT_(McdGeometryGetTypeId(box) == kMcdGeometryTypeBox, "IxBoxLineSegment");
    
    lsTransform *gtm = (lsTransform*)McdModelGetTransformPtr(model);
    
    lsVec3 p0_inboxcs, p1_inboxcs;
    
    // transform two points into box's c.s.
    gtm->inverseTransform( *(lsVec3*)inOrig, &p0_inboxcs );
    gtm->inverseTransform( *(lsVec3*)inDest, &p1_inboxcs );
    
    // get box radii
    lsVec3 &r = *(lsVec3*)McdBoxGetRadii((McdBoxID)box);
    
    lsVec3 minB, inMaxB, outInst, dirLS;
    MeReal dist;
    int i;
    for (i =0; i< 3; i++) {
        minB[i] = -r[i]; inMaxB[i] = r[i];
    }
    
    dirLS = p1_inboxcs - p0_inboxcs;
    MeReal normDir = dirLS.norm();
    
    // TempAssert( normDir > ME_SMALL_EPSILON ); // zero length LineSegment
    if ( normDir < ME_SMALL_EPSILON ) return 0;
    
    MeReal norDirInv = MeSafeRecip(normDir);
    dirLS *= norDirInv;
    
    lsVec3 insctPoint;
    FaceId faceId;
    
    OverlapStatus bIsct;
    bIsct = IxRayAABB(minB.v, inMaxB.v, p0_inboxcs.v, dirLS.v,
        insctPoint.v, &dist, &faceId);
    
    if ( bIsct == DISJOINT || dist > normDir ) return 0;
    
    gtm->transform( insctPoint, (lsVec3*)info->position);
    
    if ( bIsct == INTERSECT) {
        lsVec3 *axis = (lsVec3*) info->normal;
        *axis = gtm->axis(faceId.axis);
        if ( faceId.minside ) {
            info->normal[0]=-info->normal[0];
            info->normal[1]=-info->normal[1];
            info->normal[2]=-info->normal[2];
        }
    }
    
    info->distance = dist;
    
    return 1;
}


/*-------------------------------------------------------------------------*/
OverlapStatus
IxRayAABB(MeReal* const inMinB,
          MeReal* const inMaxB, MeReal* const inOrig,
          MeReal* const inDir, MeReal* outInsct,
          MeReal* outLen, FaceId* outFaceId)
{
    enum          {MAXSIDE, MINSIDE, MIDDLE};
    bool          insideBox= 1;
    unsigned int  iQuad[3];
    int           i, iPlane;
    MeReal        maxT[3], candPlane[3];
    
    /* determine candidate planes */
    for (i=0; i<3; i++) {
        if(inOrig[i] < inMinB[i]) {
            iQuad[i] = MINSIDE;
            candPlane[i] = inMinB[i];
            insideBox = 0;
        }
        else if (inOrig[i] > inMaxB[i]) {
            iQuad[i] = MAXSIDE;
            candPlane[i] = inMaxB[i];
            insideBox = 0;
        }
        else    { iQuad[i] = MIDDLE; }
    }
    
    /* ray origin inside AABB */
    if(insideBox) {
        for (i=0; i<3; i++) outInsct[i] = inOrig[i]; // return inOrig as InsctPnt
        *outLen = 0;
        return INSIDE;
    }
    
    /* distances to candidate planes */
    for (i = 0; i < 3; i++) {
        if (iQuad[i] != MIDDLE && inDir[i] !=(MeReal)(0.))
            maxT[i] = (candPlane[i]-inOrig[i]) / inDir[i];
        else
            maxT[i] = -(MeReal)(1.);
    }
    
    /* determine which plane used for potential intersection test */
    iPlane = 0;
    for (i = 1; i < 3; i++) if (maxT[iPlane] < maxT[i]) iPlane = i;
    
    /* final check for real hit */
    if (maxT[iPlane] < (MeReal)(0.)) return DISJOINT;
    
    for (i = 0; i < 3; i++) {
        if (iPlane != i) {
            outInsct[i] = inOrig[i] + maxT[iPlane] *inDir[i];
            if (outInsct[i] < inMinB[i] || outInsct[i] > inMaxB[i])
                return DISJOINT;
        }
        else
        {
            outInsct[i] = candPlane[i];
        }
    }
    
    outFaceId->axis     = iPlane;
    outFaceId->minside  = (MeU16) (iQuad[iPlane]==MINSIDE);
    *outLen             = maxT[iPlane];
    
    return INTERSECT;
}

/*--------------------------------------------------------------------------
(1). inOrig and inDest are in world c.s.                                  *
(2). spher is axis-aligned and origin centered in its local c.s.          *
--------------------------------------------------------------------------*/
int MEAPI
IxSphereLineSegment(const McdModelID model,
                    MeReal* const inOrig, MeReal* const inDest,
                    McdLineSegIntersectResult * info )
{
    
    McdSphereID sphere = (McdSphereID)McdModelGetGeometry(model);
    MCD_CHECK_ASSERT_(inOrig != NULL && inDest != NULL && sphere != NULL, "IxSphereLineSegment");
    MCD_CHECK_ASSERT_( McdGeometryGetTypeId(sphere) == kMcdGeometryTypeSphere, "IxSphereLineSegment");
    
    lsVec3 *p0 = (lsVec3*)inOrig;
    lsVec3 *p1 = (lsVec3*)inDest;
    lsTransform *gtm = (lsTransform*)McdModelGetTransformPtr(model);
    
    lsVec3 center;  // sphere center in world's c.s.
    gtm->getTranslation( &center );
    MeReal R = McdSphereGetRadius( sphere);
    MeReal R2 = R*R;
    
    lsVec3 rayDir = (*p1 - *p0);
    
    MeReal normRayDir = rayDir.norm();
    // TempAssert( normRayDir > ME_SMALL_EPSILON );
    if (normRayDir < ME_SMALL_EPSILON) return 0;
    
    MeReal normRayDirInv = MeSafeRecip(normRayDir);
    rayDir *= normRayDirInv;
    
    // rayDir is unit vector in direction of ray.

    lsVec3 origToCenter = center - *p0; // vector from start of line to center of sphere.
    MeReal D = origToCenter.dot(rayDir); // distance of sphere centre in direction of query.
    MeReal L2 = origToCenter.dot(origToCenter); // Distance of line start from sphere centre (squared).
    
    // We are inside sphere
    if ( L2 < R2 )
    {
        info->distance = 0;
        MeVector3Copy(info->position, inOrig);
        return 1;
    }

    if ( D < 0 && L2 > R2 )
        return 0; // sphere is behind us, but we are not inside it
    
    MeReal D2 = D*D;
    MeReal M2 = L2 - D2; // pythag - triangle involving origToCenter, (rayDir * D) and vec between centre & (rayDir * D)
    
    if (M2 > R2) 
        return 0; // ray misses sphere
    
    // this does pythag again. Q2 = R2 (radius squared) - M2
    MeReal Q = MeSqrt(R2 - M2);
    MeReal t = D - Q;
    
    // If ray doesn't reach sphere, reject here.
    if ( t > normRayDir ) 
        return 0;

    lsVec3 *p = (lsVec3*)info->position;
    *p = *p0 + rayDir*t;
    
    MeReal RI = MeSafeRecip(R);
    for (int i=0; i<3; i++) 
        info->normal[i] = RI*(info->position[i]-center[i]);
    
    info->distance = t;
    
    return 1;
}

/*--------------------------------------------------------------------------*
inOrig and inDest are in world c.s.                                        *
---------------------------------------------------------------------------*/
int MEAPI
IxPlaneLineSegment( const McdModelID model,
                   MeReal* const inOrig, MeReal* const inDest,
                   McdLineSegIntersectResult * info )
{
    McdGeometry *plane = McdModelGetGeometry(model);
    MCD_CHECK_ASSERT_(inOrig != NULL && inDest != NULL && plane != NULL, "IxPlaneLineSegment");
    MCD_CHECK_ASSERT_( McdGeometryGetTypeId(plane) == kMcdGeometryTypePlane,"IxPlaneLineSegment");
    
    lsVec3 *p0 = (lsVec3*)inOrig;
    lsVec3 *p1 = (lsVec3*)inDest;
    lsTransform *gtm = (lsTransform*)McdModelGetTransformPtr(model);
    
    lsVec3 p0_intrics, p1_intrics;
    
    // tranform two points into tris's c.s.
    gtm->inverseTransform( *p0, &p0_intrics );
    gtm->inverseTransform( *p1, &p1_intrics );
    
    if ( p0_intrics[2] > 0 && p1_intrics[2] > 0 ) return 0;
    
    // the line segment is on the back-side of the palne.
    // can be considered as intersection ? (maybe yes)
    // Dilip: Yes, but we have to start making clear
    // that the plane is a halfspace
    
    if(p0_intrics[2] < 0)
    {
        *(lsVec3*) info->position = *p0;
        *(lsVec3*) info->normal = gtm->axis(2);
        info->distance = 0;
        return 1;
    }

    lsVec3 dirLS = p1_intrics - p0_intrics;

    if ( MeFabs(dirLS[2]) < ME_SMALL_EPSILON && p0_intrics[2]>0) 
        return 0;
    
    lsVec3 ip;
    
    MeReal t = -p0_intrics[2]/dirLS[2];
    ip[0] = p0_intrics[0]+dirLS[0]*t;  // re-use of this variable, bad?
    ip[1] = p0_intrics[1]+dirLS[1]*t;
    ip[2] = 0;

    gtm->transform( ip, (lsVec3*)info->position);    
    *(lsVec3*) info->normal = gtm->axis(2);
    info->distance = t*dirLS.norm();
    
    if (p0_intrics[2] < 0) info->distance = -info->distance;
    
    return 1;
}

MCD_IMPLEMENT_LINESEG_REGISTRATION(Box)
MCD_IMPLEMENT_LINESEG_REGISTRATION(Sphere)
MCD_IMPLEMENT_LINESEG_REGISTRATION(Plane)

