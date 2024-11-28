#include "keaInternal.hpp"
#include "MdtKeaProfile.h"
#include <string.h>
#include <stdlib.h>
#include <MeMemory.h>
#include "keaMemory.hpp"
#include <stdio.h>
#include <MeAssert.h>
#include <keaDebug.h>
#include <MeMath.h>

#include <keaMatrix_PcSparse_vanilla.hpp>

void keaMatrix_pcSparse_vanilla::allocate(int size)
{
    m_numRows   = size;
    m_blocks    = (size+3)>>2;
    m_padded    = m_blocks*4;
    
    rsD         = (MeReal *)ALIGN_16(keaPoolAlloc (m_padded * sizeof (MeReal)+16, "rsD"));

    matrix      = (MeReal *)ALIGN_16(keaPoolAlloc  (m_padded * m_padded * sizeof (MeReal)+16, "A"));
    matrixChol  = (MeReal *)ALIGN_16(keaPoolAlloc  (m_padded * m_padded * sizeof (MeReal)+16, "Achol"));
    mLP         = matrix;
    mcLP        = matrixChol;
    
    NAZ         = (MeReal**)keaPoolAlloc (((m_blocks*m_blocks+15)&~15)* sizeof(MeReal*), "NAZ");
    NCZ         = (MeReal**)keaPoolAlloc (((m_blocks*m_blocks+15)&~15)* sizeof(MeReal*), "NCZ");
    NR          = (int *)   keaPoolAlloc (((m_blocks+15)&~15)* sizeof (int), "NR");
    NC          = (int *)   keaPoolAlloc (((m_blocks+15)&~15)* sizeof (int), "NC");
}

static void Multiply46BlockBy46BlockTranspose(MeReal* ainvblock, const MeReal *jmblock,const MeReal *jblock, int notzero);
//multiply JInvM with JT (upper triangle only.)
void keaMatrix_pcSparse_vanilla::makeFromJMJT(const MeReal *JM,
                                              const MeReal *Js,
                                              const int *num_in_strip,
                                              const int *block2body,
                                              const MeReal *slipfactor,
                                              const MeReal epsilon,
                                              const MeReal hinv)
{
    int strip,jm,z,k ;
    int step=m_blocks;
    const int* Jb=block2body;
    const int* JMb;
    const MeReal* jmptr;
    const MeReal* J=Js;
    MeReal* AMatrix=mLP;
    MeReal* AcholMatrix=mcLP;
    for(strip=0, z=0; strip<m_blocks;strip++)
    {
        jmptr=JM;
        JMb=block2body;
        step--;
        k=0x0FFFFFFF;
        for(jm=0;jm<=strip;jm++,z++)
        {
            int block, body, i, j, notzero;
            const int* Jbodys=Jb;
            const int* JMblock2bodystrip=JMb;
            const MeReal *jmstrip=jmptr;
            int   num_in_JMstrip=num_in_strip[jm];
            int   num_in_Jstrip=num_in_strip[strip];
            notzero=0;
            for(block=0;block<num_in_JMstrip;block++)
            {
                body=*JMblock2bodystrip;
                if(body!=-1)
                {
                    for(i=0; i<num_in_Jstrip;i++)
                    {
                        if(body==Jbodys[i])
                        {
                            Multiply46BlockBy46BlockTranspose(AcholMatrix, jmstrip, J+24*i, notzero);
                            notzero=1;   //flag true
                        }
                    }
                }
                jmstrip+=24;
                JMblock2bodystrip++;
            }
            if(notzero)
            {
                //set sparse ptrs
                NAZ[z]=AMatrix;
                NCZ[z]=AcholMatrix;
                //copy new nonzero block
                for(i=0; i<16;i+=4)
                {
                    for(j=0; j<4;j++)
                    {
                        AMatrix[i+j]=AcholMatrix[i+j];
                    }
                }
                //update matrix block ptrs
                AMatrix+=16;
                AcholMatrix+=16;
                //if(k==unset) k=jm;
                int d=(k-jm)>>31;
                k=(d&k)+(~d&jm);
            }
            else {
                NAZ[z]=0;
                NCZ[z]=0;
            }
            JMb+=8;
            jmptr+=num_in_strip[jm]*24;
        }
        NC[strip]=k;
        J+=num_in_strip[strip]*24;
        Jb+=8;
        z+=step;
    }

    mLP = AMatrix;
    mcLP= AcholMatrix;
    
    //Complete matrix
    int i,j,c,p,stridem;
    stridem=(m_padded-1)*4;
    
    //add epsilon and scaled diagonal vector
    for(i=0, k=0; i<m_blocks; i++)
    {
        AMatrix = NAZ[i*m_blocks+i];
        AcholMatrix = NCZ[i*m_blocks+i];
        for(j=0, p=0; j<4; j++, p+=5)
        {
            AMatrix[p] +=epsilon+hinv*slipfactor[k];
            AcholMatrix[p] +=epsilon+hinv*slipfactor[k++];
        }
    }

    // pad the inverse with 1s along the end of the diagonal
    // so it remains non-singular
    i=((m_numRows-1)&3)+1;
    k=i*5;
    for(; i<4; i++, k+=5)
    {
        AcholMatrix[k] = AMatrix[k] = 1.0f;
    }
    
    //complete sparse maps
    for(i=0, c=0; i<m_blocks;i++)
    {
        for(j=i; j<m_blocks;j++)
        {
            if(NAZ[j*m_blocks+i]!=0 && j>c) c=j;
        }
        NR[i]=c;
    }

}

#ifdef _MSC_VER
__forceinline 
#endif
void Multiply46BlockBy46BlockTranspose(MeReal* acholblock, const MeReal *jmblock,const MeReal *jblock, int notzero)
{
    MeReal tmp[4];
    if(notzero)
    {
        //add block
        for(int i=0; i<4; i++)
        {
            tmp[0]=tmp[1]=tmp[2]=tmp[3]=0.0f;
            for(int j=0; j<6; j++)
            {
                tmp[0]+=jmblock[4*j+i]*jblock[4*j];
                tmp[1]+=jmblock[4*j+i]*jblock[4*j+1];
                tmp[2]+=jmblock[4*j+i]*jblock[4*j+2];
                tmp[3]+=jmblock[4*j+i]*jblock[4*j+3];
            }
            acholblock[4*i]+=tmp[0];
            acholblock[4*i+1]+=tmp[1];
            acholblock[4*i+2]+=tmp[2];
            acholblock[4*i+3]+=tmp[3];
        }
    }
    else
    {
        //just calc block
        for(int i=0; i<4; i++)
        {
            tmp[0]=tmp[1]=tmp[2]=tmp[3]=0.0f;
            for(int j=0; j<6; j++)
            {
                tmp[0]+=jmblock[4*j+i]*jblock[4*j];
                tmp[1]+=jmblock[4*j+i]*jblock[4*j+1];
                tmp[2]+=jmblock[4*j+i]*jblock[4*j+2];
                tmp[3]+=jmblock[4*j+i]*jblock[4*j+3];
            }
            acholblock[4*i]=tmp[0];
            acholblock[4*i+1]=tmp[1];
            acholblock[4*i+2]=tmp[2];
            acholblock[4*i+3]=tmp[3];
        }
    }
}

// sparse multiply ----------------------------------------------------------

void keaMatrix_pcSparse_vanilla::multiply(
         MeReal       B[],      /* Output */
         const MeReal X[])      /* Input  */
{
#ifdef PRINT_MULTIPLY_INPUT

#endif

    MeReal* G;
    MeReal  tmp[4];
    int   i,j,k,g,h;
    for(i=0, g=0; i<m_blocks; i++, g+=4)
    {
        tmp[0]=tmp[1]=tmp[2]=tmp[3]=0.0f;
        for(k=0, h=0; k<i; k++, h+=4)
        {
            G=NAZ[i*m_blocks+k];
            if(G!=0)
            {
                //U_Mblock
                for(j=0; j<4; j++)
                {
                    tmp[0]+=G[4*j]*X[h+j];
                    tmp[1]+=G[4*j+1]*X[h+j];
                    tmp[2]+=G[4*j+2]*X[h+j];
                    tmp[3]+=G[4*j+3]*X[h+j];
                }
            }
        }
        for(j=i;j<m_blocks;j++,h+=4)
        {
            G=NAZ[j*m_blocks+i];
            if(G!=0)
            {
                //D_Mblock()
                for(k=0; k<4; k++)
                {
                    tmp[k]+=G[4*k]*X[h]+\
                        G[4*k+1]*X[h+1]+\
                        G[4*k+2]*X[h+2]+\
                        G[4*k+3]*X[h+3];
                }
            }
        }
        //F_Mblock2()
        B[g]=tmp[0];
        B[g+1]=tmp[1];
        B[g+2]=tmp[2];
        B[g+3]=tmp[3];
    }

    /* Zero pad up to next 12 boundary */
    for(i=m_numRows;i!=MeMathCEIL12(m_numRows);i++)
    {
        B[i]=0.0f;
    }

}
/*######## SPARSE BLOCK CHOLESKY FACTORIZE #########*/

//Sparse is as good as dense on a dense matrix so just this implementation

static void vc_strip_Cholesky(MeReal* tmp, MeReal* rsD, int z, int w, int d, int n, MeReal** NZ, MeReal** mcLPptr);
static void vc_dstrip_Cholesky(MeReal* tmp, MeReal* rsD, int z, int d, int s, MeReal** NZ);

//sparse vanilla cholesky factorize
void keaMatrix_pcSparse_vanilla::factorize()
{
#if PROFILE_MDTKEA
    MdtKeaProfileStart("factorize");
#endif

#if PRINT_FACTORISER_INPUT    
    //printFactorisePCSparseVanillaInput();
#endif

    int i,j,z,w,stride;
    LOCAL_ARRAY(ablock, 16);

    stride=m_blocks;
    // Row Resolve
    for(i=0, z=0; i<m_blocks; i++, z+=stride)
    {
        vc_dstrip_Cholesky(ablock, rsD, z, i, NC[i], NCZ);
        for(j=i+1, w=z+stride; j<=NR[i]; j++, w+=stride)
        {
            if(NC[j]<=i)
                vc_strip_Cholesky(ablock, rsD, z, w, i, NC[j], NCZ, &mcLP);
        }
    }

#if PROFILE_MDTKEA
    MdtKeaProfileEnd("factorize");
#endif
}

#ifdef _MSC_VER
__forceinline
#endif 
void vc_strip_Cholesky(MeReal* tmp, MeReal* rsD, int z, int w, int d, int s, MeReal** NZ, MeReal** mcLPptr)
{
    int i,j,r,t,p,n;
    MeReal* a;
    MeReal* b;

    memset(tmp, 0, 16*sizeof(MeReal));
    n=d-s;
    t=z+s;
    p=w+s;
    for(i=0; i<n; i++, t++, p++)
    {
        a=NZ[t];
        b=NZ[p];
        if(a!=0 && b!=0)
        {
            //sse_block_mulad();
            for(r=0; r<4; r++)
            {
                for(j=0; j<4; j++)
                {
                    tmp[j*4]+=a[4*r+j]*b[4*r];
                    tmp[j*4+1]+=a[4*r+j]*b[4*r+1];
                    tmp[j*4+2]+=a[4*r+j]*b[4*r+2];
                    tmp[j*4+3]+=a[4*r+j]*b[4*r+3];
                }
            }
        }
    }
    a=NZ[t];
    b=NZ[p];
    if(b!=0)
    {
        //nsse_block_subdiv()
        for(j=0; j<4; j++)
        {
            b[4*j]=(b[4*j]-tmp[4*j])*rsD[4*d+j];
            b[4*j+1]=(b[4*j+1]-tmp[4*j+1])*rsD[4*d+j];
            b[4*j+2]=(b[4*j+2]-tmp[4*j+2])*rsD[4*d+j];
            b[4*j+3]=(b[4*j+3]-tmp[4*j+3])*rsD[4*d+j];
            //bcmuladd
            for(r=j+1; r<4; r++)
            {
                tmp[4*r]+=a[4*j+r]*b[4*j];
                tmp[4*r+1]+=a[4*j+r]*b[4*j+1];
                tmp[4*r+2]+=a[4*j+r]*b[4*j+2];
                tmp[4*r+3]+=a[4*j+r]*b[4*j+3];
            }
        }
    }
    else
    {
        //printf("Fill: %d\n", d);
        b = *mcLPptr;
        (*mcLPptr)+=16;
        //nsse_block_subdivs()
        for(j=0; j<4; j++)
        {
            b[4*j]=-tmp[4*j]*rsD[4*d+j];
            b[4*j+1]=-tmp[4*j+1]*rsD[4*d+j];
            b[4*j+2]=-tmp[4*j+2]*rsD[4*d+j];
            b[4*j+3]=-tmp[4*j+3]*rsD[4*d+j];
            //bcmuladd
            for(r=j+1; r<4; r++)
            {
                tmp[4*r]+=a[4*j+r]*b[4*j];
                tmp[4*r+1]+=a[4*j+r]*b[4*j+1];
                tmp[4*r+2]+=a[4*j+r]*b[4*j+2];
                tmp[4*r+3]+=a[4*j+r]*b[4*j+3];
            }
        }
        NZ[p]=b;
    }
}

#ifdef _MSC_VER
__forceinline
#endif
void vc_dstrip_Cholesky(MeReal* tmp, MeReal* rsD, int z, int d, int s, MeReal** NZ)
{
    int i,j,r,c,t,n;
    MeReal* MB;
    memset(tmp, 0, 16*sizeof(MeReal));
    n=d-s;
    t=z+s;
    for(i=0; i<n; i++, t++)
    {
        MB=NZ[t];
        if(MB!=0)
        {
            //sse_block_mulads()
            for(r=0; r<4; r++)
            {
                for(j=0; j<4; j++)
                {
                    for(c=j; c<4; c++)
                        tmp[j*4+c]+=MB[4*r+j]*MB[4*r+c];
                }
            }
        }
    }
    MB=NZ[t];
    //nsse_block_subroot()
    for(j=0; j<4; j++)
    {
        //if(MB[5*j]-tmp[5*j]<0) printf("BADCHOL! %d\n", i);
        rsD[4*d+j]=1/(MB[5*j]=MeSqrt(MB[5*j]-tmp[5*j]));
        for(r=j+1; r<4; r++)
        {
            MB[4*j+r]=(MB[4*j+r]-tmp[4*j+r])*rsD[4*d+j];
        }
        //bcmulad
        for(r=j+1; r<4; r++)
        {
            for(c=r; c<4; c++)
                tmp[r*4+c]+=MB[4*j+r]*MB[4*j+c];
        }
    }
}

/* solve
 * -----
 *
 * find rhs, such that matrix*rhs = orig(rhs)
 */

void keaMatrix_pcSparse_vanilla::solve(MeReal       x[],
                                       const MeReal b[])
{
#if PRINT_SOLVE_INPUT
    printSolveInput(b,m_blocks);
#endif

    MeReal  tmp[4];
    int     i,j,k,g,h,r,s,t;
    MeReal* G;

    /* 
       If dest and source dont point to the same place, 
       then copy source to dest, so we can solve in-place
    */

    if(x!=b)
    {
        for(i=0;i!=m_padded;i++)
        {
            x[i]=b[i];
        }
    }
    
    /* Solve lower, transposed */
    
    for (i=0; i<m_blocks; i++)
    {
        tmp[0]=tmp[1]=tmp[2]=tmp[3]=0.0f;
        for(j=0, h=0, k=i*m_blocks; j<i; j++, k++, h+=4)
        {
            G=NCZ[k];
            if (G!=0)
            {
                //do vert dot prod thang... 4 accums (O_sblockL)
                for(r=0; r<4; r++)
                {
                    tmp[0]+=G[4*r]*x[h+r];
                    tmp[1]+=G[4*r+1]*x[h+r];
                    tmp[2]+=G[4*r+2]*x[h+r];
                    tmp[3]+=G[4*r+3]*x[h+r];
                }
            }
        }
        G=NCZ[k];
        //do diag vert dot prod thingy... store results (D_blockL)
        for(r=0; r<4; r++)
        {
            x[h+r]=(x[h+r]-tmp[r])*rsD[h+r];
            for(s=r+1; s<4; s++)
            {
                tmp[s]+=G[4*r+s]*x[h+r];
            }
        }
    }
    
    /* Solve upper */
    t=(m_blocks-1)*m_blocks;
    for (i=m_blocks-1; i>=0; i--)
    {
        tmp[0]=tmp[1]=tmp[2]=tmp[3]=0.0f;
        for(j=m_blocks-1, g=h, k=t+i; j>i; j--, g-=4)
        {
            G=NCZ[k];
            if (G!=0)
            {
                //do horz dot proddy thang... 4 accums
                //O_blockU();
                for(r=0; r<4; r++)
                {
                    tmp[r]+=G[4*r]*x[g]+\
                            G[4*r+1]*x[g+1]+\
                            G[4*r+2]*x[g+2]+\
                            G[4*r+3]*x[g+3];
                }
            }
            k-=m_blocks;
        }
        G=NCZ[k];
        //do diag horz dot proddy thingymijigga... store results
        //(D_blockU)
        for(r=3; r>=0; r--)
        {
            for(s=r+1; s<4; s++)
            {
                tmp[r]+=G[4*r+s]*x[g+s];
            }
            x[g+r]=(x[g+r]-tmp[r])*rsD[g+r];
        }
    }
    for(i=m_numRows;i!=m_padded;i++)
    {
        x[i]=0;
    }
#if PRINT_SOLVE_OUTPUT
    printSolvePCSparseSSEOutput(x,
                                m_padded);
#endif

}

void keaMatrix_pcSparse_vanilla::solveUnits(
         MeReal       Ainv[],          /* Output */
         int          cached[],        /* Output / Input  */
         const int    clamped[],       /* Input  */
         int          numClamped,      /* Input  */
         int          AinvStride)      /* Input  */
{
    int c,w,t,step,num_rhs;
    int i,j,k,g,h,r,s,v;
    MeReal *G,*res,*wrk;
    MeReal* B=Ainv;
    LOCAL_INT_ARRAY(rp,numClamped);
    // Setup rhs units
    for(c=0,t=0;c<numClamped;c++) {
        w=clamped[c];
        if(!cached[w]) {
            memset(&B[w*AinvStride],0,AinvStride*sizeof(MeReal));
            B[w*(AinvStride+1)]=1;
            cached[w]=1;
            rp[t++]=w;
        }
    }
    // Solve lower, transposed 
    num_rhs=t;
    step=m_blocks*16;
    for(i=0; i<m_blocks; i++) {
        for(j=0, h=0, k=i*m_blocks; j<i; j++, k++, h+=4) {
            G=NCZ[k];
            if(G!=0) {
                for(t=0; t<num_rhs; t++) {
                    if(rp[t]>>2<=j) {
                        res=&B[AinvStride*rp[t]+4*i];
                        wrk=&B[AinvStride*rp[t]+h];
                        //do vert dot prod thang... (O_sblockL)
                        for(r=0; r<4; r++) {
                            res[0]-=G[4*r]*wrk[r];
                            res[1]-=G[4*r+1]*wrk[r];
                            res[2]-=G[4*r+2]*wrk[r];
                            res[3]-=G[4*r+3]*wrk[r];
                        }
                    }
                }
            }
        }
        G=NCZ[k];
        for(t=0;t<num_rhs;t++) {
            if(rp[t]>>2<=i) {
                res=&B[AinvStride*rp[t]+4*i];
                //do diag vert dot prod thingy... store results (D_blockL)
                for(r=0; r<4; r++) {
                    res[r]=res[r]*rsD[h+r];
                    for(s=r+1; s<4; s++) {
                        res[s]-=G[4*r+s]*res[r];
                    }
                }
            }
        }
    }
    // Solve upper 
    v=(m_blocks-1)*m_blocks;
    for(i=m_blocks-1; i>=0; i--) {
        for(j=m_blocks-1, g=h, k=v+i; j>i; j--, g-=4, k-=m_blocks) {
            G=NCZ[k];
            if (G!=0) {
                for(t=0; t<num_rhs; t++) {
                    res=&B[AinvStride*rp[t]+4*i];
                    wrk=&B[AinvStride*rp[t]+g];
                    //do horz dot proddy thang... O_blockU();
                    for(r=0; r<4; r++) {
                        res[r]-=G[4*r]*wrk[0]+\
                                G[4*r+1]*wrk[1]+\
                                G[4*r+2]*wrk[2]+\
                                G[4*r+3]*wrk[3];
                    }
                }
            }
        }
        G=NCZ[k];
        for(t=0; t<num_rhs; t++) {
            res=&B[AinvStride*rp[t]+4*i];
            //do diag horz dot proddy thingymijigga... (D_blockU)
            for(r=3; r>=0; r--) {
                for(s=r+1; s<4; s++) {
                    res[r]-=G[4*r+s]*res[s];
                }
                res[r]=res[r]*rsD[g+r];
            }
        }
    }

#if PRINT_SOLVE_UNIT_OUTPUT
    for(c=0;c<num_rhs;c++) 
    {
        printSolvePCSparseSSEOutput(Ainv+AinvStride*rp[c],
                                    m_padded);
        
    }
#endif
}
