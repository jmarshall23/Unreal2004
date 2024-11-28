#ifndef _KEAMATRIXPS2SMALLDENSE_HPP
#define _KEAMATRIXPS2SMALLDENSE_HPP

#include <MePrecision.h>
#include <keaInternal.hpp>
#include <keaMatrix.hpp>

class keaMatrix_ps2smalldense: public keaMatrix
{
public:
    void allocate(int n);

    void makeFromJMJT(const MeReal *  JM,
                      const MeReal *  Js,
                      const int *     num_in_strip,
                      const int *     block2body,
                      const MeReal *  slipfactor,
                      const MeReal    epsilon,
                      const MeReal    hinv);

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
         MeReal       b[],  /* Output */
         const MeReal x[]); /* Input  */
    
    void solveUnits(
         MeReal       Ainv[],          /* Output */
         int          cached[],        /* Output / Input  */
         const int    clamped[],       /* Input  */
         int          numClamped,      /* Input  */
         int          AinvStride);     /* Input  */

    /* On PS2, use this method to write matrixChol back to main memory */
    void writebackMatrixChol();

    /* On PS2, use this method to prefetch matrixChol into vumem */
    void prefetchMatrixChol();

    //int m_c16c12n;
    int num_12_blocks;

    MeReal* recipSqrt;
    MeReal* matrix;
    MeReal* matrixChol;
};

#endif