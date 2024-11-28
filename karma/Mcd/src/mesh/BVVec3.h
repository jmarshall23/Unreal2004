/* -*-c++-*-
 *===============================================================
 * File:        BVVec3.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.7.18.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#ifndef BVVEC3_H
#define BVVEC3_H

#include <math.h>
#include <stdlib.h>
#include "lsVec3.h"

typedef MeReal BVReal;

// class definition
class     BVVec3 {
public:
    BVReal    v[3];
    BVVec3() {} BVVec3  (const BVReal &t1, const BVReal &t2, const BVReal &t3) {
    v[0] = t1;
    v[1] = t2;
    v[2] = t3;
    }

    // definitions taken from svector

    BVVec3(const BVVec3 & V) {
    v[0] = V[0];
    v[1] = V[1];
    v[2] = V[2];
    }

    void      setValue(const BVReal &x, const BVReal &y, const BVReal &z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
    }

    void    setZero() {
    v[0] = v[1] = v[2] = (BVReal) (0.);
    }

    const BVReal *getValue() const {
    return v;
    }

    void    getValue(double *x, double *y, double *z) const {
    *x = (double) v[0];
    *y = (double) v[1];
    *z = (double) v[2];
    }

    void    getValue(double *v1) const {
    v1[0] = (double) v[0];
    v1[1] = (double) v[1];
    v1[2] = (double) v[2];
    }

    void    getValue(float *x, float *y, float *z) const {
    *x = (float) v[0];
    *y = (float) v[1];
    *z = (float) v[2];
    }

    void    getValue(float *v1) const {
    v1[0] = (float) v[0];
    v1[1] = (float) v[1];
    v1[2] = (float) v[2];
    }

    void    setValue(const double *V) {
    v[0] = (BVReal) V[0];
    v[1] = (BVReal) V[1];
    v[2] = (BVReal) V[2];
    }

    void    setValue(const float *V) {
    v[0] = (BVReal) V[0];
    v[1] = (BVReal) V[1];
    v[2] = (BVReal) V[2];
    }

    const BVReal &operator[] (size_t i) const {
    return v[i];
    }

    BVReal &operator[] (size_t i) {
    return v[i];
    }

    BVVec3 & operator = (const BVVec3 & v1) {
    v[0] = v1[0];
    v[1] = v1[1];
    v[2] = v1[2];
    return *this;
    }

    BVVec3 & operator += (const BVVec3 & v1) {
    v[0] += v1[0];
    v[1] += v1[1];
    v[2] += v1[2];
    return *this;
    }

    BVVec3 & operator -= (const BVVec3 & v1) {
    v[0] -= v1[0];
    v[1] -= v1[1];
    v[2] -= v1[2];
    return *this;
    }

    BVVec3 & operator *= (const BVVec3 & v1) {
    v[0] *= v1[0];
    v[1] *= v1[1];
    v[2] *= v1[2];
    return *this;
    }

    BVVec3 & operator *= (const BVReal &x) {
    v[0] *= x;
    v[1] *= x;
    v[2] *= x;
    return *this;
    }

    BVVec3 & operator /= (const BVReal &x) {
    BVReal    recipX = (BVReal) (1.0) / x;
    v[0] *= recipX;
    v[1] *= recipX;
    v[2] *= recipX;
    return *this;
    }

    BVVec3 operator *(const BVReal &t) const {
    return BVVec3(v[0] * t, v[1] * t, v[2] * t);
    }

    BVVec3  operator / (const BVReal &t) const {
    BVReal    recipT = (BVReal) (1.0) / t;
    return BVVec3(v[0] * recipT, v[1] * recipT, v[2] * recipT);
    }

    unsigned int /*bool */ operator == (const BVReal &x) const {
    return (v[0] == x && v[1] == x && v[2] == x);
    }

    unsigned int /*bool */ operator != (const BVReal &x) const {
    return !(*this == x);
    }

    unsigned int /*bool */ operator == (const BVVec3 & v1) const {
    // using BVReal equal
    return (v[0] == v1[0] && v[1] == v1[1] && v[2] == v1[2]);
    }

    unsigned int checkAlmostEqual(const BVVec3 & w, const BVReal &eps = 0) const {
    return !(MeFabs(v[0] - w[0]) > eps || MeFabs(v[1] - w[1]) > eps || MeFabs(v[2] - w[2]) > eps);
    }

    unsigned int /*bool */ operator != (const BVVec3 & v1) const {
    // using BVReal equal
    return (v[0] != v1[0] || v[1] != v1[1] || v[2] != v1[2]);
    }

    BVReal  dot(const BVVec3 & v1) const {
    return v[0] * v1[0] + v[1] * v1[1] + v[2] * v1[2];
    }

    MeReal  dot(const lsVec3 &v1) const {
    return (MeReal) (v[0] * v1[0] + v[1] * v1[1] + v[2] * v1[2]);
    }

    void  setLsVec(lsVec3* lsvec) const {
    lsvec->v[0] = v[0];
    lsvec->v[1] = v[1];
    lsvec->v[2] = v[2];
    }

    BVReal  square_norm(void) const {
    return this->dot(*this);
    }

    BVReal  norm(void) const {
    return (BVReal) sqrt(this->dot(*this));
    }

    BVReal  normalize() {
    BVReal    t = norm();
    // if (fabs(t) < ??) should warn
    if        (t)
        *this /= t;
    // else *this = BVVec3(0,0,0);
    return t;
    }

    BVVec3  operator + (const BVVec3 & v2) const {
    return BVVec3(v[0] + v2[0], v[1] + v2[1], v[2] + v2[2]);
    }

    BVVec3  operator - (const BVVec3 & v2) const {
    return BVVec3(v[0] - v2[0], v[1] - v2[1], v[2] - v2[2]);
    }

    BVVec3  operator - () const {
    return BVVec3(-v[0], -v[1], -v[2]);
    }

    void  addMultiple(const BVReal a, const BVVec3 & v2) {
    v[0] += a * v2[0];
    v[1] += a * v2[1];
    v[2] += a * v2[2];
    }

    void    substractMultiple(const BVReal a, const BVVec3 & v2) {
    v[0] -= a * v2[0];
    v[1] -= a * v2[1];
    v[2] -= a * v2[2];
    }

    static void cross(const BVVec3 & v1, const BVVec3 & v2, BVVec3 * v) {
    (*v).v[0] = v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1];
    (*v).v[1] = v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2];
    (*v).v[2] = v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0];
    }

    BVVec3  cross(const BVVec3 & v) const {
    BVVec3    vCross;
    BVVec3::cross(*this, v, &vCross);
    return vCross;
    }

    static int  iLargest(const BVVec3 & v0, const BVVec3 & v1, const BVVec3 & v2) {
    BVReal longest = v0.square_norm();
    int       i = 0;
    if (v1.square_norm() > longest) {
        longest = v1.square_norm();
        i = 1;
    }
    if (v2.square_norm() > longest) {
        i = 2;
    }
    return i;
    }
};

// non member functions

inline  BVVec3 operator * (const BVReal &x, const BVVec3 & v) {
    return BVVec3(x * v[0], x * v[1], x * v[2]);
}

#endif
