/* -*-c++-*-
 *===============================================================
 * File:        BVTree.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.8.10.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#ifndef BVTree_H
#define BVTree_H

#include "BVTransform.h"
#include "BVNode.h"
#include "McdCheck.h"
#include "CxTriangleMesh.h"


/**
 * BVTree
 */
class     BVTree {
  public:
              BVTree();
             ~BVTree();
    BVTree(CxTriangleMesh *mesh, int useDistance);
    void      setMesh(CxTriangleMesh *mesh);
    unsigned int build();
    bool      isBuilt();
    void      printTree();
    BVNode   *getNodePtr(int id) const;
    BVNode   *getRootNodePtr() const;
    MeReal    computeTotalVolume();
    CxTriangleMesh *mMesh;
  private:
    unsigned int splitNode(int *triList, int numTris, BVNode *parentNode);
    void      computeNodeVolume(BVNode *node, MeReal &v);
    void      printNode(BVNode *node, int id);
    BVNode   *mBVTree;
    int       mBuiltNodeCount;
    int mProperties;
};

/**
 * constructor
 */
inline BVTree::BVTree(CxTriangleMesh *mesh, int useDistance):
mBVTree(0), mBuiltNodeCount(0), mProperties(useDistance)
{
    setMesh(mesh);
}

/**
 * constructor
 */
/*
inline BVTree::BVTree():
mBVTree(0), mBuiltNodeCount(0), mProperties(0)
{
}
*/

/**
 * get node ptr
 */
inline BVNode *
BVTree::getNodePtr(int id) const
{
  // return &(mBVTree[id]);
  if (mProperties == McdTriangleMeshOptionNoDistance)
    return &(mBVTree[id]);
  else
    return &( ((BVDNode*)mBVTree)[id]);
  // should really be a virtual function...
}

/**
 *
 */
inline    bool
BVTree::isBuilt()
{
    return (mBVTree != 0);
}

/**
 * get root node ptr
 */
inline BVNode *
BVTree::getRootNodePtr() const
{
    return &(mBVTree[0]);
}

#endif              /* BVTree */
