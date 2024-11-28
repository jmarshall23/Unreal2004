#include <keaMatrix_ps244smalldense.hpp>
#include <MeAssert.h>
#include <stdio.h>
#include <MeMath.h>
#include <keaMemory.hpp>
#include <MeMessage.h>
#include <keaEeDefs.hpp>
#include <MdtKeaProfile.h>
#include <keaDebug.h>

#define MACROCODE

void keaMatrix_ps244smalldense :: factorize()
{
    MdtKeaProfileStart("4 factorize");

#if PRINT_FACTORISER_INPUT
    printMat44(matrixChol);
#endif
    __asm__ __volatile__("
    __expression_asm

    col0
    col1
    col2
    col3
    
    'lqc2 @col0,0x00(%0)
    'lqc2 @col1,0x10(%0)
    'lqc2 @col2,0x20(%0)
    'lqc2 @col3,0x30(%0)

    Q=K.w/ | col0.x
    'vwaitq
    col0.xyzw=col0.xyzw*Q		
    'sqc2 @col0,0x00(%0)

    ACC.yzw=col1.yzw+K.x		
    ACC.yzw=ACC.yzw-col0.yzw*col0.y	
    col1.yzw=ACC.yzw+K*K.x
    Q=K.w/ | col1.y
    'vwaitq
    col1.yzw=col1.yzw*Q             
    'sqc2 @col1,0x10(%0)
    		
    ACC.zw=col2.zw+K.x			
    ACC.zw=ACC.zw-col0.zw*col0.z	
    ACC.zw=ACC.zw-col1.zw*col1.z	
    col2.zw=ACC.zw+K*K.x			
    Q=K.w/ | col2.z
    'vwaitq 
    col2.zw=col2.zw*Q

    'sqc2 @col2,0x20(%0)
    
    ACC.w=col3.w+K.x			
    ACC.w=ACC.w-col0.w*col0.w	
    ACC.w=ACC.w-col1.w*col1.w	    
    ACC.w=ACC.w-col2.w*col2.w
    col3.w=ACC.w+K*K.x
    Q=K.w/ | col3.w
    'vwaitq
    col3.w=col3.w*Q           	
    'sqc2 @col3,0x30(%0)

    ~col0
    ~col1
    ~col2
    ~col3
    
    __end_expression_asm
    " : : "r" (matrixChol));
    
    MdtKeaProfileEnd("4 factorize");
    
}

void keaMatrix_ps244smalldense :: solve(
         MeReal       x[],   /* Output */
         const MeReal rhs[]) /* Input  */
{
    int i;

#if PRINT_SOLVE_INPUT
    printSolvePS244SmallDenseInput(matrixChol,rhs);
#endif

    MdtKeaProfileStart("4 solve");

    /* 
       If dest and source dont point to the same place, 
       then copy source to dest, so we can solve in-place
    */

    if(x!=rhs)
    {
        for(i=0;i!=4;i++)
        {
            x[i]=rhs[i];
        }
    }

    __asm__ __volatile__("
    __expression_asm
    a0
    a1
    a2
    a3
    x
    t

    'lqc2 @a0,0x00(%0)
    'lqc2 @a1,0x10(%0)
    'lqc2 @a2,0x20(%0)
    'lqc2 @a3,0x30(%0)
    'lqc2 @x ,0x00(%1)

    Q = K.w/a0.x
    'vwaitq
    x.x = x * Q

    ACC = a0 * x.x
    t = ACC + K * K.x

    Q = K.w/a1.y
    x.y = x - t
    'vwaitq
    x.y = x * Q

    Q = K.w/a2.z    
    ACC = ACC + a1 * x.y
    t = ACC + K * K.x

    x.z = x - t
    'vwaitq
    x.z = x * Q

    Q = K.w/a3.w
    ACC = ACC + a2 * x.z
    t = ACC + K * K.x

    x.w = x - t
    'vwaitq
    x.w = x * Q

    ones

    ones.xyz = K + K.w
    ones.w   = K + K.x

    Q       = K.w / a3.w
    'vwaitq
    x.w     = x.w * Q

    Q       = K.w / a2.z
    t.w     = a2 * x
    ACC.z   = x.z + K.x
    x.z     = ACC.z - ones * t.w
    'vwaitq
    x.z     = x.z * Q
    
    Q       = K.w / a1.y
    t.zw    = a1 * x  
    ACC.y   = x.y + K.x
    ACC.y   = ACC.y - ones * t.z 
    x.y     = ACC.y - ones * t.w 
    'vwaitq
    x.y     = x.y * Q

    Q       = K.w / a0.x
    t.yzw   = a0 * x
    ACC.x   = x.x + K.x
    ACC.x   = ACC.x - ones * t.y
    ACC.x   = ACC.x - ones * t.z
    x.x     = ACC.x - ones * t.w
    'vwaitq
    x.x     = x.x * Q

    'sqc2 @x,0x00(%1)

    ~a0
    ~a1
    ~a2
    ~a3
    ~x
    ~t
    ~ones
    __end_expression_asm
    " : : "r" (matrixChol), "r" (x));

    /* Zero pad up to next 12 boundary */
    for(i=m_numRows;i!=12;i++)
    {
        x[i]=0.0f;
    }

    MdtKeaProfileEnd("4 solve");

#if PRINT_SOLVE_OUTPUT
    printSolvePS244SmallDenseOutput(x);
#endif

}
void keaMatrix_ps244smalldense :: allocate(int n)
{
    MEASSERT(n<=4);

#if PRINT_MATRIX_TYPE
    printf("small4\n");
#endif

    this->m_numRows    = n;
    this->m_padded     = 4;

    /* This matrix is either a factorisable, solveable matrix or a 
     * factorisable, solvable, LCP solveable matrix
     * If it is the latter, then extra data is needed, so allocate it here
    */

    matrix         = (MeReal *)keaPoolAlloc(MeMathCEIL64(4*4 * sizeof(MeReal)) + 64,"matrix");
    matrixChol     = (MeReal *)keaPoolAlloc(MeMathCEIL64(4*4 * sizeof(MeReal)) + 64,"matrixChol");

    matrix         = (MeReal *)MeMemory64ALIGN(matrix    );
    matrixChol     = (MeReal *)MeMemory64ALIGN(matrixChol);
    
    matrix         = (MeReal *)UNCACHED(matrix);        
    matrixChol     = (MeReal *)UNCACHED(matrixChol);
}

MeReal jmjtconstants[4] __attribute__ ((aligned(64)));

void keaMatrix_ps244smalldense :: makeFromJMJT(
         const MeReal *  JM,
         const MeReal *  J,
         const int *     jlen,
         const int *     jbltobody,
         const MeReal *  slipfactor,
         const MeReal    epsilon,
         const MeReal    hinv)
{
    int i,j;

    MdtKeaProfileStart("4 calcJinvMJT");

    __asm__ __volatile__("
    __expression_asm
    r0
    r1
    r2
    r3

    r0 = K - K
    r1 = K - K
    r2 = K - K
    r3 = K - K
    __end_expression_asm
    ");

    for(i=0;i!=jlen[0];i++)
    {
        int jmbody       = jbltobody[i];
        const MeReal *pj = J; 

        //printf("% 3d ",jbltobody[i]);

        __asm__ __volatile__("
        __expression_asm

        jm0
        jm1
        jm2
        jm3
        jm4
        jm5
        'lqc2  @jm0, 0x00(%0)
        'lqc2  @jm1, 0x10(%0)
        'lqc2  @jm2, 0x20(%0)
        'lqc2  @jm3, 0x30(%0)
        'lqc2  @jm4, 0x40(%0)
        'lqc2  @jm5, 0x50(%0)

        __end_expression_asm
        " : : "r" (JM));

        for(j=0;j!=jlen[0];j++)
        {
            if(jbltobody[j]==jmbody)
            {
                __asm__ __volatile__("
                __expression_asm
                j0
                j1
                j2
                j3
                j4
                j5

                'lqc2  @j0,  0x00(%0)
                'lqc2  @j1,  0x10(%0)
                'lqc2  @j2,  0x20(%0)
                'lqc2  @j3,  0x30(%0)
                'lqc2  @j4,  0x40(%0)
                'lqc2  @j5,  0x50(%0)

                ACC = r0 + K.x
                ACC = ACC + j0 * jm0.x
                ACC = ACC + j1 * jm1.x
                ACC = ACC + j2 * jm2.x
                ACC = ACC + j3 * jm3.x
                ACC = ACC + j4 * jm4.x
                r0  = ACC + j5 * jm5.x

                ACC = r1 + K.x
                ACC = ACC + j0 * jm0.y
                ACC = ACC + j1 * jm1.y
                ACC = ACC + j2 * jm2.y
                ACC = ACC + j3 * jm3.y
                ACC = ACC + j4 * jm4.y
                r1  = ACC + j5 * jm5.y

                ACC = r2 + K.x
                ACC = ACC + j0 * jm0.z
                ACC = ACC + j1 * jm1.z
                ACC = ACC + j2 * jm2.z
                ACC = ACC + j3 * jm3.z
                ACC = ACC + j4 * jm4.z
                r2  = ACC + j5 * jm5.z

                ACC = r3 + K.x
                ACC = ACC + j0 * jm0.w
                ACC = ACC + j1 * jm1.w
                ACC = ACC + j2 * jm2.w
                ACC = ACC + j3 * jm3.w
                ACC = ACC + j4 * jm4.w
                r3  = ACC + j5 * jm5.w

                ~j0
                ~j1
                ~j2
                ~j3
                ~j4
                ~j5

                __end_expression_asm
                " : : "r" (pj) );            
            }
            pj  = pj + 24;
        }

        __asm__ __volatile__("
        __expression_asm

        ~jm0
        ~jm1
        ~jm2
        ~jm3
        ~jm4
        ~jm5
        __end_expression_asm
        ");
        
        JM = JM + 24;
    }
    //printf("\n");

    jmjtconstants[0] = epsilon + hinv*slipfactor[0];
    jmjtconstants[1] = epsilon + hinv*slipfactor[1];
    jmjtconstants[2] = epsilon + hinv*slipfactor[2];
    jmjtconstants[3] = epsilon + hinv*slipfactor[3];

    //printf("epsilon=%12.6f\n",jmjtconstants[0]);

    __asm__ __volatile__("
    __expression_asm

    epsilon

    'lqc2  @epsilon,0x00(%1)
    r0.x = r0 + epsilon.x
    r1.y = r1 + epsilon.y
    r2.z = r2 + epsilon.z
    r3.w = r3 + epsilon.w

    'sqc2  @r0,0x00(%0)
    'sqc2  @r1,0x10(%0)
    'sqc2  @r2,0x20(%0)
    'sqc2  @r3,0x30(%0)
    'sqc2  @r0,0x00(%2)
    'sqc2  @r1,0x10(%2)
    'sqc2  @r2,0x20(%2)
    'sqc2  @r3,0x30(%2)

    ~epsilon
    ~r0
    ~r1
    ~r2
    ~r3
    __end_expression_asm
    " : : "r" (matrix) ,"r" (jmjtconstants), "r" (matrixChol));    

	//** Pad the diagonal with ones

	for(i=m_numRows;i!=4;i++)
	{
		matrix[4*i+i]     = 1.0f;
		matrixChol[4*i+i] = 1.0f;
	}

    MdtKeaProfileEnd("4 calcJinvMJT");

}
void keaMatrix_ps244smalldense :: multiply(
         MeReal       b[],      /* Output */
         const MeReal x[])      /* Input  */
{
    int i;

    __asm__ __volatile__("
    __expression_asm
    A0
    A1
    A2
    A3
    X

    'lqc2 @A0,0x00(%1)
    'lqc2 @A1,0x10(%1)
    'lqc2 @A2,0x20(%1)
    'lqc2 @A3,0x30(%1)
    'lqc2 @X ,0x00(%2)

    ACC =       A0 * X.x
    ACC = ACC + A1 * X.y
    ACC = ACC + A2 * X.z
    X   = ACC + A3 * X.w

    'sqc2 @X,0x00(%0)
    ~A0
    ~A1
    ~A2
    ~A3
    ~X
    __end_expression_asm
    " : : "r" (b), "r" (matrix), "r" (x) );

    /* Zero pad up to next 12 boundary */
    for(i=m_numRows;i!=12;i++)
    {
        b[i]=0.0f;
    }

}

void keaMatrix_ps244smalldense :: solveUnits(
         MeReal       Ainv[],          /* Output */
         int          cached[],        /* Output / Input  */
         const int    clamped[],       /* Input  */
         int          numClamped,      /* Input  */
         int          AinvStride)      /* Input  */
{
    int c;
    int index;

    for(c=0;c!=numClamped;c++) 
    {
        index = clamped[c];
        if(!cached[index]) 
        {
            memset(Ainv + index*AinvStride,0,AinvStride*sizeof(MeReal));
            Ainv[index*AinvStride+index]=1;
            cached[index]=1;
            {
                solve(Ainv + index*AinvStride,
                      Ainv + index*AinvStride);
            }
        }
    }
}

void keaMatrix_ps244smalldense :: makeFromColMajorPSM(
         MeReal          Qrhs[],          /* Output */
         const MeReal *  Ainv,            /* Input */
         const MeReal    clampedValues[], /* Input */
         const MeReal    initialSolve[],  /* Input */
         const int       unclamped[],     /* Input */
         const int       clamped[],       /* Input */
         int             numUnclamped,    /* Input */
         int             numClamped,      /* Input */
         int             AinvStride)      /* Input */
{
    int i,j;
    MeReal *pA;

    MdtKeaProfileStart("4 Make Q");

    /* Make the Qrhs */

    for(i=0; i!=numClamped; i++) 
    {
        j=clamped[i];
        Qrhs[i]=clampedValues[j]-initialSolve[j];
    }
    for(i=numClamped; i!=MeMathCEIL4(numClamped); i++) 
    {
        Qrhs[i]=0.0f;
    }

    /* Make the Qchol matrix */

    pA = matrixChol;
    for(i=0;i!=numClamped;i++)
    {
        for(j=0;j!=numClamped;j++)
        {
            (*pA++) = 
                Ainv[clamped[j] * AinvStride + clamped[i]];            
        }
        for(j=numClamped;j!=4;j++)
        {
            (*pA++) = 0.0f;            
        }
    }
    for(i=numClamped;i!=4;i++)
    {
        for(j=0;j!=4;j++) pA[j]=0.0f;
        pA[j]=1.0f;
        pA+=4;
    }

    MdtKeaProfileEnd("4 Make Q");
}
