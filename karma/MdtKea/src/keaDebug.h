/* -*- mode: C; -*- */
#ifndef _KEADEBUG_H
#define _KEADEBUG_H

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/11/05 11:22:03 $ - Revision: $Revision: 1.41.2.7.4.1 $

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

#include <MdtKea.h>
#include "calcA.hpp"

#ifdef PS2
#include "JM_block.hpp"
#include "cf_block.hpp"
#endif /* PS2 */

/*
  Define these to print debugging info at each pipeline stage.
*/
#define PRINT_KEA_INPUT                                0
#define PRINT_KEA_INPUT_CONSTRAINTS                    0
#define PRINT_KEA_SIZES                                0
#define PRINT_XI                                       0
#define PRINT_C                                        0
#define PRINT_LO                                       0
#define PRINT_HI                                       0
#define PRINT_SLIPFACTOR                               0
#define PRINT_XGAMMA                                   0
#define PRINT_JSIZE                                    0
#define PRINT_JOFS                                     0
#define PRINT_JBODY                                    0
#define PRINT_J                                        0
#define PRINT_JNORMS                                   0

#define PRINT_KEA_INPUT_BODIES                         0
#define PRINT_KEA_INPUT_PARAMETERS                     0

/* Iworld and vhmf */

#define PRINT_CALCIWORLDNONINERTIALFORCEANDVHMF_INPUT  0
#define PRINT_CALCIWORLDNONINERTIALFORCEANDVHMF_OUTPUT 0
#define PRINT_INVIWORLD_NIF_VHMF                       0

/* jlen and block2body */

#define PRINT_JLENANDBL2BODY_INPUT                     0
#define PRINT_JLENANDBL2BODY_OUTPUT                    0

/* JM and rhs */

#define PRINT_CALCJMINVANDRHS_INPUT                    0
#define PRINT_CALCJMINVANDRHS_OUTPUT                   0
#define PRINT_JMINVANDRHSBLOCK_INPUT                   0
#define PRINT_JMINVANDRHSBLOCK_OUTPUT                  0

/* JinvMJT */

#define PRINT_CALCJMINVJT_INPUT                        0
#define PRINT_CALCJMINVJT_PS2SPARSE_OUTPUT             0
#define PRINT_CALCJMINVJT_PS2SMALLDENSE_OUTPUT         0
#define PRINT_CALCJMINVJT_PS2LDLT_OUTPUT               0
#define PRINT_JMINVJTBLOCK_INPUT                       0
#define PRINT_JMINVJTBLOCK_OUTPUT                      0

/* Factoriser */

#define PRINT_FACTORISER_INPUT                         0
#define PRINT_FACTORISER_PS2SPARSE_OUTPUT              0
#define PRINT_FACTORISER_PS2SMALLDENSE_OUTPUT          0
#define PRINT_FACTORISER_PCSPARSE_OUTPUT               0

/* LCP */

#define PRINT_SOLVE_INPUT                              0
#define PRINT_SOLVE_OUTPUT                             0
#define PRINT_SOLVE_UNIT_INPUT                         0
#define PRINT_SOLVE_UNIT_OUTPUT                        0
#define PRINT_WRITEBACKMATRIXCHOL_OUTPUT               0

#define PRINT_LCP_INITIAL_SOLVE_OUTPUT                 0
#define PRINT_GETFIRSTBADINDEX_INPUT                   0
#define PRINT_LCP_OUTPUT                               0

/* Principle submatrix technique */

#define PRINT_PSM_MAKE_Q_INPUT                         0
#define PRINT_PSM_MAKE_Q_OUTPUT                        0

/* Principle pivot technique */

#define PRINT_PPT_MAKE_Q_INPUT                         0
#define PRINT_PPT_MAKE_Q_OUTPUT                        0
#define PRINT_PPT_OUTPUT                               0

/* Calc constraint forces */

#define PRINT_CALC_CONSTRAINT_FORCES_INPUT             0
#define PRINT_CALC_CONSTRAINT_FORCES_OUTPUT            0
#define PRINT_CALC_CONSTRAINT_FORCES_BLOCK_INPUT       0
#define PRINT_CALC_CONSTRAINT_FORCES_BLOCK_OUTPUT      0

/* Calc resultant forces */

#define PRINT_CALC_RESULTANT_FORCES_INPUT              0
#define PRINT_CALC_RESULTANT_FORCES_OUTPUT             0
#define PRINT_CALC_RESULTANT_FORCES_BLOCK_INPUT        0
#define PRINT_CALC_RESULTANT_FORCES_BLOCK_OUTPUT       0

/* Integrator */

#define PRINT_INTEGRATOR_INPUT                         0
#define PRINT_INTEGRATOR_INPUT_ACCEL                   0
#define PRINT_INTEGRATOR_INPUT_TRANSFORMATIONS         0
#define PRINT_INTEGRATOR_OUTPUT                        0

/*
  Define this to dump debug data at each stage of the kea pipeline
*/

#undef DUMP_DEBUG_DATA

//#undef _NOTIC

#define PROFILE_KEA 1
#define PROFILE_LCP 0

#define TEST_KEAMATRIX 0
#define PRINT_MATRIX_TYPE 0
#define PRINT_LCP_ITERATION 0
#define WRITE_CHOL_BACK_TO_SPR 1

/* Printing functions */

void printKeaInput(const MdtKeaConstraints constraints,const MdtKeaParameters parameters,
                   const MdtKeaBody *const blist[],int num_bodies);

void printCalcIworldNonInertialForceandVhmfInput(const MdtKeaBody *blist,int num_bodies);
void printCalcIworldNonInertialForceandVhmfOutput(const MeReal *vhmf,const MeReal *invIworld,int num_bodies);

void printJlenandBl2BodyOutput(const int *jlen,const int *bl2body,const int *bl2cony, int num_strips);

void printJinvMandrhsInput(
         const MeReal                  rhs[],                /* Input */
         const MdtKeaJBlockPair        jmstore[],            /* Input */
         const MdtKeaJBlockPair        jstore[],             /* Input */
         const MeReal                  xgamma[],             /* Input */
         const MeReal                  c[],                  /* Input */
         const MeReal                  xi[],                 /* Input */
         const MdtKeaInverseMassMatrix invIworld[],          /* Input */
         const MdtKeaBl2BodyRow        bl2body[],            /* Input */
         const int                     jlen[],               /* Input */
         const MdtKeaVelocity          vhmf[],               /* Input */
         int                           num_bodies,           /* Input */
         int                           num_rows_exc_padding, /* Input */
         int                           num_rows_inc_padding, /* Input */
         MeReal                        stepsize,             /* Input */
         MeReal                        gamma);               /* Input */

void printJinvMandrhsOutput(
         const MeReal           rhs[],
         const MdtKeaJBlockPair jm[],
         int                    num_rows, 
         const int              jlen[],
         int                    num_rows_inc_padding);

void printCalcJinvMJTps2sparse_Output(const MeReal *A,const int *rlist,const int *rlist_len,int num_blocks);

void printFactoriserInput(const MeReal *newA,const MeReal *lo,const MeReal *hi,int ceil4_num_rows);

void printFactoriseps2sparse_Output(
         const MeReal A[],
         const int    rlist[],
         const int    rlist_len[],
         int          num_blocks);

void printLCPInitialSolveOutput(const MeReal *x,int c4n);

void printCalculateConstraintForcesOutput(
         MdtKeaBody *const      blist[],
         const MdtKeaForcePair  cforces[],
         int                    num_bodies,
         int                    num_constraints);
 
void printCalcConstraintForcesInput(
         const MeReal              lambda[],
         const MdtKeaBl2CBodyRow   bl2cbody[],
         const MdtKeaBl2BodyRow    bl2body[],
         int                       c4size);

void printMakeFromColMajorPSMInput(
        const MeReal A[],               /* Input */
        const MeReal clampedValues[],   /* Input */
        const MeReal initialSolve[],    /* Input */
        const int    unclamped[],       /* Input */
        const int    clamped[],         /* Input */
        int          numUnclamped,      /* Input */
        int          numClamped,        /* Input */
        int          n_padded,          /* Input */
        int          AinvStride);       /* Input */

void printMakeFromColMajorPSMOutput(const MeReal         rsD[],
                                    const MeReal *const  NAZ[],
                                    const MeReal *const  NCZ[],
                                    const int            NR[],
                                    const int            NC[],
                                    int                  m_blocks,
                                    int                  m_padded);

void printMakeFromColMajorPSMOutput_ps2smalldense(
        MeReal Q[],        /* Input */
        int    c4numRows); /* Input */

void printPrinciplePivotTransformOutput(const MeReal x[],
                                        const MeReal w[],
                                        int          n);

void printGetFirstBadIndexInput(const MeReal x[],
                                int          n);

void printSolveInput(const MeReal rhs[],
                     int          num_blocks);

void printSolvePCSparseSSEOutput(const MeReal x[],
                                 int          m_padded);

void printLCPOutput(const MeReal x[],
                    int   c4numRows);

void printFactoriserPS2SmallDenseOutput(const MeReal A[],
                                        int          c4numRows);

void printFactoriserPCSparseOutput(MeReal *  NCZ[],
                                   int       m_blocks,
                                   int       m_padded);

void printFactoriserPS2SmallDenseInput(const MeReal A[],
                                       int          c4n);

void printMakeFromColMajorPSMOutput_ps2sparse(
         const MeReal matrix[],
         const int    rlist[],
         const int    rlist_len[],
         int          num_12_blocks);

/* PS2 Specific stuff */
#ifdef PS2
void printJinvMandrhsBlockInput(newCalc_JM_buf *buf);
void printJinvMJTBlockInput(Calc_block_buf* b);
void printJinvMJTBlockOutput(Calc_block_buf* b,int shape,int foundmatch);
void printCalcJinvMJTInput(const blocktobodyandlen *jinfo,const blocktobodyandlen *jminfo,
                           const MeReal *j,const MeReal *jm,int num_strips);
void printCalculateConstraintForcesBlockInput(Calc_forces_buf* b,int num_strips);
void printCalculateConstraintForcesBlockOutput(Calc_forces_buf* b,int num_strips);

void printCalcResultantForcesInput(const MdtKeaBody *const blist[],
                                   const MeReal *          cforces,
                                   int                     num_bodies,
                                   int                     num_constraints);

void printCalcResultantForcesOutput(const MdtKeaBody *const blist[],
                                    int num_bodies);

void printCalculateResultantForcesBlockInput(Sum_forces_buf* b,int nconstraints);
void printCalculateResultantForcesBlockOutput(Sum_forces_buf* b,int nconstraints);
void printSolvePS2SmallDenseInput(const MeReal A[],const MeReal x[],int c4n);
void printSolvePS2SmallDenseOutput(const MeReal x[],int c4n);
void printSolvePS244SmallDenseInput(const MeReal matrixChol[],const MeReal rhs[]);
void printSolvePS244SmallDenseOutput(const MeReal x[]);
void printWritebackMatrixCholOutput(const MeReal recipSqrt[],
                                    int          numElts);

#endif /* PS2 */

#endif

