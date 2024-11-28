#ifndef _KEASPARSEMATRIX_PS2_HPP
#define _KEASPARSEMATRIX_PS2_HPP
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.14.2.2 $

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
#include "keaLCP.hpp"

// Tiled matrix.  This has two levels of blocking.  Blocks of size bk
// are stored in row major order inside the big matrix with leading
// dimension ld in block units.  Those blocks contain smaller blocks of
// size bk1 which are stored in row major order with leading dimension ld1
// in units of bk1.  Finally, the data is contained in those blocks in
// column major order with leading dimension bk1.

typedef float Float;

struct keaLCPSparseMatrix_ps2: public keaLCPMatrix
{
    float *A;     // the floating point data for the matrix
    float *Achol;                // the floating point data for the factorised matrix
    int   *rlist;         // n arrays or n column indices (right lists)
    int   *llist;   // n arrays of n column indices (left lists)
    int   *rlist_len;           // rlist_len[i]=length of rlist[i]
    int   *llist_len;           // llist_len[i]=length of llist[i]
    int   num_12_blocks;  // current number of row blocks and col blocks in the array
    int   ld;     // leading dimension of the array
    int   ld1;      // leading dimension for nested blocks
        // within block

    MeReal              m_epsilon;

    void                allocate(int n);
    void                AssignWallpaperToHalfSpTileMatRight(Float *B, int ldb);
    void                AssignHalfSpTileMatLeftToStd(int n, Float *A, int lda, Float alpha,Float beta);
    void                HalfSpTileMatInsertRight(Float *destrow,int i, int j, const Float *x);
    void                calcJinvMJT(const MeReal *JM,const MeReal *j,                            /* Input */
                                    const int *jlen,                                             /* Input */
                                    const int *jbltobody,                                        /* Input */
                                    const MeReal *slipfactor,const MdtKeaParameters parameters); /* Input */

    void                factorize();
    void                multiply(const MeReal *X, MeReal *B, int m);
    void                solve(MeReal *B, int m);
    void                solveUnit(MeReal *B, int posn);
    void                keaFactorQ(MeReal *A, int n);
    void                keaSolveQ(MeReal *A, MeReal *x, int n);

    MeReal              getEpsilon();

};

typedef struct SpTileMat
{
  Float *a;     // array of data: this is nxn
  int   *rlist;           // n arrays or n column indices (right lists)
  int   *llist;           // n arrays of n column indices (left lists)
  int   *heads;           // n arrays for heads of right and left lists
  int   *rlist_len;             // rlist_len[i]=length of rlist[i]
  int   *llist_len;             // llist_len[i]=length of llist[i]
  int    n;     // current number of rows and cols in the array
  int   ld;     // leading dimension of the array
  int bk;     // block size
  int bksz;     // total size i.e., bk*bk;
  int bk1;      // size of nested block
  int bksz1;      // total size of nested block
  int ld1;      // leading dimension for nested blocks
        // within block
}
SpTileMat;

typedef struct {
int sendcholeskytime;
int choleskytime;
int solvetime;
int inserttime;
int pairtime;
int pairtime2;
int headtime;
int headtime2;
int sendsgemmtime;
int sendssyrktime;
int sendstrsmtime;
int totaltime;
int microtime;
int storediagtime;
int storetime;
int firstrowtime;
int runfirstrowtime;
int sendtosprtime;
int alloctime;
} stats;

#define STARTCOUNT(y) y=scePcGetCounter0();
#define STOPCOUNT(x,y) x+=scePcGetCounter0()-y;

void      microcodeSpTileMatCholesky(SpTileMat *A,stats *stats);
void      AssignWallpaperToSpTileMat(int n, SpTileMat *A, Float *B, int ldb);
void      AssignWallpaperToHalfSpTileMatRight(int n, SpTileMat *A, Float *B, int ldb);
void      SpTileMatCholesky(SpTileMat *A);
void      SpTileMatMult(SpTileMat *A, const Float *x, Float *y, int factors);
void      SpTileMatSolve(SpTileMat *A, Float *b);
SpTileMat *newSpTileMat(int n, int bk, int bk1);
void      deleteSpTileMat(SpTileMat *M);

void    make_first_row_vif_prog(float *Ajj,int len_xs,float **xs);
void    execute_vif_chain();
void    send_microcode_ssyrk_chol_sgemm_strsm_to_scratchpad();
void    send_microcode_1212ssyrk();
void    send_microcode_1212sgemm_strsm();
void    send_microcode_1212chol();

void    run_microcode_1212ssyrk();
void    run_microcode_1212sgemm();
void    run_microcode_1212strsm();
void    run_microcode_1212chol();

void    fold_1212ssyrk(float** xs, int len_xs,float* S);
void    make_fold_1212sgemm_thenstrsm_vifchain(float** xs, int len_xs,float* S);

void HalfSpTileMatInsertRight(Float *destrow,                             /* Output */
                              SpTileMat *A,int i, int j, const Float *x); /* Input  */

void AssignHalfSpTileMatLeftToStd(int n, Float *A, int lda, Float alpha,
                SpTileMat *sourcematrix, Float beta);

#endif /* _KEASPARSEMATRIX_PS2_HPP */
