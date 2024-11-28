#ifndef _CALCJINVMJTANDRHS_HPP
#define _CALCJINVMJTANDRHS_HPP
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.9.6.1 $

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

/* Calculates JM = J * Minv, AND rhs, which is a vector, one element per constraint
 * row, and given by the expression c/h - (gamma*xi)/(h squared) - J * vhmf.
 *
 * The number of constraint rows is given by c4size. This is the length of vhmf,
 * xi, gamma and rhs.
 *
 * Note that there are different values of c, xi and gamma for each constraint row;
 * but different values of vhmf for each _body_. h is the same for all bodies and
 * constraints (it is 1/timestep).
 *
 * J * vhmf gives a vector of length c4size. Each element is the J * vhmf term in
 * the right-hand-side for that constraint row.
 *
 * (We work out J * vhmf at the same time as J * Minv because it means only one
 * pass through J for both things)
 *
 *
 * OUTPUT
 *
 * rhs                         - right hand side, MeReal array of length c4size
 * jm, as an array of 'strips' - JM
 * jm has exactly the same dimensions as constraints->Jstore.
 *
 * INPUT
 *
 * constraints->Jstore         - J, in strips
 * constraints->xgamma         - gamma. An array of c4size MeReals giving gamma for each
 *                               constraint row.
 * constraints->xi             - xi. An array of c4size MeReals giving xi for each
 *                               constraint row.
 * constraints->c              - c. An array of c4size MeReals giving c for each
 *                               constraint row.
 * invIworld                   - Minv. It's an array of MdtKeaInverseMassMatrix's and
 *                               each one is a diagonal block of Minv. It's num_bodies long.
 * bl2body                     - block to body table. 8 columns, one row
 *                               per J strip (i.e. c4size/4). Each entry corresponds to
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
 * c4size                      - the number of constraint rows altogether, rounded up
 *                               to the nearest multiple of 4. c4size/4, therefore, is the
 *                               total number of J strips.
 * stepsize                    - h = 1/stepsize. You need h to work out the right hand side
 *                               (see above).
 */

#ifdef PS2
void calcJinvMandRHS(MeReal                        rhs[],
                     MeReal                        jm[],
                     MeReal                        jstore[],
                     MeReal                        gamma[],
                     MeReal                        c[],
                     MeReal                        xi[],
                     const MdtKeaInverseMassMatrix invIworld[],
                     const int                     bl2body[],
                     const int                     jlen[],
                     const MeReal                  vhmf[],
                     int                           num_bodies,
                     int                           c4size,
                     int                           num_rows_inc_padding,
                     int                           num_rows_inc_padding_partition[],
                     int                           num_rows_exc_padding_partition[],
                     int                           num_constraints_partition[],
                     int                           num_partitions,
                     MeReal                        stepsize);

void oldcalcJinvMandRHS(MeReal                        rhs[],
                        MeReal                        jm[],
                        MeReal                        jstore[],
                        MeReal                        gamma[],
                        MeReal                        c[],
                        MeReal                        xi[],
                        const MdtKeaInverseMassMatrix invIworld[],
                        const int                     bl2body[],
                        const int                     jlen[],
                        const MeReal                  vhmf[],
                        int                           num_bodies,
                        int                           c4size,
                        int                           num_rows_inc_padding,
                        MeReal                        stepsize);

void newcalcJinvMandRHS(MeReal                        rhs[],
                        MeReal                        jm[],
                        MeReal                        jstore[],
                        MeReal                        gamma[],
                        MeReal                        c[],
                        MeReal                        xi[],
                        const MdtKeaInverseMassMatrix invIworld[],
                        const int                     bl2body[],
                        const int                     jlen[],
                        const MeReal                  vhmf[],
                        int                           num_bodies,
                        int                           c4size,
                        int                           num_rows_inc_padding,
                        MeReal                        stepsize);

#else
void calcJinvMandRHS(MeReal *rhs,MeReal *jm,                                /* Outputs */
                     const MdtKeaConstraints *constraints,                  /* Inputs */
                     const MdtKeaInverseMassMatrix *invIworld,              /* Inputs */
                     const int* bl2body,const int* jlen,const MeReal* vhmf, /* Inputs */
                     int num_bodies,int c4size,MeReal stepsize);            /* Inputs */
#endif


#endif // _CALCJINVMJTANDRHS_HPP
