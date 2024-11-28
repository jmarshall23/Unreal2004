/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:57 $ - Revision: $Revision: 1.41.2.3 $

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

#include <McdProfile.h>
#include <vectormath.h>

#include <McdCylinder.h>
#include <McdPlane.h>
#include <McdModel.h>
#include <McdContact.h>
#include "McdCheck.h"
#include <McdModelPair.h>
#include <McdInteractionTable.h>
#include <McdPrimitives.h>
#include <lsTransform.h>


static inline MeBool McdVanillaSegmentPlaneIntersect(MeReal &tInMax,
    MeReal &tOutMin,
    const lsVec3 &orig, const lsVec3 &disp, const MeReal invDisp)
{
    /* t = (p*n)/(r*n) */

    const MeReal den = disp[2];  /* = r*n */
    const MeReal t = -orig[2]*invDisp;

    if (den > (MeReal)(0.0f))
    {
        if (t < tOutMin)
            tOutMin = t;
    }
    else if (den < (MeReal)(0.0f))
    {
        if (t > tInMax)
            tInMax = t;
    }
    else if (orig[2] > (MeReal)(0.0f))
        return false; /* outside slab */

    return tOutMin >= tInMax;
}

static inline void McdVanillaAddPlaneBoxClippedPoints(lsVec3* &outList,
    const lsVec3 &orig,const lsVec3 &disp,const MeReal invDisp)
{
    MeReal tIn = -(MeReal) 1.0f;
    MeReal tOut = (MeReal) 1.0f;

    if (McdVanillaSegmentPlaneIntersect(tIn,tOut,orig,disp,invDisp))
    {
        if (tIn > -(MeReal) 1.0f)
            *outList++ = orig+tIn*disp;

        if (tOut < (MeReal) 1.0f)
            *outList++ = orig+tOut*disp;
    }
}

static inline void McdVanillaAddPlaneBoxSegmentPoints(lsVec3* &outList,
    const lsVec3 &orig,const lsVec3 &disp,const MeReal invDisp)
{
    MeReal tIn = -(MeReal) 1.0f;
    MeReal tOut = (MeReal) 1.0f;

    if (McdVanillaSegmentPlaneIntersect(tIn,tOut,orig,disp,invDisp))
    {
        *outList++ = orig+tIn*disp;
        *outList++ = orig+tOut*disp;
    }
}

static void CylPlaneIntersect(lsVec3* &outList,
    const lsTransform &tAB,const MeReal inR, const MeReal inHH,
    const lsVec3 &nPerp, const lsVec3 &nPara, const lsVec3 &cylAxis)
{
    const lsVec3 d0 = inR*nPara;
    const lsVec3 d1 = inR*nPerp;
    const lsVec3 d2 = inHH*cylAxis;

    const MeReal invD0Z = MeSafeRecip(d0[2]);
    const MeReal invD1Z = MeSafeRecip(d1[2]);
    const MeReal invD2Z = MeSafeRecip(d2[2]);

    lsVec3 *listBase = outList;

    // z-direction edges:
    McdVanillaAddPlaneBoxSegmentPoints(outList,tAB.t()+d0,d2,invD2Z); // +0
    McdVanillaAddPlaneBoxSegmentPoints(outList,tAB.t()+d1,d2,invD2Z); // 0+
    McdVanillaAddPlaneBoxSegmentPoints(outList,tAB.t()-d0,d2,invD2Z); // -0
    McdVanillaAddPlaneBoxSegmentPoints(outList,tAB.t()-d1,d2,invD2Z); // 0-

    // cyl. perp-axes:
    McdVanillaAddPlaneBoxClippedPoints(outList,tAB.t()+d2,d1,invD1Z);
    McdVanillaAddPlaneBoxClippedPoints(outList,tAB.t()-d2,d1,invD1Z);


    if (outList == listBase) 
    {
        MeReal n2 = cylAxis[2]*cylAxis[2];
        lsVec3 *v0 = outList++;
        *v0 = tAB.t();
        if (n2 > (MeReal)(1.0)-1.0e-10) 
        {
            if (cylAxis[2] < 0.0) 
                *v0 += d2;
            else 
                *v0 -= d2;
        } 
        else if (n2 < (MeReal)(1.0e-10)) 
        {
            v0->operator[](2) -= inR;
        } 
        else 
        {
            *v0 -= d1;
            if (cylAxis[2] < 0.0) 
                *v0 += d2;
            else 
                *v0 -= d2;
        }
    }
}


static bool OverlapCylPlane(MeReal &outSep, lsVec3* &outPos,
    const MeReal inEps,const MeReal inRCyl,
    const MeReal inHHCyl,const lsTransform &inT12)
{
    const lsVec3 &cylAxis = inT12.axis(2);
    lsVec3 nPara = Vec3CrossAxis(cylAxis,2);
    lsVec3 nPerp;

    if (nPara.square_norm() < 1.0e-15) 
    {
        nPara.setValue(0.0,1.0,0.0);
        nPerp.setValue(1.0,0.0,0.0);
    } else 
    {
        nPara.normalize();
        nPerp = nPara.cross(cylAxis);
    }

    const MeReal rZ = MeFabs(cylAxis[2])*inHHCyl+MeFabs(nPerp[2])*inRCyl;

    outSep = inT12.t()[2]-rZ;

    if (outSep > inEps) {
        return false;
    }

    CylPlaneIntersect(outPos,inT12,inRCyl,inHHCyl,nPara,nPerp,cylAxis);

    return true;
}

MeBool MEAPI McdCylinderPlaneIntersect(McdModelPair *p,
    McdIntersectResult *result)
{
    McdProfileStart("McdCylinderPlaneIntersect");
    
    lsTransform *tA = (lsTransform*)McdModelGetTransformPtr( p->model1 );
    lsTransform *tB = (lsTransform*)McdModelGetTransformPtr( p->model2 );
    
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    McdCylinderID geometry1 = (McdCylinderID)McdModelGetGeometry( p->model1 );
    McdPlaneID geometry2 = (McdPlaneID)McdModelGetGeometry( p->model2 );

    McdFramework *fwk = p->model1->frame;
    result->touch = 0;
    result->contactCount = 0;

    // Radii
    const MeReal rCyl = McdCylinderGetRadius(geometry1);
    const MeReal hhCyl = McdCylinderGetHalfHeight(geometry1);


    lsTransform tBA;
        tBA.thisIsFirstThenInverseSecond(*tA,*tB);

    lsVec3 footprint[12];
    lsVec3 *verts = footprint;
    MeReal separation;

    // Overlap test, return overlap values

    if (!OverlapCylPlane(separation,verts,eps,rCyl,hhCyl,tBA))
    {
        McdProfileEnd("McdCylinderPlaneIntersect");
        return 0;
    }

    // Transform footprint into global frame, and find depths
    *(lsVec3*)result->normal = tB->axis(2);

    MeReal ZZ2 = tBA.axis(2)[2]*tBA.axis(2)[2];
    MeI16 dimA;

    if (ZZ2 > (MeReal)(0.999999f)) 
        dimA = 2;
    else if (ZZ2 < (MeReal)(0.000001f)) 
        dimA = 3; // usually reserved for sphere - need a different type?
    else
        dimA = 1;

#ifdef MCD_CURVATURE
#endif

    // Transform footprint into global frame, and find depths
    lsVec3 *v = footprint;
    McdContact *c = result->contacts;
    result->contactCount = 0;
    while (v != verts && result->contactCount < result->contactMaxCount) 
    {
        MeReal s = v->operator[](2);
        if (s < separation*(MeReal)(0.01)) 
        {
            tB->transform(*v,(lsVec3*)c->position);
            c->dims = (2<<8)|dimA;
            c->separation = s;

#ifdef MCD_CURVATURE
            if (ZZ2 > (MeReal)(0.999999)) 
                c->curvature1 = 0.0;
            else if (ZZ2 < (MeReal)(0.000001)) 
                c->curvature1 = rCyl;
            else
                c->curvature1 = rCyl*MeRecipSqrt((MeReal)(1.0)-ZZ2);
            c->curvature2 = 0;
#endif
            *(lsVec3*)c->normal = *(lsVec3*)result->normal;
            result->contactCount++;
            c++;
        }
        v++;
    }
    
    McdProfileEnd("McdCylinderPlaneIntersect");
    return result->touch = 1;
}

MCD_IMPLEMENT_INTERSECT_REGISTRATION(Cylinder,Plane,1)
