#ifndef _CALCA_HPP
#define _CALCA_HPP
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.10.6.1 $

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

enum _shapes
{
    SQUARE,
    TRIANGLE,
};

void calc_block(float* buf);

/* nstrips is the number of J strips (or JM strips, for they are the same), i.e.
 * it isn't the _total_ number. For a 32x32 matrix it's 8. */
void calc_whole(float* buf,int nstrips); // Always makes triangular block

// Result block has to be long enough to contain 78 quadwords
//
// Small dense case - dma tags are stored inbetween data:
//
// If a diagonal block, result looks like this:
//
// result[0..4)     dmatag      ( 1 quad)  0   copy of A for Achol
// result[4..20)    rowblock 0  ( 4 quads) 1
// result[20..24)   dmatag      ( 1 quad)  5
// result[24..56)   rowblock 1  ( 8 quads) 6
// result[56..60)   dmatag      ( 1 quad)  14
// result[60..108)  rowblock 2  (12 quads) 15
// result[108..112) dmatag      ( 1 quad)  27
// result[112..128) rowblock 0  ( 4 quads) 31     A
// result[128..132) dmatag      ( 1 quad)  32
// result[132..164) rowblock 1  ( 8 quads) 33
// result[164..168) dmatag      ( 1 quad)  41
// result[168..216) rowblock 2  (12 quads) 42
// result[216..312) unused      (24 quads) 54
//
// Total used space is 54 quadwords
//
// If a non diagonal block,result looks like this:
//
// result[0..4)     dmatag      ( 1 quad)  0    copy of A for Achol
// result[4..52)    rowblock 0  (12 quads) 1
// result[52..56)   dmatag      ( 1 quad)  13
// result[56..104)  rowblock 1  (12 quads) 14
// result[104..108) dmatag      ( 1 quad)  26
// result[108..156) rowblock 2  (12 quads) 27
// result[156..160) dmatag      ( 1 quad)  39    A
// result[160..208) rowblock 0  (12 quads) 40
// result[208..212) dmatag      ( 1 quad)  52
// result[212..260) rowblock 1  (12 quads) 53
// result[260..264) dmatag      ( 1 quad)  65
// result[264..312) rowblock 2  (12 quads) 66
//
// Total space for result is 78 quadwords
//--------------------------------------------------
// Sparse case, destination chain with 2 tags
//
// result[0..4)     dmatag             ( 1 quad)   0
// result[4..148)   copy of A for Achol ( 36 quads) 1
// result[148..152) dmatag             ( 1 quad)   37
// result[152..196) A                  ( 36 quads) 38
//
// Total space for result is 74 quadwords
//
#define BUFFERADDR_SMALL_DIAG_ACOPY    108
#define BUFFERADDR_SMALL_NONDIAG_ACOPY 156
#define BUFFERADDR_SPARSE_ACOPY        148

#define BLOCK_RESULT_LEN (4*78)
#define BLOCK_JM_LEN (3*8*6*4)
#define BLOCK_J_LEN (3*8*6*4)
#define BLOCK_JM_BL2BODY_LEN (8*3)
#define BLOCK_J_BL2BODY_LEN (8*3)
#define BLOCK_JM_LEN_LEN (3)
#define BLOCK_J_LEN_LEN (3)

// This is all the data we need to work out a 12x12 block of A.
typedef struct _calc_block_buf
{
    //*** Output

    // 12x12 result block
    float result[BLOCK_RESULT_LEN];

    // Result flags quadword.This is used for the 'iszero flag'

    int isnonzero;
    int padding[3];

    //*** Input

    // Input flags quadwords

    int    shape;
    int    gap_for_tag;
    float  epsilon;
    float  hinv;

    int    modify_diag;
    int    padding2[3];

    float  slipfactor[12];

    // Lengths of JM strips
    int jm_len[BLOCK_JM_LEN_LEN];
    int pad0;

    // Lengths of J strips
    int j_len[BLOCK_J_LEN_LEN];
    int pad1;

    // Now the JM block2body table
    int jm_bl2body[BLOCK_JM_BL2BODY_LEN];

    // Now the J block2body table
    int j_bl2body[BLOCK_J_BL2BODY_LEN];

    // Room for 3 JM strips and 3 J strips
    // (worst case a JM strip is 8 6x4 blocks)
    float jmandj[BLOCK_JM_LEN*2];

} Calc_block_buf;

#define WHOLE_RESULT_LEN (144) // half-wallpapered
#define WHOLE_JM_LEN (8*8*6*4)
#define WHOLE_J_LEN (8*8*6*4)
#define WHOLE_JM_BL2BODY_LEN (8*8)
#define WHOLE_J_BL2BODY_LEN (8*8)
#define WHOLE_JM_LEN_LEN (8)
#define WHOLE_J_LEN_LEN (8)

// We have to make sure that 2 buffers fit in the scratchpad
// BUFFERLENGTH_BYTES = ((78+1+2+3+1+1+6+6+288)*16) = 6176
// 6176 * 2 = 12352, which is less than 16384, so we are ok

#define BUFFERADDR_RESULT        ((0)*16)
#define BUFFERADDR_OUTPUTFLAGS   ((78)*16)
#define BUFFERADDR_INPUTFLAGS    ((78+1)*16)
#define BUFFERADDR_SLIPFACTOR    ((78+1+2)*16)
#define BUFFERADDR_JMLEN         ((78+1+2+3)*16)
#define BUFFERADDR_JLEN          ((78+1+2+3+1)*16)
#define BUFFERADDR_INPUT         ((78+1+2+3+1+1)*16)              // offset of input from start of SPR in bytes
#define BUFFERADDR_JMBLOCKTOBODY ((78+1+2+3+1+1)*16)
#define BUFFERADDR_JBLOCKTOBODY  ((78+1+2+3+1+1+6)*16)
#define BUFFERADDR_JMANDJ        ((78+1+2+3+1+1+6+6)*16)
#define BUFFERLENGTH_BYTES       ((78+1+2+3+1+1+6+6+288)*16)
#define BUFFERLENGTH_WORDS       ((78+1+2+3+1+1+6+6+288)*4)

// This is all the data we need to work out a 32x32 half-wallpapered A
typedef struct _calc_whole_buf
{
    // 12x12 result block
    float result[WHOLE_RESULT_LEN];

    // Now the JM block2body table
    int jm_bl2body[WHOLE_JM_BL2BODY_LEN];

    // Now the J block2body table
    int j_bl2body[WHOLE_J_BL2BODY_LEN];

    // Lengths of JM strips
    int jm_len[WHOLE_JM_LEN_LEN];

    // Lengths of J strips
    int j_len[WHOLE_J_LEN_LEN];

    // Room for 3 JM strips
    // (worst case a JM strip is 8 6x4 blocks)
    float jmandj[WHOLE_JM_LEN*2];

} Calc_whole_buf;

typedef struct {
    int len[3];
    int qwc;
    int blocktobody[3*8];
} blocktobodyandlen;

#endif // _CALCA_HPP
