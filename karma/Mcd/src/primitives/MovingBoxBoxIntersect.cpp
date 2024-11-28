/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:58 $ - Revision: $Revision: 1.14.4.1 $

   This software and its accompanying manuals have been developed
   by MathEngine PLC ("MathEngine") and the copyright and all other
   intellectual property rights in them belong to MathEngine. All
   rights conferred by law (including rights under international
   copyright conventions) are reserved to MathEngine. This software
   may also incorporate information which is confidential to
   MathEngine.

   Save to the extent permitted by law, or as otherwise expressly
   permitted by MathEngine, this software and the manuals must not
   be copied (in whole or in part), re-arranged, altered or adapted
   in any way without the prior written consent of the Company. In
   addition, the information contained in the software may not be
   disseminated without the prior written consent of MathEngine.

 */

#include "MovingBoxBoxIntersect.h"
#include "lsTransform.h"
#include <McdBox.h>
#include <stdio.h>

#if 0
enum
{
    INTERSECTION,
    AXIS_A0, AXIS_A1, AXIS_A2,
    AXIS_B0, AXIS_B1, AXIS_B2,
    AXIS_A0xB0, AXIS_A0xB1, AXIS_A0xB2,
    AXIS_A1xB0, AXIS_A1xB1, AXIS_A1xB2,
    AXIS_A2xB0, AXIS_A2xB1, AXIS_A2xB2,
    AXIS_WxA0,  AXIS_WxA1,  AXIS_WxA2,
    AXIS_WxB0,  AXIS_WxB1,  AXIS_WxB2
};
#endif

//---------------------------------------------------------------------------
// macros for fast arithmetic
//---------------------------------------------------------------------------
#define DIFF(diff,p,q) \
{ \
    diff[0] = p[0]-q[0]; \
    diff[1] = p[1]-q[1]; \
    diff[2] = p[2]-q[2]; \
}
//---------------------------------------------------------------------------
#define DOT(p,q) \
    (p[0]*q[0]+p[1]*q[1]+p[2]*q[2])
//---------------------------------------------------------------------------
#define CROSS(cross,p,q) \
{ \
    cross[0] = p[1]*q[2]-p[2]*q[1]; \
    cross[1] = p[2]*q[0]-p[0]*q[2]; \
    cross[2] = p[0]*q[1]-p[1]*q[0]; \
}
//---------------------------------------------------------------------------
#define COMBO(combo,p,t,q) \
{ \
    combo[0] = p[0]+t*q[0]; \
    combo[1] = p[1]+t*q[1]; \
    combo[2] = p[2]+t*q[2]; \
}
//---------------------------------------------------------------------------

#define FIND0(p,q,r,t,tmax) \
{ \
    MeReal tmp; \
    if ( (p) > (r) ) \
    { \
        if ( (q) > (r) ) \
            return 0; /*axis;*/ \
        tmp = (t)*((r)-(p))/((q)-(p)); \
        if ( tmp > tmax ) \
        { \
            tmax = tmp; \
        } \
    } \
    else if ( (p) < -(r) ) \
    { \
        if ( (q) < -(r) ) \
            return 0;/*axis;*/ \
        tmp = -(t)*((r)+(p))/((q)-(p)); \
        if ( tmp > tmax ) \
        { \
            tmax = tmp; \
        } \
    } \
}

//---------------------------------------------------------------------------
#define FIND1(p,q0,q1,q2,q3,r,t,tmax) \
{ \
    MeReal q, tmp; \
    if ( (p) > (r) ) \
    { \
        q = (q0)*(q1)-(q2)*(q3); \
        if ( q > (r) ) \
            return 0; /*axis;*/ \
        tmp = (t)*((r)-(p))/(q-(p)); \
        if ( tmp > tmax ) \
        { \
            tmax = tmp; \
        } \
    } \
    else if ( (p) < -(r) ) \
    { \
        q = (q0)*(q1)-(q2)*(q3); \
        if ( q < -(r) ) \
            return 0; /*axis;*/ \
        tmp = -(t)*((r)+(p))/(q-(p)); \
        if ( tmp > tmax ) \
        { \
            tmax = tmp; \
        } \
    } \
}
//---------------------------------------------------------------------------
#define GET_COEFF(coeff,sign0,sign1,side,cmat,ext) \
{ \
    if ( cmat > 0.0f ) \
    { \
        coeff = sign0 side*ext; \
    } \
    else if ( cmat < 0.0f ) \
    { \
        coeff = sign1 side*ext; \
    } \
    else \
    { \
        coeff = 0.0f; \
    } \
}
//---------------------------------------------------------------------------
#define GET_POINT(P,box,T,V,coeff) \
{ \
    lsTransform *tm = McdModelGetTransformPtr(box); \
    const lsVec3& center = tm->t(); \
    lsVec3 axis[3]; \
    axis[0] = tm->axis(0); \
    axis[1] = tm->axis(1); \
    axis[2] = tm->axis(2); \
    for (int k = 0; k < 3; k++) \
    { \
        P[k] = center[k] + T*V[k] + coeff[0]*axis[0][k] + \
            coeff[1]*axis[1][k] + coeff[2]*axis[2][k];    \
    } \
}
//---------------------------------------------------------------------------

void printV3( char* str, const lsVec3 &V3 )
{
  printf("%s: %f %f %f \n", str, V3.v[0], V3.v[1], V3.v[2]);
}

//---------------------------------------------------------------------------
unsigned int
MovingBoxBoxIntersect(  const MeReal* ExtBox0, const lsTransform* tm0, const lsVec3& V0,
                        const MeReal* ExtBox1, const lsTransform* tm1, const lsVec3& V1,
                        MeReal dt, MeReal& T, lsVec3& P )
{
    const lsVec3 *(A[3]), *(B[3]);

    A[0] = &(tm0->axis(0));
    A[1] = &(tm0->axis(1));
    A[2] = &(tm0->axis(2));

    B[0] = &(tm1->axis(0));
    B[1] = &(tm1->axis(1));
    B[2] = &(tm1->axis(2));

    lsVec3 W;          // relative velocity between boxes
    lsVec3 D0;         // difference of box centers at time '0'
    lsVec3 D1;         // difference of box centers at time 'dt'
    MeReal AB[3][3];   // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    MeReal fAB[3][3];  // fabs(c_{ij})
    MeReal AD0[3];     // Dot(A_i,D0)
    MeReal AD1[3];     // Dot(A_i,D1)
    MeReal BD0[3];     // Dot(B_i,D0)
    MeReal BD1[3];     // Dot(B_i,D1)
    MeReal AR1[3], BR0[3];
    MeReal R0, R1, R;  // interval radii and distance between centers
    MeReal R01;        // R0+R1

    // Compute relative velocity of box1 with respect to box0 so that box0
    // may as well be stationary.
    DIFF(W.v,V1.v,V0.v);

    // compute difference of box centers (time 0 and time dt)
    // D0 = box1.GetWorldCenter() - box0.GetWorldCenter();
    D0 = tm1->t() - tm0->t();
    COMBO(D1,D0,dt,W);

    // track maximum time of projection-intersection
    unsigned int type = 1;
    int side = 0;
    T = 0;

    // axis C0+t*A0
    AB[0][0] = DOT(A[0]->v,B[0]->v);
    AB[0][1] = DOT(A[0]->v,B[1]->v);
    AB[0][2] = DOT(A[0]->v,B[2]->v);
    AD0[0] = DOT(A[0]->v,D0.v);
    AD1[0] = DOT(A[0]->v,D1.v);
    fAB[0][0] = MeFabs(AB[0][0]);
    fAB[0][1] = MeFabs(AB[0][1]);
    fAB[0][2] = MeFabs(AB[0][2]);
    AR1[0] = ExtBox1[0]*fAB[0][0] + ExtBox1[1]*fAB[0][1] + ExtBox1[2]*fAB[0][2];
    R01 = ExtBox0[0] + AR1[0];
    FIND0(AD0[0],AD1[0],R01,dt,T);

    // axis C0+t*A1
    AB[1][0] = DOT(A[1]->v,B[0]->v);
    AB[1][1] = DOT(A[1]->v,B[1]->v);
    AB[1][2] = DOT(A[1]->v,B[2]->v);
    AD0[1] = DOT(A[1]->v,D0.v);
    AD1[1] = DOT(A[1]->v,D1.v);
    fAB[1][0] = MeFabs(AB[1][0]);
    fAB[1][1] = MeFabs(AB[1][1]);
    fAB[1][2] = MeFabs(AB[1][2]);
    AR1[1] = ExtBox1[0]*fAB[1][0] + ExtBox1[1]*fAB[1][1] + ExtBox1[2]*fAB[1][2];
    R01 = ExtBox0[1] + AR1[1];
    FIND0(AD0[1],AD1[1],R01,dt,T);

    // axis C0+t*A2
    AB[2][0] = DOT(A[2]->v,B[0]->v);
    AB[2][1] = DOT(A[2]->v,B[1]->v);
    AB[2][2] = DOT(A[2]->v,B[2]->v);
    AD0[2] = DOT(A[2]->v,D0.v);
    AD1[2] = DOT(A[2]->v,D1.v);
    fAB[2][0] = MeFabs(AB[2][0]);
    fAB[2][1] = MeFabs(AB[2][1]);
    fAB[2][2] = MeFabs(AB[2][2]);
    AR1[2] = ExtBox1[0]*fAB[2][0] + ExtBox1[1]*fAB[2][1] + ExtBox1[2]*fAB[2][2];
    R01 = ExtBox0[2] + AR1[2];
    FIND0(AD0[2],AD1[2],R01,dt,T);

    // axis C0+t*B0
    BD0[0] = DOT(B[0]->v,D0.v);
    BD1[0] = DOT(B[0]->v,D1.v);
    BR0[0] = ExtBox0[0]*fAB[0][0] + ExtBox0[1]*fAB[1][0] + ExtBox0[2]*fAB[2][0];
    R01 = BR0[0] + ExtBox1[0];
    FIND0(BD0[0],BD1[0],R01,dt,T);

    // axis C0+t*B1
    BD0[1] = DOT(B[1]->v,D0.v);
    BD1[1] = DOT(B[1]->v,D1.v);
    BR0[1] = ExtBox0[0]*fAB[0][1] + ExtBox0[1]*fAB[1][1] + ExtBox0[2]*fAB[2][1];
    R01 = BR0[1] + ExtBox1[1];
    FIND0(BD0[1],BD1[1],R01,dt,T);

    // axis C0+t*B2
    BD0[2] = DOT(B[2]->v,D0.v);
    BD1[2] = DOT(B[2]->v,D1.v);
    BR0[2] = ExtBox0[0]*fAB[0][2] + ExtBox0[1]*fAB[1][2] + ExtBox0[2]*fAB[2][2];
    R01 = BR0[2] + ExtBox1[2];
    FIND0(BD0[2],BD1[2],R01,dt,T);

    // axis C0+t*A0xB0
    R0 = ExtBox0[1]*fAB[2][0] + ExtBox0[2]*fAB[1][0];
    R1 = ExtBox1[1]*fAB[0][2] + ExtBox1[2]*fAB[0][1];
    R = AD0[2]*AB[1][0]-AD0[1]*AB[2][0];
    R01 = R0 + R1;
    FIND1(R,AD1[2],AB[1][0],AD1[1],AB[2][0],R01,dt,T);

    // axis C0+t*A0xB1
    R0 = ExtBox0[1]*fAB[2][1] + ExtBox0[2]*fAB[1][1];
    R1 = ExtBox1[0]*fAB[0][2] + ExtBox1[2]*fAB[0][0];
    R = AD0[2]*AB[1][1]-AD0[1]*AB[2][1];
    R01 = R0 + R1;
    FIND1(R,AD1[2],AB[1][1],AD1[1],AB[2][1],R01,dt,T);

    // axis C0+t*A0xB2
    R0 = ExtBox0[1]*fAB[2][2] + ExtBox0[2]*fAB[1][2];
    R1 = ExtBox1[0]*fAB[0][1] + ExtBox1[1]*fAB[0][0];
    R = AD0[2]*AB[1][2]-AD0[1]*AB[2][2];
    R01 = R0 + R1;
    FIND1(R,AD1[2],AB[1][2],AD1[1],AB[2][2],R01,dt,T);

    // axis C0+t*A1xB0
    R0 = ExtBox0[0]*fAB[2][0] + ExtBox0[2]*fAB[0][0];
    R1 = ExtBox1[1]*fAB[1][2] + ExtBox1[2]*fAB[1][1];
    R = AD0[0]*AB[2][0]-AD0[2]*AB[0][0];
    R01 = R0 + R1;
    FIND1(R,AD1[0],AB[2][0],AD1[2],AB[0][0],R01,dt,T);

    // axis C0+t*A1xB1
    R0 = ExtBox0[0]*fAB[2][1] + ExtBox0[2]*fAB[0][1];
    R1 = ExtBox1[0]*fAB[1][2] + ExtBox1[2]*fAB[1][0];
    R = AD0[0]*AB[2][1]-AD0[2]*AB[0][1];
    R01 = R0 + R1;
    FIND1(R,AD1[0],AB[2][1],AD1[2],AB[0][1],R01,dt,T);

    // axis C0+t*A1xB2
    R0 = ExtBox0[0]*fAB[2][2] + ExtBox0[2]*fAB[0][2];
    R1 = ExtBox1[0]*fAB[1][1] + ExtBox1[1]*fAB[1][0];
    R = AD0[0]*AB[2][2]-AD0[2]*AB[0][2];
    R01 = R0 + R1;
    FIND1(R,AD1[0],AB[2][2],AD1[2],AB[0][2],R01,dt,T);

    // axis C0+t*A2xB0
    R0 = ExtBox0[0]*fAB[1][0] + ExtBox0[1]*fAB[0][0];
    R1 = ExtBox1[1]*fAB[2][2] + ExtBox1[2]*fAB[2][1];
    R = AD0[1]*AB[0][0]-AD0[0]*AB[1][0];
    R01 = R0 + R1;
    FIND1(R,AD1[1],AB[0][0],AD1[0],AB[1][0],R01,dt,T);

    // axis C0+t*A2xB1
    R0 = ExtBox0[0]*fAB[1][1] + ExtBox0[1]*fAB[0][1];
    R1 = ExtBox1[0]*fAB[2][2] + ExtBox1[2]*fAB[2][0];
    R = AD0[1]*AB[0][1]-AD0[0]*AB[1][1];
    R01 = R0 + R1;
    FIND1(R,AD1[1],AB[0][1],AD1[0],AB[1][1],R01,dt,T);

    // axis C0+t*A2xB2
    R0 = ExtBox0[0]*fAB[1][2] + ExtBox0[1]*fAB[0][2];
    R1 = ExtBox1[0]*fAB[2][1] + ExtBox1[1]*fAB[2][0];
    R = AD0[1]*AB[0][2]-AD0[0]*AB[1][2];
    R01 = R0 + R1;
    FIND1(R,AD1[1],AB[0][2],AD1[0],AB[1][2],R01,dt,T);

#if 0
    // At this point none of the 15 axes separate the boxes.  It is still
    // possible that they are separated as viewed in any plane orthogonal
    // to the relative direction of motion W.  In the worst case, the two
    // projected boxes are hexagons.  This requires three separating axis
    // tests per box.

    lsVec3 WxD0 = W.cross(D0);

    MeReal WA[3], WB[3];

    // axis C0 + t*WxA0
    WA[1] = W.dot(*A[1]);
    WA[2] = W.dot(*A[2]);
    R = MeFabs(A[0]->dot(WxD0));
    R0 = ExtBox0[1]*WA[2] + ExtBox0[2]*WA[1];
    R1 =
        ExtBox1[0]*MeFabs(AB[1][0]*WA[2] - AB[2][0]*WA[1]) +
        ExtBox1[1]*MeFabs(AB[1][1]*WA[2] - AB[2][1]*WA[1]) +
        ExtBox1[2]*MeFabs(AB[1][2]*WA[2] - AB[2][2]*WA[1]);
    R01 = R0 + R1;
    if ( R > R01 )
        return 0;// AXIS_WxA0; // 16

    // axis C0 + t*WxA1
    WA[0] = W.dot(*A[0]);
    R = MeFabs(A[1]->dot(WxD0));
    R0 = ExtBox0[2]*WA[0] + ExtBox0[0]*WA[2];
    R1 =
        ExtBox1[0]*MeFabs(AB[2][0]*WA[0] - AB[0][0]*WA[2]) +
        ExtBox1[1]*MeFabs(AB[2][1]*WA[0] - AB[0][1]*WA[2]) +
        ExtBox1[2]*MeFabs(AB[2][2]*WA[0] - AB[0][2]*WA[2]);
    R01 = R0 + R1;
    if ( R > R01 )
        return 0; //AXIS_WxA1; // 17

    // axis C0 + t*WxA2
    R = MeFabs(A[2]->dot(WxD0));
    R0 = ExtBox0[0]*WA[1] + ExtBox0[1]*WA[0];
    R1 =
        ExtBox1[0]*MeFabs(AB[0][0]*WA[1] - AB[1][0]*WA[0]) +
        ExtBox1[1]*MeFabs(AB[0][1]*WA[1] - AB[1][1]*WA[0]) +
        ExtBox1[2]*MeFabs(AB[0][2]*WA[1] - AB[1][2]*WA[0]);
    R01 = R0 + R1;
    if ( R > R01 )
        return 0;// AXIS_WxA2;

    // axis C0 + t*WxB0
    WB[1] = W.dot(*B[1]);
    WB[2] = W.dot(*B[2]);
    R = MeFabs(B[0]->dot(WxD0));
    R0 =
        ExtBox0[0]*MeFabs(AB[0][1]*WB[2] - AB[0][2]*WB[1]) +
        ExtBox0[1]*MeFabs(AB[1][1]*WB[2] - AB[1][2]*WB[1]) +
        ExtBox0[2]*MeFabs(AB[2][1]*WB[2] - AB[2][2]*WB[1]);
    R1 = ExtBox1[1]*WB[2] + ExtBox1[2]*WB[1];
    R01 = R0 + R1;
    if ( R > R01 )
        return 0;// AXIS_WxB0;

    // axis C0 + t*WxB1
    WB[0] = W.dot(*B[0]);
    R = MeFabs(B[1]->dot(WxD0));
    R0 =
        ExtBox0[0]*MeFabs(AB[0][2]*WB[0] - AB[0][0]*WB[2]) +
        ExtBox0[1]*MeFabs(AB[1][2]*WB[0] - AB[1][0]*WB[2]) +
        ExtBox0[2]*MeFabs(AB[2][2]*WB[0] - AB[2][0]*WB[2]);
    R1 = ExtBox1[2]*WB[0] + ExtBox1[0]*WB[2];
    R01 = R0 + R1;
    if ( R > R01 )
        return 0;//AXIS_WxB1;

    // axis C0 + t*WxB2
    R = MeFabs(B[2]->dot(WxD0));
    R0 =
        ExtBox0[0]*MeFabs(AB[0][0]*WB[1] - AB[0][1]*WB[0]) +
        ExtBox0[1]*MeFabs(AB[1][0]*WB[1] - AB[1][1]*WB[0]) +
        ExtBox0[2]*MeFabs(AB[2][0]*WB[1] - AB[2][1]*WB[0]);
    R1 = ExtBox1[0]*WB[1] + ExtBox1[1]*WB[0];
    R01 = R0 + R1;
    if ( R > R01 )
        return 0;//AXIS_WxB2;


    // determine the point of intersection
    const MeReal epsilon = 1e-06f;
    int i, j;
    MeReal x[3], y[3], ad, bd, div, tmp;
    lsVec3 D;

    switch ( type )
    {
        case AXIS_A0:
        case AXIS_A1:
        case AXIS_A2:
        {
            i = type-1;
            for (j = 0; j < 3; j++)
            {
                GET_COEFF(y[j],-,+,side,AB[i][j],ExtBox1[j]);
            }
            GET_POINT(P,cmbox1,T,V1,y);
            break;
        }
        case AXIS_B0:
        case AXIS_B1:
        case AXIS_B2:
        {
            j = type-4;
            for (i = 0; i < 3; i++)
            {
                GET_COEFF(x[i],+,-,side,AB[i][j],ExtBox0[i]);
            }
            GET_POINT(P,cmbox0,T,V0,x);
            break;
        }
        case AXIS_A0xB0:
        {
            GET_COEFF(x[1],-,+,side,AB[2][0],ExtBox0[1]);
            GET_COEFF(x[2],+,-,side,AB[1][0],ExtBox0[2]);
            GET_COEFF(y[1],-,+,side,AB[0][2],ExtBox1[1]);
            GET_COEFF(y[2],+,-,side,AB[0][1],ExtBox1[2]);
            div = 1.0f-AB[0][0]*AB[0][0];
            if ( MeFabs(div) > epsilon )
            {
                COMBO(D,D0,T,W);
                ad = DOT(A[0]->v,D);
                bd = DOT(B[0]->v,D);
                tmp = AB[1][0]*x[1]+AB[2][0]*x[2]-bd;
                x[0] = (ad+AB[0][0]*tmp+AB[0][1]*y[1]+AB[0][2]*y[2])/div;
            }
            else
            {
                x[0] = 0.0f;
            }
            GET_POINT(P,cmbox0,T,V0,x);
            break;
        }
        case AXIS_A0xB1:
        {
            GET_COEFF(x[1],-,+,side,AB[2][1],ExtBox0[1]);
            GET_COEFF(x[2],+,-,side,AB[1][1],ExtBox0[2]);
            GET_COEFF(y[0],+,-,side,AB[0][2],ExtBox1[0]);
            GET_COEFF(y[2],-,+,side,AB[0][0],ExtBox1[2]);
            div = 1.0f-AB[0][1]*AB[0][1];
            if ( MeFabs(div) > epsilon )
            {
                COMBO(D,D0,T,W);
                ad = DOT(A[0]->v,D);
                bd = DOT(B[1]->v,D);
                tmp = AB[1][1]*x[1]+AB[2][1]*x[2]-bd;
                x[0] = (ad+AB[0][1]*tmp+AB[0][0]*y[0]+AB[0][2]*y[2])/div;
            }
            else
            {
                x[0] = 0.0f;
            }
            GET_POINT(P,cmbox0,T,V0,x);
            break;
        }
        case AXIS_A0xB2:
        {
            GET_COEFF(x[1],-,+,side,AB[2][2],ExtBox0[1]);
            GET_COEFF(x[2],+,-,side,AB[1][2],ExtBox0[2]);
            GET_COEFF(y[0],-,+,side,AB[0][1],ExtBox1[0]);
            GET_COEFF(y[1],+,-,side,AB[0][0],ExtBox1[1]);
            div = 1.0f-AB[0][2]*AB[0][2];
            if ( MeFabs(div) > epsilon )
            {
                COMBO(D,D0,T,W);
                ad = DOT(A[0]->v,D);
                bd = DOT(B[2]->v,D);
                tmp = AB[1][2]*x[1]+AB[2][2]*x[2]-bd;
                x[0] = (ad+AB[0][2]*tmp+AB[0][0]*y[0]+AB[0][1]*y[1])/div;
            }
            else
            {
                x[0] = 0.0f;
            }
            GET_POINT(P,cmbox0,T,V0,x);
            break;
        }
        case AXIS_A1xB0:
        {
            GET_COEFF(x[0],+,-,side,AB[2][0],ExtBox0[0]);
            GET_COEFF(x[2],-,+,side,AB[0][0],ExtBox0[2]);
            GET_COEFF(y[1],-,+,side,AB[1][2],ExtBox1[1]);
            GET_COEFF(y[2],+,-,side,AB[1][1],ExtBox1[2]);
            div = 1.0f-AB[1][0]*AB[1][0];
            if ( MeFabs(div) > epsilon )
            {
                COMBO(D,D0,T,W);
                ad = DOT(A[1]->v,D);
                bd = DOT(B[0]->v,D);
                tmp = AB[0][0]*x[0]+AB[2][0]*x[2]-bd;
                x[1] = (ad+AB[1][0]*tmp+AB[1][1]*y[1]+AB[1][2]*y[2])/div;
            }
            else
            {
                x[1] = 0.0f;
            }
            GET_POINT(P,cmbox0,T,V0,x);
            break;
        }
        case AXIS_A1xB1:
        {
            GET_COEFF(x[0],+,-,side,AB[2][1],ExtBox0[0]);
            GET_COEFF(x[2],-,+,side,AB[0][1],ExtBox0[2]);
            GET_COEFF(y[0],+,-,side,AB[1][2],ExtBox1[0]);
            GET_COEFF(y[2],-,+,side,AB[1][0],ExtBox1[2]);
            div = 1.0f-AB[1][1]*AB[1][1];
            if ( MeFabs(div) > epsilon )
            {
                COMBO(D,D0,T,W);
                ad = DOT(A[1]->v,D);
                bd = DOT(B[1]->v,D);
                tmp = AB[0][1]*x[0]+AB[2][1]*x[2]-bd;
                x[1] = (ad+AB[1][1]*tmp+AB[1][0]*y[0]+AB[1][2]*y[2])/div;
            }
            else
            {
                x[1] = 0.0f;
            }
            GET_POINT(P,cmbox0,T,V0,x);
            break;
        }
        case AXIS_A1xB2:
        {
            GET_COEFF(x[0],+,-,side,AB[2][2],ExtBox0[0]);
            GET_COEFF(x[2],-,+,side,AB[0][2],ExtBox0[2]);
            GET_COEFF(y[0],-,+,side,AB[1][1],ExtBox1[0]);
            GET_COEFF(y[1],+,-,side,AB[1][0],ExtBox1[1]);
            div = 1.0f-AB[1][2]*AB[1][2];
            if ( MeFabs(div) > epsilon )
            {
                COMBO(D,D0,T,W);
                ad = DOT(A[1]->v,D);
                bd = DOT(B[2]->v,D);
                tmp = AB[0][2]*x[0]+AB[2][2]*x[2]-bd;
                x[1] = (ad+AB[1][2]*tmp+AB[1][0]*y[0]+AB[1][1]*y[1])/div;
            }
            else
            {
                x[1] = 0.0f;
            }
            GET_POINT(P,cmbox0,T,V0,x);
            break;
        }
        case AXIS_A2xB0:
        {
            GET_COEFF(x[0],-,+,side,AB[1][0],ExtBox0[0]);
            GET_COEFF(x[1],+,-,side,AB[0][0],ExtBox0[1]);
            GET_COEFF(y[1],-,+,side,AB[2][2],ExtBox1[1]);
            GET_COEFF(y[2],+,-,side,AB[2][1],ExtBox1[2]);
            div = 1.0f-AB[2][0]*AB[2][0];
            if ( MeFabs(div) > epsilon )
            {
                COMBO(D,D0,T,W);
                ad = DOT(A[2]->v,D);
                bd = DOT(B[0]->v,D);
                tmp = AB[0][0]*x[0]+AB[1][0]*x[1]-bd;
                x[2] = (ad+AB[2][0]*tmp+AB[2][1]*y[1]+AB[2][2]*y[2])/div;
            }
            else
            {
                x[2] = 0.0f;
            }
            GET_POINT(P,cmbox0,T,V0,x);
            break;
        }
        case AXIS_A2xB1:
        {
            GET_COEFF(x[0],-,+,side,AB[1][1],ExtBox0[0]);
            GET_COEFF(x[1],+,-,side,AB[0][1],ExtBox0[1]);
            GET_COEFF(y[0],+,-,side,AB[2][2],ExtBox1[0]);
            GET_COEFF(y[2],-,+,side,AB[2][0],ExtBox1[2]);
            div = 1.0f-AB[2][1]*AB[2][1];
            if ( MeFabs(div) > epsilon )
            {
                COMBO(D,D0,T,W);
                ad = DOT(A[2]->v,D);
                bd = DOT(B[1]->v,D);
                tmp = AB[0][1]*x[0]+AB[1][1]*x[1]-bd;
                x[2] = (ad+AB[2][1]*tmp+AB[2][0]*y[0]+AB[2][2]*y[2])/div;
            }
            else
            {
                x[2] = 0.0f;
            }
            GET_POINT(P,cmbox0,T,V0,x);
            break;
        }
        case AXIS_A2xB2:
        {
            GET_COEFF(x[0],-,+,side,AB[1][2],ExtBox0[0]);
            GET_COEFF(x[1],+,-,side,AB[0][2],ExtBox0[1]);
            GET_COEFF(y[0],-,+,side,AB[2][1],ExtBox1[0]);
            GET_COEFF(y[1],+,-,side,AB[2][0],ExtBox1[1]);
            div = 1.0f-AB[2][2]*AB[2][2];
            if ( MeFabs(div) > epsilon )
            {
                COMBO(D,D0,T,W);
                ad = DOT(A[2]->v,D);
                bd = DOT(B[2]->v,D);
                tmp = AB[0][2]*x[0]+AB[1][2]*x[1]-bd;
                x[2] = (ad+AB[2][2]*tmp+AB[2][0]*y[0]+AB[2][1]*y[1])/div;
            }
            else
            {
                x[2] = 0.0f;
            }
            GET_POINT(P,cmbox0,T,V0,x);
            break;
        }
    };
#endif
    // assert: type != 0

    return 1;
}
//---------------------------------------------------------------------------
