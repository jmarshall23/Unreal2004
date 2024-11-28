#ifndef _CARSOLVER_H
#define _CARSOLVER_H

#include <MePrecision.h>
#include <MdtTypes.h>
#include <KeaInternal.hpp>

#ifndef PS2
#define  CSS_ALIGN  __declspec(align(16))
#else
#define  CSS_ALIGN
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/*  Look up tables for LCP */

const int unclampedIndexSet[16*4] = {  0, 1, 2, 3, 

                                       0, 1, 2,-1, 
                                       0, 1, 3,-1,
                                       0, 2, 3,-1,
                                       1, 2, 3,-1,
                                       
                                       0, 1,-1,-1, 
                                       0, 2,-1,-1,
                                       1, 2,-1,-1,
                                       0, 3,-1,-1,
                                       1, 3,-1,-1,
                                       2, 3,-1,-1,
                                       
                                       0,-1,-1,-1, 
                                       1,-1,-1,-1,
                                       2,-1,-1,-1,
                                       3,-1,-1,-1,
                                       
                                       -1,-1,-1,-1};

const int numUnclamped[16] = {4,3,3,3,3,2,2,2,2,2,2,1,1,1,1,0};

/**/

typedef struct CSS_ALIGN
{    
    MeReal JM[24];              // Quadwords [0..6)
    MeReal A[16];               // Quadwords [6..10)
    MeReal rhs[4];              // Quadwords [10..11)
    MeReal vhmf[8];             // Quadwords [11..13)

    MeReal x[16*4];             // Quadwords [13..29)
    MeReal w[16*4];             // Quadwords [29..45)
    MeReal PSMA[16*16];         // Quadwords [45..109)
    MeReal PSMAchol[16*16];     // Quadwords [109..173)
    MeReal PSMb[4*16];          // Quadwords [173..189)
    MeReal PSMx[4*16];          // Quadwords [189..205)
    int    iterations;          // Quadwords [205..206)
    int    pad[3];

} carSolverIntermediateResults;

typedef struct CSS_ALIGN
{
    MeReal  J[24];               
    MeReal  xi[4];              
    MeReal  c[4];               
    MeReal  slipfactor[4];      
    
} carSolverConstraintInput;

typedef struct CSS_ALIGN
{
    int count;
    int maxCount;
    
    carSolverConstraintInput * constraintInput;
    MdtKeaBody **              body;

} carSolverConstraintList;

typedef struct CSS_ALIGN
{
    MeReal          bodyForce[4];       /* Output / Input */
    MeReal          bodyTorque[4];      /* Output / Input */                            
    MeReal          vel[4];             
    MeReal          velrot[4];          
    MeReal          invInertiaAndMass[4];

} carSolverBodyInput;

typedef struct CSS_ALIGN
{
    MeReal          lambda[4];          
    MeReal          constraintForce[8*4]; 
                                
} carSolverConstraintOutput;

typedef struct CSS_ALIGN
{
    MeReal          bodyForce[4];       /* Output / Input */
    MeReal          bodyTorque[4];      /* Output / Input */                            
    MeReal          accel[4];           
    MeReal          accelRot[4];        

} carSolverBodyOutput;

typedef struct  CSS_ALIGN
{
    MeReal          invH;
    MeReal          gammaOverHSquared;
    MeReal          epsilon;
    MeReal          tol;
} carSolverParameters;

/*******************************************************************************************/

#if 0
void carAdd4Contacts(carSolverConstraintInput *  constraints, /* Output */
                     MdtContact *const           contacts[],  /* Input */ 
                     const MeReal                pos[],       /* Input */ 
                     const MeReal                vel[]);      /* Input */ 
#endif
/**/

void carAddConstraintForces(carSolverConstraintOutput *     constraintOutput,  /* Output */
                            carSolverBodyOutput *           bodyOutput,        /* Output */
                            carSolverIntermediateResults *  scratch,           /* Output (just for testing) */
                            const carSolverConstraintInput* constraintInput,   /* Input  */
                            const carSolverBodyInput *      bodyInput,         /* Input  */
                            const carSolverParameters *     params,            /* Input  */
                            const int                       cpu_rec);          /* Input  */

void carAddConstraintForces_vanilla(carSolverConstraintOutput *     constraintOutput,  /* Output */
                                    carSolverBodyOutput *           bodyOutput,        /* Output */
                                    carSolverIntermediateResults *  scratch,           /* Output (just for testing) */
                                    const carSolverConstraintInput* constraintInput,   /* Input  */
                                    const carSolverBodyInput *      bodyInput,         /* Input  */
                                    const carSolverParameters *     params);           /* Input  */


/**/

void carCalcVhmf(MeReal        vhmf[],          /* Output */
                 const MeReal  invM[],          /* Input  */    
                 const MeReal  vel[],           /* Input  */    
                 const MeReal  velrot[],        /* Input  */ 
                 const MeReal  force[],         /* Input  */ 
                 const MeReal  torque[],        /* Input  */ 
                 MeReal        invStepsize);    /* Input  */

void carCalcJMandRHS(MeReal        JM[],        /* Output */
                     MeReal        rhs[],       /* Output */
                     const MeReal  jstore[],    /* Input */
                     const MeReal  c[],         /* Input */
                     const MeReal  xi[],        /* Input */
                     const MeReal  invIworld[], /* Input */
                     const MeReal  vhmf[],      /* Input */
                     MeReal        invStepsize, /* Input */
                     MeReal        gamma);      /* Input */

void carCalcJMJT(MeReal        A[],           /* Output */
                 const MeReal  JM[],          /* Input */
                 const MeReal  J[],           /* Input */
                 const MeReal  slipfactor[],  /* Input */
                 MeReal        epsilon,       /* Input */
                 MeReal        invStepsize);  /* Input */
                               
void carFactorise(MeReal Achol[],    /* Output */
                  const  MeReal A[]); /* Input  */

void carLCP(MeReal       lambda[],   /* Output */
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

void carCalcXandW(MeReal        x[],              /* Output */
                  MeReal        w[],              /* Output */
                  MeReal        PSMA[],           /* Output */
                  MeReal        PSMAchol[],       /* Output */
                  MeReal        PSMb[],           /* Output */
                  MeReal        PSMx[],           /* Output */
                  const MeReal  A[],              /* Input */
                  const MeReal  b[],              /* Input */
                  const int     unclampedIndex[], /* Input */
                  int           numUnClamped);    /* Input */

void carPSM(MeReal        PSMAchol[],        /* Output */
            MeReal        PSMb[],            /* Output */
            const MeReal  Achol[],           /* Input  */
            const MeReal  b[],               /* Input  */
            const int     unclampedIndex[],  /* Input  */
            int           numUnClamped);     /* Input  */

void carSolve(MeReal x[],           /* Output */
              const MeReal Achol[], /* Input  */
              const MeReal b[],     /* Input  */
              int          n);      /* Input  */

void carUnPSM(MeReal        x[],              /* Output */
              const MeReal  PSMx[],           /* Input  */
              const int     unClampedIndex[], /* Input  */
              int           numUnClamped);    /* Input  */

void carCalcW(MeReal       w[],  /* Output */
              const MeReal A[],  /* Input  */
              const MeReal x[],  /* Input  */
              const MeReal b[]); /* Input  */

int carCheckLCPSolution(const MeReal x[], /* Input */
                        const MeReal w[], /* Input */
                        MeReal       tol);/* Input */

void carCalcConstraintForces(MeReal        constraintForce[], /* Output */
                             const MeReal  J[],               /* Input  */
                             const MeReal  lambda[]);         /* Input  */

void carCalcResultantForces(MeReal        bodyForce[],        /* Output */
                            MeReal        bodyTorque[],       /* Output */
                            const MeReal  externalForce[],    /* Input  */
                            const MeReal  externalTorque[],   /* Input  */
                            const MeReal  constraintForce[]); /* Input  */

void carCalcAccel(MeReal        accel[],      /* Output */
                  MeReal        accelRot[],   /* Input  */
                  const MeReal  bodyForce[],  /* Input  */
                  const MeReal  bodyTorque[], /* Input  */
                  const MeReal  invIworld[]); /* Input  */

#ifdef __cplusplus
}
#endif
                            
#endif