/* -*-c++-*-
 *===============================================================
 * File:        TriTriIsctTest.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 * $Revision: 1.10.18.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

/*
 * int tri_tri_intersect(MeReal V0[3],MeReal V1[3],MeReal V2[3],
 *                         MeReal U0[3],MeReal U1[3],MeReal U2[3])
 *
 * parameters: vertices of triangle 1: V0,V1,V2
 *             vertices of triangle 2: U0,U1,U2
 * result    : returns 1 if the triangles intersect, otherwise 0
 *
 */

#include <math.h>
#include <stdio.h>

#include <MePrecision.h>
#include "TriTriIsctTest.h"
#include "McdCheck.h"

/* if USE_EPSILON_TEST is true then we do a check:
         if |dv|<EPSILON then dv=0.0;
   else no check is done (which is less robust)
*/

#define USE_EPSILON_TEST TRUE
// #define EPSILON ME_SMALL_EPSILON

/* some macros */
#define CROSS(dest,v1,v2)                      \
              dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
              dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
              dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2)          \
            dest[0]=v1[0]-v2[0]; \
            dest[1]=v1[1]-v2[1]; \
            dest[2]=v1[2]-v2[2];

/* sort so that a<=b */
#define SORT(a,b)       \
             if(a>b)    \
             {          \
               MeReal c; \
               c=a;     \
               a=b;     \
               b=c;     \
             }

#define ISECT(VV0,VV1,VV2,D0,D1,D2,isect0,isect1) \
              isect0=VV0+(VV1-VV0)*D0/(D0-D1);    \
              isect1=VV0+(VV2-VV0)*D0/(D0-D2);

#define COMPUTE_INTERVALS(VV0,VV1,VV2,D0,D1,D2,D0D1,D0D2,isect0,isect1) \
  if(D0D1>0.0f)                                         \
  {                                                     \
    /* here we know that D0D2<=0.0 */                   \
    /* that is D0, D1 are on the same side, D2 on the other or on the plane */ \
    ISECT(VV2,VV0,VV1,D2,D0,D1,isect0,isect1);          \
  }                                                     \
  else if(D0D2>0.0f)                                    \
  {                                                     \
    /* here we know that d0d1<=0.0 */                   \
    ISECT(VV1,VV0,VV2,D1,D0,D2,isect0,isect1);          \
  }                                                     \
  else if(D1*D2>0.0f || D0!=0.0f)                       \
  {                                                     \
    /* here we know that d0d1<=0.0 or that D0!=0.0 */   \
    ISECT(VV0,VV1,VV2,D0,D1,D2,isect0,isect1);          \
  }                                                     \
  else if(D1!=0.0f)                                     \
  {                                                     \
    ISECT(VV1,VV0,VV2,D1,D0,D2,isect0,isect1);          \
  }                                                     \
  else if(D2!=0.0f)                                     \
  {                                                     \
    ISECT(VV2,VV0,VV1,D2,D0,D1,isect0,isect1);          \
  }                                                     \
  else                                                  \
  {                                                     \
    /* triangles are coplanar */                        \
    return TriTriIsectCoplanar(N1,V0,V1,V2,U0,U1,U2);   \
  }

/* this edge to edge test is based on Franlin Antonio's gem:
   "Faster Line Segment Intersection", in Graphics Gems III,
   pp. 199-202 */
#define EDGE_EDGE_TEST(V0,U0,U1)                      \
  Bx=U0[i0]-U1[i0];                                   \
  By=U0[i1]-U1[i1];                                   \
  Cx=V0[i0]-U0[i0];                                   \
  Cy=V0[i1]-U0[i1];                                   \
  f=Ay*Bx-Ax*By;                                      \
  d=By*Cx-Bx*Cy;                                      \
  if((f>0 && d>=0 && d<=f) || (f<0 && d<=0 && d>=f))  \
  {                                                   \
    e=Ax*Cy-Ay*Cx;                                    \
    if(f>0)                                           \
    {                                                 \
      if(e>=0 && e<=f) return INTERSECT_COPLANAR;     \
    }                                                 \
    else                                              \
    {                                                 \
      if(e<=0 && e>=f) return INTERSECT_COPLANAR;     \
    }                                                 \
  }

#define EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2) \
{                                              \
  MeReal Ax,Ay,Bx,By,Cx,Cy,e,d,f;              \
  Ax=V1[i0]-V0[i0];                            \
  Ay=V1[i1]-V0[i1];                            \
  /* test edge U0,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U0,U1);                    \
  /* test edge U1,U2 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U1,U2);                    \
  /* test edge U2,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U2,U0);                    \
}

#define POINT_IN_TRI(V0,U0,U1,U2)           \
{                                           \
  MeReal a,b,c,d0,d1,d2;                    \
  /* is T1 completly inside T2? */          \
  /* check if V0 is inside tri(U0,U1,U2) */ \
  a=U1[i1]-U0[i1];                          \
  b= -(U1[i0]-U0[i0]);                      \
  c= -a*U0[i0]-b*U0[i1];                    \
  d0=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U2[i1]-U1[i1];                          \
  b= -(U2[i0]-U1[i0]);                      \
  c= -a*U1[i0]-b*U1[i1];                    \
  d1=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U0[i1]-U2[i1];                          \
  b= -(U0[i0]-U2[i0]);                      \
  c= -a*U2[i0]-b*U2[i1];                    \
  d2=a*V0[i0]+b*V0[i1]+c;                   \
  if(d0*d1>0.0)                             \
  {                                         \
    if(d0*d2>0.0) return INTERSECT_COPLANAR;\
  }                                         \
}

unsigned int
TriTriIsectCoplanar(const MeReal *N, const MeReal *V0, const MeReal *V1,
            const MeReal *V2, const MeReal *U0, const MeReal *U1, const MeReal *U2)
{
    MeReal    A[3];
    short     i0, i1;
    /* first project onto an axis-aligned plane, that maximizes the area */
    /* of the triangles, compute indices: i0,i1. */
    A[0] = (MeReal) MeFabs(N[0]);
    A[1] = (MeReal) MeFabs(N[1]);
    A[2] = (MeReal) MeFabs(N[2]);
    if (A[0] > A[1]) {
    if (A[0] > A[2]) {
        i0 = 1;     /* A[0] is greatest */
        i1 = 2;
    } else {
        i0 = 0;     /* A[2] is greatest */
        i1 = 1;
    }
    } else {            /* A[0]<=A[1] */

    if (A[2] > A[1]) {
        i0 = 0;     /* A[2] is greatest */
        i1 = 1;
    } else {
        i0 = 0;     /* A[1] is greatest */
        i1 = 2;
    }
    }

    /* test all edges of triangle 1 against the edges of triangle 2 */
    EDGE_AGAINST_TRI_EDGES(V0, V1, U0, U1, U2);
    EDGE_AGAINST_TRI_EDGES(V1, V2, U0, U1, U2);
    EDGE_AGAINST_TRI_EDGES(V2, V0, U0, U1, U2);

    /* finally, test if tri1 is totally contained in tri2 or vice versa */
    POINT_IN_TRI(V0, U0, U1, U2);
    POINT_IN_TRI(U0, V0, V1, V2);

    return DISJOINT;
}

/**
 * intersection test
 */
unsigned int
TriTriIsect(const MeReal *V0, const MeReal *V1, const MeReal *V2,
        const MeReal *U0, const MeReal *U1, const MeReal *U2)
{
    MeReal    E1[3], E2[3];
    MeReal    N1[3], N2[3], d1, d2;
    MeReal    du0, du1, du2, dv0, dv1, dv2;
    MeReal    D[3];
    MeReal    isect1[2], isect2[2];
    MeReal    du0du1, du0du2, dv0dv1, dv0dv2;
    short     index;
    MeReal    vp0, vp1, vp2;
    MeReal    up0, up1, up2;
    MeReal    b, c, max;

    /* compute plane equation of triangle(V0,V1,V2) */
    SUB(E1, V1, V0);
    SUB(E2, V2, V0);
    CROSS(N1, E1, E2);
    d1 = -DOT(N1, V0);
    /* plane equation 1: N1.X+d1=0 */

    /* put U0,U1,U2 into plane equation 1 to compute signed distances to the plane */
    du0 = DOT(N1, U0) + d1;
    du1 = DOT(N1, U1) + d1;
    du2 = DOT(N1, U2) + d1;

    /* coplanarity robustness check */
#if USE_EPSILON_TEST==TRUE
    if (MeFabs(du0) < ME_SMALL_EPSILON)
    du0 = 0.0;
    if (MeFabs(du1) < ME_SMALL_EPSILON)
    du1 = 0.0;
    if (MeFabs(du2) < ME_SMALL_EPSILON)
    du2 = 0.0;
#endif
    du0du1 = du0 * du1;
    du0du2 = du0 * du2;

    if (du0du1 > 0.0f && du0du2 > 0.0f) /* same sign on all of them + not equal 0 ? */
    return DISJOINT;    /* no intersection occurs */

    /* compute plane of triangle (U0,U1,U2) */
    SUB(E1, U1, U0);
    SUB(E2, U2, U0);
    CROSS(N2, E1, E2);
    d2 = -DOT(N2, U0);
    /* plane equation 2: N2.X+d2=0 */

    /* put V0,V1,V2 into plane equation 2 */
    dv0 = DOT(N2, V0) + d2;
    dv1 = DOT(N2, V1) + d2;
    dv2 = DOT(N2, V2) + d2;

#if USE_EPSILON_TEST==TRUE
    if (MeFabs(dv0) < ME_SMALL_EPSILON)
    dv0 = 0.0;
    if (MeFabs(dv1) < ME_SMALL_EPSILON)
    dv1 = 0.0;
    if (MeFabs(dv2) < ME_SMALL_EPSILON)
    dv2 = 0.0;
#endif

    dv0dv1 = dv0 * dv1;
    dv0dv2 = dv0 * dv2;

    if (dv0dv1 > 0.0f && dv0dv2 > 0.0f) /* same sign on all of them + not equal 0 ? */
    return DISJOINT;    /* no intersection occurs */

    /* compute direction of intersection line */
    CROSS(D, N1, N2);

    /* compute and index to the largest component of D */
    max = (MeReal) MeFabs(D[0]);
    index = 0;
    b = (MeReal) MeFabs(D[1]);
    c = (MeReal) MeFabs(D[2]);
    if (b > max)
    max = b, index = 1;
    if (c > max)
    max = c, index = 2;

    /* this is the simplified projection onto L */
    vp0 = V0[index];
    vp1 = V1[index];
    vp2 = V2[index];

    up0 = U0[index];
    up1 = U1[index];
    up2 = U2[index];

    /* compute interval for triangle 1 */
    COMPUTE_INTERVALS(vp0, vp1, vp2, dv0, dv1, dv2, dv0dv1, dv0dv2, isect1[0], isect1[1]);

    /* compute interval for triangle 2 */
    COMPUTE_INTERVALS(up0, up1, up2, du0, du1, du2, du0du1, du0du2, isect2[0], isect2[1]);

    SORT(isect1[0], isect1[1]);
    SORT(isect2[0], isect2[1]);

    if (isect1[1] < isect2[0] || isect2[1] < isect1[0])
    return DISJOINT;

    return INTERSECT;
}

/*----------------------------------------------------------------------*/

#define getIndices(index,id)                    \
  if (index == 0) { id[0] = 1; id[1] = 2;}      \
  else if (index == 1) { id[0] = 0; id[1] = 2;} \
  else if (index == 2) { id[0] = 0; id[1] = 1;}

#define intersect_3d(index,idk,ido,p,N1,N2,d1,d2,res) \
{                                               \
  MeReal S=N2[ido]*N1[idk]-N1[ido]*N2[idk];     \
  MeReal T=N1[ido]*N2[index]-N2[ido]*N1[index]; \
  MeReal Q=d2*N1[ido]-d1*N2[ido];               \
  MCD_CHECK_ASSERT_( MeFabs(S) > 0, "A triangle mesh function");               \
  res = (T*p+Q)/S;                              \
}

#define COMPUTE_INTERVALS_D(VV0,VV1,VV2,D0,D1,D2,D0D1,D0D2,isect0,isect1) \
  if(D0D1>0.0f)                                         \
  {                                                     \
    /* here we know that D0D2<=0.0 */                   \
    /* that is D0, D1 are on the same side, D2 on the other or on the plane */ \
    ISECT(VV2,VV0,VV1,D2,D0,D1,isect0,isect1);          \
  }                                                     \
  else if(D0D2>0.0f)                                    \
  {                                                     \
    /* here we know that d0d1<=0.0 */                   \
    ISECT(VV1,VV0,VV2,D1,D0,D2,isect0,isect1);          \
  }                                                     \
  else if(D1*D2>0.0f || D0!=0.0f)                       \
  {                                                     \
    /* here we know that d0d1<=0.0 or that D0!=0.0 */   \
    ISECT(VV0,VV1,VV2,D0,D1,D2,isect0,isect1);          \
  }                                                     \
  else if(D1!=0.0f)                                     \
  {                                                     \
    ISECT(VV1,VV0,VV2,D1,D0,D2,isect0,isect1);          \
  }                                                     \
  else if(D2!=0.0f)                                     \
  {                                                     \
    ISECT(VV2,VV0,VV1,D2,D0,D1,isect0,isect1);          \
  }                                                     \
  else                                                  \
  {                                                     \
    /* triangles are coplanar */                        \
    return TriTriIsectCoplanar_d(d1,N1,V0,V1,V2,U0,U1,U2,IsectPt,numMaxPts,numPts);\
  }

inline int
AddPoint(lsVec3 *p, int LastId)
{
    for (int i = 0; i < LastId; i++) {
    if ((p[i] - p[LastId]).square_norm() < ME_SMALL_EPSILON) {
        return 0;
    }
    }
    return 1;
}

/* this edge to edge test is based on Franlin Antonio's gem:
   "Faster Line Segment Intersection", in Graphics Gems III,
   pp. 199-202 */
#define EDGE_EDGE_TEST_D(V0,U0,U1)                         \
  Bx=U0[i0]-U1[i0];                                        \
  By=U0[i1]-U1[i1];                                        \
  Cx=V0[i0]-U0[i0];                                        \
  Cy=V0[i1]-U0[i1];                                        \
  f=Ay*Bx-Ax*By;                                           \
  d=By*Cx-Bx*Cy;                                           \
  if((f>0 && d>=0 && d<=f) || (f<0 && d<=0 && d>=f))       \
  {                                                        \
    e=Ax*Cy-Ay*Cx;                                         \
    if(f>0)                                                \
    {                                                      \
      if(e>=0 && e<=f) {                                   \
        MeReal alfa = d/f;                                 \
        MeReal x = V0[i0]+alfa*Ax;                         \
        MeReal y = V0[i1]+alfa*Ay;                         \
        IsectPt[numPts][i0] = x;                           \
        IsectPt[numPts][i1] = y;                           \
        IsectPt[numPts][id] = -(N[i0]*x+N[i1]*y+deq)/N[id];\
        if (AddPoint(IsectPt, numPts)) numPts++;           \
      }                                                    \
    }                                                      \
    else                                                   \
    {                                                      \
      if(e<=0 && e>=f) {                                   \
        MeReal alfa = d/f;                                 \
        MeReal x = V0[i0]+alfa*Ax;                         \
        MeReal y = V0[i1]+alfa*Ay;                         \
        IsectPt[numPts][i0] = x;                           \
        IsectPt[numPts][i1] = y;                           \
        IsectPt[numPts][id] = -(N[i0]*x+N[i1]*y+deq)/N[id];\
        if (AddPoint(IsectPt, numPts)) numPts++;           \
      }                                                    \
    }                                                      \
  }

#define EDGE_AGAINST_TRI_EDGES_D(V0,V1,U0,U1,U2) \
{                                                \
  MeReal Ax,Ay,Bx,By,Cx,Cy,e,d,f;                \
  Ax=V1[i0]-V0[i0];                              \
  Ay=V1[i1]-V0[i1];                              \
  /* test edge U0,U1 against V0,V1 */            \
  EDGE_EDGE_TEST_D(V0,U0,U1);                    \
  /* test edge U1,U2 against V0,V1 */            \
  EDGE_EDGE_TEST_D(V0,U1,U2);                    \
  /* test edge U2,U1 against V0,V1 */            \
  EDGE_EDGE_TEST_D(V0,U2,U0);                    \
}

#define POINT_IN_TRI_D(V0,U0,U1,U2)         \
{                                           \
  MeReal a,b,c,d0,d1,d2;                    \
  /* is T1 completly inside T2? */          \
  /* check if V0 is inside tri(U0,U1,U2) */ \
  a=U1[i1]-U0[i1];                          \
  b= -(U1[i0]-U0[i0]);                      \
  c= -a*U0[i0]-b*U0[i1];                    \
  d0=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U2[i1]-U1[i1];                          \
  b= -(U2[i0]-U1[i0]);                      \
  c= -a*U1[i0]-b*U1[i1];                    \
  d1=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U0[i1]-U2[i1];                          \
  b= -(U0[i0]-U2[i0]);                      \
  c= -a*U2[i0]-b*U2[i1];                    \
  d2=a*V0[i0]+b*V0[i1]+c;                   \
  if(d0*d1>0.0)                             \
  {                                         \
    if(d0*d2>0.0) {                         \
      bInside =1;                           \
      /* return INTERSECT_COPLANAR;*/       \
    }                                       \
  }                                         \
}

/*---------------------------------------------------------*/

typedef MeReal MyVec2[2];

inline MeReal
v2dotv2(MyVec2 v1, MyVec2 v2)
{
    return (v1[0] * v2[0] + v1[1] * v2[1]);
}

inline MeReal
v2normalize(MyVec2 & v)
{
    MeReal    t = (MeReal) sqrt(v2dotv2(v, v));
    if (t > 0) {
    v[0] /= t;
    v[1] /= t;
    }
    return t;
}

inline MeReal
cosAngle(MyVec2 lp, MyVec2 ba, const lsVec3 &p, int i0, int i1)
{
    MyVec2    v;
    v[0] = p[i0] - lp[0];
    v[1] = p[i1] - lp[1];
    v2normalize(v);
    return v2dotv2(ba, v);
}

inline void
OrderIsectPts(lsVec3 *IsectPt, int numPts, short i0, short i1)
{
    int       i, j = 0, is;
    MyVec2    lp, ba;
    MeReal    val, cosa[12], lv;
    lsVec3    vtmp;

    lv = IsectPt[0][i1];
    for (i = 1; i < numPts; i++) {
    if (IsectPt[i][i1] < lv) {
        lv = IsectPt[i][i1];
        j = i;
    }
    }

    lp[0] = IsectPt[j][i0];
    lp[1] = IsectPt[j][i1];

    // swap
    if (j != 0) {
    vtmp = IsectPt[0];
    IsectPt[0] = IsectPt[j];
    IsectPt[j] = vtmp;
    }

    cosa[0] = 1;
    ba[0] = 1;
    ba[1] = 0;
    for (i = 1; i < numPts; i++) {
    cosa[i] = cosAngle(lp, ba, IsectPt[i], i0, i1);
    }

    for (i = 1; i < numPts; i++) {
    is = i;
    val = cosa[i];
    for (j = i + 1; j < numPts; j++) {
        if (cosa[j] > val) {
        is = j;
        val = cosa[j];
        }
    }

    // swap
    if (is != i) {
        val = cosa[i];
        cosa[i] = cosa[is];
        cosa[is] = val;
        vtmp = IsectPt[i];
        IsectPt[i] = IsectPt[is];
        IsectPt[is] = vtmp;
    }
    }
}

/*---------------------------------------------------------*/

unsigned int
TriTriIsectCoplanar_d(MeReal deq, const MeReal *N, const MeReal *V0, const MeReal *V1,
              const MeReal *V2, const MeReal *U0, const MeReal *U1, const MeReal *U2,
              lsVec3 *IsectPt, int numMaxPts, int &numPts)
{
    MeReal    A[3];
    short     i0, i1, id;
    /* first project onto an axis-aligned plane, that maximizes the area */
    /* of the triangles, compute indices: i0,i1. */
    A[0] = (MeReal) MeFabs(N[0]);
    A[1] = (MeReal) MeFabs(N[1]);
    A[2] = (MeReal) MeFabs(N[2]);

    if (A[0] > A[1]) {
    if (A[0] > A[2]) {
        id = 0;
        i0 = 1;     /* A[0] is greatest */
        i1 = 2;
    } else {
        i0 = 0;     /* A[2] is greatest */
        i1 = 1;
        id = 2;
    }
    } else {            /* A[0]<=A[1] */

    if (A[2] > A[1]) {
        i0 = 0;     /* A[2] is greatest */
        i1 = 1;
        id = 2;
    } else {
        i0 = 0;     /* A[1] is greatest */
        id = 1;
        i1 = 2;
    }
    }

    /* test all edges of triangle 1 against the edges of triangle 2 */
    EDGE_AGAINST_TRI_EDGES_D(V0, V1, U0, U1, U2);
    EDGE_AGAINST_TRI_EDGES_D(V1, V2, U0, U1, U2);
    EDGE_AGAINST_TRI_EDGES_D(V2, V0, U0, U1, U2);

    int       bInside = 0;

    if (numPts > 0) {
    POINT_IN_TRI_D(V0, U0, U1, U2);
    if (bInside) {
        IsectPt[numPts] = *((lsVec3 *) V0);
        if (AddPoint(IsectPt, numPts))
        numPts++;
    }

    bInside = 0;
    POINT_IN_TRI_D(V1, U0, U1, U2);
    if (bInside) {
        IsectPt[numPts] = *((lsVec3 *) V1);
        if (AddPoint(IsectPt, numPts))
        numPts++;
    }

    bInside = 0;
    POINT_IN_TRI_D(V2, U0, U1, U2);
    if (bInside) {
        IsectPt[numPts] = *((lsVec3 *) V2);
        if (AddPoint(IsectPt, numPts))
        numPts++;
    }

    bInside = 0;
    POINT_IN_TRI_D(U0, V0, V1, V2);
    if (bInside) {
        IsectPt[numPts] = *((lsVec3 *) U0);
        if (AddPoint(IsectPt, numPts))
        numPts++;
    }

    bInside = 0;
    POINT_IN_TRI_D(U1, V0, V1, V2);
    if (bInside) {
        IsectPt[numPts] = *((lsVec3 *) U1);
        if (AddPoint(IsectPt, numPts))
        numPts++;
    }

    bInside = 0;
    POINT_IN_TRI_D(U2, V0, V1, V2);
    if (bInside) {
        IsectPt[numPts] = *((lsVec3 *) U2);
        if (AddPoint(IsectPt, numPts))
        numPts++;
    }

    if (numPts > 3)
        OrderIsectPts(IsectPt, numPts, i0, i1);

    return INTERSECT_COPLANAR;
    }

    /* finally, test if tri1 is totally contained in tri2 or vice versa */

    POINT_IN_TRI_D(V0, U0, U1, U2);
    if (bInside) {
    IsectPt[numPts++] = *((lsVec3 *) V0);
    IsectPt[numPts++] = *((lsVec3 *) V1);
    IsectPt[numPts++] = *((lsVec3 *) V2);
    return INTERSECT_COPLANAR;
    }

    POINT_IN_TRI_D(U0, V0, V1, V2);
    if (bInside) {
    IsectPt[numPts++] = *((lsVec3 *) U0);
    IsectPt[numPts++] = *((lsVec3 *) U1);
    IsectPt[numPts++] = *((lsVec3 *) U2);
    return INTERSECT_COPLANAR;
    }

    return DISJOINT;
}

/**
 * intersection determination
 */
unsigned int
TriTriIsect(const MeReal *V0, const MeReal *V1, const MeReal *V2,
        const MeReal *U0, const MeReal *U1, const MeReal *U2,
        lsVec3 *IsectPt, int numMaxPts, int &numPts)
{
    MeReal    E1[3], E2[3];
    MeReal    N1[3], N2[3], d1, d2;
    MeReal    du0, du1, du2, dv0, dv1, dv2;
    MeReal    D[3];
    MeReal    isect1[2], isect2[2];
    MeReal    du0du1, du0du2, dv0dv1, dv0dv2;
    short     index;
    MeReal    vp0, vp1, vp2;
    MeReal    up0, up1, up2;
    MeReal    b, c, max;

    numPts = 0;

    /* compute plane equation of triangle(V0,V1,V2) */
    SUB(E1, V1, V0);
    SUB(E2, V2, V0);
    CROSS(N1, E1, E2);
    d1 = -DOT(N1, V0);
    /* plane equation 1: N1.X+d1=0 */

    /* put U0,U1,U2 into plane equation 1 to compute signed distances to the plane */
    du0 = DOT(N1, U0) + d1;
    du1 = DOT(N1, U1) + d1;
    du2 = DOT(N1, U2) + d1;

    /* coplanarity robustness check */
#if USE_EPSILON_TEST==TRUE
    if (MeFabs(du0) < ME_SMALL_EPSILON)
    du0 = 0.0;
    if (MeFabs(du1) < ME_SMALL_EPSILON)
    du1 = 0.0;
    if (MeFabs(du2) < ME_SMALL_EPSILON)
    du2 = 0.0;
#endif
    du0du1 = du0 * du1;
    du0du2 = du0 * du2;

    if (du0du1 > 0.0f && du0du2 > 0.0f) /* same sign on all of them + not equal 0 ? */
    return DISJOINT;    /* no intersection occurs */

    /* compute plane of triangle (U0,U1,U2) */
    SUB(E1, U1, U0);
    SUB(E2, U2, U0);
    CROSS(N2, E1, E2);
    d2 = -DOT(N2, U0);
    /* plane equation 2: N2.X+d2=0 */

    /* put V0,V1,V2 into plane equation 2 */
    dv0 = DOT(N2, V0) + d2;
    dv1 = DOT(N2, V1) + d2;
    dv2 = DOT(N2, V2) + d2;

#if USE_EPSILON_TEST==TRUE
    if (MeFabs(dv0) < ME_SMALL_EPSILON)
    dv0 = 0.0;
    if (MeFabs(dv1) < ME_SMALL_EPSILON)
    dv1 = 0.0;
    if (MeFabs(dv2) < ME_SMALL_EPSILON)
    dv2 = 0.0;
#endif

    dv0dv1 = dv0 * dv1;
    dv0dv2 = dv0 * dv2;

    if (dv0dv1 > 0.0f && dv0dv2 > 0.0f) /* same sign on all of them + not equal 0 ? */
    return DISJOINT;    /* no intersection occurs */

    /* compute direction of intersection line */
    CROSS(D, N1, N2);

    /* compute and index to the largest component of D */
    max = (MeReal) MeFabs(D[0]);
    index = 0;
    b = (MeReal) MeFabs(D[1]);
    c = (MeReal) MeFabs(D[2]);
    if (b > max)
    max = b, index = 1;
    if (c > max)
    max = c, index = 2;

    /* this is the simplified projection onto L */
    vp0 = V0[index];
    vp1 = V1[index];
    vp2 = V2[index];

    up0 = U0[index];
    up1 = U1[index];
    up2 = U2[index];

    /* compute interval for triangle 1 */
    COMPUTE_INTERVALS_D(vp0, vp1, vp2, dv0, dv1, dv2, dv0dv1, dv0dv2, isect1[0], isect1[1]);

    /* compute interval for triangle 2 */
    COMPUTE_INTERVALS_D(up0, up1, up2, du0, du1, du2, du0du1, du0du2, isect2[0], isect2[1]);

    SORT(isect1[0], isect1[1]);
    SORT(isect2[0], isect2[1]);

    if (isect1[1] < isect2[0] || isect2[1] < isect1[0])
    return DISJOINT;

    int       id[2];
    getIndices(index, id);
    MeReal    resl;

    numPts = 2;

    // both points from triangle 1
    if (isect1[0] >= isect2[0] && isect1[1] <= isect2[1]) {
    IsectPt[0][index] = isect1[0];
    IsectPt[1][index] = isect1[1];

    intersect_3d(index, id[0], id[1], isect1[0], N1, N2, d1, d2, resl);
    IsectPt[0][id[0]] = resl;
    intersect_3d(index, id[1], id[0], isect1[0], N1, N2, d1, d2, resl);
    IsectPt[0][id[1]] = resl;

    intersect_3d(index, id[0], id[1], isect1[1], N1, N2, d1, d2, resl);
    IsectPt[1][id[0]] = resl;
    intersect_3d(index, id[1], id[0], isect1[1], N1, N2, d1, d2, resl);
    IsectPt[1][id[1]] = resl;

    return INTERSECT_1;
    }
    // both points from triangle 2
    if (isect2[0] >= isect1[0] && isect2[1] <= isect1[1]) {
    IsectPt[0][index] = isect2[0];
    IsectPt[1][index] = isect2[1];

    intersect_3d(index, id[0], id[1], isect2[0], N1, N2, d1, d2, resl);
    IsectPt[0][id[0]] = resl;
    intersect_3d(index, id[1], id[0], isect2[0], N1, N2, d1, d2, resl);
    IsectPt[0][id[1]] = resl;

    intersect_3d(index, id[0], id[1], isect2[1], N1, N2, d1, d2, resl);
    IsectPt[1][id[0]] = resl;
    intersect_3d(index, id[1], id[0], isect2[1], N1, N2, d1, d2, resl);
    IsectPt[1][id[1]] = resl;

    return INTERSECT_2;
    }
    // #1 from left point of triangle 2,
    // #2 from right point of triangle 1
    if (isect2[0] >= isect1[0] && isect2[1] >= isect1[1]) {
    IsectPt[0][index] = isect1[1];
    IsectPt[1][index] = isect2[0];

    intersect_3d(index, id[0], id[1], isect1[1], N1, N2, d1, d2, resl);
    IsectPt[0][id[0]] = resl;
    intersect_3d(index, id[1], id[0], isect1[1], N1, N2, d1, d2, resl);
    IsectPt[0][id[1]] = resl;

    intersect_3d(index, id[0], id[1], isect2[0], N1, N2, d1, d2, resl);
    IsectPt[1][id[0]] = resl;
    intersect_3d(index, id[1], id[0], isect2[0], N1, N2, d1, d2, resl);
    IsectPt[1][id[1]] = resl;

    return INTERSECT_3;
    }
    // #1 from left point of triangle 1,
    // #2 from right point of triangle 2
    if (isect2[0] <= isect1[0] && isect2[1] <= isect1[1]) {
    IsectPt[0][index] = isect2[1];
    IsectPt[1][index] = isect1[0];

    intersect_3d(index, id[0], id[1], isect2[1], N1, N2, d1, d2, resl);
    IsectPt[0][id[0]] = resl;
    intersect_3d(index, id[1], id[0], isect2[1], N1, N2, d1, d2, resl);
    IsectPt[0][id[1]] = resl;

    intersect_3d(index, id[0], id[1], isect1[0], N1, N2, d1, d2, resl);
    IsectPt[1][id[0]] = resl;
    intersect_3d(index, id[1], id[0], isect1[0], N1, N2, d1, d2, resl);
    IsectPt[1][id[1]] = resl;
    }

    return INTERSECT_3;
}
