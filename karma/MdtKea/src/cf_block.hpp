#ifndef _CF_BLOCK_HPP
#define _CF_BLOCK_HPP

/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.8.2.1 $

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

#define MAXBODIES (200)

/* CF_NSTRIPS must be a multiple of 4 */
#define CF_NSTRIPS (4)
#define MAXBLOCKS (8)

#define CF_J_LEN (CF_NSTRIPS*MAXBLOCKS/2)
#define J_BL2CBODY_LEN (CF_NSTRIPS)
#define CF_J_LEN_LEN (CF_NSTRIPS)

#define LAMBDA_LEN (CF_NSTRIPS*4)

/* A worst-case J strip, of 8 blocks, can only refer to 4 constraints */
#define FORCES_LEN (CF_NSTRIPS*4)

/* bl2cbody means block2constraintbody, and it works like this.
 * It's constraint*2 + 1 if it's the Secondary Body, and + 0 if
 * it's the Primary Body.
 *
 * That way, bl2cbody*8 is the offset where the force goes.
 * (We store force force force blank torque torque torque blank)
 *
 * The 'constraint body forces' are an array of Constraint_forces.
 *
 * a bl2cbody number is an index into this array. Element x of this
 * array means Constraint number x.
 *
 * It tells you the forces on the primary and secondary bodies in
 * that constraint.
 *
 * One can think of the array of constraint forces as a union of two arrays.
 *
 * 1. the array of constraint forces, consisting of a force for the primary
 * and secondary body for each constraint.
 *
 * 2. the array of constraint-body-forces, consisting of a force for each body
 * in each constraint.
 *
 * the bl2cbody indices identify constraint-bodies belonging to each J block.
 */

typedef struct _calc_forces_buf
{
    // output
    MdtKeaForcePair forces[FORCES_LEN];

    // input
    MdtKeaBl2CBodyRow j_bl2cbody[J_BL2CBODY_LEN];
    int               j_len[CF_J_LEN_LEN];
    float             lambda[LAMBDA_LEN];

    // Room for 4 J strips
    MdtKeaJBlockPair  j[CF_J_LEN]; // variable length

} Calc_forces_buf;

struct Calc_forces_spr
{
    Calc_forces_buf dbuf1;
    Calc_forces_buf dbuf2;
};

/* About 12 seems to give good performance. Theoretical maximum is 69 */
#define SUM_CFORCES_LEN (12)

typedef struct _sum_forces_buf{
    MdtKeaForcePair     cforces[SUM_CFORCES_LEN];
    MdtKeaBodyIndexPair jbody[SUM_CFORCES_LEN];
} Sum_forces_buf;

typedef struct _sum_forces_spr
{
    MdtKeaForce forces[MAXBODIES];
    Sum_forces_buf dbuf1;
    Sum_forces_buf dbuf2;
} Sum_forces_spr;

/* make first true if it's the first block in the block-multiply you're doing */
void cf_block(
         int buffer,
         int num_strips_to_calc,
         int overlap);

void sum_forces_block(Sum_forces_buf* b,int nconstraints);


#endif // _CF_BLOCK_HPP
