/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.15.2.2 $

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
#include <MdtKeaProfile.h>
#include "keaFunctions.hpp"

#ifndef _MSC_VER
  #define __forceinline inline
#endif

static void Multiply4VectorWith46Matrix(MeReal                bodyforce[], /* Output */
                                        MdtKeaForce *         conyforce,   /* Output */
                                        const MeReal          lambda[],    /* Input  */
                                        const MdtKeaJBlock *  jstrip);     /* Input  */

/* calculateConstraintForces
 * -------------------------
 *
 * On Entry:
 *
 * blist
 * cforces                        - Empty array of floats
 * Jstore
 * lambda
 * bl2cbody
 * jlen
 * num_rows_exc_padding_partition
 * num_rows_inc_padding_partition
 * num_constraints_partition
 * num_partitions
 *
 * On Exit:
 *
 * blist
 * cforces                        - Empty array of floats
 *
*/
void keaFunctions_Vanilla :: calculateConstraintAndResultantForces(
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
        const MdtKeaJBlock * jstrip;
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
                Multiply4VectorWith46Matrix(&blist[body]->force[0],
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

#define PRINT_DEBUG 0

__forceinline void Multiply4VectorWith46Matrix(MeReal                bodyforce[], /* Output */
                                               MdtKeaForce *         conyforce,   /* Output */
                                               const MeReal          lambda[],    /* Input  */
                                               const MdtKeaJBlock *  jstrip)      /* Input  */
{
    int i;
    MeReal f;

#if PRINT_DEBUG
    printf("Multiply4VectorWith46Matrix\n");
    printf("---------------------------\n");

    printf("lambda\n");

    for(i=0;i!=4;i++) printf("%12.6f ",lambda[i]);printf("\n");

    printf("J\n");
    for(i=0;i!=4;i++) printf("%12.6f ",jstrip->col[0][i]);printf("\n");
    for(i=0;i!=4;i++) printf("%12.6f ",jstrip->col[1][i]);printf("\n");
    for(i=0;i!=4;i++) printf("%12.6f ",jstrip->col[2][i]);printf("\n");
    for(i=0;i!=4;i++) printf("%12.6f ",jstrip->col[3][i]);printf("\n");
    for(i=0;i!=4;i++) printf("%12.6f ",jstrip->col[4][i]);printf("\n");
    for(i=0;i!=4;i++) printf("%12.6f ",jstrip->col[5][i]);printf("\n");
    printf("\n");
#endif    

    for(i=0; i<3; i++)
    {
        f=lambda[0]*jstrip->col[i][0]+
          lambda[1]*jstrip->col[i][1]+
          lambda[2]*jstrip->col[i][2]+
          lambda[3]*jstrip->col[i][3];
        conyforce->force[i]+=f;
        bodyforce[i]+=f;
    }
    bodyforce+=4;
    for(i=0; i<3; i++)
    {
        f=lambda[0]*jstrip->col[i+3][0]+
          lambda[1]*jstrip->col[i+3][1]+
          lambda[2]*jstrip->col[i+3][2]+
          lambda[3]*jstrip->col[i+3][3];
        conyforce->torque[i]+=f;
        bodyforce[i]+=f;
    }

#if PRINT_DEBUG
    printf("constraint force\n");
    printf("%08x\n",conyforce); 
    for(i=0;i!=3;i++) printf("%12.6f ",conyforce->force[i]); printf("\n");
    for(i=0;i!=3;i++) printf("%12.6f ",conyforce->torque[i]);printf("\n");
    printf("\n");
#endif
}

//**********************************************************
