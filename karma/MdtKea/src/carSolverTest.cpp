#include <MePrecision.h>
#include <stdio.h>
#include "carSolver.h"
#include "carSolverTest.h"
#include "stdlib.h"
#include "MeSimpleFile.h"
#include "MeStream.h"

carSolverConstraintOutput    PCconstraintOutput;
carSolverBodyOutput          PCbodyOutput;
carSolverIntermediateResults PCinterResults;

carSolverConstraintOutput    PS2constraintOutput;
carSolverBodyOutput          PS2bodyOutput;
carSolverIntermediateResults PS2interResults;

carSolverConstraintInput     constraintInput;
carSolverBodyInput           bodyInput;
carSolverParameters          parameters;

#define NUM_TESTS 100

int main()
{

#ifdef PS2
    carUploadVU0Microcode();
#endif

#if 1
    int i;
    int passed;
    MeStream file;
    char buf[256];

    file = MeStreamOpen("h://4FCtest.txt",kMeOpenModeRDONLY);

    i      = 0;
    passed = 1;

    while(i!=NUM_TESTS&&passed==1)
    {
        int tmp;
            
        printf("-- test %d\n",i);

        freadInt(&tmp,file,"test");

        passed=testSolverAndIntegrator(file);
        i++;
    }

    if(passed==0) printf("failed on test %d\n",i-1);
    

    MeStreamClose(file);

#endif

#if 0
    testCarSolver();
#endif


    //testFactorAndSolve();

    return 0;
}

MeReal vec2Norm(const MeReal x[], /* Input */
                int   count)      /* Input */
{
    MeReal norm;
    int i;

    norm = 0.0f;
    for(i=0;i!=count;i++)
    {
        norm += x[i]*x[i];
    }
    
    norm=MeSqrt(norm);
    return norm;
}
#ifndef PS2
void testFactoriser()
{
    MeReal A[16];
    MeReal B[16];
    MeReal C[16];
    MeReal D[16];
    MeReal E[16];
    MeReal F[16];
    MeReal G[16];

    int i,j;

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=i+1;j++)
        {
            A[i+j*4]=(MeReal)(i+1);
        }
        for(j=i+1;j!=4;j++)
        {
            A[i+j*4]=0.0f;
        }
    }

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=4;j++)
        {
            B[4*i+j]=A[4*j+i];
        }
    }
    
    matmul(C,A,B);

    printMat44(C,"C");

    carFactorise(F,C);

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=i+1;j++)
        {
            D[j*4+i]=F[j*4+i];
        }
        for(j=i+1;j!=4;j++)
        {
            D[j*4+i]=0.0f;
        }
    }

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=i;j++)
        {
            E[j*4+i]=0.0f;
        }
        for(j=i;j!=4;j++)
        {
            E[j*4+i]=F[i*4+j];
        }
    }
    printMat44(F,"factorised C");
    printMat44(D,"L");
    printMat44(E,"L'");

    matmul(G,D,E);

    printMat44(G,"L*L'");
}
#endif
void testCholOutput(const MeReal Achol[], /* Input */
                    const MeReal A[])     /* Input */
{
    MeReal L[16];
    MeReal Ltrans[16];
    MeReal LLtrans[16];
    MeReal error[16];
    MeReal norm;
    int i,j;

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=i+1;j++)
        {
            L[j*4+i]=Achol[j*4+i];
        }
        for(j=i+1;j!=4;j++)
        {
            L[j*4+i]=0.0f;
        }
    }

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=i;j++)
        {
            Ltrans[j*4+i]=0.0f;
        }
        for(j=i;j!=4;j++)
        {
            Ltrans[j*4+i]=Achol[i*4+j];
        }
    }
    matmul(LLtrans,L,Ltrans);

    printMat44(LLtrans,"L*transpose(L)");

    matsub44(error,LLtrans,A);

    printMat44(error,"L*transpose(L)-A");

    norm = vec2Norm(error,16);
    printf("norm = %e\n",norm);
}
#ifndef PS2
void testSolver()
{
    MeReal A[16];
    MeReal B[16];
    MeReal C[16];
    MeReal D[16];
    MeReal b[4];
    MeReal x[4];
    MeReal calculatedx[4];

    int i,j;

    /* Make the test matrix, C */

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=i+1;j++)
        {
            A[i+j*4]=(MeReal)(i+1);
        }
        for(j=i+1;j!=4;j++)
        {
            A[i+j*4]=0.0f;
        }
    }

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=4;j++)
        {
            B[4*i+j]=A[4*j+i];
        }
    }
    
    matmul(C,A,B);
    printMat44(C,"C");

    /* Make the test vector x */

    for(i=0;i!=4;i++)
    {
        x[i]=(MeReal)(i+1);
    }

    printVec4(x,"x");


    /* Make the rhs vector, b */

    matvecmul(b,C,x);

    printVec4(b,"rhs");

    /* factorise */

    carFactorise(D,C);

    carSolve(calculatedx,D,b,4);

    printVec4(calculatedx,"calculated x");

}
#endif

#if 0
void testCarSolver()
{
    carSolverConstraintInput  constraintInput;
    carSolverConstraintOutput constraintOutput;
    carSolverBodyInput        bodyInput;
    carSolverBodyOutput       bodyOutput;
    MdtContactGroup           contactGroup;
    MdtContact                contacts[4];
    MdtKeaBody                body;
    MeReal                    tm[16];

    contacts[0].penetration    =  0.2f;
    contacts[0].normal[0]      =  0.0f;
    contacts[0].normal[1]      =  0.0f;
    contacts[0].normal[2]      =  1.0f;
    contacts[0].cpos[0]        = -1.0f;
    contacts[0].cpos[1]        =  1.0f;
    contacts[0].cpos[2]        =  0.0f;
    contacts[0].params.options =  0;

    contacts[1].penetration    =  0.2f;
    contacts[1].normal[0]      =  0.0f;
    contacts[1].normal[1]      =  0.0f;
    contacts[1].normal[2]      =  1.0f;
    contacts[1].cpos[0]        =  1.0f;
    contacts[1].cpos[1]        =  1.0f;
    contacts[1].cpos[2]        =  0.0f;
    contacts[1].params.options =  0;

    contacts[2].penetration    =  0.2f;
    contacts[2].normal[0]      =  0.0f;
    contacts[2].normal[1]      =  0.0f;
    contacts[2].normal[2]      =  1.0f;
    contacts[2].cpos[0]        = -1.0f;
    contacts[2].cpos[1]        = -1.0f;
    contacts[2].cpos[2]        =  0.0f;
    contacts[2].params.options =  0;

    contacts[3].penetration    =  0.2f;
    contacts[3].normal[0]      =  0.0f;
    contacts[3].normal[1]      =  0.0f;
    contacts[3].normal[2]      =  1.0f;
    contacts[3].cpos[0]        =  1.0f;
    contacts[3].cpos[1]        = -1.0f;
    contacts[3].cpos[2]        =  0.0f;
    contacts[3].params.options =  0;

    contactGroup.first = &contacts[0];
    contacts[0].nextContact = &contacts[1];
    contacts[1].nextContact = &contacts[2];
    contacts[2].nextContact = &contacts[3];

    body.vel[0] = 0.0f;
    body.vel[1] = 0.0f;
    body.vel[2] = 0.0f;
    body.vel[4] = 0.0f;
    body.vel[5] = 0.0f;
    body.vel[6] = 0.0f;

    tm[12] = 0.0f;
    tm[13] = 0.0f;
    tm[14] = 1.0f;

    bodyInput.bodyForce[0]         = 0.0f;
    bodyInput.bodyForce[1]         = 0.0f;
    bodyInput.bodyForce[2]         = 0.0f;
    bodyInput.bodyTorque[0]        = 0.0f;      
    bodyInput.bodyTorque[1]        = 0.0f;      
    bodyInput.bodyTorque[2]        = 0.0f;      
    bodyInput.invInertiaAndMass[0] = 0.5f;
    bodyInput.invInertiaAndMass[1] = 0.5f;
    bodyInput.vel[0]               = 0.0f;
    bodyInput.vel[1]               = 0.0f;
    bodyInput.vel[2]               = 0.0f;
    bodyInput.velrot[0]            = 0.0f;          
    bodyInput.velrot[1]            = 0.0f;          
    bodyInput.velrot[2]            = 0.0f;          

    carAdd4Contacts(&constraintInput,/* Output */
                    &contactGroup,   /* Input  */ 
                    tm+12,           /* Input  */ 
                    body.vel);       /* Input  */ 

    parameters.invH              = 60.0f;
    parameters.gammaOverHSquared = 0.5f*parameters.invH*parameters.invH; /* 1800! */
    parameters.epsilon           = 0.01f;    
    parameters.tol               = (MeReal)(1e-3);

    printConstraintInput(constraintInput);   /* Input */

    carAddConstraintForces(&constraintOutput, /* Output */
                           &bodyOutput,       /* Output */
                           &interResults,     /* Output */
                           constraintInput,   /* Input  */
                           bodyInput,         /* Input  */
                           parameters);       /* Input  */

    printIntermediateResults(interResults);   /* Input */
    printConstraintOutput(constraintOutput); /* Input */
    printBodyOutput(bodyOutput);             /* Input */
    
}
#endif



int testSolverAndIntegrator(MeStream file)
{
    MeReal                    newvel[8];
    int                       passed;

    readConstraintInputFromFile(&constraintInput,file);
    readBodyInputFromFile(&bodyInput,file);
    readParametersFromFile(&parameters,file);

#if 0
    printConstraintInput(constraintInput);
    printBodyInput(bodyInput);
    printParameters(parameters);
#endif

    carAddConstraintForces(&PS2constraintOutput, /* Output */
                           &PS2bodyOutput,       /* Output */
                           &PS2interResults,     /* Output */
                           constraintInput,   /* Input  */
                           bodyInput,         /* Input  */
                           parameters);       /* Input  */

    readConstraintOutputFromFile(&PCconstraintOutput,file);
    readBodyOutputFromFile(&PCbodyOutput,file);
    readIntermediateResultsFromFile(&PCinterResults,file);
 
    compareConstraintOutput(PS2constraintOutput,
                            PCconstraintOutput);

    compareBodyOutput(PS2bodyOutput,
                      PCbodyOutput);

    compareIntermediateResults(PS2interResults,
                               PCinterResults);

    newvel[0] = bodyInput.vel[0] + MeRecip(parameters.invH) * PS2bodyOutput.accel[0];
    newvel[1] = bodyInput.vel[1] + MeRecip(parameters.invH) * PS2bodyOutput.accel[1];
    newvel[2] = bodyInput.vel[2] + MeRecip(parameters.invH) * PS2bodyOutput.accel[2];
    newvel[4] = bodyInput.velrot[0] + MeRecip(parameters.invH) * PS2bodyOutput.accelRot[0];
    newvel[5] = bodyInput.velrot[1] + MeRecip(parameters.invH) * PS2bodyOutput.accelRot[1];
    newvel[6] = bodyInput.velrot[2] + MeRecip(parameters.invH) * PS2bodyOutput.accelRot[2];

    //printVec4(constraintOutput.lambda,"lambda");

    passed = checkResult(constraintInput.J,newvel,PS2constraintOutput.lambda);
    return passed;
}

#ifndef PS2
void testFactorAndSolve()
{
    int i,j;

    MeReal A[16] = { 
        000000.214888f,  000000.076243f, -000000.380292f,  000000.090624f,
        000000.076242f,  000000.075737f, -000000.168205f,  000000.069672f,
       -000000.380292f, -000000.168205f,  000000.815719f, -000000.512024f,
        000000.090624f,  000000.069672f, -000000.512023f,  000000.953161f};
    MeReal Achol[16];
    MeReal x[4];
    MeReal w[4];
    MeReal b[4] = {000003.225384f,  000001.087968f, -000000.161298f, -000001.326589f};
    MeReal acc[4];

    carFactorise(Achol,A);
    carSolve(x,Achol,b,4);

    printMat44(A,"A");
    printMat44(Achol,"Achol");
    printVec4(x,"x");

    acc[0] = 0.0f;
    acc[1] = 0.0f;
    acc[2] = 0.0f;
    acc[3] = 0.0f;

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=4;j++)
        {
            acc[j] = acc[j] + A[i*4+j]*x[i];
        }
    }

    for(j=0;j!=4;j++)
    {
        w[j] = acc[j] - b[j];
    }
    printVec4(w,"w");
}
#endif