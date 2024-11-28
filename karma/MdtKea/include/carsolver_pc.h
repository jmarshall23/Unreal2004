#ifndef _CARSOLVER_PC_H
#define _CARSOLVER_PC_H

#include <MdtTypes.h>
#include "carSolver.h"
#include <KeaSSEi.h>

#ifdef __cplusplus
extern "C"
{
#endif


#ifndef _BUILD_VANILLA

void carAddConstraintForces_sse(carSolverConstraintOutput *     constraintOutput, 
                                carSolverBodyOutput *           bodyOutput,       
                                carSolverIntermediateResults *  scratch,        
                                const carSolverConstraintInput* constraintInput,  
                                const carSolverBodyInput *      bodyInput,        
                                const carSolverParameters *     params);          

/**/

void carCalcVhmf_SSE(MeReal        vhmf[],          /* Output */
                 const MeReal  invM[],          /* Input  */    
                 const MeReal  vel[],           /* Input  */    
                 const MeReal  velrot[],        /* Input  */ 
                 const MeReal  force[],         /* Input  */ 
                 const MeReal  torque[],        /* Input  */ 
                 MeReal        invStepsize);    /* Input  */

void carCalcJMandRHS_SSE(MeReal        JM[],        /* Output */
                     MeReal        rhs[],       /* Output */
                     const MeReal  jstore[],    /* Input */
                     const MeReal  c[],         /* Input */
                     const MeReal  xi[],        /* Input */
                     const MeReal  invIworld[], /* Input */
                     const MeReal  vhmf[],      /* Input */
                     MeReal        invStepsize, /* Input */
                     MeReal        gamma);      /* Input */

void carCalcJMJT_SSE(MeReal        A[],           /* Output */
                 const MeReal  JM[],          /* Input */
                 const MeReal  J[],           /* Input */
                 const MeReal  slipfactor[],  /* Input */
                 MeReal        epsilon,       /* Input */
                 MeReal        invStepsize);  /* Input */
                               
void carFactorise_SSE(MeReal Achol[],    /* Output */
                  float  Drsrt[],    /* Output */
                  const MeReal A[],  /* Input  */
                  const int     n);  /* Input  */

void carLCP_SSE(MeReal       lambda[],   /* Output */
            MeReal       x[],        /* Output */
            MeReal       w[],        /* Output */
            MeReal       PSMA[],     /* Output */
            MeReal       PSMAchol[], /* Output */
            MeReal       PSMb[],     /* Output */
            MeReal       PSMx[],     /* Output */
            int *        iteration,  /* Output */
            const MeReal A[],        /* Input  */
            const MeReal rhs[],      /* Input  */
            MeReal       tol);       /* Input  */

void carCalcXandW_SSE(MeReal        x[],              /* Output */
                  MeReal        w[],              /* Output */
                  MeReal        PSMA[],           /* Output */
                  MeReal        PSMAchol[],       /* Output */
                  MeReal        PSMb[],           /* Output */
                  MeReal        PSMx[],           /* Output */
                  const MeReal  A[],              /* Input */
                  const MeReal  b[],              /* Input */
                  const int     unclampedIndex[], /* Input */
                  int           numUnClamped);    /* Input */

void carPSM_SSE(MeReal        PSMAchol[],        /* Output */
            MeReal        PSMb[],            /* Output */
            const MeReal  Achol[],           /* Input  */
            const MeReal  b[],               /* Input  */
            const int     unclampedIndex[],  /* Input  */
            int           numUnClamped);     /* Input  */

void carSolve_SSE(MeReal x[],           /* Output */
              const MeReal Achol[], /* Input  */
              const MeReal b[],     /* Input  */
              const float  Drsrt[], /* Input  */
              int          n);      /* Input  */

void carUnPSM_SSE(MeReal        x[],              /* Output */
              const MeReal  PSMx[],           /* Input  */
              const int     unClampedIndex[], /* Input  */
              int           numUnClamped);    /* Input  */

void carCalcW_SSE(MeReal       w[],  /* Output */
              const MeReal A[],  /* Input  */
              const MeReal x[],  /* Input  */
              const MeReal b[]); /* Input  */

int carCheckLCPSolution_SSE(const MeReal x[], /* Input */
                        const MeReal w[], /* Input */
                        MeReal       tol);/* Input */

void carCalcConstraintForces_SSE(MeReal        constraintForce[], /* Output */
                             const MeReal  J[],               /* Input  */
                             const MeReal  lambda[]);         /* Input  */

void carCalcResultantForces_SSE(MeReal        bodyForce[],        /* Output */
                            MeReal        bodyTorque[],       /* Output */
                            const MeReal  externalForce[],    /* Input  */
                            const MeReal  externalTorque[],   /* Input  */
                            const MeReal  constraintForce[]); /* Input  */

void carCalcAccel_SSE(MeReal        accel[],      /* Output */
                  MeReal        accelRot[],   /* Input  */
                  const MeReal  bodyForce[],  /* Input  */
                  const MeReal  bodyTorque[], /* Input  */
                  const MeReal  invIworld[]); /* Input  */

#endif

#ifdef __cplusplus
}
#endif
                            
#endif