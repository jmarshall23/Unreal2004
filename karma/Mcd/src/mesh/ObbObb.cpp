/* -*-c++-*-
 *===============================================================
 * File:        ObbObb.cpp
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.6.18.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#include <stdio.h>
#include <math.h>
#include "ObbObb.h"

// static
enum {
    INTERSECTION = 0,
    AXIS_A0, AXIS_A1, AXIS_A2,
    AXIS_B0, AXIS_B1, AXIS_B2,
    AXIS_A0xB0, AXIS_A0xB1, AXIS_A0xB2,
    AXIS_A1xB0, AXIS_A1xB1, AXIS_A1xB2,
    AXIS_A2xB0, AXIS_A2xB1, AXIS_A2xB2
};

//---------------------------------------------------------------------------
// macros for fast arithmetic
//---------------------------------------------------------------------------
#define DIFF(diff,p,q) \
{ \
    diff[0] = p[0]-q[0]; \
    diff[1] = p[1]-q[1]; \
    diff[2] = p[2]-q[2]; \
}
#define DOT(p,q) (p[0]*q[0]+p[1]*q[1]+p[2]*q[2])

//---------------------------------------------------------------------------

const BVReal TOL = (BVReal) 1.0E-6;

#define NON_SIMPLIFY_TEST

//---------------------------------------------------------------------------
unsigned int
separation(const CxObb &box0, const BVTransform & m0, const CxObb &box1, const BVTransform & m1)
{
#ifdef CXTRIANGLE_USE_TIMESTAMP
    const BVVec3 *A = box0.mTmGlobal.row;
    const BVVec3 *B = box1.mTmGlobal.row;
#else
    BVTransform mm0, mm1;
    mm0.thisIsFirstThenSecond(box0.mTm, m0);
    mm1.thisIsFirstThenSecond(box1.mTm, m1);
    const BVVec3 *A = mm0.row;
    const BVVec3 *B = mm1.row;
#endif

    const lsVec3 *extA = &(box0.mBox.ext);
    const lsVec3 *extB = &(box1.mBox.ext);

    BVVec3    D0;       // difference of box centers at time '0'
    BVReal    AB[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    BVReal    fAB[3][3];    // fabs(c_{ij})
    BVReal    AD0[3];       // Dot(A_i,D0)
    BVReal    R0, R1, R;    // interval radii and distance between centers
    BVReal    R01;      // R0+R1

    // compute difference of box centers
#ifdef CXTRIANGLE_USE_TIMESTAMP
    DIFF(D0, box1.mTmGlobal.row[3], box0.mTmGlobal.row[3])
#else
    DIFF(D0, mm1.row[3], mm0.row[3])
#endif
    // axis C0+t*A0
    AB[0][0] = (BVReal) DOT(A[0], B[0]);
    AB[0][1] = (BVReal) DOT(A[0], B[1]);
    AB[0][2] = (BVReal) DOT(A[0], B[2]);

    AD0[0] = (BVReal) DOT(A[0], D0);

    fAB[0][0] = (BVReal) MeFabs(AB[0][0]);
    fAB[0][1] = (BVReal) MeFabs(AB[0][1]);
    fAB[0][2] = (BVReal) MeFabs(AB[0][2]);
    R1 = (BVReal) (extB->v[0] * fAB[0][0] + extB->v[1] * fAB[0][1] + extB->v[2] * fAB[0][2]);
    R01 = (BVReal) (extA->v[0] + R1 + TOL);
    if (AD0[0] > R01)
    return AXIS_A0;
    else if (AD0[0] < -R01)
    return AXIS_A0;

    // axis C0+t*A1
    AB[1][0] = (BVReal) DOT(A[1], B[0]);
    AB[1][1] = (BVReal) DOT(A[1], B[1]);
    AB[1][2] = (BVReal) DOT(A[1], B[2]);
    AD0[1] = (BVReal) DOT(A[1], D0);
    fAB[1][0] = MeFabs(AB[1][0]);
    fAB[1][1] = MeFabs(AB[1][1]);
    fAB[1][2] = MeFabs(AB[1][2]);
    R1 = (BVReal) (extB->v[0] * fAB[1][0] + extB->v[1] * fAB[1][1] + extB->v[2] * fAB[1][2]);
    R01 = (BVReal) (extA->v[1] + R1 + TOL);
    if (AD0[1] > R01)
    return AXIS_A1;
    else if (AD0[1] < -R01)
    return AXIS_A1;

    // axis C0+t*A2
    AB[2][0] = (BVReal) DOT(A[2], B[0]);
    AB[2][1] = (BVReal) DOT(A[2], B[1]);
    AB[2][2] = (BVReal) DOT(A[2], B[2]);
    AD0[2] = (BVReal) DOT(A[2], D0);
    fAB[2][0] = MeFabs(AB[2][0]);
    fAB[2][1] = MeFabs(AB[2][1]);
    fAB[2][2] = MeFabs(AB[2][2]);
    R1 = (BVReal) (extB->v[0] * fAB[2][0] + extB->v[1] * fAB[2][1] + extB->v[2] * fAB[2][2]);
    R01 = (BVReal) (extA->v[2] + R1 + TOL);
    if (AD0[2] > R01)
    return AXIS_A2;
    else if (AD0[2] < -R01)
    return AXIS_A2;

    // axis C0+t*B0
    R0 = extA->v[0] * fAB[0][0] + extA->v[1] * fAB[1][0] + extA->v[2] * fAB[2][0];
    R = (BVReal) DOT(B[0], D0);
    R01 = R0 + extB->v[0] + TOL;
    if (R > R01)
    return AXIS_B0;
    else if (R < -R01)
    return AXIS_B0;

    // axis C0+t*B1
    R0 = extA->v[0] * fAB[0][1] + extA->v[1] * fAB[1][1] + extA->v[2] * fAB[2][1];
    R = (BVReal) DOT(B[1], D0);
    R01 = R0 + extB->v[1] + TOL;
    if (R > R01)
    return AXIS_B1;
    else if (R < -R01)
    return AXIS_B1;

    // axis C0+t*B2
    R0 = extA->v[0] * fAB[0][2] + extA->v[1] * fAB[1][2] + extA->v[2] * fAB[2][2];
    R = (BVReal) DOT(B[2], D0);
    R01 = R0 + extB->v[2] + TOL;
    if (R > R01)
    return AXIS_B2;
    else if (R < -R01)
    return AXIS_B2;

    // axis C0+t*A0xB0
    R0 = extA->v[1] * fAB[2][0] + extA->v[2] * fAB[1][0];
    R1 = extB->v[1] * fAB[0][2] + extB->v[2] * fAB[0][1];
    R = AD0[2] * AB[1][0] - AD0[1] * AB[2][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB0;
    else if (R < -R01)
    return AXIS_A0xB0;

    // axis C0+t*A0xB1
    R0 = extA->v[1] * fAB[2][1] + extA->v[2] * fAB[1][1];
    R1 = extB->v[0] * fAB[0][2] + extB->v[2] * fAB[0][0];
    R = AD0[2] * AB[1][1] - AD0[1] * AB[2][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB1;
    else if (R < -R01)
    return AXIS_A0xB1;

    // axis C0+t*A0xB2
    R0 = extA->v[1] * fAB[2][2] + extA->v[2] * fAB[1][2];
    R1 = extB->v[0] * fAB[0][1] + extB->v[1] * fAB[0][0];
    R = AD0[2] * AB[1][2] - AD0[1] * AB[2][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB2;
    else if (R < -R01)
    return AXIS_A0xB2;

    // axis C0+t*A1xB0
    R0 = extA->v[0] * fAB[2][0] + extA->v[2] * fAB[0][0];
    R1 = extB->v[1] * fAB[1][2] + extB->v[2] * fAB[1][1];
    R = AD0[0] * AB[2][0] - AD0[2] * AB[0][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB0;
    else if (R < -R01)
    return AXIS_A1xB0;

    // axis C0+t*A1xB1
    R0 = extA->v[0] * fAB[2][1] + extA->v[2] * fAB[0][1];
    R1 = extB->v[0] * fAB[1][2] + extB->v[2] * fAB[1][0];
    R = AD0[0] * AB[2][1] - AD0[2] * AB[0][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB1;
    else if (R < -R01)
    return AXIS_A1xB1;

    // axis C0+t*A1xB2
    R0 = extA->v[0] * fAB[2][2] + extA->v[2] * fAB[0][2];
    R1 = extB->v[0] * fAB[1][1] + extB->v[1] * fAB[1][0];
    R = AD0[0] * AB[2][2] - AD0[2] * AB[0][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB2;
    else if (R < -R01)
    return AXIS_A1xB2;

    // axis C0+t*A2xB0
    R0 = extA->v[0] * fAB[1][0] + extA->v[1] * fAB[0][0];
    R1 = extB->v[1] * fAB[2][2] + extB->v[2] * fAB[2][1];
    R = AD0[1] * AB[0][0] - AD0[0] * AB[1][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB0;
    else if (R < -R01)
    return AXIS_A2xB0;

    // axis C0+t*A2xB1
    R0 = extA->v[0] * fAB[1][1] + extA->v[1] * fAB[0][1];
    R1 = extB->v[0] * fAB[2][2] + extB->v[2] * fAB[2][0];
    R = AD0[1] * AB[0][1] - AD0[0] * AB[1][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB1;
    else if (R < -R01)
    return AXIS_A2xB1;

    // axis C0+t*A2xB2
    R0 = extA->v[0] * fAB[1][2] + extA->v[1] * fAB[0][2];
    R1 = extB->v[0] * fAB[2][1] + extB->v[1] * fAB[2][0];
    R = AD0[1] * AB[0][2] - AD0[0] * AB[1][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB2;
    else if (R < -R01)
    return AXIS_A2xB2;

    // intersection occurs
    return INTERSECTION;
}

unsigned int
separation_rect_rect(const CxObb &box0, const BVTransform & m0,
             const CxObb &box1, const BVTransform & m1)
{
#ifdef CXTRIANGLE_USE_TIMESTAMP
    const BVVec3 *A = box0.mTmGlobal.row;
    const BVVec3 *B = box1.mTmGlobal.row;
#else
    BVTransform mm0, mm1;
    mm0.thisIsFirstThenSecond(box0.mTm, m0);
    mm1.thisIsFirstThenSecond(box1.mTm, m1);
    const BVVec3 *A = mm0.row;
    const BVVec3 *B = mm1.row;
#endif

    const lsVec3 *extA = &(box0.mBox.ext);
    const lsVec3 *extB = &(box1.mBox.ext);

    BVVec3    D0;       // difference of box centers at time '0'
    BVReal    AB[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    BVReal    fAB[3][3];    // fabs(c_{ij})
    BVReal    AD0[3];       // Dot(A_i,D0)
    BVReal    R0, R1, R;    // interval radii and distance between centers
    BVReal    R01;      // R0+R1

#ifdef CXTRIANGLE_USE_TIMESTAMP
    DIFF(D0, box1.mTmGlobal.row[3], box0.mTmGlobal.row[3])
#else
    DIFF(D0, mm1.row[3], mm0.row[3])
#endif
    // axis C0+t*A0
    AB[0][0] = (BVReal) DOT(A[0], B[0]);
    AB[0][1] = (BVReal) DOT(A[0], B[1]);
    AB[0][2] = (BVReal) DOT(A[0], B[2]);

    AD0[0] = (BVReal) DOT(A[0], D0);

    fAB[0][0] = (BVReal) MeFabs(AB[0][0]);
    fAB[0][1] = (BVReal) MeFabs(AB[0][1]);
    fAB[0][2] = (BVReal) MeFabs(AB[0][2]);
    R1 = (BVReal) (extB->v[0] * fAB[0][0] + extB->v[1] * fAB[0][1]);
    R01 = (BVReal) (extA->v[0] + R1 + TOL);
    if (AD0[0] > R01)
    return AXIS_A0;
    else if (AD0[0] < -R01)
    return AXIS_A0;

    // axis C0+t*A1
    AB[1][0] = (BVReal) DOT(A[1], B[0]);
    AB[1][1] = (BVReal) DOT(A[1], B[1]);
    AB[1][2] = (BVReal) DOT(A[1], B[2]);
    AD0[1] = (BVReal) DOT(A[1], D0);
    fAB[1][0] = MeFabs(AB[1][0]);
    fAB[1][1] = MeFabs(AB[1][1]);
    fAB[1][2] = MeFabs(AB[1][2]);
    R1 = (BVReal) (extB->v[0] * fAB[1][0] + extB->v[1] * fAB[1][1]);
    R01 = (BVReal) (extA->v[1] + R1 + TOL);
    if (AD0[1] > R01)
    return AXIS_A1;
    else if (AD0[1] < -R01)
    return AXIS_A1;

    // axis C0+t*A2
    AB[2][0] = (BVReal) DOT(A[2], B[0]);
    AB[2][1] = (BVReal) DOT(A[2], B[1]);
    AB[2][2] = (BVReal) DOT(A[2], B[2]);
    AD0[2] = (BVReal) DOT(A[2], D0);
    fAB[2][0] = MeFabs(AB[2][0]);
    fAB[2][1] = MeFabs(AB[2][1]);
    fAB[2][2] = MeFabs(AB[2][2]);
    R1 = (BVReal) (extB->v[0] * fAB[2][0] + extB->v[1] * fAB[2][1]);
    R01 = (BVReal) (R1 + TOL);
    if (AD0[2] > R01)
    return AXIS_A2;
    else if (AD0[2] < -R01)
    return AXIS_A2;

    // axis C0+t*B0
    R0 = extA->v[0] * fAB[0][0] + extA->v[1] * fAB[1][0];
    R = (BVReal) DOT(B[0], D0);
    R01 = R0 + extB->v[0] + TOL;
    if (R > R01)
    return AXIS_B0;
    else if (R < -R01)
    return AXIS_B0;

    // axis C0+t*B1
    R0 = extA->v[0] * fAB[0][1] + extA->v[1] * fAB[1][1];
    R = (BVReal) DOT(B[1], D0);
    R01 = R0 + extB->v[1] + TOL;
    if (R > R01)
    return AXIS_B1;
    else if (R < -R01)
    return AXIS_B1;

    // axis C0+t*B2
    R0 = extA->v[0] * fAB[0][2] + extA->v[1] * fAB[1][2];
    R = (BVReal) DOT(B[2], D0);
    R01 = R0 + TOL;
    if (R > R01)
    return AXIS_B2;
    else if (R < -R01)
    return AXIS_B2;

    // axis C0+t*A0xB0
    R0 = extA->v[1] * fAB[2][0];
    R1 = extB->v[1] * fAB[0][2];
    R = AD0[2] * AB[1][0] - AD0[1] * AB[2][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB0;
    else if (R < -R01)
    return AXIS_A0xB0;

    // axis C0+t*A0xB1
    R0 = extA->v[1] * fAB[2][1];
    R1 = extB->v[0] * fAB[0][2];
    R = AD0[2] * AB[1][1] - AD0[1] * AB[2][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB1;
    else if (R < -R01)
    return AXIS_A0xB1;

    // axis C0+t*A0xB2
    R0 = extA->v[1] * fAB[2][2];
    R1 = extB->v[0] * fAB[0][1] + extB->v[1] * fAB[0][0];
    R = AD0[2] * AB[1][2] - AD0[1] * AB[2][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB2;
    else if (R < -R01)
    return AXIS_A0xB2;

    // axis C0+t*A1xB0
    R0 = extA->v[0] * fAB[2][0];
    R1 = extB->v[1] * fAB[1][2];
    R = AD0[0] * AB[2][0] - AD0[2] * AB[0][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB0;
    else if (R < -R01)
    return AXIS_A1xB0;

    // axis C0+t*A1xB1
    R0 = extA->v[0] * fAB[2][1];
    R1 = extB->v[0] * fAB[1][2];
    R = AD0[0] * AB[2][1] - AD0[2] * AB[0][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB1;
    else if (R < -R01)
    return AXIS_A1xB1;

    // axis C0+t*A1xB2
    R0 = extA->v[0] * fAB[2][2];
    R1 = extB->v[0] * fAB[1][1] + extB->v[1] * fAB[1][0];
    R = AD0[0] * AB[2][2] - AD0[2] * AB[0][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB2;
    else if (R < -R01)
    return AXIS_A1xB2;

    // axis C0+t*A2xB0
    R0 = extA->v[0] * fAB[1][0] + extA->v[1] * fAB[0][0];
    R1 = extB->v[1] * fAB[2][2];
    R = AD0[1] * AB[0][0] - AD0[0] * AB[1][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB0;
    else if (R < -R01)
    return AXIS_A2xB0;

    // axis C0+t*A2xB1
    R0 = extA->v[0] * fAB[1][1] + extA->v[1] * fAB[0][1];
    R1 = extB->v[0] * fAB[2][2];
    R = AD0[1] * AB[0][1] - AD0[0] * AB[1][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB1;
    else if (R < -R01)
    return AXIS_A2xB1;

    // axis C0+t*A2xB2
    R0 = extA->v[0] * fAB[1][2] + extA->v[1] * fAB[0][2];
    R1 = extB->v[0] * fAB[2][1] + extB->v[1] * fAB[2][0];
    R = AD0[1] * AB[0][2] - AD0[0] * AB[1][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB2;
    else if (R < -R01)
    return AXIS_A2xB2;

    // intersection occurs
    return INTERSECTION;
}

//---------------------------------------------------------------------------
unsigned int
separation_rect_box(const CxObb &box0, const BVTransform & m0,
            const CxObb &box1, const BVTransform & m1)
{
#ifdef CXTRIANGLE_USE_TIMESTAMP
    const BVVec3 *A = box0.mTmGlobal.row;
    const BVVec3 *B = box1.mTmGlobal.row;
#else
    BVTransform mm0, mm1;
    mm0.thisIsFirstThenSecond(box0.mTm, m0);
    mm1.thisIsFirstThenSecond(box1.mTm, m1);
    const BVVec3 *A = mm0.row;
    const BVVec3 *B = mm1.row;
#endif

    const lsVec3 *extA = &(box0.mBox.ext);
    const lsVec3 *extB = &(box1.mBox.ext);

    BVVec3    D0;       // difference of box centers at time '0'
    BVReal    AB[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    BVReal    fAB[3][3];    // fabs(c_{ij})
    BVReal    AD0[3];       // Dot(A_i,D0)
    BVReal    R0, R1, R;    // interval radii and distance between centers
    BVReal    R01;      // R0+R1

    // compute difference of box centers
#ifdef CXTRIANGLE_USE_TIMESTAMP
    DIFF(D0, box1.mTmGlobal.row[3], box0.mTmGlobal.row[3])
#else
    DIFF(D0, mm1.row[3], mm0.row[3])
#endif
    // axis C0+t*A0
    AB[0][0] = (BVReal) DOT(A[0], B[0]);
    AB[0][1] = (BVReal) DOT(A[0], B[1]);
    AB[0][2] = (BVReal) DOT(A[0], B[2]);

    AD0[0] = (BVReal) DOT(A[0], D0);

    fAB[0][0] = (BVReal) MeFabs(AB[0][0]);
    fAB[0][1] = (BVReal) MeFabs(AB[0][1]);
    fAB[0][2] = (BVReal) MeFabs(AB[0][2]);
    R1 = (BVReal) (extB->v[0] * fAB[0][0] + extB->v[1] * fAB[0][1] + extB->v[2] * fAB[0][2]);
    R01 = (BVReal) (extA->v[0] + R1 + TOL);
    if (AD0[0] > R01)
    return AXIS_A0;
    else if (AD0[0] < -R01)
    return AXIS_A0;

    // axis C0+t*A1
    AB[1][0] = (BVReal) DOT(A[1], B[0]);
    AB[1][1] = (BVReal) DOT(A[1], B[1]);
    AB[1][2] = (BVReal) DOT(A[1], B[2]);
    AD0[1] = (BVReal) DOT(A[1], D0);
    fAB[1][0] = MeFabs(AB[1][0]);
    fAB[1][1] = MeFabs(AB[1][1]);
    fAB[1][2] = MeFabs(AB[1][2]);
    R1 = (BVReal) (extB->v[0] * fAB[1][0] + extB->v[1] * fAB[1][1] + extB->v[2] * fAB[1][2]);
    R01 = (BVReal) (extA->v[1] + R1 + TOL);
    if (AD0[1] > R01)
    return AXIS_A1;
    else if (AD0[1] < -R01)
    return AXIS_A1;

    // axis C0+t*A2
    AB[2][0] = (BVReal) DOT(A[2], B[0]);
    AB[2][1] = (BVReal) DOT(A[2], B[1]);
    AB[2][2] = (BVReal) DOT(A[2], B[2]);
    AD0[2] = (BVReal) DOT(A[2], D0);
    fAB[2][0] = MeFabs(AB[2][0]);
    fAB[2][1] = MeFabs(AB[2][1]);
    fAB[2][2] = MeFabs(AB[2][2]);
    R1 = (BVReal) (extB->v[0] * fAB[2][0] + extB->v[1] * fAB[2][1] + extB->v[2] * fAB[2][2]);
    R01 = (BVReal) (R1 + TOL);
    if (AD0[2] > R01)
    return AXIS_A2;
    else if (AD0[2] < -R01)
    return AXIS_A2;

    // axis C0+t*B0
    R0 = extA->v[0] * fAB[0][0] + extA->v[1] * fAB[1][0];
    R = (BVReal) DOT(B[0], D0);
    R01 = R0 + extB->v[0] + TOL;
    if (R > R01)
    return AXIS_B0;
    else if (R < -R01)
    return AXIS_B0;

    // axis C0+t*B1
    R0 = extA->v[0] * fAB[0][1] + extA->v[1] * fAB[1][1];
    R = (BVReal) DOT(B[1], D0);
    R01 = R0 + extB->v[1] + TOL;
    if (R > R01)
    return AXIS_B1;
    else if (R < -R01)
    return AXIS_B1;

    // axis C0+t*B2
    R0 = extA->v[0] * fAB[0][2] + extA->v[1] * fAB[1][2];
    R = (BVReal) DOT(B[2], D0);
    R01 = R0 + extB->v[2] + TOL;
    if (R > R01)
    return AXIS_B2;
    else if (R < -R01)
    return AXIS_B2;

    // axis C0+t*A0xB0
    R0 = extA->v[1] * fAB[2][0];
    R1 = extB->v[1] * fAB[0][2] + extB->v[2] * fAB[0][1];
    R = AD0[2] * AB[1][0] - AD0[1] * AB[2][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB0;
    else if (R < -R01)
    return AXIS_A0xB0;

    // axis C0+t*A0xB1
    R0 = extA->v[1] * fAB[2][1];
    R1 = extB->v[0] * fAB[0][2] + extB->v[2] * fAB[0][0];
    R = AD0[2] * AB[1][1] - AD0[1] * AB[2][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB1;
    else if (R < -R01)
    return AXIS_A0xB1;

    // axis C0+t*A0xB2
    R0 = extA->v[1] * fAB[2][2];
    R1 = extB->v[0] * fAB[0][1] + extB->v[1] * fAB[0][0];
    R = AD0[2] * AB[1][2] - AD0[1] * AB[2][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB2;
    else if (R < -R01)
    return AXIS_A0xB2;

    // axis C0+t*A1xB0
    R0 = extA->v[0] * fAB[2][0];
    R1 = extB->v[1] * fAB[1][2] + extB->v[2] * fAB[1][1];
    R = AD0[0] * AB[2][0] - AD0[2] * AB[0][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB0;
    else if (R < -R01)
    return AXIS_A1xB0;

    // axis C0+t*A1xB1
    R0 = extA->v[0] * fAB[2][1];
    R1 = extB->v[0] * fAB[1][2] + extB->v[2] * fAB[1][0];
    R = AD0[0] * AB[2][1] - AD0[2] * AB[0][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB1;
    else if (R < -R01)
    return AXIS_A1xB1;

    // axis C0+t*A1xB2
    R0 = extA->v[0] * fAB[2][2];
    R1 = extB->v[0] * fAB[1][1] + extB->v[1] * fAB[1][0];
    R = AD0[0] * AB[2][2] - AD0[2] * AB[0][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB2;
    else if (R < -R01)
    return AXIS_A1xB2;

    // axis C0+t*A2xB0
    R0 = extA->v[0] * fAB[1][0] + extA->v[1] * fAB[0][0];
    R1 = extB->v[1] * fAB[2][2] + extB->v[2] * fAB[2][1];
    R = AD0[1] * AB[0][0] - AD0[0] * AB[1][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB0;
    else if (R < -R01)
    return AXIS_A2xB0;

    // axis C0+t*A2xB1
    R0 = extA->v[0] * fAB[1][1] + extA->v[1] * fAB[0][1];
    R1 = extB->v[0] * fAB[2][2] + extB->v[2] * fAB[2][0];
    R = AD0[1] * AB[0][1] - AD0[0] * AB[1][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB1;
    else if (R < -R01)
    return AXIS_A2xB1;

    // axis C0+t*A2xB2
    R0 = extA->v[0] * fAB[1][2] + extA->v[1] * fAB[0][2];
    R1 = extB->v[0] * fAB[2][1] + extB->v[1] * fAB[2][0];
    R = AD0[1] * AB[0][2] - AD0[0] * AB[1][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB2;
    else if (R < -R01)
    return AXIS_A2xB2;

    // intersection occurs
    return INTERSECTION;
}

// not in use for now, so comment out for reducing lib size
#if 0

// m transforms a vector in 1's c.s. into a vector in 0's c.s.
unsigned int
separation(const BVTransform & m, MeReal *r_box0, MeReal *r_box1)
{
    // convenience variables
    const BVVec3 *B = m.row;

    BVVec3    D0;       // difference of box centers at time '0'
    BVReal    AB[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    BVReal    fAB[3][3];    // fabs(c_{ij})
    BVReal    AD0[3];       // Dot(A_i,D0)
    BVReal    R0, R1, R;    // interval radii and distance between centers
    BVReal    R01;      // R0+R1

    // compute difference of box centers
    D0 = m.row[3];

    // axis C0+t*A0
    // AB[0][0] = DOT(A[0],B[0]);
    // AB[0][1] = DOT(A[0],B[1]);
    // AB[0][2] = DOT(A[0],B[2]);
    // AD0[0] = DOT(A[0],D0);
    AB[0][0] = B[0][0];
    AB[0][1] = B[1][0];
    AB[0][2] = B[2][0];
    AD0[0] = D0[0];

    fAB[0][0] = MeFabs(AB[0][0]);
    fAB[0][1] = MeFabs(AB[0][1]);
    fAB[0][2] = MeFabs(AB[0][2]);
    R1 = r_box1[0] * fAB[0][0] + r_box1[1] * fAB[0][1] + r_box1[2] * fAB[0][2];
    R01 = r_box0[0] + R1 + TOL;
    if (AD0[0] > R01)
    return AXIS_A0;
    else if (AD0[0] < -R01)
    return AXIS_A0;

    // axis C0+t*A1
    // AB[1][0] = DOT(A[1],B[0]);
    // AB[1][1] = DOT(A[1],B[1]);
    // AB[1][2] = DOT(A[1],B[2]);
    // AD0[1] = DOT(A[1],D0);

    AB[1][0] = B[0][1];
    AB[1][1] = B[1][1];
    AB[1][2] = B[2][1];
    AD0[1] = D0[1];

    fAB[1][0] = MeFabs(AB[1][0]);
    fAB[1][1] = MeFabs(AB[1][1]);
    fAB[1][2] = MeFabs(AB[1][2]);
    R1 = r_box1[0] * fAB[1][0] + r_box1[1] * fAB[1][1] + r_box1[2] * fAB[1][2];
    R01 = r_box0[1] + R1 + TOL;
    if (AD0[1] > R01)
    return AXIS_A1;
    else if (AD0[1] < -R01)
    return AXIS_A1;

    // axis C0+t*A2
    // AB[2][0] = DOT(A[2],B[0]);
    // AB[2][1] = DOT(A[2],B[1]);
    // AB[2][2] = DOT(A[2],B[2]);
    // AD0[2] = DOT(A[2],D0);

    AB[2][0] = B[0][2];
    AB[2][1] = B[1][2];
    AB[2][2] = B[2][2];
    AD0[2] = D0[2];

    fAB[2][0] = MeFabs(AB[2][0]);
    fAB[2][1] = MeFabs(AB[2][1]);
    fAB[2][2] = MeFabs(AB[2][2]);
    R1 = r_box1[0] * fAB[2][0] + r_box1[1] * fAB[2][1] + r_box1[2] * fAB[2][2];
    R01 = r_box0[2] + R1 + TOL;
    if (AD0[2] > R01)
    return AXIS_A2;
    else if (AD0[2] < -R01)
    return AXIS_A2;

    // axis C0+t*B0
    R0 = r_box0[0] * fAB[0][0] + r_box0[1] * fAB[1][0] + r_box0[2] * fAB[2][0];
    R = DOT(B[0], D0);
    R01 = R0 + r_box1[0] + TOL;
    if (R > R01)
    return AXIS_B0;
    else if (R < -R01)
    return AXIS_B0;

    // axis C0+t*B1
    R0 = r_box0[0] * fAB[0][1] + r_box0[1] * fAB[1][1] + r_box0[2] * fAB[2][1];
    R = DOT(B[1], D0);
    R01 = R0 + r_box1[1] + TOL;
    if (R > R01)
    return AXIS_B1;
    else if (R < -R01)
    return AXIS_B1;

    // axis C0+t*B2
    R0 = r_box0[0] * fAB[0][2] + r_box0[1] * fAB[1][2] + r_box0[2] * fAB[2][2];
    R = DOT(B[2], D0);
    R01 = R0 + r_box1[2] + TOL;
    if (R > R01)
    return AXIS_B2;
    else if (R < -R01)
    return AXIS_B2;

    // axis C0+t*A0xB0
    R0 = r_box0[1] * fAB[2][0] + r_box0[2] * fAB[1][0];
    R1 = r_box1[1] * fAB[0][2] + r_box1[2] * fAB[0][1];
    R = AD0[2] * AB[1][0] - AD0[1] * AB[2][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB0;
    else if (R < -R01)
    return AXIS_A0xB0;

    // axis C0+t*A0xB1
    R0 = r_box0[1] * fAB[2][1] + r_box0[2] * fAB[1][1];
    R1 = r_box1[0] * fAB[0][2] + r_box1[2] * fAB[0][0];
    R = AD0[2] * AB[1][1] - AD0[1] * AB[2][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB1;
    else if (R < -R01)
    return AXIS_A0xB1;

    // axis C0+t*A0xB2
    R0 = r_box0[1] * fAB[2][2] + r_box0[2] * fAB[1][2];
    R1 = r_box1[0] * fAB[0][1] + r_box1[1] * fAB[0][0];
    R = AD0[2] * AB[1][2] - AD0[1] * AB[2][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A0xB2;
    else if (R < -R01)
    return AXIS_A0xB2;

    // axis C0+t*A1xB0
    R0 = r_box0[0] * fAB[2][0] + r_box0[2] * fAB[0][0];
    R1 = r_box1[1] * fAB[1][2] + r_box1[2] * fAB[1][1];
    R = AD0[0] * AB[2][0] - AD0[2] * AB[0][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB0;
    else if (R < -R01)
    return AXIS_A1xB0;

    // axis C0+t*A1xB1
    R0 = r_box0[0] * fAB[2][1] + r_box0[2] * fAB[0][1];
    R1 = r_box1[0] * fAB[1][2] + r_box1[2] * fAB[1][0];
    R = AD0[0] * AB[2][1] - AD0[2] * AB[0][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB1;
    else if (R < -R01)
    return AXIS_A1xB1;

    // axis C0+t*A1xB2
    R0 = r_box0[0] * fAB[2][2] + r_box0[2] * fAB[0][2];
    R1 = r_box1[0] * fAB[1][1] + r_box1[1] * fAB[1][0];
    R = AD0[0] * AB[2][2] - AD0[2] * AB[0][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A1xB2;
    else if (R < -R01)
    return AXIS_A1xB2;

    // axis C0+t*A2xB0
    R0 = r_box0[0] * fAB[1][0] + r_box0[1] * fAB[0][0];
    R1 = r_box1[1] * fAB[2][2] + r_box1[2] * fAB[2][1];
    R = AD0[1] * AB[0][0] - AD0[0] * AB[1][0];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB0;
    else if (R < -R01)
    return AXIS_A2xB0;

    // axis C0+t*A2xB1
    R0 = r_box0[0] * fAB[1][1] + r_box0[1] * fAB[0][1];
    R1 = r_box1[0] * fAB[2][2] + r_box1[2] * fAB[2][0];
    R = AD0[1] * AB[0][1] - AD0[0] * AB[1][1];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB1;
    else if (R < -R01)
    return AXIS_A2xB1;

    // axis C0+t*A2xB2
    R0 = r_box0[0] * fAB[1][2] + r_box0[1] * fAB[0][2];
    R1 = r_box1[0] * fAB[2][1] + r_box1[1] * fAB[2][0];
    R = AD0[1] * AB[0][2] - AD0[0] * AB[1][2];
    R01 = R0 + R1 + TOL;
    if (R > R01)
    return AXIS_A2xB2;
    else if (R < -R01)
    return AXIS_A2xB2;

    // intersection occurs
    return INTERSECTION;
}

#endif
