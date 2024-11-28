/* -*- mode: C; -*- */

/*
    Copyright (c) 1997-2002 MathEngine PLC

    $Name: t-stevet-RWSpre-030110 $
  
    $Date: 2002/04/04 15:29:08 $ - $Revision: 1.1.2.5 $
    
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

  ========================================================================

  Sample User Geometry: Cone

  This is a sample implementation of a user geometry.  The basic 
  operations that every geometry must support are:

      - create
      - destroy
      - AABB, get the bounding box of a geometry instance
      - MassProperties, compute volume, moments of inertia, etc
      - type registration, and type id

  In addition the user geometry may also implement:

      - BSphere, get the bounding sphere of a geometry
      - MaximumPoint, farthest point in given direction
      - DebugDraw, line drawing for debug purposes
      - Mutators, to change the radius, height, etc
      - collision detection with sphere, box, plane, etc
      - line segment query
      - safe time intersection

  In this example, we have not implemented safe time.
  For the collision detection we just call the GJK function.  This is
  not a fast as a special purpose function.  Also, since GJK does not
  recognize the Cone geometry, the contact generation is minimal and
  that is why the cone is somewhat wobbly.

  ========================================================================
*/

#include "ConeGeometry.h"
#include <McdModel.h>
#include <MeDebugDraw.h>
#include <McdGjk.h>

MCD_IMPLEMENT_GEOMETRY_TYPE(Cone, "Cone", Cone);


/****************************************************************************
  This is a helper function to compute the bounding sphere of the cone
  whenever the height or radius of the cone is changed.

  If the height < radius, then bsRadius = radius.  Otherwise,
  according to Heron's formula, bsRadius = (r^2 + h^2) / 2*h.
*/
static void ConeUpdateBoundingSphere(Cone *cone)
{
    cone->radius = MeFabs(cone->radius);
    cone->height = MeFabs(cone->height);

    if (cone->height < cone->radius || cone->height == 0)
    {
        cone->bsRadius = cone->radius;
        cone->bsZ = -cone->height / 4;
    }
    else
    {
        cone->bsRadius = cone->radius*cone->radius;
        cone->bsRadius += cone->height*cone->height;
        cone->bsRadius /= 2*cone->height;
        cone->bsZ = -cone->bsRadius + cone->height * 3/4;
    }
}

/****************************************************************************
  Create a Cone.
*/
McdGeometry* MEAPI
ConeCreate(McdFramework *frame, MeReal radius, MeReal height)
{
    Cone *cone;

    cone = (Cone *)MeMemoryAPI.createAligned(sizeof(Cone), MeALIGNTO);
    if (!cone) 
        return 0;

    McdGeometryInit(&cone->m_g, frame, kMcdGeometryTypeCone);

    cone->radius = radius;
    cone->height = height;

    ConeUpdateBoundingSphere(cone);

    return &cone->m_g;
}

  
/****************************************************************************
  This sets the height of the cone.
*/
void MEAPI ConeSetHeight(Cone *cone, MeReal height)
{
    cone->height = height;
    ConeUpdateBoundingSphere(cone);
}

/****************************************************************************
  This sets the radius of the cone.
*/
void MEAPI ConeSetRadius(Cone *cone, MeReal radius)
{
    cone->radius = radius;
    ConeUpdateBoundingSphere(cone);
}

/****************************************************************************
  This frees any resources allocated by a cone, and calls the "parent"
  destructor, McdGeometryDeinit.
*/
void MEAPI
ConeDestroy(McdGeometry* g)
{
    McdGeometryDeinit(g);
}

/****************************************************************************
  Return the center and radius of the smallest sphere containing the cone.
  The center is returned in local reference frame.
*/
void MEAPI
ConeGetBSphere(McdGeometry* g, MeVector3 center, MeReal *radius )
{
    Cone *cone = (Cone*)g;

    MeVector3Set(center, 0, 0, cone->bsZ);
    *radius = cone->radius;
}

/****************************************************************************
  This is a helper function for ConeMaximumPoint
*/
void MEAPI
ConeMaximumPointLocal(Cone *cone, MeReal* inDir, MeReal* outPoint)
{
    MeReal r = MeSqrt(inDir[0]*inDir[0] + inDir[1]*inDir[1]);

    if (r*cone->radius <= inDir[2]*cone->height)
    {
        /*  Return the apex  */
        MeVector3Set(outPoint, 0, 0, cone->height * 3/4);
        return;
    }

    if (r < ME_SMALL_EPSILON)
        r = 0;
    else
        r = cone->radius / r;

    /*  Return a point on the perimeter of the base  */

    MeVector3Set(outPoint, r*inDir[0], r*inDir[1], -cone->height/4 );
}

/****************************************************************************
  Return the farthest point (in world coordinates) on the cone in a 
  given direction.  The point returned will always be either the apex
  of the cone, or a point on the perimeter of the base (or the center of
  the base).
*/
void MEAPI
ConeMaximumPoint(McdGeometryInstanceID ins,
          MeReal * const inDir, MeReal * const outPoint)
{
    Cone *cone = (Cone*)McdGeometryInstanceGetGeometry(ins);
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);
    MeVector3 dir2, p2;

    MeMatrix4TMInverseRotate(dir2, tm, inDir);
    ConeMaximumPointLocal(cone, dir2, p2);
    MeMatrix4TMTransform(outPoint, tm, p2);
}


/****************************************************************************
  This is a helper function used by ConeUpdateAABB.
  This computes the bounding box of a cone at specified location and rotation.
  The "tight" flag indicates to use a high precision algorithm if available.

  Returns min1 and max1.
*/
static void
ConeGetMinMax(Cone *cone, MeMatrix4 tm, MeBool tight,
              MeVector3 min1, MeVector3 max1)
{
    MeVector3 t, u;
    int i;

    /*  If a tight fit is not required, just use the bounding sphere radius. */

    if (!tight)
    {
        MeVector3Set(t, cone->bsRadius, cone->bsRadius, cone->bsRadius + cone->bsZ);
        MeVector3Add(max1, tm[3], t);
        t[2] -= 2 * cone->bsZ;
        MeVector3Subtract(min1, tm[3], t);
        return;
    }

    /*  For the "tight" bounding box, call ConeMaximumPoint six times. */

    for (i = 0; i < 3; ++i)
    {
        MeVector3Set(u, tm[0][i], tm[1][i], tm[2][i]);
        ConeMaximumPointLocal(cone, u, t);
        MeMatrix4TMTransform(u, tm, t);
        max1[i] = u[i];

        MeVector3Set(u, -tm[0][i], -tm[1][i], -tm[2][i]);
        ConeMaximumPointLocal(cone, u, t);
        MeMatrix4TMTransform(u, tm, t);
        min1[i] = u[i];
    }
}

/****************************************************************************
  This updates the geometry axis-aligned bounding box.
  The "tight" flag indicates to use a high precision algorithm if available.
  The finalTM, if not null, indicates the new location and rotation of the 
  cone at the end of the time-step.  The finalTM is used for safeTime.
*/
void MEAPI
ConeUpdateAABB(McdGeometryInstanceID ins, MeMatrix4 finalTM, MeBool tight)
{
    Cone *cone = (Cone*)McdGeometryInstanceGetGeometry(ins);
    MeMatrix4Ptr tm = McdGeometryInstanceGetTransformPtr(ins);  

    ConeGetMinMax(cone, tm, tight, ins->min, ins->max);

    if (finalTM)
    {
        MeVector3 min2, max2;

        ConeGetMinMax(cone, finalTM, tight, min2, max2);
        MeVector3Min(ins->min, ins->min, min2);
        MeVector3Max(ins->max, ins->max, max2);
    }
}

/****************************************************************************
  Compute and return the volume and inertia tensor of the cone.
  The relTM can be used to specify the offset (in local coordinates)
  to the center of mass.  However, by definition, the center of mass of the
  cone is always at the local coordinates origin (one quarter of the height
  above the base). 

  always returns zero
*/
MeI16 MEAPI
ConeGetMassProperties(McdGeometry *g,
                      MeMatrix4 relTM,
                      MeMatrix3 m,
                      MeReal *volume)
{
    Cone *cone = (Cone*)g;
    MeReal r = cone->radius;
    MeReal h = cone->height;

    memset(m, 0, sizeof(MeMatrix3));
    m[0][0] = (4*r*r + h*h)*3/80;
    m[1][1] = (4*r*r + h*h)*3/80;
    m[2][2] = r*r*3/10;

    MeMatrix4TMMakeIdentity( relTM );

    *volume = ME_PI*r*r*h / 3;

    return 0;
}

/****************************************************************************
  This draws the cone for debug purposes.
  Assumes MeDebugDraw line function has been set up.
*/
void MEAPI ConeDebugDraw(const McdGeometryID geom, const MeMatrix4 tm, const MeReal colour[3])
{
    Cone *cone = (Cone *)geom;
    MeReal r = cone->radius;
    MeReal h = cone->height;
    MeVector3 p0, p1, p2, t;
    int i;

    /*  Get the apex, and the 11th point */
    MeVector3Set(t, 0, 0, h*3/4);
    MeMatrix4TMTransform(p0, tm, t);
    MeVector3Set(t, MeCos(ME_PI*11/6)*r, MeSin(ME_PI*11/6)*r, -h/4);
    MeMatrix4TMTransform(p1, tm, t);

    /*  Draw the cone as a pyramid with 12 sides */
    for(i = 0; i < 12; i++)
    {
        MeVector3Set(t, MeCos(ME_PI*i/6)*r, MeSin(ME_PI*i/6)*r, -h/4);
        MeMatrix4TMTransform(p2, tm, t);
        MeDebugDrawAPI.line(p0,p2,colour[1],colour[2],colour[3]);        
        MeDebugDrawAPI.line(p1,p2,colour[1],colour[2],colour[3]);        
        MeVector3Copy(p1, p2);
    }
}

/****************************************************************************
  Compute the intersection of a ray with a cone.
  This is only an approximation using the bounding sphere radius.
*/
int MEAPI ConeLineSegment(const McdModelID model,
                          MeReal* const inOrig, MeReal* const inDest,
                          McdLineSegIntersectResult *result)
{
    Cone *cone = (Cone *)McdModelGetGeometry(model);
    MeMatrix4Ptr tm = McdModelGetTransformPtr(model);
    MeVector3 a, b, c;
    MeReal ab, aa, r;

    MeVector3Subtract(a, inDest, inOrig);
    MeVector3Subtract(b, tm[3], inOrig);
    aa = MeVector3Dot(a, a);
    ab = MeVector3Dot(a, b);

    if (aa < ME_SMALL_EPSILON || ab < ME_SMALL_EPSILON)
        return 0;                   /*  RETURN - no collision */

    MeVector3MultiplyScalar(c, a, ab/aa);
    r = MeVector3Distance(c, b);

    if (r > cone->bsRadius)      /*  RETURN - no collision */
        return 0;

    MeVector3Add(result->position, c, inOrig);
    result->distance = MeVector3Normalize(c);
    MeVector3MultiplyScalar(result->normal, c, -1);
    return 1;
}


/****************************************************************************
  This registers the interaction functions (i.e. "nearfield collision
  detection") of Cone with primitives.

  GJK is used for all interactions.  This could be made more efficient
  with specific primitive-cone functions.
*/
MeBool MEAPI ConePrimitivesRegisterInteractions(McdFramework *frame)
{             
    int i;
    McdInteractions interactions;               
    
    interactions.helloFn = McdCacheHello;        
    interactions.goodbyeFn = McdCacheGoodbye;    
    interactions.intersectFn = McdGjkCgIntersect;
    interactions.safetimeFn = 0;
    interactions.cull = 0;
    interactions.warned = 0;

    for (i = 1; i <= kMcdGeometryTypeCone; ++i)
        McdFrameworkSetInteractions(frame, kMcdGeometryTypeCone, i, &interactions);

    McdFrameworkSetLineSegInteraction(frame,kMcdGeometryTypeCone, ConeLineSegment); 

    return 1;
}


