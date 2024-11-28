// -*-c++-*-
//=============================================================================
// File:        RectRectDist.cpp
// 
// 
// 
// $Revision: 1.4 $ 
// $Date: 2001/05/04 15:48:28 $ 
// 
//=============================================================================

#include "RectRectDist.h"   

const MeReal lsEPS_P =  1.0E-4f;
const MeReal lsEPS_N = -1.0E-4f;

//------------------------------------------------------------------------------
// id0 -> from rect0; id1 -> from rect1
unsigned int
Rectrect::minVertexVertex( const int& idv0, const int& idv1 )
{
    int i(0);
    lsVec3 axis = v[1][idv1] - v[0][idv0];
    axis.normalize(); // necessary when two rects are far part away

    lsVec3 p;

    for ( i = 0; i < 4; i++ ) {
	if ( i != idv0 ) { // && valid_v0[i] )
	    p = v[0][i] - v[0][idv0];
	    if ( axis.dot( p ) > 0 )
		return 0; // not a minimum distance feature pair
	}
    }

    for ( i = 0; i < 4; i++ ) {
	if ( i != idv1 ) { // && valid_v1[i] )
	    p = v[1][i] - v[1][idv1];
	    if ( axis.dot( p ) < 0 )
		return 0; // not a minimum distance feature pair
	}
    }	

    return 1; // this is a minimum distance feature pair
}


//------------------------------------------------------------------------------
unsigned int
Rectrect::processVertexVertex()
{
  int i, j;
  for ( i = 0; i < 4; i++ )
    {
      // if ( valid_v[0][i] )
      {
	for ( j = 0; j < 4; j++ )
	  {
				// if ( valid_v[1][j] )
	    {
	      if ( minVertexVertex( i, j ) == 1 )
		{
		  lsVec3 diff = v[0][i] - v[1][j];
		  minDist = (MeReal) sqrt( diff.dot( diff ) ); 
#ifdef MIN_POINTS
      vsct[0] = v[0][i];
      vsct[1] = v[1][j];
#endif
		  return 1;
		}
	    }				
	  }
      }
    }

  return 0;
}
 
//------------------------------------------------------------------------------
unsigned int 
Rectrect::minVertexFace( const int& idv, const int& idf )
{ 
  lsVec3 n = (rect[idf]->e0).cross( rect[idf]->e1 );	 

  if ( n.normalize() == 0 )
    {  
      // cerr << "lsRectrect::minVertexFace: Degenerate case ......" << endl;
      // exit( 0 );
      /*
      cerr << "edge 0: " << rect[idf]->e0 << endl;
      cerr << "edge 1: " << rect[idf]->e1 << endl;
      MeReal min_dist = 1.0E30;
      for ( int k = 0; k < 3; k++ )
	{
	  lsVec3 diff = v[idv][k] - rect[idf]->b;
	  MeReal my_dist = sqrt( diff.dot(diff) );
          if ( my_dist < min_dist )
	    {
	      min_dist = my_dist;
	    }
	}
      return min_dist;
      */
      return 0;
    } 
	
  // project one vertex of rect onto n
  MeReal proj = n.dot( rect[idf]->b );	    
	
  // project vertices of rect0 onto n
  MeReal sign;
  sign = n.dot( v[idv][0] ); 

  // not very robust, to be re-done, introduce tolerance
  int id_min = 0;
  unsigned int bPos = 1;
  MeReal dist = sign - proj;
  MeReal dist_min = (MeReal) MeFabs( dist );
 
  if ( dist < 0 ) bPos = 0;  	  

  for ( int i = 1; i < 4; i++ )
    {
      sign = n. dot( v[idv][i] );		 

      dist = sign - proj;
      if ( bPos == 0 )
	{
	  if ( dist > 0 ) return 0;			 
	}
      else
	{		
	  if ( dist < 0 ) return 0; 
	}

      MeReal dist_abs = (MeReal) MeFabs( dist );		 

      if ( dist_abs < dist_min )
	{
	  dist_min = dist_abs;
	  id_min = i;
	}
    }	  
 
  // when get here, all vertices of rect[idv] are found 
  // in one side of rect[idf]	 

  lsVec3 eu = rect[idf]->e0;
  MeReal len_e = eu.normalize();  // if len_e = 0: degenerate rect

  MeReal proj_b = eu.dot( rect[idf]->b );
  MeReal proj_p = eu.dot( v[idv][id_min] );

  if ( proj_p < proj_b ) return 0; 	 
  if ( (proj_p - proj_b) > len_e ) return 0;	 

  eu = rect[idf]->e1;
  len_e = eu.normalize();        // if len_e = 0: degenerate rect

  proj_b = eu.dot( rect[idf]->b );
  proj_p = eu.dot( v[idv][id_min] );

  if ( proj_p < proj_b ) return 0;	 
  if ( (proj_p - proj_b) > len_e ) return 0;	 

  minDist = dist_min;      

#ifdef MIN_POINTS
  vsct[0] = v[idv][id_min];
  MeReal d = -n.dot( v[idf][0]);
  MeReal A = n.dot(vsct[0])+d;
  MeReal t = -A/n.dot(n);
  vsct[1] = vsct[0] + n*t;
#endif

  return 1;
}

//------------------------------------------------------------------------------
unsigned int 
Rectrect::processVertexFace()
{	
  if ( minVertexFace( 0, 1 ) == 1 )
    {
      return 1;
    } 

  if ( minVertexFace( 1, 0 ) == 1 )
    {
      return 1;
    }
	 
  return 0;
}

//------------------------------------------------------------------------------
unsigned int
Rectrect::minVertexEdge( const int& idv_rect, const int& idv_v,
			   const int& ide_rect, const int& ide_e )
{
    // should pass directly -> todo
    int i(0);
    lsVec3 p = v[idv_rect][idv_v];         // the vertex	
    lsVec3 pe1 = v[ide_rect][ide_e];       // the point1
    lsVec3 pe2 = v[ide_rect][(ide_e+1)%4]; // the point2

    lsVec3 diff = pe2 - pe1; 

    MeReal len_sq = diff.dot( diff );

    if ( len_sq == 0 ) return 0; // degenerate rectangle
	 
    MeReal t = diff.dot( p) - diff.dot( pe1 );
    t /= len_sq; 
	 
    if ( t < lsEPS_N || t > (1 + lsEPS_P) ) return 0; 

    lsVec3 ist = pe1 + t * diff;

    lsVec3 axis = ist - p; 
    axis.normalize();   // necessary when two rects are far part away

    for ( i = 0; i < 4; i++ ) {		 
	if ( i != idv_v ) {
	    diff = v[idv_rect][i] - p;	
			 
	    if ( diff.dot( axis ) > lsEPS_P )			 
		return 0; 
	} 
    }

    for ( i = 0; i < 4; i++ ) {
	diff = v[ide_rect][i] - ist;
		 
	if ( diff.dot( axis ) < lsEPS_N )		 
	    return 0;	 
    }

    diff = ist - p;
    minDist = (MeReal) sqrt( diff.dot( diff ) );

#ifdef MIN_POINTS
    vsct[0] = p;
    vsct[1] = ist;
#endif
	 
    return 1;
}


//------------------------------------------------------------------------------
unsigned int 
Rectrect::processVertexEdge()
{
  int i, j;
  for ( i = 0; i < 4; i++ )
    {
      // if ( valid_v[0][i] )
      {
	for ( j = 0; j < 4; j++ )
	  {
				// if ( valid_e[1][j] )
	    {
	      if ( minVertexEdge( 0, i, 1, j ) == 1 )
		return 1;
	    }
	  }
      }
    }

  for ( i = 0; i < 4; i++ )
    {
      // if ( valid_v[1][i] )
      {
	for ( j = 0; j < 4; j++ )
	  {
				// if ( valid_e[0][j] )
	    {
	      if ( minVertexEdge( 1, i, 0, j ) == 1 )
		return 1;
	    }
	  }
      }
    }

  return 0;
}


//------------------------------------------------------------------------------
// Vertices A and B come from rect0
// Vertices C and D come from rect1
unsigned int 
Rectrect::minEdgeEdge( const lsVec3& A, const lsVec3& B,
			 const lsVec3& C, const lsVec3& D )
{		
    int i(0);
    lsVec3 dirab = B - A;
    lsVec3 dircd = D - C;
    lsVec3 diff =  A - C;
    MeReal H = dirab.dot( dircd );
    MeReal Q = dirab.dot( dirab );
    MeReal P = dircd.dot( dircd );

    MeReal div = H*H - Q*P;

    if (  div == 0 )
	return 0;  // two edges parallel ?

    MeReal S = diff.dot( dirab );
    MeReal T = diff.dot( dircd );

    MeReal parm_t = (S*P - T*H)/div;
    if ( parm_t > 1.0 + lsEPS_P || parm_t < lsEPS_N )
	return 0;

    MeReal parm_s = (S*H - T*Q)/div;
    if ( parm_s > 1.0 + lsEPS_P || parm_s < lsEPS_N )
	return 0;

    lsVec3 p0 = A + parm_t*dirab;
    lsVec3 p1 = C + parm_s*dircd;

    lsVec3 axis = p1 - p0;
    // !!!!
    axis.normalize();  // necessary when two rects are far part away
 
    for ( i = 0; i < 4; i++ ) {		 
	diff = v[0][i] - p0;			
	if ( diff.dot( axis ) > lsEPS_P )
	    return 0; 	 
    }

    for ( i = 0; i < 4; i++ ) {	 	
	diff = v[1][i] - p1;
	if ( diff.dot( axis ) < lsEPS_N )
	    return 0;	 
    }

    diff = p1 -p0;

#ifdef MIN_POINTS
    vsct[0] = p0;
    vsct[1] = p1;
#endif

    minDist = (MeReal) sqrt( diff.dot( diff ) );	 

    return 1;
}

//------------------------------------------------------------------------------
unsigned int
Rectrect::processEdgeEdge()
{	 
  int i1, j1, res;
  for ( int i = 0; i < 4; i++ )
    {
      // if ( valid_e[0][i] )
      {
	i1 = (i + 1) % 4;
	for ( int j = 0; j < 4; j++ )
	  {
				// if ( valid_e[1][j] )
	    {					 
	      j1 = (j + 1) % 4;
	      res = minEdgeEdge( v[0][i],v[0][i1], v[1][j], v[1][j1] );				 
	      if ( res == 1 )
		return 1;
	    }
	  }
      }
    }	 

  return 0;
}


#if 0
//------------------------------------------------------------------------------
void 
lsRectrect::invalidateVertices()
{
  // lsVec3 axis = c[1] - c[0];
  lsVec3 axis = c[1] - c[0];

  Normalize( axis );

  // !!!!
  if ( MeFabs( Dot( axis, axis ) ) < lsEPS_P )  // used for |eps|
    return;

	// !!!!
  lsVec3 n0 = Cross( rect[0]->e0, rect[0]->e1 );
  Normalize( n0 );
  lsVec3 n1 = Cross( rect[1]->e0, rect[1]->e1 );
  Normalize( n1 );

	// !!!!
  lsVec3 alg = Cross( n0, axis );
  if ( MeFabs( Dot( alg, alg ) ) < 0.01 )
    return;

	// !!!!
  alg = Cross( n1, axis );
  if ( MeFabs( Dot( alg, alg ) ) < 0.01 )
    return;	

  int i, count = 0;

  lsVec3 p;
 	
  for ( i = 0; i < 4; i++ )
    {
      p = v[0][i] - c[0];
      if ( Dot( p, axis ) < 0 )
	{		 
	  valid_v[0][i] = 0;
	  count++;
	}
    } 

  if ( count != 2 )
    {	 
      for ( i = 0; i < 4; i++ )
	valid_v[0][i] = 1;
    }
 
  // rect1:
  count = 0;		 
	
  for ( i = 0; i < 4; i++ )
    {
      p = v[1][i] - c[1];
      if ( Dot( p, axis ) > 0 )
	{
	  valid_v[1][i] = 0;
	  count++;
	}
    }  

  if ( count != 2 )
    {		 
      for ( i = 0; i < 4; i++ )
	valid_v[1][i] = 1;
    }	
}


//------------------------------------------------------------------------------
void 
lsRectrect::invalidateEdge()
{
  int j;
  for ( int i = 0; i < 4; i++ )
    {
      j = (i + 1) % 4;
      if ( !valid_v[0][i] && !valid_v[0][j] )
	valid_e[0][i] = 0;

      if ( !valid_v[1][i] && !valid_v[1][j] )
	valid_e[1][i] = 0;
    } 
}

#endif
