/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.20.2.2 $

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
#include <MeMath.h>
#include "string.h"
#include "keaDebug.h"
#include "KeaSSEi.h"
#include <MdtKeaProfile.h>
#include "keaFunctions.hpp"
#include "keaInternal.hpp"

static void Multiply4VectorWith46Matrix(MdtKeaBody*          bodyforce, /* Output */
                                        MdtKeaForce*         conyforce,   /* Output */
                                        const MeReal         lambda[],    /* Input  */
                                        const MdtKeaJBlock*  jstrip);     /* Input  */

void keaFunctions_SSE :: calculateConstraintAndResultantForces(
    MdtKeaBody *const          blist[],              /* Output/input */
    MdtKeaForcePair            cforcePairs[],        /* Output */
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
    int strip, block, body;
    const MeReal *  Lambda;
    const MdtKeaJBlock * J;
    
    MdtKeaForce * cforces = &cforcePairs->primary_body;

#if PRINT_CALC_CONSTRAINT_FORCES_INPUT
	printCalcConstraintForcesInput(lambda,
                                   bl2cbody,
                                   bl2body,
                                   num_rows_exc_padding);
#endif
#ifndef _NOTIC
    MdtKeaProfileStart("calculateConstraintForces");
#endif

    Lambda = lambda;
    J      = (MdtKeaJBlock *)Jstore;

    for(strip=0; strip!=num_rows_exc_padding/4; strip++)
    {
        const MdtKeaJBlock* jstrip;
        const int *        JMb;
        const int *        JMc;

        jstrip = J;
        JMb    = (int *)bl2body;
        JMc    = (int *)bl2cbody;

        for(block=0; block!=*jlen; block++)
        {
            body=*JMb;

            if(body!=-1)
            {
                Multiply4VectorWith46Matrix(blist[body],
                                            cforces+(*JMc),
                                            Lambda, 
                                            jstrip);
            }
            JMb++;
            JMc++;
            jstrip++;
        }
        Lambda+=4;
        J+= (*jlen);
        bl2body+=1;
        bl2cbody+=1;
        jlen++;
    }

#if PRINT_CALC_CONSTRAINT_FORCES_OUTPUT
    printCalculateConstraintForcesOutput(blist,
                                         cforcePairs,
                                         num_bodies,
                                         num_constraints);
#endif
#ifndef _NOTIC
    MdtKeaProfileEnd("calculateConstraintForces");
#endif
}

//**********************************************************

__forceinline void Multiply4VectorWith46Matrix(MdtKeaBody*         body,
                                               MdtKeaForce*        cony,
                                               const MeReal*       lambda,
                                               const MdtKeaJBlock* jstrip)
{
    __m128 l = _mm_load_ps(lambda);
    __m128 z = _mm_setzero_ps();
    __m128 r1, r2, r3, r;
    //force
    r1 = _mm_mul_ps(l, _mm_load_ps(jstrip->col[0]));
    r2 = _mm_mul_ps(l, _mm_load_ps(jstrip->col[1]));
    r3 = _mm_mul_ps(l, _mm_load_ps(jstrip->col[2]));
    r  = horzadd4(r1, r2, r3, z);
    _mm_store_ps(body->force, _mm_add_ps(_mm_load_ps(body->force) , r));
    _mm_store_ps(cony->force, _mm_add_ps(_mm_load_ps(cony->force) , r));
    
    //torque
    r1 = _mm_mul_ps(l, _mm_load_ps(jstrip->col[3]));
    r2 = _mm_mul_ps(l, _mm_load_ps(jstrip->col[4]));
    r3 = _mm_mul_ps(l, _mm_load_ps(jstrip->col[5]));
    r  = horzadd4(r1, r2, r3, z);
    _mm_store_ps(body->torque, _mm_add_ps(_mm_load_ps(body->torque) , r));
    _mm_store_ps(cony->torque, _mm_add_ps(_mm_load_ps(cony->torque) , r));

}

//**********************************************************
