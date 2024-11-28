/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.29.2.3 $

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

#include <MeAssert.h>
#include <stdio.h>
#include <string.h>
#include "keaCalcJinvMandRHS.hpp"
#include "JM_block.hpp"
#include "keaEeDefs.hpp"
#include <MdtKea.h>
#include "keaInternal.hpp"
#include <MdtKeaProfile.h>
#include "keaDebug.h"
#include "calcA.hpp"
#include <keaFunctions.hpp>

// The start of the first buffer
#define BUF1 (0x70002580)

// The offset to the start of the second buffer
#define BUFOFFSET (0xa20)

/* This is the DMA chain used to upload the data to the scratchpad */
struct Jm_dmachain
{
    sceDmaTag gamma;
    sceDmaTag c;
    sceDmaTag bl2body;
    sceDmaTag xi;
    sceDmaTag jstore;
    sceDmaTag pad0;
    sceDmaTag pad1;
    sceDmaTag pad2;
} ;

/* Note that bl2body is given ss NSTRIPS*2 + 2 quadwords. The +2 is 1 quadword
 * for jlen, and 1 quadword for the DmaTag for sending xi_rhs *back*. Basically
 * we want to leave a 2 quadword gap between bl2body and xi, and this is the only
 * way to do it.
 *
 * Similarly, the quadword count of xi is +1, leaving room for the Destination Chain tag
 * for sending back the jm strips.
 */

typedef struct {
    sceDmaTag invm;
    sceDmaTag vhmf;
} invMandVhmfDmaChain;

typedef struct {
    sceDmaTag bl2body;
    sceDmaTag gamma;
    sceDmaTag c;
    sceDmaTag xi;
    sceDmaTag jstore;
    invMandVhmfDmaChain mAndVhmf[NSTRIPS*8];
    sceDmaTag pad0;
    sceDmaTag pad1;
    sceDmaTag pad2;    
} JMandRHSDmaChain;

#define BL2BROW_QWC 2
#define J_QWC       12
#define M0_QWC      3
#define M1_QWC      3
#define VHMF0_QWC   2
#define VHMF1_QWC   2
#define GAMMA_QWC   1
#define XI_QWC      1 
#define C_QWC       1

JMandRHSDmaChain JMdmachain __attribute__ ((aligned(128))) = {
    // {short qwc,char padding,char id,void *addr,int pad0,int pad1}

    {BL2BROW_QWC*NSTRIPS,0,REF_ID,0,{0,0}}, /* bl2body */
    {GAMMA_QWC          ,0,REF_ID,0,{0,0}}, /* gamma   */
    {C_QWC              ,0,REF_ID,0,{0,0}}, /* c       */
    {XI_QWC             ,0,REF_ID,0,{0,0}}, /* xi      */
    {0                  ,0,REF_ID,0,{0,0}}, /* jstore  */

    {
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm0  */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf0  */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm1  */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf1  */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm2  */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf2  */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm3  */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf3  */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm4  */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf4  */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm5  */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf5  */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm6  */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf6  */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm7  */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf7  */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm8  */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf8  */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm9  */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf9  */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm10 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf10 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm11 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf11 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm12 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf12 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm13 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf13 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm14 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf14 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm15 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf15 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm16 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf16 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm17 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf17 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm18 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf18 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm19 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf19 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm20 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf20 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm21 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf21 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm22 */
         {VHMF0_QWC ,0,REF_ID,0,{0,0}}},  /* vhmf22 */
        {{M0_QWC    ,0,REF_ID,0,{0,0}},   /* invm23 */
        {VHMF0_QWC ,0,REF_ID,0,{0,0}}}   /* vhmf23 */
    }
};
/*
   Set up dma chain to send J,xi,c,xgamma,vhmf,invIworld to scratchpad
   Send jlen is not sent, but bl2body is
   bl2body is used to make the ref tags for invIworld and vhmf

   Format of dmachain:

   typedef struct {
       sceDmaTag invm;
       sceDmaTag vhmf;
   } invMandVhmfDmaChain;

   typedef struct {
       sceDmaTag bl2body;
       sceDmaTag gamma;
       sceDmaTag c;
       sceDmaTag xi;
       sceDmaTag jstore;
       invMandVhmfDmaChain mAndVhmf[NSTRIPS*8];
       sceDmaTag pad0;
       sceDmaTag pad1;
       sceDmaTag pad2;    
   } JMandRHSDmaChain;

*/
void sendToSpr(JMandRHSDmaChain *                   dmachain,
               const MdtKeaJBlock                   jstore[],
               const MeReal                         gamma[],
               const MeReal                         c[],
               const MeReal                         xi[],
               const MdtKeaInverseMassMatrix        invIworld[],
               const MdtKeaBl2BodyRow               bl2body[],
               const int                            jlen[],
               const MdtKeaVelocity                 vhmf[],
               int                                  strips_to_do,
               int                                  num_blocks,
               int                                  dest_buffer)
{
    int i,j,index,body;
    int blocks;

    newCalc_JM_buf *buf = ((newCalc_JM_buf *)0x70000000) + dest_buffer;
    buf->parameters.num_blocks = num_blocks;
    buf->parameters.num_strips = strips_to_do;


    *((sceDmaTag**)(UNCACHED(&(dmachain->bl2body.next  )))) = (sceDmaTag*)DMAADDR(bl2body);
    *((sceDmaTag**)(UNCACHED(&(dmachain->gamma.next    )))) = (sceDmaTag*)DMAADDR(gamma);
    *((sceDmaTag**)(UNCACHED(&(dmachain->c.next        )))) = (sceDmaTag*)DMAADDR(c);
    *((sceDmaTag**)(UNCACHED(&(dmachain->xi.next       )))) = (sceDmaTag*)DMAADDR(xi);
    *((sceDmaTag**)(UNCACHED(&(dmachain->jstore.next   )))) = (sceDmaTag*)DMAADDR(jstore);

    /* c and xi are 1 quad longer thna necessary to leave room for recv tags */

    *((u_short*)   (UNCACHED(&(dmachain->bl2body.qwc   )))) = (u_short)NSTRIPS*BL2BROW_QWC;
    *((u_short*)   (UNCACHED(&(dmachain->gamma.qwc     )))) = (u_short)NSTRIPS;
    *((u_short*)   (UNCACHED(&(dmachain->c.qwc         )))) = (u_short)NSTRIPS+1;
    *((u_short*)   (UNCACHED(&(dmachain->xi.qwc        )))) = (u_short)NSTRIPS+1;

    index  = 0;
    blocks = 0;
    for(i=0;i!=strips_to_do-1;i++)
    {
        for(j=0;j!=jlen[i];j++)
        {
            body = bl2body[i][j];
            *((sceDmaTag**)(UNCACHED(&(dmachain->mAndVhmf[index].invm.next  )))) = (sceDmaTag*)DMAADDR(invIworld+body);
            *((sceDmaTag**)(UNCACHED(&(dmachain->mAndVhmf[index].vhmf.next  )))) = (sceDmaTag*)DMAADDR(vhmf+body);
            *((u_char*)    (UNCACHED(&(dmachain->mAndVhmf[index].vhmf.id    )))) = (u_char)REF_ID;
            index++;
        }
        blocks = blocks + jlen[i];
    }

    for(j=0;j!=jlen[i]-1;j++)
    {
        body = bl2body[i][j];

        *((sceDmaTag**)(UNCACHED(&(dmachain->mAndVhmf[index].invm.next  )))) = (sceDmaTag*)DMAADDR(invIworld+body);
        *((sceDmaTag**)(UNCACHED(&(dmachain->mAndVhmf[index].vhmf.next  )))) = (sceDmaTag*)DMAADDR(vhmf+body);
        *((u_char*)    (UNCACHED(&(dmachain->mAndVhmf[index].vhmf.id    )))) = (u_char)REF_ID;
        index++;
    }
    blocks = blocks + jlen[i];
    
    body = bl2body[i][j];

    *((sceDmaTag**)(UNCACHED(&(dmachain->mAndVhmf[index].invm.next  )))) = (sceDmaTag*)DMAADDR(invIworld+body);
    *((sceDmaTag**)(UNCACHED(&(dmachain->mAndVhmf[index].vhmf.next  )))) = (sceDmaTag*)DMAADDR(vhmf+body);
    *((u_char*)    (UNCACHED(&(dmachain->mAndVhmf[index].vhmf.id    )))) = (u_char)REFE_ID;
    index++;

    *((u_short*)   (UNCACHED(&(dmachain->jstore.qwc  )))) = (u_short)6*blocks;

}
void recvFromSpr(
         MeReal *        rhs,    /* Output */
         MdtKeaJBlock *  jm,     /* Input  */
         int             buffer) /* Input  */
{
    newCalc_JM_buf *buf;
    sceDmaTag   *rhs_tag;
    sceDmaTag   *jm_tag;
    int         num_blocks;

    buf = ((newCalc_JM_buf *)0x70000000)+buffer;
    num_blocks = buf->parameters.num_blocks;
    
    //** Zero the Destination Chain tags.

    rhs_tag = &(buf->rhs_tag);
    jm_tag  = &(buf->jm_tag);

    __asm__ __volatile__("
        sqc2  $vf1,0x0(%0)
        sqc2  $vf1,0x0(%1)
    " : : "r" (jm_tag), "r" (rhs_tag));

    //** Setup the rhs tag

    rhs_tag->qwc  = NSTRIPS;
    rhs_tag->id   = CNT_ID;
    rhs_tag->next = (sceDmaTag*)DMAADDR(rhs);

    //** Set up the jm tag

    jm_tag->qwc  = 6*num_blocks;
    jm_tag->id   = END_ID;
    jm_tag->next = (sceDmaTag*)DMAADDR(jm);

}

/*
 * num_rows_exc_padding - num rows exc padding
 *
 * Uses a scratchpad triple buffer:
 *
 * Scratchpad is divided up into 3 buffers, an input buffer, and output buffer and a processing buffer
 * Data is processed 'in place', ie results of calculation are written over the input
 *
 * Iteration    | tospr dma            | from spr dma          | processor
 * ------------------------------------------------------------------------------------
 * 0              load into buffer 0     nop                     nop
 *
 * 1              load into buffer 1     nop                     buffer0 := calc(buffer 0)
 *
 * 2              load into buffer 2     save buffer 0           buffer1 := calc(buffer 1)
 *
 * 3              load into buffer 0     save buffer 1           buffer2 := calc(buffer 2)
 *
 * n              load into load_buffer  save (load_buffer+1)%3  calc (load_buffer+2)%3
 *
 * etc....
 * 
 * Epilogue
 * --------
 * num_strips/3   nop                    save (load_buffer+1)%3  calc (load_buffer+2)%3
 *
 * num_strips/3+1 nop                    save (load_buffer+2)%3  nop
 *
 * Base cases
 * ----------
 *
 * 0 blocks: (nothing done)
 *
 * 1 block: (num_rows_exc_padding/4<=NSTRIPS)
 *
 * 0              load into buffer 0     nop                     nop
 *
 * 1 (epilog 0)   nop                    nop                     calc (load_buffer+2)%3
 *          
 * 2 (epilog 1)   nop                    save (load_buffer+2)%3  nop
 *  
 *
 * How scratchpad memory is divided up:
 *
*/
/* Calculates JM = J * Minv, AND rhs, which is a vector, one element per constraint
 * row, and given by the expression c/h - (gamma*xi)/(h squared) - J * vhmf.
 *
 * The number of constraint rows is given by num_rows_exc_padding. This is the length of vhmf,
 * xi, gamma and rhs.
 *
 * Note that there are different values of c, xi and gamma for each constraint row;
 * but different values of vhmf for each _body_. h is the same for all bodies and
 * constraints (it is 1/timestep).
 *
 * J * vhmf gives a vector of length num_rows_exc_padding. Each element is the J * vhmf term in
 * the right-hand-side for that constraint row.
 *
 * (We work out J * vhmf at the same time as J * Minv because it means only one
 * pass through J for both things)
 *
 *
 * OUTPUT
 *
 * rhs                         - right hand side, MeReal array of length num_rows_exc_padding
 * jm, as an array of 'strips' - JM
 * jm has exactly the same dimensions as constraints->Jstore.
 *
 * INPUT
 *
 * constraints->Jstore         - J, in strips
 * constraints->xgamma         - gamma. An array of num_rows_exc_padding MeReals giving gamma for each
 *                               constraint row.
 * constraints->xi             - xi. An array of num_rows_exc_padding MeReals giving xi for each
 *                               constraint row.
 * constraints->c              - c. An array of num_rows_exc_padding MeReals giving c for each
 *                               constraint row.
 * invIworld                   - Minv. It's an array of MdtKeaInverseMassMatrix's and
 *                               each one is a diagonal block of Minv. It's num_bodies long.
 * bl2body                     - block to body table. 8 columns, one row
 *                               per J strip (i.e. num_rows_exc_padding/4). Each entry corresponds to
 *                               a 6x4 J block and tells you which body (i.e. column block
 *                               of abstract J) it refers to.
 * jlen                        - jlen table. One word per J strip, which is
 *                               the number of 6x4 blocks in that strip.
 * vhmf                        - A vector, with 8 words per body; i.e. 8 words
 *                               per MdtKeaInverseMassMatrix in invIworld.
 *                               There are actuallly 6 elements per body, but they're stored in
 *                               8 words as {thing,thing,thing,blank,thing,thing,thing,blank}.
 * num_bodies                  - this is the length of the invIworld array, and num_bodies*8
 *                               will give you the length of the vhmf array in words.
 * num_rows_exc_padding        - the number of constraint rows altogether, rounded up
 *                               to the nearest multiple of 4. num_rows_exc_padding/4, therefore, is the
 *                               total number of J strips.
 * stepsize                    - h = 1/stepsize. You need h to work out the right hand side
 *                               (see above).
 */

void keaFunctions_PS2 :: calcJinvMandRHS(
    MeReal                        rhs[],                /* Output */
    MdtKeaJBlockPair              jmstore[],            /* Output */
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
    MeReal                        gamma)                /* Input */
{
#if PRINT_CALCJMINVANDRHS_INPUT
    printJinvMandrhsInput(
        rhs,                  /* Output */
        jmstore,              /* Output */
        jstore,               /* Input */
        xgamma,               /* Input */
        c,                    /* Input */
        xi,                   /* Input */
        invIworld,            /* Input */
        bl2body,              /* Input */
        jlen,                 /* Input */
        vhmf,                 /* Input */
        num_bodies,           /* Input */
        num_rows_exc_padding, /* Input */
        num_rows_inc_padding, /* Input */
        stepsize,             /* Input */
        gamma);               /* Input */
#endif

#if PROFILE_KEA
    MdtKeaProfileStart("ps2 calcJinvMandRHS");
#endif

    newCalc_JM_buf *buf;
    int strips_done;
    int strips_to_do;
    int load_buffer;
    int num_blocks;
    int i;
    MeReal *              prhs;
    MdtKeaJBlock *        pjm;
    const MdtKeaJBlock *  pj;

    MeReal invh  = MeRecip(stepsize);
    MeReal invhh = MeRecip(stepsize*stepsize);

    prhs        = rhs;
    pjm         = (MdtKeaJBlock *)jmstore;
    pj          = (MdtKeaJBlock *)jstore;
    strips_done = 0;
    load_buffer = 0;
    while(strips_done!=num_rows_exc_padding/4)
    {
        if( ((num_rows_exc_padding/4)-strips_done) >= NSTRIPS ) strips_to_do = NSTRIPS;
        else                                                    strips_to_do = (num_rows_exc_padding/4)-strips_done;

        num_blocks = 0;
        for(i=0;i!=strips_to_do;i++) num_blocks += jlen[i];

        sendToSpr(&JMdmachain,
                  pj,
                  xgamma,
                  c,
                  xi,
                  invIworld,
                  bl2body,
                  jlen,
                  vhmf,
                  strips_to_do,
                  num_blocks,
                  load_buffer);

        bl2body             = bl2body + strips_to_do;
        xgamma              = xgamma + strips_to_do*4;
        c                   = c + strips_to_do*4;
        xi                  = xi + strips_to_do*4;
        pj                  = pj + num_blocks;

        if(strips_done>NSTRIPS)  
        {
            buf        = ((newCalc_JM_buf *)0x70000000)+((load_buffer+1)%3);
            num_blocks = buf->parameters.num_blocks;

            recvFromSpr(prhs,pjm,(load_buffer+1)%3);

            prhs = prhs + NSTRIPS*4;
            pjm  = pjm + num_blocks;

            DPUT_D_STAT(D_STAT_CISfromSPR_M|D_STAT_CIStoSPR_M);
            DPUT_D_PCR(D_PCR_CPCfromSPR_M|D_PCR_CPCtoSPR_M);
            CPUSYNC

            DPUT_CHFROMSPR_QWC(0);
            DPUT_CHFROMSPR_SADR((((load_buffer+1)%3)*sizeof(newCalc_JM_buf))+RHSTAG_OFFSET_BYTES);
            CPUSYNC
            DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_CHAIN);
        }
        else
        {
            DPUT_D_STAT(D_STAT_CIStoSPR_M);
            DPUT_D_PCR(D_PCR_CPCtoSPR_M);
            CPUSYNC
        }

        /* Kick off send to spr dma */

        DPUT_CHTOSPR_SADR(load_buffer*sizeof(newCalc_JM_buf)+sizeof(JMparam)); // Dest in SPR
        DPUT_CHTOSPR_QWC(0);
        DPUT_CHTOSPR_TADR((u_int)&JMdmachain);                 // Address of chain
        CPUSYNC
        DPUT_CHTOSPR_CHCR(D_CHCR_START | D_CHCR_MODE_CHAIN);

        if(strips_done!=0)       jm_block((load_buffer+2)%3,invh,invhh,gamma);

        /* wait for dma to finish */

        BC0FWAIT;        

        load_buffer = (load_buffer + 1)%3;
        strips_done = strips_done + strips_to_do;
        jlen        = jlen + strips_to_do;

    }

    /* Epilogue 0 */

    if(num_rows_exc_padding!=0)
    {
        if(num_rows_exc_padding/4>NSTRIPS)
        {
            buf        = ((newCalc_JM_buf *)0x70000000)+((load_buffer+1)%3);
            num_blocks = buf->parameters.num_blocks;

            recvFromSpr(prhs,pjm,(load_buffer+1)%3);
            prhs = prhs + NSTRIPS*4;
            pjm  = pjm + num_blocks;

            DPUT_D_STAT(D_STAT_CISfromSPR_M);
            DPUT_D_PCR(D_PCR_CPCfromSPR_M);
            CPUSYNC

            DPUT_CHFROMSPR_QWC(0);
            DPUT_CHFROMSPR_SADR((((load_buffer+1)%3)*sizeof(newCalc_JM_buf))+RHSTAG_OFFSET_BYTES);
            CPUSYNC
            DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_CHAIN);

            jm_block((load_buffer+2)%3,invh,invhh,gamma);

            BC0FWAIT;        
        }
        else
        {
            jm_block((load_buffer+2)%3,invh,invhh,gamma);
        }

        /* Epilogue 1 */

        recvFromSpr(prhs,pjm,(load_buffer+2)%3);

        DPUT_D_STAT(D_STAT_CISfromSPR_M);
        DPUT_D_PCR(D_PCR_CPCfromSPR_M);
        CPUSYNC

        DPUT_CHFROMSPR_QWC(0);
        DPUT_CHFROMSPR_SADR((((load_buffer+2)%3)*sizeof(newCalc_JM_buf))+RHSTAG_OFFSET_BYTES);
        CPUSYNC
        DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_CHAIN);

        BC0FWAIT;        
    }

#if PRINT_CALCJMINVANDRHS_OUTPUT
    printJinvMandrhsOutput(
        rhs,
        jmstore,
        num_rows_exc_padding, 
        jlen, 
        num_rows_inc_padding);
#endif

#if PROFILE_KEA
    MdtKeaProfileEnd("ps2 calcJinvMandRHS");
#endif

}
