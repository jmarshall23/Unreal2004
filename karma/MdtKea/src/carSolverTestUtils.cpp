#include <MePrecision.h>
#include <stdio.h>
#include "carSolver.h"
#include "carSolverTest.h"
#include "stdlib.h"
#include "MeSimpleFile.h"
#include "MeStream.h"

#define EPS 0.01f

/* Functions for comparing basic types */

void compareMat46(const MeReal ps2[], /* Input */
                  const MeReal pc[],  /* Input */
                  const char * desc)  /* Input */
{
    MeReal error[24];
    MeReal norm;

    matsub46(error, /* Output */
             ps2,   /* Input */
             pc);   /* Input */

    norm = matnorm46(error); /* Input */

    if(norm < EPS) 
    {
        //printf("%s same (norm=%e) \n",desc,norm);
    }
    else         
    {
        printf("%s different (norm=%e) \n",desc,norm);

        printMat46(ps2,"ps2");
        printMat46(pc,"pc");
        printf("\n");
    }
}

void compareMat44(const MeReal ps2[], /* Input */
                  const MeReal pc[],  /* Input */
                  const char * desc)  /* Input */
{
    MeReal error[16];
    MeReal norm;

    matsub44(error, /* Output */
             ps2,   /* Input */
             pc);   /* Input */

    norm = matnorm44(error); /* Input */

    if(norm < EPS) 
    {
        //printf("%s same (norm=%e) \n",desc,norm);
    }
    else         
    {
        printf("%s different (norm=%e) \n",desc,norm);

        printMat44(ps2,"ps2");
        printMat44(pc,"pc");
        printf("\n");
    }
}
void compareVec3(const MeReal ps2[], /* Input */
                 const MeReal pc[],  /* Input */
                 const char * desc)  /* Input */
{
    MeReal error[3];
    MeReal norm;

    vecsub3(error, /* Output */
            ps2,   /* Input */
            pc);   /* Input */

    norm = vecnorm3(error); /* Input */

    if(norm < EPS) 
    {
        //printf("%s same (norm=%e) \n",desc,norm);
    }
    else         
    {
        printf("%s different (norm=%e) \n",desc,norm);

        printVec3(ps2,"ps2");
        printVec3(pc,"pc");
        printf("\n");
    }
}

void compareVec4(const MeReal ps2[], /* Input */
                 const MeReal pc[],  /* Input */
                 const char * desc)  /* Input */
{
    MeReal error[4];
    MeReal norm;

    vecsub4(error, /* Output */
            ps2,   /* Input */
            pc);   /* Input */

    norm = vecnorm4(error); /* Input */

    if(norm < EPS) 
    {
        //printf("%s same (norm=%e) \n",desc,norm);
    }
    else         
    {
        printf("%s different (norm=%e) \n",desc,norm);

        printVec4(ps2,"ps2");
        printVec4(pc,"pc");
        printf("\n");
    }
}
void compareInt(int ps2,            /* Input */
                int pc,             /* Input */
                const char * desc)  /* Input */
{
    if(ps2==pc) 
    {
        printf("%s same (%d)\n",desc,ps2);
    }
    else        
    {
        printf("%s different\n",desc);
        printf("ps2=%d pc=%d\n",ps2,pc);
    }
}

/* Functions for comparing constraint/body/inter data */

void compareConstraintOutput(const carSolverConstraintOutput   ps2, /* Input */
                             const carSolverConstraintOutput   pc)  /* Input */
{

}
void compareBodyOutput(const carSolverBodyOutput   ps2, /* Input */
                       const carSolverBodyOutput   pc)  /* Input */
{

}

void compareIntermediateResults(const carSolverIntermediateResults ps2, /* Input */
                                const carSolverIntermediateResults pc)  /* Input */
{
    int minIterations;
    int i;

#if 0
    printf("Comparing intermediate results\n");
    printf("------------------------------\n");
    printf("\n");
#endif

    compareMat46(ps2.JM,pc.JM,       "JM      ");
    compareMat44(ps2.A,pc.A,         "A       ");
    compareVec4(ps2.rhs,pc.rhs,      "rhs     ");
    compareVec3(ps2.vhmf,pc.vhmf,    "vhmf lin");
    compareVec3(ps2.vhmf+4,pc.vhmf+4,"vhmf ang");

    compareInt(ps2.iterations,pc.iterations,"iterations");

    minIterations = ps2.iterations;
    if(pc.iterations>minIterations) minIterations = pc.iterations; 

    for(i=0;i!=pc.iterations;i++)
    {
#if 0
        printf("Comparing iteration %d\n",i);
#endif
        compareVec4 (ps2.x+i*4,        pc.x+i*4        ,"x       ");
        compareVec4 (ps2.w+i*4,        pc.w+i*4        ,"w       ");
        
        if(i!=15)
        {
            compareVec4 (ps2.PSMb+i*4,     pc.PSMb+i*4     ,"PSMb    ");
            compareVec4 (ps2.PSMx+i*4,     pc.PSMx+i*4     ,"PSMx    ");
            compareMat44(ps2.PSMA+i*16,    pc.PSMA+i*16    ,"PSMA    ");
            compareMat44(ps2.PSMAchol+i*16,pc.PSMAchol+i*16,"PSMAchol");
        }
    }
}
/* Functions for reading basic types from a MeStream */

void freadMat46(MeReal        A[],  /* Output */
                MeStream      file, /* Input */
                const char *  desc) /* Input */
{
    int i,j;
    char buf[256];

    MeStreamReadLine(buf,256,file);

    MeStreamReadLine(buf,256,file);
	sscanf(buf,"%s \n",desc);

    for(i=0;i!=4;i++)
    {
        MeStreamReadLine(buf,256,file);
	    sscanf(buf,"%f %f %f %f %f %f \n",
            A+0*4+i,
            A+1*4+i,
            A+2*4+i,
            A+3*4+i,
            A+4*4+i,
            A+5*4+i);
    }
}
void freadVec4(MeReal        A[],  /* Output */
               MeStream      file, /* Input */
               const char *  desc) /* Input */
{
    int j;
    char buf[256];

    MeStreamReadLine(buf,256,file);

    //printf("vec4 desc:  %s \n",buf);

    MeStreamReadLine(buf,256,file);
    //printf("vec4 value: %s \n",buf);

    sscanf(buf,"%f %f %f %f \n",A+0,A+1,A+2,A+3);
}
void freadMeReal(MeReal *      x,    /* Output */
                 MeStream      file, /* Input */
                 const char *  desc) /* Input */
{
    char buf[256];

    MeStreamReadLine(buf,256,file);
	sscanf(buf,"%s \n",desc);

    MeStreamReadLine(buf,256,file);
	sscanf(buf,"%f\n",x);
}
void freadMat44(MeReal        A[],  /* Output */
                MeStream      file, /* Input */
                const char *  desc) /* Input */
{
    int i,j;
    char buf[256];

    MeStreamReadLine(buf,256,file);

    MeStreamReadLine(buf,256,file);
	sscanf(buf,"%s \n",desc);

    for(i=0;i!=4;i++)
    {
        MeStreamReadLine(buf,256,file);
	    sscanf(buf,"%f %f %f %f ",A+0*4+i,A+1*4+i,A+2*4+i,A+3*4+i);
    }
}
void freadInt(int *         x,    /* Output */
              MeStream      file, /* Input */
              const char *  desc) /* Input */
{
    char buf[256];

    MeStreamReadLine(buf,256,file);

    MeStreamReadLine(buf,256,file);
	sscanf(buf,"% 014d\n",x);
}
/**
 *  Functions for printing to a file 
 *
*/

void fwriteMat46(int           file, /* Output */
                 const MeReal  A[],  /* Input */
                 const char *  desc) /* Input */
{
    int i,j;
    int count;
    char buf[256];

    count = sprintf(buf,"\n");
    MeWrite(file,buf,count);
    count = sprintf(buf,"%s \n",desc);
    MeWrite(file,buf,count);

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=6;j++)
        {
            count = sprintf(buf,"% 014.6f ",A[j*4+i]);
            MeWrite(file,buf,count);
        }
        count = sprintf(buf,"\n");
        MeWrite(file,buf,count);
    }
    
}
void fwriteVec4(int           file, /* Output */
                const MeReal  A[],  /* Input */
                const char *  desc) /* Input */
{
    int j;
    int count;
    char buf[256];

    count = sprintf(buf,"%s \n",desc);
    MeWrite(file,buf,count);

    for(j=0;j!=4;j++)
    {
        count = sprintf(buf,"% 014.6f ",A[j]);
        MeWrite(file,buf,count);
    }
    count = sprintf(buf,"\n");
    MeWrite(file,buf,count);
}
void fwriteMeReal(int file,         /* Output */
                  MeReal x,         /* Input */
                  const char *desc) /* Input */
{
    int count;
    char buf[256];

    count = sprintf(buf,"%s \n",desc);
    MeWrite(file,buf,count);
    count = sprintf(buf,"% 014.6f\n",x);
    MeWrite(file,buf,count);
}
void fwriteMat44(int           file, /* Output */
                 const MeReal  A[],  /* Input */
                 const char *  desc) /* Input */
{
    int i,j;
    int count;
    char buf[256];

    count = sprintf(buf,"\n");
    MeWrite(file,buf,count);
    count = sprintf(buf,"%s \n",desc);
    MeWrite(file,buf,count);

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=4;j++)
        {
            count = sprintf(buf,"% 014.6f ",A[j*4+i]);
            MeWrite(file,buf,count);
        }
        count = sprintf(buf,"\n");
        MeWrite(file,buf,count);
    }
}
void fwriteInt(int           file, /* Output */
               int           x,    /* Input */
               const char *  desc) /* Input */
{
    int count;
    char buf[256];

    count = sprintf(buf,"%s \n",desc);
    MeWrite(file,buf,count);
    count = sprintf(buf,"% 014d\n",x);
    MeWrite(file,buf,count);
}
/**
 *  Functions for printing to the screen 
 *
*/
void printMeReal(MeReal x,         /* Input */
                 const char *desc) /* Input */
{
    printf("%s ",desc);
    printf("% 014.6f\n",x);
}
void printMat44(const MeReal  A[],  /* Input */
                const char *  desc) /* Input */
{
    int i,j;

    printf("\n");
    printf("%s \n",desc);

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=4;j++)
        {
            printf("% 014.6f ",A[j*4+i]);
        }
        printf("\n");
    }
}

void printMat46(const MeReal  A[],  /* Input */
                const char *  desc) /* Input */
{
    int i,j;

    printf("\n");
    printf("%s \n",desc);

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=6;j++)
        {
            printf("% 014.6f ",A[j*4+i]);
        }
        printf("\n");
    }
}

void printVec4(const MeReal  A[],  /* Input */
               const char *  desc) /* Input */
{
    int j;

    printf("%s \n",desc);

    for(j=0;j!=4;j++)
    {
        printf("% 014.6f ",A[j]);
    }
    printf("\n");
}

void printVec3(const MeReal  A[],  /* Input */
               const char *  desc) /* Input */
{
    int j;

    printf("%s \n",desc);

    for(j=0;j!=3;j++)
    {
        printf("% 014.6f ",A[j]);
    }
    printf("\n");
}
int checkResult(const MeReal J[],      /* Input */
                const MeReal v[],      /* Input */
                const MeReal lambda[]) /* Input */
{
    MeReal jv[4];
    MeReal acc;
    int i,j;
    int passed;

    #define TOL (MeReal)(1e-3)

    for(i=0;i!=4;i++)
    {
        acc = 0.0f;
        for(j=0;j!=3;j++)
        {
            acc = acc + J[j*4+i]*v[j];
        }
        for(j=0;j!=3;j++)
        {
            acc = acc + J[(j+3)*4+i]*v[j+4];
        }
        jv[i] = acc;
    }

    passed = 1;

    for(i=0;i!=4;i++)
    {
        if(lambda[i]<-TOL) 
        {
            printf("lambda %d bad %014.6f\n",i,lambda[i]);
            passed = 0;
        }
    }

    for(i=0;i!=4;i++)
    {
        if(jv[i]<-TOL) 
        {
            printf("phi(newpos) %d bad %014.6f\n",i,jv[i]);
            passed = 0;
        }
    }

    for(i=0;i!=4;i++)
    {
        if(jv[i]>TOL && lambda[i]>TOL)
        {
            printf("Complementarity problem index %d - lambda=%014.6f phi(newpos)=%014.6f\n",
                        i,lambda[i],jv[i]);
            passed = 0;
        }
    }

#if 0
    printConstraintInput(constraintInput);   /* Input */
    printBodyInput(bodyInput);               /* Input */
    printParameters(parameters);             /* Input */
    
    printIntermediateResults(interResults); /* Input */
#endif
    
    //printf("New velocity calculated by test\n");
    //printVec4(jv,"phidot(pnew)");

    if(passed==0)
    {
        printf("Failed\n");
    }    

    return passed;
}
/* Functions for reading constraint/body data from an MeStream */

void readConstraintInputFromFile(carSolverConstraintInput *  c,    /* Output */
                                 MeStream                    file) /* Input */                                
{
    freadMat46(c->J,         file,"J"         );
    freadVec4 (c->xi,        file,"xi        ");
    freadVec4 (c->c,         file,"c         ");
    freadVec4 (c->slipfactor,file,"slipfactor");
}
void readConstraintOutputFromFile(carSolverConstraintOutput *  c,    /* Output */
                                  MeStream                     file) /* Input */
{
    freadVec4(c->lambda,            file,"lambda");
    freadVec4(c->constraintForce+0 ,file,"constraint 0 force ");
    freadVec4(c->constraintForce+4 ,file,"constraint 0 torque");
    freadVec4(c->constraintForce+8 ,file,"constraint 1 force ");
    freadVec4(c->constraintForce+12,file,"constraint 1 torque");
    freadVec4(c->constraintForce+16,file,"constraint 2 force ");
    freadVec4(c->constraintForce+20,file,"constraint 2 torque");
    freadVec4(c->constraintForce+24,file,"constraint 3 force ");
    freadVec4(c->constraintForce+28,file,"constraint 3 torque");
}
void readBodyInputFromFile(carSolverBodyInput *  b,     /* Output */
                           MeStream              file)  /* Input */
{
    freadVec4  (b->bodyForce,            file,"body external force  ");
    freadVec4  (b->bodyTorque,           file,"body external torque ");
    freadVec4  (b->vel,                  file,"body linear velocity ");             
    freadVec4  (b->velrot,               file,"body angular velocity");          
    freadMeReal(&b->invInertiaAndMass[0],file,"inverse inertia");
    freadMeReal(&b->invInertiaAndMass[1],file,"inverse mass   ");
}
void readBodyOutputFromFile(carSolverBodyOutput *  b,    /* Output */
                            MeStream               file) /* Input */
{
    freadVec4(b->bodyForce, file,"body resultant force               ");
    freadVec4(b->bodyTorque,file,"body resultant torque              ");
    freadVec4(b->accel,     file,"body resultant linear acceleration ");             
    freadVec4(b->accelRot,  file,"body resultant angular acceleration");          
}
void readParametersFromFile(carSolverParameters *  p,    /* Output */
                            MeStream               file) /* Input */
{
    freadMeReal(&p->invH,             file,"stepsize                 ");
    freadMeReal(&p->gammaOverHSquared,file,"gamma/(stepsize*stepsize)");
    freadMeReal(&p->epsilon,          file,"epsilon                  ");
    freadMeReal(&p->tol,              file,"tol                      ");
}
void readIntermediateResultsFromFile(carSolverIntermediateResults *  i,    /* Output */                                     
                                     MeStream                        file) /* Input */
{
    int j;
    char buf[256];

    freadMat46(i->JM,  file,"JM  ");
    freadMat44(i->A,   file,"A   ");
    freadVec4 (i->rhs, file,"rhs ");
    freadVec4 (i->vhmf,file,"vhmf lin");
    freadVec4 (i->vhmf+4,file,"vhmf ang");

    freadInt(&i->iterations,file,"iterations");

    for(j=0;j!=i->iterations;j++)
    {
        int tmp;
        
        freadInt(&tmp,file,"index set");

        freadMat44(i->PSMA+j*16,    file,"PSMA");
        freadMat44(i->PSMAchol+j*16,file,"PSMAchol");
        
        MeStreamReadLine(buf,256,file);
                
        freadVec4(i->PSMb+j*4,file,"PSMb");
        freadVec4(i->PSMx+j*4,file,"PSMx");

        MeStreamReadLine(buf,256,file);
        
        freadVec4(i->x+j*4,file,"x");
        freadVec4(i->w+j*4,file,"w");
    }
}
/* Functions for writing constraint/body data to file */

void writeIntermediateResultsToFile(int                                file, /* Output */ 
                                    const carSolverIntermediateResults i)    /* Input */
{
    int j;
    int count;
    char buf[256];

    fwriteMat46(file,i.JM, "JM  ");
    fwriteMat44(file,i.A,  "A   ");
    fwriteVec4(file,i.rhs, "rhs ");
    fwriteVec4(file,i.vhmf,"vhmf lin");
    fwriteVec4(file,i.vhmf+4,"vhmf ang");

    fwriteInt(file,i.iterations,"iterations");

    for(j=0;j!=i.iterations;j++)
    {
        fwriteInt(file,j,"index set");
        fwriteMat44(file,i.PSMA+j*16,"PSMA");
        fwriteMat44(file,i.PSMAchol+j*16,"PSMAchol");
        count = sprintf(buf,"\n");
        MeWrite(file,buf,count);
        fwriteVec4(file,i.PSMb+j*4,"PSMb");
        fwriteVec4(file,i.PSMx+j*4,"PSMx");
        count = sprintf(buf,"\n");
        MeWrite(file,buf,count);
        fwriteVec4(file,i.x+j*4,"x");
        fwriteVec4(file,i.w+j*4,"w");
    }
}
void makeRandArray(MeReal x[],   /* Output */
                   int    count) /* Input  */
{
    int i;

    for(i=0;i!=count;i++)
    {
        x[i] = (((MeReal)(rand()+1))/RAND_MAX)-0.5f;
    }
}

void makeZeroArray(MeReal x[],   /* Output */
                   int    count) /* Input  */
{
    int i;

    for(i=0;i!=count;i++)
    {
        x[i] = 0.0f;
    }
}

MeReal makeRandMeRealPositive()
{
    return ((MeReal)(rand()+1))/RAND_MAX;
}

void testFullRank()
{
    MeReal J[24];
    int i,j;

    for(j=0;j!=6;j++)
    {
        J[4*j+0] = 1.0f;
    }

    for(i=1;i!=4;i++)
    {
        for(j=0;j!=6;j++)
        {
            J[4*j+i] = J[4*j+i-1] * 2.0f;
        }
    }

    printMat46(J,"J");
}

int fullRank(const MeReal J[],    /* Input */
             const MeReal invM[]) /* Input */
{
    MeReal JM[24];
    MeReal A[16];
    MeReal tmp[4];
    int i,j,k;

    for(i=0; i<3; i++)
    {
        JM[4*i]   = J[4*i]   * invM[1];
        JM[4*i+1] = J[4*i+1] * invM[1];
        JM[4*i+2] = J[4*i+2] * invM[1];
        JM[4*i+3] = J[4*i+3] * invM[1];
    }
    for(j=0; j<3; j++)
    {        
        tmp[0]=tmp[1]=tmp[2]=tmp[3]=0.0f;

        for(int i=0; i<3; i++)
        {
            tmp[0] += J[12+4*i]   * invM[0];
            tmp[1] += J[12+4*i+1] * invM[0];
            tmp[2] += J[12+4*i+2] * invM[0];
            tmp[3] += J[12+4*i+3] * invM[0];
        }
        JM[12+4*j]   = tmp[0];
        JM[12+4*j+1] = tmp[1];
        JM[12+4*j+2] = tmp[2];
        JM[12+4*j+3] = tmp[3];
    }

    /* A:= JM*transpose(J) */

    for(i=0;i!=4;i++)
    {
        MeReal acc[4];

        acc[0]=0.0f;
        acc[1]=0.0f;
        acc[2]=0.0f;
        acc[3]=0.0f;

        for(j=0;j!=6;j++)
        {            
            for(k=0;k!=4;k++)
            {
                acc[k] = acc[k] + JM[j*4+k]*J[j*4+i];
            }
        }

        for(k=0;k!=4;k++)
        {
            A[i*4+k] = acc[k];
        }
    }

#if 0
    for(i=0;i!=16;i++) Achol[i]=A[i];

    if(Achol[0+0*4]<0.0f) return 0;
    Q = MeRecipSqrt(Achol[0+0*4]);
    for(i=0;i!=4;i++) Achol[i+0*4] *=Q;

    for(i=1;i!=4;i++) Achol[i+1*4] -= Achol[i+0*4] * Achol[1+0*4];
    if(Achol[1+1*4]<0.0f) return 0;
    Q = MeRecipSqrt(Achol[1+1*4]);
    for(i=1;i!=4;i++) Achol[i+1*4] *=Q;
    		
    for(i=2;i!=4;i++) Achol[i+2*4] = Achol[i+2*4] - Achol[i+0*4]*Achol[2+0*4] - Achol[i+1*4]*Achol[2+1*4];			
    if(Achol[2+2*4]<0.0f) return 0;
    Q = MeRecipSqrt(Achol[2+2*4]);
    for(i=2;i!=4;i++) Achol[i+2*4] *=Q;
    
    Achol[3+3*4] = Achol[3+3*4] - Achol[3+0*4]*Achol[3+0*4] - Achol[3+1*4]*Achol[3+1*4] - Achol[3+2*4]*Achol[3+2*4];
    if(Achol[3+3*4]<0.0f) return 0;
    Q = MeRecipSqrt(Achol[3+3*4]);
    Achol[3+3*4] *=Q;
#endif

#if 0
    det = Achol[4*0+0];
    for(i=1;i!=4;i++)
    {
        det = det * Achol[4*i+i];
    }
    det=det*det;
    printf("det(A)=% 12.10f\n",det);

    if(det<0.001) return 0;
#endif

    return 1;
}   
void makeRandFullRank46Mat(MeReal        J[],    /* Output */
                           const MeReal  invM[]) /* Input  */
{
    makeRandArray(J,24);

#if 0
    while(!fullRank(J,invM))
    {
        makeRandArray(J,24);
    }
#endif
}
void printConstraintInput(const carSolverConstraintInput c) /* Input */
{ 
    printMat46(c.J,"J");
    printVec4(c.xi,        "xi        ");
    printVec4(c.c,         "c         ");
    printVec4(c.slipfactor,"slipfactor");
}

void writeConstraintInputToFile(int                            file, /* Output */
                                const carSolverConstraintInput c)    /* Input */
{
    fwriteMat46(file,c.J,"J");
    fwriteVec4(file,c.xi,        "xi        ");
    fwriteVec4(file,c.c,         "c         ");
    fwriteVec4(file,c.slipfactor,"slipfactor");
}

void printConstraintOutput(const carSolverConstraintOutput c) /* Input */
{
    printVec4(c.lambda,"lambda");
    printVec4(c.constraintForce+0 ,"constraint 0 force ");
    printVec4(c.constraintForce+4 ,"constraint 0 torque");
    printVec4(c.constraintForce+8 ,"constraint 1 force ");
    printVec4(c.constraintForce+12,"constraint 1 torque");
    printVec4(c.constraintForce+16,"constraint 2 force ");
    printVec4(c.constraintForce+20,"constraint 2 torque");
    printVec4(c.constraintForce+24,"constraint 3 force ");
    printVec4(c.constraintForce+28,"constraint 3 torque");
}

void writeConstraintOutputToFile(int                             file, /* Output */
                                 const carSolverConstraintOutput c)    /* Input */
{
    fwriteVec4(file,c.lambda,"lambda");
    fwriteVec4(file,c.constraintForce+0 ,"constraint 0 force ");
    fwriteVec4(file,c.constraintForce+4 ,"constraint 0 torque");
    fwriteVec4(file,c.constraintForce+8 ,"constraint 1 force ");
    fwriteVec4(file,c.constraintForce+12,"constraint 1 torque");
    fwriteVec4(file,c.constraintForce+16,"constraint 2 force ");
    fwriteVec4(file,c.constraintForce+20,"constraint 2 torque");
    fwriteVec4(file,c.constraintForce+24,"constraint 3 force ");
    fwriteVec4(file,c.constraintForce+28,"constraint 3 torque");
}

void printBodyInput(const carSolverBodyInput b) /* Input */
{
    printVec4(b.bodyForce, "body external force  ");
    printVec4(b.bodyTorque,"body external torque ");
    printVec4(b.vel,       "body linear velocity ");             
    printVec4(b.velrot,    "body angular velocity");          
    printMeReal(b.invInertiaAndMass[0],"inverse inertia");
    printMeReal(b.invInertiaAndMass[1],"inverse mass   ");
}

void writeBodyInputToFile(int                      file, /* Output */
                          const carSolverBodyInput b)    /* Input */
{
    fwriteVec4(file,b.bodyForce, "body external force  ");
    fwriteVec4(file,b.bodyTorque,"body external torque ");
    fwriteVec4(file,b.vel,       "body linear velocity ");             
    fwriteVec4(file,b.velrot,    "body angular velocity");          
    fwriteMeReal(file,b.invInertiaAndMass[0],"inverse inertia");
    fwriteMeReal(file,b.invInertiaAndMass[1],"inverse mass   ");
}

void printBodyOutput(const carSolverBodyOutput b) /* Input */
{
    printVec4(b.bodyForce, "body resultant force               ");
    printVec4(b.bodyTorque,"body resultant torque              ");
    printVec4(b.accel,     "body resultant linear acceleration ");             
    printVec4(b.accelRot,  "body resultant angular acceleration");          
}

void writeBodyOutputToFile(int                       file, /* Output */
                           const carSolverBodyOutput b)    /* Input */
{
    fwriteVec4(file,b.bodyForce, "body resultant force               ");
    fwriteVec4(file,b.bodyTorque,"body resultant torque              ");
    fwriteVec4(file,b.accel,     "body resultant linear acceleration ");             
    fwriteVec4(file,b.accelRot,  "body resultant angular acceleration");          
}

void printParameters(const carSolverParameters p) /* Input */
{
    printMeReal(p.invH,             "stepsize                 ");
    printMeReal(p.gammaOverHSquared,"gamma/(stepsize*stepsize)");
    printMeReal(p.epsilon,          "epsilon                  ");
    printMeReal(p.tol,              "tol                      ");
}

void writeParametersToFile(int                       file, /* Output */
                           const carSolverParameters p)    /* Input */
{
    fwriteMeReal(file,p.invH,             "stepsize                 ");
    fwriteMeReal(file,p.gammaOverHSquared,"gamma/(stepsize*stepsize)");
    fwriteMeReal(file,p.epsilon,          "epsilon                  ");
    fwriteMeReal(file,p.tol,              "tol                      ");
}

void printIntermediateResults(const carSolverIntermediateResults i) /* Input */
{
    int j;

    printMat46(i.JM, "JM  ");
    printMat44(i.A,  "A   ");
    printVec4(i.rhs, "rhs ");
    printVec4(i.vhmf,"vhmf");

    printf("iterations %2d\n",i.iterations);

    for(j=0;j!=i.iterations;j++)
    {
        MeReal xdotw;
        MeReal det;
        int k;

        printf("index set %2d\n",j);
        printf("------------\n");
        printMat44(i.PSMA+j*16,"PSMA");
        printMat44(i.PSMAchol+j*16,"PSMAchol");
        printf("\n");
        printVec4(i.PSMb+j*4,"PSMb");
        printVec4(i.PSMx+j*4,"PSMx");
        printf("\n");
        printVec4(i.x+j*4,"x");
        printVec4(i.w+j*4,"w");

        xdotw=0;
        for(k=0;k!=4;k++)
        {
            xdotw += i.x[j*4+k] * i.w[j*4+k];
        }

        printMeReal(xdotw,"xdotw");

        det = i.PSMAchol[j*16+4*0+0];
        for(k=1;k!=4;k++)
        {
            det = det * i.PSMAchol[j*16+4*k+k];
        }
        det=det*det;
        printf("det(PSMA)=% 12.10f\n",det);
#if 0
        testCholOutput(i.PSMAchol+j*16,i.PSMA+j*16);
#endif
        printf("\n");
    }
}
void matvecmul(MeReal        b[], /* Output */
               const MeReal  A[], /* Input */
               const MeReal  x[]) /* Input */
{
    int i,j;

    for(i=0;i!=4;i++)
    {
        MeReal acc = 0.0f;

        for(j=0;j!=4;j++)
        {
            acc = acc + A[i+j*4]*x[j];
        }
        b[i]=acc;
    }
}
void matmul(MeReal        A[], /* Output */
            const MeReal  B[], /* Input  */
            const MeReal  C[]) /* Input  */
{
    int i,j,k;

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=4;j++)
        {
            MeReal acc = 0.0f;
            for(k=0;k!=4;k++)
            {
                acc = acc + B[k*4+j] * C[i*4+k];
            }
            A[j+i*4] = acc;
        }
    }
}

void vecsub4(MeReal        A[], /* Output */
             const MeReal  B[], /* Input  */
             const MeReal  C[]) /* Input  */
{
    int i;

    for(i=0;i!=4;i++)
    {
        A[i]=B[i]-C[i];
    }
}
void vecsub3(MeReal        A[], /* Output */
             const MeReal  B[], /* Input  */
             const MeReal  C[]) /* Input  */
{
    int i;

    for(i=0;i!=3;i++)
    {
        A[i]=B[i]-C[i];
    }
}

void matsub44(MeReal        A[], /* Output */
              const MeReal  B[], /* Input  */
              const MeReal  C[]) /* Input  */
{
    int i;

    for(i=0;i!=16;i++)
    {
        A[i]=B[i]-C[i];
    }
}

void matsub46(MeReal        A[], /* Output */
              const MeReal  B[], /* Input  */
              const MeReal  C[]) /* Input  */
{
    int i;

    for(i=0;i!=24;i++)
    {
        A[i]=B[i]-C[i];
    }
}
MeReal matnorm46(const MeReal A[])
{
    int i;
    MeReal norm;

    norm = 0.0f;

    for(i=0;i!=24;i++)
    {
        norm = norm + A[i]*A[i];
    }

    norm = MeSqrt(norm);

    return norm;
}
MeReal matnorm44(const MeReal A[])
{
    int i;
    MeReal norm;

    norm = 0.0f;

    for(i=0;i!=16;i++)
    {
        norm = norm + A[i]*A[i];
    }

    norm = MeSqrt(norm);

    return norm;
}
MeReal vecnorm4(const MeReal A[])
{
    int i;
    MeReal norm;

    norm = 0.0f;

    for(i=0;i!=4;i++)
    {
        norm = norm + A[i]*A[i];
    }

    norm = MeSqrt(norm);

    return norm;
}
MeReal vecnorm3(const MeReal A[])
{
    int i;
    MeReal norm;

    norm = 0.0f;

    for(i=0;i!=3;i++)
    {
        norm = norm + A[i]*A[i];
    }

    norm = MeSqrt(norm);

    return norm;
}