/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2003/01/09 16:16:11 $ - Revision: $Revision: 1.15.2.1.4.1 $

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
#include "keaFunctions.hpp"
#include "keaDebug.h"
#include <MdtKeaProfile.h>
#include "KeaSSEi.h"

static void Multiply46BlockByMBlock(
                       MdtKeaJBlock *                   jmblock,   /* Output */
                       const MdtKeaJBlock            *  jblock,    /* Input  */
                       const MdtKeaInverseMassMatrix *  invMblock);/* Input  */

static __m128 Multiply46BlockBy61Vector(
                        __m128                  xmmreg,            /* Output/Input */
                        const MdtKeaJBlock *    jblock,            /* Input  */ 
                        const MdtKeaVelocity *  vmMif);            /* Input  */

//***************************************************

void keaFunctions_SSE :: calcJinvMandRHS(
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

#ifndef _NOTIC
    MdtKeaProfileStart("calcJinvMandRHS");
#endif

    int strip, block, body;
    const MdtKeaJBlock *  J         = (MdtKeaJBlock *)jstore;
    MdtKeaJBlock *        JM        = (MdtKeaJBlock *)jmstore;
    
    float hi = MeRecip(stepsize);
    __m128 hinv = rspread(hi);              //FPP
    __m128 gama = rspread(gamma);

    for(strip=0;strip!=num_rows_exc_padding/4;strip++)
    {
        //accumulator for rhs block
        __m128 acc = _mm_setzero_ps();

        MdtKeaJBlock*        jmstrip = JM;
        const MdtKeaJBlock*  jstrip  = J;
        
        for(block=0; block!=*jlen; block++)
        {
            body=bl2body[strip][block];
            if(body!=-1)
            {
                Multiply46BlockByMBlock(jmstrip, jstrip, invIworld+body);
                acc = Multiply46BlockBy61Vector(acc, jstrip, vhmf+body);
            }
            jmstrip++;
            jstrip++;
        }
        JM   = JM + (*jlen);
        J    = J  + (*jlen);
        jlen = jlen + 1;

        //  Complete the right hand side we are solving for
        //  rhs[i]' = (c[i]-((xgamma[i]+gamma)*x[i])/h)/h - rhs[i].

        __m128 mxg = _mm_load_ps(&xgamma[4*strip]);
        __m128 mxi = _mm_load_ps(&xi[4*strip]);
        __m128 mci = _mm_load_ps(&c[4*strip]);

        _mm_store_ps(&rhs[4*strip] ,_mm_sub_ps(_mm_mul_ps(_mm_sub_ps(mci, _mm_mul_ps(_mm_mul_ps(mxi, _mm_add_ps(mxg, gama)), hinv)), hinv), acc));
    }

#if PRINT_CALCJMINVANDRHS_OUTPUT
    printJinvMandrhsOutput(
        rhs,
        jmstore,
        num_rows_exc_padding, 
        jlen, 
        num_rows_inc_padding);
#endif

#ifndef _NOTIC
    MdtKeaProfileEnd("calcJinvMandRHS");
#endif
}

//***************************************************

__forceinline void Multiply46BlockByMBlock(
                            MdtKeaJBlock*                  jmblock,
                            const MdtKeaJBlock*            jblock,
                            const MdtKeaInverseMassMatrix* invMblock)
{
    __m128 ac1, ac2, ac3, r, sm1, sm2;

    __m128 im0 = _mm_load_ps(invMblock->invI0);
    __m128 ima  = broadcast(im0, 3);

    //first three loners
    _mm_store_ps(jmblock->col[0], _mm_mul_ps(_mm_load_ps(jblock->col[0]), ima));
    _mm_store_ps(jmblock->col[1], _mm_mul_ps(_mm_load_ps(jblock->col[1]), ima));
    _mm_store_ps(jmblock->col[2], _mm_mul_ps(_mm_load_ps(jblock->col[2]), ima));

    //accumulate next three columns
    ac1 = _mm_mul_ps(r =_mm_load_ps(jblock->col[3]), broadcast(im0, 0));
    ac2 = _mm_mul_ps(r, sm1 = broadcast(im0, 1));
    ac3 = _mm_mul_ps(r, sm2 = broadcast(im0, 2));

    __m128 im1 = _mm_load_ps(invMblock->invI1);
    ac1 = _mm_add_ps(ac1, _mm_mul_ps(r =_mm_load_ps(jblock->col[4]), sm1));
    ac2 = _mm_add_ps(ac2, _mm_mul_ps(r, broadcast(im1, 1)));
    ac3 = _mm_add_ps(ac3, _mm_mul_ps(r, sm1 = broadcast(im1, 2)));

    ac1 = _mm_add_ps(ac1, _mm_mul_ps(r =_mm_load_ps(jblock->col[5]), sm2));
    ac2 = _mm_add_ps(ac2, _mm_mul_ps(r, sm1));
    ac3 = _mm_add_ps(ac3, _mm_mul_ps(r, broadcast(_mm_load_ps(invMblock->invI2), 2)));

    //store results
    _mm_store_ps(jmblock->col[3], ac1);
    _mm_store_ps(jmblock->col[4], ac2);
    _mm_store_ps(jmblock->col[5], ac3);

}

__forceinline __m128 Multiply46BlockBy61Vector(
                            __m128                r,
                            const MdtKeaJBlock*   jblock,
                            const MdtKeaVelocity* vmMif)
{
    __m128 v1 = _mm_load_ps(vmMif->velocity);
    __m128 v2 = _mm_load_ps(vmMif->angVelocity);

    r = _mm_add_ps(r, rbcrmulp(_mm_load_ps(jblock->col[0]), v1, 0));
    r = _mm_add_ps(r, rbcrmulp(_mm_load_ps(jblock->col[1]), v1, 1));
    r = _mm_add_ps(r, rbcrmulp(_mm_load_ps(jblock->col[2]), v1, 2));

    r = _mm_add_ps(r, rbcrmulp(_mm_load_ps(jblock->col[3]), v2, 0));
    r = _mm_add_ps(r, rbcrmulp(_mm_load_ps(jblock->col[4]), v2, 1));
    r = _mm_add_ps(r, rbcrmulp(_mm_load_ps(jblock->col[5]), v2, 2));

    return r;
}

//***************************************************
