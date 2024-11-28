/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.1.2.4 $

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

#include "carSolver_pc.h"

#ifndef _BUILD_VANILLA

void carAddConstraintForces_sse(carSolverConstraintOutput *           constraintOutput,  /* Output */
                                carSolverBodyOutput *                 bodyOutput,        /* Output */
                                carSolverIntermediateResults *        scratch,           /* Output (just for testing) */
                                const carSolverConstraintInput *      constraintInput,   /* Input  */
                                const carSolverBodyInput *            bodyInput,         /* Input  */
                                const carSolverParameters *           params)            /* Input  */
{
    carCalcVhmf_SSE(scratch->vhmf,                /* Output */
                    bodyInput->invInertiaAndMass, /* Input  */    
                    bodyInput->vel,               /* Input  */    
                    bodyInput->velrot,            /* Input  */ 
                    bodyInput->bodyForce,         /* Input  */ 
                    bodyInput->bodyTorque,        /* Input  */ 
                    params->invH);                /* Input  */
    
    carCalcJMandRHS_SSE(scratch->JM,                  /* Output */
                        scratch->rhs,                 /* Output */
                        constraintInput->J,           /* Input  */
                        constraintInput->c,           /* Input  */
                        constraintInput->xi,          /* Input  */
                        bodyInput->invInertiaAndMass, /* Input  */
                        scratch->vhmf,                /* Input  */
                        params->invH,                 /* Input  */
                        params->gammaOverHSquared);   /* Input  */
    
    carCalcJMJT_SSE(scratch->A,                 /* Output */
                    scratch->JM,                /* Input  */
                    constraintInput->J,         /* Input  */
                    constraintInput->slipfactor,/* Input  */
                    params->epsilon,            /* Input  */
                    params->invH);              /* Input  */
    
    carLCP_SSE(constraintOutput->lambda, /* Output */
               scratch->x,               /* Output */
               scratch->w,               /* Output */
               scratch->PSMA,            /* Output */
               scratch->PSMAchol,        /* Output */
               scratch->PSMb,            /* Output */
               scratch->PSMx,            /* Output */
               &(scratch->iterations),   /* Output */
               scratch->A,               /* Input  */
               scratch->rhs,             /* Input  */ 
               params->tol);             /* Input  */
    
    carCalcConstraintForces_SSE(constraintOutput->constraintForce,  /* Output */
                                constraintInput->J,                  /* Input  */
                                constraintOutput->lambda);          /* Input  */
    
    carCalcResultantForces_SSE(bodyOutput->bodyForce,               /* Output */
                               bodyOutput->bodyTorque,              /* Output */
                               bodyInput->bodyForce,                 /* Input  */
                               bodyInput->bodyTorque,                /* Input  */
                               constraintOutput->constraintForce);  /* Input  */
    
    carCalcAccel_SSE(bodyOutput->accel,            /* Output */
                     bodyOutput->accelRot,         /* Input  */
                     bodyOutput->bodyForce,        /* Input  */
                     bodyOutput->bodyTorque,       /* Input  */
                     bodyInput->invInertiaAndMass);/* Input  */
}

/********************************************************************/

__forceinline void carCalcVhmf_SSE(MeReal    vhmf[],      /* Output */
                               const MeReal  invM[],      /* Input  */
                               const MeReal  vel[],       /* Input  */    
                               const MeReal  velrot[],    /* Input  */ 
                               const MeReal  force[],     /* Input  */ 
                               const MeReal  torque[],    /* Input  */ 
                               MeReal        invH)        /* Input  */
{
    __m128 f,t,v,vr,ih;

    f   = _mm_load_ps(&force[0]);
    t   = _mm_load_ps(&torque[0]);
    v   = _mm_load_ps(&vel[0]);
    vr  = _mm_load_ps(&velrot[0]);

    ih  = spread(&invH);

    _mm_store_ps(&vhmf[0], _mm_add_ps(_mm_mul_ps(ih, v),  rbcmulp(f, invM[1])));
    _mm_store_ps(&vhmf[4], _mm_add_ps(_mm_mul_ps(ih, vr), rbcmulp(t, invM[0])));

}

__forceinline void carCalcJMandRHS_SSE(MeReal        JM[],              /* Output */
                                   MeReal        rhs[],             /* Output */
                                   const MeReal  J[],               /* Input  */
                                   const MeReal  c[],               /* Input  */
                                   const MeReal  xi[],              /* Input  */
                                   const MeReal  invM[],            /* Input  */
                                   const MeReal  vhmf[],            /* Input  */
                                   MeReal        invH,              /* Input  */
                                   MeReal        gammaOverHSquared) /* Input  */
{
    int i;

    __m128 im0,im1;

    im0 = spread(&invM[0]);
    im1 = spread(&invM[1]);

    for(i=0; i<3;i++)
        _mm_store_ps(&JM[i*4], _mm_mul_ps(_mm_load_ps(&J[i*4]), im1));

    for(i=3; i<6;i++)
        _mm_store_ps(&JM[i*4], _mm_mul_ps(_mm_load_ps(&J[i*4]), im0));

    __m128 r,d;

    r = abcmulp(c, invH);
    d = abcmulp(xi, gammaOverHSquared);

    r = _mm_sub_ps(r, d);
    
    for(i=0; i<3;i++)
        r = _mm_sub_ps(r, abcmulp(&J[i*4], vhmf[i]));

    for(i=3; i<6;i++)
        r = _mm_sub_ps(r, abcmulp(&J[i*4], vhmf[i+1]));
    
    _mm_store_ps(&rhs[0], r);

}

/**
 *
 *  A:=JM * transpose(J)
 *
*/
__forceinline void carCalcJMJT_SSE(MeReal        A[],          /* Output */
                                   const MeReal  JM[],         /* Input  */
                                   const MeReal  J[],          /* Input  */
                                   const MeReal  slipfactor[], /* Input  */
                                   MeReal        epsilon,      /* Input  */
                                   MeReal        invH)         /* Input  */
{
    int k;

    __m128 da = rbcaddp(abcmulp(&slipfactor[0], invH), epsilon);

    for(k=0; k<4;k++)
    {
        __m128 ac = _mm_setzero_ps();
        __m128 msk= _mm_load_ps((float*)MASK_ID[k]);
        //for(int h=0; h<6;h++)
            //ac = _mm_add_ps(ac, abcmulp(&JM[h*4], J[h*4+k]));
            ac = _mm_add_ps(ac, abcmulp(&JM[0*4], J[0*4+k]));
            ac = _mm_add_ps(ac, abcmulp(&JM[1*4], J[1*4+k]));
            ac = _mm_add_ps(ac, abcmulp(&JM[2*4], J[2*4+k]));
            ac = _mm_add_ps(ac, abcmulp(&JM[3*4], J[3*4+k]));
            ac = _mm_add_ps(ac, abcmulp(&JM[4*4], J[4*4+k]));
            ac = _mm_add_ps(ac, abcmulp(&JM[5*4], J[5*4+k]));
        ac = _mm_add_ps(ac, _mm_and_ps(da, msk));
        _mm_store_ps(&A[k*4], ac);
    }

}
/**
 *  Cholesky factorise the A matrix
*/
void carFactorise_SSE(MeReal        Achol[],   /* Output */
                  MeReal        Drsrt[],   /* Output */
                  const MeReal  A[],       /* Input  */
                  const int     n)         /* Input  */
{
    __m128 r0,r1,r2,r3;
    __m128 r4,r5;

    r0 = _mm_load_ps(&A[0]);
    r1 = _mm_load_ps(&A[4]);
    r2 = _mm_load_ps(&A[8]);
    r3 = _mm_load_ps(&A[12]);

    //r0
    r0 = _mm_mul_ps(r0, r5 = rcpsqrt_nr_ps(_mm_shuffle_ps(r0,r0, bcast(0))));
    _mm_store_ss(&Drsrt[0], r5);
    //r1
    if(n>=2) {
    r1 = _mm_sub_ps(r1, rbcrmulp(r0, r0, 1));
    r1 = _mm_mul_ps(r1, r5 = rcpsqrt_nr_ps(_mm_shuffle_ps(r1,r1, bcast(1))));
    _mm_store_ss(&Drsrt[1], r5);
    }
    //r2
    if(n>=3) {
    r4 = _mm_movehl_ps(r1, r0);                         //abcd
    r5 = _mm_shuffle_ps(r4, r4, _MM_SHUFFLE(2,2,0,0));  //aacc
    r4 = _mm_mul_ps(r4, r5);
    r2 = _mm_sub_ps(r2, _mm_add_ps(r4, _mm_movelh_ps(r4, r4)));
    r2 = _mm_mul_ps(r2, r5 = rcpsqrt_nr_ps(_mm_shuffle_ps(r2,r2, bcast(2))));
    _mm_store_ss(&Drsrt[2], r5);
    }
    //r3
    if(n==4) {
    r4 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(3,0,3,0));
    r4 = _mm_mul_ps(r4, r4);
    r3 = _mm_sub_ps(r3, _mm_add_ps(_mm_mul_ps(r2, r2), _mm_add_ps(r4, _mm_movelh_ps(r4, r4))));
    r3 = _mm_mul_ps(r3, r5 = rcpsqrt_nr_ps(_mm_shuffle_ps(r3,r3, bcast(3))));
    _mm_store_ss(&Drsrt[3], r5);
    }
    
    _mm_store_ps(&Achol[0], r0);
    _mm_store_ps(&Achol[4], r1);
    _mm_store_ps(&Achol[8], r2);
    _mm_store_ps(&Achol[12],r3);

}
 
__forceinline void copyVec4_SSE(MeReal       a[], /* Output */
                                const MeReal b[]) /* Input  */
{
    acopy(a,b);
}

/**
 *  LCP solver
*/

__forceinline void carLCP_SSE(MeReal       lambda[],   /* Output */
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
    int notsolved;
    int iteration;

    iteration = 0;
    notsolved = 1;

    while(notsolved && iteration!=16)
    {
        carCalcXandW_SSE(x + iteration*4,                 /* Output */
                     w + iteration*4,                 /* Output */
                     PSMA + iteration*16,             /* Output */
                     PSMAchol + iteration*16,         /* Output */
                     PSMb + iteration*4,              /* Output */
                     PSMx + iteration*4,              /* Output */
                     A,                               /* Input  */
                     rhs,                             /* Input  */
                     unclampedIndexSet + iteration*4, /* Input  */
                     numUnclamped[iteration]);        /* Input  */

        notsolved  = carCheckLCPSolution_SSE(x + iteration*4,  /* Input */
                                             w + iteration*4,  /* Input */
                                             tol);             /* Input */
        iteration = iteration + 1;
    }

    (*iterations) = iteration;
    copyVec4_SSE(lambda, x+(iteration-1)*4);

    if(notsolved)
    {
        printf("Error - LCP solution not found\n");
    }
}

__forceinline void carCalcXandW_SSE(
                  MeReal        x[],/* Output */
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
    float _MM_ALIGN16 Drsrt[4];

    carPSM_SSE(PSMA, PSMb, A, b, unclampedIndex, numUnClamped);  

    carFactorise_SSE(PSMAchol, Drsrt, PSMA, numUnClamped);
    
    carSolve_SSE(PSMx, PSMAchol, PSMb, Drsrt, numUnClamped); 
    
    carUnPSM_SSE(x, PSMx, unclampedIndex, numUnClamped);   
  
    carCalcW_SSE(w, A, x, b);

}
__forceinline void carPSM_SSE(
            MeReal        PSMAchol[],        /* Output */
            MeReal        PSMb[],            /* Output */
            const MeReal  Achol[],           /* Input  */
            const MeReal  b[],               /* Input  */
            const int     unclampedIndex[],  /* Input  */
            int           numUnClamped)      /* Input  */
{
    int i,j;

    __m128 r = _mm_set_ss(1.0f);

    _mm_store_ps(&PSMAchol[0], r);
    _mm_store_ps(&PSMAchol[4], _mm_shuffle_ps(r, r, _MM_SHUFFLE(1,1,0,1)));
    _mm_store_ps(&PSMAchol[8], _mm_shuffle_ps(r, r, _MM_SHUFFLE(1,0,1,1)));
    _mm_store_ps(&PSMAchol[12],_mm_shuffle_ps(r, r, _MM_SHUFFLE(0,1,1,1)));

    for(i=0;i!=numUnClamped;i++)
    {
        for(j=0;j!=numUnClamped;j++)
        {
            PSMAchol[i*4+j] = Achol[unclampedIndex[i]*4 + unclampedIndex[j]];
        }
        PSMb[i] = b[unclampedIndex[i]];
    }
}

void carSolve_SSE(MeReal        x[],     /* Output */
              const MeReal  Achol[], /* Input  */
              const MeReal  b[],     /* Input  */
              const float   Drsrt[], /* Input  */
              int           n)       /* Input  */
{
    MeReal t;
    
    //acopy(x,b);

    __m128 r0,r1,r2,r3;
    __m128 rs,ac,rx,rt;

    r0 = _mm_load_ps(&Achol[0]);
    r1 = _mm_load_ps(&Achol[4]);
    r2 = _mm_load_ps(&Achol[8]);
    r3 = _mm_load_ps(&Achol[12]);

    rx = _mm_load_ps(b);
    rs = _mm_load_ps(Drsrt);

    if(n==4)
    {
        //** Lower solve
        //0
        rt = _mm_mul_ss(rx, rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(0));
        _mm_store_ss(&x[0], rt);

        //1
        ac = _mm_mul_ps(r0, rt);
        rt = _mm_mul_ps(_mm_sub_ps(rx, ac), rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(1));
        _mm_store_ss(&x[1], rt);

        //2
        ac = _mm_add_ps(ac, _mm_mul_ps(r1, rt));
        rt = _mm_mul_ps(_mm_sub_ps(rx, ac), rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(2));
        _mm_store_ss(&x[2], rt);

        //3
        ac = _mm_add_ps(ac, _mm_mul_ps(r2, rt));
        rt = _mm_mul_ps(_mm_sub_ps(rx, ac), rs);
        _mm_store_ss(&x[3], _mm_shuffle_ps(rt, rt, bcast(3)));

        //** Upper solve

        _MM_TRANSPOSE4_PS(r0, r1, r2, r3)

        rx = _mm_load_ps(x);

        //3
        //x[3]    = x[3] * Drsrt[3];
        rt = _mm_mul_ps(rx, rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(3));
        _mm_store_ss(&x[3], rt);

        //2
        ac = _mm_mul_ps(r3, rt);
        rt = _mm_mul_ps(_mm_sub_ps(rx, ac), rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(2));
        _mm_store_ss(&x[2], rt);
        
        //1
        ac = _mm_add_ps(ac, _mm_mul_ps(r2, rt));
        rt = _mm_mul_ps(_mm_sub_ps(rx, ac), rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(1));
        _mm_store_ss(&x[1], rt);

        //0
        ac = _mm_add_ps(ac, _mm_mul_ps(r1, rt));
        rt = _mm_mul_ps(_mm_sub_ps(rx, ac), rs);
        _mm_store_ss(&x[0], rt);

    }
    else if(n==3)
    {
        //** Lower solve
        
        //0
        rt = _mm_mul_ss(rx, rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(0));
        _mm_store_ss(&x[0], rt);
        
        //1
        ac = _mm_mul_ps(r0, rt);
        rt = _mm_mul_ps(_mm_sub_ps(rx, ac), rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(1));
        _mm_store_ss(&x[1], rt);
        
        //2
        ac = _mm_add_ps(ac, _mm_mul_ps(r1, rt));
        rt = _mm_mul_ps(_mm_sub_ps(rx, ac), rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(2));
        _mm_store_ss(&x[2], rt);

        //** Upper solve

        _MM_TRANSPOSE4_PS(r0, r1, r2, r3)

        rx = _mm_load_ps(x);

        //2
        rt = _mm_mul_ps(rx, rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(2));
        _mm_store_ss(&x[2], rt);

        //1
        ac = _mm_mul_ps(r2, rt);
        rt = _mm_mul_ps(_mm_sub_ps(rx, ac), rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(1));
        _mm_store_ss(&x[1], rt);
        
        //0
        ac = _mm_add_ps(ac, _mm_mul_ps(r1, rt));
        rt = _mm_mul_ps(_mm_sub_ps(rx, ac), rs);
        _mm_store_ss(&x[0], rt);
    }
    else if(n==2)
    {
        //** Lower solve
        //0
        rt = _mm_mul_ss(rx, rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(0));
        _mm_store_ss(&x[0], rt);
        
        //1
        ac = _mm_mul_ps(r0, rt);
        rt = _mm_mul_ps(_mm_sub_ps(rx, ac), rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(1));
        _mm_store_ss(&x[1], rt);

        //** Upper solve
        /*
        rx = _mm_load_ps(x);

        //1
        rt = _mm_mul_ps(rx, rs);
        rt = _mm_shuffle_ps(rt, rt, bcast(1));
        _mm_store_ss(&x[1], rt);

        //0
        ac = _mm_mul_ps(r0, rt);
        ac = _mm_shuffle_ps(ac, ac, bcast(1));
        rt = _mm_mul_ss(_mm_sub_ss(rx, ac), rs);
        _mm_store_ss(&x[0], rt);
        */
        
        x[1]    = x[1] * Drsrt[1];
        t       = Achol[1+0*4] * x[1];  
        x[0]    = x[0] - t;
        x[0]    = x[0] * Drsrt[0];
    }
    else if(n==1)
    {
        //** Lower solve/Upper solve
        _mm_store_ss(&x[0], _mm_mul_ps(rx, _mm_mul_ps(rs, rs)));
    }

}
__forceinline void carUnPSM_SSE(MeReal        x[],              /* Output */
              const MeReal  PSMx[],           /* Input  */
              const int     unclampedIndex[], /* Input  */
              int           numUnClamped)     /* Input  */
{
    int i;

    _mm_store_ps(x, _mm_setzero_ps());

    for(i=0;i!=numUnClamped;i++)
        x[unclampedIndex[i]] = PSMx[i];
}

__forceinline void carCalcW_SSE(MeReal       w[],  /* Output */
              const MeReal A[],  /* Input  */
              const MeReal x[],  /* Input  */
              const MeReal b[])  /* Input  */
{
    __m128 ac = abcmulp(&A[0], x[0]);

    ac = _mm_add_ps(ac, abcmulp(&A[4], x[1]));
    ac = _mm_add_ps(ac, abcmulp(&A[8], x[2]));
    ac = _mm_add_ps(ac, abcmulp(&A[12],x[3]));

    _mm_store_ps(w, _mm_sub_ps(ac, _mm_load_ps(b)));

}

__forceinline int carCheckLCPSolution_SSE(const MeReal x[],  /* Input */
                        const MeReal w[],  /* Input */
                        MeReal       tol)  /* Input */
{
    __m128 t,tx,tw;

    t  = rspread(-tol);

    /*
    tx = _mm_cmpnlt_ps(_mm_load_ps(x), t);
    tw = _mm_cmpnlt_ps(_mm_load_ps(w), t);
    return !(15^_mm_movemask_ps(_mm_and_ps(tx, tw)));
    //=solved
    */

    tx = _mm_cmplt_ps(_mm_load_ps(x), t);
    tw = _mm_cmplt_ps(_mm_load_ps(w), t);

    return _mm_movemask_ps(_mm_or_ps(tx, tw));
    //=notsolved
}

/**
 *
 *  Calculate the constraint force to apply to maintain each of the 4 constraints
 *
 */
__forceinline void carCalcConstraintForces_SSE(MeReal        constraintForce[], /* Output */
                             const MeReal  J[],               /* Input  */
                             const MeReal  lambda[])          /* Input  */
{
    __m128 l,r0,r1,r2,r3;

    l  = _mm_load_ps(&lambda[0]);
    r0 = _mm_mul_ps(_mm_load_ps(&J[0]), l);
    r1 = _mm_mul_ps(_mm_load_ps(&J[4]), l);
    r2 = _mm_mul_ps(_mm_load_ps(&J[8]), l);
    r3 = _mm_setzero_ps();

    _MM_TRANSPOSE4_PS(r0, r1, r2, r3)

    _mm_store_ps(&constraintForce[0*8], r0);
    _mm_store_ps(&constraintForce[1*8], r1);
    _mm_store_ps(&constraintForce[2*8], r2);
    _mm_store_ps(&constraintForce[3*8], r3);

    r0 = _mm_mul_ps(_mm_load_ps(&J[12]), l);
    r1 = _mm_mul_ps(_mm_load_ps(&J[16]), l);
    r2 = _mm_mul_ps(_mm_load_ps(&J[20]), l);
    r3 = _mm_setzero_ps();

    _MM_TRANSPOSE4_PS(r0, r1, r2, r3)

    _mm_store_ps(&constraintForce[0*8+4], r0);
    _mm_store_ps(&constraintForce[1*8+4], r1);
    _mm_store_ps(&constraintForce[2*8+4], r2);
    _mm_store_ps(&constraintForce[3*8+4], r3);

}
/**
 *  Add together the 4 constraint forces to get the body force
 *
*/
__forceinline void carCalcResultantForces_SSE(MeReal        bodyForce[],       /* Output */
                            MeReal        bodyTorque[],      /* Output */
                            const MeReal  externalForce[],   /* Input  */
                            const MeReal  externalTorque[],  /* Input  */
                            const MeReal  constraintForce[]) /* Input  */
{
    int j;

    __m128 fac = _mm_load_ps(externalForce);
    __m128 tac = _mm_load_ps(externalTorque);

    for(j=0;j<4;j++)
    {
        fac = _mm_add_ps(fac, _mm_load_ps(&constraintForce[j*8]));
        tac = _mm_add_ps(tac, _mm_load_ps(&constraintForce[j*8+4]));
    }
    
    _mm_store_ps(bodyForce, fac);
    _mm_store_ps(bodyTorque, tac);

}

/** 
 *
 *  acceleration := inverse(mass) * force
*/
__forceinline void carCalcAccel_SSE(MeReal        accel[],      /* Output */
                  MeReal        accelRot[],   /* Input  */
                  const MeReal  bodyForce[],  /* Input  */
                  const MeReal  bodyTorque[], /* Input  */
                  const MeReal  invM[])       /* Input  */
{
    _mm_store_ps(accel, abcmulp(bodyForce, invM[1]));
    _mm_store_ps(accelRot, abcmulp(bodyTorque, invM[0]));
}

#endif

/********************************************************************/
