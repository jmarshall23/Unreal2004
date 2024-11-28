#ifndef _KEAMATRIXPS2SPARSE_HPP
#define _KEAMATRIXPS2SPARSE_HPP

#include <MePrecision.h>
#include <keaInternal.hpp>
#include <keaMatrix.hpp>

class keaMatrix_ps2sparse: public keaMatrix
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
         MeReal       b[],      /* Output */
         const MeReal x[]);     /* Input  */
    
    void solveUnits(
         MeReal       Ainv[],          /* Output */
         int          cached[],        /* Output / Input  */
         const int    clamped[],       /* Input  */
         int          numClamped,      /* Input  */
         int          AinvStride);     /* Input  */

    MeReal*  rhs;
    MeReal*  matrix;
    MeReal*  matrixChol;
    MeReal*  zeroBlock;
    MeReal** nzPairs;

    int ld;
    int num_12_blocks;
    int * llist;
    int * rlist;
    int * llist_len;
    int * rlist_len;
    int * rlist_len_copy;

};

void make_first_row_vif_prog(float *Ajj,int len_xs,float **xs);
void send_microcode_ssyrk_chol_sgemm_strsm_to_scratchpad();
void execute_vif_chain();
void send_microcode_1212sgemv();
void make_fold_1212sgemm_thenstrsm_vifchain(float** xs, int len_xs,float* S);
void send_microcode_1212sgemm_strsm();
void BkMat_strsv4ColLowerNoTransNonUnit3A3X1(const float *A, float *X);


#endif