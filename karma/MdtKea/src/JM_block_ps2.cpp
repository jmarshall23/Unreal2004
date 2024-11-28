/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.13.2.1 $

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

#include <stdio.h>
#include "JM_block.hpp"
#include "MdtKea.h"
#include "MeAssert.h"
#include "keaDebug.h"
#include <keaPrintBasicTypes.h>

MeReal test2[4] __attribute__ ((aligned(64)));

/* jm_block
 * --------
 *
 * On Entry:
 * 
 * buf->parameters.num_blocks -
 * buf->parameters.num_strips -
 * buf->j_bl2body             - 3*8 j_bl2body table
 * buf->gamma                 -
 * buf->c                     -
 *
 * buf->rhs_tag               -
 * buf->xi_rhs                -
 * buf->jm_tag                -
 *   
 * buf->j_m_and_vhmf[0..num_blocks*24) - array of 'num_blocks' j blocks
 * buf->j_m_and_vhmf[num_blocks*24..)  - array of m_and_vhmf blocks
*/
void jm_block(int   calc_buf, /* Input / Output */
              float invh,     /* Input */
              float invhh,    /* Input */
              float gamma)    /* Input */
{
    newCalc_JM_buf *buf = ((newCalc_JM_buf *)0x70000000)+calc_buf;

//    printf("jmblock sizeof=%d\n",sizeof(Calc_JM_buf));

    int         num_jblocks = buf->parameters.num_blocks;
    int         num_strips  = buf->parameters.num_strips;
    float       *j_block    = buf->j_m_and_vhmf;
    const int   *j_bl2body  = buf->j_bl2body;
    const float *c          = buf->c;
    const float *xgamma     = buf->gamma;
    float       *xi         = buf->xi_rhs;
    m_and_vhmf  *m_vhmf     = (m_and_vhmf *)(buf->j_m_and_vhmf + num_jblocks*24);

//    printf("newcalc buf %d num_strips=%d\n",calc_buf,num_strips);

#if PRINT_JMINVANDRHSBLOCK_INPUT
    printJinvMandrhsBlockInput(buf);
#endif

#if 0
    if(frame==45)
    {
        printBL2BODY(j_bl2body,num_strips);
    }
#endif
    __asm__ __volatile__("
    __expression_asm

    m0
    m1
    m2
    j0
    j1
    j2
    j3
    j4
    j5
    jm3
    jm4
    jm5
    v0
    v1
    jv

    __end_expression_asm
    ");

    for(int i=0;i<num_strips;i++)
    {
//        printf("strip %d\n",i);

        __asm__ __volatile__("
        __expression_asm
        jv = K - K
        __end_expression_asm
        ");

        int j=0;
        while(j!=MAXBLOCKS && j_bl2body[j]!=-3 )
        {
            MEASSERT(j_bl2body+j<buf->j_bl2body+J_BL2BODY_LEN);

            if(j_bl2body[j]>=0)
            {
                const MdtKeaInverseMassMatrix *m = &m_vhmf->m;
                const float                   *v = m_vhmf->vhmf;

                //printf("Jv input\n");
                //printM(m);
                //printVec4(v,"V");
                //printJ(j_block);

                __asm__ __volatile__("
                __expression_asm

                'lqc2  @m0, 0x0(%1)
                'lqc2  @m1, 0x10(%1)
                'lqc2  @m2, 0x20(%1)

                'lqc2  @j0, 0x0(%0)
                'lqc2  @j1, 0x10(%0)
                'lqc2  @j2, 0x20(%0)
                'lqc2  @j3, 0x30(%0)
                'lqc2  @v0, 0x0(%2)
                'lqc2  @v1, 0x10(%2)
                'lqc2  @j4, 0x40(%0)
                'lqc2  @j5, 0x50(%0)

                ACC = jv + K.x

                ACC = ACC + j0 * v0.x
                ACC = ACC + j1 * v0.y
                ACC = ACC + j2 * v0.z

                ACC = ACC + j3 * v1.x
                ACC = ACC + j4 * v1.y
                jv  = ACC + j5 * v1.z

                j0 = j0 * m0.w
                j1 = j1 * m0.w
                j2 = j2 * m0.w

                ACC = j3 * m0.x
                ACC = ACC + j4 * m1.x
                jm3 = ACC + j5 * m2.x

                ACC = j3 * m0.y
                ACC = ACC + j4 * m1.y
                jm4 = ACC + j5 * m2.y

                ACC = j3 * m0.z
                ACC = ACC + j4 * m1.z
                jm5 = ACC + j5 * m2.z

                'sqc2  @j0, 0x0(%0)
                'sqc2  @j1, 0x10(%0)
                'sqc2  @j2, 0x20(%0)
                'sqc2  @jm3, 0x30(%0)
                'sqc2  @jm4, 0x40(%0)
                'sqc2  @jm5, 0x50(%0)

                __end_expression_asm
                " : : "r" (j_block), "r" (m), "r" (v));
            }

#if PRINT_46OUTPUT
            if(frame==45)
            {
            printJ(j_block);
            }
#endif

            m_vhmf     = m_vhmf + 1;
            j_block    = j_block + 6*4;
            j          = j+1;

        }


        j_bl2body+=MAXBLOCKS;

        //printf("invhh - %12.6f\n",invhh);
        //printf("gamma - ");for(int ii=0;ii!=4;ii++) printf("%12.6f ",gamma[i*4+ii]);printf("\n");
        //printf("xi    - ");for(int ii=0;ii!=4;ii++) printf("%12.6f ",xi[i*4+ii]);printf("\n");
#if PRINT_RHS4INPUT
        __asm__ __volatile__("
        __expression_asm
            'sqc2 @jv, 0x00(%0);
        __end_expression_asm
        " : : "r" (test2) );    
        printf("invh=%12.6f invhh=%12.6f\n",invh,invhh);
        printVec4(test2,"jv");   
        printVec4(c+i*4,"C");
        printVec4(xgamma+i*4,"xgamma");
        printVec4(xi+i*4,"xi");
#endif

        __asm__ __volatile__("
        __expression_asm

        rhs
        c
        xgamma
        xi
        invh
        invhh
        gamma
        gammaxi

        'lqc2   @c, 0x0(%0)
        'lqc2   @xgamma, 0x0(%1)
        'lqc2   @xi, 0x0(%2)
        'mfc1   $8,  %3
        'mfc1   $9,  %4
        'mfc1   $10, %5
        'qmtc2  $8, @invh
        'qmtc2  $9, @invhh
        'qmtc2  $10, @gamma

        xgamma  = xgamma + gamma.x
        gammaxi = xgamma * xi
        ACC = c * invh.x
        ACC = ACC - gammaxi * invhh.x
        rhs = ACC - jv * K.w

        'sqc2  @rhs, 0x0(%2)

        ~rhs
        ~c
        ~xgamma
        ~xi
        ~invh
        ~invhh
        ~gamma
        ~gammaxi

        __end_expression_asm
        " : : "r" (c+i*4),
              "r" (xgamma+i*4),
              "r" (xi+i*4),
              "f" (invh),
              "f" (invhh),
              "f" (gamma)
            : "$8", "$9", "$10");

#if PRINT_RHS4OUTPUT
    printVec4(xi+i*4,"rhs");
#endif
    }

    __asm__ __volatile__("
    __expression_asm

    ~m0
    ~m1
    ~m2
    ~j0
    ~j1
    ~j2
    ~j3
    ~j4
    ~j5
    ~jm3
    ~jm4
    ~jm5
    ~v0
    ~v1
    ~jv

    __end_expression_asm
    ");

#if PRINT_JMINVANDRHSBLOCK_OUTPUT
    //intJinvMandrhsBlockOutput(buf->xi_rhs,buf->j_m_and_vhmf,num_strips,num_jblocks);
#endif
}