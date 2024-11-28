/* -*- mode: C++; -*- */ 
/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/11/05 11:23:44 $ - Revision: $Revision: 1.45.2.4.4.1 $

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
#include <keaDebug.h>
#include <stdio.h>
#include <MeMath.h>
#include <keaPrintBasicTypes.h>

#ifdef PS2
#include "calcA.hpp"
#include "JM_block.hpp"
#include "cf_block.hpp"
#endif /* PS2 */

void printKeaInput(const MdtKeaConstraints constraints,const MdtKeaParameters parameters,
                   const MdtKeaBody *const blist[], int num_bodies)
{
#if PRINT_KEA_INPUT_CONSTRAINTS

    int i,j,k;

    MeInfo(0,"KeaConstraints");
    MeInfo(0,"--------------");

#if PRINT_KEA_SIZES
    MeInfo(0,"constraints->num_partitions=%d;",
        constraints.num_partitions);

    for (i = 0; i != constraints.num_partitions; i++)
        MeInfo(0,"constraints->num_rows_exc_padding_partition[%d]=%d;\n",i,constraints.num_rows_exc_padding_partition[i]);

    for (i = 0; i != constraints.num_partitions; i++)
        MeInfo(0,"constraints->num_rows_inc_padding_partition[%d]=%d;\n",i,constraints.num_rows_inc_padding_partition[i]);

    for (i = 0; i != constraints.num_partitions; i++)
        MeInfo(0,"constraints.num_constraints_partition[%d]=%d;\n",i,constraints.num_constraints_partition[i]);

    MeInfo(0,"constraints->num_rows_exc_padding=%d;", constraints.num_rows_exc_padding);
    MeInfo(0,"constraints->num_rows_inc_padding=%d;", constraints.num_rows_inc_padding);

    MeInfo(0,"constraints->num_constraints=%d;",
        constraints.num_constraints);
#endif

#if PRINT_XI
    for (i = 0; i != constraints.num_rows_exc_padding; i++)
        MeInfo(0,"constraints->xi[%d]=%12.6f;",i,constraints.xi[i]);
#endif

#if PRINT_C
    for (i = 0; i != constraints.num_rows_exc_padding; i++)
        MeInfo(0,"constraints->c[%d]=%12.6f;",i,constraints.c[i]);
#endif

#if PRINT_LO
    for (i = 0; i != constraints.num_rows_exc_padding; i++)
        MeInfo(0,"constraints->lo[%d]=%12.6f;",i,constraints.lo[i]);
#endif

#if PRINT_HI
    for (i = 0; i != constraints.num_rows_exc_padding; i++)
        MeInfo(0,"constraints->hi[%d]=%12.6f;",i,constraints.hi[i]);
#endif

#if PRINT_SLIPFACTOR
    for (i = 0; i != constraints.num_rows_exc_padding; i++)
        MeInfo(0,"constraints->slipfactor[%d]=%12.6f;",i,constraints.slipfactor[i]);
#endif

#if PRINT_XGAMMA
    for (i = 0; i != constraints.num_rows_exc_padding; i++)
        MeInfo(0,"constraints->xgamma[%d]=%12.6f;",i,constraints.xgamma[i]);
#endif

#if PRINT_JSIZE
    for (i = 0; i != constraints.num_constraints; i++)
        MeInfo(0,"%1d ",i,constraints.Jsize[i]);
    MeInfo(0,"");
#endif

#if PRINT_JOFS
    for (i = 0; i != constraints.num_constraints; i++)
        MeInfo(0,"constraints->Jofs[%d]=%d; ",i,constraints.Jofs[i]);
#endif

#if PRINT_JBODY
    for (i = 0; i != 2 * constraints.num_constraints; i++)
        MeInfo(0,"constraints->Jbody[%d]=%d; ",i,constraints.Jbody[i]);
#endif

#if PRINT_J
    /*
       Print the input J matrix
    */

    for (i = 0; i != constraints.num_rows_inc_padding / 4; i++)
    {
        for (j = 0; j != 4; j++)
        {
            for (k = 0; k != 12; k++)
                MeInfo(0,"% 6.3f; ", constraints.Jstore[j + k * 4 + i * 48]);

            MeInfo(0,"");
        }

        MeInfo(0,"");
    }
#endif

#if PRINT_JNORMS
    MeInfo(0,"J norms");

    MeReal *Jp=constraints.Jstore;

    for(i=0;i!=constraints.num_rows_inc_padding/4;i++)
    {
        for(j=0;j!=2;j++)
        {
            MeReal norm=0.0f;
            for(k=0;k!=24;k++)
            {
                norm = norm + (*Jp)*(*Jp);
                Jp++;
            }
            MeInfo(0,"%12.6f ",norm);
        }
        MeInfo(0,"");
    }
#endif

#endif

#if PRINT_KEA_INPUT_BODIES
    MeInfo(0,"keaBodies");
    MeInfo(0,"---------");

    for(i=0;i!=num_bodies;i++)
    {
        const MdtKeaBody *const b = blist[i];

        MeInfo(0," blist[i] %d:",i);
        MeInfo(0,"  invmass=%12.6f flags=%d",
            b->invmass,b->flags);
        printVec4(b->force,"force");
        printVec4(b->torque,"torque");
        printVec4(b->invI0,"invI0");
        printVec4(b->invI1,"invI1");
        printVec4(b->invI2,"invI2");
        printVec4(b->I0,"I0");
        printVec4(b->I1,"I1");
        printVec4(b->I2,"I2");
        printVec4(b->vel,"vel");
        printVec4(b->velrot,"velrot");
        printVec4(b->qrot,"qrot");
        printVec4(b->accel,"accel"); 
        printVec4(b->accelrot,"accelrot");
        printVec4(b->fastSpinAxis,"fastSpinAxis");
    }
#endif

#if PRINT_KEA_INPUT_PARAMETERS
    MeInfo(0,"MdtKeaParameters");
    MeInfo(0,"-------------");
    MeInfo(0,"gamma   =%12.6f", parameters.gamma);
    MeInfo(0,"epsilon =%12.6f", parameters.epsilon);
    MeInfo(0,"stepsize=%12.6f", parameters.stepsize);
#endif

}
void printFactoriserInput(const MeReal *newA,const MeReal *lo,const MeReal *hi,int ceil4_num_rows)
{
    int i,j;

    for (i = 0; i != ceil4_num_rows; i++)
    {
        for (j = 0; j != ceil4_num_rows; j++)
            MeInfo(0,"%12.6f ",
                newA[((j / 4) * ceil4_num_rows + i) * 4 + j % 4]);

        MeInfo(0,"");
    }
    MeInfo(0,"");

    MeInfo(0,"lo         =");
    for (i = 0; i != ceil4_num_rows; i++)
        MeInfo(0,"%17.6f ", lo[i]);
    MeInfo(0,"\n");

    MeInfo(0,"hi         =");
    for (i = 0; i != ceil4_num_rows; i++)
        MeInfo(0,"%17.6f ", hi[i]);
    MeInfo(0,"\n");
}


void printJlenandBl2BodyOutput(const int *jlen,const int *bl2body,const int *bl2cony,int num_strips)
{
    int i,j;

    printf("Jlen and Bl2Body Output\n");
    printf("-----------------------\n");

    printf("num_strips=%d\n",num_strips);

    printf("bl2body:\n");
    for(i=0;i!=num_strips;i++)
    {
        printf("body:");
        for(j=0;j!=jlen[i];j++)
        {
            printf("% d ",bl2body[i*8+j]);
        }
        printf("\ncnst:");
        for(j=0;j!=jlen[i];j++)
        {
            printf("% d ",bl2cony[i*8+j]);
        }
        printf("\n");
    }
    printf("jlen:\n");
    for(i=0;i!=num_strips;i++)
    {
        printf("%08x: %2d\n",(unsigned int)(jlen+i),jlen[i]);
    }
}

void printCalcIworldNonInertialForceandVhmfInput(const MdtKeaBody *blist,int num_bodies)
{
    int i,j;

    printf("CalcIworldNonInertialForceandVhmf Input\n");
    printf("---------------------------------------\n");

    printf("v\n");
    for(i=0;i!=num_bodies;i++)
    {
        for(j=0;j!=8;j++)
        {
            printf("v[%3d]=%12.6f\n",i*8+j,blist[i].vel[j]);
        }
        printf("\n");
    }
    printf("f\n");
    for(i=0;i!=num_bodies;i++)
    {
        for(j=0;j!=8;j++)
        {
            printf("f[%3d]=%12.6f\n",i*8+j,blist[i].force[j]);
        }
        printf("\n");
    }

}

void printCalcIworldNonInertialForceandVhmfOutput(const MeReal *vhmf,const MeReal *invIworld,int num_bodies)
{
    int i,j,k;

    printf("CalcIworldNonInertialForceandVhmf Output\n");
    printf("----------------------------------------\n");
    printf("invIworld\n");

    for(i=0;i!=num_bodies;i++)
    {
        for(j=0;j!=3;j++)
        {
            for(k=0;k!=4;k++)
            {
                printf("%12.6f ",invIworld[i*12+j*4+k]);
            }
            printf("\n");
        }
        printf("\n");
    }

    printf("vhmf\n");
    for(i=0;i!=num_bodies*8;i++)
    {
        printf("vhmf[%d]=%12.6f\n",i,vhmf[i]);
    }

}

void printJinvMandrhsInput(
    const MeReal                  rhs[],                /* Input */
    const MdtKeaJBlockPair        jmstore[],            /* Input */
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
    printf("JinvMandrhs Input\n");
    printf("-----------------\n");

    printPtr(rhs,"rhs");
    printPtr(jmstore,"jm");

    printMdtKeaJBlockPairArray(jstore,num_rows_inc_padding);

    printVec(xgamma,num_rows_exc_padding,"xgamma");
    printVec(c,num_rows_exc_padding,"c");
    printVec(xi,num_rows_exc_padding,"xi");
    printInvMassMatrixArray(invIworld,num_bodies,"invIworld");
    printMdtKeaBl2BodyRowArray(bl2body,num_rows_exc_padding);
    printIntVec(jlen,num_rows_exc_padding/4,"jlen");
    printMdtKeaVelocityArray(vhmf,num_bodies,"vhmf");
    printMeReal(stepsize,"stepsize");
    printMeReal(gamma,"gamma");
}
void printJinvMandrhsOutput(
         const MeReal           rhs[],
         const MdtKeaJBlockPair jm[],
         int                    num_rows, 
         const int              jlen[],
         int                    num_rows_inc_padding)
{
    int i;

    printf("JinvMandrhs Output\n");
    printf("-----------------\n");

    printf("rhs=%p\n",rhs);
    printf("jm =%p\n",jm);

    for(i=0;i!=num_rows;i++)
    {
        printf("rhs[%3d]=%12.6f\n",i,rhs[i]);
    }
    printf("\n");

    printMdtKeaJBlockPairArray(
        jm,                    /* Input */
        num_rows_inc_padding); /* Input */

}

void printCalculateConstraintForcesOutput(
        MdtKeaBody *const      blist[],
        const MdtKeaForcePair  cforces[],
        int                    num_bodies,
        int                    num_constraints)
{
    printf("calculateConstraintAndResultantForces Output\n");
    printf("--------------------------------------------\n");

#if 0
    for(i=0;i!=num_bodies;i++)
    {
        for(j=0;j!=8;j++)
        {
            printf("body[%2d].force[%1d]=%12.6f\n",i,j,blist[i]->force[j]);
        }
        printf("\n");
    }
#endif
    printMdtKeaForcePairArray(cforces,        /* Input */
                              num_constraints);/* Input */
}

void printCalcJinvMJTInput(const blocktobodyandlen *jinfo,const blocktobodyandlen *jminfo,
                           const MeReal *J,const MeReal *JM,int num_blocks)
{
    int i,j,k,chunk,strip,block,elt;

    printf("CalcJinvMJT input\n");
    printf("-----------------\n");

    printf("\n");
    printf("num_blocks=%d\n",num_blocks);
    printf("\n");

    printf("norms of jm strips\n");

    const MeReal *pJM               = JM;
    const blocktobodyandlen *pjminfo = jminfo;

    for(chunk=0;chunk!=num_blocks;chunk++)
    {
        for(strip=0;strip!=3;strip++)
        {
            for(block=0;block!=pjminfo->len[strip];block++)
            {
                MeReal norm=0.0f;
                for(elt=0;elt!=24;elt++)
                {
                    norm = norm+(*pJM)*(*pJM);
                    pJM++;
                }
                printf("%8.2f ",norm);
            }
            printf("\n");
        }
        pjminfo++;
    }
    printf("norms of j strips\n");

    const MeReal *pJ                = J;
    const blocktobodyandlen *pjinfo = jinfo;

    for(chunk=0;chunk!=num_blocks;chunk++)
    {
        for(strip=0;strip!=3;strip++)
        {
            for(block=0;block!=pjinfo->len[strip];block++)
            {
                MeReal norm=0.0f;
                for(elt=0;elt!=24;elt++)
                {
                    norm = norm+(*pJ)*(*pJ);
                    pJ++;
                }
                printf("%8.2f ",norm);
            }
            printf("\n");
        }
        pjinfo++;
    }

    printf("jmblocktobodyandlen is at %p\n",jminfo);
    for(i=0;i!=num_blocks;i++)
    {
        for(j=0;j!=3;j++)
        {
            for(k=0;k!=8;k++)
            {
                printf("%2d",jminfo[i].blocktobody[j*8+k]);
            }
            printf("\n");
        }
        for(k=0;k!=3;k++)
        {
            printf("%2d",jminfo[i].len[k]);
        }
        printf("\n");
        printf("%2d\n",jminfo[i].qwc);
    }

    printf("jblocktobodyandlen is at %p\n",jinfo);
    for(i=0;i!=num_blocks;i++)
    {
        for(j=0;j!=3;j++)
        {
            for(k=0;k!=8;k++)
            {
                printf("%2d",jinfo[i].blocktobody[j*8+k]);
            }
            printf("\n");
        }
        for(k=0;k!=3;k++)
        {
            printf("%2d",jinfo[i].len[k]);
        }
        printf("\n");
        printf("%2d\n",jinfo[i].qwc);
    }

}

void printFactoriseps2sparse_Output(
         const MeReal A[],
         const int    rlist[],
         const int    rlist_len[],
         int          num_blocks)
{
    int i,j,k;
    const MeReal *pa;

    printf("ps2Sparse factoriser output\n");
    printf("---------------------------\n");
    printf("norms of Achol blocks (not necessarily in the order they are the actual matrix)\n");

    pa = A;
    for(i=0;i!=num_blocks;i++)
    {
        for(j=0;j!=i+1;j++)
        {
            MeReal norm = 0.0f;
            for(k=0;k!=144;k++)
            {
                norm = norm + pa[k] * pa[k];
            }
            printf("%12.6f ",norm);

            pa = pa + 144;
        }
        printf("\n");
    }

    printPS2SparseMatrix(
         A,
         rlist,
         rlist_len,
         num_blocks);
}

void printCalcJinvMJTps2sparse_Output(
         const MeReal  A[],
         const int     rlist[],
         const int     rlist_len[],
         int           num_blocks)
{
    int i,j,k;
    const MeReal *pa;

    printf("ps2sparse CalcJinvMJT output\n");
    printf("----------------------------\n");

#if 0
    for (i = 0; i != num_rows; i++)
    {
        for (j = 0; j != num_rows; j++)
            printf("%12.6f ",
                A[((j / 4) * num_rows + i) * 4 + j % 4]);

        printf("\n");
    }
#endif

    pa = A;
    for(i=0;i!=num_blocks;i++)
    {
        for(j=0;j!=i+1;j++)
        {
            MeReal norm = 0.0f;
            for(k=0;k!=144;k++)
            {
                norm = norm + pa[k] * pa[k];
            }
            printf("%12.6f ",norm);

            pa = pa + 144;
        }
        printf("\n");
    }

    for(i=0;i!=num_blocks;i++)
    {
        for(j=0;j!=rlist_len[i];j++)
        {
            printf("%d ",rlist[i*num_blocks+j]);
        }
        printf("\n");
    }

    for(i=0;i!=num_blocks;i++)
    {
        printf("len=%d\n",rlist_len[i]);
    }

    printPS2SparseMatrix(
         A,
         rlist,
         rlist_len,
         num_blocks);
}

void printLCPInitialSolveOutput(const MeReal *x,int c4n)
{
    int i;

    printf("LCP Initial Solve Output\n");
    printf("------------------------\n");

    for(i=0;i!=c4n;i++)
    {
        printf("%12.6f\n",x[i]);
    }
}
void printSolvePCSparseSSEOutput(const MeReal x[],
                                 int          m_padded)
{
    printf("PCSparseSSE solve output\n");
    printf("------------------------\n");

    printVec(x,m_padded,"solve result");
    printf("\n");
}
void printCalcConstraintForcesInput(const MeReal              lambda[],
                                    const MdtKeaBl2CBodyRow   bl2cbody[],
                                    const MdtKeaBl2BodyRow    bl2body[],
                                    int                       c4size)
{
    int i;

    printf("calculateConstraintAndResultantForces Input\n");
    printf("-------------------------------------------\n");

    printf("lambda:\n");
    for(i=0;i!=c4size;i++)
    {
        printf("% 12.6f\n",lambda[i]);
    }
    printf("\n");
    printf("bl2cbody:\n");

    printMdtKeaBl2CBodyRowArray(bl2cbody,   /* Input */
                                c4size/4);  /* Input */

    printf("\n");

}

void printMakeFromColMajorPSMInput(
        const MeReal Ainv[],            /* Input */
        const MeReal clampedValues[],   /* Input */
        const MeReal initialSolve[],    /* Input */
        const int    unclamped[],       /* Input */
        const int    clamped[],         /* Input */
        int          numUnclamped,      /* Input */
        int          numClamped,        /* Input */
        int          n_padded,          /* Input */
        int          AinvStride)        /* Input */
{
    printf("PPT make Q input\n");
    printf("----------------\n");
    printf("\n");
    printf("numUnclamped=%d\n",numUnclamped);
    printf("numClamped=%d\n",numClamped);
    printf("\n");

    printIntVec(clamped,numClamped,"clamped");
    printColMajorMat(Ainv,        /* Input */
                     AinvStride,  /* Input */
                     AinvStride,  /* Input */
                     "Ainv");     /* Input */

    printf("\n");
}
void printMakeFromColMajorPSMOutput(const MeReal         rsD[],
                                    const MeReal *const  NAZ[],
                                    const MeReal *const  NCZ[],
                                    const int            NR[],
                                    const int            NC[],
                                    int                  m_blocks,
                                    int                  m_padded)
{
    int i,j,k;

    printf("PCsparse make Q output\n");
    printf("----------------------\n");

    for(i=0;i!=m_blocks;i++)
    {
        for(j=0;j!=m_blocks;j++)
        {
            printf("%08x ",NCZ[m_blocks*i+j]);
        }
        printf("\n");
    }

    printf("\n");
    printf("Q:\n");

    for(i=0; i<m_padded; i++)
    {
        for(j=0; j<m_blocks; j++)
        {
            if(NCZ[j*(m_blocks)+(i>>2)]!=0)
            {
                for(k=0; k<4; k++)
                    if(4*j+k>=i)
                        printf("% 6.2f ", (NCZ[j*(m_blocks)+(i>>2)])[4*(i&3)+k]);
                    else
                        printf("% 6.2f ",0.0f);
            }
            else
            {
                for(k=0; k<4; k++)
                    printf("0.00 ");
            }
        }
        printf("\n");
    }
    printf("\n");

}
void printMakeFromColMajorPSMOutput_ps2smalldense(
        MeReal Q[],        /* Input */
        int    c4numRows)  /* Input */
{
    printf("PS2 smalldense MakeFromColMajorPSM Output\n");
    printf("-----------------------------------------\n");

    printHalfWallpaperMatrix(Q,         /* Input */
                             c4numRows);/* Input */
}

void printMakeFromColMajorPSMOutput_ps2sparse(
         const MeReal matrix[],
         const int    rlist[],
         const int    rlist_len[],
         int          num_12_blocks)
{
    printf("PS2 sparse MakeFromColMajorPSM Output\n");
    printf("-------------------------------------\n");

    printPS2SparseMatrix(
         matrix,
         rlist,
         rlist_len,
         num_12_blocks);

}

void printFactoriserPS2SmallDenseOutput(const MeReal A[],
                                        int          c4numRows)
{
    printf("PS2SmallDense Factoriser Output\n");
    printf("-------------------------------\n");

    printHalfWallpaperMatrix(A,         /* Input */
                             c4numRows);/* Input */
}
void printFactoriserPCSparseOutput(MeReal *  NCZ[],
                                   int       m_blocks,
                                   int       m_padded)
{
    int i,j,k;

    printf("PC Sparse Factoriser Output\n");
    printf("---------------------------\n");
    printf("\n");

    for(i=0; i<m_padded; i++)
    {
        for(j=0; j<m_blocks; j++)
        {
            if(NCZ[j*(m_blocks)+(i>>2)]!=0)
            {
                for(k=0; k<4; k++)
                    if(4*j+k>=i)
                        printf("%4.2f ", (NCZ[j*(m_blocks)+(i>>2)])[4*(i&3)+k]);
                    else
                        printf("x.xx ");
            }
            else
            {
                for(k=0; k<4; k++)
                    printf("0.00 ");
            }
        }
        printf("\n");
    }
    printf("\n");

    printf("\n");
}
void printPrinciplePivotTransformOutput(const MeReal x[],
                                        const MeReal w[],
                                        int          n)
{
    printf("PPT output\n");
    printf("\n");
    printVec(x,n,"x");
    printVec(w,n,"w");
    printf("\n");
}
void printGetFirstBadIndexInput(const MeReal x[],
                                int          n)
{
    printf("getFirstBadIndexInput\n");
    printf("---------------------\n");
    printVec(x,MeMathCEIL4(n),"initial solve result");
    printf("\n");
}
void printSolveInput(const MeReal rhs[],
                     int          num_blocks)
{
    printf("solve input\n");
    printf("-----------\n");
    printVec(rhs,num_blocks*4,"rhs");
    printf("\n");
}

void printLCPOutput(const MeReal x[],
                    int   c4numRows)
{
    printVec(x,c4numRows,"lambda");
}

#ifdef PS2
void printJinvMandrhsBlockInput(newCalc_JM_buf *buf)
{
    MeReal *gamma   = buf->gamma;
    MeReal *xi      = buf->xi_rhs;
    MeReal *c       = buf->c;
    MeReal *j_block = buf->j_m_and_vhmf;
    int *j_bl2body  = buf->j_bl2body;

    int num_strips = 1;

    printf("JinvMandrhs Block Input\n");
    printf("-----------------------\n");

    int i,j,k;

    printf("%08x bl2body: ",(unsigned int)j_bl2body);for(i=0;i!=2*4*num_strips    ;i++) printf("%d "   ,j_bl2body[i]);printf("\n");
    printf("%08x gamma:   \n",(unsigned int)gamma    );for(i=0;i!=1*4*num_strips    ;i++) printf("%12.6f\n",gamma[i]    );printf("\n");
    printf("%08x xi:      \n",(unsigned int)xi       );for(i=0;i!=1*4*num_strips    ;i++) printf("%12.6f\n",xi[i]       );printf("\n");   
    printf("%08x c:       \n",(unsigned int)c        );for(i=0;i!=1*4*num_strips    ;i++) printf("%12.6f\n",c[i]        );printf("\n");
    printf("\n");
    printf("%08x:\n",(unsigned int)j_block);
    for(i=0;i!=num_strips;i++)
    {
        for(k=0;k!=4;k++)
        {
            for(j=0;j!=12;j++)
            {
                printf("% 6.3f ",j_block[i*48+j*4+k]);
            }
            printf("\n");
        }
        printf("\n");
    }
#if 0 
    for(j=0;j!=num_jblocks;j++)
    {
        MeReal *m = (MeReal *)&(m_vhmf[j].m);
        MeReal *v = (MeReal *)&(m_vhmf[j].vhmf);
        printf("M:     ");for(i=0;i!=3*4;i++) printf("%6.0f ",m[i]);printf("\n");   
        printf("VHMF:  ");for(i=0;i!=2*4;i++) printf("%6.0f ",v[i]);printf("\n");
    }
#endif
}
void printJinvMJTBlockInput(Calc_block_buf* b)
{
    int strip,block,elt,j,k;
    MeReal *Jp=b->jmandj;

    printf("calc JinvMJT block input\n");
    for(strip=0;strip!=3;strip++)
    {
        printf("Jlen[%d]=%08x\n",strip,(unsigned int)b->j_len[strip]);
    }
    for(strip=0;strip!=3;strip++)
    {
        printf("JMlen[%d]=%08x\n",strip,(unsigned int)b->jm_len[strip]);
    }

    printf("JM block to body\n");
    for(j=0;j!=3;j++)
    {
        for(k=0;k!=8;k++)
        {
            printf("%2d",b->jm_bl2body[j*8+k]);
        }
        printf("\n");
    }


    printf("J block to body\n");
    for(j=0;j!=3;j++)
    {
        for(k=0;k!=8;k++)
        {
            printf("%2d",b->j_bl2body[j*8+k]);
        }
        printf("\n");
    }

#if 0
    printf("jm strips\n");
    for(strip=0;strip!=3;strip++)
    {
        for(row=0;row!=4;row++)
        {
            for(block=0;block!=b->jm_len[strip];block++)
            {
                for(col=0;col!=6;col++)
                {
                    printf("%5.2f ",Jp[row+block*24+col*4]);
                }
                printf(" ");
            }
            printf("\n");
        }
        printf("\n");
        Jp+=24*b->jm_len[strip];
    }

    printf("j strips\n");
    for(strip=0;strip!=3;strip++)
    {
        for(row=0;row!=4;row++)
        {
            for(block=0;block!=b->j_len[strip];block++)
            {
                for(col=0;col!=6;col++)
                {
                    printf("%5.2f ",Jp[row+block*24+col*4]);
                }
                printf(" ");
            }
            printf("\n");
        }
        printf("\n");
        Jp+=24*b->j_len[strip];
    }
#endif
    Jp=b->jmandj;

    printf("norms of jm strips\n");
    for(strip=0;strip!=3;strip++)
    {
        for(block=0;block!=b->jm_len[strip];block++)
        {
            MeReal norm=0.0f;
            for(elt=0;elt!=24;elt++)
            {
                norm = norm+(*Jp)*(*Jp);
                Jp++;
            }
             printf("%8.2f ",norm);
        }
        printf("\n");
    }
    printf("norms of j strips\n");
    for(strip=0;strip!=3;strip++)
    {
        for(block=0;block!=b->j_len[strip];block++)
        {
            MeReal norm=0.0f;
            for(elt=0;elt!=24;elt++)
            {
                norm = norm+(*Jp)*(*Jp);
                Jp++;
            }
             printf("%8.2f ",norm);
        }
        printf("\n");
    }

}
/*
 * shape==0 - shape is square
 * shape==1 - shape is triangle
*/
void printJinvMJTBlockOutput(Calc_block_buf* b,int shape,int foundmatch)
{
    printf("Calc block result\n");
    printf("-----------------\n");

    if(foundmatch) printf("block is nonzero\n");
    else           printf("block is zero\n");

    printf("shape=%d\n",shape);
    int ii;
    int jj;

    if(shape==0) /* If shape is square */
    {
        for(ii=0;ii!=12;ii++)
        {
            for(jj=0;jj!=12;jj++)
            {
                printf("%8.2f ",b->result[4+ii%4+(ii/4)*48+jj*4]);
            }
            printf("\n");
        }
    }
    else /* If shape is triangle */
    {
        for(ii=0;ii!=4;ii++)
        {
            for(jj=0;jj!=4;jj++)
            {
                printf("%8.2f ",b->result[4+jj*4+ii]);
            }
            printf("\n");
        }
        for(ii=0;ii!=4;ii++)
        {
            for(jj=0;jj!=8;jj++)
            {
                printf("%8.2f ",b->result[24+jj*4+ii]);
            }
            printf("\n");
        }
        for(ii=0;ii!=4;ii++)
        {
            for(jj=0;jj!=12;jj++)
            {
                printf("%8.2f ",b->result[60+jj*4+ii]);
            }
            printf("\n");
        }
    }
}
void printCalculateConstraintForcesBlockInput(Calc_forces_buf* b,int num_strips_to_calc)
{
    int i,j;

    printf("Calc constraint forces block input\n");
    printf("----------------------------------\n");

    printf("num_strips_to_calc=%d\n",num_strips_to_calc);

#if 0
    printf("lambda:\n");
    for(i=0;i!=num_strips_to_calc*4;i++)
    {
        printf("%12.6f\n",b->lambda[i]);
    }
#endif

    printf("block to constraint body\n");
    for(i=0;i!=num_strips_to_calc;i++)
    {
        for(j=0;j!=8;j++)
        {
            printf("%d ",b->j_bl2cbody[i][j]);
        }
        printf("\n");
    }
#if 1
    printf("jlen\n");
    for(i=0;i!=num_strips_to_calc;i++)
    {
        printf("%d\n",b->j_len[i]);
    }
#endif
}
void printCalculateConstraintForcesBlockOutput(Calc_forces_buf* b,int num_strips)
{
    int i,j,num_force_pairs;

    printf("Calc constraint forces block output\n");
    printf("-----------------------------------\n");
    printf("spr output buffer at %08x\n",(unsigned int)b->forces);

    MeReal *forces = (MeReal *)b->forces;

    printf("b->j_bl2cbody[num_strips-1][b->j_len[num_strips-1]-2]=%d\n",b->j_bl2cbody[num_strips-1][b->j_len[num_strips-1]-2]);
    printf("b->j_bl2cbody[0][0]=%d\n",b->j_bl2cbody[0][0]);

    num_force_pairs = (b->j_bl2cbody[num_strips-1][b->j_len[num_strips-1]-2] - b->j_bl2cbody[0][0])/2 + 1;

    for(i=0;i!=num_force_pairs;i++)
    {
        printf("constraint[%d] %08x - (",i,(unsigned int)(forces + 16*i + 0));
        for(j=0;j!=3;j++) printf("%9.5f ",forces[16*i + j + 0]);
        for(j=0;j!=3;j++) printf("%9.5f ",forces[16*i + j + 4]);
        printf(") (");
        for(j=0;j!=3;j++) printf("%9.5f ",forces[16*i + j + 8]);
        for(j=0;j!=3;j++) printf("%9.5f ",forces[16*i + j + 12]);
        printf(")\n");

    }
}
void printCalcResultantForcesInput(const MdtKeaBody *const blist[],
                                   const MeReal *          cforces,
                                   int                     num_bodies,
                                   int                     num_constraints)
{
    int i,j;

    printf("Calculate resultant forces input\n");    
    printf("--------------------------------\n");

    printf("num_bodies      = %d\n",num_bodies);
    printf("num_constraints = %d\n",num_constraints);

    printf("external forces:\n");
    printf("\n");
    for(i=0;i!=num_bodies;i++)
    {
        for(j=0;j!=3;j++)
        {
            printf("%12.6f\n",blist[i]->force[j]);
        }
        for(j=0;j!=3;j++)
        {
            printf("%12.6f\n",blist[i]->torque[j]);
        }

        printf("\n");
    }
    printf("\n");
    printf("constraint forces\n");
    for(i=0;i!=num_constraints;i++)
    {
        printf("Constraint %d body 0\n",i);
        for(j=0;j!=3;j++)
        {   
            printf("%12.6f\n",cforces[i*16+j]);
        }
        for(j=0;j!=3;j++)
        {   
            printf("%12.6f\n",cforces[i*16+4+j]);
        }

        printf("Constraint %d body 1\n",i);
        for(j=0;j!=3;j++)
        {   
            printf("%12.6f\n",cforces[i*16+8+j]);
        }
        for(j=0;j!=3;j++)
        {   
            printf("%12.6f\n",cforces[i*16+12+j]);
        }

    }
}
void printCalcResultantForcesOutput(const MdtKeaBody *const blist[],int num_bodies)
{
    int i,j;

    printf("Calculate resultant forces output\n");    

    printf("resultant forces:\n");
    printf("\n");
    for(i=0;i!=num_bodies;i++)
    {
        for(j=0;j!=3;j++)
        {
            printf("%12.6f\n",blist[i]->force[j]);
        }
        for(j=0;j!=3;j++)
        {
            printf("%12.6f\n",blist[i]->torque[j]);
        }

        printf("\n");
    }

}
void printCalculateResultantForcesBlockOutput(Sum_forces_buf* b,int num_bodies)
{
    int i,j;

    printf("Calculate Resultant Forces Block Output\n");
    printf("---------------------------------------\n");

    Sum_forces_spr *spr = (Sum_forces_spr*)(0x70000000);
    MdtKeaForce *  f=spr->forces;

    for(i=0;i!=num_bodies;i++)
    {
        for(j=0;j!=3;j++)
        {
            printf("%12.6f\n",f[i].force[j]);
        }

        for(j=0;j!=3;j++)
        {
            printf("%12.6f\n",f[i].torque[j]);            
        }
    }   
}
void printCalculateResultantForcesBlockInput(Sum_forces_buf* b,int nconstraints)
{

    int i,j;
    printf("Calculate Resultant Forces Block Input\n");
    printf("---------------------------------------\n");

    MeReal *forces = (MeReal *)b->cforces;

    printf("constraint forces\n");
    for(i=0;i!=nconstraints;i++)
    {
        printf("constraint[%d] %08x - \n",i,(unsigned int)(forces + 16*i + 0));
        for(j=0;j!=3;j++) printf("%9.5f\n",forces[16*i + j + 0]);
        for(j=0;j!=3;j++) printf("%9.5f\n",forces[16*i + j + 4]);
        printf(") (");
        for(j=0;j!=3;j++) printf("%9.5f\n",forces[16*i + j + 8]);
        for(j=0;j!=3;j++) printf("%9.5f\n",forces[16*i + j + 12]);
        printf(")\n");

    }

    printf("Jbody\n");
    for(i=0;i!=nconstraints;i++)
    {
        printf("%d %d\n",b->jbody[i][0],b->jbody[i][1]);
    }

}
void printFactoriserPS2SmallDenseInput(const MeReal A[],
                                       int          c4n)
{
    printf("PS2Smalldense factoriser input\n");
    printf("------------------------------\n");
    printHalfWallpaperMatrix(A,         /* Input */
                             c4n);      /* Input */
    printf("\n");
}
void printFactorisePCSparseVanillaInput()
{
}

void printSolvePS2SmallDenseInput(const MeReal A[],const MeReal x[],int c4n)
{
    int i,j;
    printf("PS2Smalldense solve input\n");
    printf("-------------------------\n");
    printf("\n");
    printf("A %08x:\n",(unsigned int)A);
    
    printHalfWallpaperMatrix(A,         /* Input */
                             c4n);      /* Input */

    printf("rhs %08x:\n",(unsigned int)x);
    for(i=0;i!=c4n;i++)
    {
        printf("% 12.6f\n",x[i]);
    }
    printf("\n");
}
void printSolvePS244SmallDenseInput(const MeReal matrixChol[],const MeReal rhs[])
{
    printf("PS244Smalldense solve input\n");
    printf("---------------------------\n");
    printf("\n");
    printMat44(matrixChol,"matrixChol");
    printVec4(rhs,"rhs");
    printf("\n");
}
void printSolvePS244SmallDenseOutput(const MeReal x[])
{
    printf("PS244Smalldense solve output\n");
    printf("----------------------------\n");
    printf("\n");
    printVec4(x,"solve result");
    printf("\n");
}
void printSolvePS2SmallDenseOutput(const MeReal x[],int c4n)
{
    int i,j;
    printf("PS2Smalldense solve output\n");
    printf("--------------------------\n");
    printf("\n");
    
    printf("rhs %08x:\n",(unsigned int)x);
    for(i=0;i!=c4n;i++)
    {
        printf("% 12.6f\n",x[i]);
    }
    printf("\n");
}

void printWritebackMatrixCholOutput(const MeReal recipSqrt[],
                                    int          numElts)
{
    printf("WritebackMatrixChol Output\n");
    printf("--------------------------\n");
    printf("\n");
    printVec(recipSqrt,numElts*4,"recipSqrt (1 quad per elt)");
    printf("\n");
}

#endif /* PS2 */

