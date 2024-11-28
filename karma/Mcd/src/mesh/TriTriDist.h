// -*-c++-*-
//=============================================================================
// File:        TriTriDist.h
//  
// 
//  $Id: TriTriDist.h,v 1.2 2001/05/04 15:48:28 piercarl Exp $
// 
//=============================================================================

#ifndef TRITRIDIST_H_
#define TRITRIDIST_H_

#include <math.h> 
#include "lsVec3.h"
 

/* temporary replacement in order to be able to compile */
struct Triangle {
    const lsVec3 *v[3];
    const lsVec3 *unitNormal;    
}; 
  
//-----------------------------------------------------------------------------
// find distance between two triangles in 3D
class TriTriDist
{
public:

  // TriTriDist( const Triangle* tri0_, const Triangle* tri1_ ); 

  TriTriDist( const lsVec3 *v00, const lsVec3 *v01, const lsVec3 *v02, 
              const lsVec3 *normal0, 
              const lsVec3 *v10, const lsVec3 *v11, const lsVec3 *v12, 
              const lsVec3 *normal1 );

  ~TriTriDist() {}   

  MeReal getMinDist( void );

  // In penetration case, pt0 = pt1 = NULL
  MeReal getMinDist( lsVec3* pt0, lsVec3* pt1 );

private:

  TriTriDist(); // unimplemented		

  unsigned int minVertexVertex( const int& idv0, const int& idv1 );

  unsigned int processVertexVertex();

  unsigned int minVertexFace( const int& idv, const int& idf );

  unsigned int processVertexFace();

  unsigned int minVertexEdge( const int& idv_tri, const int& idv_v, const int& ide_tri, const int& ide_e );

  unsigned int processVertexEdge();

  unsigned int minEdgeEdge( const lsVec3* A, const lsVec3* B,const lsVec3* C, const lsVec3* D );

  unsigned int processEdgeEdge();

  inline void process();
 

public:

  enum { VV = 0, VF = 1, VE = 2, EE = 3, PENETRATION = 4 };

  unsigned int type;

public:
  Triangle tri[2];      // two tris
  MeReal minDist;       // min. distance 

  // for now, we use as two data member !!!!
  lsVec3 pt[2];	 
};

//------------------------------------------------------------------------------
inline 
TriTriDist::TriTriDist(   
              const lsVec3 *v00, const lsVec3 *v01, const lsVec3 *v02, 
              const lsVec3 *normal0, 
              const lsVec3 *v10, const lsVec3 *v11, const lsVec3 *v12, 
              const lsVec3 *normal1 ) 
              : minDist(0), type( PENETRATION )
{  
  tri[0].v[0] = v00;
  tri[0].v[1] = v01;
  tri[0].v[2] = v02;
  tri[0].unitNormal = normal0;
  tri[1].v[0] = v10;
  tri[1].v[1] = v11;
  tri[1].v[2] = v12;   
  tri[1].unitNormal = normal1;  
}
 


#endif    // TRITRIDIST_H_
