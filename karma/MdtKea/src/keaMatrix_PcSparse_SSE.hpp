#ifndef _KEAMATRIX_PCSPARSE_SSE_HPP
#define _KEAMATRIX_PCSPARSE_SSE_HPP

#include <keaMatrix_PcSparse.hpp>

#ifndef _BUILD_VANILLA

class keaMatrix_pcSparse_SSE: public keaMatrix_pcSparse
{
public:
    void factorize();       

    void solve(MeReal x[],
               const MeReal rhs[]);

    void allocate(int n);

    void multiply(
         MeReal       b[],      /* Output */
         const MeReal x[]);     /* Input  */

    void solveUnits(
         MeReal       Ainv[],          /* Output */
         int          cached[],        /* Output / Input  */
         const int    clamped[],       /* Input  */
         int          numClamped,      /* Input  */
         int          AinvStride);     /* Input  */

    void makeFromJMJT(
             const MeReal *  JM,
             const MeReal *  Js,
             const int *     num_in_strip,
             const int *     block2body,
             const MeReal *  slipfactor,
             const MeReal    epsilon,
             const MeReal    hinv);
};

#endif
#endif
