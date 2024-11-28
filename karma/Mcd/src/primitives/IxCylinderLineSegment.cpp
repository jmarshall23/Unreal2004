/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:57 $ - Revision: $Revision: 1.33.2.2 $

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

#include "McdCheck.h"
#include <McdCylinder.h>
#include <McdInteractionTable.h>
#include <McdPrimitives.h>
#include <McdModel.h>
#include <MeMath.h>
#include <lsTransform.h>

/*--------------------------------------------------------------------------*
 inOrig and inDest are in world c.s.                                        *
 ---------------------------------------------------------------------------*/



int MEAPI IxCylinderLineSegment( const McdModelID model,
                                MeReal* const inOrig, MeReal* const inDest,
                                McdLineSegIntersectResult* info )
{
    lsTransform *tm = (lsTransform *)McdModelGetTransformPtr(model);
    McdCylinderID inCylinder = (McdCylinderID)McdModelGetGeometry(model);
    
    MCD_CHECK_ASSERT_(inOrig != NULL && inDest != NULL && inCylinder != NULL, "IxCylinderLineSegment");
    MCD_CHECK_ASSERT_( McdGeometryGetTypeId(inCylinder) == kMcdGeometryTypeCylinder, "IxCylinderLineSegment");
    
    MeReal r  = McdCylinderGetRadius(inCylinder);
    MeReal hh = McdCylinderGetHalfHeight(inCylinder);
    
    if ( hh < ME_SMALL_EPSILON) return 0;
    
    lsVec3 p0, p1;
    tm->inverseTransform( *((lsVec3*)inOrig), &p0);
    tm->inverseTransform( *((lsVec3*)inDest), &p1);
    
    lsVec3 dir = p1 - p0;    
    MeReal s1, s = MEINFINITY, pt;
    lsVec3 normal(0,0,0);

    // not necessary variables:
    const MeReal dx = dir[0];
    const MeReal dy = dir[1];
    const MeReal dz = dir[2];
    const MeReal x0 = p0[0];
    const MeReal y0 = p0[1];
    const MeReal z0 = p0[2];
    
    // check for start inside cylinder

    if(x0*x0+y0*y0<=r*r && z0<=hh && z0>= -hh)
    {
        s = 0;
        normal[2] = 1;
    }
        
    else 
    {
        // ends of cylinder

        if ( MeFabs(dz) > ME_SMALL_EPSILON ) 
        {
            // top face
            s1 = (hh-z0)/dz;          
            if(s1 >= 0 && s1 < s)
            {
                pt = (x0+s1*dx) * (x0+s1*dx) + (y0+s1*dy)*(y0+s1*dy);        
                if(pt <= r*r)
                {
                    s = s1;
                    normal[2] = 1;
                }
            }
            
            // bottom face
            s1 = (-hh-z0)/dz;
            if(s1 >= 0 && s1 < s)
            {
                pt = (x0+s1*dx) * (x0+s1*dx) + (y0+s1*dy)*(y0+s1*dy);
                if(pt <= r*r)
                {
                    s = s1;
                    normal[2] = -1;
                }
            }
        }        
        
        // wall of cylinder
        
        MeReal A = dx*dx + dy*dy;
        MeReal B = 2*(x0*dx+y0*dy);
        MeReal C = (x0*x0+y0*y0-r*r);
        
        MeReal root = B*B - 4*A*C;
        
        if (root >= 0 && MeFabs(A) > ME_SMALL_EPSILON*ME_SMALL_EPSILON) {
            
            MeReal z1;
            root = MeSqrt(root);
            
            s1 = (-B+root)/(2*A);
            if ( s1 >= 0 && s1 < s) 
            {
                z1 = z0 + s1*dz;
                if (z1 <= hh && z1 >= -hh)  
                {
                    s = s1;
                    normal[2] = 0;
                }
            }
            
            s1 = (-B-root)/(2*A);
            if ( s1 >= 0 && s1 < s) 
            {
                z1 = z0 + s1*dz;
                if (z1 <= hh && z1 >= -hh) 
                {   
                    s = s1;
                    normal[2] = 0;
                }
            }
        }
    }

    if (s < 1) {
        lsVec3 ip = p0 + dir * s;
        tm->transform( ip, (lsVec3*)(info->position));
        info->distance = dir.norm()*s;
        if(normal[2] == 0) 
        {                                   // it's a wall
            normal[0] = ip[0];
            normal[1] = ip[1];
            normal.normalize();
        }
        tm->transformWithoutTranslate(normal,(lsVec3 *)(info->normal));
        return 1;
    }

    return 0;

}

MCD_IMPLEMENT_LINESEG_REGISTRATION(Cylinder);

