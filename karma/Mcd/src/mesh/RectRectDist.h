// -*-c++-*-
//=============================================================================
// File:        RectRectDist.h
//  
// 
// $Revision: 1.4 $ 
// $Date: 2001/05/04 15:48:28 $ 
// 
//=============================================================================

#ifndef LSRECTRECTDIST_H_
#define LSRECTRECTDIST_H_

#include <math.h> 
#include "lsVec3.h"

// #define MIN_POINTS
#undef  MIN_POINTS

//-----------------------------------------------------------------------------
class CxRectangle
{   
public:
    CxRectangle(){};

  void setData( const lsVec3& b_, const lsVec3& e0_, const lsVec3& e1_ )
    {
      b = b_; e0 = e0_; e1 = e1_;
    }

private:
  unsigned int isRectangle( const MeReal eps = 1.0E-4);

public:
  lsVec3 b, e0, e1;  		
};

  
//-----------------------------------------------------------------------------
// find distance between two rectangles in 3D
class Rectrect
{
public:

  Rectrect( const CxRectangle* rect0_, const CxRectangle* rect1_ );

  ~Rectrect() {}

  void set( const CxRectangle* rect0_, const CxRectangle* rect1_ );

  MeReal getMinDist( void ) const;

#ifdef MIN_POINTS
  MeReal getMinDist( lsVec3* p0, lsVec3* p1) const;
#endif

private:

  Rectrect(); // unimplemented		

  unsigned int minVertexVertex( const int& idv0, const int& idv1 );

  unsigned int processVertexVertex();

  unsigned int minVertexFace( const int& idv, const int& idf );

  unsigned int processVertexFace();

  unsigned int minVertexEdge( const int& idv_rect, const int& idv_v,
			      const int& ide_rect, const int& ide_e );

  unsigned int processVertexEdge();

  unsigned int minEdgeEdge( const lsVec3& A, const lsVec3& B,
			    const lsVec3& C, const lsVec3& D );

  unsigned int processEdgeEdge();

  inline void process();

  // void invalidateVertices(); 
  // void invalidateEdge();

public:

  enum { VV = 0, VF = 1, VE = 2, EE = 3, PENETRATION = 4 };

  unsigned int type;

public:
  const CxRectangle *rect[2];   // two rects	
  lsVec3 c[2];                  // centers
  lsVec3 v[2][4];               // vertices
  MeReal minDist;               // min. distance 

#ifdef MIN_POINTS
  lsVec3 vsct[2];               // two min_dist points 
#endif

  // unsigned int valid_v[2][4];	    // 
  // unsigned int valid_e[2][4];	    // 
};

//------------------------------------------------------------------------------
inline 
Rectrect::Rectrect( const CxRectangle* rect0_, const CxRectangle* rect1_ ) 	
  :minDist(0), type( PENETRATION )
{  
  rect[0] = rect0_;
  rect[1] = rect1_;
  v[0][0] = rect[0]->b;
  v[0][1] = v[0][0] + rect[0]->e0;
  v[0][2] = v[0][0] + rect[0]->e0 + rect[0]->e1;
  v[0][3] = v[0][0] + rect[0]->e1;
  v[1][0] = rect[1]->b;
  v[1][1] = v[1][0] + rect[1]->e0;
  v[1][2] = v[1][0] + rect[1]->e0 + rect[1]->e1;
  v[1][3] = v[1][0] + rect[1]->e1; 

  /*
    valid_v[0][0] = valid_v[0][1] = valid_v[0][2] = valid_v[0][3] = 1;
    valid_v[1][0] = valid_v[1][1] = valid_v[1][2] = valid_v[1][3] = 1;
    valid_e[0][0] = valid_e[0][1] = valid_e[0][2] = valid_e[0][3] = 1;
    valid_e[1][0] = valid_e[1][1] = valid_e[1][2] = valid_e[1][3] = 1;	 
  */
	 
  c[0] = ( rect[0]->b + (rect[0]->e0 + rect[0]->e1)*0.5 );
  c[1] = ( rect[1]->b + (rect[1]->e0 + rect[1]->e1)*0.5 );

  // invalidateVertices();
  // invalidateEdge();

  process();
}

//------------------------------------------------------------------------------
inline void 
Rectrect::process()
{
  if ( processVertexVertex() == 1 )
    {
      type = VV;
      return;
    }	
 
  if ( processVertexFace() == 1 )
    {
      type = VF;
      return;
    }	

  if ( processVertexEdge() ==  1 )
    {
      type = VE;
      return;
    }

  if ( processEdgeEdge() == 1 )
    {
      type = EE;
      return;
    } 

}

//------------------------------------------------------------------------------
inline void 
Rectrect::set( const CxRectangle* rect0_, const CxRectangle* rect1_ )
{	
  rect[0] = rect0_;
  rect[1] = rect1_;
  v[0][0] = rect[0]->b;
  v[0][1] = v[0][0] + rect[0]->e0;
  v[0][2] = v[0][0] + rect[0]->e0 + rect[0]->e1;
  v[0][3] = v[0][0] + rect[0]->e1;
  v[1][0] = rect[1]->b;
  v[1][1] = v[1][0] + rect[1]->e0;
  v[1][2] = v[1][0] + rect[1]->e0 + rect[1]->e1;
  v[1][3] = v[1][0] + rect[1]->e1; 

  /*
    valid_v[0][0] = valid_v[0][1] = valid_v[0][2] = valid_v[0][3] = 1;
    valid_v[1][0] = valid_v[1][1] = valid_v[1][2] = valid_v[1][3] = 1;
    valid_e[0][0] = valid_e[0][1] = valid_e[0][2] = valid_e[0][3] = 1;
    valid_e[1][0] = valid_e[1][1] = valid_e[1][2] = valid_e[1][3] = 1;	 
  */

  c[0] = ( rect[0]->b + (rect[0]->e0 + rect[0]->e1)*0.5 );
  c[1] = ( rect[1]->b + (rect[1]->e0 + rect[1]->e1)*0.5 );

  minDist = 0;

  // invalidateVertices();
  // invalidateEdge();	
}

//------------------------------------------------------------------------------
inline MeReal
Rectrect::getMinDist( void ) const
{
  return minDist;
}

#ifdef MIN_POINTS
//------------------------------------------------------------------------------
inline MeReal
Rectrect::getMinDist( lsVec3* p0, lsVec3* p1) const
{
  *p0 = vsct[0];
  *p1 = vsct[1];
  return minDist;
}
#endif

#endif   // RECTRECT_H_
