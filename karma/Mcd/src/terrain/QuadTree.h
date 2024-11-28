/* -*-c++-*-
 *===============================================================
 * File:        QuadTree.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.5.18.1 $
 * $Date: 2002/04/04 15:28:58 $
 *
 *================================================================
 */

#ifndef QuadTree_H
#define QuadTree_H

#include "McdRGHeightFieldUtils.h"

class QuadTreeNode {
public:
  QuadTreeNode ();

  QuadTreeNode *getNode( int i) const
  {
    return mChild[i];
  }

  // y ---------
  //   | 3 | 2 |
  //   ---------
  //   | 0 | 1 |
  //   --------- x
  QuadTreeNode *mChild[4];

  MeReal xmin, xmax;
  MeReal ymin, ymax;
  MeReal zmin, zmax;
  int ix0, ix1;
  int iy0, iy1;

  enum{ DEFAULT_ID = -1 };
};


/**
 * QuadTree
 */
class QuadTree {

public:

  QuadTree( const McdRGHeightField *rghf );
  QuadTree();
  ~QuadTree();
  void build();
  void divideNode( QuadTreeNode *node );

public:

  const McdRGHeightField *mRGHF;

private:

  QuadTreeNode *mTree;


private:

};

/* QuadTreeNode */
/*---------------------------------------------------------------------------*/
inline
QuadTreeNode::QuadTreeNode()
: xmin(0), xmax(0), ymin(0), ymax(0), zmin(0), zmax(0),
  ix0(DEFAULT_ID), ix1(DEFAULT_ID), iy0(DEFAULT_ID), iy1(DEFAULT_ID)
{
  for (int i=0; i<4; i++) mChild[i] = 0;
}


/* QuadTree */
/*---------------------------------------------------------------------------*/
inline
QuadTree::QuadTree(const McdRGHeightField *rghf)
: mTree(0), mRGHF( rghf )
{}

inline
QuadTree::QuadTree()
: mTree(0)
{}

#endif /* QuadTree */





