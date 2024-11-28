/* -*-c++-*-
 *===============================================================
 * File:        MeshMeshDist.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.28.2.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#define METHOD_USING_QUEUE
// #undef METHOD_USING_QUEUE
#ifdef  METHOD_USING_QUEUE

#include <stdio.h>
#include <MePrecision.h>
#include <McdModel.h>
#include <McdTriangleMesh.h>
#include <McdModelPair.h>
#include <McdCTypes.h>

#include "CxTriangleMesh.h"
#include "BVTree.h"
#include "ObbObb.h"
#include "RectRectDist.h"
#include "TriTriDist.h"

#include <queue>

#include "TriTriDist.h"
#include "SSR.h"

static struct McdMeshMeshDistDataType{
  CxTriangleMesh* m0;
//  BVTransform *btm0;
  CxTriangleMesh* m1;
//  BVTransform *btm1;
  BVTransform m2To1Global_distance;
  McdMeshMeshDistDataType() {};

}  McdMeshMeshDistData;


inline MeReal DistBVDNodeLeafLeaf(CxTriangleMesh* m0, CxTriangleMesh* m1,
               const BVNode* node0, const BVNode* node1,
               const BVTransform *m2to1 )
{
  lsVec3 *v0[3], *v1[3], *n0, *n1;
  lsVec3 v1g[3], n0g, n1g;

  int       triId0 = node0->mTriId;
  int       triId1 = node1->mTriId;

  CxTriangleMeshGetTriangleVertexPtrs(m0, triId0, &v0[0], &v0[1], &v0[2]);
  CxTriangleMeshGetTriangleVertexPtrs(m1, triId1, &v1[0], &v1[1], &v1[2]);
  CxTriangleMeshGetTriangleNormalPtr(m0, triId0, &n0);
  CxTriangleMeshGetTriangleNormalPtr(m1, triId1, &n1);


  m2to1->transform(*v1[0], &v1g[0]);
  m2to1->transform(*v1[1], &v1g[1]);
  m2to1->transform(*v1[2], &v1g[2]);
  m2to1->transformWithoutTranslate(*n1, &n1g);

  TriTriDist tt( v0[0], v0[1], v0[2], n0, &v1g[0], &v1g[1], &v1g[2], &n1g);

  lsVec3 p0, p1;
  // return tt.getMinDist(&p0,&p1);
  return tt.getMinDist();
}


/////////////////////////////////////////////////////////////////////////////

inline void DistBVDNodeSSRSSR(
               CxTriangleMesh* m0, CxTriangleMesh* m1,
               const BVNode* node0, const BVNode* node1,
               const BVTransform &m1To0,
               MeReal *dist, MeReal *distUB
               )

{
  lsVec3 b, e0, e1;
  CxRectangle rect0, rect1;

  CxSSR* ssr0 = &(((BVDNode*)node0)->mSSR);
  CxSSR* ssr1 = &(((BVDNode*)node1)->mSSR);

  b.setValue(-ssr0->mExtX, -ssr0->mExtY, 0);
  e0.setValue( 2*ssr0->mExtX, 0, 0);
  e1.setValue( 0, 2*ssr0->mExtY, 0);
  rect0.setData(b, e0, e1 );

  b.setValue(-ssr1->mExtX, -ssr1->mExtY, 0);
  e0.setValue( 2*ssr1->mExtX, 0, 0);
  e1.setValue( 0, 2*ssr1->mExtY, 0);

  lsVec3 bTo0;//, e0To0, e1To0;
  m1To0.transform(b, &bTo0);
//  m1To0.transformWithoutTranslate(e0, &e0To0);
//  m1To0.transformWithoutTranslate(e1, &e1To0);
  MeReal x = (2*ssr1->mExtX);
  lsVec3 e0To0(m1To0.row[0][0]*x, m1To0.row[0][1]*x, m1To0.row[0][2]*x);
  x = (2*ssr1->mExtY);
  lsVec3 e1To0(m1To0.row[1][0]*x, m1To0.row[1][1]*x, m1To0.row[1][2]*x);

  rect1.setData(bTo0, e0To0, e1To0 );

  Rectrect rr(&rect0, &rect1);
  MeReal d = rr.getMinDist();
  MeReal rads = ssr0->mRadius + ssr1->mRadius;

  *dist =  d - (rads);
  //*distUB =  //d + rads;
   // lsMaxDistanceBoxBox( *((BVDNode*)node0), *((BVDNode*)node1), m1To0 );
  *distUB = MeSqrt(m1To0.t().square_norm()) + rads;
}


/****************************************************************************/
void
inline find_m2To1( const BVDNode* n1, const BVDNode* n2,
           const BVTransform* m2To1 , BVTransform * m2To1Root)
{
  BVTransform m2To1Temp;
  m2To1Temp.thisIsFirstThenInverseSecond( *m2To1, n1->mOBB.mTm );
  m2To1Root->thisIsFirstThenSecond( n2->mOBB.mTm, m2To1Temp );
}

inline void distanceBoxBoxLbUb(const BVDNode * left, const BVDNode * right,
   MeReal *dist, MeReal *distUb)
{

  if (left->isLeaf() && right->isLeaf()) {
      *dist = *distUb = DistBVDNodeLeafLeaf( McdMeshMeshDistData.m0,
               McdMeshMeshDistData.m1,
               left, right,
               &McdMeshMeshDistData.m2To1Global_distance );
      return;
  }

  BVTransform m2To1;
  find_m2To1(left,right,&McdMeshMeshDistData.m2To1Global_distance, &m2To1);
  DistBVDNodeSSRSSR( McdMeshMeshDistData.m0, McdMeshMeshDistData.m1,
               left, right,
               m2To1,
               dist, distUb );
}

// Fast Min Distance estimate.
// MeReal distanceBoxBoxApproxLb(BVDNode * left, BVDNode * right, const BVTransform &trans){
// }

// only for returning the two points
MeReal distanceTriTriTwoTrans(  CxTriangleMesh* m0,
                const int triId0,
                CxTriangleMesh* m1,
                const int triId1,
                const BVTransform *btm0,
                const BVTransform *btm1,
                lsVec3 *p1, lsVec3 *p2)
{
  /*
  lsTriangle leftT(*left, trans1);
  lsTriangle rightT(*right, trans2);

  return lsTriTriDist(&leftT,&rightT).getMinDist(p1,p2);
  */
    lsVec3 *v0[3], *v1[3], *n0, *n1;
    lsVec3 v0g[3], v1g[3], n0g, n1g;

    CxTriangleMeshGetTriangleVertexPtrs(m0, triId0, &v0[0], &v0[1], &v0[2]);
    CxTriangleMeshGetTriangleVertexPtrs(m1, triId1, &v1[0], &v1[1], &v1[2]);
    CxTriangleMeshGetTriangleNormalPtr(m0, triId0, &n0);
    CxTriangleMeshGetTriangleNormalPtr(m1, triId1, &n1);

    btm0->transform(*v0[0], &v0g[0]);
    btm0->transform(*v0[1], &v0g[1]);
    btm0->transform(*v0[2], &v0g[2]);
    btm1->transform(*v1[0], &v1g[0]);
    btm1->transform(*v1[1], &v1g[1]);
    btm1->transform(*v1[2], &v1g[2]);
    btm0->transformWithoutTranslate(*n0, &n0g);
    btm1->transformWithoutTranslate(*n1, &n1g);

    TriTriDist tt( &v0g[0], &v0g[1], &v0g[2], &n0g, &v1g[0], &v1g[1], &v1g[2], &n1g);

    return tt.getMinDist(p1, p2);
}


/////////////////////////////////////////////////////////////////////////////
// class BVPairDist:  for distance algorithm
// pair of BVDNode with distance

class BVPairDist
{
public:
  const BVDNode * first;
  const BVDNode * second;
  // BVTransform trans;  we use absolute transforms to root now

  //int firstDepth;
  //int secondDepth;
  int relDepth; // depth of 2 - depth of 1

private:
  MeReal _distance_lb;
  MeReal _distance_ub;

  // bool distanceComputed_lb;
  // bool distanceComputed_ub;

public:
  BVPairDist(const BVDNode *const x, const BVDNode *const y, const int rdepth
         /*const BVTransform &tt*/):
    first(x), second(y),
    relDepth(rdepth)
    // _distance_lb(__infinity),
    // _distance_ub(-__infinity),
    // distanceComputed_lb(false),
    // distanceComputed_ub(false),
    //trans(tt)
    {
      distanceBoxBoxLbUb(first, second, &_distance_lb, &_distance_ub);
    }

  /*
  MeReal distance_lb() {
    if (distanceComputed_lb) return _distance_lb;
    distanceComputed_lb = true;
    return _distance_lb = distanceBoxBoxLb(first , second, trans);
  }

  MeReal distance_ub() {
    if (distanceComputed_ub) return _distance_ub;
    distanceComputed_ub = true;
    return _distance_ub = distanceBoxBoxUb(first , second, trans);
  }
  */

  MeReal distance_lb() const { return _distance_lb; }
  MeReal distance_ub() const { return _distance_ub; }

  // for priority queue
  struct less:
  public std::binary_function<const BVPairDist&, const BVPairDist&, bool>
  {
    bool operator()(const BVPairDist& x, const BVPairDist& y) {
      return x.distance_lb() > y.distance_lb();
    }
  };

};


/////////////////////////////////////////////////////////////////////////////
// definitions for distance code

// typedef priority_queue<BVPairDist, vector<BVPairDist>, BVPairDist::less > DistQueueWithMax;

// const double almostInfinity = 1.7976931348623157e+308;
// largest double , but < __infinity
const MeReal almostInfinity = MEINFINITY;

class DistQueueWithMax : public
std::priority_queue<BVPairDist, std::vector<BVPairDist>, BVPairDist::less >
{
  MeReal QmaxMin; // min over all elements of top.distance_ub
public:
  DistQueueWithMax(): QmaxMin(almostInfinity) {};

  void pushIfSmall(const BVPairDist & d)
    {
      MeReal dtemp;
      if ( (dtemp = d.distance_ub()) < QmaxMin) { QmaxMin = dtemp; }

      if (d.distance_lb() <= QmaxMin ) { push(d); }
      //else {fprintf(stderr,".");}
    }

  MeReal minUB() const { return QmaxMin; }
  MeReal minLB() const { return top().distance_lb(); }
};

inline void
breakNode1_distance(const BVPairDist& top, DistQueueWithMax &Q);

inline void
breakNode2_distance(const BVPairDist& top, DistQueueWithMax &Q);


/////////////////////////////////////////////////////////////////////////////
// distance algorithm

static void distance( const BVDNode& left, const BVTransform& m1,
         const BVDNode& right, const BVTransform& m2,
         int *tri1, int *tri2, MeReal *distLB, MeReal *distUB )
{
  DistQueueWithMax Q; // (100);
  int size = 0;

  // setup transforms
  McdMeshMeshDistData.m2To1Global_distance.thisIsFirstThenInverseSecond( m2, m1 );

  //BVTransform m2To1Root;
  //find_m2To1(&left,&right,& McdMeshMeshDistData.m2To1Global_distance,& m2To1Root);

  // BVTransform m2To1Temp;
  // m2To1Temp.thisIsFirstThenInverseSecond( m2To1Global_distance, m1 );
  // m2To1Root.thisIsFirstThenSecond( m2, m2To1Temp );

  // ready Queue
  Q.push(BVPairDist(&left, &right,0));

  while(! Q.empty() ){
    BVPairDist top = Q.top();
    Q.pop();

    if (top.first->isLeaf()) {
      if (top.second->isLeaf())
      {
         *tri1 = top.first->mTriId;
         *tri2 = top.second->mTriId;
         *distUB = *distLB = top.distance_lb();
         return;
      }
      else // only first is a leaf
    {
      breakNode2_distance(top,Q);
    }
    }
    else // first is not a leaf
      {
    if (top.second->isLeaf())
      {
        breakNode1_distance(top,Q);
      }
    else { // neither is a leaf
      // pick a box to be opened
      if (top.relDepth < 0) { //top.firstDepth > top.secondDepth) { // open right
      //if (top.first->getObbSize() < top.second->getObbSize() ) {
        breakNode2_distance(top,Q);
      } else { // open left
        breakNode1_distance(top,Q);
      }

    }
      }

  }

  *distLB = *distUB = MEINFINITY;
}

/****************************************************************************/
inline void
breakNode1_distance(const BVPairDist& top, DistQueueWithMax &Q)
  // const BVDNode& node1, const BVDNode& node2, const BVTransform& m2To1  )
{
  //if (top.first->getLeftNode()->getObbSize() > top.first->getRightNode()->getObbSize()) {
  Q.pushIfSmall(BVPairDist( top.first->getLeftNode(),  top.second, top.relDepth-1 ));
  Q.pushIfSmall(BVPairDist( top.first->getRightNode(),  top.second, top.relDepth-1));
  //} else {
  //Q.pushIfSmall(BVPairDist( top.first->getRightNode(),  top.second, top.relDepth-1));
  //Q.pushIfSmall(BVPairDist( top.first->getLeftNode(),  top.second, top.relDepth-1));
  //}
}


/****************************************************************************/
inline void
breakNode2_distance(const BVPairDist& top, DistQueueWithMax &Q)
{
  //if (top.second->getLeftNode()->getObbSize() > top.second->getRightNode()->getObbSize()) {
      Q.pushIfSmall(BVPairDist( top.first, top.second->getLeftNode(), top.relDepth+1));
      Q.pushIfSmall(BVPairDist( top.first, top.second->getRightNode(), top.relDepth+1));
  //} else {
  //  Q.pushIfSmall(BVPairDist( top.first, top.second->getLeftNode(), top.relDepth+1));
  //  Q.pushIfSmall(BVPairDist( top.first, top.second->getRightNode(), top.relDepth+1));
  //}
}


/////////////////////////////////////////////////////////////////////////////
// distance function:
//////////////////////////////////////////////////////////////////////////
/**
   Returns, in @a r, the exact distance between two meshes,
   the closest points on each meshes realizing this distance,
   as well as the triangles containing each point.
   @see McdDistanceResult
*/

#ifdef LICENSE_CHECK
#define PROTECTED_FEATURE FEATURE_MCD
#include "MeLicenseHeader.c"
#endif

void MEAPI
McdTriangleMeshTriangleMeshDistance(McdModelPair *p,
                                    McdDistanceResult* r )
{

    #ifdef LICENSE_CHECK
    #include "MeLicenseVars.c"
    #endif

    CxTriangleMesh_ *mesh1 = (CxTriangleMesh_ *)McdModelGetGeometry(p->model1);
    CxTriangleMesh_ *mesh2 = (CxTriangleMesh_ *)McdModelGetGeometry(p->model2);

    BVTree   *bvt1 = (BVTree *) mesh1->m_bvt;
    BVTree   *bvt2 = (BVTree *) mesh2->m_bvt;
    BVDNode *root1 = (BVDNode*) bvt1->getRootNodePtr();
    BVDNode *root2 = (BVDNode*) bvt2->getRootNodePtr();

    lsTransform *tm1 = (lsTransform *) McdModelGetTransformPtr(p->model1);
    lsTransform *tm2 = (lsTransform *) McdModelGetTransformPtr(p->model2);

    BVTransform btm1(tm1);
    BVTransform btm2(tm2);

    McdMeshMeshDistData.m0 = (CxTriangleMesh*) mesh1;
    McdMeshMeshDistData.m1 = (CxTriangleMesh*) mesh2;
 //   McdMeshMeshDistData.btm0 = &btm1;
 //   McdMeshMeshDistData.btm1 = &btm2;


    #ifdef LICENSE_CHECK
    #include "MeLicensePreFeature.c"
    #endif

    r->element1.tag = r->element2.tag = -1;

    #ifdef LICENSE_CHECK
    if (FAIL_VAR(PROTECTED_FEATURE)) {
        btm1.setIdentity();
        btm2.setIdentity();
    }
    #endif

    distance( *root1, btm1, *root2, btm2,
              &r->element1.tag, &r->element2.tag,
              &r->distanceLB, &r->distanceUB);

    // find the two closest points again given the triangle IDs.
    if (r->element1.tag>=0)
       distanceTriTriTwoTrans((CxTriangleMesh*)mesh1, r->element1.tag,
                              (CxTriangleMesh*)mesh2, r->element2.tag,
                              &btm1, &btm2,
                              (lsVec3*)r->point1, (lsVec3*)r->point2);
}

#else  // METHOD_USING_QUEUE

/* -*-c++-*-
 *===============================================================
 * File:        MeshMeshDist.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.28.2.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */
#include <stdio.h>
#include <MePrecision.h>
#include <McdModel.h>
#include "CxTriangleMesh.h"
#include <McdTriangleMesh.h>
#include "McdDistanceResult.h"
#include "BVTree.h"
#include "ObbObb.h"
#include "IxTriangleMeshLineSegment.h"
#include "TriTriDist.h"
#include "RectRectDist.h"
#include "SSR.h"

//---------------------------------------------------------------------
void DistBVDNodeBVDNode( struct McdDistanceResult* r, MeReal &dist,
        CxTriangleMesh* m0, CxTriangleMesh* m1,
        BVTransform *btm0, BVTransform *btm1, BVTransform *MM,
        BVTree* bvt0, BVTree *bvt1,
        BVNode* node0, BVNode* node1)
{
  if (node0->isLeaf() && node1->isLeaf()) {
    lsVec3 *v0[3], *v1[3], *n0, *n1;
    lsVec3 v0g[3], v1g[3], n0g, n1g;

    int       triId0 = node0->mTriId;
    int       triId1 = node1->mTriId;

    CxTriangleMeshGetTriangleVertexPtrs(m0, triId0, &v0[0], &v0[1], &v0[2]);
    CxTriangleMeshGetTriangleVertexPtrs(m1, triId1, &v1[0], &v1[1], &v1[2]);
    CxTriangleMeshGetTriangleNormalPtr(m0, triId0, &n0);
    CxTriangleMeshGetTriangleNormalPtr(m1, triId1, &n1);
#if 0
    btm0->transform(*v0[0], &v0g[0]);
    btm0->transform(*v0[1], &v0g[1]);
    btm0->transform(*v0[2], &v0g[2]);
    btm1->transform(*v1[0], &v1g[0]);
    btm1->transform(*v1[1], &v1g[1]);
    btm1->transform(*v1[2], &v1g[2]);
    btm0->transformWithoutTranslate(*n0, &n0g);
    btm1->transformWithoutTranslate(*n1, &n1g);

    TriTriDist tt( &v0g[0], &v0g[1], &v0g[2], &n0g, &v1g[0], &v1g[1], &v1g[2], &n1g);

    lsVec3 p0, p1;
    MeReal tridist= tt.getMinDist(&p0, &p1);
    if ( tridist < dist ) {
      dist = tridist;
      r->distanceLB = dist;
      r->distanceUB = dist;
      r->point1[0] = p0[0];
      r->point1[1] = p0[1];
      r->point1[2] = p0[2];
      r->point2[0] = p1[0];
      r->point2[1] = p1[1];
      r->point2[2] = p1[2];
    }

#else

    MM->transform(*v1[0], &v1g[0]);
    MM->transform(*v1[1], &v1g[1]);
    MM->transform(*v1[2], &v1g[2]);
    MM->transformWithoutTranslate(*n1, &n1g);

    TriTriDist tt( v0[0], v0[1], v0[2], n0, &v1g[0], &v1g[1], &v1g[2], &n1g);

    lsVec3 p0, p1, p0g, p1g;
    MeReal tridist= tt.getMinDist(&p0, &p1);
    if ( tridist < dist ) {
      btm0->transform(p0, &p0g);
      btm0->transform(p1, &p1g);
      dist = tridist;
      r->distanceLB = dist;
      r->distanceUB = dist;
      r->point1[0] = p0g[0];
      r->point1[1] = p0g[1];
      r->point1[2] = p0g[2];
      r->point2[0] = p1g[0];
      r->point2[1] = p1g[1];
      r->point2[2] = p1g[2];
    }

#endif

    return;
  }

  if ( dist <= 0 ) return;

  BVTransform mm0, mm1, m1To0;
#if 0
  mm0.thisIsFirstThenSecond(node0->mOBB.mTm, *btm0);
  mm1.thisIsFirstThenSecond(node1->mOBB.mTm, *btm1);
  m1To0.thisIsFirstThenInverseSecond(mm1, mm0);
#else
  mm1.thisIsFirstThenSecond(node1->mOBB.mTm, *MM);
  m1To0.thisIsFirstThenInverseSecond(mm1, node0->mOBB.mTm);
#endif

  lsVec3 b, e0, e1;
  CxRectangle rect0, rect1;

  CxSSR* ssr0 = &(((BVDNode*)node0)->mSSR);
  CxSSR* ssr1 = &(((BVDNode*)node1)->mSSR);

  b.setValue(-ssr0->mExtX, -ssr0->mExtY, 0);
  e0.setValue( 2*ssr0->mExtX, 0, 0);
  e1.setValue( 0, 2*ssr0->mExtY, 0);
  rect0.setData(b, e0, e1 );

  b.setValue(-ssr1->mExtX, -ssr1->mExtY, 0);
  e0.setValue( 2*ssr1->mExtX, 0, 0);
  e1.setValue( 0, 2*ssr1->mExtY, 0);

  lsVec3 bTo0, e0To0, e1To0;
  m1To0.transform(b, &bTo0);
  m1To0.transformWithoutTranslate(e0, &e0To0);
  m1To0.transformWithoutTranslate(e1, &e1To0);

  rect1.setData(bTo0, e0To0, e1To0 );

  Rectrect rr(&rect0, &rect1);

  MeReal d = rr.getMinDist();

  if ( (d - (ssr0->mRadius + ssr1->mRadius)) > dist ) {
    return;
  }

  if ( !node0->isLeaf() && !node1->isLeaf() ) {
    if (node0->getObbSize() > node1->getObbSize() ) {
      if (node0->getLeftNode()->getObbSize() > node0->getRightNode()->getObbSize()) {
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0->getLeftNode(), node1);
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0->getRightNode(), node1);
      } else {
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0->getRightNode(), node1);
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0->getLeftNode(), node1);
      }
    }  else  {
      if (node1->getLeftNode()->getObbSize() > node1->getRightNode()->getObbSize()) {
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0, node1->getLeftNode());
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0, node1->getRightNode());
      } else {
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0, node1->getRightNode());
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0, node1->getLeftNode());
      }
    }
  } else {
    if (!node0->isLeaf()) {
      if (node0->getLeftNode()->getObbSize() > node0->getRightNode()->getObbSize()) {
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0->getLeftNode(), node1);
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0->getRightNode(), node1);
      } else {
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0->getRightNode(), node1);
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0->getLeftNode(), node1);
      }
    }  else  {
      if (node1->getLeftNode()->getObbSize() > node1->getRightNode()->getObbSize()) {
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0, node1->getLeftNode());
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0, node1->getRightNode());
      } else {
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0, node1->getRightNode());
        DistBVDNodeBVDNode( r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1, node0, node1->getLeftNode());
      }
    }
  }

}

//---------------------------------------------------------------------
void DistBVDTreeBVDTree( struct McdDistanceResult* r, MeReal &dist,
        CxTriangleMesh* m0, CxTriangleMesh* m1,
        BVTransform *btm0, BVTransform *btm1, BVTransform *MM,
        BVTree* bvt0, BVTree *bvt1)
{
  DistBVDNodeBVDNode(r, dist, m0, m1, btm0, btm1, MM, bvt0, bvt1,
    bvt0->getRootNodePtr(), bvt1->getRootNodePtr());
}

//---------------------------------------------------------------------
void MEAPI
McdTriangleMeshTriangleMeshDistance( struct McdModelPair *p,
        struct McdDistanceResult* r )
{
  CxTriangleMesh *m0 = (CxTriangleMesh*) McdModelGetGeometry(p->model1);
  CxTriangleMesh *m1 = (CxTriangleMesh*) McdModelGetGeometry(p->model2);

  if (McdGeometryGetTypeId(m0) != kMcdGeometryTypeTriangleMesh) return;
  if (McdGeometryGetTypeId(m1) != kMcdGeometryTypeTriangleMesh) return;
  if (((CxTriangleMesh_*)m0)->m_flags < 1) return;
  if (((CxTriangleMesh_*)m1)->m_flags < 1) return;

  lsTransform *tm0 = (lsTransform *) McdModelGetTransformPtr(p->model1);
  lsTransform *tm1 = (lsTransform *) McdModelGetTransformPtr(p->model2);

  BVTransform btm0(tm0);
  BVTransform btm1(tm1);

  BVTransform MM;
  MM.thisIsFirstThenInverseSecond(btm1, btm0);

  BVTree   *bvt0 = (BVTree *) ((CxTriangleMesh_*)m0)->m_bvt;
  BVTree   *bvt1 = (BVTree *) ((CxTriangleMesh_*)m1)->m_bvt;

  lsVec3 orig, dest;

  tm0->getTranslation( &orig );
  tm1->getTranslation( &dest );

  MeReal dist = (MeReal) 1.0E20;

#if 1
  McdLineSegIntersectResult res0, res1;
  int ixres0 = IxTriangleMeshLineSegment(p->model1, dest.v, orig.v, &res0 );
  int ixres1 = IxTriangleMeshLineSegment(p->model2, orig.v, dest.v, &res1 );

  if ( ixres0 && ixres1 ) {
    dist = (*((lsVec3*)res0.position) - *((lsVec3*)res1.position)).norm();
    r->distanceLB = dist;
    r->distanceUB = dist;
    r->point1[0] = res0.position[0];
    r->point1[1] = res0.position[1];
    r->point1[2] = res0.position[2];
    r->point2[0] = res1.position[0];
    r->point2[1] = res1.position[1];
    r->point2[2] = res1.position[2];
  }

#endif

  lsVec3 *v0[3], *v1[3];
  lsVec3 v0g[3], v1g[3], n0g, n1g;
  MeReal tridist;

#if 0
  lsVec3 *n0, *n1;
  CxTriangleMeshGetTriangleVertexPtrs(m0, 0, &v0[0], &v0[1], &v0[2]);
  CxTriangleMeshGetTriangleVertexPtrs(m1, 0, &v1[0], &v1[1], &v1[2]);
  CxTriangleMeshGetTriangleNormalPtr(m0, 0, &n0);
  CxTriangleMeshGetTriangleNormalPtr(m1, 0, &n1);

  tm0->transform(*v0[0], &v0g[0]);
  tm0->transform(*v0[1], &v0g[1]);
  tm0->transform(*v0[2], &v0g[2]);
  tm1->transform(*v1[0], &v1g[0]);
  tm1->transform(*v1[1], &v1g[1]);
  tm1->transform(*v1[2], &v1g[2]);
  tm0->transformWithoutTranslate(*n0, &n0g);
  tm1->transformWithoutTranslate(*n1, &n1g);

  TriTriDist tt( &v0g[0], &v0g[1], &v0g[2], &n0g, &v1g[0], &v1g[1], &v1g[2], &n1g);

  lsVec3 p0, p1;
  tridist= tt.getMinDist(&p0, &p1);

  if ( tridist < dist ) {
    dist = tridist;
    r->distanceLB = dist;
    r->distanceUB = dist;
    r->point1[0] = p0[0];
    r->point1[1] = p0[1];
    r->point1[2] = p0[2];
    r->point2[0] = p1[0];
    r->point2[1] = p1[1];
    r->point2[2] = p1[2];
  }
#endif

#if 1
  lsVec3 dir = dest - orig;
  lsVec3 dir0, dir1;
  dir.normalize();

  tm0->inverseTransformWithoutTranslate(dir, &dir0);
  tm1->inverseTransformWithoutTranslate(dir, &dir1);

  MeReal pmin0, pmax0;
  MeReal pmin1, pmax1;

  // re-use some variables.
  ComputeSampledExtremalVerticesEntireMesh(m0, dir0, &v0[0], &v0[1], &pmin0, &pmax0);
  ComputeSampledExtremalVerticesEntireMesh(m1, dir1, &v1[0], &v1[1], &pmin1, &pmax1);

  tm0->transform(*v0[0], &v0g[2]);
  tm1->transform(*v1[1], &v1g[2]);

  tridist = (v0g[2] - v1g[2]).norm();

  if ( tridist < dist ) {
    dist = tridist;
    r->distanceLB = dist;
    r->distanceUB = dist;
    r->point1[0] = v0g[2][0];
    r->point1[1] = v0g[2][1];
    r->point1[2] = v0g[2][2];
    r->point2[0] = v1g[2][0];
    r->point2[1] = v1g[2][1];
    r->point2[2] = v1g[2][2];
  }

#endif

  DistBVDTreeBVDTree(r, dist, m0, m1, &btm0, &btm1, &MM, bvt0, bvt1 );
}
#endif  // METHOD_USING_QUEUE
