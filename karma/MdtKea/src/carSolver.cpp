/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.3.2.5 $

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
#include <stdio.h>
#include <MePrecision.h>
#include <MdtTypes.h>
#include <KeaInternal.hpp>

#include "carSolver.h"

#ifndef PS2
#ifndef _BUILD_VANILLA

extern "C" void carAddConstraintForces_sse(carSolverConstraintOutput *     constraintOutput, 
                                           carSolverBodyOutput *           bodyOutput,       
                                           carSolverIntermediateResults *  scratch,        
                                           const carSolverConstraintInput* constraintInput,  
                                           const carSolverBodyInput *      bodyInput,        
                                           const carSolverParameters *     params);          
#endif
#endif

/********************************************************************/

/** carAdd4Contacts
 *  ---------------
 *
 *  On Entry:
 *
 *  contacts[0..4).penetration         - penetration for each contact
 *  contacts[0..4).normal              - normal vector for each contact
 *  contacts[0..4).cpos                - position vectori for each contact
 *  contacts[0..4).params.options      - MdtBclContactOptionBounce set if restitution required
 *                                     - MdtBclContactOptionSoft set if softness required
 *  contacts[0..4).params.velThreshold - something to do with restitution
 *  contacts[0..4).params.restitution  - amount of restitution required
 *  contacts[0..4).params.softness     - amount of softness required
 *
 *  On Exit:
 *
 *  constraints.J[ 0.. 4)          - x component of each contact's normal
 *  constraints.J[ 4.. 8)          - y component of each contact's normal
 *  constraints.J[ 8..12)          - z component of each contact's normal
 *  constraints.J[12..16)          - x component of each contact's cross(relative position,normal)
 *  constraints.J[12..16)          - y component of each contact's cross(relative position,normal)
 *  constraints.J[12..16)          - z component of each contact's cross(relative position,normal)
 *  constraints.xi[0..4)           - negative penetration of each contact
 *  constraints.c[0..4)            - nonzero if restitution requested, zero otherwise      
 *  constraints.slipfactor[0..4)   - nonzero if softness requested, zero otherwise     
 *
*/

void carAdd4Contacts(carSolverConstraintInput *  constraints, /* Output */
                     const MdtContactGroup*      contactGroup,/* Input */ 
                     const MeReal                pos[],       /* Input */ 
                     const MeReal                vel[])       /* Input */ 
{
    MeVector3 p;
    int i,j;
    MdtContact *  contacts[4];

    contacts[0] = contactGroup->first;
    contacts[1] = contacts[0]->nextContact;
    contacts[2] = contacts[1]->nextContact;
    contacts[3] = contacts[2]->nextContact;

    for(i=0;i!=4;i++)
    {
        /* Set xi vector 
        */
        constraints->xi[i] = -(contacts[i]->penetration);

        /* Set J matrix
        */
        
        for (j = 0; j != 3; j++)
        {
            constraints->J[i+j*4] = contacts[i]->normal[j];
        }
        
        for (j = 0; j != 3; j++)
        {
            p[j] = contacts[i]->cpos[j] - pos[j];
        }
        
        constraints->J[i+3*4] = p[1] * contacts[i]->normal[2] - p[2] * contacts[i]->normal[1];
        constraints->J[i+4*4] = p[2] * contacts[i]->normal[0] - p[0] * contacts[i]->normal[2];
        constraints->J[i+5*4] = p[0] * contacts[i]->normal[1] - p[1] * contacts[i]->normal[0];
        
        /* If user requested restitution, set c
        */
        
        if(contacts[i]->params.options & MdtBclContactOptionBounce)
        {
            MeReal v = 0;
            
            for (j = 0; j < 6; j++)
            {
                v += constraints->J[i+j*4] * vel[j];
            }
            
            if (v < -(contacts[0]->params.velThreshold))
            {
                constraints->c[i] = -(contacts[i]->params.restitution) * v;
            }
            else
            {
                for(j=0;j!=4;j++) constraints->c[j] = 0.0f;
            }
        }
        else
        {
            for(j=0;j!=4;j++) constraints->c[j] = 0.0f;
        }
        
        /* If user requested softness, set slipfactor
        *
        */
        
        if(contacts[i]->params.options & MdtBclContactOptionSoft)
        {
            constraints->slipfactor[i] = contacts[i]->params.softness;
        }
        else
        {
            constraints->slipfactor[i] = 0.0f;
        }
    }
}

/********************************************************************/

void carAddConstraintForces(carSolverConstraintOutput *     constraintOutput,  /* Output */
                            carSolverBodyOutput *           bodyOutput,        /* Output */
                            carSolverIntermediateResults *  scratch,           /* Output (just for testing) */
                            const carSolverConstraintInput* constraintInput,   /* Input  */
                            const carSolverBodyInput *      bodyInput,         /* Input  */
                            const carSolverParameters *     params,            /* Input  */
                            const int                       cpu_rec)           /* Input  */
{
    /*  Set up platform/build dependant function table */
    
    typedef void (*pcarSolverFunc)(carSolverConstraintOutput *     constraintOutput, 
                                   carSolverBodyOutput *           bodyOutput,       
                                   carSolverIntermediateResults *  scratch,
                                   const carSolverConstraintInput* constraintInput,  
                                   const carSolverBodyInput *      bodyInput,        
                                   const carSolverParameters *     params);

    pcarSolverFunc   carAddConstraintForcesX;

#ifndef PS2

#ifdef _BUILD_VANILLA
    carAddConstraintForcesX = carAddConstraintForces_vanilla;
#else
    if(cpu_rec == MeSSE)//(MeCPUResources)MeSSE)
        carAddConstraintForcesX = carAddConstraintForces_sse;
    else
        carAddConstraintForcesX = carAddConstraintForces_vanilla;
#endif

#else   //PS2
    carAddConstraintForcesX = carAddConstraintForces_vanilla;
#endif
    
    carAddConstraintForcesX(constraintOutput, 
                            bodyOutput,       
                            scratch,
                            constraintInput,  
                            bodyInput,        
                            params);
}

void carAddConstraintForces_vanilla(carSolverConstraintOutput *     constraintOutput,  /* Output */
                                    carSolverBodyOutput *           bodyOutput,        /* Output */
                                    carSolverIntermediateResults *  scratch,           /* Output (just for testing) */
                                    const carSolverConstraintInput* constraintInput,   /* Input  */
                                    const carSolverBodyInput *      bodyInput,         /* Input  */
                                    const carSolverParameters *     params)            /* Input  */
{
    carCalcVhmf(scratch->vhmf,                /* Output */
                bodyInput->invInertiaAndMass, /* Input  */    
                bodyInput->vel,               /* Input  */    
                bodyInput->velrot,            /* Input  */ 
                bodyInput->bodyForce,         /* Input  */ 
                bodyInput->bodyTorque,        /* Input  */ 
                params->invH);                /* Input  */

    carCalcJMandRHS(scratch->JM,                  /* Output */
                    scratch->rhs,                 /* Output */
                    constraintInput->J,           /* Input  */
                    constraintInput->c,           /* Input  */
                    constraintInput->xi,          /* Input  */
                    bodyInput->invInertiaAndMass, /* Input  */
                    scratch->vhmf,                /* Input  */
                    params->invH,                 /* Input  */
                    params->gammaOverHSquared);   /* Input  */

    carCalcJMJT(scratch->A,                 /* Output */
                scratch->JM,                /* Input  */
                constraintInput->J,         /* Input  */
                constraintInput->slipfactor,/* Input  */
                params->epsilon,            /* Input  */
                params->invH);              /* Input  */

    carLCP(constraintOutput->lambda, /* Output */
           scratch->x,               /* Output */
           scratch->w,               /* Output */
           scratch->PSMA,            /* Output */
           scratch->PSMAchol,        /* Output */
           scratch->PSMb,            /* Output */
           scratch->PSMx,            /* Output */
           &scratch->iterations,     /* Output */
           scratch->A,               /* Input  */
           scratch->rhs,             /* Input  */ 
           params->tol);             /* Input  */

    carCalcConstraintForces(constraintOutput->constraintForce,  /* Output */
                            constraintInput->J,                 /* Input  */
                            constraintOutput->lambda);          /* Input  */

    carCalcResultantForces(bodyOutput->bodyForce,               /* Output */
                           bodyOutput->bodyTorque,              /* Output */
                           bodyInput->bodyForce,                /* Input  */
                           bodyInput->bodyTorque,               /* Input  */
                           constraintOutput->constraintForce);  /* Input  */

    carCalcAccel(bodyOutput->accel,             /* Output */
                 bodyOutput->accelRot,          /* Input  */
                 bodyOutput->bodyForce,         /* Input  */
                 bodyOutput->bodyTorque,        /* Input  */
                 bodyInput->invInertiaAndMass); /* Input  */

}

/****************************************************************************/

void carCalcVhmf(MeReal        vhmf[],      /* Output */
                 const MeReal  invM[],      /* Input  */
                 const MeReal  vel[],       /* Input  */    
                 const MeReal  velrot[],    /* Input  */ 
                 const MeReal  force[],     /* Input  */ 
                 const MeReal  torque[],    /* Input  */ 
                 MeReal        invH)        /* Input  */
{
    int i;

    /* Calc vhmf */

    for(i=0;i!=3;i++) vhmf[i]   = invH*vel[i]    + force[i]*invM[1];
    for(i=0;i!=3;i++) vhmf[i+4] = invH*velrot[i] + torque[i]*invM[0];
}

void carCalcJMandRHS(MeReal        JM[],              /* Output */
                     MeReal        rhs[],             /* Output */
                     const MeReal  J[],               /* Input */
                     const MeReal  c[],               /* Input */
                     const MeReal  xi[],              /* Input */
                     const MeReal  invM[],            /* Input */
                     const MeReal  vhmf[],            /* Input */
                     MeReal        invH,              /* Input */
                     MeReal        gammaOverHSquared) /* Input */
{
    int i;

    for(i=0; i!=4; i++) JM[0*4+i] = J[0*4+i] * invM[1];
    for(i=0; i!=4; i++) JM[1*4+i] = J[1*4+i] * invM[1];
    for(i=0; i!=4; i++) JM[2*4+i] = J[2*4+i] * invM[1];
    for(i=0; i!=4; i++) JM[3*4+i] = J[3*4+i] * invM[0];
    for(i=0; i!=4; i++) JM[4*4+i] = J[4*4+i] * invM[0];
    for(i=0; i!=4; i++) JM[5*4+i] = J[5*4+i] * invM[0];

    for(i=0; i!=4; i++) rhs[i] = c[i]*invH;
    for(i=0; i!=4; i++) rhs[i] = rhs[i] - xi[i]*gammaOverHSquared;

    for(i=0; i!=4; i++) rhs[i] = rhs[i] - J[0*4+i] * vhmf[0];
    for(i=0; i!=4; i++) rhs[i] = rhs[i] - J[1*4+i] * vhmf[1];
    for(i=0; i!=4; i++) rhs[i] = rhs[i] - J[2*4+i] * vhmf[2];
    for(i=0; i!=4; i++) rhs[i] = rhs[i] - J[3*4+i] * vhmf[4];
    for(i=0; i!=4; i++) rhs[i] = rhs[i] - J[4*4+i] * vhmf[5];
    for(i=0; i!=4; i++) rhs[i] = rhs[i] - J[5*4+i] * vhmf[6];
    
}
/**
 *
 *  A:=JM * transpose(J)
 *
*/
void carCalcJMJT(MeReal        A[],          /* Output */
                 const MeReal  JM[],         /* Input */
                 const MeReal  J[],          /* Input */
                 const MeReal  slipfactor[], /* Input */
                 MeReal        epsilon,      /* Input */
                 MeReal        invH          /* Input */
                )
{
    int k;
    MeReal acc[4];

    for(k=0;k!=4;k++) acc[k]   =          JM[0*4+k]*J[0*4+0];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[1*4+k]*J[1*4+0];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[2*4+k]*J[2*4+0];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[3*4+k]*J[3*4+0];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[4*4+k]*J[4*4+0];
    for(k=0;k!=4;k++) A[0*4+k] = acc[k] + JM[5*4+k]*J[5*4+0];

    for(k=0;k!=4;k++) acc[k]   =          JM[0*4+k]*J[0*4+1];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[1*4+k]*J[1*4+1];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[2*4+k]*J[2*4+1];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[3*4+k]*J[3*4+1];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[4*4+k]*J[4*4+1];
    for(k=0;k!=4;k++) A[1*4+k] = acc[k] + JM[5*4+k]*J[5*4+1];

    for(k=0;k!=4;k++) acc[k]   =          JM[0*4+k]*J[0*4+2];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[1*4+k]*J[1*4+2];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[2*4+k]*J[2*4+2];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[3*4+k]*J[3*4+2];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[4*4+k]*J[4*4+2];
    for(k=0;k!=4;k++) A[2*4+k] = acc[k] + JM[5*4+k]*J[5*4+2];

    for(k=0;k!=4;k++) acc[k]   =          JM[0*4+k]*J[0*4+3];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[1*4+k]*J[1*4+3];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[2*4+k]*J[2*4+3];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[3*4+k]*J[3*4+3];
    for(k=0;k!=4;k++) acc[k]   = acc[k] + JM[4*4+k]*J[4*4+3];
    for(k=0;k!=4;k++) A[3*4+k] = acc[k] + JM[5*4+k]*J[5*4+3];

    A[0*4+0] += epsilon + invH * slipfactor[0];
    A[1*4+1] += epsilon + invH * slipfactor[1];
    A[2*4+2] += epsilon + invH * slipfactor[2];
    A[3*4+3] += epsilon + invH * slipfactor[3];
}
/**
 *  Cholesky factorise the A matrix
*/
void carFactorise(MeReal        Achol[],   /* Output */
                  const MeReal  A[])       /* Input */
{
    MeReal Q;
    int i;

#if PRINT_DEBUG_INFO
    printMat44(A,"PSMA");
#endif

    for(i=0;i!=16;i++) Achol[i]=A[i];

    /* Column 0 */

    if(Achol[0+0*4]<0.0f) printf("error: sqrt of negative %12.6f\n",Achol[0+0*4]);
    Q = MeRecipSqrt(Achol[0+0*4]);
    for(i=0;i!=4;i++) Achol[i+0*4] *=Q;

    /* Column 1 */

    for(i=1;i!=4;i++) Achol[i+1*4] -= Achol[i+0*4] * Achol[1+0*4];
    if(Achol[1+1*4]<0.0f) printf("error: sqrt of negative %12.6f\n",Achol[1+1*4]);
    Q = MeRecipSqrt(Achol[1+1*4]);
    for(i=1;i!=4;i++) Achol[i+1*4] *=Q;
    
    /* Column 2 */
    
    for(i=2;i!=4;i++) Achol[i+2*4] = Achol[i+2*4] - 
                                     Achol[i+0*4]*Achol[2+0*4] - 
                                     Achol[i+1*4]*Achol[2+1*4];			
    if(Achol[2+2*4]<0.0f) printf("error: sqrt of negative %12.6f\n",Achol[2+2*4]);
    Q = MeRecipSqrt(Achol[2+2*4]);
    for(i=2;i!=4;i++) Achol[i+2*4] *=Q;
    
    /* Column 3 */

    Achol[3+3*4] = Achol[3+3*4] - 
                   Achol[3+0*4]*Achol[3+0*4] - 
                   Achol[3+1*4]*Achol[3+1*4] - 
                   Achol[3+2*4]*Achol[3+2*4];
    if(Achol[3+3*4]<0.0f) printf("error: sqrt of negative %12.6f\n",Achol[3+3*4]);
    Q = MeRecipSqrt(Achol[3+3*4]);
    Achol[3+3*4] *=Q;
    
}

void copyVec4(MeReal       a[], /* Output */
              const MeReal b[]) /* Input  */
{
    int i;
    for(i=0;i!=4;i++)
    {
        a[i]=b[i];
    }
}

/**
 *  LCP solver
*/

void carLCP(MeReal       lambda[],   /* Output */
            MeReal       x[],        /* Output */
            MeReal       w[],        /* Output */
            MeReal       PSMA[],     /* Output */
            MeReal       PSMAchol[], /* Output */
            MeReal       PSMb[],     /* Output */
            MeReal       PSMx[],     /* Output */
            int *        iterations, /* Output */
            const MeReal A[],        /* Input  */
            const MeReal rhs[],      /* Input  */
            MeReal       tol)        /* Input  */
{
    int solved;
    int iteration;

    iteration = 0;
    solved    = 0;

    while(!solved && iteration!=16)
    {
        carCalcXandW(x + iteration*4,                 /* Output */
                     w + iteration*4,                 /* Output */
                     PSMA + iteration*16,             /* Output */
                     PSMAchol + iteration*16,         /* Output */
                     PSMb + iteration*4,              /* Output */
                     PSMx + iteration*4,              /* Output */
                     A,                               /* Input */
                     rhs,                             /* Input */
                     unclampedIndexSet + iteration*4, /* Input */
                     numUnclamped[iteration]);        /* Input */

        solved    = carCheckLCPSolution(x + iteration*4,  /* Input */
                                        w + iteration*4,  /* Input */
                                        tol);             /* Input */
        iteration = iteration + 1;
    }

    (*iterations) = iteration;
    copyVec4(lambda, x+(iteration-1)*4);

    if(!solved)
    {
        printf("Error - LCP solution not found\n");
    }
}

void carCalcXandW(MeReal        x[],              /* Output */
                  MeReal        w[],              /* Output */
                  MeReal        PSMA[],           /* Output */
                  MeReal        PSMAchol[],       /* Output */
                  MeReal        PSMb[],           /* Output */
                  MeReal        PSMx[],           /* Output */
                  const MeReal  A[],              /* Input */
                  const MeReal  b[],              /* Input */
                  const int     unclampedIndex[], /* Input */
                  int           numUnClamped)     /* Input */
{
    carPSM(PSMA,           /* Output */
           PSMb,           /* Output */
           A,              /* Input  */
           b,              /* Input  */
           unclampedIndex, /* Input  */
           numUnClamped);  /* Input  */

    carFactorise(PSMAchol,PSMA);

    carSolve(PSMx,          /* Output */  
             PSMAchol,      /* Input  */
             PSMb,          /* Input  */
             numUnClamped); /* Input  */

    carUnPSM(x,               /* Output */
             PSMx,            /* Input  */
             unclampedIndex,  /* Input  */
             numUnClamped);   /* Input  */

    carCalcW(w,  /* Output */
             A,  /* Input */
             x,  /* Input */
             b); /* Input */

}
void carPSM(MeReal        PSMAchol[],        /* Output */
            MeReal        PSMb[],            /* Output */
            const MeReal  Achol[],           /* Input  */
            const MeReal  b[],               /* Input  */
            const int     unclampedIndex[],  /* Input  */
            int           numUnClamped)      /* Input  */
{
    int i,j;

    for(i=0;i!=16;i++) PSMAchol[i] = 0;
    PSMAchol[0+0*4] = 1.0f;
    PSMAchol[1+1*4] = 1.0f;
    PSMAchol[2+2*4] = 1.0f;
    PSMAchol[3+3*4] = 1.0f;

    for(i=0;i!=numUnClamped;i++)
    {
        for(j=0;j!=numUnClamped;j++)
        {
            PSMAchol[i*4+j] = Achol[unclampedIndex[i]*4 + unclampedIndex[j]];
        }
        PSMb[i] = b[unclampedIndex[i]];
    }
}
void carSolve(MeReal        x[],     /* Output */
              const MeReal  Achol[], /* Input */
              const MeReal  b[],     /* Input */
              int           n)       /* Input */
{
    MeReal Q;
    MeReal ACC[4];
    MeReal t[4];
    int i;

    x[0] = b[0];
    x[1] = b[1];
    x[2] = b[2];
    x[3] = b[3];

    if(n==4)
    {
        //** Lower solve
        
        Q = MeRecip(Achol[0+0*4]);
        x[0] = x[0] * Q;
        
        Q = MeRecip(Achol[1+1*4]);
        for(i=0;i!=4;i++) ACC[i] = Achol[i+0*4] * x[0];
        x[1] = x[1] - ACC[1];
        x[1] = x[1] * Q;
        
        Q = MeRecip(Achol[2+2*4]);    
        for(i=0;i!=4;i++) ACC[i] = ACC[i] + Achol[i+1*4] * x[1];
        x[2] = x[2] - ACC[2];
        x[2] = x[2] * Q;
        
        Q = MeRecip(Achol[3+3*4]);
        for(i=0;i!=4;i++) ACC[i] = ACC[i] + Achol[i+2*4] * x[2];
        x[3] = x[3] - ACC[3];
        x[3] = x[3] * Q;

        //** Upper solve

        Q       = MeRecip(Achol[3+3*4]);
        x[3]    = x[3] * Q;
        
        Q       = MeRecip(Achol[2+2*4]);
        t[3]    = Achol[3+2*4] * x[3];
        ACC[2]  = x[2];
        x[2]    = ACC[2] - t[3];
        x[2]    = x[2] * Q;
        
        Q       = MeRecip(Achol[1+1*4]);
        for(i=2;i!=4;i++) t[i] = Achol[i+1*4] * x[i];  
        ACC[1]  = x[1];
        ACC[1]  = ACC[1] - t[2]; 
        x[1]    = ACC[1] - t[3]; 
        x[1]    = x[1] * Q;
        
        Q       = MeRecip(Achol[0+0*4]);
        for(i=1;i!=4;i++) t[i] = Achol[i+0*4] * x[i];  
        ACC[0]  = x[0];
        ACC[0]  = ACC[0] - t[1];
        ACC[0]  = ACC[0] - t[2];
        x[0]    = ACC[0] - t[3];
        x[0]    = x[0] * Q;        
    }
    else if(n==3)
    {
        //** Lower solve
        
        Q = MeRecip(Achol[0+0*4]);
        x[0] = x[0] * Q;
        
        Q = MeRecip(Achol[1+1*4]);
        for(i=0;i!=4;i++) ACC[i] = Achol[i+0*4] * x[0];
        x[1] = x[1] - ACC[1];
        x[1] = x[1] * Q;
        
        Q = MeRecip(Achol[2+2*4]);    
        for(i=0;i!=4;i++) ACC[i] = ACC[i] + Achol[i+1*4] * x[1];
        x[2] = x[2] - ACC[2];
        x[2] = x[2] * Q;

        //** Upper solve
                
        Q       = MeRecip(Achol[2+2*4]);
        x[2]    = x[2] * Q;
        
        Q       = MeRecip(Achol[1+1*4]);
        t[2]    = Achol[2+1*4] * x[2];  
        ACC[1]  = x[1];
        x[1]    = ACC[1] - t[2]; 
        x[1]    = x[1] * Q;
        
        Q       = MeRecip(Achol[0+0*4]);
        for(i=1;i!=3;i++) t[i] = Achol[i+0*4] * x[i];  
        ACC[0]  = x[0];
        ACC[0]  = ACC[0] - t[1];
        x[0]    = ACC[0] - t[2];
        x[0]    = x[0] * Q;                
    }
    else if(n==2)
    {
        //** Lower solve
        
        Q = MeRecip(Achol[0+0*4]);
        x[0] = x[0] * Q;
        
        Q = MeRecip(Achol[1+1*4]);
        for(i=0;i!=4;i++) ACC[i] = Achol[i+0*4] * x[0];
        x[1] = x[1] - ACC[1];
        x[1] = x[1] * Q;

        //** Upper solve
                        
        Q       = MeRecip(Achol[1+1*4]);
        x[1]    = x[1] * Q;
        
        Q       = MeRecip(Achol[0+0*4]);
        t[1]    = Achol[1+0*4] * x[1];  
        ACC[0]  = x[0];
        x[0]    = ACC[0] - t[1];
        x[0]    = x[0] * Q;                
                
    }
    else if(n==1)
    {
        //** Lower solve
        
        Q       = MeRecip(Achol[0+0*4]);
        x[0]    = x[0] * Q;

        //** Upper solve
                                
        Q       = MeRecip(Achol[0+0*4]);
        x[0]    = x[0] * Q;                
                
    }
}
void carUnPSM(MeReal        x[],              /* Output */
              const MeReal  PSMx[],           /* Input  */
              const int     unclampedIndex[], /* Input  */
              int           numUnClamped)     /* Input  */
{
    int i;

    for(i=0;i!=4;i++) x[i] = 0.0f;

    for(i=0;i!=numUnClamped;i++)
    {
        x[unclampedIndex[i]] = PSMx[i];
    }
}

void carCalcW(MeReal       w[],  /* Output */
              const MeReal A[],  /* Input  */
              const MeReal x[],  /* Input  */
              const MeReal b[])  /* Input  */
{
    MeReal acc[4];
    int j;

    for(j=0;j!=4;j++) acc[j] =          A[0*4+j]*x[0];
    for(j=0;j!=4;j++) acc[j] = acc[j] + A[1*4+j]*x[1];
    for(j=0;j!=4;j++) acc[j] = acc[j] + A[2*4+j]*x[2];
    for(j=0;j!=4;j++) acc[j] = acc[j] + A[3*4+j]*x[3];

    for(j=0;j!=4;j++) w[j]   = acc[j] - b[j];
}

int carCheckLCPSolution(const MeReal x[],  /* Input */
                        const MeReal w[],  /* Input */
                        MeReal       tol)  /* Input */
{
    int i;

    for(i=0;i!=4;i++)
    {
        if(x[i]<-tol) 
        {
#if PRINT_DEBUG_INFO
            printf("x[%d]=% 014.6f\n",i,x[i]);
#endif
            return 0;
        }
    }

    for(i=0;i!=4;i++)
    {
        if(w[i]<-tol) 
        {
#if PRINT_DEBUG_INFO
            printf("w[%d]=% 014.6f\n",i,w[i]);
#endif
            return 0;
        }
    }

    return 1;
}

/**
 *
 *  Calculate the constraint force to apply to maintain each of the 4 constraints
 *
 */
void carCalcConstraintForces(MeReal        constraintForce[], /* Output */
                             const MeReal  J[],               /* Input  */
                             const MeReal  lambda[])          /* Input  */
{
    int i,j;

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=3;j++) constraintForce[i*8+j]   = J[j*4+i]     * lambda[i];
        for(j=0;j!=3;j++) constraintForce[i*8+j+4] = J[(j+3)*4+i] * lambda[i];
    }
}
/**
 *  Add together the 4 constraint forces to get the body force
 *
*/
void carCalcResultantForces(MeReal        bodyForce[],       /* Output */
                            MeReal        bodyTorque[],      /* Output */
                            const MeReal  externalForce[],   /* Input  */
                            const MeReal  externalTorque[],  /* Input  */
                            const MeReal  constraintForce[]) /* Input  */
{
    int j;

    for(j=0;j!=3;j++) bodyForce[j]  = externalForce[j];
    for(j=0;j!=3;j++) bodyForce[j]  = bodyForce[j] + constraintForce[0*8+j];
    for(j=0;j!=3;j++) bodyForce[j]  = bodyForce[j] + constraintForce[1*8+j];
    for(j=0;j!=3;j++) bodyForce[j]  = bodyForce[j] + constraintForce[2*8+j];
    for(j=0;j!=3;j++) bodyForce[j]  = bodyForce[j] + constraintForce[3*8+j];

    for(j=0;j!=3;j++) bodyTorque[j] = externalTorque[j];
    for(j=0;j!=3;j++) bodyTorque[j] = bodyTorque[j] + constraintForce[0*8+4+j];
    for(j=0;j!=3;j++) bodyTorque[j] = bodyTorque[j] + constraintForce[1*8+4+j];
    for(j=0;j!=3;j++) bodyTorque[j] = bodyTorque[j] + constraintForce[2*8+4+j];
    for(j=0;j!=3;j++) bodyTorque[j] = bodyTorque[j] + constraintForce[3*8+4+j];
}

/** 
 *
 *  acceleration := inverse(mass) * force
*/
void carCalcAccel(MeReal        accel[],      /* Output */
                  MeReal        accelRot[],   /* Input  */
                  const MeReal  bodyForce[],  /* Input  */
                  const MeReal  bodyTorque[], /* Input  */
                  const MeReal  invM[])       /* Input  */
{
    int j;
    
    for(j=0; j<3; j++) accel[j]    = bodyForce[j]  * invM[1];        
    for(j=0; j<3; j++) accelRot[j] = bodyTorque[j] * invM[0];
}

/********************************************************************/
