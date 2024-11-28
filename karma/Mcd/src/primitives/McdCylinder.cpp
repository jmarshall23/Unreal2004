/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/17 13:44:35 $ - Revision: $Revision: 1.37.2.2 $

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

/**
  @file McdCylinder.h
*/

#include <McdCylinder.h>
#include <math.h>
#include <lsTransform.h>
#include <McdCheck.h>
#include <MeMath.h>
#include <McdGeometryInstance.h>
#include <MeDebugDraw.h>
#include <MeMemory.h>

#define PARALLEL_THRESH_EPS ((MeReal)0.0001)

MCD_IMPLEMENT_GEOMETRY_TYPE( McdCylinder, "McdCylinder", Cylinder );

/* type-specific functions */


/**
    Create a new cylinder geometry.
    @param r : radius
    @param h : height

    The axis of the cylinder is along the z-axis of its local coordinate
    system.
*/

McdCylinderID MEAPI
McdCylinderCreate(McdFramework *frame,MeReal inRadius, MeReal inHeight)
{
    McdCylinderID c;

    c = (McdCylinderID)
        MeMemoryAPI.createAligned(sizeof(McdCylinder),MeALIGNTO);

    if (c != 0)
    {
        McdGeometryInit((McdGeometryID)c, frame, kMcdGeometryTypeCylinder);
        McdCylinderSetGeometricalParameters(c,inRadius,inHeight);
    }

    return c;
}

/**
    Return the cylinder's radius
    @param r : radius

*/
extern "C"
MeReal MEAPI
McdCylinderGetRadius( McdCylinderID g )
{
    return ((McdCylinder*)g)->mR;
}

/**
    Return the cylinder's height
    @param h : height

*/
extern "C"
MeReal MEAPI
McdCylinderGetHeight( McdCylinderID g )
{
    return 2 * ((McdCylinder*)g)->mRz;
}

extern "C"
MeReal MEAPI
McdCylinderGetHalfHeight( McdCylinderID g )
{
    return ((McdCylinder*)g)->mRz;
}

/**
    Set the cylinder's radius
    @param r : radius
*/
extern "C"
void MEAPI
McdCylinderSetRadius( McdCylinderID g, MeReal r )
{
  McdCylinder *c = (McdCylinder*)g;

  c->mR = r;
  c->mSphereRadius = MeSqrt(c->mR * c->mR + c->mRz * c->mRz );
}

/**
    Set the cylinder's height
    @param h : height
*/
extern "C"
void MEAPI
McdCylinderSetHeight( McdCylinderID g, MeReal h )
{
  McdCylinder *c = (McdCylinder*)g;

  c->mRz = (MeReal)0.5 * h;
  c->mSphereRadius = MeSqrt(c->mR * c->mR + c->mRz * c->mRz );
}

/**
    Set the cylinder's height and radius
    @param inRadius : radius
    @param inHeight : height
*/
void MEAPI
McdCylinderSetGeometricalParameters( McdGeometry* g,
             MeReal inRadius,
             MeReal inHeight)
{
  McdCylinder *c = (McdCylinder*)g;

  c->mR = inRadius;
  c->mRz = (MeReal)0.5 * inHeight;
  c->mSphereRadius = MeSqrt(c->mR * c->mR + c->mRz * c->mRz );
}


/* polymorphic functions */

void MEAPI
McdCylinderDestroy( McdGeometry* g)
{
    MCD_CHECKGEOMETRY(g, "McdCylinderDestroy");
    McdGeometryDeinit(g);
}

// Yuk. Clean me up one day!

void MEAPI
McdCylinderUpdateAABB(McdGeometryInstanceID ins, MeMatrix4Ptr finalTM, MeBool tight)
{
    McdCylinder *c = (McdCylinder*)McdGeometryInstanceGetGeometry(ins);
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);
    
    MeVector3 rFit;
    const MeReal Zx = tm[2][0];
    const MeReal Zy = tm[2][1];
    const MeReal Zz = tm[2][2];
    
    const MeReal s2x = (MeReal)(1.0)-Zx*Zx;
    const MeReal s2y = (MeReal)(1.0)-Zy*Zy;
    const MeReal s2z = (MeReal)(1.0)-Zz*Zz;

    const MeReal sx = (s2x>(MeReal)(0.0))?MeSqrt(s2x):(MeReal)(0.0);
    const MeReal sy = (s2y>(MeReal)(0.0))?MeSqrt(s2y):(MeReal)(0.0);
    const MeReal sz = (s2z>(MeReal)(0.0))?MeSqrt(s2z):(MeReal)(0.0);

    rFit[0] = c->mR*sx + c->mRz * MeFabs(Zx);
    rFit[1] = c->mR*sy + c->mRz * MeFabs(Zy);
    rFit[2] = c->mR*sz + c->mRz * MeFabs(Zz);
    
    MeVector3Subtract(ins->min,(MeVector3Ptr)(tm[3]),rFit);
    MeVector3Add(ins->max,(MeVector3Ptr)(tm[3]),rFit);

    if(finalTM)
    {
        MeVector3 min,max;
        const MeReal Zx = finalTM[2][0];
        const MeReal Zy = finalTM[2][1];
        const MeReal Zz = finalTM[2][2];
    
        const MeReal s2x = (MeReal)(1.0)-Zx*Zx;
        const MeReal s2y = (MeReal)(1.0)-Zy*Zy;
        const MeReal s2z = (MeReal)(1.0)-Zz*Zz;
        
        const MeReal sx = (s2x>(MeReal)(0.0))?MeSqrt(s2x):(MeReal)(0.0);
        const MeReal sy = (s2y>(MeReal)(0.0))?MeSqrt(s2y):(MeReal)(0.0);
        const MeReal sz = (s2z>(MeReal)(0.0))?MeSqrt(s2z):(MeReal)(0.0);
    
        rFit[0] = c->mR*sx + c->mRz * MeFabs(Zx);
        rFit[1] = c->mR*sy + c->mRz * MeFabs(Zy);
        rFit[2] = c->mR*sz + c->mRz * MeFabs(Zz);
    
        MeVector3Subtract(min,(MeVector3Ptr)(finalTM[3]),rFit);
        MeVector3Add(max,(MeVector3Ptr)(finalTM[3]),rFit);
        MeVector3Min(ins->min,ins->min,min);
        MeVector3Max(ins->max,ins->max,max);
    }
}

void MEAPI
McdCylinderGetXYAABB( McdGeometry* g, MeMatrix4 _tm, MeReal bounds[4])
{
    McdCylinder *c = (McdCylinder*)g;
    lsTransform *tm = (lsTransform*)_tm;
    
    MeReal rFit[3];
    const MeReal Zx = tm->axis(2)[0];
    const MeReal Zy = tm->axis(2)[1];
    
    rFit[0] = c->mR*MeSqrt((MeReal)(1.0)-Zx*Zx)+c->mRz*MeFabs(Zx);
    rFit[1] = c->mR*MeSqrt((MeReal)(1.0)-Zy*Zy)+c->mRz*MeFabs(Zy);
    
    bounds[0] = tm->t()[0]-rFit[0];
    bounds[1] = tm->t()[0]+rFit[0];
    bounds[2] = tm->t()[1]-rFit[1];
    bounds[3] = tm->t()[1]+rFit[1];
}

void MEAPI
McdCylinderGetBSphere( McdGeometry* g, MeVector3 center, MeReal *radius )
{
    McdCylinder *c = (McdCylinder*)g;
    
    center[0] = center[1] = center[2] = (MeReal)(0.);
    *radius = c->mSphereRadius;
}

MeReal MEAPI
McdCylinderGetBSphereRadius( McdGeometryID g )
{
    return ((McdCylinder*)g) -> mSphereRadius;
}

void MEAPI
McdCylinderMaximumPoint( McdGeometryInstanceID ins,
                        MeReal * const inDir, MeReal * const outPoint)
{
    McdCylinder *c = (McdCylinder*)McdGeometryInstanceGetGeometry(ins);
    
    lsTransform *tm = (lsTransform*)McdGeometryInstanceGetTransformPtr(ins);
    lsVec3 n = *(lsVec3*)inDir;
    lsVec3 &p = *(lsVec3*)outPoint;
    p = tm->t();

    const MeReal nZ = tm->axis(2).dot(n);
    if (MeFabs(nZ) > PARALLEL_THRESH_EPS)
    {
        if (nZ > 0)
        {
            p += c->mRz*tm->axis(2);
        } else
        {
            p -= c->mRz*tm->axis(2);
        }
        n -= nZ*tm->axis(2);
        MeReal n2 = n.square_norm();
        if (n2 > PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
        {
            n *= MeRecipSqrt(n2);
        }
    }

    p += c->mR*n;
}


/*--------------------------------------------------------------------*/
// inDir and outPoint are both in g's local coordinate system
void MEAPI
McdCylinderMaximumPointLocal( McdGeometry *g,
                             MeReal * const inDir, MeReal * const outPoint)
{
    McdCylinder *c = (McdCylinder*)g;

    lsVec3 n = *(lsVec3*)inDir;
    lsVec3 &p = *(lsVec3*)outPoint;

    if (MeFabs(n[2]) > PARALLEL_THRESH_EPS)
    {
        if (n[2] > PARALLEL_THRESH_EPS)
        {
            p[2] += c->mRz;
        } else
        {
            p[2] -= c->mRz;
        }
        MeReal n2 = n[0]*n[0] + n[1]*n[1];
        if (n2 > PARALLEL_THRESH_EPS*PARALLEL_THRESH_EPS)
        {
            MeReal invN = MeRecipSqrt(n2);
            n[0] *= invN;
            n[1] *= invN;
        }
    }

    p[0] += c->mR*n[0];
    p[1] += c->mR*n[1];
}

/**
Unit mass assumed.  The inertia for a cylinder is calculated with the
z-axis through the centers of the ends.  r is the radius of
the cylinder, l is the length.
Inertia about the CM leaves zero in the cross terms in the
mass-matrix.
Ixx = Iyy = 1/4*M*r^2 + 1/12*M*l^2
Izz = 1/2*M*r^2
*/
MeI16 MEAPI
McdCylinderGetMassProperties( McdGeometry *g,
                             MeMatrix4 relTM,
                             MeMatrix3 m,
                             MeReal *volume)
{
    McdCylinder *c = (McdCylinder*)g;
    MeReal r = c->mR;
    MeReal l = 2 * c->mRz;
    MeReal mass = 1.0;
    
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            m[i][j] = 0.0;
        
    m[0][0] = (MeReal) (((1/4.0) * mass * r * r) + ((1/12.0) * mass * l * l));
    m[1][1] = m[0][0];
    m[2][2] = (MeReal) ((1/2.0) * mass * r * r);
    
    MeMatrix4TMMakeIdentity( relTM );
    
    *volume = ME_PI * r * r * l;
    
    return 0;
}

#define HALFSQRT2 ((MeReal)0.7071)




/**
Draw a cylinder for debug purposes. Assumes MeDebugDraw line function has been set up
*/



void MEAPI McdCylinderDebugDraw(const McdGeometryID geom, const MeMatrix4 tm, const MeReal colour[3])
{
    static const MeVector3 cylDraw[20][2] =
    {
        {{0, -1, -1},            {HALFSQRT2, -HALFSQRT2, -1}},
        {{HALFSQRT2, -HALFSQRT2, -1},  {1, 0, -1}},
        {{1, 0, -1},             {HALFSQRT2, HALFSQRT2, -1}},
        {{HALFSQRT2, HALFSQRT2, -1},   {0, 1, -1}},
        {{0, 1, -1},             {-HALFSQRT2, HALFSQRT2, -1}},
        {{-HALFSQRT2, HALFSQRT2, -1},  {-1, 0, -1}},
        {{-1, 0, -1},            {-HALFSQRT2, -HALFSQRT2, -1}},
        {{-HALFSQRT2, -HALFSQRT2, -1}, {0, -1, -1}},
        
        {{0, -1, 1},            {HALFSQRT2, -HALFSQRT2, 1}},
        {{HALFSQRT2, -HALFSQRT2, 1},  {1, 0, 1}},
        {{1, 0, 1},             {HALFSQRT2, HALFSQRT2, 1}},
        {{HALFSQRT2, HALFSQRT2, 1},   {0, 1, 1}},
        {{0, 1, 1},             {-HALFSQRT2, HALFSQRT2, 1}},
        {{-HALFSQRT2, HALFSQRT2, 1},  {-1, 0, 1}},
        {{-1, 0, 1},            {-HALFSQRT2, -HALFSQRT2, 1}},
        {{-HALFSQRT2, -HALFSQRT2, 1}, {0, -1, 1}},
        
        {{0, -1, -1},   {0, -1, 1}},
        {{0, 1, -1},    {0, 1, 1}},
        {{1, 0, -1},    {1, 0, 1}},
        {{-1, 0, -1},   {-1, 0, 1}}
    };

    if(McdGeometryGetTypeId(geom) != kMcdGeometryTypeCylinder)
        return;

    McdCylinderID cyl = (McdCylinderID)geom;
    int i;
    MeVector3 rads;
    MeVector3 wv1,wv2,lv;

    rads[0] = rads[1] = McdCylinderGetRadius(cyl);
    rads[2] = McdCylinderGetHeight(cyl)/2;

    for(i=0; i<20; i++)
    {
        MeVector3MultiplyElements(lv,cylDraw[i][0],rads);
        MeMatrix4TMTransform(wv1, tm, lv);

        MeVector3MultiplyElements(lv,cylDraw[i][1],rads);
        MeMatrix4TMTransform(wv2, tm, lv);

        MeDebugDrawAPI.line(wv1,wv2,colour[1],colour[2],colour[3]);        
    }
}

