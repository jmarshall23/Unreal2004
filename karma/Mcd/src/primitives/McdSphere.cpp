/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/17 13:44:35 $ - Revision: $Revision: 1.31.2.2 $

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
#include <MeMemory.h>
#include <McdSphere.h>
#include "McdCheck.h"
#include <McdGeometryInstance.h>
#include <McdMessage.h>
#include "lsTransform.h"
#include <MeMath.h>
#include <MeDebugDraw.h>

MCD_IMPLEMENT_GEOMETRY_TYPE( McdSphere, "McdSphere", Sphere );

/**
Create a sphere of radius @param inRadius
*/
McdSphereID MEAPI
McdSphereCreate(McdFramework *frame, MeReal inRadius)
{
    McdSphereID s;
    
    s = (McdSphereID)
        MeMemoryAPI.createAligned(sizeof(McdSphere),MeALIGNTO);

    if (s != 0)
    {
        McdGeometryInit((McdGeometryID)s, frame, kMcdGeometryTypeSphere);
        McdSphereSetRadius(s,inRadius);
    }
    
    return s;
}

/*
Register the McdSphere geometry type with the Mcd system.
McdSphere is often registered implicitly via
McdPrimitivesRegisterTypes() or McdSphereBoxPlaneRegisterTypes(),
in which case it is an error to call McdBoxRegisterType() explicitly as
well.
*/

/* type-specific functions */

/**
Set a sphere's radius to the value @param inRadius
*/
void MEAPI
McdSphereSetRadius( McdSphereID g, MeReal inRadius)
{
    McdSphere* s = (McdSphere*)g;
    
    s->mRadius = inRadius;
}

/**
Return a sphere's radius
*/
MeReal MEAPI
McdSphereGetRadius( McdSphereID g )
{
    return ((McdSphere*)g)->mRadius;
}


/* polymorphic functions */

void MEAPI
McdSphereDestroy( McdSphereID g)
{
    MCD_CHECKGEOMETRY(g, "McdSphereDestroy");
    McdGeometryDeinit(g);
}

void MEAPI
McdSphereUpdateAABB(McdGeometryInstanceID ins, MeMatrix4Ptr finalTM, MeBool tight)
{
    
    MeVector3 r;
    McdSphere* s = (McdSphere*)McdGeometryInstanceGetGeometry(ins);
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);

    MeVector3Set(r, s->mRadius, s->mRadius, s->mRadius);
    MeVector3Copy(ins->min,(MeVector3Ptr)(tm[3]));
    MeVector3Copy(ins->max,(MeVector3Ptr)(tm[3]));

    if(finalTM)
    {
        MeVector3Min(ins->min,ins->min,(MeVector3Ptr)(finalTM[3]));
        MeVector3Max(ins->max,ins->max,(MeVector3Ptr)(finalTM[3]));
    }

    MeVector3Subtract(ins->min,ins->min,r);
    MeVector3Add(ins->max,ins->max,r);
}

void MEAPI
McdSphereGetBSphere( McdSphereID g, MeVector3 center, MeReal *radius )
{
    McdSphere* s = (McdSphere*)g;
    
    center[0] = center[1] = center[2] = (MeReal)(0.);
    *radius = s->mRadius;
}

void MEAPI
McdSphereMaximumPoint(McdGeometryInstanceID ins,
                      MeReal * const inDir, MeReal * const outPoint)
{
    McdSphere* s = (McdSphere*)McdGeometryInstanceGetGeometry(ins);    
    lsTransform *tm = (lsTransform*)McdGeometryInstanceGetTransformPtr(ins);

    lsVec3 &n = *(lsVec3*)inDir;
    lsVec3 &p = *(lsVec3*)outPoint;
    p = tm->t() + s->mRadius*n;
}

/**
Unit mass assumed.  The inertia for a sphere about it's CM
can be shown to be I = 2/5 MR^2 through simple integration.
Inertia about the CM leaves zero in the cross terms in the
mass-matrix.
*/
MeI16 MEAPI
McdSphereGetMassProperties( McdSphereID g,
                           MeMatrix4 relTM,
                           MeMatrix3 m,
                           MeReal *volume)
{
    McdSphere* s = (McdSphere*)g;
    MeReal Sph_coef = (MeReal)(2.0 / 5.0);
    MeReal mass = 1;
    
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            m[i][j] = 0.0;
        
        m[0][0] = Sph_coef * s->mRadius * s->mRadius * mass;
        m[1][1] = Sph_coef * s->mRadius * s->mRadius * mass;
        m[2][2] = Sph_coef * s->mRadius * s->mRadius * mass;
        
        MeMatrix4TMMakeIdentity( relTM );
        
        *volume = (MeReal)(4.0/3.0) * ME_PI * s->mRadius * s->mRadius * s->mRadius;
        
        return 0;
}



/**
Draw a sphere for debug purposes. Assumes MeDebugDraw line function has been set up
*/

#define HALFSQRT2 ((MeReal)0.7071)

void MEAPI McdSphereDebugDraw(const McdGeometryID geom, const MeMatrix4 tm, const MeReal colour[3])
{
    static const MeVector3 sphereDraw[24][2] = 
    {
        {{0, 0, -1},            {0, HALFSQRT2, -HALFSQRT2}},
        {{0, HALFSQRT2, -HALFSQRT2},  {0, 1, 0}},
        {{0, 1, 0},             {0, HALFSQRT2, HALFSQRT2}},
        {{0, HALFSQRT2, HALFSQRT2},   {0, 0, 1}},
        {{0, 0, 1},             {0, -HALFSQRT2, HALFSQRT2}},
        {{0, -HALFSQRT2, HALFSQRT2},  {0, -1, 0}},
        {{0, -1, 0},            {0, -HALFSQRT2, -HALFSQRT2}},
        {{0, -HALFSQRT2, -HALFSQRT2}, {0, 0, -1}},
        
        {{0, 0, -1},            {HALFSQRT2, 0, -HALFSQRT2}},
        {{HALFSQRT2, 0, -HALFSQRT2},  {1, 0, 0}},
        {{1, 0, 0},             {HALFSQRT2, 0, HALFSQRT2}},
        {{HALFSQRT2, 0, HALFSQRT2},   {0, 0, 1}},
        {{0, 0, 1},             {-HALFSQRT2, 0, HALFSQRT2}},
        {{-HALFSQRT2, 0, HALFSQRT2},  {-1, 0, 0}},
        {{-1, 0, 0},            {-HALFSQRT2, 0, -HALFSQRT2}},
        {{-HALFSQRT2, 0, -HALFSQRT2}, {0, 0, -1}},
        
        {{0, -1, 0},            {HALFSQRT2, -HALFSQRT2, 0}},
        {{HALFSQRT2, -HALFSQRT2, 0},  {1, 0, 0}},
        {{1, 0, 0},             {HALFSQRT2, HALFSQRT2, 0}},
        {{HALFSQRT2, HALFSQRT2, 0},   {0, 1, 0}},
        {{0, 1, 0},             {-HALFSQRT2, HALFSQRT2, 0}},
        {{-HALFSQRT2, HALFSQRT2, 0},  {-1, 0, 0}},
        {{-1, 0, 0},            {-HALFSQRT2, -HALFSQRT2, 0}},
        {{-HALFSQRT2, -HALFSQRT2, 0}, {0, -1, 0}}
    };

    if(McdGeometryGetTypeId(geom) != kMcdGeometryTypeSphere)
        return;
    
    McdSphereID sphere = (McdSphereID)geom;
    MeReal radius = McdSphereGetRadius(sphere);
    int i;

    for(i=0; i<24; i++)
    {
        MeVector3 lv, wv1, wv2;

        MeVector3MultiplyScalar(lv, sphereDraw[i][0],radius);
        MeMatrix4TMTransform(wv1, tm, lv);

        MeVector3MultiplyScalar(lv, sphereDraw[i][1],radius);
        MeMatrix4TMTransform(wv2, tm, lv);

        MeDebugDrawAPI.line(wv1, wv2, colour[1],colour[2],colour[3]);
    }

}
