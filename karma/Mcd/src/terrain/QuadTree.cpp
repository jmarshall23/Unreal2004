/* -*-c++-*-
 *===============================================================
 * File:        QuadTree.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.5.10.1 $
 * $Date: 2002/04/04 15:28:58 $
 *
 *================================================================
 */

#include "QuadTree.h"

/**
 *
 */
void
findZMinMax( const McdGeometry *g, int ix0, int ix1, int iy0, int iy1,
            MeReal& zmin, MeReal& zmax )
{
  lsVec3 v;
  CxRGHeightFieldGetVertex( g, &v, ix0, iy0);

  zmin = v[2];
  zmax = v[2];

  for (int ix=ix0; ix<ix1; ix++) {
    for (int iy=iy0; iy<iy1; iy++) {
      CxRGHeightFieldGetVertex( (McdGeometry*)g, &v, ix, iy);
      if (v[2] > zmax) zmax = v[2];
      if (v[2] < zmin) zmin = v[2];
    }
  }
}

/**
 *
 */
void
QuadTree::build() {

  CX_NEW(mTree, QuadTreeNode);

  mTree->ix0 = 0;
  mTree->ix1 = ((CxRGHeightField_*)mRGHF)->m_xNbVertices;
  mTree->iy0 = 0;
  mTree->iy1 = ((CxRGHeightField_*)mRGHF)->m_yNbVertices;

  mTree->xmin = ((CxRGHeightField_*)mRGHF)->m_x0;
  mTree->xmax = ((CxRGHeightField_*)mRGHF)->m_xLength + mTree->xmin;
  mTree->ymin = ((CxRGHeightField_*)mRGHF)->m_y0;
  mTree->ymax = ((CxRGHeightField_*)mRGHF)->m_yLength + mTree->ymin;

  findZMinMax( mRGHF, mTree->ix0, mTree->ix1, mTree->iy0, mTree->iy1,
               mTree->zmin , mTree->zmax );

  divideNode( mTree );
}

/**
 *
 */
void
QuadTree::divideNode( QuadTreeNode* node ) {

}

