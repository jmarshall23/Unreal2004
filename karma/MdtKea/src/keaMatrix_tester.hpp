#ifndef _KEAMATRIX_TESTER_HPP
#define _KEAMATRIX_TESTER_HPP

#include <MePrecision.h>
#include <keaInternal.hpp>
#include <keaMatrix.hpp>

class keaMatrix_tester : public keaMatrix
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
        const MeReal *  Ainv,            /* Input */
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

    void writebackMatrixChol();

    void prefetchMatrixChol();

    keaMatrix *suspect;
    keaMatrix *correct;

    int *     suspectCached;
    int *     correctCached;
    MeReal *  suspectQrhs; 
    MeReal *  correctQrhs; 
    MeReal *  suspectX; 
    MeReal *  correctX; 
    MeReal *  suspectB; 
    MeReal *  correctB; 
    MeReal *  suspectAinv; 
    MeReal *  correctAinv; 

};

#endif

