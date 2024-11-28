/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:57 $ - Revision: $Revision: 1.55.2.2 $

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
#include <McdPlane.h>
#include <McdModel.h>
#include <McdContact.h>
#include <McdCheck.h>
#include <McdInteractionTable.h>
#include <McdPrimitives.h>
#include <MeMessage.h>

#include <lsTransform.h>

/*********************************************************
 Sphere to Plane collision detection
*********************************************************/

int MEAPI
McdSpherePlaneIntersect( McdModelPair* p, McdIntersectResult *result )
{
    McdProfileStart("McdSpherePlaneIntersect");

    lsTransform &tA = *(lsTransform*)McdModelGetTransformPtr( p->model1 );
    lsTransform *tB = (lsTransform*)McdModelGetTransformPtr( p->model2 );
    
    const MeReal eps
        = McdModelGetContactTolerance( p->model1 )
        + McdModelGetContactTolerance( p->model2 );
    
    McdSphereID geometry1 = (McdSphereID)McdModelGetGeometry( p->model1 );
    McdPlaneID geometry2 = (McdPlaneID)McdModelGetGeometry( p->model2 );
    
    result->contactCount = 0;
    result->touch = 0;

    const MeReal rA = McdSphereGetRadius( geometry1);
    const lsVec3 &pA = tA.t();
    
    lsVec3 disp = pA - tB->t();
    lsVec3 normal = tB->axis(2);
    
    const MeReal separation = disp.dot(normal)-rA;

    if (separation > eps) 
    {
        McdProfileEnd("McdSpherePlaneIntersect");
        return 0;
    }

    MCD_CHECK_ASSERT((result->contactMaxCount>0),
        kMcdErrorNum_contactBufferOverrun,
        "", "McdSpherePlaneIntersect");

    if (result->contactMaxCount > 0) 
    {
        result->contacts->dims = (2<<8)|3;
#ifdef MCD_CURVATURE
        result->contacts->curvature1 = rA;
        result->contacts->curvature2 = 0;
#endif
        *(lsVec3*)result->contacts->normal = normal;
        result->contacts->separation = separation;
        *(lsVec3*)result->contacts->position = pA-rA*normal;
        *(lsVec3*)result->normal = normal;
        result->contactCount = 1;
    }

    McdProfileEnd("McdSpherePlaneIntersect");
    return result->touch = 1;
}

/**
SafeTime of flight calculation
Time should be less than or equal to maxTime
return: 0 - no valid contact predicted
1 - valid result returned
2 - Bug report please Error in internal cases
*/

int MEAPI
McdSpherePlaneSafeTime( McdModelPair* p, MeReal maxTime, McdSafeTimeResult *result) {
    result->pair = p;
    result->time = maxTime;
    MeReal scale = p->model1->frame->mScale;
    lsVec3 velS, velP;
    MeReal *velocityS = McdModelGetLinearVelocityPtr(p->model1);
    lsTransform& xformS = *(lsTransform*)McdModelGetTransformPtr(p->model1);
    MeReal *velocityP = *(MeVector3 *)McdModelGetLinearVelocityPtr(p->model2);
    lsTransform& xformP = *(lsTransform*)McdModelGetTransformPtr(p->model2);
    lsVec3& posS=xformS.t();
    lsVec3& posP=xformP.t();
    
    McdSphereID geometry1 = (McdSphereID)McdModelGetGeometry( p->model1 );
    McdPlaneID geometry2 = (McdPlaneID)McdModelGetGeometry( p->model2 );
    
    velS.setValue(velocityS[0],velocityS[1],velocityS[2]);
    velP.setValue(velocityP[0],velocityP[1],velocityP[2]);
    
    const MeReal radS = McdSphereGetRadius(geometry1);
    lsVec3 normal;
    McdPlaneGetNormal(geometry2, McdModelGetTransformPtr(p->model2), (MeReal*)&normal);
    //  Move Sphere to Plane frame of reference for easy distance test.
    //    lsVec3 pose = posS - posP ;
    lsVec3 velocity = velS - velP;
    //    MeReal proj = pose.dot(velocity);
    
    MeReal dist = McdPlaneGetDistanceToPoint(geometry2, McdModelGetTransformPtr(p->model2), posS.getValue());
    MeReal vel_norm = ((velocity.dot(normal))*normal).norm();
    
    if (vel_norm * maxTime < 0.001 * scale) {
        return 0; //time = maxTime above
    }
    
    result->time = (dist-radS) / vel_norm;
    return 1;
    
}

MCD_IMPLEMENT_SAFETIME_REGISTRATION(Sphere,Plane,0)


