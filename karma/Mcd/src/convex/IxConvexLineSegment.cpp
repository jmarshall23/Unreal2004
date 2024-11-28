/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:54 $ - Revision: $Revision: 1.24.2.1 $

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

#include <McdGeometryTypes.h>
#include <McdConvexMesh.h>
#include <McdPrimitives.h>
#include <McdQHullTypes.h>
#include <McdInteractionTable.h>
#include <McdCheck.h>
#include <McdModel.h>
#include <lsTransform.h>

int MEAPI
IxConvexMeshLineSegment(const McdModelID model,
          MeReal* const inOrig, MeReal* const inDest,
          McdLineSegIntersectResult * info )
{
    McdConvexMeshID convex = (McdConvexMeshID)McdModelGetGeometry(model);
    MCD_CHECK_ASSERT_(inOrig != NULL && inDest != NULL && convex != NULL && info != NULL, "IxConvexMeshLineSegment");
    MCD_CHECK_ASSERT_(McdGeometryGetTypeId(convex) == kMcdGeometryTypeConvexMesh, "IxConvexMeshLineSegment");

    lsTransform *gtm = (lsTransform*)McdModelGetTransformPtr(model);

    const lsVec3 *v0 = (lsVec3*)inOrig;
    const lsVec3 *v1 = (lsVec3*)inDest;

    lsVec3 p0, p1, *normalOut = 0;

    // tranform two points into polyhedron's c.s.
    gtm->inverseTransform( *v0, &p0 );
    gtm->inverseTransform( *v1, &p1 );

    lsVec3 dir = p1 - p0;
    MeReal len = dir.normalize();
    MCD_CHECK_ASSERT_( len > 0, "IxConvexMeshLineSegment");

    const McdConvexHull *poly = McdConvexMeshGetPolyhedron(convex);

    MeReal tnear, tfar, t, vn, vd;
    int fnorm_num, bnorm_num;

    tnear = (MeReal) -1.0E20;
    tfar = len;

    /* Graphics Gems II*/
    for(int i = 0; i < poly->numFace; i++)
    {
      lsVec3 *normal = (lsVec3 *)poly->face[i].normal;
      lsVec3 *pointInPlane = (lsVec3 *)McdCnvFaceGetVertexPosition(poly, i, 0);

      vd = normal->dot(dir);
      vn = normal->dot(p0) - normal->dot(*pointInPlane);

      if ( tnear > tfar) return 0;

      if ( MeFabs(vd) < ME_SMALL_EPSILON ) {
        if ( vn > 0.0 ) return 0;
      } else {
        t = -vn / vd ;
        if ( vd < 0.0 ) {
          if ( t > tfar ) return 0;
          if ( t > tnear ) {
            fnorm_num = i ;
            tnear = t ;
          }
        } else {
          if ( t < 0.0 ) return 0 ;
          if ( t < tfar ) {
            bnorm_num = i ;
            tfar = t ;
          }
        }
      }
    }

    if ( tnear >= 0.0 ) {
      normalOut = (lsVec3 *)poly->face[fnorm_num].normal;
      t = tnear ;
    } else if ( tfar < len ) {
      normalOut = (lsVec3 *)poly->face[bnorm_num].normal;
      t = tfar ;
    } else {
      return 0;
    }

    (*v0+(t/len)*(*v1 - *v0)).getValue(info->position);
    info->distance = t; // t*len;
    MCD_CHECK_ASSERT_(normalOut != NULL, "IxConvexMeshLineSegment");
    lsVec3 normalW;
    gtm->transformWithoutTranslate( *normalOut, &normalW);
    normalW.getValue(info->normal);
    info->model = model;

    return 1;
}

MCD_IMPLEMENT_LINESEG_REGISTRATION(ConvexMesh);

