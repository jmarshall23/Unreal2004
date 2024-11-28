// -*-c++-*-
//=============================================================================
// File:        TriTriDist.cpp
//  
// 
//  $Id: TriTriDist.cpp,v 1.4 2001/05/04 15:48:28 piercarl Exp $
// 
//=============================================================================

#include "TriTriDist.h"  

const MeReal lsEPS_P =  1.0E-4f;
const MeReal lsEPS_N = -1.0E-4f;

// id0 -> from tri0; id1 -> from tri1
unsigned int
TriTriDist::minVertexVertex( const int& idv0, const int& idv1 )
{
  int i(0);
  lsVec3 axis = *(tri[1].v[idv1]) - *(tri[0].v[idv0]);
  axis.normalize(); // necessary when two rects are far part away

  lsVec3 p;

  for ( i = 0; i < 3; i++ )
    {
      if ( i != idv0 ) 
	{
	  p = *(tri[0].v[i]) - *(tri[0].v[idv0]);
	  if ( axis.dot( p ) > 0 )
	    return 0; // not a minimum distance feature pair
	}
    }

  for ( i = 0; i < 3; i++ )
    {
      if ( i != idv1 ) // && valid_v1[i] )
	{
	  p = *(tri[1].v[i]) - *(tri[1].v[idv1]);
	  if ( axis.dot( p ) < 0 )
	    return 0; // not a minimum distance feature pair
	}
    }	

  return 1; // this is a minimum distance feature pair
}


//------------------------------------------------------------------------------
unsigned int
TriTriDist::processVertexVertex()
{
  int i, j;
  for ( i = 0; i < 3; i++ )
    { 	 
      for ( j = 0; j < 3; j++ )
	{ 			 
	  if ( minVertexVertex( i, j ) == 1 )
	    {
	      lsVec3 diff = *(tri[0].v[i]) - *(tri[1].v[j]);
		  pt[0] = *(tri[0].v[i]);
		  pt[1] = *(tri[1].v[j]);
	      minDist = (MeReal) sqrt( diff.dot( diff ) ); 
	      return 1;
	    }		 
	}	
    }

  return 0;
}

//------------------------------------------------------------------------------
unsigned int 
TriTriDist::minVertexFace( const int& idv, const int& idf )
{ 
    int i(0);
    const lsVec3 *n =  tri[idf].unitNormal;
 
    MeReal proj = n->dot( *(tri[idf].v[0]) );	 
 
    MeReal dist[3];    
  
    // project vertices of tri[idv] onto n
    MeReal sign = n->dot( *(tri[idv].v[0]) ); 

    // not very robust, to be re-done, introduce tolerance
    int id_min = 0;
    unsigned int bPos = 1;
    dist[0] = sign - proj;
    MeReal dist_min = (MeReal) MeFabs( dist[0] );
 
    if ( dist[0] < 0 ) bPos = 0;  	  

    for ( i = 1; i < 3; i++ )
    {
	sign = n->dot( *(tri[idv].v[i]) );		 

	dist[i] = sign - proj;
	if ( bPos == 0 )
	{
	    if ( dist[i] > 0 ) return 0;			 
	}
	else
	{		
	    if ( dist[i] < 0 ) return 0; 
	}

	MeReal dist_abs = (MeReal) MeFabs( dist[i] );		 

	if ( dist_abs < dist_min )
	{
	    dist_min = dist_abs;
	    id_min = i;
	}
    }	  
   
    lsVec3 np, edge;
    MeReal d;
	 
    for ( i = 0; i < 3; i++ )
    {
	int j = (i + 1) % 3;
	edge  = *(tri[idf].v[j]) - *(tri[idf].v[i]);
	np = n->cross( edge );
	d  = -np.dot( *(tri[idf].v[i]) );
	MeReal lhs = np.dot( *(tri[idv].v[id_min]) ) + d;
	if ( lhs < 0.0 )
	    return 0;	
    }	 

    pt[idv] = *(tri[idv].v[id_min]);
    pt[idf] = *(tri[idv].v[id_min]) - (*n)*dist[id_min];
    minDist = dist_min;  
 
    return 1;
}

//------------------------------------------------------------------------------
unsigned int 
TriTriDist::processVertexFace()
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
TriTriDist::minVertexEdge( const int& idv_tri, const int& idv_v,
			     const int& ide_tri, const int& ide_e )
{
    // should pass directly -> todo
    lsVec3 p = *(tri[idv_tri].v[idv_v]);         // the vertex	
    lsVec3 pe1 = *(tri[ide_tri].v[ide_e]);       // the point1
    lsVec3 pe2 = *(tri[ide_tri].v[(ide_e+1)%3]); // the point2
    int i(0);
    lsVec3 diff = pe2 - pe1; 
    MeReal len_sq = diff.dot( diff );

    if ( len_sq == 0 ) return 0; // degenerate rectangle
	 
    MeReal t = diff.dot( p) - diff.dot( pe1 );
    t /= len_sq; 
	 
    if ( t < lsEPS_N || t > (1 + lsEPS_P) ) return 0; 

    lsVec3 ist = pe1 + t * diff;

    lsVec3 axis = ist - p; 
    axis.normalize();   // necessary when two rects are far part away

    for ( i = 0; i < 3; i++ ) {		 
	if ( i != idv_v ) {
	    diff = *(tri[idv_tri].v[i]) - p;	
			 
	    if ( diff.dot( axis ) > lsEPS_P )			 
		return 0; 
	} 
    }

    for ( i = 0; i < 3; i++ ) {	
	diff = *(tri[ide_tri].v[i]) - ist;
		 
	if ( diff.dot( axis ) < lsEPS_N )		 
	    return 0;	 
    }

    diff = ist - p;
    minDist = (MeReal) sqrt( diff.dot( diff ) );
  
    pt[idv_tri] = p;
    pt[ide_tri] = ist;
 
    return 1;
}


//------------------------------------------------------------------------------
unsigned int 
TriTriDist::processVertexEdge()
{
  int i, j;
  for ( i = 0; i < 3; i++ )
    {  
      for ( j = 0; j < 3; j++ )
	{ 
	  if ( minVertexEdge( 0, i, 1, j ) == 1 )
	    return 1;	 
	}	 
    }

  for ( i = 0; i < 3; i++ )
    { 
      for ( j = 0; j < 3; j++ )
	{
	  if ( minVertexEdge( 1, i, 0, j ) == 1 )
	    return 1; 
	} 
    }

  return 0;
}

//------------------------------------------------------------------------------
// Vertices A and B come from tri[0]
// Vertices C and D come from tri[1]
unsigned int 
TriTriDist::minEdgeEdge( const lsVec3* A, const lsVec3* B,
			   const lsVec3* C, const lsVec3* D )
{		
    int i(0);
    lsVec3 dirab = *B - *A;
    lsVec3 dircd = *D - *C;
    lsVec3 diff =  *A - *C;
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

    lsVec3 p0 = *A + parm_t*dirab;
    lsVec3 p1 = *C + parm_s*dircd;

    lsVec3 axis = p1 - p0;
    // !!!!
    axis.normalize();  // necessary when two tris are far part away
 
    for ( i = 0; i < 3; i++ ) {		 
	diff = *(tri[0].v[i]) - p0;			
	if ( diff.dot( axis ) > lsEPS_P )
	    return 0; 	 
    }

    for ( i = 0; i < 3; i++ ){	
	diff = *(tri[1].v[i]) - p1;
	if ( diff.dot( axis ) < lsEPS_N )
	    return 0;	 
    }

    diff = p1 -p0;

    pt[0] = p0;
    pt[1] = p1;

    minDist = (MeReal) sqrt( diff.dot( diff ) );	 

    return 1;
}


//------------------------------------------------------------------------------
unsigned int
TriTriDist::processEdgeEdge()
{	 
  int i1, j1, res;
  for ( int i = 0; i < 3; i++ )
    { 
      i1 = (i + 1) % 3;
      for ( int j = 0; j < 3; j++ )
	{ 
	  j1 = (j + 1) % 3;
	  res = minEdgeEdge( tri[0].v[i], tri[0].v[i1], 
			     tri[1].v[j], tri[1].v[j1] );				 
	  if ( res == 1 )
	    return 1; 
	} 
    }	 

  return 0;
}


//------------------------------------------------------------------------------
MeReal 
TriTriDist::getMinDist( lsVec3* pt0, lsVec3* pt1 )
{
  process();

  if ( type != PENETRATION )
  {
	  *pt0 = pt[0];
	  *pt1 = pt[1];
  }
  else
  {
	  pt0 = 0; pt1 = 0;
  }

  // cerr << "type: " << type << endl;

  return minDist;
}
 
//------------------------------------------------------------------------------
MeReal
TriTriDist::getMinDist( void )
{
  process();
  return minDist;
}

//------------------------------------------------------------------------------
void 
TriTriDist::process()
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
