#ifndef _KEALCPSOLVER_HPP
#define _KEALCPSOLVER_HPP

#include "MePrecision.h"
#include "MdtKea.h"
#include <keaInternal.hpp>
#include "keaMatrix.hpp"

class keaLCPSolver
{
public:
    int solveLCP(
            keaMatrix *    A,
            MeReal *       b, 
            MeReal *       lower,
            MeReal *       upper,
            int            max_iterations,
            MeCPUResources cpuType,
            MeReal         velocityZeroTol);

    int  getFirstBadIndex();
    void copyXtoInitialSolve();
    void getClampIndices(int* I, int* C);
    void setClampedValues(                 
             int        clamped[],   /* Output */
             int        unclamped[], /* Output */
             int *      csize,       /* Output */
             int *      usize,       /* Output */
             const int  I[],         /* Input  */
             const int  C[]);        /* Input  */

    int  commonPivot(int MASK, int I[], int C[], int clampedhere[], int hilohere[]);

    int blockMurtyChooseNewIndices(
        int       I[],           /* Output */
        int       C[],           /* Output */
        const int clamped[],     /* Input  */       
        const int unclamped[],   /* Input  */ 
        int       num_clamped,   /* Input  */
        int       num_unclamped);/* Input  */

    int singleMurtyChooseNewIndices(
        int       I[],           /* Output */
        int       C[],           /* Output */
        const int clamped[],     /* Input  */       
        const int unclamped[],   /* Input  */ 
        const int iorder[],      /* Input  */ 
        int       num_clamped,   /* Input  */
        int       num_unclamped);/* Input  */

    void allocate(int num_rows);
  
    void makeXandW(MeReal *b,
                   int unclamped[],
                   int numUnclamped,
                   int clamped[],
                   int numClamped);

    void PrincipalSubmatrix(int* unclamped, int usize,
                            int* clamped, int msize,
                            MeReal* b);

    void PrincipalPivotTransform(
        const int unclamped[],      /* Input */
        int       numUnclamped,     /* Input */
        const int clamped[],        /* Input */
        int       numClamped);      /* Input */

    void PrincipalPivotTransformMakeW(
        MeReal       w[],           /* Output */
        const MeReal Qrhs[],        /* Input  */
        const int    clamped[],     /* Input  */
        const int    unclamped[],   /* Input  */
        int          numClamped,    /* Input  */
        int          numUnclamped); /* Input  */

    void PrincipalPivotTransformMakeX(
        MeReal       x[],            /* Output */
        const MeReal initialSolve[], /* Input  */
        const MeReal Ainv[],         /* Input  */
        const MeReal Qrhs[],         /* Input  */
        const int    clamped[],      /* Input  */
        const int    unClamped[],    /* Input  */
        int          numClamped,     /* Input  */
        int          numUnclamped,   /* Input  */
        int          n,              /* Input  */
        int          n_padded,       /* Input  */
        int          AinvStride);    /* Input  */

    void setUpper(MeReal* upper);
    void setLower(MeReal* lower);

    int AinvStride;
    int n;
    int n_blocks;
    int n_padded;
    int deadIndex;
    int c16c12n;

    MeCPUResources cpuType;

    MeReal  velocityZeroTol;
    int*    cached;

    keaMatrix* A;
    keaMatrix* Q;

    MeReal* x;
    MeReal* w;
    MeReal* upper;
    MeReal* lower;
    MeReal* initialSolve;
    MeReal* clampedValues;
    MeReal* Ainv;
    MeReal* Qrhs;

};

keaLCPSolver* keaLCPSolverFactory(MeCPUResources platform);

#endif
