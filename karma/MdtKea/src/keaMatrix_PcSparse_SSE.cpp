#include "keaInternal.hpp"
#include "MdtKeaProfile.h"
#include <string.h>
#include <stdlib.h>
#include <MeMemory.h>
#include "keaMemory.hpp"
#include <stdio.h>
#include <MeAssert.h>
#include <KeaSSEi.h>
#include <keaDebug.h>
#include <MeMath.h>

#include <keaMatrix_pcSparse_SSE.hpp>

void keaMatrix_pcSparse_SSE::allocate(int size)
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


//multiply JInvM with JT (upper triangle only.)
void keaMatrix_pcSparse_SSE::makeFromJMJT(const MeReal *JM,
                                          const MeReal *Js,
                                          const int *num_in_strip,
                                          const int *block2body,
                                          const MeReal *slipfactor,
                                          const MeReal epsilon,
                                          const MeReal hinv)
{          
    int strip,jm,z,k;          
    int step=m_blocks;          
    const int* Jb=block2body;          
    const int* JMb;
    const MeReal *jmptr;
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
            int block, body, notzero;
            const int* Jbodys=Jb;
            const int* JMblock2bodystrip=JMb;
            const MeReal* jmstrip=jmptr;
            int   num_in_JMstrip=num_in_strip[jm];
            int   num_in_Jstrip=num_in_strip[strip];

            __m128 z1 = _mm_setzero_ps();
            __m128 z2 = _mm_setzero_ps();
            __m128 z3 = _mm_setzero_ps();
            __m128 z4 = _mm_setzero_ps();
            __m128 r1,r2;

            for(block=0, notzero=0; block<num_in_JMstrip; block++)
            {
                body=*JMblock2bodystrip;
                if(body!=-1)
                {
                    for(int i=0; i<num_in_Jstrip;i++)
                    {
                        if(body==Jbodys[i])
                        {
                            //Multiply46BlockBy46BlockTranspose(AcholMatrix, jmstrip, J+24*i, notzero);
                            for(int j=0; j<6; j++)
                            {
                                r1 = _mm_load_ps(jmstrip + 4*j);
                                r2 = _mm_load_ps(J+ 24*i + 4*j);
                                z1 = _mm_add_ps(z1 , rbcrmulp(r2, r1, 0));
                                z2 = _mm_add_ps(z2 , rbcrmulp(r2, r1, 1));
                                z3 = _mm_add_ps(z3 , rbcrmulp(r2, r1, 2));
                                z4 = _mm_add_ps(z4 , rbcrmulp(r2, r1, 3));
                            }
                            notzero=1;   //flag true
                        }
                    }
                }
                jmstrip+=24;
                JMblock2bodystrip++;
            }
            if(notzero)
            {
                //set new nonzero blocks
                _mm_store_ps(AMatrix,   z1);
                _mm_store_ps(AMatrix+4, z2);
                _mm_store_ps(AMatrix+8, z3);
                _mm_store_ps(AMatrix+12,z4);
                _mm_store_ps(AcholMatrix,   z1);
                _mm_store_ps(AcholMatrix+4, z2);
                _mm_store_ps(AcholMatrix+8, z3);
                _mm_store_ps(AcholMatrix+12,z4);
                //set sparse ptrs
                NAZ[z]=AMatrix;
                NCZ[z]=AcholMatrix;
                //update matrix block ptrs
                AMatrix+=16;
                AcholMatrix+=16;
                //if(k==unset) k=jm;
                int d=(k-jm)>>31;       //POSS BUG! compiler must a.s.r.
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

    //Complete matix
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
    
    // pad the inverse with 1s along the end of the diagonal so it remains non-singular
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

// sparse multiply ----------------------------------------------------------

void keaMatrix_pcSparse_SSE::multiply(
         MeReal       B[],      /* Output */
         const MeReal X[])      /* Input  */
{
    MeReal* G;
    int     i,j,k,g,h;

    for(i=0, g=0; i<m_blocks; i++, g+=4)
    {
        __m128 z0 = _mm_setzero_ps();
        __m128 z1 = _mm_setzero_ps();
        __m128 z2 = _mm_setzero_ps();
        __m128 z3 = _mm_setzero_ps();
        __m128 z4 = _mm_setzero_ps();
        __m128 r1, r2;

        for(k=0, h=0; k<i; k++, h+=4)
        {
            G=NAZ[i*m_blocks+k];
            if(G!=0)
            {
                //U_Mblock
                r2 = _mm_load_ps(X+h);
                r1 = _mm_load_ps(G);
                z0 = _mm_add_ps(z0 , rbcrmulp(r1, r2, 0));
                r1 = _mm_load_ps(G+4);
                z0 = _mm_add_ps(z0 , rbcrmulp(r1, r2, 1));
                r1 = _mm_load_ps(G+8);
                z0 = _mm_add_ps(z0 , rbcrmulp(r1, r2, 2));
                r1 = _mm_load_ps(G+12);
                z0 = _mm_add_ps(z0 , rbcrmulp(r1, r2, 3));
            }
        }
        for(j=i;j<m_blocks;j++,h+=4)
        {
            G=NAZ[j*m_blocks+i];
            if(G!=0)
            {
                //D_Mblock()
                r2 = _mm_load_ps(X+h);
                r1 = _mm_load_ps(G);
                z1 = _mm_add_ps(z1, _mm_mul_ps(r1, r2));
                r1 = _mm_load_ps(G+4);
                z2 = _mm_add_ps(z2, _mm_mul_ps(r1, r2));
                r1 = _mm_load_ps(G+8);
                z3 = _mm_add_ps(z3, _mm_mul_ps(r1, r2));
                r1 = _mm_load_ps(G+12);
                z4 = _mm_add_ps(z4, _mm_mul_ps(r1, r2));
            }
        }
        //F_Mblock2()
        _mm_store_ps(B+g, _mm_add_ps(z0, horzadd4(z1, z2, z3, z4)));
    }

    /* Zero pad up to next 12 boundary */
    for(i=m_numRows;i!=MeMathCEIL12(m_numRows);i++)
    {
        B[i]=0.0f;
    }

}
/*######## SPARSE BLOCK CHOLESKY FACTORIZE #########*/

//Sparse is as good as dense on a dense matrix so just this implementation

static void sse_strip_Cholesky(MeReal* rsD, int z, int w, int d, int n, MeReal** NZ, MeReal** mcLPptr);
static void sse_dstrip_Cholesky(MeReal* rsD, int z, int d, int s, MeReal** NZ);

//sparse sse cholesky factorize
void keaMatrix_pcSparse_SSE::factorize()
{
#if PROFILE_MDTKEA
    MdtKeaProfileStart("factorize");
#endif
    
    int i,j,z,w;
    int stride=m_blocks;

    // Row Resolve
    for(i=0, z=0; i<m_blocks; i++, z+=stride)
    {
        sse_dstrip_Cholesky(rsD, z, i, NC[i], NCZ);
        for(j=i+1, w=z+stride; j<=NR[i]; j++, w+=stride)
        {
            if(NC[j]<=i)
                sse_strip_Cholesky(rsD, z, w, i, NC[j], NCZ, &mcLP);
        }
    }

#if PRINT_FACTORISER_PCSPARSE_OUTPUT
    printFactoriserPCSparseOutput(NCZ,m_blocks,m_padded);
#endif

#if PROFILE_MDTKEA
    MdtKeaProfileEnd("factorize");
#endif
}

__forceinline void sse_strip_Cholesky(MeReal* rsd, 
                                      int z, 
                                      int w, 
                                      int d, 
                                      int s, 
                                      MeReal** NZ, 
                                      MeReal** mcLPptr)
{
    int i,t,p,n;
    MeReal* a;
    MeReal* b;

    __m128 z1 = _mm_setzero_ps();
    __m128 z2 = _mm_setzero_ps();
    __m128 z3 = _mm_setzero_ps();
    __m128 z4 = _mm_setzero_ps();
    __m128 r1,r2,rd,rz;

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
            for(int k=0; k<4; k++)
            {
                //bcmulad4()
                r1 = _mm_load_ps(a+4*k);
                r2 = _mm_load_ps(b+4*k);
                z1 = _mm_add_ps(z1 , rbcrmulp(r2, r1, 0));
                z2 = _mm_add_ps(z2 , rbcrmulp(r2, r1, 1));
                z3 = _mm_add_ps(z3 , rbcrmulp(r2, r1, 2));
                z4 = _mm_add_ps(z4 , rbcrmulp(r2, r1, 3));
            }
        }
    }
    a=NZ[t];
    b=NZ[p];

    //load reciprocals of diagonal (chol) elements
    rd=_mm_load_ps(&rsd[4*d]);

    if(b!=0)
    {
        //nsse_block_subdiv();
        r2 = _mm_sub_ps(_mm_load_ps(b), z1);
        r2 = _mm_mul_ps(r2, broadcast(rd, 0));
        _mm_store_ps(b, r2);
    
        r1 = _mm_load_ps(a);
        z2 = _mm_add_ps(z2 , rbcrmulp(r2, r1, 1));
        z3 = _mm_add_ps(z3 , rbcrmulp(r2, r1, 2));
        z4 = _mm_add_ps(z4 , rbcrmulp(r2, r1, 3));
        r2 = _mm_sub_ps(_mm_load_ps(b+4), z2);
        r2 = _mm_mul_ps(r2, broadcast(rd, 1));
        _mm_store_ps(b+4, r2);
    
        r1 = _mm_load_ps(a+4);
        z3 = _mm_add_ps(z3 , rbcrmulp(r2, r1, 2));
        z4 = _mm_add_ps(z4 , rbcrmulp(r2, r1, 3));
        r2 = _mm_sub_ps(_mm_load_ps(b+8), z3);
        r2 = _mm_mul_ps(r2, broadcast(rd, 2));
        _mm_store_ps(b+8, r2);
    
        r1 = _mm_load_ps(a+8);
        z4 = _mm_add_ps(z4 , rbcrmulp(r2, r1, 3));
        r2 = _mm_sub_ps(_mm_load_ps(b+12), z4);
        r2 = _mm_mul_ps(r2, broadcast(rd, 3));
        _mm_store_ps(b+12, r2);
    }
    else
    {
        //setup new block
        b = *mcLPptr;
        (*mcLPptr)+=16;
        NZ[p]=b;
        //nsse_block_subdivs();
        rz = _mm_setzero_ps();
        r2 = _mm_sub_ps(rz, z1);
        r2 = _mm_mul_ps(r2, broadcast(rd, 0));
        _mm_store_ps(b, r2);
    
        r1 = _mm_load_ps(a);
        z2 = _mm_add_ps(z2 , rbcrmulp(r2, r1, 1));
        z3 = _mm_add_ps(z3 , rbcrmulp(r2, r1, 2));
        z4 = _mm_add_ps(z4 , rbcrmulp(r2, r1, 3));
        r2 = _mm_sub_ps(rz, z2);
        r2 = _mm_mul_ps(r2, broadcast(rd, 1));
        _mm_store_ps(b+4, r2);

        r1 = _mm_load_ps(a+4);
        z3 = _mm_add_ps(z3 , rbcrmulp(r2, r1, 2));
        z4 = _mm_add_ps(z4 , rbcrmulp(r2, r1, 3));
        r2 = _mm_sub_ps(rz, z3);
        r2 = _mm_mul_ps(r2, broadcast(rd, 2));
        _mm_store_ps(b+8, r2);

        r1 = _mm_load_ps(a+8);
        z4 = _mm_add_ps(z4 , rbcrmulp(r2, r1, 3));
        r2 = _mm_sub_ps(rz, z4);
        r2 = _mm_mul_ps(r2, broadcast(rd, 3));
        _mm_store_ps(b+12, r2);
    }
}
__forceinline void sse_dstrip_Cholesky(MeReal* rsd, int z, int d, int s, MeReal** NZ)
{
    int i,t,n;
    MeReal* a;

    __m128 z1 = _mm_setzero_ps();
    __m128 z2 = _mm_setzero_ps();
    __m128 z3 = _mm_setzero_ps();
    __m128 z4 = _mm_setzero_ps();
    __m128 r1, m1;

    n=d-s;
    t=z+s;

    for(i=0; i<n; i++, t++)
    {
        a=NZ[t];

        if(a!=0)
        {
            //sse_block_mulads();
            for(int k=0; k<4; k++)
            {
                //bcmulads4()
                r1 = _mm_load_ps(a+4*k);
                z1 = _mm_add_ps(z1 , rbcrmulp(r1, r1, 0));
                z2 = _mm_add_ps(z2 , rbcrmulp(r1, r1, 1));
                z3 = _mm_add_ps(z3 , rbcrmulp(r1, r1, 2));
                z4 = _mm_add_ps(z4 , rbcrmulp(r1, r1, 3));
            }
        }
    }
    a=NZ[t];

#define DO_RCP_SSE (0)

#if !DO_RCP_SSE
    float _MM_ALIGN16 tmp[4];
#endif
    //nsse_block_subroot();
    r1 = _mm_sub_ps(_mm_load_ps(a), z1);
#if DO_RCP_SSE
    z1 = rcpsqrt_nr_ps(broadcast(r1, 0));           //FPP
#else
    _mm_store_ps(tmp, r1);
    if(tmp[0])
        tmp[0]=1/MeSqrt(tmp[0]);
    else
        *((int*)&tmp[0])=(0x0ffc00000);
    z1 = rspread(tmp[0]); 
#endif
    r1 = _mm_mul_ps(r1, z1);
    _mm_store_ps(a, m1 = r1);

    z2 = _mm_add_ps(z2 , rbcrmulp(r1, r1, 1));
    z3 = _mm_add_ps(z3 , rbcrmulp(r1, r1, 2));
    z4 = _mm_add_ps(z4 , rbcrmulp(r1, r1, 3));
    r1 = _mm_sub_ps(_mm_load_ps(a+4), z2);
#if DO_RCP_SSE
    z2 = rcpsqrt_nr_ps(broadcast(r1, 1));           //FPP
#else
    _mm_store_ps(tmp, r1);
    if(tmp[1])
        tmp[1]=1/MeSqrt(tmp[1]);
    else
        *((int*)&tmp[1])=(0x0ffc00000);
    z2 = rspread(tmp[1]); 
#endif
    r1 = _mm_mul_ps(r1, z2);

    r1 = _mm_movehl_ps(r1, _mm_unpacklo_ps(m1, r1));
    m1 = _mm_unpackhi_ps(m1, r1);
    _mm_store_ps(a+4, r1);

    z3 = _mm_add_ps(z3 , rbcrmulp(r1, r1, 2));
    z4 = _mm_add_ps(z4 , rbcrmulp(r1, r1, 3));
    r1 = _mm_sub_ps(_mm_load_ps(a+8), z3);
#if DO_RCP_SSE
    z3 = rcpsqrt_nr_ps(broadcast(r1, 2));           //FPP
#else
    _mm_store_ps(tmp, r1);
    if(tmp[2])
        tmp[2]=1/MeSqrt(tmp[2]);
    else
        *((int*)&tmp[2])=(0x0ffc00000);
    z3 = rspread(tmp[2]); 
#endif
    r1 = _mm_mul_ps(r1, z3);

    r1 = _mm_shuffle_ps(m1, r1, shuffhilo);
    _mm_store_ps(a+8, r1);

    m1 = _mm_shuffle_ps(m1, r1, shuffhihi);
    z4 = _mm_add_ps(z4 , rbcrmulp(r1, r1, 3));
    r1 = _mm_sub_ps(_mm_load_ps(a+12), z4);
#if DO_RCP_SSE
    z4 = rcpsqrt_nr_ps(broadcast(r1, 3));           //FPP
#else
    _mm_store_ps(tmp, r1);
    if(tmp[3])
        tmp[3]=1/MeSqrt(tmp[3]);
    else
        *((int*)&tmp[3])=(0x0ffc00000);
    z4 = rspread(tmp[3]); 
#endif
    r1 = _mm_mul_ps(r1, z4);

    r1 = _mm_shuffle_ps(m1, _mm_unpackhi_ps(m1, r1), shuffhilo);
    _mm_store_ps(a+12, r1);

    _mm_store_ps(&rsd[4*d], merge4(z1, z2, z3, z4));
}

// sparse solve ------------------------------------------------

void keaMatrix_pcSparse_SSE::solve(MeReal       x[],
                                   const MeReal b[])
{
#if PRINT_SOLVE_INPUT
    printSolveInput(b,m_blocks);
#endif

    __m128  r1, r2, rd, rs;
    __m128  z1, z2, z3, z4;

    int     i,j,k,t,g,h;
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

    // Solve lower, transposed
    
    for(i=0; i<m_blocks; i++)
    {
        rs = _mm_setzero_ps();

        for(j=0, h=0, k=i*m_blocks; j<i; j++, k++, h+=4)
        {
            G=NCZ[k];
            if(G!=0)
            {
                //do vert dot prod thang (O_sblockL)
                r2 = _mm_load_ps(x+h);
                r1 = _mm_load_ps(G);
                rs = _mm_add_ps(rs , rbcrmulp(r1, r2, 0));
                r1 = _mm_load_ps(G+4);
                rs = _mm_add_ps(rs , rbcrmulp(r1, r2, 1));
                r1 = _mm_load_ps(G+8);
                rs = _mm_add_ps(rs , rbcrmulp(r1, r2, 2));
                r1 = _mm_load_ps(G+12);
                rs = _mm_add_ps(rs , rbcrmulp(r1, r2, 3));
            }
        }
        G=NCZ[k];

        //do diag vert dot prod thingy... store results (D_blockL)
        //(remember that the diagonal blocks are symmetric!)
        //TODO? some of the earlier arith ops could be written scalarly here
        r2 = _mm_load_ps(x+h);
        rd = _mm_load_ps(rsD+h);

        r1 = _mm_load_ps(G);
        z1 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
        rs = _mm_add_ps(rs, _mm_mul_ps(r1, broadcast(z1, 0)));

        r1 = _mm_load_ps(G+4);
        z2 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
        rs = _mm_add_ps(rs, _mm_mul_ps(r1, broadcast(z2, 1)));

        r1 = _mm_load_ps(G+8);
        z3 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
        rs = _mm_add_ps(rs, _mm_mul_ps(r1, broadcast(z3, 2)));

        z4 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));

        _mm_store_ps(x+h, merge4(z1, z2, z3, z4));
    }

    // Solve upper 
    t=(m_blocks-1)*m_blocks;
    for(i=m_blocks-1; i>=0; i--)
    {
        z1 = _mm_setzero_ps();
        z2 = _mm_setzero_ps();
        z3 = _mm_setzero_ps();
        z4 = _mm_setzero_ps();

        for(j=m_blocks-1, g=h, k=t+i; j>i; j--, g-=4)
        {
            G=NCZ[k];
            if (G!=0)
            {
                //do horz dot proddy thang... 4 accums (O_blockU)
                r2 = _mm_load_ps(x+g);
                r1 = _mm_load_ps(G);
                z1 = _mm_add_ps(z1, _mm_mul_ps(r1, r2));
                r1 = _mm_load_ps(G+4);
                z2 = _mm_add_ps(z2, _mm_mul_ps(r1, r2));
                r1 = _mm_load_ps(G+8);
                z3 = _mm_add_ps(z3, _mm_mul_ps(r1, r2));
                r1 = _mm_load_ps(G+12);
                z4 = _mm_add_ps(z4, _mm_mul_ps(r1, r2));
            }
            k-=m_blocks;
        }
        G=NCZ[k];

        //do diag horz dot proddy thingymijigga... store results (D_blockU)
        //(remember that the diagonal blocks are symmetric!)
        //TODO? some of the later arith ops could be written scalarly here
        r2 = _mm_load_ps(x+g);
        rd = _mm_load_ps(rsD+g);
        rs = horzadd4(z1, z2, z3, z4);

        r1 = _mm_load_ps(G+12);
        z4 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
        rs = _mm_add_ps(rs, _mm_mul_ps(r1, broadcast(z4, 3)));

        r1 = _mm_load_ps(G+8);
        z3 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
        rs = _mm_add_ps(rs, _mm_mul_ps(r1, broadcast(z3, 2)));

        r1 = _mm_load_ps(G+4);
        z2 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
        rs = _mm_add_ps(rs, _mm_mul_ps(r1, broadcast(z2, 1)));

        z1 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));

        _mm_store_ps(x+g, merge4(z1, z2, z3, z4));

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


void keaMatrix_pcSparse_SSE::solveUnits(
         MeReal       Ainv[],          /* Output */
         int          cached[],        /* Output / Input  */
         const int    clamped[],       /* Input  */
         int          numClamped,      /* Input  */
         int          AinvStride)      /* Input  */
{
    __m128  r1, r2, rd, rs;
    __m128  z1, z2, z3, z4;

    int c,w,t,step,num_rhs;
    int i,j,k,g,h,v;

    MeReal *G,*res;
    LOCAL_INT_ARRAY(rp,numClamped);

    // Setup rhs units
    for(c=0,t=0;c<numClamped;c++) {
        w=clamped[c];
        if(!cached[w]) {
            //memset(&Ainv[w*m_padded],0,m_padded*sizeof(MeReal));
            rs = _mm_setzero_ps();
            res = &Ainv[w*AinvStride];
            for(int m =0; m<AinvStride; m+=4)
                _mm_store_ps(res+m, rs);
            Ainv[w*(AinvStride+1)]=1;
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
                        res=&Ainv[AinvStride*rp[t]+4*i];
                        //do vert dot prod thang... (O_sblockL)
                        rs = _mm_load_ps(res);
                        r2 = _mm_load_ps(&Ainv[AinvStride*rp[t]+h]);
                        r1 = _mm_load_ps(G);
                        rs = _mm_sub_ps(rs, rbcrmulp(r1, r2, 0));
                        r1 = _mm_load_ps(G+4);
                        rs = _mm_sub_ps(rs, rbcrmulp(r1, r2, 1));
                        r1 = _mm_load_ps(G+8);
                        rs = _mm_sub_ps(rs, rbcrmulp(r1, r2, 2));
                        r1 = _mm_load_ps(G+12);
                        _mm_store_ps(res, _mm_sub_ps(rs, rbcrmulp(r1, r2, 3)));
                    }
                }
            }
        }
        G=NCZ[k];
        for(t=0;t<num_rhs;t++) {
            if(rp[t]>>2<=i) {
                res=&Ainv[AinvStride*rp[t]+4*i];
                rd = _mm_load_ps(rsD+h);
                r2 = _mm_load_ps(res);
                //do diag vert dot prod thingy... store results (D_blockL)
                r1 = _mm_load_ps(G);
                z1 = _mm_mul_ps(rd, r2);
                rs = _mm_mul_ps(r1, broadcast(z1, 0));
        
                r1 = _mm_load_ps(G+4);
                z2 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
                rs = _mm_add_ps(rs, _mm_mul_ps(r1, broadcast(z2, 1)));
        
                r1 = _mm_load_ps(G+8);
                z3 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
                rs = _mm_add_ps(rs, _mm_mul_ps(r1, broadcast(z3, 2)));
        
                z4 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
                _mm_store_ps(res, merge4(z1, z2, z3, z4));
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
                    res=&Ainv[AinvStride*rp[t]+4*i];
                    r2 = _mm_load_ps(&Ainv[AinvStride*rp[t]+g]);
                    //do horz dot proddy thang... O_blockU();
                    z1 = _mm_mul_ps(_mm_load_ps(G),   r2);
                    z2 = _mm_mul_ps(_mm_load_ps(G+4), r2);
                    z3 = _mm_mul_ps(_mm_load_ps(G+8), r2);
                    z4 = _mm_mul_ps(_mm_load_ps(G+12),r2);
                    rs = horzadd4(z1, z2, z3, z4); //surely not...
                    _mm_store_ps(res, _mm_sub_ps(_mm_load_ps(res), rs));
                }
            }
        }
        G=NCZ[k];
        for(t=0; t<num_rhs; t++) {
            res=&Ainv[AinvStride*rp[t]+4*i];
            r2 = _mm_load_ps(res);
            rd = _mm_load_ps(rsD+g);
            //do diag horz dot proddy thingymijigga... (D_blockU)
            r1 = _mm_load_ps(G+12);
            z4 = _mm_mul_ps(rd, r2);
            rs = _mm_mul_ps(r1, broadcast(z4, 3));

            r1 = _mm_load_ps(G+8);
            z3 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
            rs = _mm_add_ps(rs, _mm_mul_ps(r1, broadcast(z3, 2)));

            r1 = _mm_load_ps(G+4);
            z2 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
            rs = _mm_add_ps(rs, _mm_mul_ps(r1, broadcast(z2, 1)));

            z1 = _mm_mul_ps(rd, _mm_sub_ps(r2, rs));
            _mm_store_ps(res, merge4(z1, z2, z3, z4));
        }
    }
}

