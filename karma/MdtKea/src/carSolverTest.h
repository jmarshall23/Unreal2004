#ifndef _CARSOLVERTEST_H
#define _CARSOLVERTEST_H

#define PRINT_DEBUG_INFO 0

#include "carSolver.h"
#include "MeStream.h"

/* Matrix maths functions */

void matmul(MeReal        A[],  /* Output */
            const MeReal  B[],  /* Input  */
            const MeReal  C[]); /* Input  */

void matsub44(MeReal        A[],  /* Output */
              const MeReal  B[],  /* Input  */
              const MeReal  C[]); /* Input  */

void matsub46(MeReal        A[],  /* Output */
              const MeReal  B[],  /* Input  */
              const MeReal  C[]); /* Input  */

MeReal matnorm44(const MeReal A[]); /* Input */

MeReal matnorm46(const MeReal A[]); /* Input */

void testFullRank();
int fullRank(const MeReal J[],     /* Input */
             const MeReal invM[]); /* Input */

void LU(MeReal        LUJ[],     /* Output */
        const MeReal  J[]);      /* Input */

/* Vector maths functions */

void vecsub4(MeReal        A[],  /* Output */
             const MeReal  B[],  /* Input  */
             const MeReal  C[]); /* Input  */

MeReal vecnorm4(const MeReal A[]); /* Input */

void vecsub3(MeReal        A[],  /* Output */
             const MeReal  B[],  /* Input  */
             const MeReal  C[]); /* Input  */

MeReal vecnorm3(const MeReal A[]); /* Input */


/* Functions for testing the solver and its components */

void testFactoriser();
void testSolver();
void testCarSolver();
int testSolverAndIntegrator(MeStream file); /* Input */
void testFactorAndSolve();

/* Functions for printing basic datatypes */

void printMat44(const MeReal  A[],   /* Input */
                const char *  desc); /* Input */

void printMat46(const MeReal  A[],   /* Input */
                const char *  desc); /* Input */

void printVec4(const MeReal  A[],   /* Input */
               const char *  desc); /* Input */
void printVec3(const MeReal  A[],   /* Input */
               const char *  desc); /* Input */

/* Functions for printing constraint/body data */

void printConstraintInput(const carSolverConstraintInput   c);      /* Input */
void printConstraintOutput(const carSolverConstraintOutput   c);    /* Input */
void printBodyInput(const carSolverBodyInput   b);                  /* Input */
void printBodyOutput(const carSolverBodyOutput   b);                /* Input */
void printParameters(const carSolverParameters   p);                /* Input */
void printIntermediateResults(const carSolverIntermediateResults i); /* Input */

/* Functions for comparing constraint/body/inter data */

void compareConstraintOutput(const carSolverConstraintOutput   ps2, /* Input */
                             const carSolverConstraintOutput   pc); /* Input */

void compareBodyOutput(const carSolverBodyOutput   ps2, /* Input */
                       const carSolverBodyOutput   pc); /* Input */

void compareIntermediateResults(const carSolverIntermediateResults ps2, /* Input */
                                const carSolverIntermediateResults pc); /* Input */

/* Functions for initialising datatypes to random values */

MeReal makeRandMeRealPositive();

void makeRandArray(MeReal x[],   /* Output */
                   int    count);/* Input  */

void makeRandFullRank46Mat(MeReal        J[],    /* Output */
                           const MeReal  invM[]);/* Input  */

/* Functions for zeroing datatypes */ 

void makeZeroArray(MeReal x[],   /* Output */
                   int    count);/* Input  */

/* Functions for writing basic types to a file */

void fwriteMat46(int           file, /* Output */
                 const MeReal  A[],  /* Input */
                 const char *  desc);/* Input */

void fwriteVec4(int           file, /* Output */
                const MeReal  A[],  /* Input */
                const char *  desc);/* Input */

void fwriteMeReal(int file,         /* Output */
                  MeReal x,         /* Input */
                  const char *desc);/* Input */

void fwriteMat44(int           file, /* Output */
                 const MeReal  A[],  /* Input */
                 const char *  desc);/* Input */

void fwriteInt(int           file, /* Output */
               int           x,    /* Input */
               const char *  desc);/* Input */

/* Functions for reading basic types from a MeStream */

void freadMat46(MeReal        A[],  /* Output */
                MeStream      file, /* Input */
                const char *  desc);/* Input */

void freadVec4(MeReal        A[],  /* Output */
               MeStream      file, /* Input */
               const char *  desc);/* Input */

void freadMeReal(MeReal *      x,    /* Output */
                 MeStream      file, /* Input */
                 const char *  desc);/* Input */

void freadMat44(MeReal        A[],  /* Output */
                MeStream      file, /* Input */
                const char *  desc);/* Input */

void freadInt(int *         x,    /* Output */
              MeStream      file, /* Input */
              const char *  desc);/* Input */


/* Functions for writing constraint/body data to file */

void writeConstraintInputToFile(int                            file, /* Output */
                                const carSolverConstraintInput c);   /* Input */

void writeConstraintOutputToFile(int                             file, /* Output */
                                 const carSolverConstraintOutput c);   /* Input */

void writeBodyInputToFile(int                      file, /* Output */
                          const carSolverBodyInput b);   /* Input */

void writeBodyOutputToFile(int                       file, /* Output */
                           const carSolverBodyOutput b);   /* Input */

void writeParametersToFile(int                       file, /* Output */
                           const carSolverParameters p);   /* Input */

void writeIntermediateResultsToFile(int                                file, /* Output */ 
                                    const carSolverIntermediateResults i);   /* Input */

/* Functions for reading constraint/body data from an MeStream */

void readConstraintInputFromFile(carSolverConstraintInput *  c,    /* Output */
                                 MeStream                    file);/* Input */                                

void readConstraintOutputFromFile(carSolverConstraintOutput *  c,    /* Output */
                                  MeStream                     file);/* Input */
                                 
void readBodyInputFromFile(carSolverBodyInput *  b,     /* Output */
                           MeStream              file); /* Input */
                          
void readBodyOutputFromFile(carSolverBodyOutput *  b,    /* Output */
                            MeStream               file);/* Input */
                           
void readParametersFromFile(carSolverParameters *  p,    /* Output */
                            MeStream               file);/* Input */
                           
void readIntermediateResultsFromFile(carSolverIntermediateResults *  i,    /* Output */                                     
                                     MeStream                        file);/* Input */

/* Function for checking that a resultant velocity satisfies a constraint */

int checkResult(const MeReal J[],      /* Input */
                const MeReal v[],      /* Input */
                const MeReal lambda[]);/* Input */

#endif