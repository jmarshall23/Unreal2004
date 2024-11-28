
#include <MePrecision.h>

#ifndef KEAPRINTBASIC_TYPES_H

#define KEAPRINTBASIC_TYPES_H

void printMeReal(MeReal x,         /* Input */
                 const char *desc);/* Input */

void printColMajorMat(const MeReal  A[],   /* Input */
                      int           rows,  /* Input */
                      int           cols,  /* Input */
                      const char *  desc); /* Input */

void printIntMat(const int     A[],   /* Input */
                 int           rows,  /* Input */
                 int           cols,  /* Input */
                 const char *  desc); /* Input */

void printMat44(const MeReal  A[],   /* Input */
                const char *  desc); /* Input */

void printMat46(const MeReal  A[],   /* Input */
                const char *  desc); /* Input */

void printMat412(const MeReal  A[],   /* Input */
                 const char *  desc); /* Input */

void printIntVec(const int     A[],      /* Input */
                 int           numElts,  /* Input */                 
                 const char *  desc);    /* Input */

void printVec4(const MeReal  A[],   /* Input */
               const char *  desc); /* Input */

void printVec3(const MeReal  A[],   /* Input */
               const char *  desc); /* Input */

void printVec(const MeReal  A[],     /* Input */
              int           numElts, /* Input */
              const char *  desc);   /* Input */

void printPtr(const void *  ptr,   /* Input */
              const char *  desc); /* Input */

void printInvMassMatrix(const MdtKeaInverseMassMatrix invM,  /* Input */
                        const char *                  desc); /* Input */

void printInvMassMatrixArray(const MdtKeaInverseMassMatrix invM[],  /* Input */
                             int                           numElts, /* Input */
                             const char *                  desc);   /* Input */

void printMdtKeaForcePairArray(const MdtKeaForcePair cforces[],      /* Input */
                               int                   numConstraints);/* Input */

void printMdtKeaBl2CBodyRowArray(const MdtKeaBl2BodyRow  bl2cbody[],   /* Input */
                                 int                     numElts);     /* Input */

void printMdtKeaBl2BodyRowArray(const MdtKeaBl2BodyRow  bl2cbody[],   /* Input */
                                int                     numElts);     /* Input */

void printMdtKeaJBlockPairArray(const MdtKeaJBlockPair J[],                  /* Input */
                                int                    num_rows_inc_padding);/* Input */

void printMdtKeaVelocityArray(const MdtKeaVelocity v[],         /* Input */
                              int                  num_bodies,  /* Input */
                              const char *         desc);       /* Input */

void printHalfWallpaperMatrix(const MeReal A[],       /* Input */
                              int          c4numRows);/* Input */

void printPS2SparseMatrix(
         const MeReal matrix[],
         const int    rlist[],
         const int    rlist_len[],
         int          num_12_blocks);

#endif

