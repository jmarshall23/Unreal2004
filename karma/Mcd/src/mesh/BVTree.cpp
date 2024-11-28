/* -*-c++-*-
 *===============================================================
 * File:        BVTree.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.15.2.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */
#include <new.h>
#include <MeMemory.h>

#include "ObbFinder.h"
#include "SSR.h"
#include "BVTree.h"


/**
 * AABB
 */
void
findOBB(CxTriangleMesh *mesh, CxObb *obb, int *triList, int numTris)
{
    (obb)->mTm.setIdentity();

    lsVec3    axis(1, 0, 0);
    lsVec3    *vMin, *vMax;
    BVVec3    t;
    MeReal    projMin, projMax;

    ComputeExtremalVertices(mesh, triList, numTris, axis, &vMax, &vMin, &projMax, &projMin);
    (obb)->mBox.ext[0] = (projMax - projMin) * (MeReal) 0.5;
    t[0] = (projMax + projMin) * (BVReal) 0.5;

    axis.setValue(0, 1, 0);
    ComputeExtremalVertices(mesh, triList, numTris, axis, &vMax, &vMin, &projMax, &projMin);
    (obb)->mBox.ext[1] = (projMax - projMin) * (MeReal) 0.5;
    t[1] = (projMax + projMin) * (BVReal) 0.5;

    axis.setValue(0, 0, 1);
    ComputeExtremalVertices(mesh, triList, numTris, axis, &vMax, &vMin, &projMax, &projMin);
    (obb)->mBox.ext[2] = (projMax - projMin) * (MeReal) 0.5;
    t[2] = (projMax + projMin) * (BVReal) 0.5;

    (obb)->mTm.setTranslation(t);
}

/**
 *
 */
BVReal
computeTriangleMeanProjection(lsVec3 *v[3], BVVec3 * axis)
{
    BVReal    proj = axis->dot(*v[0]);

    BVReal    projMin = proj;
    BVReal    projMax = proj;

    for (int i = 1; i < 3; i++) {
    proj = axis->dot(*v[i]);
    if (proj > projMax)
        projMax = proj;
    else if (proj < projMin)
        projMin = proj;
    }

    return (projMin + projMax) * (BVReal) 0.5;
}

/**
 *
 */
void
splitOBB(CxTriangleMesh *mesh, CxObb *obb, int *triList, int numTris,
     int *leftTriList, int *rightTriList, int &numLeftTris, int &numRightTris)
{
    lsVec3   *v[3];
    BVVec3   *axis, center;
    BVReal    projCenter, proj;
    int       k, i;

    for (k = 0; k < 3; k++) {
    // first axis is longest one
    axis = &(obb->mTm.axis(k));
    obb->mTm.getTranslation(&center);
    projCenter = axis->dot(center);
    numRightTris = 0;
    numLeftTris = 0;

    for (i = 0; i < numTris; i++) {
        CxTriangleMeshGetTriangleVertexPtrs((CxTriangleMesh *) mesh,
                        triList[i], &v[0], &v[1], &v[2]);
        proj = computeTriangleMeanProjection(v, axis);
        if (proj < projCenter) {
        leftTriList[numLeftTris] = triList[i];
        numLeftTris++;
        } else {
        rightTriList[numRightTris] = triList[i];
        numRightTris++;
        }
    }

    MeReal    ratio = (MeReal) numLeftTris / (MeReal) numTris;
    bool      refine = 0;
    if (ratio > 0.9) {
        projCenter -= obb->mBox.ext[k] * (MeReal) 0.1;
        refine = 1;
    }
    if (ratio < 0.1) {
        projCenter += obb->mBox.ext[k] * (MeReal) 0.1;
        refine = 1;
    }
    if (refine) {
        numRightTris = 0;
        numLeftTris = 0;

        for (i = 0; i < numTris; i++) {
        CxTriangleMeshGetTriangleVertexPtrs((CxTriangleMesh *) mesh,
                            triList[i], &v[0], &v[1], &v[2]);
        proj = computeTriangleMeanProjection(v, axis);
        if (proj < projCenter) {
            leftTriList[numLeftTris] = triList[i];
            numLeftTris++;

        } else {
            rightTriList[numRightTris] = triList[i];
            numRightTris++;
        }
        }
    }
    if (numRightTris > 0 && numLeftTris > 0)
        return;
    }

    // experimentation showed that
    if (numRightTris == 0 || numLeftTris == 0) {
    numRightTris = 0;
    numLeftTris = 0;
    for (i = 0; i < numTris; i++) {
        if (i % 2 == 0) {
        leftTriList[numLeftTris] = triList[i];
        numLeftTris++;
        } else {
        rightTriList[numRightTris] = triList[i];
        numRightTris++;
        }
    }
    }
}

/**
 * build the OBB tree
 */
unsigned int
BVTree::build()
{
    int       numTris = CxTriangleMeshGetTriangleCount(mMesh);
    int      *id = (int *)MeMemoryAPI.create(numTris * sizeof(int));;

    MCD_CHECK_NULLPTR(id, "int", "BVTree::build");

    for (int i = 0; i < numTris; i++)
    id[i] = i;
    splitNode(id, numTris, 0);
    return 1;
}

/**
 *
 */
unsigned int
BVTree::splitNode(int *triList, int numTris, BVNode *parentNode)
{
    BVNode   *currentNode = getNodePtr(mBuiltNodeCount);
    currentNode->setParentNode(parentNode);

//  This is where we can differentially build AABB / OBB
#if 0
    findOBB(mMesh, &(currentNode->mOBB), triList, numTris); // AABB
#else
    ObbFinder OF;
//  This looks like the orienter for the triList    
    OF.process(mMesh, &(currentNode->mOBB), triList, numTris);	// OBB
    if ( ((CxTriangleMesh_*)mMesh)->m_flags > 0 )
      McdBuildSSRFromOBB( mMesh, (BVDNode*)currentNode, triList, numTris);
#endif

    if (numTris == 1) {
    currentNode->mTriId = triList[0];
    MeMemoryAPI.destroy(triList);
    return 0;
    }

    int      *leftTriList, *rightTriList;
    int       numLeftTris, numRightTris;

    leftTriList = (int *)MeMemoryAPI.create(numTris * sizeof(int));
    rightTriList = (int *)MeMemoryAPI.create(numTris * sizeof(int));
    MCD_CHECK_NULLPTR(leftTriList, "int", "BVTree::splitNode");
    MCD_CHECK_NULLPTR(rightTriList, "int", "BVTree::splitNode");

//  This splits the current OBB in r/l halves
    splitOBB(mMesh, &(currentNode->mOBB), triList, numTris,
         leftTriList, rightTriList, numLeftTris, numRightTris);

    MeMemoryAPI.destroy(triList);

//  Recurse r/l
    if (numLeftTris > 0) {
    mBuiltNodeCount++;
    currentNode->setLeftNode(getNodePtr(mBuiltNodeCount));
    splitNode(leftTriList, numLeftTris, currentNode);
    } else {
        MeMemoryAPI.destroy(leftTriList);
    }

    if (numRightTris > 0) {
    mBuiltNodeCount++;
    currentNode->setRightNode(getNodePtr(mBuiltNodeCount));
    splitNode(rightTriList, numRightTris, currentNode);
    } else {
        MeMemoryAPI.destroy(rightTriList);
    }

    return 1;
}

/**
 * destructor
 */

BVTree::~BVTree()
{
    int numTris = CxTriangleMeshGetTriangleCount(mMesh);
    if (mBVTree)
    { 
        for(BVNode * tmp=mBVTree; tmp < mBVTree + (numTris * 2 - 1); tmp++)
            tmp->~BVNode();
        MeMemoryAPI.destroy(mBVTree);
    }
}

/**
 * set the mesh
 */

void
BVTree::setMesh(CxTriangleMesh *mesh)
{
    mBuiltNodeCount = 0;
    mMesh = mesh;
    MCD_CHECK_NULLPTR(mMesh, "m_mesh", "BVTree::setMesh");

    int       numTris = CxTriangleMeshGetTriangleCount(mMesh);

    if (mProperties == McdTriangleMeshOptionNoDistance) 
    {
        mBVTree = (BVNode*) MeMemoryAPI.create(sizeof(BVNode)*(numTris * 2 - 1)); 
        for (BVNode * tmp = mBVTree; tmp < mBVTree+(numTris * 2 - 1); tmp++)
            new(tmp) BVNode(); 

    } else 
    {
        BVDNode *n = (BVDNode*) MeMemoryAPI.create(sizeof(BVDNode)*(numTris * 2 - 1)); 
        for (BVDNode * tmp = n; tmp < n+(numTris * 2 - 1); tmp++)
            new(tmp) BVDNode(); 
        mBVTree = (BVDNode*)n;
    }
    MCD_CHECK_NULLPTR(mBVTree, "m_BVTree", "BVTree::setMesh");
}

/**
 *
 */
MeReal
BVTree::computeTotalVolume()
{
    MeReal    v = (MeReal) 0.0;
    computeNodeVolume(this->getRootNodePtr(), v);
    return v;
}

/**
 *
 */
void
BVTree::computeNodeVolume(BVNode *node, MeReal &v)
{
    v += node->mOBB.mBox.ext[0] * node->mOBB.mBox.ext[1] * node->mOBB.mBox.ext[2];
    if (!node->isLeaf()) {
    computeNodeVolume(node->getLeftNode(), v);
    computeNodeVolume(node->getRightNode(), v);
    }
}

/**
 *
 */
void
BVTree::printTree()
{
    printNode(getNodePtr(0), 0);
}

/**
 *
 */
void
BVTree::printNode(BVNode *node, int id)
{
#if 0
    printf("C: %d ", id);
    printf("P: %d ", node->getParentId());
    printf("L: %d ", node->getLeftId());
    printf("R: %d ", node->getRightId());

    if (node->isLeaf()) {
    printf("TRI: %d -> ", node->mTriId);
    printf("%f %f %f", node->mOBB.mBox.ext[0], node->mOBB.mBox.ext[1], node->mOBB.mBox.ext[2]);
    }

    printf("\n");

    if (!node->isLeaf()) {
    printNode(getNodePtr(node->getLeftId()), node->getLeftId());
    printNode(getNodePtr(node->getRightId()), node->getRightId());
    }
#endif
}
