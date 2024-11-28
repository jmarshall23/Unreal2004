#ifndef _KEAMATRIX_HPP
#define _KEAMATRIX_HPP

#include <MePrecision.h>
#include <keaInternal.hpp>

class keaMatrix
{
public:
    virtual void allocate(int n)     = 0;

    virtual void makeFromJMJT(const MeReal *  JM,
                              const MeReal *  Js,
                              const int *     num_in_strip,
                              const int *     block2body,
                              const MeReal *  slipfactor,
                              const MeReal    epsilon,
                              const MeReal    hinv) = 0;

    virtual void makeFromColMajorPSM(
             MeReal          Qrhs[],          /* Output */
             const MeReal *  Ainv,            /* Input */
             const MeReal    clampedValues[], /* Input */
             const MeReal    initialSolve[],  /* Input */
             const int       unclamped[],     /* Input */
             const int       clamped[],       /* Input */
             int             usize,           /* Input */
             int             csize,           /* Input */
             int             n_padded) = 0;   /* Input */

    virtual void factorize()              = 0;

    virtual void solve(MeReal x[],
                       const MeReal rhs[])= 0;

    virtual void multiply(
         MeReal       b[],      /* Output */
         const MeReal x[]) = 0; /* Input  */
    
    virtual void solveUnits(
         MeReal       Ainv[],          /* Output */
         int          cached[],        /* Output / Input  */
         const int    clamped[],       /* Input  */
         int          numClamped,      /* Input  */
         int          AinvStride) = 0; /* Input  */

    /* On PS2, use this method to write matrixChol back to main memory */
    virtual void writebackMatrixChol();

    /* On PS2, use this method to prefetch matrixChol into vumem */
    virtual void prefetchMatrixChol();

    int m_numRows;
    int m_padded;

    MeReal* matrix;
    MeReal* matrixChol;
};

keaMatrix * keaAMatrixFactory(int num_rows,MeCPUResources cpu_resources); /* Input */
keaMatrix * keaQMatrixFactory(int numQrows,                  /* Input */
                              int numArows,                  /* Input */
                              MeCPUResources cpu_resources); /* Input */

#endif

