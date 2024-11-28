/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.16.2.1 $

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
#include "keaInternal.hpp"
#include <MdtKea.h>

#ifndef _MSC_VER
  #define __forceinline inline
#endif

static void Multiply46BlockByMBlock(
                MdtKeaJBlock *                   jmblock,   /* Output */
                const MdtKeaJBlock *             jblock,    /* Input */
                const MdtKeaInverseMassMatrix *  invMblock);/* Input */

static void Multiply46BlockBy61Vector(
                MeReal *                arhs,   /* Output */
                const MdtKeaJBlock *    jblock, /* Input */
                const MdtKeaVelocity *  vmMif); /* Input */

/** keaFunctions_Vanilla::calcJinvMandRHS
 *  -------------------------------------
 *  On Entry:
 *
 *  rhs                       - empty array of ceil4_num_rows MeReals for this function to put the rhs in         
 *  jm                        - empty array of 12*ceil4_num_rows MeReals for this function to put jm in          
 *  constraints->Jstore       - concrete J matrix (array of ?? elements)
 *  invIworld                 - concrete M matrix (array of type MdtKeaInverseMassMatrix, has 'num_bodies' elements)   
 *  bl2body                   - 'block to body' mapping made by makejlenandbl2body - array of 8* ceil4_num_rows/4 ints    
 *  jlen                      - 'strip number to strip length' mapping made by makejlenandbl2body - array of ceil4_num_rows/4 ints       
 *  vhmf                      - 'v/h + inverse(M)*fe' vector made by calcInvIworld (used only in rhs calculation) - array of 6* num_bodies MeReals        
 *  num_bodies                - the number of bodies    
 *  ceil4_num_rows            - the number of rows in the abstract J matrix and the rhs vector        
 *  params->stepsize          - the stepsize (h)
 *  params->gamma             - the projection rate (gamma)
 *
 *  On Exit:
 *
 *  rhs[0..ceil4_num_rows)   - the rhs vector
 *  jm[0..12*ceil4_num_rows) - the jm matrix
 *
 */
void keaFunctions_Vanilla::calcJinvMandRHS(
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

    MeReal *              ptrhs     = rhs;
    const MdtKeaJBlock *  J         = (MdtKeaJBlock *)jstore;
    MdtKeaJBlock *        JM        = (MdtKeaJBlock *)jmstore;
    const MeReal *        xgammaptr = xgamma;
    const MeReal *        cptr      = c;
    const MeReal *        xiptr     = xi;
    MeReal                hinv      = MeRecip(stepsize);

    for(strip=0;strip!=num_rows_exc_padding/4;strip++)
    {
        MdtKeaJBlock *        jmstrip = JM;
        const MdtKeaJBlock *  jstrip  = J;

        //zero rhs accum
        ptrhs[0]=ptrhs[1]=ptrhs[2]=ptrhs[3]=0.0f;
        //make jm blocks and build rhs
        for(block=0; block!=jlen[strip]; block++)
        {
            body=bl2body[strip][block];
            if(body!=-1)
            {
                Multiply46BlockByMBlock(jmstrip, jstrip, invIworld+body);
                Multiply46BlockBy61Vector(ptrhs, jstrip, vhmf+body);
            }
            jmstrip++;
            jstrip++;
        }
        JM+=jlen[strip];
        J+=jlen[strip];

        //  Complete the right hand side we are solving for, set:
        //  rhs' = c[i]/h - (xgamma[i]+gamma)*x[i]/(h^2) - rhs;

        ptrhs[0]=(cptr[0]-(xgammaptr[0]+gamma)*xiptr[0]*hinv)*hinv-ptrhs[0];
        ptrhs[1]=(cptr[1]-(xgammaptr[1]+gamma)*xiptr[1]*hinv)*hinv-ptrhs[1];
        ptrhs[2]=(cptr[2]-(xgammaptr[2]+gamma)*xiptr[2]*hinv)*hinv-ptrhs[2];
        ptrhs[3]=(cptr[3]-(xgammaptr[3]+gamma)*xiptr[3]*hinv)*hinv-ptrhs[3];
        ptrhs+=4;
        cptr+=4;
        xiptr+=4;
        xgammaptr+=4;
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
                       MdtKeaJBlock *                   jmblock,   /* Output */
                       const MdtKeaJBlock *             jblock,    /* Input */
                       const MdtKeaInverseMassMatrix *  invMblock) /* Input */
{
    //first three loners
    for(int i=0; i<3; i++)
    {
        jmblock->col[i][0]=jblock->col[i][0]*invMblock->invmass;
        jmblock->col[i][1]=jblock->col[i][1]*invMblock->invmass;
        jmblock->col[i][2]=jblock->col[i][2]*invMblock->invmass;
        jmblock->col[i][3]=jblock->col[i][3]*invMblock->invmass;
    }
    for(int j=0; j<3; j++)
    {
        MeReal tmp[4];
        tmp[0]=tmp[1]=tmp[2]=tmp[3]=0.0f;

        tmp[0]+=jblock->col[3][0]*invMblock->invI0[j];
        tmp[1]+=jblock->col[3][1]*invMblock->invI0[j];
        tmp[2]+=jblock->col[3][2]*invMblock->invI0[j];
        tmp[3]+=jblock->col[3][3]*invMblock->invI0[j];

        tmp[0]+=jblock->col[4][0]*invMblock->invI1[j];
        tmp[1]+=jblock->col[4][1]*invMblock->invI1[j];
        tmp[2]+=jblock->col[4][2]*invMblock->invI1[j];
        tmp[3]+=jblock->col[4][3]*invMblock->invI1[j];

        tmp[0]+=jblock->col[5][0]*invMblock->invI2[j];
        tmp[1]+=jblock->col[5][1]*invMblock->invI2[j];
        tmp[2]+=jblock->col[5][2]*invMblock->invI2[j];
        tmp[3]+=jblock->col[5][3]*invMblock->invI2[j];

        jmblock->col[j+3][0]=tmp[0];
        jmblock->col[j+3][1]=tmp[1];
        jmblock->col[j+3][2]=tmp[2];
        jmblock->col[j+3][3]=tmp[3];
    }
}
__forceinline void Multiply46BlockBy61Vector(
                       MeReal *                arhs,   /* Output */
                       const MdtKeaJBlock *    jblock, /* Input */
                       const MdtKeaVelocity *  vmMif)  /* Input */
{
    int i;
    MeReal tmp[4];
    tmp[0]=tmp[1]=tmp[2]=tmp[3]=0.0f;
    for(i=0; i<3; i++)
    {
        tmp[0]+=jblock->col[i][0]*vmMif->velocity[i];
        tmp[1]+=jblock->col[i][1]*vmMif->velocity[i];
        tmp[2]+=jblock->col[i][2]*vmMif->velocity[i];
        tmp[3]+=jblock->col[i][3]*vmMif->velocity[i];
    }
    for(i=0; i<3; i++)
    {
        tmp[0]+=jblock->col[i+3][0]*vmMif->angVelocity[i];
        tmp[1]+=jblock->col[i+3][1]*vmMif->angVelocity[i];
        tmp[2]+=jblock->col[i+3][2]*vmMif->angVelocity[i];
        tmp[3]+=jblock->col[i+3][3]*vmMif->angVelocity[i];
    }
    arhs[0]+=tmp[0];
    arhs[1]+=tmp[1];
    arhs[2]+=tmp[2];
    arhs[3]+=tmp[3];
}

/*#####################################################
######################################################
#####################################################*/
