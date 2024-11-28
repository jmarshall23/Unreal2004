/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:56 $ - Revision: $Revision: 1.9.4.1 $

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
#ifndef _MESFFNMIN_H
#define _MESFFNMIN_H

#include <MePrecision.h>

// From Numerical Recipes in C, Chapter 10.

// Maximum function domain dimensionality (for pre-allocated vectors):
#define kMaxD 4

// Minimizes a function in N dimensions.  p is the starting point of the search, tol is the
// abcissa tolerance for the minimum.  fTol is the function value tolerance for the minimum.
// fis the function to be minimized, and grad is its gradient.  The return value is the
// function value at the minimum.
MeReal FnMinNd(const int N, MeReal *p, const MeReal tol, const MeReal fTol,
         MeReal (*f)(MeReal * const),
         void (*grad)(MeReal *, MeReal * const));

// Minimizes a function in 1 dimension.  p is the starting point of the search,
// fTol is the function value tolerance for the minimum.  f is the function to be minimized,
// and grad is its derivative.  The return value is the function value at the minimum.
MeReal FnMin1d(MeReal &p, const MeReal fTol,
         MeReal (*f)(const MeReal ), MeReal (*df)(const MeReal ));

// 1-D minimization in N-dimensional space.
MeReal f1d(const int N, const MeReal x);
MeReal df1d(const int N, const MeReal x);
MeReal LineFnMinNd(const int N, MeReal *p, MeReal *xi,
           MeReal (*f)(MeReal * const),
           void (*grad)(MeReal *, MeReal * const));

// 1-D minimization.
void mnbrak(MeReal &ax, MeReal &bx, MeReal &cx, MeReal &fa, MeReal &fb, MeReal &fc,
      MeReal (*f)(const MeReal ));
MeReal dbrent(MeReal &xmin, MeReal ax, MeReal bx, MeReal cx,
        MeReal (*f)(const MeReal ), MeReal (*df)(const MeReal ),
        MeReal tol);

#endif // _MESFFNMIN_H
