#ifndef _KEACALCCONSTRAINTFORCES_HPP
#define _KEACALCCONSTRAINTFORCES_HPP
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.15.2.1 $

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

/* Calculates J transposed x lambda.
 *
 * It works out two things.
 *
 * 1. Jt x lambda for _each body_. This is the total resultant force due to all the constraints
 * on that body.
 *
 * 2. Jt x lambda for the primary and secondary bodies in _each constraint_. This is so users
 * can get this information out to tweak their cars &c.
 *
 *
 * OUTPUT
 *
 * blist[i]->force             - An MeVector4 giving the resultant force on the ith body.
 * blist[i]->torque            - An MeVector4 giving the resultant torque on the ith body.
 * constraints->force          - An array of ConstraintForces giving the forces on the
 *                             - primary and secondary body in that constraint. TODO it's actually
 *                             - an MeReal* at the moment, so beware!
 *
 * INPUT
 *
 * constraints->Jstore         - J. We are working out J transposed lambda, so we use the transpose
 *                             - of J. J is in the form of J strips, and there are c4size/4 of them.
 * constraints->lambda         - lambda. A vector of length c4size.
 * constraint->Jbody           - An array of ConstrainedBodyPairs giving the body numbers of the
 *                             - primary and secondary bodies in that constraint.
 * bl2cbody                    - block to constraint body table. 8 columns, one row
 *                               per J strip (i.e. c4size/4). Each entry corresponds to
 *                               a 6x4 J block and tells you which constraint body it refers to.
 *                               Constraint Bodies work like this. There's an array of
 *                               ConstrainedBodyPairs, containing two Forces, the one on the Primary
 *                               Body and the one on the Seconday Body in that constraint. So you could
 *                               think of that array as an array of Forces (or, indeed, cast it to one).
 *                               A bl2cbody value is an index into that Force array. So, it's the
 *                               _constraint_ that that J block is from, multiplied by 2, and plus 1 if
 *                               that block is from the Secondary body in that constraint. Confused?
 *
 * jlen                        - jlen table. One word per J strip, which is
 *                               the number of 6x4 blocks in that strip.
 * num_bodies                  - the number of bodies. This is the number of bodies that you'll be
 *                             - computing total resultant forces for.
 * num_constraints             - the number of constraints. This is the total number of ConstraintForces
 *                             - you will be computing.
 * c4size                      - the number of constraint rows altogether, rounded up
 *                               to the nearest multiple of 4. c4size/4, therefore, is the
 *                               total number of J strips.
 */
void calculateConstraintForces(MdtKeaBody *const blist[],                          /* Output/input */
                               MeReal            cforces[],                        /* Output */
                               const MeReal      Jstore[],                         /* Input */
                               const MeReal      lambda[],                         /* Input */
                               const int         bl2cbody[],                       /* Input */
                               const int         jlen[],                           /* Input */
                               const int         num_rows_exc_padding_partition[], /* Input */
                               const int         num_rows_inc_padding_partition[], /* Input */
                               const int         num_constraints_partition[],      /* Input */
                               int               num_partitions);                  /* Input */

void oldcalculateConstraintForces(MdtKeaBody *const  blist[],
                               MeReal *           cforces,
                               const MeReal *     Jstore,
                               const MeReal *     lambda,
                               const int *        bl2cbody,
                               const int *        jlen,
                               int                c4size,
                               int                num_constraints);

/* This one gets called by calculateConstraintForces, so don't worry about it.
 * The way it works is that calculateConstraintForces begins by working out
 * the force on the Primary and Secondary body in each constraint. Then it calls
 * this. This one adds up all those constraint forces and adds them to any body
 * forces that may exist preternaturally (e.g. user forces), and thus works
 * out what the resultant force on each each individual body is going to be. */

void calculateResultantForces(MdtKeaBody *const          blist[],            /* input/output */
                              const MdtKeaForcePair      force[],            /* input */
                              const MdtKeaBodyIndexPair  Jbody[],            /* input */
                              int                        num_constraints,    /* input */
                              int                        num_bodies);        /* input */

void calculateAcceleration(MdtKeaBody *const       blist[],                  /* input/output */
                           MdtKeaInverseMassMatrix invIworld[],              /* input */
                           int                     num_bodies);              /* input */

#endif // _KEACALCCONSTRAINTFORCES_HPP
