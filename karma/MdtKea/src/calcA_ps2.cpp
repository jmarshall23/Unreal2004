/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.13.2.2 $

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

#include <libpc.h>
#include <eeregs.h>
#include <MeAssert.h>
#include <stdio.h>
#include "calcA.hpp"
#include "keaDebug.h"


#define BLOCK_NUMSTRIPS (3)

#define DGET_T0_COUNT()         (*T0_COUNT)

/* calc_block
 * ----------
 *
 * calculate a 12*12 block of JinvMJT
 *
 * gap_for_tag - gap to insert at the end of each destination column (in words)
 *               set to 0 if not using destination chain mode
 *               set to 4 if using destination chain mode
*/

#undef TIME_CALCABLOCK

//#define GETTIME() (scePcGetCounter0())
#define GETTIME() (scePcGetCounter1())

void calc_block(float* buf)
{
#ifdef TIME_CALCABLOCK
    int *time  = (int *)(0x70000000+BUFFERLENGTH_BYTES*2);
    int *ptime = time;

    *(ptime++) = 0;
    *(ptime++) = GETTIME(); (*time)++;
#endif

    Calc_block_buf* b=(Calc_block_buf*)buf;

    int shape       = b->shape;
    int gap_for_tag = b->gap_for_tag;

    // The first result quadword is always a tag
    // So the actual result always starts 4 words in

    float* ai     = b->result + 4;
    float* aicopy;

    if(gap_for_tag!=0) // if we are doing a small dense matrix
    {
        if(b->shape==TRIANGLE)
            aicopy = b->result + BUFFERADDR_SMALL_DIAG_ACOPY + gap_for_tag;
        else
            aicopy = b->result + BUFFERADDR_SMALL_NONDIAG_ACOPY + gap_for_tag;
    }
    else
    {
        aicopy = b->result + BUFFERADDR_SPARSE_ACOPY + 4;
    }

    float* jbase = b->jmandj + 24*(b->jm_len[0]+b->jm_len[1]+b->jm_len[2]);

    //const float* jbase=b->j;
    int* j_bl2body=b->j_bl2body;

    int* jsli=b->j_len;

#if PRINT_JMINVJTBLOCK_INPUT
    printJinvMJTBlockInput(b);
#endif

    __asm__ __volatile__("
    __expression_asm

    a0
    a1
    a2
    a3
    a4
    a5
    a6

    b0
    b1
    b2
    b3
    b4
    b5
    b6

    r0
    r1
    r2
    r3
    r4

    __end_expression_asm
    ");

    int foundmatch = 0;

    // J strips
    for(int stripi=0;stripi<BLOCK_NUMSTRIPS;stripi++){

        /* We'll do square blocks for now (for triangles,
         * loop up to stripi instead of BLOCK_NUMSTRIPS) */
        const float* jmi=b->jmandj;
        int* jm_bl2body=b->jm_bl2body;

        // JM strips

        int stripjend=(shape==SQUARE) ? BLOCK_NUMSTRIPS : stripi+1;

        int j_bl2body0=j_bl2body[0];
        int j_bl2body1=j_bl2body[1];
        int j_bl2body2=j_bl2body[2];
        int j_bl2body3=j_bl2body[3];
        int j_bl2body4=j_bl2body[4];
        int j_bl2body5=j_bl2body[5];
        int j_bl2body6=j_bl2body[6];
        int j_bl2body7=j_bl2body[7];

        //printf("j_bl2body0=%d j_bl2body1=%d j_bl2body2=%d j_bl2body3=%d\n",j_bl2body0,j_bl2body1,j_bl2body2,j_bl2body3);

        int* jmsli=b->jm_len;

        for(int stripj=0;stripj<stripjend;stripj++,jmsli++)
        {
#ifdef TIME_CALCABLOCK
            *(ptime++) = GETTIME(); (*time)++;
#endif
            /* Combine two strips to make a 4x4 block
             * --------------------------------------
             * On entry:
             *
             * j_bl2body0 -
             * j_bl2body1 -
             * j_bl2body2 -
             * j_bl2body3 -
             * j_bl2body4 -
             * j_bl2body5 -
             * j_bl2body6 -
             * j_bl2body7 -
             * jm_bl2body - pointer to bl2body entry for this jm strip
             * jmsli      - length of the jm strip
             * jmi        - pointer to jm strip
            */
            int* bodyi=jm_bl2body;

            __asm__ __volatile__("
            __expression_asm

            r0 = K - K
            r1 = K - K
            r2 = K - K
            r3 = K - K

            __end_expression_asm
            ");

            // Combine two strips, loop up to the number of blocks in the strip
            int iend=*jmsli;

            for(int i=0;i<iend;i++){

                // Load a block from JM
                __asm__ __volatile__("
                __expression_asm

                'lqc2  @a0, 0x0(%0)
                'lqc2  @a1, 0x10(%0)
                'lqc2  @a2, 0x20(%0)
                'lqc2  @a3, 0x30(%0)
                'lqc2  @a4, 0x40(%0)
                'lqc2  @a5, 0x50(%0)

                __end_expression_asm
                " : : "r" (jmi));

                jmi+=4*6;


                /* Start of inner loop.
                 * This loop loops over the up to 4 possible J blocks that get combined
                 * with each JM block.
                 * We unroll x8 to save looping over columns of block2body tables
                 */

                int body=*bodyi++;

                /* Now we need to search for J blocks corresponding to that
                 * body, combining them as we find them. For the strip we're on */

                int jbi=0;
                const float* ji;

#if 0
                for(int ii=0;ii!=8;ii++)
                {
                    if(j_bl2body[ii]==body)
                    {
                        foundmatch = 1;

                        __asm__ __volatile__("
                        __expression_asm

                        'addu  %0, %4, %3 # ji=jbase+jbi

                        'lqc2  @b0, 0x0(%2)
                        'lqc2  @b1, 0x10(%2)
                        'lqc2  @b2, 0x20(%2)
                        'lqc2  @b3, 0x30(%2)
                        'lqc2  @b4, 0x40(%2)
                        'lqc2  @b5, 0x50(%2)

                        ACC = r0 + K.x

                        'addu  %0, %0, 96

                        ACC = ACC + b0 * a0.x
                        ACC = ACC + b1 * a1.x
                        ACC = ACC + b2 * a2.x
                        ACC = ACC + b3 * a3.x
                        ACC = ACC + b4 * a4.x
                        r0 = ACC + b5 * a5.x

                        ACC = r1 + K.x

                        ACC = ACC + b0 * a0.y
                        ACC = ACC + b1 * a1.y
                        ACC = ACC + b2 * a2.y
                        ACC = ACC + b3 * a3.y
                        ACC = ACC + b4 * a4.y
                        r1 = ACC + b5 * a5.y

                        ACC = r2 + K.x

                        ACC = ACC + b0 * a0.z
                        ACC = ACC + b1 * a1.z
                        ACC = ACC + b2 * a2.z
                        ACC = ACC + b3 * a3.z
                        ACC = ACC + b4 * a4.z
                        r2 = ACC + b5 * a5.z

                        ACC = r3 + K.x

                        ACC = ACC + b0 * a0.w
                        ACC = ACC + b1 * a1.w
                        ACC = ACC + b2 * a2.w
                        ACC = ACC + b3 * a3.w
                        ACC = ACC + b4 * a4.w
                        r3 = ACC + b5 * a5.w

                        __end_expression_asm
                        " : "=r" (ji), "=r" (jbi) : "0" (ji), "1" (jbi), "r" (jbase));
                    }
                    jbi+=96;
                }
#else
                // UNROLL 0

                if(j_bl2body0==body)
                {
                foundmatch = 1;

                __asm__ __volatile__("
                __expression_asm

                'addu  %0, %4, %3 # ji=jbase+jbi

                'lqc2  @b0, 0x0(%2)
                'lqc2  @b1, 0x10(%2)
                'lqc2  @b2, 0x20(%2)
                'lqc2  @b3, 0x30(%2)
                'lqc2  @b4, 0x40(%2)
                'lqc2  @b5, 0x50(%2)

                ACC = r0 + K.x

                'addu  %0, %0, 96

                ACC = ACC + b0 * a0.x
                ACC = ACC + b1 * a1.x
                ACC = ACC + b2 * a2.x
                ACC = ACC + b3 * a3.x
                ACC = ACC + b4 * a4.x
                r0 = ACC + b5 * a5.x

                ACC = r1 + K.x

                ACC = ACC + b0 * a0.y
                ACC = ACC + b1 * a1.y
                ACC = ACC + b2 * a2.y
                ACC = ACC + b3 * a3.y
                ACC = ACC + b4 * a4.y
                r1 = ACC + b5 * a5.y

                ACC = r2 + K.x

                ACC = ACC + b0 * a0.z
                ACC = ACC + b1 * a1.z
                ACC = ACC + b2 * a2.z
                ACC = ACC + b3 * a3.z
                ACC = ACC + b4 * a4.z
                r2 = ACC + b5 * a5.z

                ACC = r3 + K.x

                ACC = ACC + b0 * a0.w
                ACC = ACC + b1 * a1.w
                ACC = ACC + b2 * a2.w
                ACC = ACC + b3 * a3.w
                ACC = ACC + b4 * a4.w
                r3 = ACC + b5 * a5.w

                __end_expression_asm
                " : "=r" (ji), "=r" (jbi) : "0" (ji), "1" (jbi), "r" (jbase));
                }
                // UNROLL 1

                jbi+=96;
                if(j_bl2body1==body)
                {
                //printf("combine with 1\n");
                foundmatch = 1;

                __asm__ __volatile__("
                __expression_asm

                'addu  %0, %4, %3 # ji=jbase+jbi

                'lqc2  @b0, 0x0(%2)
                'lqc2  @b1, 0x10(%2)
                'lqc2  @b2, 0x20(%2)
                'lqc2  @b3, 0x30(%2)
                'lqc2  @b4, 0x40(%2)
                'lqc2  @b5, 0x50(%2)

                ACC = r0 + K.x

                'addu  %0, %0, 96

                ACC = ACC + b0 * a0.x
                ACC = ACC + b1 * a1.x
                ACC = ACC + b2 * a2.x
                ACC = ACC + b3 * a3.x
                ACC = ACC + b4 * a4.x
                r0 = ACC + b5 * a5.x

                ACC = r1 + K.x

                ACC = ACC + b0 * a0.y
                ACC = ACC + b1 * a1.y
                ACC = ACC + b2 * a2.y
                ACC = ACC + b3 * a3.y
                ACC = ACC + b4 * a4.y
                r1 = ACC + b5 * a5.y

                ACC = r2 + K.x

                ACC = ACC + b0 * a0.z
                ACC = ACC + b1 * a1.z
                ACC = ACC + b2 * a2.z
                ACC = ACC + b3 * a3.z
                ACC = ACC + b4 * a4.z
                r2 = ACC + b5 * a5.z

                ACC = r3 + K.x

                ACC = ACC + b0 * a0.w
                ACC = ACC + b1 * a1.w
                ACC = ACC + b2 * a2.w
                ACC = ACC + b3 * a3.w
                ACC = ACC + b4 * a4.w
                r3 = ACC + b5 * a5.w

                __end_expression_asm
                " : "=r" (ji), "=r" (jbi) : "0" (ji), "1" (jbi), "r" (jbase) : "$8");
                }
                // UNROLL 2

                jbi+=96;
                if(j_bl2body2==body)
                {
                //printf("combine with 2\n");
                foundmatch = 1;

                __asm__ __volatile__("
                __expression_asm

                'addu  %0, %4, %3 # ji=jbase+jbi

                'lqc2  @b0, 0x0(%2)
                'lqc2  @b1, 0x10(%2)
                'lqc2  @b2, 0x20(%2)
                'lqc2  @b3, 0x30(%2)
                'lqc2  @b4, 0x40(%2)
                'lqc2  @b5, 0x50(%2)

                ACC = r0 + K.x

                'addu  %0, %0, 96

                ACC = ACC + b0 * a0.x
                ACC = ACC + b1 * a1.x
                ACC = ACC + b2 * a2.x
                ACC = ACC + b3 * a3.x
                ACC = ACC + b4 * a4.x
                r0 = ACC + b5 * a5.x

                ACC = r1 + K.x

                ACC = ACC + b0 * a0.y
                ACC = ACC + b1 * a1.y
                ACC = ACC + b2 * a2.y
                ACC = ACC + b3 * a3.y
                ACC = ACC + b4 * a4.y
                r1 = ACC + b5 * a5.y

                ACC = r2 + K.x

                ACC = ACC + b0 * a0.z
                ACC = ACC + b1 * a1.z
                ACC = ACC + b2 * a2.z
                ACC = ACC + b3 * a3.z
                ACC = ACC + b4 * a4.z
                r2 = ACC + b5 * a5.z

                ACC = r3 + K.x

                ACC = ACC + b0 * a0.w
                ACC = ACC + b1 * a1.w
                ACC = ACC + b2 * a2.w
                ACC = ACC + b3 * a3.w
                ACC = ACC + b4 * a4.w
                r3 = ACC + b5 * a5.w

                __end_expression_asm
                " : "=r" (ji), "=r" (jbi) : "0" (ji), "1" (jbi), "r" (jbase));
                }

                // UNROLL 3
                jbi+=96;
                if(j_bl2body3==body)
                {
                //printf("combine with 3\n");
                foundmatch = 1;

                __asm__ __volatile__("
                __expression_asm

                'addu  %0, %4, %3 # ji=jbase+jbi

                'lqc2  @b0, 0x0(%2)
                'lqc2  @b1, 0x10(%2)
                'lqc2  @b2, 0x20(%2)
                'lqc2  @b3, 0x30(%2)
                'lqc2  @b4, 0x40(%2)
                'lqc2  @b5, 0x50(%2)

                ACC = r0 + K.x

                'addu  %0, %0, 96

                ACC = ACC + b0 * a0.x
                ACC = ACC + b1 * a1.x
                ACC = ACC + b2 * a2.x
                ACC = ACC + b3 * a3.x
                ACC = ACC + b4 * a4.x
                r0 = ACC + b5 * a5.x

                ACC = r1 + K.x

                ACC = ACC + b0 * a0.y
                ACC = ACC + b1 * a1.y
                ACC = ACC + b2 * a2.y
                ACC = ACC + b3 * a3.y
                ACC = ACC + b4 * a4.y
                r1 = ACC + b5 * a5.y

                ACC = r2 + K.x

                ACC = ACC + b0 * a0.z
                ACC = ACC + b1 * a1.z
                ACC = ACC + b2 * a2.z
                ACC = ACC + b3 * a3.z
                ACC = ACC + b4 * a4.z
                r2 = ACC + b5 * a5.z

                ACC = r3 + K.x

                ACC = ACC + b0 * a0.w
                ACC = ACC + b1 * a1.w
                ACC = ACC + b2 * a2.w
                ACC = ACC + b3 * a3.w
                ACC = ACC + b4 * a4.w
                r3 = ACC + b5 * a5.w

                __end_expression_asm
                " : "=r" (ji), "=r" (jbi) : "0" (ji), "1" (jbi), "r" (jbase));
                }

                // UNROLL 4
                jbi+=96;
                if(j_bl2body4==body)
                {
                //printf("combine with 4\n");
                foundmatch = 1;

                __asm__ __volatile__("
                __expression_asm

                'addu  %0, %4, %3 # ji=jbase+jbi

                'lqc2  @b0, 0x0(%2)
                'lqc2  @b1, 0x10(%2)
                'lqc2  @b2, 0x20(%2)
                'lqc2  @b3, 0x30(%2)
                'lqc2  @b4, 0x40(%2)
                'lqc2  @b5, 0x50(%2)

                ACC = r0 + K.x

                'addu  %0, %0, 96

                ACC = ACC + b0 * a0.x
                ACC = ACC + b1 * a1.x
                ACC = ACC + b2 * a2.x
                ACC = ACC + b3 * a3.x
                ACC = ACC + b4 * a4.x
                r0 = ACC + b5 * a5.x

                ACC = r1 + K.x

                ACC = ACC + b0 * a0.y
                ACC = ACC + b1 * a1.y
                ACC = ACC + b2 * a2.y
                ACC = ACC + b3 * a3.y
                ACC = ACC + b4 * a4.y
                r1 = ACC + b5 * a5.y

                ACC = r2 + K.x

                ACC = ACC + b0 * a0.z
                ACC = ACC + b1 * a1.z
                ACC = ACC + b2 * a2.z
                ACC = ACC + b3 * a3.z
                ACC = ACC + b4 * a4.z
                r2 = ACC + b5 * a5.z

                ACC = r3 + K.x

                ACC = ACC + b0 * a0.w
                ACC = ACC + b1 * a1.w
                ACC = ACC + b2 * a2.w
                ACC = ACC + b3 * a3.w
                ACC = ACC + b4 * a4.w
                r3 = ACC + b5 * a5.w

                __end_expression_asm
                " : "=r" (ji), "=r" (jbi) : "0" (ji), "1" (jbi), "r" (jbase));
                }

                // UNROLL 5
                jbi+=96;
                if(j_bl2body5==body)
                {
                //printf("combine with 5\n");
                foundmatch = 1;

                __asm__ __volatile__("
                __expression_asm

                'addu  %0, %4, %3 # ji=jbase+jbi

                'lqc2  @b0, 0x0(%2)
                'lqc2  @b1, 0x10(%2)
                'lqc2  @b2, 0x20(%2)
                'lqc2  @b3, 0x30(%2)
                'lqc2  @b4, 0x40(%2)
                'lqc2  @b5, 0x50(%2)

                ACC = r0 + K.x

                'addu  %0, %0, 96

                ACC = ACC + b0 * a0.x
                ACC = ACC + b1 * a1.x
                ACC = ACC + b2 * a2.x
                ACC = ACC + b3 * a3.x
                ACC = ACC + b4 * a4.x
                r0 = ACC + b5 * a5.x

                ACC = r1 + K.x

                ACC = ACC + b0 * a0.y
                ACC = ACC + b1 * a1.y
                ACC = ACC + b2 * a2.y
                ACC = ACC + b3 * a3.y
                ACC = ACC + b4 * a4.y
                r1 = ACC + b5 * a5.y

                ACC = r2 + K.x

                ACC = ACC + b0 * a0.z
                ACC = ACC + b1 * a1.z
                ACC = ACC + b2 * a2.z
                ACC = ACC + b3 * a3.z
                ACC = ACC + b4 * a4.z
                r2 = ACC + b5 * a5.z

                ACC = r3 + K.x

                ACC = ACC + b0 * a0.w
                ACC = ACC + b1 * a1.w
                ACC = ACC + b2 * a2.w
                ACC = ACC + b3 * a3.w
                ACC = ACC + b4 * a4.w
                r3 = ACC + b5 * a5.w

                __end_expression_asm
                " : "=r" (ji), "=r" (jbi) : "0" (ji), "1" (jbi), "r" (jbase));
                }
                // UNROLL 6
                jbi+=96;
                if(j_bl2body6==body)
                {
                //printf("combine with 6\n");
                foundmatch = 1;

                __asm__ __volatile__("
                __expression_asm

                'addu  %0, %4, %3 # ji=jbase+jbi

                'lqc2  @b0, 0x0(%2)
                'lqc2  @b1, 0x10(%2)
                'lqc2  @b2, 0x20(%2)
                'lqc2  @b3, 0x30(%2)
                'lqc2  @b4, 0x40(%2)
                'lqc2  @b5, 0x50(%2)

                ACC = r0 + K.x

                'addu  %0, %0, 96

                ACC = ACC + b0 * a0.x
                ACC = ACC + b1 * a1.x
                ACC = ACC + b2 * a2.x
                ACC = ACC + b3 * a3.x
                ACC = ACC + b4 * a4.x
                r0 = ACC + b5 * a5.x

                ACC = r1 + K.x

                ACC = ACC + b0 * a0.y
                ACC = ACC + b1 * a1.y
                ACC = ACC + b2 * a2.y
                ACC = ACC + b3 * a3.y
                ACC = ACC + b4 * a4.y
                r1 = ACC + b5 * a5.y

                ACC = r2 + K.x

                ACC = ACC + b0 * a0.z
                ACC = ACC + b1 * a1.z
                ACC = ACC + b2 * a2.z
                ACC = ACC + b3 * a3.z
                ACC = ACC + b4 * a4.z
                r2 = ACC + b5 * a5.z

                ACC = r3 + K.x

                ACC = ACC + b0 * a0.w
                ACC = ACC + b1 * a1.w
                ACC = ACC + b2 * a2.w
                ACC = ACC + b3 * a3.w
                ACC = ACC + b4 * a4.w
                r3 = ACC + b5 * a5.w

                __end_expression_asm
                " : "=r" (ji), "=r" (jbi) : "0" (ji), "1" (jbi), "r" (jbase));
                }

                // UNROLL 7
                jbi+=96;
                if(j_bl2body7==body)
                {
                //printf("combine with 7\n");
                foundmatch = 1;

                __asm__ __volatile__("
                __expression_asm

                'addu  %0, %4, %3 # ji=jbase+jbi

                'lqc2  @b0, 0x0(%2)
                'lqc2  @b1, 0x10(%2)
                'lqc2  @b2, 0x20(%2)
                'lqc2  @b3, 0x30(%2)
                'lqc2  @b4, 0x40(%2)
                'lqc2  @b5, 0x50(%2)

                ACC = r0 + K.x

                'addu  %0, %0, 96

                ACC = ACC + b0 * a0.x
                ACC = ACC + b1 * a1.x
                ACC = ACC + b2 * a2.x
                ACC = ACC + b3 * a3.x
                ACC = ACC + b4 * a4.x
                r0 = ACC + b5 * a5.x

                ACC = r1 + K.x

                ACC = ACC + b0 * a0.y
                ACC = ACC + b1 * a1.y
                ACC = ACC + b2 * a2.y
                ACC = ACC + b3 * a3.y
                ACC = ACC + b4 * a4.y
                r1 = ACC + b5 * a5.y

                ACC = r2 + K.x

                ACC = ACC + b0 * a0.z
                ACC = ACC + b1 * a1.z
                ACC = ACC + b2 * a2.z
                ACC = ACC + b3 * a3.z
                ACC = ACC + b4 * a4.z
                r2 = ACC + b5 * a5.z

                ACC = r3 + K.x

                ACC = ACC + b0 * a0.w
                ACC = ACC + b1 * a1.w
                ACC = ACC + b2 * a2.w
                ACC = ACC + b3 * a3.w
                ACC = ACC + b4 * a4.w
                r3 = ACC + b5 * a5.w

'.set reorder
           ' unroll_18:

                __end_expression_asm
                " : "=r" (ji), "=r" (jbi) : "0" (ji), "1" (jbi), "r" (jbase));
                }
#endif
            }

            __asm__ __volatile__("
            __expression_asm

            'sqc2  @r0, 0x0(%0)
            'sqc2  @r1, 0x10(%0)
            'sqc2  @r2, 0x20(%0)
            'sqc2  @r3, 0x30(%0)

            __end_expression_asm
            " : : "r" (ai));

            //** If this is a diagonal block, modify the diagonal

            if( (stripj==stripi)&&(b->modify_diag!=0) )
            {
                 ai[0*5]+=b->hinv*b->slipfactor[stripj*4+0]+b->epsilon;
                 ai[1*5]+=b->hinv*b->slipfactor[stripj*4+1]+b->epsilon;
                 ai[2*5]+=b->hinv*b->slipfactor[stripj*4+2]+b->epsilon;
                 ai[3*5]+=b->hinv*b->slipfactor[stripj*4+3]+b->epsilon;
            }

            __asm__ __volatile__("
            __expression_asm
            tmp1
            tmp2
            tmp3
            tmp4

            'lqc2 @tmp1,0x00(%1)
            'lqc2 @tmp2,0x10(%1)
            'lqc2 @tmp3,0x20(%1)
            'lqc2 @tmp4,0x30(%1)

            'sqc2 @tmp1,0x00(%0)
            'sqc2 @tmp2,0x10(%0)
            'sqc2 @tmp3,0x20(%0)
            'sqc2 @tmp4,0x30(%0)

            ~tmp1
            ~tmp2
            ~tmp3
            ~tmp4
            __end_expression_asm
            " : : "r" (&aicopy[0]) , "r" (&ai[0]));

            ai+=4*4;
            aicopy+=4*4;

            jm_bl2body+=8;
#ifdef TIME_CALCABLOCK
            *(ptime++) = GETTIME(); (*time)++;
#endif

        } // JM strips

        ai     = ai + gap_for_tag;
        aicopy = aicopy + gap_for_tag;

        jbase+=*jsli++*6*4;

        j_bl2body+=8;

    } // J strips

    b->isnonzero = foundmatch;
#ifdef TIME_CALCABLOCK
    *(ptime++) = GETTIME(); (*time)++;
#endif

#if PRINT_JMINVJTBLOCK_OUTPUT
    printJinvMJTBlockOutput(b,shape,foundmatch);
#endif

    __asm__ __volatile__("
    __expression_asm

    ~a0
    ~a1
    ~a2
    ~a3
    ~a4
    ~a5
    ~a6

    ~b0
    ~b1
    ~b2
    ~b3
    ~b4
    ~b5
    ~b6

    ~r0
    ~r1
    ~r2
    ~r3
    ~r4

    __end_expression_asm
    ");
}
