// -*-c++-*-
//===============================================================
// File:        lsTransform.h
//
// Copyright 1998, Lateral Logic, Inc. 
// ALL RIGHTS RESERVED
// 
// RESTRICTED RIGHTS LEGEND APPLICABLE TO U.S. GOVERNMENT ONLY:
// 
// Use, duplication or disclosure by the U.S. Government of this Software
// and any related material is subject to restrictions as set forth in
// subdivision (c)(1)(ii) of the Rights in
// Technical Data and Computer Software clause at DFARS 252.227f-7013,
// and/or in similar 
// or successor clauses in the FAR, DOD or NASA FAR Supplement.
// 
// 
// $Revision: 1.2 $ 
// $Date: 2000/07/20 07:22:14 $ 
// 
//================================================================

#ifndef LSTRANSFORM_H
#define LSTRANSFORM_H

#include "lsVec3.h"
#include "lsVec4.h"
#include "lsConfig.h"

#ifdef PS2
#define ALIGNED(a) ((unsigned int)(a)%0x10==0)
#endif

#ifdef WIN32
	#include <iostream>
	using std::ostream;
#else
	#include <iostream.h>
#endif

class lsTransform;

struct lsTransformRow {
    lsVec3 v;
    lsReal v3;
};

//----------------------------------------------------------------
//	class lsTransform interface 
//----------------------------------------------------------------

class LCE_API_EXPORT lsTransform {
// A representation for a coordinate system in 3D space.
// The interface is convention-independent,
// i.e. there is no explicit mention of:
//  i) row- vs. column-vectors
//  ii) left- vs. right-multiply
//  iii) use transpose of matrix vs. matrix itself
public:

  lsTransform(){}

  lsTransform( const lsTransform& );
  
  void setRotation(const lsVec3&, const lsVec3&, const lsVec3&);
  void getRotation(lsVec3*, lsVec3*, lsVec3*) const;
  void setTranslation(const lsVec3&);
  void setTranslation(const lsReal x, const lsReal y, const lsReal z);
  void getTranslation(lsVec3*) const;

  // void setScale(lsReal scale);
  // void getScale(lsReal *scale);

  // transform a vector from local to world coordinates 
  // ( multiply using matrix itself )

  void transform( const lsVec3&, lsVec3* ) const;

  // transform a vector from world coordinates to local coordinates 
  // ( multiply using matrix inverse )

  void inverseTransform( const lsVec3&, lsVec3* ) const;

  // verbose synonyms

  void thisFrameToRefFrame( const lsVec3&, lsVec3* ) const;

  void refFrameToThisFrame( const lsVec3&, lsVec3* ) const;


  void transformWithoutTranslate( const lsVec3&, lsVec3* ) const;
  void inverseTransformWithoutTranslate( const lsVec3&, lsVec3* ) const;

  // compound transformations

  // *this will operate on vectors by
  // "transform by first, then transform by second"
  // this->transform( v,&vOut ) <-> 
  // first.transform( v,&vIntermediate ) ; 
  // second.transform ( vIntermediate,&vOut)
  // i.e. first is applied first; it is the "deeper" coordinate system.

  void 
  thisIsFirstThenSecond( const lsTransform&, const lsTransform& );

  // *this will bring a vector in first into second's c.s.
  //
  // this->transform ( vInFirst, &vInSecond) ; <-> 
  // first.transform ( vInFirst,&vIntermediate );
  // second.inverseTransform( vIntermediate, &vInSecond ) ; 

  void
  thisIsFirstThenInverseSecond( const lsTransform&, const lsTransform& );

  // verbose synonym
  void 
  thisIsFirstToSecond( const lsTransform&, const lsTransform& );

  // in-place compound transformations

  //void thisIsThisThenThat( const lsTransform& );

  void thisIsThisThenInverseThat( const lsTransform& );

  void thisIsInverseThat( const lsTransform& );

  void setIdentity();

  // return 1 if orthonomal, return 0 otherwise .
  unsigned int isOrthonormal( void ) const;

public:  

  // data: 
  // specification of *this coordinate system, w.r.t. reference frame
  // row i of equivalent 3x3 would be:
  // graphics: row[i][0],row[i][1],row[i][2]
  // scientific: row[0][i],row[1][i],row[2][i]

  // represents: "rotate then translate"

  //   1.f the axes & origin
  lsTransformRow row[4];

  lsVec3 &axis(const int i) { return row[i].v; }
  const lsVec3 &axis(const int i) const { return row[i].v; }

  lsVec3 &t() { return row[3].v; }
  const lsVec3 &t() const { return row[3].v; }
};

// inline definitions

inline
void 
lsTransform::
getRotation(lsVec3* v0, lsVec3* v1, lsVec3* v2) const

{
  *v0 = row[0].v;
  *v1 = row[1].v;
  *v2 = row[2].v;

}
inline
void 
lsTransform::
setTranslation(const lsVec3& v)
{
  row[3].v = v;
}

inline
void
lsTransform::
setTranslation(const lsReal x, const lsReal y, const lsReal z)
{
  row[3].v[0] = x; row[3].v[1] = y; row[3].v[2] = z;
}



inline
void 
lsTransform::
getTranslation(lsVec3* v) const

{
  *v = row[3].v;
}






inline
void 
lsTransform::
transform( const lsVec3& vThisFrame, lsVec3* vRefFrame ) const

  // in both conventions, this is
  // equivalent to multiplying by the matrix itself

{
#ifdef PS2

    unsigned int a=(unsigned int)(&(vThisFrame.v[0]));
    unsigned int b=(unsigned int)(&(row[0].v[0]));
    unsigned int c=(unsigned int)(&(vRefFrame->v[0]));

    // Usually a and c are unaligned

    if(ALIGNED(a) && ALIGNED(c) && ALIGNED(b)){
#error commented out for gcc3.
/*

        __asm__ __volatile__("
        __expression_asm

        r
        a
        t0
        t1
        t2
        t

        'lqc2  @a,0x0(%0)
        'lqc2  @t0,0x0(%1)
        'lqc2  @t1,0x10(%1)
        'lqc2  @t2,0x20(%1)
        'lqc2  @t,0x0(%3)

        ACC = K - K
        ACC = ACC + t0 * a.x
        ACC = ACC + t1 * a.y
        ACC = ACC + t2 * a.z

        r.xyz = ACC + K * K.x
        r.xyz = r + t

        'sqc2  @r,0x0(%2)

        ~r
        ~a
        ~t0
        ~t1
        ~t2
        ~t

        __end_expression_asm
        " : : "r" (&(vThisFrame.v[0])),
              "r" (&(row[0].v[0])),
              "r" (&(vRefFrame->v[0])),
              "r" (&(row[3].v[0])));

*/
    } else{

        if(ALIGNED(b)){

#error commented out for gcc3.
/*
            __asm__ __volatile__("
            __expression_asm

            r
            a
            t0
            t1
            t2
            t
            buf0
            buf1
            zero

            zero = K - K

            # 'lqc2  @a,0x0(%0)
            'lw  $8, 0x0(%0)
            'lw  $9, 0x4(%0)
            'lw  $10,0x8(%0)

            'qmtc2  $8,@a
            'qmtc2  $9,@buf0
            'qmtc2  $10,@buf1

            a.y = zero + buf0.x
            a.z = zero + buf1.x

            'lqc2  @t0,0x0(%1)
            'lqc2  @t1,0x10(%1)
            'lqc2  @t2,0x20(%1)
            'lqc2  @t,0x0(%3)

            ACC = K - K
            ACC = ACC + t0 * a.x
            ACC = ACC + t1 * a.y
            ACC = ACC + t2 * a.z

            r.xyz = ACC + K * K.x
            r.xyz = r + t

            # 'sqc2  @r,0x0(%2)

            buf0.x = zero + r.y
            buf1.x = zero + r.z
            'qmfc2  $8,@r
            'qmfc2  $9,@buf0
            'qmfc2  $10,@buf1
            'sw     $8, 0x0(%2)
            'sw     $9, 0x4(%2)
            'sw     $10,0x8(%2)

            ~r
            ~a
            ~t0
            ~t1
            ~t2
            ~buf0
            ~buf1
            ~zero

            __end_expression_asm
            " : : "r" (&(vThisFrame.v[0])),
                  "r" (&(row[0].v[0])),
                  "r" (&(vRefFrame->v[0])),
                  "r" (&(row[3].v[0]))
                : "$8", "$9", "$10");
*/

        } else{

#else // PS2
          (*vRefFrame).v[0] = 
            vThisFrame.v[0] * row[0].v[0] +
            vThisFrame.v[1] * row[1].v[0] +
            vThisFrame.v[2] * row[2].v[0] + row[3].v[0];

          (*vRefFrame).v[1] = 
            vThisFrame.v[0] * row[0].v[1] +
            vThisFrame.v[1] * row[1].v[1] +
            vThisFrame.v[2] * row[2].v[1] + row[3].v[1] ;

          (*vRefFrame).v[2] = 
            vThisFrame.v[0] * row[0].v[2] +
            vThisFrame.v[1] * row[1].v[2] +
            vThisFrame.v[2] * row[2].v[2] + row[3].v[2] ;
#endif // PS2

#ifdef PS2
        }
    }
#endif
}

inline
void 
lsTransform::
inverseTransform( const lsVec3& vRefFrame, lsVec3* vThisFrame ) const

  // in both conventions, this is
  // equivalent to multiplying by the transpose of the matrix
{
  lsVec3 diff = vRefFrame - row[3].v ;

  (*vThisFrame).v[0] = diff.dot( axis(0) ) ;
  (*vThisFrame).v[1] = diff.dot( axis(1) ) ;
  (*vThisFrame).v[2] = diff.dot( axis(2) ) ;
}

inline
void 
lsTransform::
transformWithoutTranslate( const lsVec3& vThisFrame, lsVec3* vRefFrame ) const

  // in both conventions, this is
  // equivalent to multiplying by the matrix itself

{
#ifdef PS2

    unsigned int a=(unsigned int)&(vThisFrame.v[0]);
    unsigned int b=(unsigned int)&(row[0].v[0]);
    unsigned int c=(unsigned int)&(vRefFrame->v[0]);

    if(ALIGNED(a) && ALIGNED(b) && ALIGNED(c)){

#error commented out for gcc3.
/*
        __asm__ __volatile__("
        __expression_asm

        r
        a
        t0
        t1
        t2

        'lqc2  @a,0x0(%0)
        'lqc2  @t0,0x0(%1)
        'lqc2  @t1,0x10(%1)
        'lqc2  @t2,0x20(%1)

        ACC = K - K
        ACC = ACC + t0 * a.x
        ACC = ACC + t1 * a.y
        ACC = ACC + t2 * a.z

        r.xyz = ACC + K * K.x

        'sqc2  @r,0x0(%2)

        ~r
        ~a
        ~t0
        ~t1
        ~t2

        __end_expression_asm
        " : : "r" (&(vThisFrame.v[0])),
              "r" (&(row[0].v[0])),
              "r" (&(vRefFrame->v[0])));
*/
    } else{

#else

      (*vRefFrame).v[0] = 
        vThisFrame.v[0] * row[0].v[0] +
        vThisFrame.v[1] * row[1].v[0] +
        vThisFrame.v[2] * row[2].v[0] ;

      (*vRefFrame).v[1] = 
        vThisFrame.v[0] * row[0].v[1] +
        vThisFrame.v[1] * row[1].v[1] +
        vThisFrame.v[2] * row[2].v[1] ;

      (*vRefFrame).v[2] = 
        vThisFrame.v[0] * row[0].v[2] +
        vThisFrame.v[1] * row[1].v[2] +
        vThisFrame.v[2] * row[2].v[2] ;

#endif // PS2

#ifdef PS2
    }
#endif
}

inline
void 
lsTransform::
inverseTransformWithoutTranslate( const lsVec3& vRefFrame, 
				  lsVec3* vThisFrame ) const

  // in both conventions, this is
  // equivalent to multiplying by the transpose of the matrix
{
  (*vThisFrame).v[0] = vRefFrame.dot( axis(0) ) ;
  (*vThisFrame).v[1] = vRefFrame.dot( axis(1) ) ;
  (*vThisFrame).v[2] = vRefFrame.dot( axis(2) ) ;
}

inline
void 
lsTransform::
thisFrameToRefFrame( const lsVec3& vThisFrame, lsVec3* vRefFrame ) const
{
  transform( vThisFrame, vRefFrame );
}
inline
void 
lsTransform::
refFrameToThisFrame( const lsVec3& vRefFrame, lsVec3* vThisFrame ) const
{
  inverseTransform( vRefFrame, vThisFrame );
}

inline
void 
lsTransform::
setIdentity()
{
  row[0].v[0] = row[1].v[1] = row[2].v[2] = (MeReal)(1.) ;

  row[0].v[1] = row[0].v[2] = row[1].v[0] 
    = row[1].v[2] = row[2].v[0] = row[2].v[1] 
    = row[3].v[0] = row[3].v[1] = row[3].v[2]
    = (MeReal)(0.) ; 
}
inline
lsTransform::
lsTransform( const lsTransform& m )
{
  row[0] = m.row[0];
  row[1] = m.row[1];
  row[2] = m.row[2];
  row[3] = m.row[3];
}


inline
void 
lsTransform::
thisIsFirstThenSecond( const lsTransform& m1, const lsTransform& m2 )
{
  row[0].v[0] =
    m1.row[0].v[0] * m2.row[0].v[0] +
    m1.row[0].v[1] * m2.row[1].v[0] +
    m1.row[0].v[2] * m2.row[2].v[0] ;
  row[0].v[1] =
    m1.row[0].v[0] * m2.row[0].v[1] +
    m1.row[0].v[1] * m2.row[1].v[1] +
    m1.row[0].v[2] * m2.row[2].v[1] ;
  row[0].v[2] =
    m1.row[0].v[0] * m2.row[0].v[2] +
    m1.row[0].v[1] * m2.row[1].v[2] +
    m1.row[0].v[2] * m2.row[2].v[2] ;

  row[1].v[0] =
    m1.row[1].v[0] * m2.row[0].v[0] +
    m1.row[1].v[1] * m2.row[1].v[0] +
    m1.row[1].v[2] * m2.row[2].v[0] ;
  row[1].v[1] =
    m1.row[1].v[0] * m2.row[0].v[1] +
    m1.row[1].v[1] * m2.row[1].v[1] +
    m1.row[1].v[2] * m2.row[2].v[1] ;
  row[1].v[2] =
    m1.row[1].v[0] * m2.row[0].v[2] +
    m1.row[1].v[1] * m2.row[1].v[2] +
    m1.row[1].v[2] * m2.row[2].v[2] ;

  row[2].v[0] =
    m1.row[2].v[0] * m2.row[0].v[0] +
    m1.row[2].v[1] * m2.row[1].v[0] +
    m1.row[2].v[2] * m2.row[2].v[0] ;
  row[2].v[1] =
    m1.row[2].v[0] * m2.row[0].v[1] +
    m1.row[2].v[1] * m2.row[1].v[1] +
    m1.row[2].v[2] * m2.row[2].v[1] ;
  row[2].v[2] =
    m1.row[2].v[0] * m2.row[0].v[2] +
    m1.row[2].v[1] * m2.row[1].v[2] +
    m1.row[2].v[2] * m2.row[2].v[2] ;

  row[3].v[0] =
    m1.row[3].v[0] * m2.row[0].v[0] +
    m1.row[3].v[1] * m2.row[1].v[0] +
    m1.row[3].v[2] * m2.row[2].v[0] + m2.row[3].v[0] ;
  row[3].v[1] =
    m1.row[3].v[0] * m2.row[0].v[1] +
    m1.row[3].v[1] * m2.row[1].v[1] +
    m1.row[3].v[2] * m2.row[2].v[1] + m2.row[3].v[1] ;
  row[3].v[2] =
    m1.row[3].v[0] * m2.row[0].v[2] +
    m1.row[3].v[1] * m2.row[1].v[2] +
    m1.row[3].v[2] * m2.row[2].v[2] + m2.row[3].v[2] ;
}












inline
void 
lsTransform::
thisIsFirstThenInverseSecond( const lsTransform& m1, 
			      const lsTransform& m2 )
{

#ifdef PS2

    unsigned int a=(unsigned int)&(m1.row[0].v[0]);
    unsigned int b=(unsigned int)&(m2.row[0].v[0]);
    unsigned int c=(unsigned int)&(row[0].v[0]);

    if(ALIGNED(a) && ALIGNED(b) && ALIGNED(c)){

#error commented out for gcc3.
/*
        __asm__ __volatile__("
        __expression_asm

        a0
        a1
        a2

        b0
        b1
        b2

        # transposed
        bt0
        bt1
        bt2

        r0
        r1
        r2

        at
        bt
        diff
        rt

        zero

        'lqc2  @a0,0x0(%0)
        'lqc2  @a1,0x10(%0)
        'lqc2  @a2,0x20(%0)
        'lqc2  @b0,0x0(%1)
        'lqc2  @b1,0x10(%1)
        'lqc2  @b2,0x20(%1)
        'lqc2  @at,0x0(%2)
        'lqc2  @bt,0x0(%3)

        zero = K - K

        # Make b transposed
        bt0.x = zero + b0.x
        bt0.y = zero + b1.x
        bt0.z = zero + b2.x

        bt1.x = zero + b0.y
        bt1.y = zero + b1.y
        bt1.z = zero + b2.y

        bt2.x = zero + b0.z
        bt2.y = zero + b1.z
        bt2.z = zero + b2.z

        # Rotation Matrix Multiply part.
        ACC = K - K
        ACC = ACC + bt0 * a0.x
        ACC = ACC + bt1 * a0.y
        ACC = ACC + bt2 * a0.z
        r0  = ACC + K * K.x

        ACC = K - K
        ACC = ACC + bt0 * a1.x
        ACC = ACC + bt1 * a1.y
        ACC = ACC + bt2 * a1.z
        r1  = ACC + K * K.x

        ACC = K - K
        ACC = ACC + bt0 * a2.x
        ACC = ACC + bt1 * a2.y
        ACC = ACC + bt2 * a2.z
        r2  = ACC + K * K.x

        # Translation Vector part.
        diff = at - bt

        ACC = K - K
        ACC = ACC + bt0 * diff.x
        ACC = ACC + bt1 * diff.y
        ACC = ACC + bt2 * diff.z
        rt  = ACC + K * K.x

        'sqc2  @r0,0x0(%4)
        'sqc2  @r1,0x10(%4)
        'sqc2  @r2,0x20(%4)
        'sqc2  @rt,0x0(%5)

        ~a0
        ~a1
        ~a2
        ~b0
        ~b1
        ~b2
        ~bt0
        ~bt1
        ~bt2
        ~r0
        ~r1
        ~r2
        ~at
        ~bt
        ~diff
        ~rt
        ~zero

        __end_expression_asm

        " : : "r" (&(m1.row[0].v[0])),
              "r" (&(m2.row[0].v[0])),
              "r" (&(m1.row[3].v[0])),
              "r" (&(m2.row[3].v[0])),
              "r" (&(row[0].v[0])),
              "r" (&(row[3].v[0])));

*/
    } else{

        // Unaligned operands
#error commented out for gcc3.
/*

        __asm__ __volatile__("
        __expression_asm

        a0
        a1
        a2

        # transposed
        bt0
        bt1
        bt2

        r0
        r1
        r2

        at
        bt
        diff
        rt

        buf0
        buf1
        buf2

        zero

        zero = K - K

        #a

        'lw  $8, 0x0(%0)
        'lw  $9, 0x4(%0)
        'lw  $10,0x8(%0)

        'qmtc2 $8,@a0
        'qmtc2 $9,@buf0
        'qmtc2 $10,@buf1

        a0.y = zero + buf0.x
        a0.z = zero + buf1.x

        'lw  $8, 0x10(%0)
        'lw  $9, 0x14(%0)
        'lw  $10,0x18(%0)

        'qmtc2 $8,@a1
        'qmtc2 $9,@buf0
        'qmtc2 $10,@buf1   

        a1.y = zero + buf0.x
        a1.z = zero + buf1.x

        'lw  $8, 0x20(%0)
        'lw  $9, 0x24(%0)
        'lw  $10,0x28(%0)

        'qmtc2 $8,@a2
        'qmtc2 $9,@buf0
        'qmtc2 $10,@buf1

        a2.y = zero + buf0.x
        a2.z = zero + buf1.x

        # b transposed

        'lw  $8, 0x0(%1)
        'lw  $9, 0x4(%1)
        'lw  $10,0x8(%1)

        'qmtc2 $8,@bt0
        'qmtc2 $9,@buf0
        'qmtc2 $10,@buf1

        bt1.x = zero + buf0.x
        bt2.x = zero + buf1.x

        'lw  $8, 0x10(%1)
        'lw  $9, 0x14(%1)
        'lw  $10,0x18(%1)

        'qmtc2 $8,@buf0
        'qmtc2 $9,@buf1
        'qmtc2 $10,@buf2

        bt0.y = zero + buf0.x
        bt1.y = zero + buf1.x
        bt2.y = zero + buf2.x

        'lw  $8, 0x20(%1)
        'lw  $9, 0x24(%1)
        'lw  $10,0x28(%1)

        'qmtc2 $8,@buf0
        'qmtc2 $9,@buf1
        'qmtc2 $10,@buf2

        bt0.z = zero + buf0.x
        bt1.z = zero + buf1.x
        bt2.z = zero + buf2.x

        # at

        'lw  $8, 0x0(%2)
        'lw  $9, 0x4(%2)
        'lw  $10,0x8(%2)

        'qmtc2 $8,@at
        'qmtc2 $9,@buf0
        'qmtc2 $10,@buf1

        at.y = zero + buf0.x
        at.z = zero + buf1.x

        # bt

        'lw  $8, 0x0(%3)
        'lw  $9, 0x4(%3)
        'lw  $10,0x8(%3)

        'qmtc2 $8,@bt
        'qmtc2 $9,@buf0
        'qmtc2 $10,@buf1

        bt.y = zero + buf0.x
        bt.z = zero + buf1.x

        # Rotation Matrix Multiply part.
        ACC = K - K
        ACC = ACC + bt0 * a0.x
        ACC = ACC + bt1 * a0.y
        ACC = ACC + bt2 * a0.z
        r0  = ACC + K * K.x

        ACC = K - K
        ACC = ACC + bt0 * a1.x
        ACC = ACC + bt1 * a1.y
        ACC = ACC + bt2 * a1.z
        r1  = ACC + K * K.x

        ACC = K - K
        ACC = ACC + bt0 * a2.x
        ACC = ACC + bt1 * a2.y
        ACC = ACC + bt2 * a2.z
        r2  = ACC + K * K.x

        # Translation Vector part.
        diff = at - bt

        ACC = K - K
        ACC = ACC + bt0 * diff.x
        ACC = ACC + bt1 * diff.y
        ACC = ACC + bt2 * diff.z
        rt  = ACC + K * K.x

        # 'sqc2  @r0,0x0(%4)

        buf0.x = zero + r0.y
        buf1.x = zero + r0.z  
        'qmfc2  $8,@r0
        'qmfc2  $9,@buf0
        'qmfc2  $10,@buf1
        'sw     $8,0x0(%4)
        'sw     $9,0x4(%4)
        'sw     $10,0x8(%4)

        # 'sqc2  @r1,0x10(%4)

        buf0.x = zero + r1.y
        buf1.x = zero + r1.z    
        'qmfc2  $8,@r1
        'qmfc2  $9,@buf0
        'qmfc2  $10,@buf1
        'sw     $8,0x10(%4)
        'sw     $9,0x14(%4)
        'sw     $10,0x18(%4)

        # 'sqc2  @r2,0x20(%4)

        buf0.x = zero + r2.y
        buf1.x = zero + r2.z    
        'qmfc2  $8,@r2
        'qmfc2  $9,@buf0
        'qmfc2  $10,@buf1
        'sw     $8,0x20(%4)
        'sw     $9,0x24(%4)
        'sw     $10,0x28(%4)

        # 'sqc2  @rt,0x0(%5)

        buf0.x = zero + rt.y
        buf1.x = zero + rt.z    
        'qmfc2  $8,@rt
        'qmfc2  $9,@buf0
        'qmfc2  $10,@buf1
        'sw     $8,0x0(%5)
        'sw     $9,0x4(%5)
        'sw     $10,0x8(%5)

        ~a0
        ~a1
        ~a2
        ~bt0
        ~bt1
        ~bt2
        ~r0
        ~r1
        ~r2
        ~at
        ~bt
        ~diff
        ~rt
        ~zero
        ~buf0
        ~buf1
        ~buf2

        __end_expression_asm

        " : : "r" (&(m1.row[0].v[0])),
              "r" (&(m2.row[0].v[0])),
              "r" (&(m1.row[3].v[0])),
              "r" (&(m2.row[3].v[0])),
              "r" (&(row[0].v[0])),
              "r" (&(row[3].v[0]))
              : "$8", "$9", "$10");
*/

    }
#else // PS2    

  row[0].v[0] = 
    m1.row[0].v[0] * m2.row[0].v[0] + 
    m1.row[0].v[1] * m2.row[0].v[1] + 
    m1.row[0].v[2] * m2.row[0].v[2] ; 
  row[0].v[1] = 
    m1.row[0].v[0] * m2.row[1].v[0] + 
    m1.row[0].v[1] * m2.row[1].v[1] + 
    m1.row[0].v[2] * m2.row[1].v[2] ; 
  row[0].v[2] = 
    m1.row[0].v[0] * m2.row[2].v[0] + 
    m1.row[0].v[1] * m2.row[2].v[1] + 
    m1.row[0].v[2] * m2.row[2].v[2] ; 

  row[1].v[0] = 
    m1.row[1].v[0] * m2.row[0].v[0] + 
    m1.row[1].v[1] * m2.row[0].v[1] + 
    m1.row[1].v[2] * m2.row[0].v[2] ; 
  row[1].v[1] = 
    m1.row[1].v[0] * m2.row[1].v[0] + 
    m1.row[1].v[1] * m2.row[1].v[1] + 
    m1.row[1].v[2] * m2.row[1].v[2] ; 
  row[1].v[2] = 
    m1.row[1].v[0] * m2.row[2].v[0] + 
    m1.row[1].v[1] * m2.row[2].v[1] + 
    m1.row[1].v[2] * m2.row[2].v[2] ; 

  row[2].v[0] = 
    m1.row[2].v[0] * m2.row[0].v[0] + 
    m1.row[2].v[1] * m2.row[0].v[1] + 
    m1.row[2].v[2] * m2.row[0].v[2] ; 
  row[2].v[1] = 
    m1.row[2].v[0] * m2.row[1].v[0] + 
    m1.row[2].v[1] * m2.row[1].v[1] + 
    m1.row[2].v[2] * m2.row[1].v[2] ; 
  row[2].v[2] = 
    m1.row[2].v[0] * m2.row[2].v[0] + 
    m1.row[2].v[1] * m2.row[2].v[1] + 
    m1.row[2].v[2] * m2.row[2].v[2] ; 

  lsVec3 diff = m1.t - m2.t ;

  row[3].v[0] = 
    diff.v[0] * m2.row[0].v[0] + 
    diff.v[1] * m2.row[0].v[1] + 
    diff.v[2] * m2.row[0].v[2] ; 
  row[3].v[1] = 
    diff.v[0] * m2.row[1].v[0] + 
    diff.v[1] * m2.row[1].v[1] + 
    diff.v[2] * m2.row[1].v[2] ; 
  row[3].v[2] = 
    diff.v[0] * m2.row[2].v[0] + 
    diff.v[1] * m2.row[2].v[1] + 
    diff.v[2] * m2.row[2].v[2] ; 
#endif // PS2

}

inline
void 
lsTransform::
thisIsFirstToSecond( const lsTransform& m1, const lsTransform& m2)
{
  thisIsFirstThenInverseSecond( m1 , m2 );
}
 
LCE_API_EXPORT
ostream& operator << (ostream& os, const lsTransform& m);
 
    
#endif
