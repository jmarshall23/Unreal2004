/* -*-c++-*-
 *===============================================================
 * File:        BVNode.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 * $Revision: 1.9.18.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#ifndef BVNode_H
#define BVNode_H

#include "McdCheck.h"
#include "BVTransform.h"
#include "lsVec3.h"

// if defined, timesatmp technique will be used
// #define CXTRIANGLE_USE_TIMESTAMP
#undef CXTRIANGLE_USE_TIMESTAMP

typedef struct BVBox {
    lsVec3    ext;
} BVBox;

typedef struct CxObb {
    BVBox     mBox;
    BVTransform mTm;
#ifdef CXTRIANGLE_USE_TIMESTAMP
    BVTransform mTmGlobal;
    unsigned long mStatus;
#endif
} CxObb;

typedef struct CxSSR {
    MeReal mExtX;
    MeReal mExtY;
    MeReal mRadius;
} CxSSR;

/**
 * BVNode
 */
class BVNode {
/* data member */
private:
    BVNode   *mLeftNode;
    BVNode   *mRightNode;
    BVNode   *mParentNode;
    enum { DEFAULT_ID = -1 };
public:
    CxObb     mOBB;
    int       mTriId;
/* public functions */
public:
    BVNode    ();
    void      initialize();
    inline BVNode   *getLeftNode() const;
    inline BVNode   *getRightNode() const;
    inline BVNode   *getParentNode() const;
    inline void      setLeftNode(BVNode *node);
    inline void      setRightNode(BVNode *node);
    inline void      setParentNode(BVNode *node);
    inline void      getBoxExt(lsVec3 *ext);
    inline void      getBoxExtPtr(MeReal *ext);
    inline unsigned int isRoot() const;
    inline unsigned int isLeaf() const;
    inline BVReal    getObbSize() const;
    static int getDefaultId();
};

class BVDNode : public BVNode {
public:
  // BVDNone();
  CxSSR  mSSR;
  BVDNode   *getLeftNode() const { return (BVDNode*) BVNode::getLeftNode();} ;
  BVDNode   *getRightNode() const { return (BVDNode*) BVNode::getRightNode();} ;
  BVDNode   *getParentNode() const { return (BVDNode*) BVNode::getParentNode();} ;

};

/*----------------------------------------------------------*/

inline BVNode::BVNode ():mLeftNode(0), mRightNode(0), mParentNode(0),
mTriId(DEFAULT_ID)
{
#ifdef CXTRIANGLE_USE_TIMESTAMP
    mOBB.mStatus = 0;
#endif
}

inline void BVNode::initialize()
{
    mLeftNode = 0;
    mRightNode = 0;
    mParentNode = 0;
#ifdef CXTRIANGLE_USE_TIMESTAMP
    mOBB.mStatus = 0;
#endif
    mTriId = DEFAULT_ID;
}

inline int BVNode::getDefaultId()
{
    return DEFAULT_ID;
}

inline BVNode *BVNode::getLeftNode() const
{
    return mLeftNode;
}

inline BVNode *BVNode::getRightNode() const
{
    return mRightNode;
}

inline BVNode *BVNode::getParentNode() const
{
    return mParentNode;
}

inline BVReal
BVNode::getObbSize() const
{
    return (mOBB.mBox.ext[0] + mOBB.mBox.ext[1] + mOBB.mBox.ext[2]);
}

inline void
BVNode::setLeftNode(BVNode *node)
{
    mLeftNode = node;
}

inline void
BVNode::setRightNode(BVNode *node)
{
    mRightNode = node;
}

inline void
BVNode::setParentNode(BVNode *node)
{
    mParentNode = node;
}

inline void
BVNode::getBoxExt(lsVec3 *ext)
{
    *ext = mOBB.mBox.ext;
}

// inline void
// BVNode::getBoxExtPtr(MeReal *ext)
// {
    // ext = (MeReal *) mOBB.mBox.ext.v;
    // //????????? the above code is not doing anything
// }

inline unsigned int
BVNode::isRoot() const
{
    if (mParentNode == 0)
    return 1;
    return 0;
}

inline unsigned int
BVNode::isLeaf() const
{
    if (mLeftNode != 0 || mRightNode != 0)
    return 0;
    return 1;
}

#endif              /* BVNode */
