/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.16.2.1 $

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
#ifndef _LSVEC4_H
#define _LSVEC4_H

#include <math.h>               // for sqrt
#include <MePrecision.h>
#include "lsVec3.h"

#ifdef LS_USE_OSTREAM
#ifdef WIN32
  #include <iostream>
  using std::ostream;
#else
  #include <iostream.h>
#endif
#endif

class lsTransform;

// new declarations


class lsVec4;

// class definition

class lsVec4

{
public:
  MeReal v[4];

public:

  lsVec4()
  {
  }

/* All these functions are declared inline, because otherwise MSVC can't inline them (in contravention
   of ANSI and the MSVC docs, which says that inline directives here are irrelevant */

  explicit
  inline lsVec4(const MeReal t1, const MeReal t2, const MeReal t3, const MeReal t4)
  {
    v[0] = t1; v[1] = t2; v[2] = t3; v[3] = t4;
  }

  // definitions taken from svector

  inline lsVec4(const lsVec4& V)
  {
    v[0] = V[0]; v[1] = V[1]; v[2] = V[2]; v[3] = V[3];
  }

  inline void setValue(const MeReal x, const MeReal y, const MeReal z, const MeReal w)
  {
     v[0] = x; v[1] = y; v[2] = z; v[3] = w;
  }

  inline void setValue(const MeReal *V)
  {
    v[0] = V[0]; v[1] = V[1]; v[2] = V[2]; v[3] = V[3];
  }

  inline const MeReal *getValue() const {return v;}

  inline void getValue(MeReal *x, MeReal *y, MeReal *z, MeReal *w) const
  {
      *x=v[0]; *y=v[1]; *z=v[2]; *w = v[3];
  }

  inline void getValue(MeReal *v1) const
  {
      v1[0] = v[0]; v1[1] = v[1]; v1[2] = v[2]; v1[3] = v[3];
  }

#ifndef _ME_API_DOUBLE
  inline void setValue(const double *V)
  {
      v[0] = (MeReal)V[0]; v[1] = (MeReal)V[1]; v[2] = (MeReal)V[2]; v[3] = (MeReal)V[3];
  }

  inline void getValue(double *v1) const
  {
      v1[0] = (double)v[0]; v1[1] = (double)v[1]; v1[2] = (double)v[2]; v1[3] = (double)v[3];
  }
#endif

  inline const MeReal& operator[](size_t i) const {return v[i];}

  inline MeReal& operator[](size_t i) {return v[i];}

  inline lsVec4& operator=(const lsVec4& v1)
  {
    v[0] = v1[0]; v[1] = v1[1]; v[2] = v1[2]; v[3] = v1[3];
    return *this;
  }

  inline lsVec4& operator+=(const lsVec4& v1)
  {
    v[0] += v1[0]; v[1] += v1[1]; v[2] += v1[2]; v[3] += v1[3];
    return *this;
  }


  inline lsVec4& operator-=(const lsVec4& v1)
  {
    v[0] -= v1[0]; v[1] -= v1[1]; v[2] -= v1[2]; v[3] -= v1[3];
    return *this;
  }


  inline lsVec4& operator*=(const lsVec4& v1)
  {
    v[0] *= v1[0]; v[1] *= v1[1]; v[2] *= v1[2]; v[3] *= v1[3];
    return *this;
  }

  inline lsVec4& operator*=(const MeReal x)
  {
    v[0] *= x; v[1] *= x; v[2] *= x; v[3] *= x;
    return *this;
  }

  inline lsVec4& operator/=(const MeReal x)
  {
    MeReal recipX = (MeReal)(1.0)/x;
    v[0] *= recipX; v[1] *= recipX; v[2] *= recipX; v[3] *= recipX;
    return *this;
  }

  inline lsVec4 operator*(const MeReal t) const
  {
    return lsVec4(v[0]*t, v[1]*t, v[2]*t, v[3]*t);
  }

  inline lsVec4 operator/(const MeReal t) const
  {
    MeReal recipT = (MeReal)(1.0)/t;
    return lsVec4(v[0]*recipT, v[1]*recipT, v[2]*recipT, v[3]*recipT);
  }

  inline unsigned int operator==(const MeReal x) const
  {
    return ( v[0] == x && v[1] == x && v[2] == x && v[3] == x );
  }

  inline unsigned int operator!=(const MeReal x) const
  {
    return !( *this == x);
  }

  inline unsigned int operator==(const lsVec4& v1) const
  {
    // using MeReal equal
    return ( v[0]==v1[0] && v[1]==v1[1] && v[2]==v1[2] && v[3]==v1[3] );
  }

  inline unsigned int operator!=(const lsVec4& v1) const
  {
    // using MeReal equal
    return ( v[0]!=v1[0] || v[1]!=v1[1] || v[2]!=v1[2] || v[3]!=v1[3] );
  }

  inline MeReal dot(const lsVec4& v1) const
  {
    return v[0]*v1[0] + v[1]*v1[1] + v[2]*v1[2] + v[3]*v1[3];
  }

  inline MeReal square_norm(void) const {return this->dot(*this);}
  inline MeReal norm(void) const {return MeSqrt(this->dot(*this));}

  inline MeReal normalize()
  {
    MeReal t = norm();
    // if (fabs(t) < ??) should warn
    if(t) *this /= t;
    // else *this = lsVec4(0,0,0,0);
    return t;
  }

  inline lsVec4 operator+(const lsVec4& v2) const
  {
    return lsVec4(v[0] + v2[0], v[1] + v2[1], v[2] + v2[2], v[3] + v2[3]);
  }

  inline lsVec4 operator-(const lsVec4& v2) const
  {
    return lsVec4(v[0] - v2[0], v[1] - v2[1], v[2] - v2[2], v[3] - v2[3]);
  }

  inline lsVec4 operator-() const
  {
    return lsVec4(-v[0],-v[1],-v[2],-v[3]);
  }

  inline void addMultiple(const MeReal a, const lsVec4 &v2)
  {
    v[0] += a*v2[0]; v[1] += a*v2[1]; v[2] += a*v2[2]; v[3] += a*v2[3];

  }

  inline void substractMultiple(const MeReal a, const lsVec4 &v2)
  {
    v[0] -= a*v2[0]; v[1] -= a*v2[1]; v[2] -= a*v2[2]; v[3] -= a*v2[3];
  }


  inline unsigned int operator < (const lsVec4 &v1) const
  {
      int res = 0,i;
      for(i=0;i<4;i++)
          if(v[i]<v1[i]) res|=(1<<i);
          return res;
  }
  
  inline unsigned int operator <= (const lsVec4 &v1) const
  {
      int res = 0,i;
      for(i=0;i<4;i++)
          if(v[i]<=v1[i]) res|=(1<<i);
          return res;
  }
  
  inline unsigned int operator > (const lsVec4 &v1) const
  {
      int res = 0,i;
      for(i=0;i<4;i++)
          if(v[i]>v1[i]) res|=(1<<i);
          return res;
  }
  
  inline unsigned int operator >= (const lsVec4 &v1) const
  {
      int res = 0,i;
      for(i=0;i<4;i++)
          if(v[i]>=v1[i]) res|=(1<<i);
          return res;
  }
  


};

// non member functions

inline lsVec4
operator * (const MeReal x, const lsVec4& v)
{
  return lsVec4(x*v[0],x*v[1],x*v[2],x*v[3]);
}


#ifdef LS_USE_OSTREAM
LCE_API_EXPORT
ostream& operator << (ostream& os, const lsVec4& v);
#endif

#endif /* _LSVEC4_H */
