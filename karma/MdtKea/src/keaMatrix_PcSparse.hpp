#ifndef _KEAMATRIX_PCSPARSE_HPP
#define _KEAMATRIX_PCSPARSE_HPP

#include "keaMatrix.hpp"

class keaMatrix_pcSparse: public keaMatrix
{
public:
    int m_blocks;

    MeReal* rsD;

    MeReal** NAZ;
    MeReal** NCZ;
    MeReal*  mLP;
    MeReal*  mcLP;

    int*  NR;
    int*  NC;

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

    void makeFromPcSparsePSM(
             MeReal                      Qrhs[],          /* Output */
             const keaMatrix_pcSparse *  A,               /* Input */
             const MeReal                b[],             /* Input */
             const MeReal                clampedValues[], /* Input */
             const int                   unclamped[],     /* Input */
             const int                   clamped[],       /* Input */
             int                         usize,           /* Input */
             int                         csize,           /* Input */
             int                         n_blocks);       /* Input */    
};

#endif
