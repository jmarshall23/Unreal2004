#include <keaMatrix_PcSparse.hpp>
#include <keaDebug.h>

#define GS(_i,_j) ((n_blocks)*((_j)>>2)+((_i)>>2))
#define GM(_i,_j) (((_j)>>2)*n_padded*4+((_j)&3)+4*(_i))

void keaMatrix_pcSparse :: makeFromPcSparsePSM(
             MeReal                      Qrhs[],          /* Output */
             const keaMatrix_pcSparse *  A,               /* Input */
             const MeReal                b[],             /* Input */
             const MeReal                clampedValues[], /* Input */
             const int                   unclamped[],     /* Input */
             const int                   clamped[],       /* Input */
             int                         numUnclamped,    /* Input */
             int                         numClamped,      /* Input */
             int                         n_blocks)        /* Input */
{
    int i,j,r,k;

    int mb=m_blocks;
    int mp=m_padded;

    /* Build wallpapered Q, with padding*/

    MeReal* ri=matrixChol;
    /* set up sparse maps (no sparsity) */
    for(i=0; i<mb; i++) {
        for(j=0; j<mb; j++)
            NCZ[i*mb+j]=ri+(i*mb+j)*16;
        NC[i]=0;
        NR[i]=mb-1;
    }

    /* build... */
    for(i=0, r=0; i<numUnclamped; i++) {
        j=unclamped[i];
        for(k=0; k<numUnclamped; k++) {
            int m=unclamped[k];
            MeReal* rj=A->NAZ[(m<=j)?GS(m,j):GS(j,m)];
            if(rj!=0) {
                ri[4*k+r]=rj[(m<=j)?((4*(m&3))+(j&3)):((4*(j&3))+(m&3))];
            }
            else
                ri[4*k+r]=0;
        }
        /* padding */
        for(;k<mp;k++)
            ri[4*k+r]=0;
        /* next column */
        r=(r+1)&3;
        ri+=((~(-r))&mp)<<2;
    }

    /* padding */
    if(r!=0)
    {
    for(;r!=4;r++)
        for(k=0;k<mp;k++)
            ri[4*k+r]=0;
    }
    // pad the inverse with 1s along the end of the diagonal so it remains non-singular
    k--;
    r--;
    i=((numUnclamped-1)&3)+1;
    for(;i<4;i++,k--,r--)
        ri[4*k+r]=1.0f;

    /* Build padded rhs for solve */
    for(i=0; i<numUnclamped; i++)
        Qrhs[i]=b[unclamped[i]];
    /* padding */
    for(;i<mp;i++)
        Qrhs[i]=0;
    for(i=0; i<numClamped; i++) {
        j=clamped[i];
        if(clampedValues[j]!=0) {
            for(k=0; k<numUnclamped; k++) {
                int m=unclamped[k];
                MeReal* rj=A->NAZ[(m<=j)?GS(m,j):GS(j,m)];
                if(rj!=0)
                    Qrhs[k]-=rj[(m<=j)?((4*(m&3))+(j&3)):((4*(j&3))+(m&3))]*clampedValues[j];
            }
        }
    }
#if PRINT_PSM_MAKE_Q_OUTPUT
#endif
}

/*
 *  Initialises this->matrixChol from a psm of the input matrix
 *  The input matrix is in column major order
 */
void keaMatrix_pcSparse :: makeFromColMajorPSM(
         MeReal          Qrhs[],          /* Output */       
         const MeReal    Ainv[],          /* Input */
         const MeReal    clampedValues[], /* Input */
         const MeReal    initialSolve[],  /* Input */
         const int       unclamped[],     /* Input */
         const int       clamped[],       /* Input */
         int             numUnclamped,    /* Input */
         int             numClamped,      /* Input */
         int             AinvStride)      /* Input */
{
    int i,j,r,k;

#if PRINT_PPT_MAKE_Q_INPUT
    printMakeFromColMajorPSMInput(
        Ainv,            /* Input */
        clampedValues,   /* Input */
        initialSolve,    /* Input */
        unclamped,       /* Input */
        clamped,         /* Input */
        numUnclamped,    /* Input */
        numClamped,      /* Input */
        m_padded,        /* Input */
        AinvStride);     /* Input */
#endif

    int mb=m_blocks;
    int mp=m_padded;

    MeReal* ri=matrixChol;
    // set up sparse maps (no sparsity)
    for(i=0; i<mb; i++) 
    {
        for(j=0; j<mb; j++)
            NCZ[i*mb+j]=ri+(i*mb+j)*16;
        NC[i]=0;
        NR[i]=mb-1;
    }

    // build... 
    for(i=0, r=0; i<numClamped; i++) 
    {
        j=clamped[i];
        const MeReal *rj=Ainv+j*AinvStride;
        for(k=0; k<numClamped; k++)
        {
            ri[4*k+r]=rj[clamped[k]];
        }
        Qrhs[i]=clampedValues[j]-initialSolve[j];
        // padding 
        for(;k<mp;k++)
        {
            ri[4*k+r]=0;
        }
        // next column 
        r=(r+1)&3;
        ri+=((~(-r))&mp)<<2;
    }

    // Q padding
    
    if(r!=0)
    {
        for(;r!=4;r++)
        {
            for(k=0;k<mp;k++)
            {
                ri[4*k+r]=0;
            }
        }
    }

    // rhs padding 
    for(;i<mp;i++)
        Qrhs[i]=0;

    // pad the inverse with 1s along the end of the diagonal so it remains non-singular
    k--;
    r--;
    i=((numClamped-1)&3)+1;
    for(;i<4;i++,k--,r--)
        ri[4*k+r]=1.0f;

#if PRINT_PPT_MAKE_Q_OUTPUT
    printMakeFromColMajorPSMOutput(rsD,
                                   NAZ,
                                   NCZ,
                                   NR,
                                   NC,
                                   m_blocks,
                                   m_padded);
#endif

}

