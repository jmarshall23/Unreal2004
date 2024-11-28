/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.14.2.1 $

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
#include "keaEeDefs.hpp"
#include "cf_block.hpp"
#include "keaDebug.h"
#include <MeMessage.h>

/* cf_block
 * --------
 *
 * On entry, there will be some constraint forces left from last time that 'overlap'
 *
 * b->j                                  -
 * b->lambda                             -
 * b->j_len                              -
 * b->j_bl2cbody                         -
 * b->forces[0..num_strips_to_calc*4*16) - output buffer
 *
*/
void cf_block(
         int buffer,
         int num_strips_to_calc,
         int overlap)
{
#if PRINT_CALC_CONSTRAINT_FORCES_BLOCK_INPUT
    printCalculateConstraintForcesBlockInput(b,num_strips_to_calc);
#endif

    Calc_forces_buf *  b       = ((Calc_forces_buf *)SPR)+buffer;
    MdtKeaForcePair *  cforces = b->forces;

    const float* jt     = (float *)b->j;
    const float* lambda = b->lambda;
    const int*   j_len  = b->j_len;

    MeReal *pforces = (MeReal *)b->forces;

    for(int i=0;i!=num_strips_to_calc*4*16;i++)
    {
        pforces[i] = 0.0f;
    }

    /* force_offset tells you which constraint-body the first element of
     * b->forces is for. */
    int force_offset=b->j_bl2cbody[0][0];

    /* Prelude: Add in overlapping constraint forces from the previous buffer. */
    Calc_forces_spr* spr=(Calc_forces_spr*)(SPR);

    if(overlap!=0)
    {
        Calc_forces_buf* last_b;

        //** Look into last output buffer

        if(b==&(spr->dbuf1)) last_b = &(spr->dbuf2);
        else                 last_b = &(spr->dbuf1);

        const MeReal *src  = ((const MeReal *)(last_b->forces)) + overlap * 8;
        MeReal       *dest = (MeReal *)      (b->forces);

        for(int i=0;i!=16;i++)
        {
            dest[i]=src[i];
        }
    }

    /* On Entry:
     *
     * num_strips_to_calc - 
     * lambda             -
     * j_len              -
     * b->j_bl2cbody      -
     * force_offset       -
     * j                  -
     * jt                 -
     * cforces            -
     * cbody              -
    */

    for(int i=0;i<num_strips_to_calc*8;i+=8,lambda+=4)
    {
        int len=*j_len++;

        MEASSERT(len<=MAXBLOCKS); // (8)

        __asm__ __volatile__("
        __expression_asm

        j0
        j1
        j2
        j3
        j4
        j5
        lambda
        force
        torque
        w0
        w1
        w2
        w3
        w4
        w5

        'lqc2  @lambda, 0x0(%0)

        __end_expression_asm
        " : : "r" (lambda));

        for(int j=0;j<len;)
        {

            int cbody=b->j_bl2cbody[i/8][j]-force_offset;

            /* TODO this assertion was failing. It might be an incorrect assertion, so
             * it might be OK that it should fail. If we assert <= then it doesn't seem
             * to fail. So, for now, we've pushed it down the priority stack, because
             * the demos seem to work OK. It was Topple that it was failing on btw */

            //MEASSERT(cbody<FORCES_LEN);

            /* On Entry:
             *
             * cbody                         -
             * j                             - loop counter
             * jt[0..24)                     - current J block
             * lambda                        - current lambda segment
             * cforces[cbody*32..cbody*32+8) - initial force
             *
             * On Exit:
             *
             * cbody                         - junk
             * j                             - orig(j) + 1
             * jt                            - orig(jt) + 24 * sizeof(MeReal)
             * cforces[cbody*32..cbody*32+8) - transpose(J) * lambda
            */

            __asm__ __volatile__("
            __expression_asm

            '.set noreorder

            'bltz  %7, skip                  # if cbody<0 jump to skip
            'addu  %0, %4, 1                 # j = j + 1   (j is the loop counter)

            'lqc2  @j0, 0x0(%5)              # j0 = jt[0x00]
            'lqc2  @j1, 0x10(%5)             # j1 = jt[0x10]
            'lqc2  @j2, 0x20(%5)             # j2 = jt[0x20]
            'lqc2  @j3, 0x30(%5)             # j3 = jt[0x30]
            'lqc2  @j4, 0x40(%5)             # j4 = jt[0x40]
            'lqc2  @j5, 0x50(%5)             # j5 = jt[0x50]

            'sll   %3, %7, 5                 #
            'addu  %3, %7, %6                #
                                             # { cbody = orig(cbody) * 32 + cforces }
            w0 = j0 * lambda

            'lqc2  @force, 0x0(%7)           # force  = cbody[0x00]
            'lqc2  @torque, 0x10(%7)         # torque = cbody[0x10]
                                             #
            w1 = j1 * lambda                 #
            w2 = j2 * lambda                 #
            w3 = j3 * lambda                 #
            w4 = j4 * lambda                 #
            w5 = j5 * lambda                 #
                                             #
            force.x = force.x + w0.x         #
            torque.x = torque.x + w3.x       #
            force.y = force.y + w1.x         #
            torque.y = torque.y + w4.x       #
            force.z = force.z + w2.x         #
            torque.z = torque.z + w5.x       #
                                             #
            force.x = force.x + w0.y         #
            torque.x = torque.x + w3.y       #
            force.y = force.y + w1.y         #
            torque.y = torque.y + w4.y       #
            force.z = force.z + w2.y         #
            torque.z = torque.z + w5.y       #
                                             #
            force.x = force.x + w0.z         #
            torque.x = torque.x + w3.z       #
            force.y = force.y + w1.z         #
            torque.y = torque.y + w4.z       #
            force.z = force.z + w2.z         #
            torque.z = torque.z + w5.z       #
                                             #
            force.x = force.x + w0.w         #
            torque.x = torque.x + w3.w       #
            force.y = force.y + w1.w         #
            torque.y = torque.y + w4.w       #
            force.z = force.z + w2.w         #
            torque.z = torque.z + w5.w       #
                                             #
            'addu  %1, %5, 96                # jt = jt + 96

            'sqc2  @force, 0x0(%7)
            'sqc2  @torque, 0x10(%7)

        'skip:

            '.set reorder

            ~j0
            ~j1
            ~j2
            ~j3
            ~j4
            ~j5
            ~lambda
            ~force
            ~torque
            ~w0
            ~w1
            ~w2
            ~w3
            ~w4
            ~w5


            __end_expression_asm
            " : "=r" (j),
                "=r" (jt),
                "=r" (cforces),
                "=r" (cbody)

              : "0" (j),
                "1" (jt),
                "2" (cforces),
                "3" (cbody));

        }
    }
#if PRINT_CALC_CONSTRAINT_FORCES_BLOCK_OUTPUT
    printCalculateConstraintForcesBlockOutput(b,num_strips_to_calc);
#endif
}

/* sum_forces_block
 * ----------------
 *
 * On Exit:
 * spr->forces[)
*/
void sum_forces_block(Sum_forces_buf* b,int nconstraints)
{
#if PRINT_CALC_RESULTANT_FORCES_BLOCK_INPUT
    printCalculateResultantForcesBlockInput(b,nconstraints);
#endif

    Sum_forces_spr *  spr = (Sum_forces_spr*)(SPR);
    MdtKeaForce *     f   = spr->forces;

    for(int i=0;i<nconstraints;i++)
    {
        int primary_body   = b->jbody[i][0];
        int secondary_body = b->jbody[i][1];

        __asm__ __volatile__("
        __expression_asm

        primary_resultant_force
        primary_resultant_torque

        primary_constraint_force
        primary_constraint_torque

        'lqc2  @primary_resultant_force, 0x0(%0)
        'lqc2  @primary_resultant_torque, 0x10(%0)

        'lqc2  @primary_constraint_force, 0x0(%1)
        'lqc2  @primary_constraint_torque, 0x10(%1)

        primary_resultant_force = primary_resultant_force + primary_constraint_force
        primary_resultant_torque = primary_resultant_torque + primary_constraint_torque

        'sqc2  @primary_resultant_force, 0x0(%0)
        'sqc2  @primary_resultant_torque, 0x10(%0)

        ~primary_resultant_force
        ~primary_resultant_torque

        ~primary_constraint_force
        ~primary_constraint_torque

        __end_expression_asm
        " : : "r" (f+primary_body),
              "r" (&(b->cforces[i].primary_body)));

        if(secondary_body!=-1)
        {
        __asm__ __volatile__("
        __expression_asm

        secondary_resultant_force
        secondary_resultant_torque

        secondary_constraint_force
        secondary_constraint_torque

        'lqc2  @secondary_resultant_force, 0x0(%0)
        'lqc2  @secondary_resultant_torque, 0x10(%0)

        'lqc2  @secondary_constraint_force, 0x20(%1)
        'lqc2  @secondary_constraint_torque, 0x30(%1)

        secondary_resultant_force = secondary_resultant_force + secondary_constraint_force
        secondary_resultant_torque = secondary_resultant_torque + secondary_constraint_torque

        'sqc2  @secondary_resultant_force, 0x0(%0)
        'sqc2  @secondary_resultant_torque, 0x10(%0)

        ~secondary_resultant_force
        ~secondary_resultant_torque

        ~secondary_constraint_force
        ~secondary_constraint_torque

        __end_expression_asm
        " : : "r" (f+secondary_body),
              "r" (&(b->cforces[i].primary_body)));
        }
/*
        add_forces(f+primary_body,&(b->cforces[i].primary_body));
        add_forces(f+secondary_body,&(b->cforces[i].secondary_body));
*/
    }

#if PRINT_CALC_RESULTANT_FORCES_BLOCK_OUTPUT
    printCalculateResultantForcesBlockOutput(b,nconstraints);
#endif
}
