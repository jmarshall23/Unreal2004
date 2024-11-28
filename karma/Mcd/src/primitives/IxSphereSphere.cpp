/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:57 $ - Revision: $Revision: 1.50.2.2 $

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
#include <MeMath.h>
#include <McdSphere.h>
#include <McdCheck.h>
#include <McdModel.h>
#include <McdContact.h>
#include <McdInteractionTable.h>
#include <McdPrimitives.h>
#include <lsTransform.h>

/*********************************************************
    Sphere to Sphere collision detection
 *********************************************************/

int MEAPI
McdSphereSphereIntersect( McdModelPair* p, McdIntersectResult *result )
{
    McdProfileStart("McdSphereSphereIntersect");

    lsTransform &tA = *(lsTransform*)McdModelGetTransformPtr( p->model1 );
    lsTransform &tB = *(lsTransform*)McdModelGetTransformPtr( p->model2 );
    
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    McdSphereID geometry1 = (McdSphereID)McdModelGetGeometry( p->model1 );
    McdSphereID geometry2 = (McdSphereID)McdModelGetGeometry( p->model2 );
    
    result->contactCount = 0;
    result->touch = 0;

    const MeReal rA = McdSphereGetRadius( geometry1 );
    const MeReal rB = McdSphereGetRadius( geometry2 );
    
    const lsVec3 &pA = tA.t();
    const lsVec3 &pB = tB.t();
    
    const MeReal sumR = rA+rB;
    lsVec3 normal = pA-pB;
    MeReal lenN = normal.square_norm();
    
    if (lenN == (MeReal)(0.0)) 
        normal.setValue(0,0,1);
    else 
    {
        lenN = MeSqrt(lenN);
        normal *= MeSafeRecip(lenN);
    }
    const MeReal separation = lenN-sumR;
    
    if (separation > eps) 
    {
        McdProfileEnd("McdSphereSphereIntersect");
        return 0;
    }
    
    MCD_CHECK_ASSERT((result->contactMaxCount>0), kMcdErrorNum_contactBufferOverrun, "", "McdSphereSphereSafeTime");
    
    if(result->contactMaxCount > 0) 
    {
        result->contacts->dims = (3<<8)|3;
        
#ifdef MCD_CURVATURE
        result->contacts->curvature1 = rA;
        result->contacts->curvature2 = rB;
#endif
        
        *(lsVec3*)result->contacts->normal = normal;
        result->contacts->separation = separation;
        
        const MeReal invSumR = MeSafeRecip(sumR);
        const MeReal wA = rB * invSumR;
        const MeReal wB = rA * invSumR;
        *(lsVec3*)result->contacts->position = wA*pA+wB*pB;
        *(lsVec3*)result->normal = normal;
        
        result->contactCount = 1;
    }
    else    
        result->contactCount = 0;

    McdProfileEnd("McdSphereSphereIntersect");
    return result->touch = 1;
}

/**
SafeTime of flight calculation
Time should be less than maxTime
return: 0 - no valid contact predicted
1 - valid result returned
2 - Bug report please Error in internal cases
*/

int MEAPI
McdSphereSphereSafeTime( McdModelPair* p, MeReal maxTime, McdSafeTimeResult *result) {
    result->pair = p;
    result->time = maxTime;
    lsVec3 velS1, velS2;
    MeReal *velocityS1 = McdModelGetLinearVelocityPtr(p->model1);
    lsTransform& xformS1 = *(lsTransform*)McdModelGetTransformPtr(p->model1);
    MeReal *velocityS2 = McdModelGetLinearVelocityPtr(p->model2);
    lsTransform& xformS2 = *(lsTransform*)McdModelGetTransformPtr(p->model2);
    lsVec3 posS1=xformS1.t();
    lsVec3 posS2=xformS2.t();
    
    velS1.setValue(velocityS1[0],velocityS1[1],velocityS1[2]);
    velS2.setValue(velocityS2[0],velocityS2[1],velocityS2[2]);
    
    McdSphereID geometry1 = (McdSphereID)McdModelGetGeometry( p->model1 );
    McdSphereID geometry2 = (McdSphereID)McdModelGetGeometry( p->model2 );
    
    const MeReal radS1 = McdSphereGetRadius(geometry1);
    const MeReal radS2 = McdSphereGetRadius(geometry2);
    const MeReal proxSq = MeSqr(radS1 + radS2);
    
    //  Move to Model1 frame of reference
    lsVec3 pose = posS2 - posS1 ;
    lsVec3 velocity = velS2 - velS1;
    MeReal proj = pose.dot(velocity);
    
    if (velocity.norm()*maxTime < 0.001) {
        return 0;
    }
    
    if (proj < 0) {
        result->time = (pose.norm()-radS1-radS2) / velocity.norm();
        return 1;
    }else {
        result->time = -1* (pose.norm()-radS1-radS2) / velocity.norm();
        return 0;
    }
    
}



MCD_IMPLEMENT_SAFETIME_REGISTRATION(Sphere,Sphere,0)

