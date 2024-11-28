/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:57 $ - Revision: $Revision: 1.70.2.4 $

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
#include <McdProfile.h>

#include <McdCTypes.h>
#include <McdBox.h>
#include <McdPlane.h>
#include <McdCheck.h>
#include <McdModel.h>
#include <McdContact.h>
#include <McdModelPair.h>
#include <McdGeometry.h>
#include <McdInteractionTable.h>
#include <McdPrimitives.h>
#include <lsTransform.h>



/*
    Temporary fix; probably required only for the 991111 PS2 compiler.
*/

#ifdef PS2
#   define inline
#endif

/*
 *  McdBoxPlane implementation
 */

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

static inline void McdVanillaBoxPlaneIntersect(lsVec3* &outList,
    const lsTransform &tAB,const lsVec3 &inRBox)
{
    const lsVec3 &t = tAB.t();
    const lsVec3 &d0 = inRBox[0]*tAB.axis(0);
    const lsVec3 &d1 = inRBox[1]*tAB.axis(1);
    const lsVec3 &d2 = inRBox[2]*tAB.axis(2);

    const MeReal invD0Z = MeSafeRecip(d0[2]);
    const MeReal invD1Z = MeSafeRecip(d1[2]);
    const MeReal invD2Z = MeSafeRecip(d2[2]);

    // x-direction edges:
    {
        const lsVec3 &t_p_d1 = t+d1;
        const lsVec3 &t_m_d1 = t-d1;

        McdVanillaAddPlaneBoxClippedPoints(outList,t_p_d1+d2,d0,invD0Z); // ++
        McdVanillaAddPlaneBoxClippedPoints(outList,t_p_d1-d2,d0,invD0Z); // +-

        McdVanillaAddPlaneBoxClippedPoints(outList,t_m_d1-d2,d0,invD0Z); // --
        McdVanillaAddPlaneBoxClippedPoints(outList,t_m_d1+d2,d0,invD0Z); // -+
    }

    // y-direction edges:
    {
        const lsVec3 &t_p_d0 = t+d0;
        const lsVec3 &t_m_d0 = t-d0;

        McdVanillaAddPlaneBoxClippedPoints(outList,t_p_d0+d2,d1,invD1Z); // ++
        McdVanillaAddPlaneBoxClippedPoints(outList,t_m_d0+d2,d1,invD1Z); // +-

        McdVanillaAddPlaneBoxClippedPoints(outList,t_m_d0-d2,d1,invD1Z); // --
        McdVanillaAddPlaneBoxClippedPoints(outList,t_p_d0-d2,d1,invD1Z); // -+

    // z-direction edges:

        McdVanillaAddPlaneBoxSegmentPoints(outList,t_p_d0+d1,d2,invD2Z); // ++
        McdVanillaAddPlaneBoxSegmentPoints(outList,t_p_d0-d1,d2,invD2Z); // +-

        McdVanillaAddPlaneBoxSegmentPoints(outList,t_m_d0-d1,d2,invD2Z); // --
        McdVanillaAddPlaneBoxSegmentPoints(outList,t_m_d0+d1,d2,invD2Z); // -+
    }
}

static inline bool McdVanillaOverlapOBBPlane(
    MeReal &outSep,lsVec3* &outPos,MeI16 &outDims,
    const MeReal inEps,const lsVec3 &inR,const lsTransform &inT12)
{
    // Plane is implicitly the z = 0 plane

    lsVec3 aCol2(MeFabs(inT12.axis(0)[2]),
                 MeFabs(inT12.axis(1)[2]),
                 MeFabs(inT12.axis(2)[2]));

    MeReal separation = inT12.t()[2]-
        (inR[0]*aCol2[0]+inR[1]*aCol2[1]+inR[2]*aCol2[2]);

    if (separation > inEps)
        return 0;

    outSep = separation;

    MeI16 dimA = (MeI16)(aCol2[0] < (MeReal)(1.0e-4f)) +
                 (MeI16)(aCol2[1] < (MeReal)(1.0e-4f)) +
                 (MeI16)(aCol2[2] < (MeReal)(1.0e-4f));

    outDims = (2<<8)|dimA;

    McdVanillaBoxPlaneIntersect(outPos,inT12,inR);

    return 1;
}

/*
    Box to Plane collision detection
*/

/*
     TEST-2000-09-26
     Add one contact when the center of box is below the plane
*/

MeBool MEAPI McdBoxPlaneIntersect(McdModelPair *p,McdIntersectResult *result)
{
    McdProfileStart("McdBoxPlaneIntersect");

    lsTransform *const tA = (lsTransform*) McdModelGetTransformPtr(p->model1);
    lsTransform *const tB = (lsTransform*) McdModelGetTransformPtr(p->model2);

    McdGeometry *const geometry1 = McdModelGetGeometry(p->model1);

    const MeReal eps =
        McdModelGetContactTolerance(p->model1)
        + McdModelGetContactTolerance(p->model2);

    McdFramework *fwk = p->model1->frame;

    lsVec3 &rA = *(lsVec3*) McdBoxGetRadii((McdBoxID) geometry1);
    
    lsVec3 footprint[24];
    MeReal separation;
    lsVec3 *verts = footprint;
    MeI16 dims;
    lsTransform tBA;

    tBA.thisIsFirstThenInverseSecond(*tA,*tB);

    result->touch =
        McdVanillaOverlapOBBPlane(separation,verts,dims,eps,rA,tBA);

    if (!result->touch)
    {
        result->contactCount = 0;
        return false;
    }

    /*
        Transform footprint into global frame, and find depths
    */

    *(lsVec3*) result->normal = tB->axis(2);
    
    lsVec3 *v;
    McdContact *c = result->contacts;
    const MeReal scaledSep = separation * 0.01f;

    for (v = footprint, result->contactCount = 0;
         v != verts && result->contactCount < result->contactMaxCount;
         v++)
    {
        const MeReal s = v->operator[](2);

        if (s < scaledSep)
        {
            tB->transform(*v, (lsVec3*) c->position);

            c->dims = dims;
            c->separation = s;
#ifdef MCD_CURVATURE
            c->curvature1 = 0;
            c->curvature2 = 0;
#endif
            *(lsVec3*)c->normal = *(lsVec3*)result->normal;

            c++, result->contactCount++;
        }
    }

    McdProfileEnd("McdBoxPlaneIntersect");

    return true;
}


int MEAPI
McdBoxPlaneSafeTime( McdModelPair* p, MeReal maxTime, McdSafeTimeResult *result){
    result->pair = p;
    
    McdBoxID boxG = (McdBoxID)McdModelGetGeometry(p->model1);
    McdPlaneID planeG = (McdPlaneID)McdModelGetGeometry(p->model2);
    MCD_ASSERT( McdGeometryGetTypeId(boxG) == kMcdGeometryTypeBox, "McdBoxPlaneSafeTime");
    MCD_ASSERT( McdGeometryGetTypeId(planeG) == kMcdGeometryTypePlane, "McdBoxPlaneSafeTime");
    
    // MeVector4* boxTM = (MeVector4*) McdModelGetTransformPtr(p->model1);
    MeMatrix4Ptr planeTM = McdModelGetTransformPtr(p->model2);
    
    lsVec3 relV = *(lsVec3*)McdModelGetLinearVelocityPtr(p->model1) -
        *(lsVec3*)McdModelGetLinearVelocityPtr(p->model2);
    //    MeReal* boxV = McdModelGetLinearVelocityPtr(p->model1);
    //    MeReal* planeV = McdModelGetLinearVelocityPtr(p->model2);
    //    lsVec3 relV, tmpV;
    //    relV.setValue(boxV[0],boxV[1],boxV[2]);
    //    tmpV.setValue(planeV[0],planeV[1],planeV[2]);
    //    relV -= tmpV;
    
    MeReal relVnorm = relV.norm();
    if (relVnorm*maxTime< 1e-4) { // should be related to box size
        result->time = maxTime;
        return 0;
    }
    
    lsVec3 planeN;
    
    McdPlaneGetNormal( planeG, McdModelGetTransformPtr(p->model2),
        (MeReal*)&planeN);
    
    planeN *= (-1);
    
    lsVec3 closestPoint; // on box to plane
    McdBoxMaximumPoint(McdModelGetGeometryInstance(p->model1),
        (MeReal*)planeN.getValue(), (MeReal*)closestPoint.getValue());
    
    const MeReal dist = McdPlaneGetDistanceToPoint(planeG, planeTM,
        closestPoint.getValue());
    
    result->time = dist/planeN.dot(relV);
    return 1;
}


MCD_IMPLEMENT_SAFETIME_REGISTRATION(Box,Plane,1)
