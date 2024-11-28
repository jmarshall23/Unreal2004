#include <keaMatrix_tester.hpp>
#include <MeMath.h>
#include <keaMemory.hpp>

#ifdef PS2
#include <keaEeDefs.hpp>
#endif    

#define PRINT_DIFFS 0

void checkNorm(
         const MeReal a[],
         const MeReal b[],
         int          numElts,
         const char * desc)
{
    int i;
    MeReal norm=0.0f;

    for(i=0;i!=numElts;i++)
    {
        norm = norm + (a[i]-b[i])*(a[i]-b[i]);
    }

    if(norm>10.0f) 
    {
 
        for(i=0;i!=numElts;i++)
        {
            printf("%12.6f %12.6f %12.6f\n",a[i],b[i],a[i]-b[i]);
        }
        printf("\n");
       
        MeWarning(0,"%s bad norm %e\n",desc,norm);

    }
}

void keaMatrix_tester :: allocate(int n)
{
    this->m_numRows      = n;
    this->m_padded       = MeMathCEIL4(n);

    suspect->allocate(n);
    correct->allocate(n);

    int c16c12n = MeMathCEIL16(MeMathCEIL12(n));

    suspectCached = (int *)   keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(int)) + 64,"cached");
    correctCached = (int *)   keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(int)) + 64,"cached");
    suspectQrhs   = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"Qrhs");
    correctQrhs   = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"Qrhs");
    suspectX      = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"x");
    correctX      = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"x");
    suspectB      = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"w");
    correctB      = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"w");
    suspectAinv   = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n*c16c12n * sizeof(MeReal)) + 64,"Ainv");
    correctAinv   = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n*c16c12n * sizeof(MeReal)) + 64,"Ainv");

    suspectCached = (int *)   MeMemory64ALIGN(suspectCached);
    correctCached = (int *)   MeMemory64ALIGN(correctCached);
    suspectQrhs   = (MeReal *)MeMemory64ALIGN(suspectQrhs);
    correctQrhs   = (MeReal *)MeMemory64ALIGN(correctQrhs);
    suspectX      = (MeReal *)MeMemory64ALIGN(suspectX);   
    correctX      = (MeReal *)MeMemory64ALIGN(correctX);   
    suspectB      = (MeReal *)MeMemory64ALIGN(suspectB);   
    correctB      = (MeReal *)MeMemory64ALIGN(correctB);   
    suspectAinv   = (MeReal *)MeMemory64ALIGN(suspectAinv);
    correctAinv   = (MeReal *)MeMemory64ALIGN(correctAinv);

#ifdef PS2
    suspectCached = (int *)   UNCACHED(suspectCached);
    correctCached = (int *)   UNCACHED(correctCached);
    suspectQrhs   = (MeReal *)UNCACHED(suspectQrhs);
    correctQrhs   = (MeReal *)UNCACHED(correctQrhs);
    suspectX      = (MeReal *)UNCACHED(suspectX);   
    correctX      = (MeReal *)UNCACHED(correctX);   
    suspectB      = (MeReal *)UNCACHED(suspectB);   
    correctB      = (MeReal *)UNCACHED(correctB);   
    suspectAinv   = (MeReal *)UNCACHED(suspectAinv);
    correctAinv   = (MeReal *)UNCACHED(correctAinv);
#endif
}

void keaMatrix_tester :: makeFromJMJT(
                              const MeReal *  JM,
                              const MeReal *  Js,
                              const int *     num_in_strip,
                              const int *     block2body,
                              const MeReal *  slipfactor,
                              const MeReal    epsilon,
                              const MeReal    hinv)
{
    suspect->makeFromJMJT(
        JM,
        Js,
        num_in_strip,
        block2body,
        slipfactor,
        epsilon,
        hinv);

    correct->makeFromJMJT(
        JM,
        Js,
        num_in_strip,
        block2body,
        slipfactor,
        epsilon,
        hinv);

}

void keaMatrix_tester :: makeFromColMajorPSM(
             MeReal          Qrhs[],          /* Output */
             const MeReal *  Ainv,            /* Input */
             const MeReal    clampedValues[], /* Input */
             const MeReal    initialSolve[],  /* Input */
             const int       unclamped[],     /* Input */
             const int       clamped[],       /* Input */
             int             numUnclamped,    /* Input */
             int             numClamped,      /* Input */
             int             n_padded)        /* Input */
{
    int i;

    suspect->makeFromColMajorPSM(
        suspectQrhs,     /* Output */
        Ainv,            /* Input */
        clampedValues,   /* Input */
        initialSolve,    /* Input */
        unclamped,       /* Input */
        clamped,         /* Input */
        numUnclamped,    /* Input */
        numClamped,      /* Input */
        n_padded);       /* Input */

    correct->makeFromColMajorPSM(
        correctQrhs,     /* Output */
        Ainv,            /* Input */
        clampedValues,   /* Input */
        initialSolve,    /* Input */
        unclamped,       /* Input */
        clamped,         /* Input */
        numUnclamped,    /* Input */
        numClamped,      /* Input */
        n_padded);       /* Input */

#if PRINT_DIFFS
    printf("Qrhs\n");
    for(i=0;i!=numClamped;i++)
    {
        printf("%12.6f %12.6f %12.6f\n",correctQrhs[i],suspectQrhs[i],correctQrhs[i]-suspectQrhs[i]);
    }
    printf("\n");
#endif

    checkNorm(correctQrhs,suspectQrhs,numClamped,"make from col major PSM");

    for(i=0;i!=numClamped;i++)
    {
        Qrhs[i]=correctQrhs[i];
    }
}

void keaMatrix_tester :: factorize()
{
    suspect->factorize();
    correct->factorize();

#ifdef PS2
    /*This seems to be needed, but I dont know why */
    FlushCache(0);
#endif
}

void keaMatrix_tester :: solve(
                       MeReal x[],
                       const MeReal rhs[])
{
    int i;

    suspect->solve(suspectX,rhs);
    correct->solve(correctX,rhs);

#if PRINT_DIFFS
    printf("solve\n");
    for(i=0;i!=m_padded;i++)
    {
        printf("%12.6f %12.6f %12.6f\n",correctX[i],suspectX[i],correctX[i]-suspectX[i]);
    }
    printf("\n");
#endif

    checkNorm(correctX,suspectX,m_padded,"solve");

    for(i=0;i!=m_padded;i++)
    {
        x[i]=correctX[i];
    }
}

void keaMatrix_tester :: multiply(
         MeReal       b[],      /* Output */
         const MeReal x[])      /* Input  */
{
    int i;

    suspect->multiply(suspectB,x);
    correct->multiply(correctB,x);

#if PRINT_DIFFS
    printf("multiply\n");
    for(i=0;i!=m_padded;i++)
    {
        printf("%12.6f %12.6f %12.6f\n",correctB[i],suspectB[i],correctB[i]-suspectB[i]);
    }
    printf("\n");
#endif

    checkNorm(correctB,suspectB,m_padded,"multiply");

    for(i=0;i!=m_padded;i++)
    {
        b[i]=correctB[i];
    }

}
    
void keaMatrix_tester :: solveUnits(
         MeReal       Ainv[],          /* Output */
         int          cached[],        /* Output / Input  */
         const int    clamped[],       /* Input  */
         int          numClamped,      /* Input  */
         int          AinvStride)      /* Input  */
{
    int i;

    for(i=0;i!=m_numRows;i++)
    {
        suspectCached[i]=cached[i];
        correctCached[i]=cached[i];
    }

    for(i=0;i!=AinvStride*AinvStride;i++)
    {
        suspectAinv[i]=Ainv[i];
        correctAinv[i]=Ainv[i];
    }

    suspect->solveUnits(
         suspectAinv,     /* Output */
         suspectCached,   /* Output / Input  */
         clamped,         /* Input  */
         numClamped,      /* Input  */
         AinvStride);     /* Input  */
    
    correct->solveUnits(
         correctAinv,     /* Output */
         correctCached,   /* Output / Input  */
         clamped,         /* Input  */
         numClamped,      /* Input  */
         AinvStride);     /* Input  */

    for(i=0;i!=m_numRows;i++)
    {
        cached[i]=correctCached[i];
    }

    for(i=0;i!=numClamped;i++)
    {
        checkNorm(correctAinv+AinvStride*clamped[i],
                  suspectAinv+AinvStride*clamped[i],
                  AinvStride,
                  "solveunits");
    }

    for(i=0;i!=AinvStride*AinvStride;i++)
    {
        Ainv[i]=correctAinv[i];
    }

}

void keaMatrix_tester :: writebackMatrixChol()
{
    suspect->writebackMatrixChol();
    correct->writebackMatrixChol();
}

void keaMatrix_tester :: prefetchMatrixChol()
{
    suspect->prefetchMatrixChol();
    correct->prefetchMatrixChol();
}

