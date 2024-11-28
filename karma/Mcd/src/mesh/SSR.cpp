// -*-c++-*-
//=============================================================================
// File:        SSR.cpp 
// 
// Author:      Zhaoheng Liu
// 
//  $Id: SSR.cpp,v 1.10 2001/05/04 15:48:28 piercarl Exp $
// 
//=============================================================================
#include "SSR.h" 
#include "RectRectDist.h"  


// inPoint is expressed in the SSR's local coordinate system.
MeBool
McdSquaredDistancePointToRectangle( const CxSSR *inSSR, const lsVec3& inPoint, MeReal &outDist )
{
    MeReal x = inPoint[0];
    MeReal y = inPoint[1];
    MeReal z = inPoint[2];
    MeReal q, r;

    if ( x<=inSSR->mExtX && x>=-inSSR->mExtX && y<=inSSR->mExtY && y>=-inSSR->mExtY ) {
        outDist = z*z;
        return 0;
    }

    if ( x >= inSSR->mExtX ) {
        q = x - inSSR->mExtX;
    } else {
        q = x + inSSR->mExtX;
    }

    if ( y >= inSSR->mExtY ) 
        r = y - inSSR->mExtY;
    else
        r = y + inSSR->mExtY;

    if ( x <= inSSR->mExtX && x >= -inSSR->mExtX ) {
        outDist = (z*z + r*r);
        return 1;
    }
     
    if ( y <= inSSR->mExtY && y >= -inSSR->mExtY ) {            
        outDist = (z*z + q*q);
        return 1;
    } else {             
        outDist = (z*z + q*q + r*r);
        return 1;
    } 
}


//----------------------------------------------------------------------------
void 
McdBuildSSRFromOBB( CxTriangleMesh *mesh, BVDNode* node, int *triList, int numTris) 
{
  node->mSSR.mRadius = node->mOBB.mBox.ext[2];    
  node->mSSR.mExtX   = node->mOBB.mBox.ext[0];
  node->mSSR.mExtY   = node->mOBB.mBox.ext[1]; 

  if (node->mSSR.mRadius <= ME_SMALL_EPSILON) return;

  const BVTransform *tm = &(node->mOBB.mTm); 

  MeReal delmin = 0;
  MeReal delmax = node->mSSR.mRadius;
  MeReal squared_r = node->mSSR.mRadius*node->mSSR.mRadius;

  MeReal delta, good_delta = 0;;
  CxSSR ssr;
  ssr.mRadius = node->mSSR.mRadius;
  int iter = 0;
  const int MAXITERS = 5;
  
  lsVec3 *v[3], vlocal[3]; 

  while ( iter < MAXITERS ) {

      delta = (MeReal)0.5*(delmin + delmax);      
      ssr.mExtX = node->mSSR.mExtX - delta;
      ssr.mExtY = node->mSSR.mExtY - delta;

      for (int i=0; i<numTris; i++) {

        CxTriangleMeshGetTriangleVertexPtrs(mesh, triList[i], &v[0], &v[1], &v[2]);        
         
        MeReal dist;
        for (int j=0; j<3; j++) {
            tm->inverseTransform( *v[j], &vlocal[j] );
            if ( McdSquaredDistancePointToRectangle(&ssr, vlocal[j], dist) ) {
                if (dist > squared_r ) {
                    delmax = delta;
                    goto done;
                }
            }
        }
      }

      delmin = delta;  
      good_delta = delta;
      done: iter++;
  }

  node->mSSR.mExtX -= good_delta;
  node->mSSR.mExtY -= good_delta;
}
 
 
//-----------------------------------------------------------------------------
// inObb->mBox.ext[i] are sorted in a descending order
// This was guaranteed by the construction of BVNode 
//-----------------------------------------------------------------------------
void 
McdBuildSSRFromOBB( const CxObb* inObb, CxSSR *outSSR ) {

  outSSR->mRadius = inObb->mBox.ext[2];    
  outSSR->mExtX = inObb->mBox.ext[0];
  outSSR->mExtY = inObb->mBox.ext[1];    
}

// NOTE: m1To0 is a final compounded transform. In other word, node0's and
//       node1's transform have been taken into account.
//       Need discussion ??
//-----------------------------------------------------------------------------
MeReal lsMinDistanceBoxBox( const BVDNode& node0, const BVDNode& node1, 
        const lsTransform& m1To0 )
{
    const CxSSR *ssr0 = &(node0.mSSR);
    const CxSSR *ssr1 = &(node1.mSSR); 
    
    CxRectangle rect0, rect1;

    lsVec3 b(-ssr0->mExtX, -ssr0->mExtY, 0);
    lsVec3 e0(2*ssr0->mExtX, 0, 0);
    lsVec3 e1(0, 2*ssr0->mExtY, 0);

    rect0.setData(b, e0, e1);

    b.setValue(-ssr1->mExtX, -ssr1->mExtY, 0);
    e0.setValue(2*ssr1->mExtX, 0, 0);
    e1.setValue(0, 2*ssr1->mExtY, 0);

    lsVec3 bTo0, e0To0, e1To0;
    m1To0.transform(b, &bTo0);
    m1To0.transformWithoutTranslate(e0, &e0To0);
    m1To0.transformWithoutTranslate(e1, &e1To0);

    rect1.setData(bTo0, e0To0, e1To0);

    Rectrect rr(&rect0, &rect1);

    MeReal dist = rr.getMinDist();

    if ( dist == 0.0 )
        return 0.0;
    else {
        dist -= ( ssr0->mRadius + ssr1->mRadius );
        return ( dist > 0 ? dist : (MeReal)0.0 );
    } 
}

// NOTE: m1To0 is a final compounded transform. In other word, node0's and
//       node1's transform have been taken into account.
//       Need discussion ?? 
//-----------------------------------------------------------------------------
MeReal lsMaxDistanceBoxBox( const BVDNode& node0, const BVDNode& node1, 
			    const BVTransform& m1To0 )
{  
  BVVec3 t;
  m1To0.getTranslation( &t );
  MeReal dist = (MeReal) (t[0]*t[0] + t[1]*t[1] + t[2]*t[2]);
  dist = (MeReal) sqrt( dist );
  return (dist+( node0.mSSR.mRadius + node1.mSSR.mRadius)); 
} 


