#ifndef _CONEGEOMETRY_H
#define _CONEGEOMETRY_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:08 $ - Revision: $Revision: 1.1.2.2 $

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
#include <McdGeometry.h>
#include <McdGeometryTypes.h>

/****************************************************************************
   Since this is the first user defined type, its typeID will be
   BuiltInTypes + 0.  The next user defined type would be 
   BuiltInTypes + 1, etc.
*/
enum { kMcdGeometryTypeCone = kMcdGeometryBuiltInTypes + 0 };


/****************************************************************************
  This is the data struct for a Cone user geometry.  As all geometries, the
  first member of the struct must be an "McdGeometry".  The axis of the cone
  is the z axis with the "top" of the cone in the positive z direction. 
  The center of the cone is defined to be at the center of mass, 1/4 of 
  the height above the base.  The bsRadius is the radius of the bounding 
  sphere.
*/

typedef struct Cone
{
  McdGeometry m_g;     /**< common geometry data */
  MeReal radius;       /**< cone base radius */
  MeReal height;       /**< height of cone */
  MeReal bsRadius;     /**< bounding sphere radius */
  MeReal bsZ;          /**< z coordinate of b.s. center */
} 
Cone;


/****************************************************************************
  Declare all the Cone functions.

  The MCD_DECLARE_GEOMETRY_TYPE(Cone) will declare 
        ConeGetTypeId
        ConeRegisterType
        ConeDestroy
        ConeUpdateAABB
        ConeGetBSphere
        ConeMaximumPoint
        ConeGetMassProperties
        ConeDebugDraw
*/
MCD_DECLARE_GEOMETRY_TYPE(Cone);

McdGeometry* MEAPI ConeCreate(McdFramework *frame, MeReal radius, MeReal height);

void MEAPI ConeSetHeight(Cone *cone, MeReal height);
void MEAPI ConeSetRadius(Cone *cone, MeReal radius);

MeBool MEAPI ConePrimitivesRegisterInteractions(McdFramework *frame);

#endif /* _CONEGEOMETRY_H */
