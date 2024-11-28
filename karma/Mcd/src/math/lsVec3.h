/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.30.2.1 $

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
#ifndef LSVEC3_H
#define LSVEC3_H

#ifdef LS_USE_OSTREAM
#   ifdef WIN32
#       include <iostream>
        using std::ostream;
#   else
#       include <iostream.h>
#   endif
#endif

#ifdef SSE
#   include <xmmintrin.h>
#endif

#include <math.h>               // for MeSqrt
#include <stdlib.h>             // for size_t
#include <MePrecision.h>

// new declarations

class lsVec3;

#if 0
    inline void cross(const lsVec3&, const lsVec3&, lsVec3*);
    inline MeReal dot(const lsVec3&, const lsVec3&);

    int iLargest(const lsVec3& v0,const lsVec3& v1,const lsVec3& v2);
#endif

// class definition (not declaration!)

class lsVec3
{
public:
#if defined(SSE)
    _MM_ALIGN16 MeReal v[4];
#elif defined(PS2)
    MeReal	v[4];
#else
    MeReal	v[3];
#endif

public:

    lsVec3()
    {
    }
    
    /*
        All these functions are declared inline, because otherwise MSVC
        can't inline them (in contravention of ANSI and the MSVC docs,
        which says that inline directives here are irrelevant
    */
    
    explicit
        inline lsVec3(const MeReal t1, const MeReal t2, const MeReal t3)
    {
        v[0] = t1; v[1] = t2; v[2] = t3;
    }
    
    // definitions taken from svector
    
    inline lsVec3(const lsVec3& V)
    {
        v[0] = V[0]; v[1] = V[1]; v[2] = V[2];
    }
    
    inline void setValue(const MeReal x, const MeReal y, const MeReal z)
    {
        v[0] = x; v[1] = y; v[2] = z;
    }
    
    inline void setZero()
    {
        v[0] = v[1] = v[2] = (MeReal)(0.);
    }
    
    inline const MeReal *getValue() const {return v;}
    
    inline void getValue(double *x, double *y, double *z) const
    {
        *x = (double) v[0]; *y = (double) v[1]; *z = (double) v[2];
    }
    
    inline void getValue(double *v1) const
    {
        v1[0] = (double) v[0]; v1[1] = (double) v[1]; v1[2] = (double) v[2];
    }
    
    inline void getValue(float *x, float *y, float *z) const
    {
        *x = (float) v[0]; *y = (float) v[1]; *z = (float) v[2];
    }
    
    inline void getValue(float *v1) const
    {
        v1[0] = (float) v[0]; v1[1] =  (float) v[1]; v1[2] =  (float) v[2];
    }
    
    inline void setValue(const double *V)
    {
        v[0] = (MeReal) V[0]; v[1] = (MeReal) V[1]; v[2] = (MeReal) V[2];
    }
    
    inline void setValue(const float *V)
    {
        v[0] = (MeReal) V[0]; v[1] = (MeReal) V[1]; v[2] = (MeReal) V[2];
    }
    
    inline static lsVec3 abs(const lsVec3 &v)
    { 
        return lsVec3(MeFabs(v[0]), MeFabs(v[1]),MeFabs(v[2]));
    }
    
    inline const MeReal& operator[](size_t i) const {return v[i];}
    
    inline MeReal& operator[](size_t i) {return v[i];}
    
    inline lsVec3& operator=(const lsVec3& v1)
    {
        v[0] = v1[0]; v[1] = v1[1]; v[2] = v1[2];
        return *this;
    }
    
    inline lsVec3& operator+=(const lsVec3& v1)
    {
        v[0] += v1[0]; v[1] += v1[1]; v[2] += v1[2];
        return *this;
    }
    
    
    inline lsVec3& operator-=(const lsVec3& v1)
    {
        v[0] -= v1[0]; v[1] -= v1[1]; v[2] -= v1[2];
        return *this;
    }
    
    
    inline lsVec3& operator*=(const lsVec3& v1)
    {
        v[0] *= v1[0]; v[1] *= v1[1]; v[2] *= v1[2];
        return *this;
    }
    
    inline lsVec3& operator*=(const MeReal x)
    {
        v[0] *= x; v[1] *= x; v[2] *= x;
        return *this;
    }
    
    inline lsVec3& operator/=(const MeReal x)
    {
        MeReal recipX = (MeReal)(1.0)/x;
        v[0] *= recipX; v[1] *= recipX; v[2] *= recipX;
        return *this;
    }
    
    inline lsVec3 operator*(const MeReal t) const
    {
        return lsVec3(v[0]*t, v[1]*t, v[2]*t);
    }
    
    inline lsVec3 operator/(const MeReal t) const
    {
        MeReal recipT = (MeReal)(1.0)/t;
        return lsVec3(v[0]*recipT, v[1]*recipT, v[2]*recipT);
    }
    
    inline unsigned int operator==(const MeReal x) const
    {
        return ( v[0] == x && v[1] == x && v[2] == x );
    }
    
    inline unsigned int operator!=(const MeReal x) const
    {
        return !( *this == x);
    }
    
    inline unsigned int operator==(const lsVec3& v1) const
    {
        // using MeReal equal
        return ( v[0]==v1[0] && v[1]==v1[1] && v[2]==v1[2] );
    }
    
    inline unsigned int checkAlmostEqual(const lsVec3& w, const MeReal eps = 0.0) const
    {
        return !(MeFabs(v[0] - w[0]) > eps ||
            MeFabs(v[1] - w[1]) > eps || fabs(v[2] - w[2]) > eps );
    }
    
    inline unsigned int operator!=(const lsVec3& v1) const
    {
        // using MeReal equal
        return ( v[0]!=v1[0] || v[1]!=v1[1] || v[2]!=v1[2] );
    }
    
    
    inline unsigned int operator > (const lsVec3 &v1) const
    {
        int res = 0,i;
        for(i=0;i<3;i++)
            if(v[i]>v1[i]) res|=(1<<i);
            return res;
    }
    
    inline unsigned int operator >= (const lsVec3 &v1) const
    {
        int res = 0,i;
        for(i=0;i<3;i++)
            if(v[i]>=v1[i]) res|=(1<<i);
            return res;
    }
    
    inline unsigned int operator < (const lsVec3 &v1) const
    {
        int res = 0,i;
        for(i=0;i<3;i++)
            if(v[i]<v1[i]) res|=(1<<i);
            return res;
    }
    inline unsigned int operator <= (const lsVec3 &v1) const
    {
        int res = 0,i;
        for(i=0;i<3;i++)
            if(v[i]<=v1[i]) res|=(1<<i);
            return res;
    }
    
    
    
    inline MeReal dot(const lsVec3& v1) const
    {
        return v[0]*v1[0] + v[1]*v1[1] + v[2]*v1[2];
    }
    
    inline MeReal square_norm(void) const {return this->dot(*this);}
    inline MeReal norm(void) const {return MeSqrt(this->dot(*this));}
    
    inline MeReal normalize()
    {
        MeReal t = norm();
        // if (fabs(t) < ??) should warn
        if(t) *this /= t;
        // else *this = lsVec3(0,0,0);
        return t;
    }
    
    inline MeReal normalize( MeReal eps )
    {
        MeReal t = norm();
        if(t>eps) *this /= t;
        return t;
    }
    
    inline lsVec3 operator+(const lsVec3& v2) const
    {
        return lsVec3(v[0] + v2[0], v[1] + v2[1], v[2] + v2[2]);
    }
    
    inline lsVec3 operator-(const lsVec3& v2) const
    {
        return lsVec3(v[0] - v2[0], v[1] - v2[1], v[2] - v2[2]);
    }
    
    inline lsVec3 operator-() const
    {
        return lsVec3(-v[0],-v[1],-v[2]);
    }
    
    inline void addMultiple(const MeReal a, const lsVec3 &v2)
    {
        v[0] += a*v2[0]; v[1] += a*v2[1]; v[2] += a*v2[2];
    }
    
    inline void substractMultiple(const MeReal a, const lsVec3 &v2)
    {
        v[0] -= a*v2[0]; v[1] -= a*v2[1]; v[2] -= a*v2[2];
    }
    
    inline static
        void
        cross( const lsVec3& v1, const lsVec3& v2, lsVec3* v )
    {
        (*v).v[0] = v1.v[1] * v2.v[2] - v1.v[2] * v2.v[1] ;
        (*v).v[1] = v1.v[2] * v2.v[0] - v1.v[0] * v2.v[2] ;
        (*v).v[2] = v1.v[0] * v2.v[1] - v1.v[1] * v2.v[0] ;
    }
    
    inline lsVec3
        cross( const lsVec3& v ) const
    {
        lsVec3 vCross;
        lsVec3::cross( *this, v, &vCross);
        return vCross;
    }
    
    static
        int iLargest( const lsVec3& v0, const lsVec3& v1, const lsVec3& v2 )
    {
        MeReal longest = v0.square_norm();
        int i = 0 ;
        if ( v1.square_norm() > longest )
        {
            longest = v1.square_norm();
            i = 1 ;
        }
        if ( v2.square_norm() > longest )
        {
            i = 2 ;
        }
        return i;
    }
    
    inline lsVec3 max(lsVec3 &v1)
    {
        lsVec3 res;
        res[0] = v1[0] > v[0] ? v1[0] : v[0];
        res[1] = v1[1] > v[1] ? v1[1] : v[1];
        res[2] = v1[2] > v[2] ? v1[2] : v[2];
        return res;
    }

    inline lsVec3 min(lsVec3 &v1)
    {
        lsVec3 res;
        res[0] = v1[0] < v[0] ? v1[0] : v[0];
        res[1] = v1[1] < v[1] ? v1[1] : v[1];
        res[2] = v1[2] < v[2] ? v1[2] : v[2];
        return res;
    }

};

// non member functions

inline lsVec3 operator *(const MeReal x, const lsVec3& v)
{
  return lsVec3(x*v[0],x*v[1],x*v[2]);
}

#if 0
    inline lsVec3 operator /(const lsVec3& v, const MeReal x)
    {
       return lsVec3(v[0]/x,v[1]/x,v[2]/x);
    }
#endif

#ifdef LS_USE_OSTREAM
    LCE_API_EXPORT ostream& operator << (ostream& os, const lsVec3& v);
#endif

#ifdef WIN32
#   undef ostream
#endif

#endif
