#include <stdio.h>
#include <MeMath.h>

#include <MdtKeaProfile.h>
#include <keaLCPSolver.hpp>
#include <keaMemory.hpp>

#include "keaMatrix.hpp"
#include "keaMatrix_PcSparse.hpp"

#include <keaMatrix_tester.hpp>
#include <keaInternal.hpp>
#include <keaDebug.h>

#if (defined PS2)
#   include <keaMatrix_ps244smalldense.hpp>
#   include <keaMatrix_ps2smalldense.hpp>
#   include <keaMatrix_ps2sparse.hpp>
#endif

#if (defined _BUILD_VANILLA)
#   include <keaMatrix_PcSparse_vanilla.hpp>
#elif (defined WIN32)
#   include "keaCheckCPU_sse.hpp"
#   include <keaMatrix_PcSparse_sse.hpp>
#   include <keaMatrix_PcSparse_vanilla.hpp>
#elif (defined _XBOX)
#   include <keaMatrix_PcSparse_sse.hpp>
#else
#   include <keaMatrix_PcSparse_vanilla.hpp>
#endif

#ifdef PS2
#include <keaEeDefs.hpp>
#endif

void keaLCPSolver::allocate(int size)
{
    n            = size;
    n_blocks     = (size+3)>>2;
    n_padded     = n_blocks*4;
    c16c12n      = MeMathCEIL16(MeMathCEIL12(size));
    AinvStride   = c16c12n;

    cached       = (int *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(int)) + 64,"cached");
    x            = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"x");
    w            = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"w");
    upper        = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"upper");
    lower        = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"lower");
    initialSolve = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"initialsolve");
    clampedValues= (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"clampedvalues");
    Ainv         = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n*c16c12n * sizeof(MeReal)) + 64,"Ainv");
    Qrhs         = (MeReal *)keaPoolAlloc(MeMathCEIL64(c16c12n * sizeof(MeReal)) + 64,"Qrhs");

    cached       = (int *)   MeMemory64ALIGN(cached);
    x            = (MeReal *)MeMemory64ALIGN(x);
    w            = (MeReal *)MeMemory64ALIGN(w);
    upper        = (MeReal *)MeMemory64ALIGN(upper);
    lower        = (MeReal *)MeMemory64ALIGN(lower);
    initialSolve = (MeReal *)MeMemory64ALIGN(initialSolve);
    clampedValues= (MeReal *)MeMemory64ALIGN(clampedValues);
    Ainv         = (MeReal *)MeMemory64ALIGN(Ainv);
    Qrhs         = (MeReal *)MeMemory64ALIGN(Qrhs);

#ifdef PS2    
    cached       = (int *)   UNCACHED(cached);       
    x            = (MeReal *)UNCACHED(x);            
    w            = (MeReal *)UNCACHED(w);            
    upper        = (MeReal *)UNCACHED(upper);        
    lower        = (MeReal *)UNCACHED(lower);        
    initialSolve = (MeReal *)UNCACHED(initialSolve); 
    clampedValues= (MeReal *)UNCACHED(clampedValues);
    Ainv         = (MeReal *)UNCACHED(Ainv);         
    Qrhs         = (MeReal *)UNCACHED(Qrhs);         
#endif

#if 0
    if(size<=36) Ainv = (MeReal *)SPR;
#endif

#ifdef _DEBUG
    for(int i=0;i!=c16c12n*c16c12n;i++)
    {
        Ainv[i]=0.0f;
    }
#endif
}

void keaLCPSolver::makeXandW(MeReal *b,
                             int unclamped[],
                             int numUnclamped,
                             int clamped[],
                             int numClamped)
{    
    if(numClamped==n) 
    {
        /*
          every index is clamped
          calculate the residual: w = A*x-b 
        */
        int i;
        A->multiply(w,clampedValues);
        /* set x <= rhs(I) */
        for(i=0; i<numClamped; i++)
            x[clamped[i]] = clampedValues[clamped[i]];
        for(i=0; i<n; i++)
            w[i]-=b[i];
    }
    else if(numClamped==0) 
    {
        /* 
          every index is free,
          copy the original unclamped solution
        */
        int i;
        for(i=0; i<n; i++)
            x[i] = initialSolve[i];
        /* set residual to 0 */
        for(i=0; i<n; i++)
            w[i] = 0.0f;
    }
    else 
    {
#if (!defined PS2)
        // get number of new rhs's 
        int nrhs=0;
        for(int i=0;i<numClamped;i++) 
        {
            int j=clamped[i];
            if(!cached[j])
                nrhs++;
        }

        if(numClamped+nrhs>numUnclamped) 
        { 
            PrincipalSubmatrix(unclamped, numUnclamped, clamped, numClamped, b);
        }
        else 
        {
            PrincipalPivotTransform(unclamped, numUnclamped, clamped, numClamped);
        }
#else
        PrincipalPivotTransform(unclamped, numUnclamped, clamped, numClamped);
#endif
    }
}
////////////////////////////////////////////////////////////////////////////

/* Principal Submatrix Technique
 * -----------------------------
 *
 * This technique is used when the number of clamped forces is high
 */

void keaLCPSolver::PrincipalSubmatrix(int       unclamped[], 
                                      int       numUnclamped,
                                      int       clamped[], 
                                      int       numClamped,
                                      MeReal *  b) 
{
#if (defined PS2)
    keaMatrix_ps244smalldense   ps244DenseQMatrix;
    keaMatrix_ps2smalldense     ps2SmallDenseQMatrix;
    keaMatrix_ps2sparse         ps2SparseQMatrix;
#   if (!defined _BUILD_VANILLA)
        keaMatrix *const            Q =
            (n_padded <= 4) ? (keaMatrix *) &ps244DenseQMatrix
            : (n_padded <= 36) ? (keaMatrix *) &ps2SmallDenseQMatrix
            : (keaMatrix *) &ps2SparseQMatrix;
#   endif
#endif

#if (defined _BUILD_VANILLA)
    keaMatrix_pcSparse_vanilla  vanillaQMatrix;
    keaMatrix *const            Q = &vanillaQMatrix;
#elif (defined WIN32)
    keaMatrix_pcSparse_SSE      sseQMatrix;
    keaMatrix_pcSparse_vanilla  vanillaQMatrix;
    keaMatrix *const            Q =
        (cpuType == (MeCPUResources) MeSSE)
        ? (keaMatrix *) &sseQMatrix
        : (keaMatrix *) &vanillaQMatrix;
#elif (defined _XBOX)
    keaMatrix_pcSparse_SSE      sseQMatrix;
    keaMatrix *const            Q = &sseQMatrix;
#else
    keaMatrix_pcSparse_vanilla  vanillaQMatrix;
#   if (!defined PS2)
        keaMatrix *const        Q = &vanillaQMatrix;
#   endif
#endif

    Q->allocate(numUnclamped);

#ifndef PS2
    ((keaMatrix_pcSparse *)Q)->makeFromPcSparsePSM(
        Qrhs,                     /* Output */
        (keaMatrix_pcSparse *)A,  /* Input */
        b,                        /* Input */
        clampedValues,            /* Input */
        unclamped,                /* Input */
        clamped,                  /* Input */
        numUnclamped,             /* Input */
        numClamped,               /* Input */
        n_blocks);                /* Input */
#endif

    Q->factorize();
    Q->solve(Qrhs,  /* Output */
             Qrhs); /* Input  */

    memset(x + n, 0, (n_padded - n) * sizeof (MeReal));

    /* set x <= rhs(I) */
    
    int i;

    for(i=0; i<numClamped; i++)
        x[clamped[i]] = clampedValues[clamped[i]];
    for(i=0; i<numUnclamped; i++)
        x[unclamped[i]] = Qrhs[i];

    /* calculate the residual: w = A*x-b */
    A->multiply(w,x);
    for(i=0; i<n; i++)
        w[i]-=b[i];

    /* maybe tidy 4 simpler lcp?
    for(i=0; i<numUnclamped; i++)
        w[unclamped[i]]=0;*/
}

void keaLCPSolver::PrincipalPivotTransformMakeW(
        MeReal       w[],          /* Output */
        const MeReal Qrhs[],       /* Input  */
        const int    clamped[],    /* Input  */
        const int    unclamped[],  /* Input  */
        int          numClamped,   /* Input  */
        int          numUnclamped) /* Input  */
{
    int i;

    MdtKeaProfileStart("PPT MakeW");

    /* w:=unpsm(Qrhs,clamped) */
    for (i=0; i<numClamped; i++)
        w[clamped[i]]=Qrhs[i];
    for (i=0; i<numUnclamped; i++)
        w[unclamped[i]]=0;

    MdtKeaProfileEnd("PPT MakeW");

}

void keaLCPSolver::PrincipalPivotTransformMakeX(
    MeReal       x[],            /* Output */
    const MeReal initialSolve[], /* Input  */
    const MeReal Ainv[],         /* Input  */
    const MeReal Qrhs[],         /* Input  */
    const int    clamped[],      /* Input  */
    const int    unclamped[],    /* Input  */
    int          numClamped,     /* Input  */
    int          numUnclamped,   /* Input  */
    int          n,              /* Input  */
    int          n_padded,       /* Input  */
    int          AinvStride)     /* Input  */
{
    int i,j,k;

    MdtKeaProfileStart("PPT MakeX");

    memset(x + n, 0, (n_padded - n) * sizeof (MeReal));
    
    /* set x(Iu) = x1(Iu) + dx(Iu,Ic)*rhs */
    for(i=0; i<numUnclamped; i++) {
        j=unclamped[i];
        x[j]=initialSolve[j];
    }
    for(i=0; i<numClamped; i++) {
        const MeReal *p=Ainv+clamped[i]*AinvStride;
        for(j=0;j<numUnclamped;j++){
            k=unclamped[j];
            x[k]+=p[k]*Qrhs[i];      //SPARSITY??...no
        }
    }

    /* set the element of x not solved for, i.e. those that are clamped. */
    for(i=0; i<numClamped; i++) {
        j=clamped[i];
        x[j]=clampedValues[j];
    }

    MdtKeaProfileEnd("PPT MakeX");
}

/* Principal Pivot Technique
 * -------------------------
 *
 * This method is used when the number of clamped forces is low
 * 
 * On Entry:
 *
 * Ainv         -
 * initialSolve -
 * 
 * On Exit:
 *
 * x            -
 * w            -
 *
 */

#if TEST_KEAMATRIX
keaMatrix_tester           QmatrixComparator;
keaMatrix_pcSparse_vanilla correctQMatrix;
#endif

void keaLCPSolver::PrincipalPivotTransform(
        const int unclamped[], 
        int       numUnclamped,
        const int clamped[], 
        int       numClamped) 
{
#if (defined PS2)
    keaMatrix_ps244smalldense   ps244DenseQMatrix;
    keaMatrix_ps2smalldense     ps2SmallDenseQMatrix;
    keaMatrix_ps2sparse         ps2SparseQMatrix;
#   if (!defined _BUILD_VANILLA)
        keaMatrix *const            Q =
            (n_padded <= 4) ? (keaMatrix *) &ps244DenseQMatrix
            : (n_padded <= 36) ? (keaMatrix *) &ps2SmallDenseQMatrix
            : (keaMatrix *) &ps2SparseQMatrix;
#   endif
#endif

#if (defined _BUILD_VANILLA)
    keaMatrix_pcSparse_vanilla  vanillaQMatrix;
    keaMatrix *const            Q = &vanillaQMatrix;
#elif (defined WIN32)
    keaMatrix_pcSparse_SSE      sseQMatrix;
    keaMatrix_pcSparse_vanilla  vanillaQMatrix;
    keaMatrix *const            Q =
        (cpuType == (MeCPUResources) MeSSE)
        ? (keaMatrix *) &sseQMatrix
        : (keaMatrix *) &vanillaQMatrix;
#elif (defined _XBOX)
    keaMatrix_pcSparse_SSE      sseQMatrix;
    keaMatrix *const            Q = &sseQMatrix;
#else
    keaMatrix_pcSparse_vanilla  vanillaQMatrix;
#   if (!defined PS2)
        keaMatrix *const        Q = &vanillaQMatrix;
#   endif
#endif
 
    /* Some columns of Ainv that we need might not have been calculated
       so do some unit solves to fill them in 
    */

    A->solveUnits(
        Ainv,        /* Output */
        cached,      /* Output / Input */
        clamped,     /* Input */
        numClamped,  /* Input */ 
        AinvStride); /* Input */ 
    
#if TEST_KEAMATRIX
    QmatrixComparator.suspect=Q;
    QmatrixComparator.correct=&correctQMatrix;
    Q=&QmatrixComparator;
#endif
    
    Q->allocate(numClamped);
    
    /* 
       Q->matrixChol := psm(Ainv,clamped)
       Qrhs          := psm(clampedvalues-initsolve,clamped)
    */
    
    Q->makeFromColMajorPSM(
        Qrhs,         /* Output */
        Ainv,         /* Input */
        clampedValues,/* Input */
        initialSolve, /* Input */
        unclamped,    /* Input */
        clamped,      /* Input */
        numUnclamped, /* Input */
        numClamped,   /* Input */
        AinvStride);  /* Input */
    
    Q->factorize();    
    
    Q->solve(Qrhs,  /* Output */
        Qrhs); /* Input  */

    /* PS2: Reload A->matrixChol into vumem, ready for the next unit solve */
    A->prefetchMatrixChol();

    PrincipalPivotTransformMakeW(
        w,             /* Output */
        Qrhs,          /* Input  */
        clamped,       /* Input  */
        unclamped,     /* Input  */
        numClamped,    /* Input  */
        numUnclamped); /* Input  */

    PrincipalPivotTransformMakeX(
        x,             /* Output */
        initialSolve,  /* Input  */
        Ainv,          /* Input  */
        Qrhs,          /* Input  */
        clamped,       /* Input  */
        unclamped,     /* Input  */
        numClamped,    /* Input  */
        numUnclamped,  /* Input  */
        n,             /* Input  */
        n_padded,      /* Input  */
        AinvStride);   /* Input  */

#if PRINT_PPT_OUTPUT
    printPrinciplePivotTransformOutput(x,w,n);
#endif
}

////////////////////////////////////////////////////////

void keaLCPSolver :: setUpper(MeReal* upper)
{
    this->upper=upper;
}

void keaLCPSolver :: setLower(MeReal* lower)
{
    this->lower=lower;
}

int keaLCPSolver :: getFirstBadIndex()
{
    int i;

#if PRINT_GETFIRSTBADINDEX_INPUT
    printGetFirstBadIndexInput(x,n);
#endif

    for(i=0; i<n; i++)
        if (x[i]>upper[i] || x[i]<lower[i])
            break;
    return i;
}

void keaLCPSolver :: copyXtoInitialSolve()
{
    for(int i=0; i<MeMathCEIL4(n); i++) {
        initialSolve[i]=x[i];
    }
}

void keaLCPSolver :: getClampIndices(int* I, int* C)
{
    for(int i = 0; i < n; i++) {
        if (x[i] < lower[i]) {
            I[i] = -1;
            C[i] = 0;
        }
        else if(x[i] > upper[i]) {
            I[i] = -1;
            C[i] = -1;
        }
        else {
            I[i] = 0;
            C[i] = 0;
        }
    }
}

void keaLCPSolver :: setClampedValues(
         int        clamped[],   /* Output */
         int        unclamped[], /* Output */
         int *      csize,       /* Output */
         int *      usize,       /* Output */
         const int  I[],         /* Input  */
         const int  C[])         /* Input  */
{
    for(int i=0; i<n; i++) 
    {
        if(I[i]==0) 
        {
            unclamped[(*usize)++] = i;
        }
        else
        {
            clamped[(*csize)++] = i;
            clampedValues[i]=(C[i]==0 ? lower[i]:upper[i]);
        }
    }
}

/* the pivotal pivoting stuff */

int keaLCPSolver :: commonPivot(int MASK, int I[], int C[], int clampedhere[], int hilohere[])
{
    int DMASK=-1;
    //int DMASK=(7FFFFFFFh)>>(30-iteration);

    for(int i=0;i<n;i++) {
        int CM=MASK;
        int count=0;
        int cyclesize=0;
        int kk=clampedhere[i];
        while(CM!=0) {
            count+=kk&1;
            cyclesize++;
            kk>>=1;
            CM>>=1;
        }
        if(count>cyclesize/2) {
            I[i]=-1;
            C[i]=( fabs(x[i]-lower[i]) < fabs(x[i]-upper[i]) ? 0:-1);
        }
        else
            I[i]=0;
        //Checking new set not cycle
        DMASK&=(~I[i])^(clampedhere[i]);
        if(I[i]&DMASK)
            DMASK&=( (C[i]&hilohere[i]) | ((~C[i])&(~hilohere[i])) );
    }
    return DMASK;
}

/*****************************/

int keaLCPSolver :: blockMurtyChooseNewIndices(
        int       I[],           /* Output */
        int       C[],           /* Output */
        const int clamped[],     /* Input  */       
        const int unclamped[],   /* Input  */ 
        int       num_clamped,   /* Input  */
        int       num_unclamped) /* Input  */
{
    int i, j;
    int indexes_switched = 0;

    for (i = 0; i < num_unclamped; i++) {
        j = unclamped[i];
        /*
        clamp index at lower bound?
        */
        if (x[j] < lower[j]) {              
            I[j] = -1;
            C[j] = 0;
            indexes_switched = 1;
        }
        /*
        clamp index at upper bound?
        */
        if (x[j] > upper[j]) {
            I[j] = -1;
            C[j] = -1;
            indexes_switched = 1;
        }
    }
    
    for (i = 0; i < num_clamped; i++) {
        j = clamped[i];
        /*
        Unclamp index at lower bound if bad residual
        */
        if (C[j] == 0 && w[j] < -velocityZeroTol) {
            I[j] = 0;
            indexes_switched = 1;
        }
        /*
        unclamp index at upper bound if bad residual
        */
        if (C[j] == -1 && w[j] > velocityZeroTol) {
            I[j] = 0;
            indexes_switched = 1;
        }
    }
    return indexes_switched;
}
int keaLCPSolver :: singleMurtyChooseNewIndices(
        int       I[],           /* Output */
        int       C[],           /* Output */
        const int clamped[],     /* Input  */       
        const int unclamped[],   /* Input  */
        const int iorder[],      /* Input  */
        int       num_clamped,   /* Input  */
        int       num_unclamped) /* Input  */
{

    int i, j;
    int indexes_switched = 0;
    
    // switch the first bad index we find.
    // puts("sip\n");
    for(i = 0; i < deadIndex; i++) {
        j = iorder[i];
        
        if (I[j] == 0) {
        /*
        clamp index at lower bound?
            */
            if (x[j] < lower[j]) {
                I[j] = -1;
                C[j] = 0;
                indexes_switched = 1;
                break;
            }
            
            /*
            clamp index at upper bound?
            */
            if (x[j] > upper[j]) {
                I[j] = -1;
                C[j] = -1;
                indexes_switched = 1;
                break;
            }
        }
        else {
        /*
        unclamp index at lower bound if bad residual
            */
            if (C[j] == 0 && w[j] < -velocityZeroTol) {
                I[j] = 0;
                indexes_switched = 1;
                break;
            }
            /*
            unclamp index at upper bound if bad residual
            */
            if (C[j] == -1 && w[j] > velocityZeroTol) {
                I[j] = 0;
                indexes_switched = 1;
                break;
            }
        }
    }
    return indexes_switched;
}
