// -*-c++-*-
//=============================================================================
// File:        SSR.h
//  
// Author:      Zhaoheng Liu
//
// $Revision: 1.3 $ 
// $Date: 2001/05/04 15:48:28 $ 
// 
//=============================================================================
#ifndef SSR_H
#define SSR_H
  
#include "lsVec3.h"
#include "BVNode.h" 
#include "CxTriangleMesh.h" 

 
/////////////////////////// global function //////////////////////////////////
MeReal MinDistanceBoxBox( const BVDNode& node1, const BVDNode& node2, 
			  const BVTransform& m2To1 );

MeReal MaxDistanceBoxBox( const BVDNode& node1, const BVDNode& node2, 
        const BVTransform& m2To1 );

 
void McdBuildSSRFromOBB( const CxObb* inObb, CxSSR *outSSR );

void 
McdBuildSSRFromOBB( CxTriangleMesh *mesh, BVDNode* node, int *triList, int numTris);

MeReal lsMaxDistanceBoxBox( const BVDNode& node0, const BVDNode& node1, 
			    const BVTransform& m1To0 );
#endif
