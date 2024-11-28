/* -*-c++-*-
 *===============================================================
 * File:        IxTriangleMeshLineSegment.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 * Author: Zhaoheng Liu
 *
 * $Revision: 1.20.2.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */
#include <stdio.h>
#include <McdInteractionTable.h>
#include <McdTriangleMesh.h>

#include "IxTriangleMeshLineSegment.h"
#include "BVTree.h"

/**
 *
 */
void
McdTriangleMeshLineSegmentRegisterInteraction(McdFramework *frame)
{
    McdFrameworkSetLineSegInteraction(frame, CxTriangleMeshGetTypeId(), IxTriangleMeshLineSegment);
}

/**
 * (1) box is axis-aligned and origin centered in the c.s.
 * (2) pt0 and pt1 are in the same c.s. as the box
 */
inline    bool
IxLineSegmentCenteredAABB(const lsVec3 &rbox, const lsVec3 &pt0, const lsVec3 &pt1)
{
    lsVec3    w = (pt1 - pt0) * (MeReal) (0.5);
    lsVec3    c = (pt1 + pt0) * (MeReal) (0.5);
    lsVec3    x(MeFabs(w[0]), MeFabs(w[1]), MeFabs(w[2]));

    // disjointness test
    if (MeFabs(c[0]) > x[0] + rbox[0])
    return 0;
    if (MeFabs(c[1]) > x[1] + rbox[1])
    return 0;
    if (MeFabs(c[2]) > x[2] + rbox[2])
    return 0;
    if (MeFabs(c[1] * w[2] - c[2] * w[1]) > rbox[1] * x[2] + rbox[2] * x[1])
    return 0;
    if (MeFabs(c[0] * w[2] - c[2] * w[0]) > rbox[0] * x[2] + rbox[2] * x[0])
    return 0;
    if (MeFabs(c[0] * w[1] - c[1] * w[0]) > rbox[0] * x[1] + rbox[1] * x[0])
    return 0;

    return 1;
}

/**
 *
 */
inline void
getProjectedIndex(const lsVec3 &v, int &i, int &j)
{
    MeReal    a = MeFabs(v[0]), b = MeFabs(v[1]), c = MeFabs(v[2]);

    if (a > b) {
    if (a > c) {
        i = 1;
        j = 2;
    } else {
        i = 0;
        j = 1;
    }
    } else {
    if (b > c) {
        i = 0;
        j = 2;
    } else {
        i = 0;
        j = 1;
    }
    }
}

/**
 *
 */
inline    bool
isInsideTri(const lsVec3 &v0, const lsVec3 &v1, const lsVec3 &v2,
        const lsVec3 &p, const int &i, const int &j)
{
    MeReal    xmx1 = p[i] - v0[i];
    MeReal    y2my1 = v1[j] - v0[j];
    MeReal    x2mx1 = v1[i] - v0[i];
    MeReal    ymy1 = p[j] - v0[j];
    MeReal    x3mx1 = v2[i] - v0[i];
    MeReal    y3my1 = v2[j] - v0[j];

    MeReal    den = x3mx1 * y2my1 - x2mx1 * y3my1;

    if (MeFabs(den) < ME_SMALL_EPSILON)
    return 0;       // degenerate tri.

    MeReal    denInverse = (MeReal) 1.0 / den;

    MeReal    w = (xmx1 * y2my1 - x2mx1 * ymy1) * denInverse;
    if (w < -ME_SMALL_EPSILON)
    return 0;

    MeReal    v = (x3mx1 * ymy1 - xmx1 * y3my1) * denInverse;
    if (v < -ME_SMALL_EPSILON)
    return 0;

    MeReal    u = (MeReal) (1.0) - w - v;
    if (u < -ME_SMALL_EPSILON)
    return 0;

    return 1;
}

/**
 * everything is in world c.s.
 */
bool
IxTriangleLineSegment(const lsVec3 &v0, const lsVec3 &v1, const lsVec3 &v2,
              const lsVec3 &normalTri,
              MeReal *const inOrig, MeReal *const inDest,
              const lsVec3 &dirLineSeg, const MeReal dirLineSegNorm,
              MeReal &t, McdLineSegIntersectResult * info)
{
    MCD_CHECK_ASSERT_(inOrig != NULL && inDest != NULL, "IxTriangleLineSegment");

    int       i, j;
    MeReal    nd = normalTri.dot(v0);
    MeReal    den = dirLineSeg.dot(normalTri);

    // LineSegment is parallel to the tri's plane. need further treatment
    if (MeFabs(den) < ME_SMALL_EPSILON)
    return 0;

    MeReal    tCurrent = -(normalTri.dot(*((lsVec3 *) inOrig)) - nd) / den;

    if (tCurrent > t || tCurrent < 0 || tCurrent > 1)
    return 0;

    lsVec3    isectPt = (*((lsVec3 *) inOrig) + tCurrent * dirLineSeg);

    getProjectedIndex(normalTri, i, j);

    if (!isInsideTri(v0, v1, v2, isectPt, i, j))
    return 0;

    t = tCurrent;

    isectPt.getValue(info->position);
    normalTri.getValue(info->normal);
    info->distance = t * dirLineSegNorm;

    return 1;
}

/**
 *
 */
void
IxNodeLineSegment(BVNode * node, McdModelID inModel, MeReal *inOrig,
          MeReal *inDest, const lsVec3 *origInModel,
          const lsVec3 *destInModel,
          const lsVec3 &dirInWorld, const MeReal dirNorm, MeReal &t,
          McdLineSegIntersectResult * resl)
{
    BVTransform *tmNode = &(node->mOBB.mTm);
    lsVec3   *extBox = &(node->mOBB.mBox.ext);

    lsVec3    origInNode, destInNode;
    tmNode->inverseTransform(*origInModel, &origInNode);
    tmNode->inverseTransform(*destInModel, &destInNode);

    if (!IxLineSegmentCenteredAABB(*extBox, origInNode, destInNode)) {
    return;
    }

    if (node->isLeaf()) {
    int       triId = node->mTriId;
    lsVec3   *v[3], vInW[3], *n, nInWorld;

    CxTriangleMesh *mesh = (CxTriangleMesh *) McdModelGetGeometry(inModel);

    CxTriangleMeshGetTriangleVertexPtrs(mesh, triId, &v[0], &v[1], &v[2]);
    CxTriangleMeshGetTriangleNormalPtr(mesh, triId, &n);

    lsTransform *tm = (lsTransform *) McdModelGetTransformPtr(inModel);
    tm->transformWithoutTranslate(*n, &nInWorld);

    tm->transform(*v[0], &vInW[0]);
    tm->transform(*v[1], &vInW[1]);
    tm->transform(*v[2], &vInW[2]);

    IxTriangleLineSegment(vInW[0], vInW[1], vInW[2], nInWorld,
                  inOrig, inDest, dirInWorld, dirNorm, t, resl);

    return;
    }

    IxNodeLineSegment(node->getLeftNode(), inModel, inOrig, inDest,
              origInModel, destInModel, dirInWorld, dirNorm, t, resl);

    IxNodeLineSegment(node->getRightNode(), inModel, inOrig, inDest,
              origInModel, destInModel, dirInWorld, dirNorm, t, resl);
}

/**
 *
 */
int       MEAPI
IxTriangleMeshLineSegment(const McdModelID inModel,
              MeReal *const inOrig, MeReal *const inDest,
              McdLineSegIntersectResult * resl)
{
    CxTriangleMesh *mesh = (CxTriangleMesh *) McdModelGetGeometry(inModel);
    BVTree   *bvt = (BVTree *) (((CxTriangleMesh_ *) mesh)->m_bvt);
    lsTransform *tm = (lsTransform *) McdModelGetTransformPtr(inModel);

    lsVec3    origInModel, destInModel;
    tm->inverseTransform(*((lsVec3 *) inOrig), &origInModel);
    tm->inverseTransform(*((lsVec3 *) inDest), &destInModel);

    MeReal    t = (MeReal) 2.0;
    lsVec3    dirInWorld = *((lsVec3 *) inDest) - *((lsVec3 *) inOrig);
    MeReal    dirNorm = dirInWorld.norm();

    IxNodeLineSegment(bvt->getRootNodePtr(), inModel, inOrig, inDest,
              &origInModel, &destInModel, dirInWorld, dirNorm, t, resl);

    return (t <= 1);
}
