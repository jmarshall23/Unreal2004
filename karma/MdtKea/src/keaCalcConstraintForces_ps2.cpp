/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.21.2.3 $

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

#include <string.h>
#include <stdio.h>
#include "keaCalcConstraintForces.hpp"
#include "keaEeDefs.hpp"
#include "cf_block.hpp"
#include <MdtKea.h>
#include <MeMath.h>
#include <MdtKeaProfile.h>
#include "keaDebug.h"
#include <MeMessage.h>
#include <keaFunctions.hpp>

typedef struct _cf_dmachain
{
    sceDmaTag bl2cbody;
    sceDmaTag jlen;
    sceDmaTag lambda;
    sceDmaTag jstore;
} Cf_dmachain;

void sendJAndLamdaToSpr(
         Cf_dmachain *            dmachain,   /* Output */
         const MdtKeaBl2CBodyRow  bl2cbody[], /* Input  */
         const int                jlen[],     /* Input  */
         const MeReal             lambda[],   /* Input  */
         const MdtKeaJBlockPair   jstore[],   /* Input  */
         int                      num_blocks);/* Input  */

void recvConstForcesFromSpr(
         MdtKeaForcePair        cforces[],               /* Input */
         int                    num_constraints_to_save, /* Input */
         int                    buffer);                 /* Input */

/* NB, even if we don't dma jlen, but memcpy it, we still need to dma _something_,
 * because we need to leave a gap in the SPR destination (there is no 'chain mode'
 * inside the SPR) */
static Cf_dmachain g_dmachain __attribute__ ((aligned(64)))=
{
  /*{qwc         ,padding  ,id     ,void *addr,{pad0,pad1}}*/

    {CF_NSTRIPS*2,0        ,REF_ID ,0         ,{0   ,0   }},    // bl2cbody
    {1           ,0        ,REF_ID ,0         ,{0   ,0   }},    // jlen (only works if CF_NSTRIPS=4)
    {CF_NSTRIPS  ,0        ,REF_ID ,0         ,{0   ,0   }},    // lambda
    {0           ,0        ,REFE_ID,0         ,{0   ,0   }}     // jstore
};

/* calculateConstraintForces
 * -------------------------
 * Loops over strips
 *
 * On Entry:
 *
 * blist    - array of MdtKeaBody*         of length 'num_bodies'                    
 * cforces  - array of MdtKeaForcePair     of length 'num_constraints'
 * Jstore   - array of MdtKeaJBlockPair    of length 'num_rows_inc_padding/4'                     
 * Jbody    - array of MdtKeaBodyIndexPair of length 'num_constraints'                    
 * lambda   - array of MeReal              of length 'num_rows_exc_padding'                   
 * bl2body  - array of MdtKeaBl2BodyRow    of length 'num_rows_exc_padding/4'                  
 * bl2cbody - array of MdtKeaBl2CBodyRow   of length 'num_rows_exc_padding/4'                                                       
 * jlen     - array of int                 of length 'num_rows_exc_padding/4'                   
 *
 * num_rows_exc_padding            
 * num_rows_inc_padding_partition
 * num_constraints_partition     
 * num_constraints                 
 * num_partitions                  
 * num_bodies                      
 *
 * Cache status of input:
 *
 * Input:
 * Jstore          - Last written by user                      - flushed in rbdcore, no flush required
 * lambda          - Written by LCP                            - flushed in rbdcore, no flush required
 * bl2cbody        - Written by makejlenandbl2body             - flushed in rbdcore, no flush required
 * cforces         - written by us,but input to calc resultant - written by dma, no flush required
 * jlen            - not dmad
 * c4size          - not dmad
 * num_constraints - not dmad
 *
 * Output:
 * cforces         - read by user                              - invalidate required
 * body[i].force   - read by user                              - not dmad
 *
 * Uses a scratchpad double buffer. There are 2 input buffers, and 2 output buffers.
 * 
 * Iteration    | tospr dma             | from spr dma           | processor
 * ------------------------------------------------------------------------------------
 * 0              load inBuf 0            nop                      nop
 *
 * 1              load inBuf 1            nop                      outBuf0 = calc(inBuf0)
 *
 * 2              load inBuf 0            store outBuf 0           outBuf1 = calc(inBuf1)
 *
 * 3              load inBuf 1            store outBuf 1           outBuf0 = calc(inBuf0)
 *
 * n              load (loadstore_buffer) store (loadstore_buffer) calc(1-loadstore_buffer)
 *
 * How scratchpad is divided up:
 *
 * OutputBuffer0
 * InputBuffer0
 * OutputBuffer1
 * InputBuffer1
*/

void keaFunctions_PS2 :: calculateConstraintAndResultantForces(
    MdtKeaBody *const          blist[],              /* Output/input */
    MdtKeaForcePair            cforces[],            /* Output */
    const MdtKeaJBlockPair     Jstore[],             /* Input */
    const MdtKeaBodyIndexPair  Jbody[],              /* Input */
    const MeReal               lambda[],             /* Input */
    const MdtKeaBl2BodyRow     bl2body[],            /* Input */
    const MdtKeaBl2CBodyRow    bl2cbody[],           /* Input */
    const int                  jlen[],               /* Input */
    int                        num_rows_exc_padding, /* Input */
    int                        num_rows_inc_padding, /* Input */
    int                        num_constraints,      /* Input */
    int                        num_bodies)           /* Input */
{
#if PRINT_CALC_CONSTRAINT_FORCES_INPUT
	printCalcConstraintForcesInput(lambda,
                                   bl2cbody,
                                   bl2body,
                                   num_rows_exc_padding);
#endif

    MdtKeaProfileStart("ps2 calcConstraintForces");

    int                num_constraints_to_save;
    Calc_forces_buf *  save_buffer;
    int                zeroth_constraint;
    int                last_constraint;
    int                strips_done;
    int                strips_to_do_pipe0;
    int                strips_to_do_pipe1;
    int                num_blocks;
    int                loadstore_buffer;
    int                overlap_pipe0;
    int                overlap_pipe1;
    int                i;

    strips_done       = 0;
    loadstore_buffer  = 0;
    while(strips_done!=num_rows_exc_padding/4)
    {
        //printf("strips_done %d\n",strips_done);

        if( ((num_rows_exc_padding/4)-strips_done) >= CF_NSTRIPS ) strips_to_do_pipe0 = CF_NSTRIPS;
        else                                                       strips_to_do_pipe0 = (num_rows_exc_padding/4)-strips_done;

        num_blocks = 0;
        for(i=0;i!=strips_to_do_pipe0;i++) num_blocks += jlen[i];
    
        /* Set up toSpr dma, but dont kick it off */
        sendJAndLamdaToSpr(
            &g_dmachain,     /* Output */
            bl2cbody,        /* Input  */
            jlen,            /* Input  */
            lambda,          /* Input  */
            Jstore,          /* Input  */
            num_blocks);     /* Input  */

        if( strips_done!=0 && (bl2cbody[-1][jlen[-1]-2] == bl2cbody[0][0]))
        { 
            overlap_pipe0 = bl2cbody[-1][jlen[-1]-2]-bl2cbody[-CF_NSTRIPS][0]; 
        }
        else
        { 
            overlap_pipe0 = 0; 
        }

        if(strips_done>CF_NSTRIPS) /* If we are not on iteration 0 or 1, then get results from spr */  
        {
            save_buffer             = ((Calc_forces_buf *)SPR) + loadstore_buffer; 
            zeroth_constraint       = save_buffer->j_bl2cbody[0][0]/2;
            last_constraint         = save_buffer->j_bl2cbody[CF_NSTRIPS-1][save_buffer->j_len[CF_NSTRIPS-1]-2]/2;
            num_constraints_to_save = last_constraint - zeroth_constraint + 1;

            /* Next BC0FWAIT, wait for fromSPR and toSPR */
            DPUT_D_STAT(D_STAT_CISfromSPR_M|D_STAT_CIStoSPR_M);
            DPUT_D_PCR(D_PCR_CPCfromSPR_M|D_PCR_CPCtoSPR_M);
            CPUSYNC

            /* Set up fromSPR dma, but dont kick it off */
            recvConstForcesFromSpr(
                cforces,                 /* Input */
                num_constraints_to_save, /* Input */
                loadstore_buffer);       /* Input */
            
            /* Start the fromSPR dma */
            CPUSYNC
            DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_NORMAL);
        }
        else
        {
            /* We are on iteration 0 or 1, so next BC0FWAIT, wait for toSPR only */
            DPUT_D_STAT(D_STAT_CIStoSPR_M);
            DPUT_D_PCR(D_PCR_CPCtoSPR_M);
            CPUSYNC
        }

        /* Kick off toSPR dma */
        DPUT_CHTOSPR_SADR(loadstore_buffer*sizeof(Calc_forces_buf)+sizeof(MdtKeaForcePair)*FORCES_LEN);
        DPUT_CHTOSPR_QWC(0); 
        DPUT_CHTOSPR_TADR((u_int)&g_dmachain);
        CPUSYNC
        DPUT_CHTOSPR_CHCR(D_CHCR_START | D_CHCR_MODE_CHAIN);

        /* If we are not on iteration 0, do some processing */
        if(strips_done!=0) cf_block(1-loadstore_buffer,strips_to_do_pipe1,overlap_pipe1);

        /* Wait for all dmas to finish */
        BC0FWAIT;

        overlap_pipe1      = overlap_pipe0;
        loadstore_buffer   = 1 - loadstore_buffer;
        strips_done        = strips_done + strips_to_do_pipe0;
        bl2cbody           = bl2cbody + strips_to_do_pipe0;
        jlen               = jlen + strips_to_do_pipe0;
        lambda             = lambda + 4 * strips_to_do_pipe0;
        Jstore             = Jstore + num_blocks/2;
        strips_to_do_pipe1 = strips_to_do_pipe0;
    }

    /* Epilogue 0 */
    if(num_rows_exc_padding!=0)
    {
        if(num_rows_exc_padding/4>CF_NSTRIPS)
        {
            /* There are cforces in the spr that need saving */

            save_buffer             = ((Calc_forces_buf *)SPR) + (loadstore_buffer); 
            zeroth_constraint       = save_buffer->j_bl2cbody[0][0]/2;
            last_constraint         = save_buffer->j_bl2cbody[CF_NSTRIPS-1][save_buffer->j_len[CF_NSTRIPS-1]-2]/2;
            num_constraints_to_save = last_constraint - zeroth_constraint + 1;
            
            /* Next BC0FWAIT, wait only for fromSpr */
            DPUT_D_STAT(D_STAT_CISfromSPR_M);
            DPUT_D_PCR(D_PCR_CPCfromSPR_M);
            CPUSYNC

            recvConstForcesFromSpr(
                cforces,                 /* Input */
                num_constraints_to_save, /* Input */
                loadstore_buffer);       /* Input */

            CPUSYNC            
            DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_NORMAL);
            
            cf_block(1-loadstore_buffer,strips_to_do_pipe1,overlap_pipe1);
            BC0FWAIT;
        }
        else
        {
            cf_block(1-loadstore_buffer,strips_to_do_pipe1,overlap_pipe1);
        }

        /* Epilogue 1 */

        save_buffer       = ((Calc_forces_buf *)SPR) + (1-loadstore_buffer); 
        zeroth_constraint = save_buffer->j_bl2cbody[0][0]/2;

        if( (num_constraints - zeroth_constraint) < FORCES_LEN ) num_constraints_to_save = num_constraints - zeroth_constraint;
        else                                                     num_constraints_to_save = FORCES_LEN;

        /* Next BC0FWAIT, wait only for fromSpr */
        DPUT_D_STAT(D_STAT_CISfromSPR_M);
        DPUT_D_PCR(D_PCR_CPCfromSPR_M);
        CPUSYNC

        recvConstForcesFromSpr(
            cforces,                /* Input */
            num_constraints_to_save, /* Input */
            1-loadstore_buffer);     /* Input */

        CPUSYNC
        DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_NORMAL);

        BC0FWAIT;
    }

    calculateResultantForces(
        blist,                          /* input/output */
        cforces,                        /* input */
        Jbody,                          /* input */
        num_constraints,                /* input */
        num_bodies);                    /* input */

#if PRINT_CALC_CONSTRAINT_FORCES_OUTPUT
    printCalculateConstraintForcesOutput(
        blist,
        cforces,
        num_bodies,
        num_constraints);
#endif

    MdtKeaProfileEnd("ps2 calcConstraintForces");

}

/* sendJAndLamdaToSpr
 * ------------------
 * Set up a dma chain to send J,lambda,bl2cbody and jlen to the scratchpad
 *
 *  This is the dma chain we are filling in:
 *
 *  {CF_NSTRIPS*2,0,REF_ID,0,0,0},                             // bl2cbody
 *  {2,0,REF_ID,0,0,0},                                        // jlen (and 2 words padding) (only works if CF_NSTRIPS=6)
 *  {CF_NSTRIPS,0,REF_ID,0,0,0},                               // lambda
 *  {0,0,REFE_ID,0,0,0},                                       // jstore
 *
*/
void sendJAndLamdaToSpr(
         Cf_dmachain *            dmachain,   /* Output */
         const MdtKeaBl2CBodyRow  bl2cbody[], /* Input  */
         const int                jlen[],     /* Input  */
         const MeReal             lambda[],   /* Input  */
         const MdtKeaJBlockPair   jstore[],   /* Input  */
         int                      num_blocks) /* Input  */
{
    *((sceDmaTag**)(UNCACHED(&(dmachain->bl2cbody.next)))) = (sceDmaTag*)DMAADDR(bl2cbody);
    *((sceDmaTag**)(UNCACHED(&(dmachain->jlen.next    )))) = (sceDmaTag*)DMAADDR(jlen);
    *((sceDmaTag**)(UNCACHED(&(dmachain->lambda.next  )))) = (sceDmaTag*)DMAADDR(lambda);
    *((sceDmaTag**)(UNCACHED(&(dmachain->jstore.next  )))) = (sceDmaTag*)DMAADDR(jstore);
    *((u_short*)   (UNCACHED(&(dmachain->jstore.qwc   )))) = (u_short)(num_blocks*6);
}

/* recvConstForcesFromSpr
 * ----------------------
 * Set up dma channel, to transfer constraint forces from spr to main mem
 *
*/
void recvConstForcesFromSpr(
         MdtKeaForcePair  cforces[],               /* Output */
         int              num_constraints_to_save, /* Input */
         int              buffer)                  /* Input */
{
    int buf                        = (int)(((Calc_forces_buf *)0x0)+buffer);
    Calc_forces_buf *  save_buffer = ((Calc_forces_buf *)SPR) + buffer; 
    MdtKeaForcePair *  dest        = cforces + save_buffer->j_bl2cbody[0][0]/2;

    DPUT_CHFROMSPR_SADR(buf);
    DPUT_CHFROMSPR_MADR((unsigned int)DMAADDR(dest));
    DPUT_CHFROMSPR_QWC ((num_constraints_to_save*sizeof(MdtKeaForcePair))/16);
}
