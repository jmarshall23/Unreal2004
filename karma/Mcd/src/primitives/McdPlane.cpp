/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/17 13:44:35 $ - Revision: $Revision: 1.29.2.3 $

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

#include <McdPlane.h>
#include "McdCheck.h"
#include <McdMessage.h>
#include <math.h>
#include "lsTransform.h"
#include <MeMath.h>
#include <McdGeometryInstance.h>
#include <MeMessage.h>
#include <MeMemory.h>

/*
    Register the McdPlane geometry type with the Mcd system.
    McdPlane is often registered implicitly via
    McdPrimitivesRegisterTypes() or McdSphereBoxPlaneRegisterTypes().
    in which case it is an error to call McdBoxRegisterType() explicitly as
    well.
*/

MCD_IMPLEMENT_GEOMETRY_TYPE( McdPlane , "McdPlane", Plane );

/* type-specific functions */

/**
    Create new Plane geometry.
    The plane is the xy-plane in its coordinate system.
*/

McdPlaneID MEAPI
McdPlaneCreate(McdFramework *frame)
{
    McdPlaneID p;
    
    p = (McdPlaneID)
        MeMemoryAPI.createAligned(sizeof(McdPlane),MeALIGNTO);

    if (p != 0)
        McdGeometryInit((McdGeometryID)p, frame, kMcdGeometryTypePlane);
    
    return p;
}



/* polymorphic functions */

void MEAPI
McdPlaneDestroy( McdGeometryID g )
{
    MCD_CHECKGEOMETRY(g, "McdPlaneDestroy");
    McdGeometryDeinit(g);
}


void MEAPI
McdPlaneUpdateAABB(McdGeometryInstanceID ins, MeMatrix4Ptr finalTM, MeBool tight)
{
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);
    const MeReal cosTolAngle = (MeReal)(0.9999995);
    
    MeVector3Set(ins->min,-MEINFINITY,-MEINFINITY,-MEINFINITY);
    MeVector3Set(ins->max,MEINFINITY,MEINFINITY,MEINFINITY);
    
    MeVector3Ptr n = (MeVector3Ptr)(tm[2]);  // z-axis
    MeVector3Ptr t = (MeVector3Ptr)(tm[3]);  // z-axis
    if (n[0] > cosTolAngle) 
        ins->max[0] = t[0];
    else if (n[0] < -cosTolAngle) 
        ins->min[0] = t[0];
    else if (n[1] > cosTolAngle) 
        ins->max[1] = t[1];
    else if (n[1] < -cosTolAngle)
        ins->min[1] = t[1];
    else if (n[2] > cosTolAngle)
        ins->max[2] = t[2];
    else if (n[2] < -cosTolAngle)
        ins->min[2] = t[2];
    
    if(finalTM)
    {
        MeVector3 min,max;
        MeVector3Set(min,-MEINFINITY,-MEINFINITY,-MEINFINITY);
        MeVector3Set(max,MEINFINITY,MEINFINITY,MEINFINITY);
        
        MeVector3Ptr n = (MeVector3Ptr)(finalTM[2]);  // z-axis
        MeVector3Ptr t = (MeVector3Ptr)(finalTM[3]);  // z-axis
        if (n[0] > cosTolAngle) 
            max[0] = t[0];
        else if (n[0] < -cosTolAngle) 
            min[0] = t[0];
        else if (n[1] > cosTolAngle) 
            max[1] = t[1];
        else if (n[1] < -cosTolAngle)
            min[1] = t[1];
        else if (n[2] > cosTolAngle)
            max[2] = t[2];
        else if (n[2] < -cosTolAngle)
            min[2] = t[2];
        MeVector3Min(ins->min,ins->min,min);
        MeVector3Max(ins->max,ins->max,max);
    }
}

void MEAPI
McdPlaneGetBSphere( McdGeometry* g, MeVector3 center, MeReal *radius )
{
    center[0] = center[1] = center[2] = (MeReal)(0.);
    *radius = MEINFINITY;
}

void MEAPI
McdPlaneMaximumPoint( McdGeometryInstanceID ins,
                     MeReal * const inDir, MeReal * const outPoint)
{
    lsTransform *tm = (lsTransform*)McdGeometryInstanceGetTransformPtr(ins);
    lsVec3 &n = *(lsVec3*)inDir;
    lsVec3 &p = *(lsVec3*)outPoint;
    const MeReal cosTolAngle = (MeReal)(0.9999995);
    if (tm->axis(2).dot(n) >= cosTolAngle) {
        p = tm->t();
    } else {
        p = MEINFINITY*n;
    }
}

/**
It's not totally clear that a plane should have mass properties.
Planes don't really adhere to manipulatable rules like the other
geom types
*/
MeI16 MEAPI
McdPlaneGetMassProperties( McdGeometry *g,
                          MeMatrix4 relTM,
                          MeMatrix3 m,
                          MeReal *volume )
{
    m[0][0] = 1;
    m[1][1] = 1;
    m[2][2] = 1;
    
    m[0][1] = 0;
    m[0][2] = 0;
    m[1][0] = 0;
    m[1][2] = 0;
    m[2][0] = 0;
    m[2][1] = 0;
    
    MeMatrix4TMMakeIdentity( relTM );
    return 0;
}

void MEAPI
McdPlaneGetNormal( const McdGeometry *g, const MeMatrix4 _tm,
                  MeVector3 _normal )
{
    lsTransform *tm = (lsTransform*)_tm;
    lsVec3 *normal = (lsVec3*) _normal;
    if (tm) {
        *normal = tm->axis(2);
    } else {
        normal->setValue(0,0,1);
    }
}

MeReal MEAPI
McdPlaneGetDistanceToPoint( const McdGeometry *g, const MeMatrix4 _tm,
                           const MeVector3 point)
{
    lsVec3 normal;
    McdPlaneGetNormal(g,_tm,(MeReal*)&normal);
    lsVec3 disp; disp.setValue(point);
    if (_tm) disp -= ((lsTransform*)_tm)->t();
    return disp.dot(normal);
}


void MEAPI McdPlaneDebugDraw(const McdGeometryID geom, const MeMatrix4 tm, const MeReal colour[3])
{
    static int warn = 1;
    if(warn)
        MeWarning(0, "McdPlaneDebugDraw: Don't know how to draw a plane!");
    warn = 0;
}
