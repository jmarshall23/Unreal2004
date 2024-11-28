/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.24.2.2 $

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
#include "keaDebug.h"
#include <MdtKeaProfile.h>
#include <MeMath.h>
#include <stdio.h>
#ifdef PS2
#include <keaEeDefs.hpp>
#endif
#include <keaFunctions.hpp>

#define NEXTDESTSTRIP \
            pjlen++;  \
            (*pjlen)=0; \
            pbl2bodybase+=8; \
            pbl2conybase+=8; \
            pbl2body=pbl2bodybase; \
            pbl2cony=pbl2conybase;

#define NEXTCONSTRAINT \
            if(*next_constraint < num_constraints - 1) { \
                (*next_constraint) = (*next_constraint) + 1; \
                body0=jbody[*next_constraint][0]; \
                body1=jbody[*next_constraint][1]; \
            }

void singPartmakejlenandbl2body(
                        int *                      next_constraint,          /* Output / Input */
                        int                        jlen[],                   /* Output */
						MdtKeaBl2BodyRow           bl2body[],                /* Output */
						MdtKeaBl2CBodyRow          bl2cony[],                /* Output */ 
						const MdtKeaBodyIndexPair  jbody[],                  /* Input */
						const int                  jsize[],                  /* Input */
						int                        num_strips_inc_padding,   /* Input */
						int                        num_strips,               /* Input */
                        int                        num_constraints)          /* Input */
{
    int i,offs,body0,body1;
    int *pbl2body;
    int *pbl2cony;
    int *pbl2bodybase;
    int *pbl2conybase;
    int *pjlen;

    offs       = 0;
    (*jlen)    = 0;

    body0        = jbody[*next_constraint][0];
    body1        = jbody[*next_constraint][1];

    pbl2body     = (int *)bl2body;
    pbl2cony     = (int *)bl2cony;
    pbl2bodybase = (int *)pbl2body;
    pbl2conybase = (int *)pbl2cony;
    pjlen        = jlen;

    // Iterate over source strips
    for(i=0;i!=num_strips_inc_padding;i++)
    {        
        // Write source strip data to dest strip
        (*pjlen)+=2;
        *(pbl2body++)=body0;
        *(pbl2body++)=body1;
        *(pbl2cony++)=2*(*next_constraint);
        *(pbl2cony++)=2*(*next_constraint)+1;

        if((offs+jsize[*next_constraint])> 4) //if dest strip will overflow when we add current constraint
        {            
            // Keep track of which row we are on
            offs=offs-4;
            // Move to next destination strip
            NEXTDESTSTRIP
        }
        else if((offs+jsize[*next_constraint])==4) //if dest strip will be full when we add current constraint
        {
//#ifdef PS2
            // Keep track of which row we are on
            offs=0;
            // Move to next destination strip
            NEXTDESTSTRIP
            // Move to next constraint, if it exists.
            NEXTCONSTRAINT
             /*
#else
            // Keep track of which row we are on
            offs=4;
            // Move to next constraint, if it exists.
            NEXTCONSTRAINT
#endif      */
        }
        else //if dest strip will have room left when we add current constraint: (offs+jsize[*next_constraint])< 4
        {
            // Keep track of which row we are on
            offs=offs+jsize[*next_constraint];
            // Move to next constraint, if it exists.
            NEXTCONSTRAINT
        }
    }

#if PRINT_JLENANDBL2BODY_OUTPUT
    printJlenandBl2BodyOutput(jlen,(int*)bl2body,(int*)bl2cony,num_strips);
#endif

}
/*  keaFunctions :: makejlenandbl2body
 *  ----------------------------------
 *
 *  Make jlen,bl2body and bl2cony tables for all partitions.
 *  Some functions require each partition of each table to occur contiguously
 *  and some need each partition to start on a particular alignment,
 *  so both padded and unpadded versions of the tables are made.
 *
 *  Note - in the following num_rows_exc_padding_partition[i] is abbreviated ep[i]
 *
 *  On Entry:
 *
 *  jlen             - empty array of size MeMathCEIL12(ep[0]) / 4 +
 *                                         MeMathCEIL12(ep[1]) / 4 +
 *                                           :            :      :
 *                                         MeMathCEIL12(ep[num_partitions-1]) / 4 ints
 *
 *  jlen_unpadded    - empty array of size MeMathCEIL4(ep[0]) / 4 +
 *                                         MeMathCEIL4(ep[1]) / 4 +
 *                                           :           :      :
 *                                         MeMathCEIL4(ep[num_partitions-1]) / 4  ints
 *
 *  bl2body          - empty array of size MeMathCEIL12(ep[0]) / 4 * 8 +
 *                                         MeMathCEIL12(ep[1]) / 4 * 8 +
 *                                           :            :      :
 *                                         MeMathCEIL12(ep[num_partitions-1]) / 4 * 8 ints
 *
 *  bl2body_unpadded - empty array of size MeMathCEIL4(ep[0]) / 4 * 8 +
 *                                         MeMathCEIL4(ep[1]) / 4 * 8 +
 *                                           :           :      :
 *                                         MeMathCEIL4(ep[num_partitions-1]) / 4 * 8 ints
 *
 *
 *  bl2cony          - empty array of size MeMathCEIL12(ep[0]) / 4 * 8 +
 *                                         MeMathCEIL12(ep[1]) / 4 * 8 +
 *                                           :            :      :
 *                                         MeMathCEIL12(ep[num_partitions-1]) / 4 * 8 ints
 *
 *  
 *  On Exit:
 *
 *  jlen
 *  jlen_unpadded
 *  bl2body
 *  bl2body_unpadded
 *  bl2cony
 *
*/
void keaFunctions :: makejlenandbl2body(
                         int                        jlen_12padded[],                   /* Output */
                         int                        jlen[],                            /* Output */
                         MdtKeaBl2BodyRow           bl2body_12padded[],                /* Output */
                         MdtKeaBl2BodyRow           bl2body[],                         /* Output */
                         MdtKeaBl2CBodyRow          bl2cbody[],                        /* Output */
                         const MdtKeaBodyIndexPair  Jbody[],                           /* Input */
                         const int                  Jsize[],                           /* Input */
                         const int                  num_rows_inc_padding_partition[],  /* Input */
                         const int                  num_rows_exc_padding_partition[],  /* Input */
                         const int                  num_constraints_partition[],       /* Input */
                         int                        num_constraints,                   /* Input */
                         int                        num_partitions)                    /* Input */
{
#if PROFILE_KEA
    MdtKeaProfileStart("makejlenandbl2body");
#endif

    int partition,i,j,constraint;

    constraint = 0;

    for (partition = 0;
         partition != num_partitions;
         partition++)
    {
        if (num_constraints_partition[partition] > 0)
        {
            int num_rows               = num_rows_exc_padding_partition[partition];
            int num_strips             = MeMathCEIL4(num_rows) / 4;
            int num_strips_inc_padding = num_rows_inc_padding_partition[partition]/4;
            int ceil3_num_strips       = MeMathCEIL12(num_rows) / 4;

            for(i=0;i!=ceil3_num_strips;i++)
            {
                jlen_12padded[i]=0;

                for(j=0;j!=8;j++)
                {
                    bl2body_12padded[i][j]=-3;
//                    bl2cbody_12padded[i][j]=-1;
                }
            }

            for(i=0;i!=num_strips;i++)
            {
                jlen[i]=0;

                for(j=0;j!=8;j++)
                {
                    bl2body[i][j]=-3;
                    bl2cbody[i][j]=-1;
                }
            }

            singPartmakejlenandbl2body(&constraint,                     /* Input / Output */
                                       jlen,                            /* Output */
                                       bl2body,                         /* Output */
                                       bl2cbody,                        /* Output */
                                       Jbody,                           /* Input */
                                       Jsize,                           /* Input */
                                       num_strips_inc_padding,          /* Input */
                                       num_strips,                      /* Input */
                                       num_constraints);                /* Input */

            for(i=0;i!=num_strips;i++)
            {
                jlen_12padded[i] = jlen[i];
                for(j=0;j!=8;j++)
                {
                    bl2body_12padded[i][j]  = bl2body[i][j];
                }
            }
            
            jlen_12padded     = jlen_12padded    + ceil3_num_strips;
            jlen              = jlen             + num_strips;
            bl2cbody          = bl2cbody         + num_strips;
            bl2body_12padded  = bl2body_12padded + ceil3_num_strips;
            bl2body           = bl2body          + num_strips;
        }
    }
         
#if PROFILE_KEA
    MdtKeaProfileEnd("makejlenandbl2body");
#endif     
}

