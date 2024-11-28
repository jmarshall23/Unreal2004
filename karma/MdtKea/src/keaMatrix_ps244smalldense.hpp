#ifndef _KEAMATRIXPS244SMALLDENSE_HPP
#define _KEAMATRIXPS244SMALLDENSE_HPP

#include <MePrecision.h>
#include <keaInternal.hpp>
#include <keaMatrix.hpp>

class keaMatrix_ps244smalldense: public keaMatrix
{
public:
    void allocate(int n);

    void makeFromJMJT(const MeReal *  JM,           /* Input */
                      const MeReal *  Js,           /* Input */ 
                      const int *     num_in_strip, /* Input */
                      const int *     block2body,   /* Input */
                      const MeReal *  slipfactor,   /* Input */
                      const MeReal    epsilon,      /* Input */
                      const MeReal    hinv);        /* Input */

    void makeFromColMajorPSM(
             MeReal          Qrhs[],          /* Output */
             const MeReal *  A,               /* Input */
             const MeReal    clampedValues[], /* Input */
             const MeReal    initialSolve[],  /* Input */
             const int       unclamped[],     /* Input */
             const int       clamped[],       /* Input */
             int             usize,           /* Input */
             int             csize,           /* Input */
             int             n_padded);       /* Input */

    void factorize();

    void solve(MeReal x[],
               const MeReal rhs[]);

    void multiply(
         MeReal       b[],      /* Output */
         const MeReal x[]);     /* Input  */
    
    void solveUnits(
         MeReal       Ainv[],          /* Output */
         int          cached[],        /* Output / Input  */
         const int    clamped[],       /* Input  */
         int          numClamped,      /* Input  */
         int          AinvStride);     /* Input  */

    MeReal* rhs;
    MeReal* matrix;
    MeReal* matrixChol;
};

#endif