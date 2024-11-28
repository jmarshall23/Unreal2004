/* -*-c++-*-
 *===============================================================
 * File:        BVTransform.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 * Note: this code was derived from MAGIC Software.
 *
 * $Revision: 1.6.18.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#ifndef BVTRANSFORM_H
#define BVTRANSFORM_H

#include "BVVec3.h"
#include "lsTransform.h"

//----------------------------------------------------------------
//      class BVTransform interface
//----------------------------------------------------------------

class     BVTransform {
// A representation for a coordinate system in 3D space.
// The interface is convention-independent,
// i.e. there is no explicit mention of:
//  i) row- vs. column-vectors
//  ii) left- vs. right-multiply
//  iii) use transpose of matrix vs. matrix itself
  public:
    BVTransform() {} BVTransform(const BVTransform &);

    BVTransform(const lsTransform * lstm) {
    row[0][0] = lstm->row[0].v[0];
    row[0][1] = lstm->row[0].v[1];
    row[0][2] = lstm->row[0].v[2];

    row[1][0] = lstm->row[1].v[0];
    row[1][1] = lstm->row[1].v[1];
    row[1][2] = lstm->row[1].v[2];

    row[2][0] = lstm->row[2].v[0];
    row[2][1] = lstm->row[2].v[1];
    row[2][2] = lstm->row[2].v[2];

    row[3][0] = lstm->row[3].v[0];
    row[3][1] = lstm->row[3].v[1];
    row[3][2] = lstm->row[3].v[2];
    }

    void      setRotation(const BVVec3 &, const BVVec3 &, const BVVec3 &);
    void      getRotation(BVVec3 *, BVVec3 *, BVVec3 *) const;
    void      setTranslation(const BVVec3 &);
    void      setTranslation(const BVReal x, const BVReal y, const BVReal z);
    void      getTranslation(BVVec3 *) const;

    // void setScale(BVReal scale);
    // void getScale(BVReal *scale);
    // transform a vector from local to world coordinates
    // ( multiply using matrix itself )
    void      transform(const lsVec3 &, lsVec3 *) const;

    // transform a vector from world coordinates to local coordinates
    // ( multiply using matrix inverse )
    void      inverseTransform(const lsVec3 &, lsVec3 *) const;

    // verbose synonyms
    void      thisFrameToRefFrame(const lsVec3 &, lsVec3 *) const;
    void      refFrameToThisFrame(const lsVec3 &, lsVec3 *) const;
    void      transformWithoutTranslate(const lsVec3 &, lsVec3 *) const;
    void      inverseTransformWithoutTranslate(const lsVec3 &, lsVec3 *) const;

    // compound transformations
    // *this will operate on vectors by
    // "transform by first, then transform by second"
    // this->transform( v,&vOut ) <->
    // first.transform( v,&vIntermediate ) ;
    // second.transform ( vIntermediate,&vOut)
    // i.e. first is applied first; it is the "deeper" coordinate system.
    void thisIsFirstThenSecond(const BVTransform &, const BVTransform &);

    // *this will bring a vector in first into second's c.s.
    //
    // this->transform ( vInFirst, &vInSecond) ; <->
    // first.transform ( vInFirst,&vIntermediate );
    // second.inverseTransform( vIntermediate, &vInSecond ) ;
    void thisIsFirstThenInverseSecond(const BVTransform &, const BVTransform &);
    void thisIsInverseFirstThenSecond(const BVTransform &, const BVTransform &);

    // verbose synonym
    void thisIsFirstToSecond(const BVTransform &, const BVTransform &);
    // in-place compound transformations
    //void thisIsThisThenThat( const BVTransform& );
    void      thisIsThisThenInverseThat(const BVTransform &);
    void      thisIsInverseThat(const BVTransform &);
    void      setIdentity();

    // return 1 if orthonomal, return 0 otherwise .
    unsigned int isOrthonormal(void) const;

  public:
    // data:
    // specification of *this coordinate system, w.r.t. reference frame
    // row i of equivalent 3x3 would be:
    // graphics: row[i][0],row[i][1],row[i][2]
    // scientific: row[0][i],row[1][i],row[2][i]
    // represents: "rotate then translate"
    //   1.f the axes & origin
    // BVTransformRow row[4];
    BVVec3 row[4];

    // BVVec3 &axis(const int i) { return row[i].v; }
    BVVec3 & axis(const int i) {
    return row[i];
    }

    const     BVVec3 & axis(const int i) const {
    return row[i];
    }
    BVVec3 & t() {
    return row[3];
    }
    const     BVVec3 & t() const {
    return row[3];
    }
    BVReal  elem(size_t i, size_t j) const {
    return row[i][j];
    }
};

// inline definitions

inline void BVTransform::getRotation(BVVec3 * v0, BVVec3 * v1, BVVec3 * v2) const
{
    *v0 = row[0];
    *v1 = row[1];
    *v2 = row[2];

}

inline void BVTransform::setTranslation(const BVVec3 & v)
{
    row[3] = v;
}

inline void BVTransform::setTranslation(const BVReal x, const BVReal y, const BVReal z)
{
    row[3].v[0] = x;
    row[3].v[1] = y;
    row[3].v[2] = z;
}

inline void BVTransform::getTranslation(BVVec3 * v) const
{
    *v = row[3];
}

inline void BVTransform::transform(const lsVec3 &vThisFrame, lsVec3 *vRefFrame) const
  // in both conventions, this is
  // equivalent to multiplying by the matrix itself
{
#ifdef PS2_BAD

    unsigned int a = (unsigned int) (&(vThisFrame.v[0]));
    unsigned int b = (unsigned int) (&(row[0].v[0]));
    unsigned int c = (unsigned int) (&(vRefFrame->v[0]));

    // Usually a and c are unaligned

    if (ALIGNED(a) && ALIGNED(c) && ALIGNED(b)) {

    __asm__
        __volatile__("
#  r
#  a
#  t0
#  t1
#  t2
#  t
             lqc2 vf2, 0x0(%0)
#  'lqc2  @a,0x0(%0)
             lqc2 vf3, 0x0(%1)
#  'lqc2  @t0,0x0(%1)
             lqc2 vf4, 0x10(%1)
#  'lqc2  @t1,0x10(%1)
             lqc2 vf5, 0x20(%1)
#  'lqc2  @t2,0x20(%1)
             lqc2 vf6, 0x0(%3)
#  'lqc2  @t,0x0(%3)
             vsuba.xyzw ACC, vf0, vf0
#  ACC = K - K
             vmaddax.xyzw ACC, vf3, vf2
#  ACC = ACC + t0 * a.x
             vmadday.xyzw ACC, vf4, vf2
#  ACC = ACC + t1 * a.y
             vmaddaz.xyzw ACC, vf5, vf2
#  ACC = ACC + t2 * a.z
             vmaddx.xyz vf1, vf0, vf0
#  r.xyz = ACC + K * K.x
             vadd.xyz vf1, vf1, vf6
#  r.xyz = r + t
             sqc2 vf1, 0x0(%2)
#  'sqc2  @r,0x0(%2)
#  ~r
#  ~a
#  ~t0
#  ~t1
#  ~t2
#  ~t
             " : : " r " (&(vThisFrame.v[0])),
             "r"(&(row[0].v[0])), "r"(&(vRefFrame->v[0])), "r"(&(row[3].v[0])));

    } else {

    if (ALIGNED(b)) {

        __asm__
        __volatile__("
#  r
#  a
#  t0
#  t1
#  t2
#  t
#  buf0
#  buf1
#  zero
             vsub.xyzw vf9, vf0, vf0
#  zero = K - K
#  # 'lqc2  @a,0x0(%0)
             lw $8, 0x0(%0)
#  'lw  $8, 0x0(%0)
             lw $9, 0x4(%0)
#  'lw  $9, 0x4(%0)
             lw $10, 0x8(%0)
#  'lw  $10,0x8(%0)
             qmtc2 $8, vf2
#  'qmtc2  $8,@a
             qmtc2 $9, vf7
#  'qmtc2  $9,@buf0
             qmtc2 $10, vf8
#  'qmtc2  $10,@buf1
             vaddx.y vf2, vf9, vf7
#  a.y = zero + buf0.x
             vaddx.z vf2, vf9, vf8
#  a.z = zero + buf1.x
             lqc2 vf3, 0x0(%1)
#  'lqc2  @t0,0x0(%1)
             lqc2 vf4, 0x10(%1)
#  'lqc2  @t1,0x10(%1)
             lqc2 vf5, 0x20(%1)
#  'lqc2  @t2,0x20(%1)
             lqc2 vf6, 0x0(%3)
#  'lqc2  @t,0x0(%3)
             vsuba.xyzw ACC, vf0, vf0
#  ACC = K - K
             vmaddax.xyzw ACC, vf3, vf2
#  ACC = ACC + t0 * a.x
             vmadday.xyzw ACC, vf4, vf2
#  ACC = ACC + t1 * a.y
             vmaddaz.xyzw ACC, vf5, vf2
#  ACC = ACC + t2 * a.z
             vmaddx.xyz vf1, vf0, vf0
#  r.xyz = ACC + K * K.x
             vadd.xyz vf1, vf1, vf6
#  r.xyz = r + t
#  # 'sqc2  @r,0x0(%2)
             vaddy.x vf7, vf9, vf1
#  buf0.x = zero + r.y
             vaddz.x vf8, vf9, vf1
#  buf1.x = zero + r.z
             qmfc2 $8, vf1
#  'qmfc2  $8,@r
             qmfc2 $9, vf7
#  'qmfc2  $9,@buf0
             qmfc2 $10, vf8
#  'qmfc2  $10,@buf1
             sw $8, 0x0(%2)
#  'sw     $8, 0x0(%2)
             sw $9, 0x4(%2)
#  'sw     $9, 0x4(%2)
             sw $10, 0x8(%2)
#  'sw     $10,0x8(%2)
#  ~r
#  ~a
#  ~t0
#  ~t1
#  ~t2
#  ~buf0
#  ~buf1
#  ~zero
             " : : " r " (&(vThisFrame.v[0])),
                 "r"(&(row[0].v[0])), "r"(&(vRefFrame->v[0])), "r"(&(row[3].v[0]))
                 :"$8", "$9", "$10");

    } else {

#endif
        (*vRefFrame).v[0] =
        vThisFrame.v[0] * (MeReal) row[0].v[0] +
        vThisFrame.v[1] * (MeReal) row[1].v[0] +
        vThisFrame.v[2] * (MeReal) row[2].v[0] + (MeReal) row[3].v[0];

        (*vRefFrame).v[1] =
        vThisFrame.v[0] * (MeReal) row[0].v[1] +
        vThisFrame.v[1] * (MeReal) row[1].v[1] +
        vThisFrame.v[2] * (MeReal) row[2].v[1] + (MeReal) row[3].v[1];

        (*vRefFrame).v[2] =
        vThisFrame.v[0] * (MeReal) row[0].v[2] +
        vThisFrame.v[1] * (MeReal) row[1].v[2] +
        vThisFrame.v[2] * (MeReal) row[2].v[2] + (MeReal) row[3].v[2];

#ifdef PS2_BAD
	}
    }
#endif
}

inline void BVTransform::inverseTransform(const lsVec3 &vRefFrame, lsVec3 *vThisFrame) const
  // in both conventions, this is
  // equivalent to multiplying by the transpose of the matrix
{
    lsVec3 diff;
    diff[0] = vRefFrame[0] - (MeReal) row[3][0];
    diff[1] = vRefFrame[1] - (MeReal) row[3][1];
    diff[2] = vRefFrame[2] - (MeReal) row[3][2];

    (*vThisFrame).v[0] = axis(0).dot(diff);
    (*vThisFrame).v[1] = axis(1).dot(diff);
    (*vThisFrame).v[2] = axis(2).dot(diff);
}

inline void BVTransform::transformWithoutTranslate(const lsVec3 &vThisFrame, lsVec3 *vRefFrame) const
  // in both conventions, this is
  // equivalent to multiplying by the matrix itself
{
#ifdef PS2_BAD

    unsigned int a = (unsigned int) &(vThisFrame.v[0]);
    unsigned int b = (unsigned int) &(row[0].v[0]);
    unsigned int c = (unsigned int) &(vRefFrame->v[0]);

    if (ALIGNED(a) && ALIGNED(b) && ALIGNED(c)) {

    __asm__
    __volatile__("
#  r
#  a
#  t0
#  t1
#  t2
             lqc2 vf2, 0x0(%0)
#  'lqc2  @a,0x0(%0)
             lqc2 vf3, 0x0(%1)
#  'lqc2  @t0,0x0(%1)
             lqc2 vf4, 0x10(%1)
#  'lqc2  @t1,0x10(%1)
             lqc2 vf5, 0x20(%1)
#  'lqc2  @t2,0x20(%1)
             vsuba.xyzw ACC, vf0, vf0
#  ACC = K - K
             vmaddax.xyzw ACC, vf3, vf2
#  ACC = ACC + t0 * a.x
             vmadday.xyzw ACC, vf4, vf2
#  ACC = ACC + t1 * a.y
             vmaddaz.xyzw ACC, vf5, vf2
#  ACC = ACC + t2 * a.z
             vmaddx.xyz vf1, vf0, vf0
#  r.xyz = ACC + K * K.x
             sqc2 vf1, 0x0(%2)
#  'sqc2  @r,0x0(%2)
#  ~r
#  ~a
#  ~t0
#  ~t1
#  ~t2
             " : : " r " (&(vThisFrame.v[0])),
             "r"(&(row[0].v[0])), "r"(&(vRefFrame->v[0])));
    } else {

#endif

    (*vRefFrame).v[0] =
        vThisFrame.v[0] * (MeReal) row[0].v[0] +
        vThisFrame.v[1] * (MeReal) row[1].v[0] + vThisFrame.v[2] * (MeReal) row[2].v[0];

    (*vRefFrame).v[1] =
        vThisFrame.v[0] * (MeReal) row[0].v[1] +
        vThisFrame.v[1] * (MeReal) row[1].v[1] + vThisFrame.v[2] * (MeReal) row[2].v[1];

    (*vRefFrame).v[2] =
        vThisFrame.v[0] * (MeReal) row[0].v[2] +
        vThisFrame.v[1] * (MeReal) row[1].v[2] + vThisFrame.v[2] * (MeReal) row[2].v[2];

#ifdef PS2_BAD
    }
#endif
}

inline void BVTransform::inverseTransformWithoutTranslate(const lsVec3 &vRefFrame, lsVec3 *vThisFrame) const
  // in both conventions, this is
  // equivalent to multiplying by the transpose of the matrix
{
/* ZL30082000
  (*vThisFrame).v[0] = vRefFrame.dot( axis(0) ) ;
  (*vThisFrame).v[1] = vRefFrame.dot( axis(1) ) ;
  (*vThisFrame).v[2] = vRefFrame.dot( axis(2) ) ;
*/

    (*vThisFrame).v[0] = axis(0).dot(vRefFrame);
    (*vThisFrame).v[1] = axis(1).dot(vRefFrame);
    (*vThisFrame).v[2] = axis(2).dot(vRefFrame);
}

inline void BVTransform::thisFrameToRefFrame(const lsVec3 &vThisFrame, lsVec3 *vRefFrame) const
{
    transform(vThisFrame, vRefFrame);
}
inline void BVTransform::refFrameToThisFrame(const lsVec3 &vRefFrame, lsVec3 *vThisFrame) const
{
    inverseTransform(vRefFrame, vThisFrame);
}

inline void BVTransform::setIdentity()
{
    row[0].v[0] = row[1].v[1] = row[2].v[2] = (BVReal) (1.);

    row[0].v[1] = row[0].v[2] = row[1].v[0]
    = row[1].v[2] = row[2].v[0] = row[2].v[1]
    = row[3].v[0] = row[3].v[1] = row[3].v[2]
    = (BVReal) (0.);
}

inline BVTransform::BVTransform(const BVTransform & m)
{
    row[0] = m.row[0];
    row[1] = m.row[1];
    row[2] = m.row[2];
    row[3] = m.row[3];
}

inline void BVTransform::thisIsFirstThenSecond(const BVTransform & m1, const BVTransform & m2)
{
    row[0].v[0] =
    m1.row[0].v[0] * m2.row[0].v[0] +
    m1.row[0].v[1] * m2.row[1].v[0] + m1.row[0].v[2] * m2.row[2].v[0];
    row[0].v[1] =
    m1.row[0].v[0] * m2.row[0].v[1] +
    m1.row[0].v[1] * m2.row[1].v[1] + m1.row[0].v[2] * m2.row[2].v[1];
    row[0].v[2] =
    m1.row[0].v[0] * m2.row[0].v[2] +
    m1.row[0].v[1] * m2.row[1].v[2] + m1.row[0].v[2] * m2.row[2].v[2];

    row[1].v[0] =
    m1.row[1].v[0] * m2.row[0].v[0] +
    m1.row[1].v[1] * m2.row[1].v[0] + m1.row[1].v[2] * m2.row[2].v[0];
    row[1].v[1] =
    m1.row[1].v[0] * m2.row[0].v[1] +
    m1.row[1].v[1] * m2.row[1].v[1] + m1.row[1].v[2] * m2.row[2].v[1];
    row[1].v[2] =
    m1.row[1].v[0] * m2.row[0].v[2] +
    m1.row[1].v[1] * m2.row[1].v[2] + m1.row[1].v[2] * m2.row[2].v[2];

    row[2].v[0] =
    m1.row[2].v[0] * m2.row[0].v[0] +
    m1.row[2].v[1] * m2.row[1].v[0] + m1.row[2].v[2] * m2.row[2].v[0];
    row[2].v[1] =
    m1.row[2].v[0] * m2.row[0].v[1] +
    m1.row[2].v[1] * m2.row[1].v[1] + m1.row[2].v[2] * m2.row[2].v[1];
    row[2].v[2] =
    m1.row[2].v[0] * m2.row[0].v[2] +
    m1.row[2].v[1] * m2.row[1].v[2] + m1.row[2].v[2] * m2.row[2].v[2];

    row[3].v[0] =
    m1.row[3].v[0] * m2.row[0].v[0] +
    m1.row[3].v[1] * m2.row[1].v[0] + m1.row[3].v[2] * m2.row[2].v[0] + m2.row[3].v[0];
    row[3].v[1] =
    m1.row[3].v[0] * m2.row[0].v[1] +
    m1.row[3].v[1] * m2.row[1].v[1] + m1.row[3].v[2] * m2.row[2].v[1] + m2.row[3].v[1];
    row[3].v[2] =
    m1.row[3].v[0] * m2.row[0].v[2] +
    m1.row[3].v[1] * m2.row[1].v[2] + m1.row[3].v[2] * m2.row[2].v[2] + m2.row[3].v[2];

}

inline void BVTransform::thisIsFirstThenInverseSecond(const BVTransform & m1, const BVTransform & m2)
{

#ifdef PS2_BAD
    unsigned int a = (unsigned int) &(m1.row[0].v[0]);
    unsigned int b = (unsigned int) &(m2.row[0].v[0]);
    unsigned int c = (unsigned int) &(row[0].v[0]);

    if (ALIGNED(a) && ALIGNED(b) && ALIGNED(c)) {

    __asm__
    __volatile__("
#  a0
#  a1
#  a2
#  b0
#  b1
#  b2
#  # transposed
#  bt0
#  bt1
#  bt2
#  r0
#  r1
#  r2
#  at
#  bt
#  diff
#  rt
#  zero
             lqc2 vf1, 0x0(%0)
#  'lqc2  @a0,0x0(%0)
             lqc2 vf2, 0x10(%0)
#  'lqc2  @a1,0x10(%0)
             lqc2 vf3, 0x20(%0)
#  'lqc2  @a2,0x20(%0)
             lqc2 vf4, 0x0(%1)
#  'lqc2  @b0,0x0(%1)
             lqc2 vf5, 0x10(%1)
#  'lqc2  @b1,0x10(%1)
             lqc2 vf7, 0x20(%1)
#  'lqc2  @b2,0x20(%1)
             lqc2 vf14, 0x0(%2)
#  'lqc2  @at,0x0(%2)
             lqc2 vf15, 0x0(%3)
#  'lqc2  @bt,0x0(%3)
             vsub.xyzw vf18, vf0, vf0
#  zero = K - K
#  # Make b transposed
             vaddx.x vf8, vf18, vf4
#  bt0.x = zero + b0.x
             vaddx.y vf8, vf18, vf5
#  bt0.y = zero + b1.x
             vaddx.z vf8, vf18, vf7
#  bt0.z = zero + b2.x
             vaddy.x vf9, vf18, vf4
#  bt1.x = zero + b0.y
             vaddy.y vf9, vf18, vf5
#  bt1.y = zero + b1.y
             vaddy.z vf9, vf18, vf7
#  bt1.z = zero + b2.y
             vaddz.x vf10, vf18, vf4
#  bt2.x = zero + b0.z
             vaddz.y vf10, vf18, vf5
#  bt2.y = zero + b1.z
             vaddz.z vf10, vf18, vf7
#  bt2.z = zero + b2.z
#  # Rotation Matrix Multiply part.
             vsuba.xyzw ACC, vf0, vf0
#  ACC = K - K
             vmaddax.xyzw ACC, vf8, vf1
#  ACC = ACC + bt0 * a0.x
             vmadday.xyzw ACC, vf9, vf1
#  ACC = ACC + bt1 * a0.y
             vmaddaz.xyzw ACC, vf10, vf1
#  ACC = ACC + bt2 * a0.z
             vmaddx.xyzw vf11, vf0, vf0
#  r0  = ACC + K * K.x
             vsuba.xyzw ACC, vf0, vf0
#  ACC = K - K
             vmaddax.xyzw ACC, vf8, vf2
#  ACC = ACC + bt0 * a1.x
             vmadday.xyzw ACC, vf9, vf2
#  ACC = ACC + bt1 * a1.y
             vmaddaz.xyzw ACC, vf10, vf2
#  ACC = ACC + bt2 * a1.z
             vmaddx.xyzw vf12, vf0, vf0
#  r1  = ACC + K * K.x
             vsuba.xyzw ACC, vf0, vf0
#  ACC = K - K
             vmaddax.xyzw ACC, vf8, vf3
#  ACC = ACC + bt0 * a2.x
             vmadday.xyzw ACC, vf9, vf3
#  ACC = ACC + bt1 * a2.y
             vmaddaz.xyzw ACC, vf10, vf3
#  ACC = ACC + bt2 * a2.z
             vmaddx.xyzw vf13, vf0, vf0
#  r2  = ACC + K * K.x
#  # Translation Vector part.
             vsub.xyzw vf16, vf14, vf15
#  diff = at - bt
             vsuba.xyzw ACC, vf0, vf0
#  ACC = K - K
             vmaddax.xyzw ACC, vf8, vf16
#  ACC = ACC + bt0 * diff.x
             vmadday.xyzw ACC, vf9, vf16
#  ACC = ACC + bt1 * diff.y
             vmaddaz.xyzw ACC, vf10, vf16
#  ACC = ACC + bt2 * diff.z
             vmaddx.xyzw vf17, vf0, vf0
#  rt  = ACC + K * K.x
             sqc2 vf11, 0x0(%4)
#  'sqc2  @r0,0x0(%4)
             sqc2 vf12, 0x10(%4)
#  'sqc2  @r1,0x10(%4)
             sqc2 vf13, 0x20(%4)
#  'sqc2  @r2,0x20(%4)
             sqc2 vf17, 0x0(%5)
#  'sqc2  @rt,0x0(%5)
#  ~a0
#  ~a1
#  ~a2
#  ~b0
#  ~b1
#  ~b2
#  ~bt0
#  ~bt1
#  ~bt2
#  ~r0
#  ~r1
#  ~r2
#  ~at
#  ~bt
#  ~diff
#  ~rt
#  ~zero
             " : : " r " (&(m1.row[0].v[0])),
             "r"(&(m2.row[0].v[0])),
             "r"(&(m1.row[3].v[0])),
             "r"(&(m2.row[3].v[0])), "r"(&(row[0].v[0])), "r"(&(row[3].v[0])));

    } else {

    // Unaligned operands

    __asm__
    __volatile__("
#  a0
#  a1
#  a2
#  # transposed
#  bt0
#  bt1
#  bt2
#  r0
#  r1
#  r2
#  at
#  bt
#  diff
#  rt
#  buf0
#  buf1
#  buf2
#  zero
             vsub.xyzw vf18, vf0, vf0
#  zero = K - K
#  #a
             lw $8, 0x0(%0)
#  'lw  $8, 0x0(%0)
             lw $9, 0x4(%0)
#  'lw  $9, 0x4(%0)
             lw $10, 0x8(%0)
#  'lw  $10,0x8(%0)
             qmtc2 $8, vf1
#  'qmtc2 $8,@a0
             qmtc2 $9, vf15
#  'qmtc2 $9,@buf0
             qmtc2 $10, vf16
#  'qmtc2 $10,@buf1
             vaddx.y vf1, vf18, vf15
#  a0.y = zero + buf0.x
             vaddx.z vf1, vf18, vf16
#  a0.z = zero + buf1.x
             lw $8, 0x10(%0)
#  'lw  $8, 0x10(%0)
             lw $9, 0x14(%0)
#  'lw  $9, 0x14(%0)
             lw $10, 0x18(%0)
#  'lw  $10,0x18(%0)
             qmtc2 $8, vf2
#  'qmtc2 $8,@a1
             qmtc2 $9, vf15
#  'qmtc2 $9,@buf0
             qmtc2 $10, vf16
#  'qmtc2 $10,@buf1
             vaddx.y vf2, vf18, vf15
#  a1.y = zero + buf0.x
             vaddx.z vf2, vf18, vf16
#  a1.z = zero + buf1.x
             lw $8, 0x20(%0)
#  'lw  $8, 0x20(%0)
             lw $9, 0x24(%0)
#  'lw  $9, 0x24(%0)
             lw $10, 0x28(%0)
#  'lw  $10,0x28(%0)
             qmtc2 $8, vf3
#  'qmtc2 $8,@a2
             qmtc2 $9, vf15
#  'qmtc2 $9,@buf0
             qmtc2 $10, vf16
#  'qmtc2 $10,@buf1
             vaddx.y vf3, vf18, vf15
#  a2.y = zero + buf0.x
             vaddx.z vf3, vf18, vf16
#  a2.z = zero + buf1.x
#  # b transposed
             lw $8, 0x0(%1)
#  'lw  $8, 0x0(%1)
             lw $9, 0x4(%1)
#  'lw  $9, 0x4(%1)
             lw $10, 0x8(%1)
#  'lw  $10,0x8(%1)
             qmtc2 $8, vf4
#  'qmtc2 $8,@bt0
             qmtc2 $9, vf15
#  'qmtc2 $9,@buf0
             qmtc2 $10, vf16
#  'qmtc2 $10,@buf1
             vaddx.x vf5, vf18, vf15
#  bt1.x = zero + buf0.x
             vaddx.x vf7, vf18, vf16
#  bt2.x = zero + buf1.x
             lw $8, 0x10(%1)
#  'lw  $8, 0x10(%1)
             lw $9, 0x14(%1)
#  'lw  $9, 0x14(%1)
             lw $10, 0x18(%1)
#  'lw  $10,0x18(%1)
             qmtc2 $8, vf15
#  'qmtc2 $8,@buf0
             qmtc2 $9, vf16
#  'qmtc2 $9,@buf1
             qmtc2 $10, vf17
#  'qmtc2 $10,@buf2
             vaddx.y vf4, vf18, vf15
#  bt0.y = zero + buf0.x
             vaddx.y vf5, vf18, vf16
#  bt1.y = zero + buf1.x
             vaddx.y vf7, vf18, vf17
#  bt2.y = zero + buf2.x
             lw $8, 0x20(%1)
#  'lw  $8, 0x20(%1)
             lw $9, 0x24(%1)
#  'lw  $9, 0x24(%1)
             lw $10, 0x28(%1)
#  'lw  $10,0x28(%1)
             qmtc2 $8, vf15
#  'qmtc2 $8,@buf0
             qmtc2 $9, vf16
#  'qmtc2 $9,@buf1
             qmtc2 $10, vf17
#  'qmtc2 $10,@buf2
             vaddx.z vf4, vf18, vf15
#  bt0.z = zero + buf0.x
             vaddx.z vf5, vf18, vf16
#  bt1.z = zero + buf1.x
             vaddx.z vf7, vf18, vf17
#  bt2.z = zero + buf2.x
#  # at
             lw $8, 0x0(%2)
#  'lw  $8, 0x0(%2)
             lw $9, 0x4(%2)
#  'lw  $9, 0x4(%2)
             lw $10, 0x8(%2)
#  'lw  $10,0x8(%2)
             qmtc2 $8, vf11
#  'qmtc2 $8,@at
             qmtc2 $9, vf15
#  'qmtc2 $9,@buf0
             qmtc2 $10, vf16
#  'qmtc2 $10,@buf1
             vaddx.y vf11, vf18, vf15
#  at.y = zero + buf0.x
             vaddx.z vf11, vf18, vf16
#  at.z = zero + buf1.x
#  # bt
             lw $8, 0x0(%3)
#  'lw  $8, 0x0(%3)
             lw $9, 0x4(%3)
#  'lw  $9, 0x4(%3)
             lw $10, 0x8(%3)
#  'lw  $10,0x8(%3)
             qmtc2 $8, vf12
#  'qmtc2 $8,@bt
             qmtc2 $9, vf15
#  'qmtc2 $9,@buf0
             qmtc2 $10, vf16
#  'qmtc2 $10,@buf1
             vaddx.y vf12, vf18, vf15
#  bt.y = zero + buf0.x
             vaddx.z vf12, vf18, vf16
#  bt.z = zero + buf1.x
#  # Rotation Matrix Multiply part.
             vsuba.xyzw ACC, vf0, vf0
#  ACC = K - K
             vmaddax.xyzw ACC, vf4, vf1
#  ACC = ACC + bt0 * a0.x
             vmadday.xyzw ACC, vf5, vf1
#  ACC = ACC + bt1 * a0.y
             vmaddaz.xyzw ACC, vf7, vf1
#  ACC = ACC + bt2 * a0.z
             vmaddx.xyzw vf8, vf0, vf0
#  r0  = ACC + K * K.x
             vsuba.xyzw ACC, vf0, vf0
#  ACC = K - K
             vmaddax.xyzw ACC, vf4, vf2
#  ACC = ACC + bt0 * a1.x
             vmadday.xyzw ACC, vf5, vf2
#  ACC = ACC + bt1 * a1.y
             vmaddaz.xyzw ACC, vf7, vf2
#  ACC = ACC + bt2 * a1.z
             vmaddx.xyzw vf9, vf0, vf0
#  r1  = ACC + K * K.x
             vsuba.xyzw ACC, vf0, vf0
#  ACC = K - K
             vmaddax.xyzw ACC, vf4, vf3
#  ACC = ACC + bt0 * a2.x
             vmadday.xyzw ACC, vf5, vf3
#  ACC = ACC + bt1 * a2.y
             vmaddaz.xyzw ACC, vf7, vf3
#  ACC = ACC + bt2 * a2.z
             vmaddx.xyzw vf10, vf0, vf0
#  r2  = ACC + K * K.x
#  # Translation Vector part.
             vsub.xyzw vf13, vf11, vf12
#  diff = at - bt
             vsuba.xyzw ACC, vf0, vf0
#  ACC = K - K
             vmaddax.xyzw ACC, vf4, vf13
#  ACC = ACC + bt0 * diff.x
             vmadday.xyzw ACC, vf5, vf13
#  ACC = ACC + bt1 * diff.y
             vmaddaz.xyzw ACC, vf7, vf13
#  ACC = ACC + bt2 * diff.z
             vmaddx.xyzw vf14, vf0, vf0
#  rt  = ACC + K * K.x
#  # 'sqc2  @r0,0x0(%4)
             vaddy.x vf15, vf18, vf8
#  buf0.x = zero + r0.y
             vaddz.x vf16, vf18, vf8
#  buf1.x = zero + r0.z
             qmfc2 $8, vf8
#  'qmfc2  $8,@r0
             qmfc2 $9, vf15
#  'qmfc2  $9,@buf0
             qmfc2 $10, vf16
#  'qmfc2  $10,@buf1
             sw $8, 0x0(%4)
#  'sw     $8,0x0(%4)
             sw $9, 0x4(%4)
#  'sw     $9,0x4(%4)
             sw $10, 0x8(%4)
#  'sw     $10,0x8(%4)
#  # 'sqc2  @r1,0x10(%4)
             vaddy.x vf15, vf18, vf9
#  buf0.x = zero + r1.y
             vaddz.x vf16, vf18, vf9
#  buf1.x = zero + r1.z
             qmfc2 $8, vf9
#  'qmfc2  $8,@r1
             qmfc2 $9, vf15
#  'qmfc2  $9,@buf0
             qmfc2 $10, vf16
#  'qmfc2  $10,@buf1
             sw $8, 0x10(%4)
#  'sw     $8,0x10(%4)
             sw $9, 0x14(%4)
#  'sw     $9,0x14(%4)
             sw $10, 0x18(%4)
#  'sw     $10,0x18(%4)
#  # 'sqc2  @r2,0x20(%4)
             vaddy.x vf15, vf18, vf10
#  buf0.x = zero + r2.y
             vaddz.x vf16, vf18, vf10
#  buf1.x = zero + r2.z
             qmfc2 $8, vf10
#  'qmfc2  $8,@r2
             qmfc2 $9, vf15
#  'qmfc2  $9,@buf0
             qmfc2 $10, vf16
#  'qmfc2  $10,@buf1
             sw $8, 0x20(%4)
#  'sw     $8,0x20(%4)
             sw $9, 0x24(%4)
#  'sw     $9,0x24(%4)
             sw $10, 0x28(%4)
#  'sw     $10,0x28(%4)
#  # 'sqc2  @rt,0x0(%5)
             vaddy.x vf15, vf18, vf14
#  buf0.x = zero + rt.y
             vaddz.x vf16, vf18, vf14
#  buf1.x = zero + rt.z
             qmfc2 $8, vf14
#  'qmfc2  $8,@rt
             qmfc2 $9, vf15
#  'qmfc2  $9,@buf0
             qmfc2 $10, vf16
#  'qmfc2  $10,@buf1
             sw $8, 0x0(%5)
#  'sw     $8,0x0(%5)
             sw $9, 0x4(%5)
#  'sw     $9,0x4(%5)
             sw $10, 0x8(%5)
#  'sw     $10,0x8(%5)
#  ~a0
#  ~a1
#  ~a2
#  ~bt0
#  ~bt1
#  ~bt2
#  ~r0
#  ~r1
#  ~r2
#  ~at
#  ~bt
#  ~diff
#  ~rt
#  ~zero
#  ~buf0
#  ~buf1
#  ~buf2
             " : : " r " (&(m1.row[0].v[0])),
             "r"(&(m2.row[0].v[0])),
             "r"(&(m1.row[3].v[0])),
             "r"(&(m2.row[3].v[0])), "r"(&(row[0].v[0])), "r"(&(row[3].v[0]))
             :"$8", "$9", "$10");
    }
#else // end PS2
    row[0].v[0] =
    m1.row[0].v[0] * m2.row[0].v[0] +
    m1.row[0].v[1] * m2.row[0].v[1] + m1.row[0].v[2] * m2.row[0].v[2];
    row[0].v[1] =
    m1.row[0].v[0] * m2.row[1].v[0] +
    m1.row[0].v[1] * m2.row[1].v[1] + m1.row[0].v[2] * m2.row[1].v[2];
    row[0].v[2] =
    m1.row[0].v[0] * m2.row[2].v[0] +
    m1.row[0].v[1] * m2.row[2].v[1] + m1.row[0].v[2] * m2.row[2].v[2];

    row[1].v[0] =
    m1.row[1].v[0] * m2.row[0].v[0] +
    m1.row[1].v[1] * m2.row[0].v[1] + m1.row[1].v[2] * m2.row[0].v[2];
    row[1].v[1] =
    m1.row[1].v[0] * m2.row[1].v[0] +
    m1.row[1].v[1] * m2.row[1].v[1] + m1.row[1].v[2] * m2.row[1].v[2];
    row[1].v[2] =
    m1.row[1].v[0] * m2.row[2].v[0] +
    m1.row[1].v[1] * m2.row[2].v[1] + m1.row[1].v[2] * m2.row[2].v[2];

    row[2].v[0] =
    m1.row[2].v[0] * m2.row[0].v[0] +
    m1.row[2].v[1] * m2.row[0].v[1] + m1.row[2].v[2] * m2.row[0].v[2];
    row[2].v[1] =
    m1.row[2].v[0] * m2.row[1].v[0] +
    m1.row[2].v[1] * m2.row[1].v[1] + m1.row[2].v[2] * m2.row[1].v[2];
    row[2].v[2] =
    m1.row[2].v[0] * m2.row[2].v[0] +
    m1.row[2].v[1] * m2.row[2].v[1] + m1.row[2].v[2] * m2.row[2].v[2];

    BVVec3
    diff = m1.t() - m2.t();

    row[3].v[0] =
    diff.v[0] * m2.row[0].v[0] + diff.v[1] * m2.row[0].v[1] + diff.v[2] * m2.row[0].v[2];
    row[3].v[1] =
    diff.v[0] * m2.row[1].v[0] + diff.v[1] * m2.row[1].v[1] + diff.v[2] * m2.row[1].v[2];
    row[3].v[2] =
    diff.v[0] * m2.row[2].v[0] + diff.v[1] * m2.row[2].v[1] + diff.v[2] * m2.row[2].v[2];
#endif  // not PS2

}

inline void BVTransform::thisIsInverseFirstThenSecond(const BVTransform & m1, const BVTransform & m2)
{

    row[0].v[0] =
    m1.row[0].v[0] * m2.row[0].v[0] +
    m1.row[1].v[0] * m2.row[1].v[0] + m1.row[2].v[0] * m2.row[2].v[0];
    row[0].v[1] =
    m1.row[0].v[0] * m2.row[0].v[1] +
    m1.row[1].v[0] * m2.row[1].v[1] + m1.row[2].v[0] * m2.row[2].v[1];
    row[0].v[2] =
    m1.row[0].v[0] * m2.row[0].v[2] +
    m1.row[1].v[0] * m2.row[1].v[2] + m1.row[2].v[0] * m2.row[2].v[2];

    row[1].v[0] =
    m1.row[0].v[1] * m2.row[0].v[0] +
    m1.row[1].v[1] * m2.row[1].v[0] + m1.row[2].v[1] * m2.row[2].v[0];
    row[1].v[1] =
    m1.row[0].v[1] * m2.row[0].v[1] +
    m1.row[1].v[1] * m2.row[1].v[1] + m1.row[2].v[1] * m2.row[2].v[1];
    row[1].v[2] =
    m1.row[0].v[1] * m2.row[0].v[2] +
    m1.row[1].v[1] * m2.row[1].v[2] + m1.row[2].v[1] * m2.row[2].v[2];

    row[2].v[0] =
    m1.row[0].v[2] * m2.row[0].v[0] +
    m1.row[1].v[2] * m2.row[1].v[0] + m1.row[2].v[2] * m2.row[2].v[0];
    row[2].v[1] =
    m1.row[0].v[2] * m2.row[0].v[1] +
    m1.row[1].v[2] * m2.row[1].v[1] + m1.row[2].v[2] * m2.row[2].v[1];
    row[2].v[2] =
    m1.row[0].v[2] * m2.row[0].v[2] +
    m1.row[1].v[2] * m2.row[1].v[2] + m1.row[2].v[2] * m2.row[2].v[2];

    row[3].v[0] =
    m2.row[3].v[0] -
    m1.row[3].v[0] * row[0].v[0] - m1.row[3].v[1] * row[1].v[0] - m1.row[3].v[2] * row[2].v[0];

    row[3].v[1] =
    m2.row[3].v[1] -
    m1.row[3].v[0] * row[0].v[1] - m1.row[3].v[1] * row[1].v[1] - m1.row[3].v[2] * row[2].v[1];

    row[3].v[2] =
    m2.row[3].v[2] -
    m1.row[3].v[0] * row[0].v[2] - m1.row[3].v[1] * row[1].v[2] - m1.row[3].v[2] * row[2].v[2];

}

inline void BVTransform::thisIsFirstToSecond(const BVTransform & m1, const BVTransform & m2)
{
    thisIsFirstThenInverseSecond(m1, m2);
}

inline void BVInverseTransformWithoutTranslate(const BVTransform * tr, const lsVec3 &in, lsVec3 *out)
{
    if (0 == tr) {
    *out = in;
    } else {
    tr->inverseTransformWithoutTranslate(in, out);
    }
}

inline void BVTransformGetTranslation(const BVTransform * tr, BVVec3 * out)
{
    if (0 == tr) {
    out->setValue(0, 0, 0);
    } else {
    tr->getTranslation(out);
    }
}

inline void BVTransform_Global(const BVTransform * tr, const lsVec3 &in, lsVec3 *out)
{
    if (0 == tr) {
    *out = in;
    } else {
    tr->transform(in, out);
    }
}

#endif
