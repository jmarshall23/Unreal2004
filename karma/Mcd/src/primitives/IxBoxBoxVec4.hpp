#ifndef _IXBOXBOXVEC4_HPP
#define _IXBOXBOXVEC4_HPP

#include "lsVec4.h"

/*
    Sort of like PS2/SSE ``intrinsics'', after an initial version by
    DilipS.

    All this stuff is probably going into its own file eventually.

    The purpose is to allow a SIMD-like transformation of code, while
    staying in C so type checking is done. Not so much so that it is
    faster (but it will be anyhow), but so that it can be safely
    transformed to something strighforwardly mappable to PS2 VU
    instructions or x86 SSE intrinsics.
*/

static const lsVec4 Vec4NegOne          (-1.0f,-1.0f,-1.0f,-1.0f);
static const lsVec4 Vec4One             ( 1.0f, 1.0f, 1.0f, 1.0f);
static const lsVec4 Vec4Zero            ( 0.0f, 0.0f, 0.0f, 0.0f);

/*
    'Vec4' predicates against 'float'.
*/

static inline unsigned Vec4Bool_LessEqualF(const lsVec4 &a, const MeReal b)
{
    return
          (a[0] <= b) << 0
        | (a[1] <= b) << 1
        | (a[2] <= b) << 2
        | (a[3] <= b) << 3;
}

static inline unsigned Vec4Bool_LessEqual4(const lsVec4 &a, const lsVec4 &b)
{
    return
          (a[0] <= b[0]) << 0
        | (a[1] <= b[1]) << 1
        | (a[2] <= b[2]) << 2
        | (a[3] <= b[3]) << 3;
}

/*
    'Vec4' operations using ``broadcast'' floats.
*/

static inline void Vec4Set_F(lsVec4 &out,const MeReal r)
{
    out[0] = out[1] = out[2] = out[3] = r;
}
static inline void Vec4Set_Add4F(lsVec4 &x,const lsVec4 &a,const MeReal b)
{
    x[0] = a[0]+b;
    x[1] = a[1]+b;
    x[2] = a[2]+b;
    x[3] = a[3]+b;
}

static inline void Vec4Set_Sub4F(lsVec4 &x,const lsVec4 &a,const MeReal b)
{
    x[0] = a[0]-b;
    x[1] = a[1]-b;
    x[2] = a[2]-b;
    x[3] = a[3]-b;
}

static inline void Vec4Set_Mul4F(lsVec4 &x,const lsVec4 &a,const MeReal b)
{
    x[0] = a[0]*b;
    x[1] = a[1]*b;
    x[2] = a[2]*b;
    x[3] = a[3]*b;
}

static inline void Vec4Set_Div4F(lsVec4 &x,const lsVec4 &a,const MeReal b)
{
    x[0] = a[0]/b;
    x[1] = a[1]/b;
    x[2] = a[2]/b;
    x[3] = a[3]/b;
}

/*
    'Vec4' operations with 'Vec3's.
*/

static inline void Vec4Set_Vec3Z(lsVec4 &x,const lsVec3 &a)
{
    x[0] = a[0];
    x[1] = a[1];
    x[2] = a[2];
    x[3] = (MeReal) 0.00f;
}

/*
    'Vec4' operations with one 'Vec4' operand.
*/

static inline void Vec4Set_Abs4(lsVec4 &x,const lsVec4 &a)
{
    x[0] = MeFabs(a[0]);
    x[1] = MeFabs(a[1]);
    x[2] = MeFabs(a[2]);
    x[3] = MeFabs(a[3]);
}

/*
    'Vec4' operations with two 'Vec4' operands.
*/

static inline void Vec4Set_Mul44(lsVec4 &x,
    const lsVec4 &a,const lsVec4 &b)
{
    x[0] = a[0] * b[0];
    x[1] = a[1] * b[1];
    x[2] = a[2] * b[2];
    x[3] = a[3] * b[3];
}

static inline void Vec4Set_Min44(lsVec4 &out,
    const lsVec4 &in1,const lsVec4 &in2)
{
    out[0] = (in1[0] < in2[0]) ? in1[0] : in2[0];
    out[1] = (in1[1] < in2[1]) ? in1[1] : in2[1];
    out[2] = (in1[2] < in2[2]) ? in1[2] : in2[2];
    out[3] = (in1[3] < in2[3]) ? in1[3] : in2[3];
}

static inline void Vec4Set_Max44(lsVec4 &out,
    const lsVec4 &in1,const lsVec4 &in2)
{
    out[0] = (in1[0] > in2[0]) ? in1[0] : in2[0];
    out[1] = (in1[1] > in2[1]) ? in1[1] : in2[1];
    out[2] = (in1[2] > in2[2]) ? in1[2] : in2[2];
    out[3] = (in1[3] > in2[3]) ? in1[3] : in2[3];
}

/*
    'Vec4' operations with two 'Vec4' operands and one 'float'.
*/

static inline void Vec4Set_Add4_Mul4F(lsVec4 &x,
    const lsVec4 &a,const lsVec4 &b,const MeReal c)
{
    x[0] = a[0] + b[0]*c;
    x[1] = a[1] + b[1]*c;
    x[2] = a[2] + b[2]*c;
    x[3] = a[3] + b[3]*c;
}

static inline void Vec4Set_Sub4_Mul4F(lsVec4 &x,
    const lsVec4 &a,const lsVec4 &b,const MeReal c)
{
    x[0] = a[0] - b[0]*c;
    x[1] = a[1] - b[1]*c;
    x[2] = a[2] - b[2]*c;
    x[3] = a[3] - b[3]*c;
}

/*
    'Vec4' operations with three 'Vec4' operands.
*/

static inline void Vec4Set_Add4_Mul4(lsVec4 &x,
    const lsVec4 &a,const lsVec4 &b,const lsVec4 &c)
{
    x[0] = a[0] + b[0]*c[0];
    x[1] = a[1] + b[1]*c[1];
    x[2] = a[2] + b[2]*c[2];
    x[3] = a[3] + b[3]*c[3];
}

static inline void Vec4Set_Sub4_Mul4(lsVec4 &x,
    const lsVec4 &a,const lsVec4 &b,const lsVec4 &c)
{
    x[0] = a[0] - b[0]*c[0];
    x[1] = a[1] - b[1]*c[1];
    x[2] = a[2] - b[2]*c[2];
    x[3] = a[3] - b[3]*c[3];
}

/*
    Operations on arrays of 'Vec4's and arrays of 'Vec3's.
*/

static inline void Vec4x3Set_Vec3x4(lsVec4 out[3], const lsVec3 in[4])
{
    int i,j;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 3; j++)
            out[j][i] = in[i][j];
}

#endif
