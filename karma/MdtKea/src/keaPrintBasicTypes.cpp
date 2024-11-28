/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/11/04 17:25:15 $ - Revision: $Revision: 1.5.2.1.4.1 $

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
#include <MdtKea.h>

void printMeReal(MeReal x,         /* Input */
                 const char *desc) /* Input */
{
    printf("%s ",desc);
    printf("% 09.6f\n",x);
}

void printColMajorMat(const MeReal  A[],   /* Input */
                      int           rows,  /* Input */
                      int           cols,  /* Input */
                      const char *  desc)  /* Input */
{    
    int i,j;

    printf("%s \n",desc);

    for(i=0;i!=rows;i++)
    {
        for(j=0;j!=cols;j++)
        {
            printf("% 6.2f ",A[i+j*rows]);
        }
        printf("\n");
    }
    printf("\n");
}

void printIntMat(const int     A[],   /* Input */
                 int           rows,  /* Input */
                 int           cols,  /* Input */
                 const char *  desc)  /* Input */
{    
    int i,j;

    printf("\n");
    printf("%s \n",desc);

    for(i=0;i!=rows;i++)
    {
        for(j=0;j!=cols;j++)
        {
            printf("% 2d ",A[i*cols+j]);
        }
        printf("\n");
    }
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
            printf("% 09.6f ",A[j*4+i]);
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
            printf("% 09.6f ",A[j*4+i]);
        }
        printf("\n");
    }
}

void printMat412(const MeReal  A[],  /* Input */
                 const char *  desc) /* Input */
{
    int i,j;

    printf("\n");
    printf("%s \n",desc);

    for(i=0;i!=4;i++)
    {
        for(j=0;j!=12;j++)
        {
            printf("% 09.6f ",A[j*4+i]);
        }
        printf("\n");
    }
}

void printIntVec(const int     A[],      /* Input */
                 int           numElts,  /* Input */                 
                 const char *  desc)     /* Input */
{
    int j;

    printf("%s \n",desc);

    for(j=0;j!=numElts;j++)
    {
        printf("% 2d ",A[j]);
    }
    printf("\n");
}

void printVec4(const MeReal  A[],  /* Input */
               const char *  desc) /* Input */
{
    int j;

    printf("%s \n",desc);

    for(j=0;j!=4;j++)
        printf(" 0x%08x % 09.6f\n",A+j,A[j]);

    printf("\n");
}

void printVec3(const MeReal  A[],  /* Input */
               const char *  desc) /* Input */
{
    int j;

    printf("%s \n",desc);

    for(j=0;j!=3;j++)
    {
        printf("% 09.6f ",A[j]);
    }
    printf("\n");
}

void printVec(const MeReal  A[],     /* Input */
              int           numElts, /* Input */
              const char *  desc)    /* Input */
{
    int j;

    printf("%s \n",desc);

    for(j=0;j!=numElts;j++)
    {
        printf("%08x % 09.6f\n",A+j,A[j]);
    }
    printf("\n");
}

void printPtr(const void *ptr,  /* Input */
              const char *desc) /* Input */
{
    printf("%20s %08x\n",desc,(unsigned int)ptr);
}

void printInvMassMatrix(const MdtKeaInverseMassMatrix invM, /* Input */
                        const char *                  desc) /* Input */
{
    int i;

    printf("%s\n",desc);
    printf("\n");

    for(i=0;i!=3;i++) printf("% 09.6f ",invM.invI0[i]);
    printf(" % 014f ",invM.invmass);printf("\n");
    for(i=0;i!=3;i++) printf("% 09.6f ",invM.invI1[i]);printf("\n");
    for(i=0;i!=3;i++) printf("% 09.6f ",invM.invI2[i]);printf("\n");
}

void printInvMassMatrixArray(const MdtKeaInverseMassMatrix invM[],  /* Input */
                             int                           numElts, /* Input */
                             const char *                  desc)    /* Input */
{
    int i,j;

    printf("%s\n",desc);
    printf("\n");

    for(j=0;j!=numElts;j++)
    {
        for(i=0;i!=3;i++) printf("% 09.6f ",invM[j].invI0[i]);
        printf(" % 014f ",invM[j].invmass);printf("\n");
        for(i=0;i!=3;i++) printf("% 09.6f ",invM[j].invI1[i]);printf("\n");
        for(i=0;i!=3;i++) printf("% 09.6f ",invM[j].invI2[i]);printf("\n");
    }
}

void printMdtKeaForcePairArray(const MdtKeaForcePair cforces[],      /* Input */
                               int                   numConstraints) /* Input */
{
    int i,j;

    printf("constraint forces\n");
    for(i=0;i!=numConstraints;i++)
    {
        printf("Constraint %3d: ",i);
        for(j=0;j!=3;j++)
        {   
            printf("% 012.06f ",cforces[i].primary_body.force[j]);
        }
        for(j=0;j!=3;j++)
        {   
            printf("% 012.06f ",cforces[i].primary_body.torque[j]);
        }

        printf("\n");
        printf("                ");

        for(j=0;j!=3;j++)
        {   
            printf("% 012.06f ",cforces[i].secondary_body.force[j]);
        }
        for(j=0;j!=3;j++)
        {   
            printf("% 012.06f ",cforces[i].secondary_body.torque[j]);
        }
        printf("\n");
    }
    printf("\n");
}
void printMdtKeaBl2CBodyRowArray(const MdtKeaBl2BodyRow  bl2cbody[],   /* Input */
                                 int                     numElts)      /* Input */
{
    int i,j;

    for(i=0;i!=numElts;i++)
    {
        for(j=0;j!=8;j++)
        {
            printf("% 3d ",bl2cbody[i][j]);
        }
        printf("\n");
    }
}
void printMdtKeaBl2BodyRowArray(const MdtKeaBl2BodyRow  bl2cbody[],   /* Input */
                                int                     numElts)      /* Input */
{
    int i,j;

    for(i=0;i!=numElts;i++)
    {
        for(j=0;j!=8;j++)
        {
            printf("% 3d ",bl2cbody[i][j]);
        }
        printf("\n");
    }

}

void printMdtKeaJBlockPairArray(const MdtKeaJBlockPair J[],                  /* Input */
                                int                    num_rows_inc_padding) /* Input */
{
    int i,j,k,l;

    for(i=0;i!=num_rows_inc_padding/4;i++)
    {
        for(l=0;l!=4;l++)
        {
            for(j=0;j!=2;j++)
            {
                for(k=0;k!=6;k++)
                {
                    printf("% 6.3f",J[i][j].col[k][l]);
                }
                printf("| ");
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("\n");
}

void printMdtKeaVelocityArray(const MdtKeaVelocity v[],         /* Input */
                              int                  num_bodies,  /* Input */
                              const char *         desc)        /* Input */
{
    int i,j;

    printf("%s\n",desc);
    for(i=0;i!=num_bodies;i++)
    {
        for(j=0;j!=3;j++) printf("% 12.6f ",v[i].velocity[j]); printf(" ");
        for(j=0;j!=3;j++) printf("% 12.6f ",v[i].angVelocity[j]); printf("\n");
        printf("\n");
    }
    printf("\n");
}

void printHalfWallpaperMatrix(const MeReal A[],       /* Input */
                              int          c4numRows) /* Input */
{
    int i,j,k,l;

    const MeReal * pRowBlock;

    pRowBlock = A;
    for(i=0;i!=c4numRows/4;i++)
    {
        /* Print a row block */
        for(k=0;k!=4;k++)
        {
            /* Loop over the blocks in the row */
            for(j=0;j!=i+1;j++)
            {
                /* Loop over the rows in the block */
                for(l=0;l!=4;l++)
                {
                    printf("% 6.2f ",pRowBlock[k+l*4+j*16]);
                }
            }
            printf("\n");
        }
        printf("\n");
        pRowBlock = pRowBlock + (i+1) * 16;
    }
    printf("\n");
}

void printPS2SparseMatrix(
         const MeReal matrix[],
         const int    rlist[],
         const int    rlist_len[],
         int          num_12_blocks)
{
    int i,j,k,l;

    for(i=0;i!=num_12_blocks;i++)
    {
        /* Loop over rows in the row block */
        for(k=0;k!=12;k++)
        {
            int rlist_index = rlist_len[i] - 1;

            for(j=0;j!=num_12_blocks;j++)
            {
                if(rlist[num_12_blocks*i+rlist_index]==j)
                {
                    /* Block is non zero, so print a row from it */
                    int destcol   = rlist[num_12_blocks*i+rlist_index];
                    int sourcecol = i-rlist_index;
    
                    const MeReal *source = matrix + ((i*(i+1)/2)+sourcecol)*144;

                    for(l=0;l!=12;l++)
                    {
                        printf("% 6.2f ",source[l*4+k%4+(k/4)*48]);
                    }
                    rlist_index = rlist_index - 1; 
                }
                else
                {
                    /* Block is zero, so print zeros */
                    for(l=0;l!=12;l++)
                    {
                        printf("% 0000.00 ");
                    }
                }
                printf(" ");
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("\n");
}

