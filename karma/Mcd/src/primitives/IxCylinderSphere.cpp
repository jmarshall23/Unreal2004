/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:57 $ - Revision: $Revision: 1.36.2.3 $

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
#include <McdCylinder.h>
#include <McdSphere.h>
#include <McdModel.h>
#include <McdContact.h>
#include <McdCheck.h>

#include <McdInteractionTable.h>
#include <MovingBoxBoxIntersect.h>
#include <McdPrimitives.h>

int MEAPI
McdCylinderSphereIntersect( McdModelPair* p, McdIntersectResult *result );

int MEAPI
McdCylinderSphereSafeTime( McdModelPair* p, MeReal maxTime, McdSafeTimeResult *result);

/*********************************************************
Cylinder to Sphere collision detection
*********************************************************/

bool OverlapCylSphere(MeReal &outSep, lsVec3 &outN, lsVec3 &outPos,
                      MeI16 &outDims, MeReal &outCylRoC, const MeReal inEps,
                      const MeReal inRCyl, const MeReal inHHCyl,
                      const MeReal inRSphere, const lsVec3 &inPos) {
    MeReal separation;
    MeReal Dend;
    MeReal Dwall;
    MeReal normWall;
    lsVec3 edgePoint;
    lsVec3 n;
    MeReal len;
    // End-cap separation:
    Dend = MeFabs(inPos[2]);
    MeReal maxSeparation = Dend-inRSphere-inHHCyl;
    MeU8 normInfo = 0;

    if (maxSeparation > inEps) 
        return 0;
    
    // Wall separation:
    Dwall = MeSqrt(inPos[0]*inPos[0]+inPos[1]*inPos[1]);
    separation = Dwall-inRCyl-inRSphere;
    if (separation > maxSeparation) 
    {
        maxSeparation = separation;
        normInfo = 1;
        if(maxSeparation > inEps)
            return 0;
    }
    
    if (maxSeparation < -inRSphere) 
        return 0;
    
    // Falling off edge
    if (Dwall > inRCyl && Dend > inHHCyl) {
        normWall = inRCyl/Dwall;
        edgePoint.setValue(normWall*inPos[0],normWall*inPos[1],(inPos[2]>0.0?inHHCyl:-inHHCyl));
        n = edgePoint-inPos;
        len = n.normalize();
        separation = len-inRSphere;
        if (separation > maxSeparation) 
        {
            maxSeparation = separation;
            normInfo = 2;
            if(maxSeparation > inEps)
                return 0;
        }
    }

    outSep = maxSeparation;
    if (normInfo == 0) 
    {  // normal is from end cap
        outN.setValue((MeReal)(0.0),(MeReal)(0.0),(inPos[2]>(MeReal)(0.0)?(MeReal)(-1.0):(MeReal)(1.0)));
        outDims = (3<<8)|2;
        outCylRoC = (MeReal)(0.0);
    } 
    else if (normInfo == 1) 
    {    // normal is from wall
        MeReal norm = MeSafeRecip(Dwall);
        outN.setValue(-norm*inPos[0],-norm*inPos[1],(MeReal)(0.0));
        outDims = (3<<8)|3;
        outCylRoC = inRCyl;
    } 
    else
    {   // normal is from edge
        outN = n;
        outDims = (3<<8)|1;
        outCylRoC = (MeReal)(0.0);
    }
    
    outPos = inPos+inRSphere*outN;
    return 1;
}

int MEAPI
McdCylinderSphereIntersect( McdModelPair* p, McdIntersectResult *result )
{
    McdProfileStart("McdCylinderSphereIntersect");

    lsTransform &tA = *(lsTransform*)McdModelGetTransformPtr( p->model1 );
    lsTransform &tB = *(lsTransform*)McdModelGetTransformPtr( p->model2 );
    
    const MeReal eps = McdModelGetContactTolerance( p->model1 )
                     + McdModelGetContactTolerance( p->model2 );

    result->contactCount = 0;
    result->touch = 0;

    McdCylinderID geometry1 = (McdCylinderID)McdModelGetGeometry( p->model1 );
    McdSphereID geometry2 = (McdSphereID)McdModelGetGeometry( p->model2 );
    // Radii
    const MeReal rCyl = McdCylinderGetRadius(geometry1);
    const MeReal hhCyl = McdCylinderGetHalfHeight(geometry1);
    const MeReal rB = McdSphereGetRadius( geometry2);
    
    lsTransform tAB;
    tAB.thisIsFirstThenInverseSecond(tB,tA);
    
    MeI16 dims;
    lsVec3 pos;
    MeReal separation;
    lsVec3 normal;
    MeReal cylRoC;
    
    // actually returns bool
    
    if (!OverlapCylSphere(separation,normal,pos,dims,cylRoC,eps,rCyl,hhCyl,rB,tAB.t()))
    {
        McdProfileEnd("McdCylinderSphereIntersect");
        return 0;
    }
    
    MCD_CHECK_ASSERT((result->contactMaxCount>0), kMcdErrorNum_contactBufferOverrun, "", 
        "McdCylinderSphereIntersect");
    
    if (result->contactMaxCount > 0) 
    {
        tA.transformWithoutTranslate(normal,(lsVec3*)result->normal);
        result->contacts->dims = dims;
#ifdef MCD_CURVATURE
        result->contacts->curvature1 = cylRoC;
        result->contacts->curvature2 = rB;
#endif
        result->contacts->separation = separation;
        // Transform vectors into global frame
        tA.transform(pos,(lsVec3*)result->contacts->position);
        *(lsVec3*)result->contacts->normal = *(lsVec3*)result->normal;
        result->contactCount = 1;
    }

    McdProfileEnd("McdCylinderSphereIntersect");
    return result->touch = 1;
}

#define box_sph_rad_const 2/(MeSqrt((MeReal)2.0)+1)

int MEAPI
McdCylinderSphereSafeTime( McdModelPair* p, MeReal maxTime, McdSafeTimeResult *result){
    result->pair = p;
    result->time = maxTime;
    
    McdCylinderID geometry1 = (McdCylinderID)McdModelGetGeometry( p->model1 );
    McdSphereID geometry2 = (McdSphereID)McdModelGetGeometry( p->model2 );
    
    lsVec3 *V0 = (lsVec3*)McdModelGetLinearVelocityPtr(p->model1);
    lsVec3 *V1 = (lsVec3*)McdModelGetLinearVelocityPtr(p->model2);
    
    MeReal radCyl = McdCylinderGetRadius( geometry1 );
    MeReal hheightCyl = McdCylinderGetHalfHeight( geometry1 );
    radCyl *= box_sph_rad_const;
    
    MeReal radS = McdSphereGetRadius( geometry2 );
    radS *= box_sph_rad_const;
    
    lsTransform *tm1 = (lsTransform *)McdModelGetTransformPtr(p->model1);
    lsTransform *tm2 = (lsTransform *)McdModelGetTransformPtr(p->model2);
    
    
    MeReal ext1[3] = {radCyl, radCyl, hheightCyl};
    MeReal ext2[3] = {radS, radS, radS};
    
    //  MeReal *ext1 = (MeReal*)McdBoxGetRadii(geometry2);
    
    MeReal T;
    lsVec3 P;
    
    unsigned int ixt = MovingBoxBoxIntersect( ext1, tm1, *V0, ext2, tm2, *V1, maxTime, T, P );
    
    if ( ixt ) {
        result->time = T;       // during [t0, t1], box/box intersect
        // if result->time = 0 --> box/box intersect at t0
    } else
        result->time = maxTime; // during [t0, t1], box/box does not intersect
    
    return ixt;
}

MCD_IMPLEMENT_SAFETIME_REGISTRATION(Cylinder,Sphere,0)
