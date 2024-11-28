/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:57 $ - Revision: $Revision: 1.59.2.3 $

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
#include <MeMath.h>
#include <McdProfile.h>

#include <McdBox.h>
#include <McdSphere.h>
#include <McdCheck.h>
#include <McdModel.h>
#include <McdContact.h>
#include <MovingBoxBoxIntersect.h>
#include <McdInteractionTable.h>
#include <McdPrimitives.h>

/*
    Temporary fix; probably required only for the 991111 PS2 compiler.
*/

#ifdef PS2
#   define inline
#endif

static inline bool McdVanillaOverlapOBBSphere(MeReal &outSep,
    lsVec3 &outN,lsVec3 &outPos,MeI16 &outDims,
    const MeReal inEps,const lsVec3 &inROBB,
    const MeReal inRSphere,const lsVec3 &inPos)
{
    int outsides = 0, i;
    MeReal dist = 0, d;

    outN.setZero();

    for(i=0;i<3;i++)
    {
        if(inPos[i]>inROBB[i])
        {
            d = inROBB[i] - inPos[i];
            dist+=d*d;
            outN[i]=d; // normal points from sphere to box
            outsides++;
        }
        else if (inPos[i]<-inROBB[i])
        {
            d = -inROBB[i] - inPos[i];
            dist+=d*d;
            outN[i]=d;
            outsides++;
        }
    }

    if(dist > (inRSphere + inEps) * (inRSphere + inEps))
        return false;

    if(dist>inEps*inEps)
    {
        outDims = (3<<8)|(3-outsides);
        MeReal len = outN.norm();
        outN/=len;
        outPos = inPos + outN * inRSphere;
        outSep = len - inRSphere;
        return true;
    }

    // Centre is inside box, so just pick the nearest face

    MeReal separation = MEINFINITY;
    int best;
    MeReal sign;
    for(i=0;i<3;i++)
    {
        if(MeFabs(inROBB[i] - inPos[i]) < separation)
        {
            separation = MeFabs(inROBB[i] - inPos[i]);
            best = i;
            sign = -1;
        }
        if(MeFabs(inPos[i]+inROBB[i]) < separation)
        {
            separation = MeFabs(inPos[i]+inROBB[i]);
            best = i;
            sign = 1;
        }
    }

    outN[best] = sign;
    outSep = MeFabs(inPos[best]) - inROBB[best] - inRSphere;
    outPos = inPos + outN * inRSphere;
    outDims = (3<<8)|2;
    return true;
}

/*
 * McdBoxSphere implementation
 */

MeBool MEAPI
McdBoxSphereIntersect( McdModelPair* p, McdIntersectResult *result )
{
    McdProfileStart("McdBoxSphereIntersect");

    lsTransform *tA = (lsTransform*)McdModelGetTransformPtr( p->model1 );
    lsTransform *tB = (lsTransform*)McdModelGetTransformPtr( p->model2 );
    
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    McdBoxID geometry1 = (McdBoxID)McdModelGetGeometry( p->model1 );
    McdSphereID geometry2 = (McdSphereID)McdModelGetGeometry( p->model2 );
    
    lsVec3 &rA = *(lsVec3*)McdBoxGetRadii( geometry1 );
    const MeReal rB = McdSphereGetRadius( geometry2);

    result->contactCount = 0;
    result->touch = 0;

    lsTransform tAB;
    tAB.thisIsFirstThenInverseSecond(*tB,*tA);
    
    MeI16 dims;
    lsVec3 pos;
    MeReal separation;
    lsVec3 normal;
    
    if (!McdVanillaOverlapOBBSphere(separation,
            normal,pos,dims,eps,rA,rB,tAB.t())) 
    {
        McdProfileEnd("McdBoxSphereIntersect");
        return false;
    }
    
    MCD_CHECK_ASSERT((result->contactMaxCount>0),
        kMcdErrorNum_contactBufferOverrun, "", "OBBSphere");
    
    if (result->contactMaxCount > 0) 
    {
        tA->transformWithoutTranslate(normal,(lsVec3*)result->normal);
        
        result->contacts->dims = dims;
#ifdef MCD_CURVATURE
        result->contacts->curvature1 = 0;
        result->contacts->curvature2 = rB;
#endif
        
        result->contacts->separation = separation;
        
        // Transform vectors into global frame
        tA->transform(pos,(lsVec3*)result->contacts->position);
        
        *(lsVec3*)result->contacts->normal = *(lsVec3*)result->normal;
        result->contactCount = 1;
    }

    McdProfileEnd("McdBoxSphereIntersect");
    return result->touch = 1;
}

#define box_sph_rad_const (2/(MeSqrt((MeReal)2.0)+1))

int MEAPI
McdBoxSphereSafeTime( McdModelPair* p, MeReal maxTime, McdSafeTimeResult *result){
    result->pair = p;
    result->time = maxTime;
    
    lsVec3 *V0 = (lsVec3*)McdModelGetLinearVelocityPtr(p->model1);
    lsVec3 *V1 = (lsVec3*)McdModelGetLinearVelocityPtr(p->model2);
    
    lsTransform *tm0 = (lsTransform *)McdModelGetTransformPtr(p->model1);
    lsTransform *tm1 = (lsTransform *)McdModelGetTransformPtr(p->model2);
    
    McdBoxID geometry0 = (McdBoxID)McdModelGetGeometry( p->model1 );
    McdSphereID geometry1 = (McdSphereID)McdModelGetGeometry( p->model2 );
    
    MeReal radS = McdSphereGetRadius(geometry1);
    radS *= box_sph_rad_const;
    
    MeReal *ext0 = (MeReal*)McdBoxGetRadii(geometry0);
    MeReal ext1[3] = {radS, radS, radS};
    
    //  MeReal *ext1 = (MeReal*)McdBoxGetRadii(geometry1);
    
    MeReal T;
    lsVec3 P;
    
    unsigned int ixt = MovingBoxBoxIntersect( ext0, tm0, *V0, ext1, tm1, *V1, maxTime, T, P );
    
    if ( ixt ) {
        result->time = T;       // during [t0, t1], box/box intersect
        // if result->time = 0 --> box/box intersect at t0
    } else
        result->time = maxTime; // during [t0, t1], box/box does not intersect
    
    return ixt;
}


MCD_IMPLEMENT_SAFETIME_REGISTRATION(Box,Sphere,0)
