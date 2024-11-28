/* -*-c++-*-
 *===============================================================
 * File:        IxTriangleMeshTriangleMesh.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.49.2.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */
#include <stdio.h>
#include <McdInteractionTable.h>
#include <McdGeometryTypes.h>
#include <McdTriangleMesh.h>

#include "BVTree.h"
#include "ObbObb.h"
#include "TimeStamp.h"
#include "TriTriIsctTest.h"
#include "CxSODistTriTri.h"
#include "CxTriangleMesh.h"
#include "IxTriangleMeshLineSegment.h"

/*---------------------------------------------------------------------------*/
bool
TriangleMeshTriangleMeshFn(McdIntersectResult *result, int numContactsReq,
               McdModelID cmMesh1, McdModelID cmMesh2,
               const MeReal eps, void *cachedData);

MCD_IMPLEMENT_INTERSECT_REGISTRATION(TriangleMesh, TriangleMesh,1)

/*---------------------------------------------------------------------------*/
const int MAX_NUM_CONTACTS = 30;

typedef struct CullInfo {
    MeReal    d[MAX_NUM_CONTACTS + 1][MAX_NUM_CONTACTS + 1];
    MeReal    sum[MAX_NUM_CONTACTS + 1];
    int       tn[MAX_NUM_CONTACTS + 1];
    int       numContactsReq;
} CullInfo;

unsigned int
IxBVTreeBVTree(McdIntersectResult *result, const unsigned long &ixTime,
           CullInfo * ci, const BVTree * bvt1, const BVTree * bvt2,
           const BVTransform * btm1, const BVTransform * btm2, bool bUseTrace);

unsigned int
IxBVTreeBVTree_touch(BVTree * bvt1, BVTree * bvt2, BVTransform * btm1, BVTransform * btm2);

/*---------------------------------------------------------------------------*/

//  #define GEOM_ISECT  // do not generate contacts.
#undef GEOM_ISECT

#ifdef GEOM_ISECT
typedef struct CxTraceElem {
    MeVector3 p[2];
} CxTraceElem;

typedef struct CxTrace {
    int       maxNum;
    int       numElems;
    CxTraceElem *theArray;
} CxTrace;
#endif              // GEOM_ISECT

#ifdef LICENSE_CHECK
#define PROTECTED_FEATURE FEATURE_MCD
/*#define MEFLEX_EXPORT_VARIABLES*/
#include "MeLicenseHeader.c"
#endif

int       MEAPI
McdTriangleMeshTriangleMeshIntersect(McdModelPair *p, McdIntersectResult *result)
{

    #ifdef LICENSE_CHECK
    #include "MeLicenseVars.c"
    #endif

    const MeReal eps = McdModelGetContactTolerance(p->model1)
    + McdModelGetContactTolerance(p->model2);
    int       rc = 0;

    int       numContactsReq = p->request->contactMaxCount;

    #ifdef LICENSE_CHECK
    #include "MeLicensePreFeature.c"
    #endif

    rc = TriangleMeshTriangleMeshFn(result, numContactsReq,
                                    p->model1, p->model2, eps,
                                    p->m_cachedData);

#if defined(LICENSE_CHECK) && !defined(PS2)
    /*
      Nasty business to screw over the output
      if there wasn't any license checked out
    */
    if (FAIL_VAR(FEATURE_MCD)) {
        for (int count = 0; count < result->contactCount; count++) {
          /* "Random" jumbling of contact info to circumvent people no-opping the next statement */
          result->contacts[count].position[count%3]   -= result->contacts[count].normal[(count+1)%3];
          result->contacts[count].normal[(count+2)%3] -= result->contacts[count].position[(count+9)%3];

          result->contactCount = 0;
      }
    }

#endif /* defined(LICENSE_CHECK) && !defined(PS2) */

    return(rc);
}

static McdModelID gcmMesh1, gcmMesh2;

/**
 *
 */
bool
TriangleMeshTriangleMeshFn(McdIntersectResult *result,
               int inNumContactsReq, McdModelID cmMesh1, McdModelID cmMesh2,
               const MeReal eps, void *cachedData)
{
    int       i;
    static TimeStamp isectTime;
    bool      bUseTrace;
    McdContact *c;

    gcmMesh1 = cmMesh1;
    gcmMesh2 = cmMesh2;

    CxTriangleMesh_ *mesh1 = (CxTriangleMesh_ *) McdModelGetGeometry(cmMesh1);
    CxTriangleMesh_ *mesh2 = (CxTriangleMesh_ *) McdModelGetGeometry(cmMesh2);

    MCD_CHECK_ASSERT(CxTriangleMeshIsBuilt((McdGeometry *) mesh1),
             kMcdErrorNum_TriangleMeshNotBuilt, "", "TriangleMeshTriangleMeshFn");

    MCD_CHECK_ASSERT(CxTriangleMeshIsBuilt((McdGeometry *) mesh2),
             kMcdErrorNum_TriangleMeshNotBuilt, "", "TriangleMeshTriangleMeshFn");

    BVTree   *bvt1 = (BVTree *) mesh1->m_bvt;
    BVTree   *bvt2 = (BVTree *) mesh2->m_bvt;

    lsTransform *tm1 = (lsTransform *) McdModelGetTransformPtr(cmMesh1);
    lsTransform *tm2 = (lsTransform *) McdModelGetTransformPtr(cmMesh2);

    int       numTris1 = CxTriangleMeshGetTriangleCount((McdGeometry *) mesh1);
    int       numTris2 = CxTriangleMeshGetTriangleCount((McdGeometry *) mesh2);

    isectTime.advance();
    unsigned long ixTime = isectTime.getTime();

    BVTransform btm1(tm1);
    BVTransform btm2(tm2);

    // temporary solution: if info->contactMaxCount == 0, "touch" request is assumed.
    if (result->contactMaxCount == 0) {

    int       touch = IxBVTreeBVTree_touch(bvt1, bvt2, &btm1, &btm2);
    result->contactCount = 0;
    result->touch = touch;
    return false;
    }

    CullInfo  ci;

    ci.numContactsReq = inNumContactsReq;

    for (i = 0; i < MAX_NUM_CONTACTS + 1; i++)
    ci.tn[i] = 1;

    bUseTrace = false;
    if ( (numTris1+numTris2) < 120 && (numTris1 < 20 || numTris2 < 20))
    bUseTrace = true;

    result->contactCount = 0;
    IxBVTreeBVTree(result, ixTime, &ci, bvt1, bvt2, &btm1, &btm2, bUseTrace);

    if (!bUseTrace) {
    for (i = 0; i < result->contactCount; i++) {
        c = result->contacts + i;
        lsVec3   *n = (lsVec3 *) c->normal;
        n->normalize();
        c->separation /= (MeReal) ci.tn[i];
    }
    }

    result->touch = result->contactCount > 0 ? 1 : 0;

    return (result->contactCount > 0);
}

/**
 *
 */
inline int
SmallestElemIndex(MeReal *v, int n)
{
    int       ir = 0;
    MeReal    elem = v[0];
    for (int i = 1; i <= n; i++) {
    if (v[i] < elem) {
        elem = v[i];
        ir = i;
    }
    }
    return ir;
}

/**
 *
 */
inline int
ClosestIndex(MeReal d[][MAX_NUM_CONTACTS + 1], int k, int last_id)
{
    MeReal    low = (MeReal) 1.0E20;
    int       cid = 0;
    for (int i = 0; i <= last_id; i++) {
    if (i != k)
        if (d[k][i] < low)
        cid = i;
    }
    return cid;
}

/**
 *
 */
int
CullContacts(McdIntersectResult *rel, CullInfo * cinfo, int num)
{
    int       i, j, k, m;
    int       last_id = rel->contactCount;
    lsVec3    diff;

    McdContact *newc = rel->contacts + last_id;
    McdContact *ci, *cj;

    for (i = 0; i < last_id; i++) {
    ci = rel->contacts + i;
    diff = *((lsVec3 *) (ci->position)) - *((lsVec3 *) (newc->position));
    if (diff.square_norm() < 0.01) {    // 0.01 should be relative to something
        ci->normal[0] += newc->normal[0];
        ci->normal[1] += newc->normal[1];
        ci->normal[2] += newc->normal[2];
        return 0;
    }
    }

    if (last_id < num) {
    rel->contactCount++;
    return 0;
    }

    for (i = 0; i <= last_id; i++) {
    ci = rel->contacts + i;
    for (j = 0; j <= last_id; j++) {
        cj = rel->contacts + j;
        diff = *((lsVec3 *) ci->position) - *((lsVec3 *) cj->position);
        cinfo->d[i][j] = diff.square_norm();
        cinfo->d[j][i] = cinfo->d[i][j];
    }
    }

    // to be optimized
    for (i = 0; i <= last_id; i++) {
    cinfo->sum[i] = (MeReal) 0.0;
    for (j = 0; j <= last_id; j++) {
        cinfo->sum[i] += cinfo->d[i][j];
    }
    }

    // (1) k should merge with another contacts
    // (2) last_id will take k's place
    k = SmallestElemIndex(cinfo->sum, last_id);

    // k will merge with m
    m = ClosestIndex(cinfo->d, k, last_id);
    cinfo->tn[m]++;
    ci = rel->contacts + m;
    cj = rel->contacts + k;

    // merge
    ci->position[0] = (ci->position[0] + cj->position[0]) * (MeReal) 0.5;
    ci->position[1] = (ci->position[1] + cj->position[1]) * (MeReal) 0.5;
    ci->position[2] = (ci->position[2] + cj->position[2]) * (MeReal) 0.5;
    ci->normal[0] += cj->normal[0];
    ci->normal[1] += cj->normal[1];
    ci->normal[2] += cj->normal[2];
    ci->separation += cj->separation;
    if (m == last_id)
    ci->separation *= (MeReal) 0.50;

    // replace
    if (k != last_id) {
    cj = rel->contacts + k;
    cj->position[0] = newc->position[0];
    cj->position[1] = newc->position[1];
    cj->position[2] = newc->position[2];
    cj->normal[0] = newc->normal[0];
    cj->normal[1] = newc->normal[1];
    cj->normal[2] = newc->normal[2];
    cj->separation = newc->separation;
    }

    return 1;
}

/**
 *
 */
inline int
minInt(int i1, int i2)
{
    if (i1 < i2)
    return i1;
    else
    return i2;
}

/**
 *
 */
inline unsigned int
CollideTriTriOneNormal(McdIntersectResult *result,
               const unsigned long &ixTime, CullInfo * ci,
               BVNode * node1, const BVTree * bvt1, const BVTransform * btm1,
               BVNode * node2, const BVTree * bvt2, const BVTransform * btm2)
{
    if (separation_rect_rect(node1->mOBB, *btm1, node2->mOBB, *btm2))
    return 0;

    int       numPts;
    unsigned int resl_isect = 0;
    lsVec3   *v[3], v1InW[3], v2InW[3], isectPts[6];

    CxTriangleMesh *mesh1 = bvt1->mMesh;
    CxTriangleMesh *mesh2 = bvt2->mMesh;
    int       triId1 = node1->mTriId;
    int       triId2 = node2->mTriId;

    // if we cash these world coods, for later use, we have a big saving
    // for deep penetration collision
    CxTriangleMeshGetTriangleVertexPtrs(mesh1, triId1, &v[0], &v[1], &v[2]);
    btm1->transform(*v[0], &v1InW[0]);
    btm1->transform(*v[1], &v1InW[1]);
    btm1->transform(*v[2], &v1InW[2]);

    CxTriangleMeshGetTriangleVertexPtrs(mesh2, triId2, &v[0], &v[1], &v[2]);
    btm2->transform(*v[0], &v2InW[0]);
    btm2->transform(*v[1], &v2InW[1]);
    btm2->transform(*v[2], &v2InW[2]);

    resl_isect = TriTriIsect(v1InW[0].v, v1InW[1].v, v1InW[2].v,
                 v2InW[0].v, v2InW[1].v, v2InW[2].v, isectPts, 6, numPts);

    if (!resl_isect)
    return 0;

#ifndef GEOM_ISECT
    // direction from #2 to #1

    lsVec3   *n1, *n2, n1w, n2w, n;

    CxTriangleMeshGetTriangleNormalPtr(mesh1, triId1, &n1);
    btm1->transformWithoutTranslate(*n1, &n1w);

    CxTriangleMeshGetTriangleNormalPtr(mesh2, triId2, &n2);
    btm2->transformWithoutTranslate(*n2, &n2w);

    if (resl_isect == INTERSECT_1)
    n = n2w;
    else if (resl_isect == INTERSECT_2)
    n = -n1w;
    else {
    MeReal    area1 = CxTriangleMeshGetTriangleArea(mesh1, triId1);
    MeReal    area2 = CxTriangleMeshGetTriangleArea(mesh2, triId2);
    if (n2w.dot(n1w) >= 0)
        n = area2 * n2w + area1 * n1w;
    else
        n = area2 * n2w - area1 * n1w;
    if (n.normalize(ME_SMALL_EPSILON) < ME_SMALL_EPSILON)
        n = n2w;
    }

    CxSODistTriTri SDTT(&v1InW[0], &v1InW[1], &v1InW[2],
            &v2InW[0], &v2InW[1], &v2InW[2], &n1w, &n2w, &n);
    SDTT.process();
    MeReal    dist = SDTT.getDist();

    if (numPts == 2) {
    for (int i = 0; i < 2; i++) {
        if (result->contactCount < result->contactMaxCount &&
        result->contactCount < MAX_NUM_CONTACTS) {
        McdContact *c = result->contacts + result->contactCount;
        (isectPts[i]).getValue(c->position);
        n.getValue(c->normal);
        c->separation = dist > 0 ? 0 : dist;

        // Cullresult is responsible for increment of result->contactCount
        CullContacts(result, ci, minInt(ci->numContactsReq, 4));
        }
    }
    } else {
    // co-planar case
    lsVec3   *n2, n2w;
    CxTriangleMeshGetTriangleNormalPtr(mesh2, triId2, &n2);
    btm1->transformWithoutTranslate(*n2, &n2w);
    for (int i = 0; i < numPts; i++) {
        if (result->contactCount < result->contactMaxCount &&
        result->contactCount < MAX_NUM_CONTACTS) {
        McdContact *c = result->contacts + result->contactCount;
        isectPts[i].getValue(c->position);
        n2w.getValue(c->normal);
        c->separation = 0;
        CullContacts(result, ci, minInt(ci->numContactsReq, 4));
        }
    }
    }

#else
    ((CxTriangleMesh_ *) mesh1)->m_auxData[triId1] = 0;
    ((CxTriangleMesh_ *) mesh2)->m_auxData[triId2] = 0;

    CxTrace  *trace = (CxTrace *) result->data;
    if (resl_isect == INTERSECT) {
    if (trace && trace->maxNum > trace->numElems) {
        CxTraceElem *te = &(trace->theArray[trace->numElems]);
        te->p[0][0] = isectPts[0][0];
        te->p[0][1] = isectPts[0][1];
        te->p[0][2] = isectPts[0][2];
        te->p[1][0] = isectPts[1][0];
        te->p[1][1] = isectPts[1][1];
        te->p[1][2] = isectPts[1][2];
        trace->numElems++;
    }
    }

    if (resl_isect == INTERSECT_COPLANAR) {
    for (int i = 0; i < numPts; i++) {
        if (trace && trace->maxNum > trace->numElems) {
        CxTraceElem *te = &(trace->theArray[trace->numElems]);
        te->p[0][0] = isectPts[i][0];
        te->p[0][1] = isectPts[i][1];
        te->p[0][2] = isectPts[i][2];
        int       j = (i + 1) % numPts;
        te->p[1][0] = isectPts[j][0];
        te->p[1][1] = isectPts[j][1];
        te->p[1][2] = isectPts[j][2];
        trace->numElems++;
        }
    }
    }
#endif

    return 1;
}

/**
 *
 */
inline unsigned int
CollideTriTri(McdIntersectResult *result, const unsigned long &ixTime,
          CullInfo * ci,
          BVNode * node1, const BVTree * bvt1, const BVTransform * btm1,
          BVNode * node2, const BVTree * bvt2, const BVTransform * btm2)
{
    if (separation_rect_rect(node1->mOBB, *btm1, node2->mOBB, *btm2))
    return 0;

    int       numPts;
    unsigned int resl_isect = 0;
    lsVec3   *v[3], v1InW[3], v2InW[3], isectPts[6];

    CxTriangleMesh *mesh1 = bvt1->mMesh;
    CxTriangleMesh *mesh2 = bvt2->mMesh;
    int       triId1 = node1->mTriId;
    int       triId2 = node2->mTriId;

    // if we cash these world coods, for later use, we have a big saving
    // for deep penetration collision
    CxTriangleMeshGetTriangleVertexPtrs(mesh1, triId1, &v[0], &v[1], &v[2]);
    btm1->transform(*v[0], &v1InW[0]);
    btm1->transform(*v[1], &v1InW[1]);
    btm1->transform(*v[2], &v1InW[2]);

    CxTriangleMeshGetTriangleVertexPtrs(mesh2, triId2, &v[0], &v[1], &v[2]);
    btm2->transform(*v[0], &v2InW[0]);
    btm2->transform(*v[1], &v2InW[1]);
    btm2->transform(*v[2], &v2InW[2]);

    resl_isect = TriTriIsect(v1InW[0].v, v1InW[1].v, v1InW[2].v,
                 v2InW[0].v, v2InW[1].v, v2InW[2].v, isectPts, 6, numPts);

    if (!resl_isect)
    return 0;

#ifndef GEOM_ISECT
    // direction from #2 to #1

    lsVec3   *n1, *n2, n1w, n2w, n;

    if (numPts == 2) {
    CxTriangleMeshGetTriangleNormalPtr(mesh1, triId1, &n1);
    btm1->transformWithoutTranslate(*n1, &n1w);

    CxTriangleMeshGetTriangleNormalPtr(mesh2, triId2, &n2);
    btm2->transformWithoutTranslate(*n2, &n2w);

    if (resl_isect == INTERSECT_1)
        n = n2w;
    else if (resl_isect == INTERSECT_2)
        n = -n1w;
    else {
        MeReal    area1 = CxTriangleMeshGetTriangleArea(mesh1, triId1);
        MeReal    area2 = CxTriangleMeshGetTriangleArea(mesh2, triId2);
        if (n2w.dot(n1w) >= 0)
        n = area2 * n2w + area1 * n1w;
        else
        n = area2 * n2w - area1 * n1w;
        if (n.normalize(ME_SMALL_EPSILON) < ME_SMALL_EPSILON)
        n = n2w;
    }

    CxSODistTriTri SDTT(&v1InW[0], &v1InW[1], &v1InW[2],
                &v2InW[0], &v2InW[1], &v2InW[2], &n1w, &n2w, &n);
    SDTT.process();
    MeReal    dist = SDTT.getDist();

    for (int i = 0; i < 2; i++) {
        if (result->contactCount < result->contactMaxCount) {
        McdContact *c = result->contacts + result->contactCount;
        lsVec3    p = isectPts[i];
        p.getValue(c->position);
        n.getValue(c->normal);
        c->separation = dist > 0 ? 0 : dist;

        // CullInfo is responsible for increment of result->contactCount
        CullContacts(result, ci, minInt(ci->numContactsReq, MAX_NUM_CONTACTS));
        }
    }

    } else {
    // co-planar case
    CxTriangleMeshGetTriangleNormalPtr(mesh2, triId2, &n2);
    btm1->transformWithoutTranslate(*n2, &n2w);
    for (int i = 0; i < numPts; i++) {
        if (result->contactCount < result->contactMaxCount) {
        McdContact *c = result->contacts + result->contactCount;
        isectPts[i].getValue(c->position);
        n2w.getValue(c->normal);
        c->separation = 0;
        CullContacts(result, ci, minInt(ci->numContactsReq, MAX_NUM_CONTACTS));
        }
    }
    }

#else
    ((CxTriangleMesh_ *) mesh1)->m_auxData[triId1] = 0;
    ((CxTriangleMesh_ *) mesh2)->m_auxData[triId2] = 0;

    CxTrace  *trace = (CxTrace *) result->data;
    if (resl_isect == INTERSECT) {
    if (trace && trace->maxNum > trace->numElems) {
        CxTraceElem *te = &(trace->theArray[trace->numElems]);
        te->p[0][0] = isectPts[0][0];
        te->p[0][1] = isectPts[0][1];
        te->p[0][2] = isectPts[0][2];
        te->p[1][0] = isectPts[1][0];
        te->p[1][1] = isectPts[1][1];
        te->p[1][2] = isectPts[1][2];
        trace->numElems++;
    }
    }

    if (resl_isect == INTERSECT_COPLANAR) {
    for (int i = 0; i < numPts; i++) {
        if (trace && trace->maxNum > trace->numElems) {
        CxTraceElem *te = &(trace->theArray[trace->numElems]);
        te->p[0][0] = isectPts[i][0];
        te->p[0][1] = isectPts[i][1];
        te->p[0][2] = isectPts[i][2];
        int       j = (i + 1) % numPts;
        te->p[1][0] = isectPts[j][0];
        te->p[1][1] = isectPts[j][1];
        te->p[1][2] = isectPts[j][2];
        trace->numElems++;
        }
    }
    }
#endif
    return 1;

}

/**
 *
 */
unsigned int
CollideNode(McdIntersectResult *result, const unsigned long &ixTime,
        CullInfo * ci,
        BVNode * node1, const BVTree * bvt1, const BVTransform * btm1,
        BVNode * node2, const BVTree * bvt2, const BVTransform * btm2, bool bUseTrace)
{
#ifdef CXTRIANGLE_USE_TIMESTAMP
    if (node1->mOBB.mStatus < ixTime) {
    node1->mOBB.mStatus = ixTime;
    node1->mOBB.mTmGlobal.thisIsFirstThenSecond(node1->mOBB.mTm, *btm1);
    }

    if (node2->mOBB.mStatus < ixTime) {
    node2->mOBB.mStatus = ixTime;
    node2->mOBB.mTmGlobal.thisIsFirstThenSecond(node2->mOBB.mTm, *btm2);
    }
#endif

    if (node1->isLeaf() && node2->isLeaf()) {

    CxTriangleMesh *mesh1 = bvt1->mMesh;
    CxTriangleMesh *mesh2 = bvt2->mMesh;
    int       numTris1, numTris2;

    numTris1 = CxTriangleMeshGetTriangleCount(mesh1);
    numTris2 = CxTriangleMeshGetTriangleCount(mesh2);

    if (bUseTrace) {
        CollideTriTriOneNormal(result, ixTime, ci, node1, bvt1, btm1, node2, bvt2, btm2);
    } else {
        CollideTriTri(result, ixTime, ci, node1, bvt1, btm1, node2, bvt2, btm2);
    }

    } else if (!node1->isLeaf() && !node2->isLeaf()) {

    if (separation(node1->mOBB, *btm1, node2->mOBB, *btm2))
        return 0;

    if (node1->getObbSize() > node2->getObbSize()) {

        CollideNode(result, ixTime, ci,
            node1->getLeftNode(), bvt1, btm1, node2, bvt2, btm2, bUseTrace);

        CollideNode(result, ixTime, ci,
            node1->getRightNode(), bvt1, btm1, node2, bvt2, btm2, bUseTrace);

    } else {

        CollideNode(result, ixTime, ci,
            node1, bvt1, btm1, node2->getLeftNode(), bvt2, btm2, bUseTrace);

        CollideNode(result, ixTime, ci,
            node1, bvt1, btm1, node2->getRightNode(), bvt2, btm2, bUseTrace);
    }

    } else if (node1->isLeaf() && !node2->isLeaf()) {

    if (separation_rect_box(node1->mOBB, *btm1, node2->mOBB, *btm2))
        return 0;

    CollideNode(result, ixTime, ci,
            node1, bvt1, btm1, node2->getLeftNode(), bvt2, btm2, bUseTrace);

    CollideNode(result, ixTime, ci,
            node1, bvt1, btm1, node2->getRightNode(), bvt2, btm2, bUseTrace);

    } else if (!node1->isLeaf() && node2->isLeaf()) {

    if (separation_rect_box(node2->mOBB, *btm2, node1->mOBB, *btm1))
        return 0;

    CollideNode(result, ixTime, ci,
            node1->getLeftNode(), bvt1, btm1, node2, bvt2, btm2, bUseTrace);

    CollideNode(result, ixTime, ci,
            node1->getRightNode(), bvt1, btm1, node2, bvt2, btm2, bUseTrace);
    }

    return 0;
}

/**
 *
 */
int
CompteNormalBasedOnTrace(McdIntersectResult *result, CullInfo * cinfo,
             const BVTree * bvt1, const BVTree * bvt2,
             const BVTransform * btm1, const BVTransform * btm2)
{
    if (result->contactCount == 0)
    return 0;

    int       i, imax = -1, num = result->contactCount;
    McdContact *c0 = result->contacts, *ci, *ci1, *ci2;

    if (num <= 2) {
    for (i = 0; i < num; i++) {
        ((lsVec3 *) ((c0 + i)->normal))->normalize();
    }
    return 1;
    }

    lsVec3    v, e0, e1, ni[MAX_NUM_CONTACTS], n(0, 0, 0);
    MeReal    fnorm, fmax = ME_SMALL_EPSILON;

    for (i = 0; i < num - 1; i++) {
    ci = c0 + i;
    ci1 = ci + 1;
    ci2 = c0 + (i + 2) % num;
    e0 = *((lsVec3 *) ((ci1)->position)) - *((lsVec3 *) (ci->position));
    e1 = *((lsVec3 *) ((ci2)->position)) - *((lsVec3 *) (ci->position));
    ni[i] = e1.cross(e0);
    fnorm = ni[i].square_norm();
    if (fnorm > fmax) {
        fmax = fnorm;
        imax = i;
    }
    }

    if (imax == -1) {
    result->contactCount = 0;
    return 0;
    }

    const lsVec3 *center1_local, *center2_local;
    lsVec3    center1, center2;

    center1_local = (lsVec3 *) (((CxTriangleMesh_ *) (bvt1->mMesh))->m_AABB_center);
    center2_local = (lsVec3 *) (((CxTriangleMesh_ *) (bvt2->mMesh))->m_AABB_center);

    btm1->transform(*center1_local, &center1);
    btm2->transform(*center1_local, &center2);

    for (i = 0; i < num - 1; i++) {
    if (ni[imax].dot(ni[i]) > 0)
        n += ni[i];
    else
        n -= ni[i];
    }

    n.normalize();
    v = center1 - center2;
    if (v.dot(n) < 0)
    n = -n;

    lsVec3    pc(0, 0, 0);

    for (i = 0; i < num; i++) {
    pc += *((lsVec3 *) ((c0 + i)->position));
    }

    pc /= (MeReal) num;

    McdLineSegIntersectResult resl;
    lsVec3    porig, pdest;

    porig = pc - (MeReal) 0.1 *n;
    pdest = pc + (MeReal) 0.9 *n;

    MeReal    d1 = 0;
    MeReal    d2 = 0;

    if (IxTriangleMeshLineSegment(gcmMesh2, porig.v, pdest.v, &resl))
    d2 = resl.distance;

    porig = pc + (MeReal) 0.1 *n;
    pdest = pc - (MeReal) 0.9 *n;

    if (IxTriangleMeshLineSegment(gcmMesh1, porig.v, pdest.v, &resl))
    d1 = resl.distance;

    MeReal    sep = -(d1 + d2 - 0.2f);
    sep = sep > 0 ? 0 : sep;

    // printf("sep: %f\n", sep);

    for (i = 0; i < num; i++) {
    ci = c0 + i;
    n.getValue(ci->normal);
    (ci)->separation = sep;
    }

    return 1;
}

/**
 *
 */
unsigned int
IxBVTreeBVTree(McdIntersectResult *result, const unsigned long &ixTime,
           CullInfo * ci, const BVTree * bvt1, const BVTree * bvt2,
           const BVTransform * btm1, const BVTransform * btm2, bool bUseTrace)
{

    CxTriangleMesh *mesh1 = bvt1->mMesh;
    CxTriangleMesh *mesh2 = bvt2->mMesh;

#ifdef GEOM_ISECT
    int       i;

    for (i = 0; i < CxTriangleMeshGetTriangleCount(mesh1); i++) {
    ((CxTriangleMesh_ *) mesh1)->m_auxData[i] = 1;
    }

    for (i = 0; i < CxTriangleMeshGetTriangleCount(mesh2); i++) {
    ((CxTriangleMesh_ *) mesh2)->m_auxData[i] = 1;
    }
#endif

    CollideNode(result, ixTime, ci, bvt1->getRootNodePtr(), bvt1, btm1,
        bvt2->getRootNodePtr(), bvt2, btm2, bUseTrace);

    if (bUseTrace) {
    return CompteNormalBasedOnTrace(result, ci, bvt1, bvt2, btm1, btm2);
    }

    return 1;
}

/**
 *
 */
unsigned int
CollideNode_touch(BVNode * node1, BVTree * bvt1, BVTransform * btm1,
          BVNode * node2, BVTree * bvt2, BVTransform * btm2, int &isect)
{
    unsigned int ix;

    if (node1->isLeaf() && node2->isLeaf()) {

    ix = separation(node1->mOBB, *btm1, node2->mOBB, *btm2);

    if (ix == 0) {      // obb-obb intersects

        CxTriangleMesh *mesh1 = bvt1->mMesh;
        CxTriangleMesh *mesh2 = bvt2->mMesh;
        int       triId1 = node1->mTriId;
        int       triId2 = node2->mTriId;

        lsVec3   *v[3];
        lsVec3    v1InW[3], v2InW[3];

        // if we cash these world coods, for later use, we have a big saving
        // for deep penetration collision
        CxTriangleMeshGetTriangleVertexPtrs(mesh1, triId1, &v[0], &v[1], &v[2]);
        btm1->transform(*v[0], &v1InW[0]);
        btm1->transform(*v[1], &v1InW[1]);
        btm1->transform(*v[2], &v1InW[2]);

        CxTriangleMeshGetTriangleVertexPtrs(mesh2, triId2, &v[0], &v[1], &v[2]);
        btm2->transform(*v[0], &v2InW[0]);
        btm2->transform(*v[1], &v2InW[1]);
        btm2->transform(*v[2], &v2InW[2]);

        if (TriTriIsect(v1InW[0].v, v1InW[1].v, v1InW[2].v, v2InW[0].v, v2InW[1].v, v2InW[2].v)) {
        isect++;
        return 1;
        }
    }

    return 0;
    }
    // ix = 0 -> INTERSECTION
    if (node1->isLeaf() && !node2->isLeaf()) {
    ix = separation_rect_box(node1->mOBB, *btm1, node2->mOBB, *btm2);
    if (!isect && ix == 0) {

        CollideNode_touch(node1, bvt1, btm1, node2->getLeftNode(), bvt2, btm2, isect);

        if (!isect)
        CollideNode_touch(node1, bvt1, btm1, node2->getRightNode(), bvt2, btm2, isect);
    }
    } else if (!node1->isLeaf() && node2->isLeaf()) {
    ix = separation_rect_box(node2->mOBB, *btm2, node1->mOBB, *btm1);
    if (!isect && ix == 0) {

        CollideNode_touch(node1->getLeftNode(), bvt1, btm1, node2, bvt2, btm2, isect);

        if (!isect)
        CollideNode_touch(node1->getRightNode(), bvt1, btm1, node2, bvt2, btm2, isect);
    }
    } else if (!node1->isLeaf() && !node2->isLeaf()) {
    ix = separation(node1->mOBB, *btm1, node2->mOBB, *btm2);
    if (!isect && ix == 0) {

        CollideNode_touch(node1->getLeftNode(), bvt1, btm1, node2, bvt2, btm2, isect);

        if (!isect)
        CollideNode_touch(node1->getRightNode(), bvt1, btm1, node2, bvt2, btm2, isect);
    }
    }

    return 0;
}

/**
 *
 */
unsigned int
IxBVTreeBVTree_touch(BVTree * bvt1, BVTree * bvt2, BVTransform * btm1, BVTransform * btm2)
{
    int       num_intersect = 0;
    CollideNode_touch(bvt1->getRootNodePtr(), bvt1, btm1,
              bvt2->getRootNodePtr(), bvt2, btm2, num_intersect);

    return (num_intersect > 0);
}

/**
 *
 */
void
printTree(BVNode * node, const unsigned long &ixTime, const BVTransform * btm)
{
    // node->mOBB.mTmGlobal.thisIsFirstThenSecond( node->mOBB.mTm, *btm );
    // if ( node->mOBB.mStatus == ixTime) printf("jfsjkfsklfklsfklsgj\n");
    if (!node->isLeaf()) {
    printTree(node->getLeftNode(), ixTime, btm);
    printTree(node->getRightNode(), ixTime, btm);
    }
}
