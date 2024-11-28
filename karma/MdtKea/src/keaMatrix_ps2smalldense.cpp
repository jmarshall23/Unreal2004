/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.1.2.6 $

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

#include <MeAssert.h>
#include <eetypes.h>
#include <eeregs.h>
#include <libdma.h>
#include <devvu0.h>
#include <devvif0.h>
#include <stdio.h>
#include <libpc.h>
#include "keaMatrix_ps2smalldense.hpp"
#include <MeMath.h>
#include "keaMemory.hpp"
#include <MdtKeaProfile.h>
#include "calcA.hpp"
#include "keaEeDefs.hpp"
#include <MeMessage.h>
#include <keaDebug.h>
#include <keaInternal.hpp>

/* The Small Cholesky solver works as follows.
 * The A matrix starts at VUMEM[0] and is stored in 'half-wallpapered'
 * format.
 *
 * The final quadword of VUMEM (VUMEM[255]) is loaded with parameters.
 * VUMEM[255].x == n, the basic dimension of the matrix (in elements)
 * VUMEM[255].Y == qwc, the number of quadwords in the half-wallpapered
 *                 representation of A
 * VUMEM[255].z == bqwc, the number of quadwords in the right-hand-side
 *                 (which is n/4).
 *
 * keaSmallCholFactorSolve will decompose the A matrix starting at VUMEM[0]
 * replacing it with its Cholesky decomposition.
 * It also does an initial solve.
 *
 * keaSmallCholSolve uses the Cholesky triangle resident in VUMEM, so
 * only call it after calling Factor, but it can be called many times
 * after Factor to solve for more right-hand-sides.
 *
 * The right-hand-side will be uploaded to an address n quadwords after
 * the end of A. (Those n quadwords are used internally for caching
 * reciprocal square roots).
 *
 * keaSmallCholSolve(MeReal* x) will copy the solution vector out of VUMEM[qwc+n] into
 * x.
 */

#define VUMEM_BASE (0x11004000)
#define ASSERT_ALIGNED(n) MEASSERT((((unsigned int)(n))%0x10)==0)

extern unsigned int small_chol_microcode     __attribute__((section(".vudata")));
extern unsigned int small_chol_microcode_end __attribute__((section(".vudata")));
extern unsigned int solve_only               __attribute__((section(".vudata")));

volatile bool keaSmallCholLoaded=false;

// Uploads the microcode to do factor and solve
static void load_code();

/* Uploads qwc quadwords from src to VUMEM+dest_offset
 * dest_offset is a quadword offset */
static void upload(int dest_offset,const float* src,int qwc);

/* Downloads qwc quadwords from VUMEM+dest_offset to dest
 * dest_offset is a quadword offset */
static void download(int src_offset,float* dest,int qwc);

/* Gives you the VU0 E-bit, which tells you if a microprogram has finished
 * yet. */
static inline int get_E(void)
{
    int rc;

    __asm__ __volatile__("
    sync.l
    sync.p
    cfc2.ni  $8,$29
    andi     %0,$8,0x1
    " : "=r" (rc) : : "$8");

    return rc;
}

// TODO JUST FOR DEBUGGING
void dump_datamem(void)
{
    float* p=(float*)(VUMEM_BASE);
    float buf[1024];
    int i;

    for(i=0;i<1024;i++) buf[i]=p[i];

    for(i=0;i<1024;i+=4){
        printf("%04d: %12.6f %12.6f %12.6f %12.6f (%08x %08x %08x %08x)\n",
               i/4,
               buf[i],buf[i+1],buf[i+2],buf[i+3],
               *(unsigned int*)(&buf[i]),*(unsigned int*)(&buf[i+1]),
               *(unsigned int*)(&buf[i+2]),*(unsigned int*)(&buf[i+3]));
    }
}
/* keaMatrix_ps2smalldense :: factorize()
 * -----------------------------------------
 *
 * Cache status
 *
 * Input:
 * this->Achol      - written by calcJinvMJT, no flush required
 * this->rlist      - not dmad
 * this->llist      - not dmad
 * this->rlist_len  - not dmad
 * this->llist_len  - not dmad
 *
 * Output:
 * this->Achol      - read by solve         , no invalidate required (because it's in VUMEM)
*/
void keaMatrix_ps2smalldense :: factorize()
{
    MdtKeaProfileStart("36 factorize");

#if PRINT_FACTORISER_INPUT
    printFactoriserPS2SmallDenseInput((MeReal *)VUMEM0,
                                      MeMathCEIL4(m_numRows));
#endif

    int c4n = MeMathCEIL4(m_numRows);

    //MEASSERT(n%4==0);
    int qwc=(c4n*c4n)/8+c4n/2;
    int bqwc=c4n/4;

    //** Transfer the factoriser and solver microcode to vu0

    load_code();

    //*** Wait to make sure that the microcode has been uploaded

    sceDmaChan* channel;

    channel=sceDmaGetChan(SCE_DMA_VIF0);

    sceDmaSync(channel,0,0);

    //load_code();

    const int end=255;

    __asm__ __volatile__("
    ctc2     %0, $1    # n
    ctc2     %1, $2    # qwc
    ctc2     %2, $3    # b-qwc
    ctc2     %3, $4

    # the w field needs to be zero
    vmulx    vf01, vf00, vf00

    vmfir.x  vf01, vi01
    vmfir.y  vf01, vi02
    vmfir.z  vf01, vi03

    vsqi     vf01, (vi04++)

    " : : "r" (c4n), "r" (qwc), "r" (bqwc), "r" (end));

    __asm__ __volatile__("
    sync.p
    sync.l

    vcallms  0 # Go
    ");

    while(get_E());

#if PRINT_FACTORISER_PS2SMALLDENSE_OUTPUT
    printFactoriserPS2SmallDenseOutput((MeReal *)VUMEM0,
                                       m_padded);
#endif

#if PROFILE_KEA
    MdtKeaProfileEnd("36 factorize");
#endif
}
/* keaMatrix_ps2smalldense :: solve
 * -----------------------------------
 * Cache status:
 *
 * Input:
 * this->Achol - written by factorize ,not dmad
 * x           - written by LCP       ,not dmad, upload happens by copy loop
 *
 * Output:
 * x           - read by LCP          ,not dmad, download happens by copy loop
*/
void keaMatrix_ps2smalldense :: solve(MeReal        x[],   /* Output */
                                      const MeReal  rhs[]) /* Input  */
{
    int c4n  = MeMathCEIL4(m_numRows);
    int i;

#if PRINT_SOLVE_INPUT
    printSolvePS2SmallDenseInput((MeReal *)VUMEM_BASE,rhs,c4n);
#endif

    MdtKeaProfileStart("36 solve");

    int qwc=(c4n*c4n)/8+c4n/2;
    int offset;

    upload(offset=qwc+c4n,rhs,c4n/4);

    __asm__ __volatile__("
    vcallms  solve_only
    ");

    while(get_E());

    download(offset,x,c4n/4);

    for(i=m_numRows;i!=m_padded;i++)
    {
        x[i]=0.0f;
    }

    MdtKeaProfileEnd("36 solve");

#if PRINT_SOLVE_OUTPUT
    printSolvePS2SmallDenseOutput(x,c4n);
#endif

}

/* Uploads qwc quadwords from src to VUMEM+dest_offset
 * dest_offset is a quadword offset */
void upload(int dest_offset,const float* src,int qwc)
{
    // sceDevVu0Reset();

/*
    float* dest=(float*)(VUMEM_BASE+dest_offset*16);

    for(int i=0;i<qwc*4;i++){
        dest[i]=src[i];
    }
*/
    __asm__ __volatile__("
    ctc2   %0, $1
    " : : "r" (dest_offset));

    for(int i=0;i<qwc*4;i+=16){
        __asm__ __volatile__("
        lqc2   vf01, 0x0(%0)
        lqc2   vf02, 0x10(%0)
        lqc2   vf03, 0x20(%0)
        lqc2   vf04, 0x30(%0)

        vsqi   vf01, ($vi01++)
        vsqi   vf02, ($vi01++)
        vsqi   vf03, ($vi01++)
        vsqi   vf04, ($vi01++)
        " : : "r" (src+i));
    }
}

/* Downloads qwc quadwords from VUMEM+dest_offset to dest
 * dest_offset is a quadword offset */
void download(int src_offset,float* dest,int qwc)
{
/*
    const float* src=(float*)(VUMEM_BASE+src_offset*16);

    for(int i=0;i<qwc*4;i++){
        dest[i]=src[i];
    }
*/

    __asm__ __volatile__("
    ctc2   %0, $1
    " : : "r" (src_offset));

    for(int i=0;i<qwc*4;i+=16){
        __asm__ __volatile__("
        vlqi   vf01, ($vi01++)
        vlqi   vf02, ($vi01++)
        vlqi   vf03, ($vi01++)
        vlqi   vf04, ($vi01++)

        sqc2   vf01, 0x0(%0)
        sqc2   vf02, 0x10(%0)
        sqc2   vf03, 0x20(%0)
        sqc2   vf04, 0x30(%0)
        " : : "r" (dest+i));
    }
}

// Uploads the microcode to do factor and solve
void load_code()
{
    //printf("Loading microcode\n");

    sceDmaChan* channel;

    //sceDmaReset(1);
    channel=sceDmaGetChan(SCE_DMA_VIF0);
    channel->chcr.TTE=0; // Don't send the tag

    int wc=&small_chol_microcode_end-&small_chol_microcode;

    sceDmaSendN(channel,&small_chol_microcode,(wc/4));

    sceDmaSync(channel,0,0);

    keaSmallCholLoaded=true;
}
/*
 * Note that calc JMJT constructs Achol matrix in vumem, then makes copy to A in main mem
*/
void keaMatrix_ps2smalldense :: allocate(int n)
{
#if PRINT_MATRIX_TYPE
    printf("small\n");
#endif

    if(n>36) MeFatalError(1,"keaMatrix_ps2smalldense only works for matrices <= 36*36");

    int ceil12_n      = MeMathCEIL12(n);
    num_12_blocks     = ceil12_n / 12;

    //m_c16c12n         = MeMathCEIL16(MeMathCEIL12(n));
    m_numRows         = n;
    m_padded          = MeMathCEIL4(n);

    matrix            = (MeReal *)keaPoolAlloc(MeMathCEIL64(ceil12_n*ceil12_n * sizeof(MeReal)) + 64,"matrix"    );
    matrixChol        = (MeReal *)keaPoolAlloc(MeMathCEIL64(ceil12_n*ceil12_n * sizeof(MeReal)) + 64,"matrixChol");
    recipSqrt         = (MeReal *)keaPoolAlloc(MeMathCEIL64(ceil12_n*4 * sizeof(MeReal))        + 64,"recipSqrt" );

    matrix            = (MeReal *) MeMemory64ALIGN(matrix);
    matrixChol        = (MeReal *) MeMemory64ALIGN(matrixChol);
    recipSqrt         = (MeReal *) MeMemory64ALIGN(recipSqrt);

    matrix            = (MeReal *) UNCACHED(matrix);
    matrixChol        = (MeReal *) UNCACHED(matrixChol);
    recipSqrt         = (MeReal *) UNCACHED(recipSqrt);
}

static sceDmaTag dmachain[4] __attribute__ ((aligned(64))) = {
    // {short qwc,char padding,char id,void *addr,int pad0,int pad1} (each tag is 1 quadword (16bytes))
    {6,0,0x30,0,{0,0}},
    {6,0,0x30,0,{0,0}},
    {0,0,0x30,0,{0,0}},
    {0,0,0x00,0,{0,0}}
    };

#define SPR_MEREAL       ((MeReal *)SPR)
#define SPR_INT          ((int *)SPR)
#define SPR_CHARSTAR     ((char **)SPR)
#define SPR_CHARSTARSTAR ((char ***)SPR)
#define SPR_INTSTAR      ((int **)SPR)

#define SPRADDR_PTIMING0_WORDS   (BUFFERLENGTH_WORDS*2+0)
#define SPRADDR_PTIMING1_WORDS   (BUFFERLENGTH_WORDS*2+1)
#define SPRADDR_PDESC_WORDS      (BUFFERLENGTH_WORDS*2+2)
#define SPRADDR_NUMTIMINGS_WORDS (BUFFERLENGTH_WORDS*2+3)
#define SPRADDR_TIMING0_WORDS    (BUFFERLENGTH_WORDS*2+4)
#define SPRADDR_TIMING1_WORDS    (BUFFERLENGTH_WORDS*2+4+50)
#define SPRADDR_DESC_WORDS       (BUFFERLENGTH_WORDS*2+4+100)

/* calculate JinvM * transpose(J)
 * ------------------------------
 *
 * On Entry:
 *
 * JM         -
 * j          -
 * jlen       -
 * jbltobody  -
 * slipfactor -
 *
 * Cache status:
 *
 * Input:
 * JM         - written by calcJinvMandrhs    - flush not required
 * j          - written by mdt                - flush not required
 * jlen       - written by calcjlenandbl2body - not dmad
 * slipfactor - written by mdt                - not dmad
 * jbltobody  - written by calcjlenandbl2body - flush not required
 *
 * Output:
 * this->A     - read by multiply              - invalidate required
 * this->Achol - is in vumem                   - no invalidate required
 *
 * Uses a scratchpad double buffer:
 *
 * Iteration | tospr dma                | from spr dma         | processor
 * ---------------------------------------------------------------------------
 * 0           load into input buffer 0   nop                    nop
 *
 * 1           load into input buffer 1   nop                    calc buffer 0
 *
 * 2           load into input buffer 0   save output buffer 0   calc buffer 1
 *
 * 3           load into input buffer 1   save output buffer 1   calc buffer 0
 *
 * etc....
 *
 *
 * toSpr dmachain:
 *
 * ref  | jmblocktobodyandlen | 7 quadwords
 * ref  | jblocktobodyandlen  | 7 quadwords
 * ref  | jm                  | num_jmblocks * 6 quadwords
 * refe | j                   | num_jblocks * 6  quadwords
 *
*/

void keaMatrix_ps2smalldense :: makeFromJMJT(
         const MeReal *  JM,
         const MeReal *  j,
         const int       jlen[],
         const int *     jbltobody,
         const MeReal *  slipfactor,
         const MeReal    epsilon,
         const MeReal    hinv)
{
#if PRINT_CALCJMINVJT_INPUT
    printCalcJinvMJTInput(ji,jmi,j,JM,num_12_blocks);
#endif

#if PROFILE_KEA
    MdtKeaProfileStart("36 calcJinvMJT");
#endif

    int    num_12_blocks = this -> num_12_blocks;
    MeReal *A            = this -> matrix;
    MeReal *Achol        = (MeReal *)VUMEM0;

    /*
     * Main loop
     *
    */

    const int  *pjlen   = jlen;
    int        buffer   = 0;
    MeReal     *Acol    = A;
    MeReal     *Acholcol = Achol;

    for(int col=0;col!=num_12_blocks;col++)
    {
        const int               *pjmlen    = jlen;
        const MeReal            *pjm       = JM;
        MeReal                  *pAchol    = Acholcol;
        MeReal                  *pA        = Acol;
        int                     num_jquads = (pjlen[0]+pjlen[1]+pjlen[2])*6;
        int                     row;

        for(row=0; row!=col+1; ++row)
        {
            //*** Setup toSPR dma chain

            {
            int num_jmquads;

            //** Put the calc block flags that this block will need directly into SPR

            Calc_block_buf *loadbuf = (Calc_block_buf *)( ((u_int)SPR) + buffer*BUFFERLENGTH_BYTES );
            loadbuf->shape          = (row==col);
            loadbuf->gap_for_tag    = 4;

            loadbuf->jm_len[0]   = pjmlen[0];
            loadbuf->jm_len[1]   = pjmlen[1];
            loadbuf->jm_len[2]   = pjmlen[2];
            loadbuf->j_len[0]    = pjlen[0];
            loadbuf->j_len[1]    = pjlen[1];
            loadbuf->j_len[2]    = pjlen[2];
            loadbuf->modify_diag = 0;

            num_jmquads = (pjmlen[0]+pjmlen[1]+pjmlen[2])*6;

            UNCACHED_SCEDMATAG(dmachain)[0].next=(sceDmaTag *)DMAADDR(jbltobody+row*24);
            UNCACHED_SCEDMATAG(dmachain)[1].next=(sceDmaTag *)DMAADDR(jbltobody+col*24);
            UNCACHED_SCEDMATAG(dmachain)[2].next=(sceDmaTag *)DMAADDR(pjm);
            UNCACHED_SCEDMATAG(dmachain)[2].qwc =             num_jmquads;
            UNCACHED_SCEDMATAG(dmachain)[3].next=(sceDmaTag *)DMAADDR(j);
            UNCACHED_SCEDMATAG(dmachain)[3].qwc =             num_jquads;


            if(row==col)
            {
                // this is a diagonal block, so send hinv,epsilon and 3 quadwords of slipfactor
                loadbuf->epsilon     = epsilon;
                loadbuf->hinv        = hinv;
                loadbuf->modify_diag = 1;

                __asm__ __volatile__("
                lqc2 vf1,0x00(%1)
                lqc2 vf2,0x10(%1)
                lqc2 vf3,0x20(%1)
                sqc2 vf1,0x00(%0)
                sqc2 vf2,0x10(%0)
                sqc2 vf3,0x20(%0)
                " : : "r" (&loadbuf->slipfactor[0]) , "r" (&slipfactor[row*12]) );
            }

            pjm     = pjm + num_jmquads*4;
            pjmlen  = pjmlen + 3;
            }

            //*** If there is a block in SPR that needs saving, kick off fromSPR dma

            if( (row+col) > 1 )
            {
                //** Set up dma controller so that executing asm("bc0f") will wait for fromspr and tospr channels to finish
                DPUT_D_STAT(D_STAT_CISfromSPR_M|D_STAT_CIStoSPR_M);
                DPUT_D_PCR(D_PCR_CPCfromSPR_M|D_PCR_CPCtoSPR_M);
                CPUSYNC

                DPUT_CHFROMSPR_QWC(0);
                DPUT_CHFROMSPR_SADR(buffer*BUFFERLENGTH_BYTES);
                CPUSYNC
                DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_CHAIN);
            }
            else
            {
                //*** Set up dma controller so that executing asm("bc0f") will wait for just tospr channel to finish
                DPUT_D_STAT(D_STAT_CIStoSPR_M);
                DPUT_D_PCR(D_PCR_CPCtoSPR_M);
                CPUSYNC
            }

            //*** Kick off toSPR dma

            DPUT_CHTOSPR_SADR(buffer*BUFFERLENGTH_BYTES+BUFFERADDR_INPUT);
            DPUT_CHTOSPR_TADR((u_int)dmachain);
            DPUT_CHTOSPR_QWC (0);
            CPUSYNC
            DPUT_CHTOSPR_CHCR(0x105);

            //** While dma is going on, do some processing

            if((row+col)!=0)
            {
                calc_block(SPR_MEREAL+(1-buffer)*BUFFERLENGTH_WORDS);
            }

            //*** Wait for load and store dma to complete

            BC0FWAIT;

            //*** Setup fromSPR dma chain to get the data back when the time comes
            //*** This has to be done after the current store is completed
            {
                u_int *ptag = (u_int *)( ((u_int)SPR) + buffer*BUFFERLENGTH_BYTES );

                if(col==row)
                {
                    // stuff for diag block

                    ptag[0*4+0]  = DESTCHAIN_TAG_ID_CNT | 4;
                    ptag[0*4+1]  = (u_int)(pAchol + 0           * 16);
                    ptag[0*4+2]  = 0;
                    ptag[0*4+3]  = 0;

                    ptag[5*4+0]  = DESTCHAIN_TAG_ID_CNT | 8;
                    ptag[5*4+1]  = (u_int)(pAchol + (3*col + 1) * 16);
                    ptag[5*4+2]  = 0;
                    ptag[5*4+3]  = 0;

                    ptag[14*4+0] = DESTCHAIN_TAG_ID_CNT | 12;
                    ptag[14*4+1] = (u_int)(pAchol + (6*col + 3) * 16);
                    ptag[14*4+2] = 0;
                    ptag[14*4+3] = 0;

                    ptag[27*4+0]  = DESTCHAIN_TAG_ID_CNT | 4;
                    ptag[27*4+1]  = (u_int)DMAADDR(pA + 0           * 16);
                    ptag[27*4+2]  = 0;
                    ptag[27*4+3]  = 0;

                    ptag[32*4+0]  = DESTCHAIN_TAG_ID_CNT | 8;
                    ptag[32*4+1]  = (u_int)DMAADDR(pA + (3*col + 1) * 16);
                    ptag[32*4+2]  = 0;
                    ptag[32*4+3]  = 0;

                    ptag[41*4+0] = DESTCHAIN_TAG_ID_END | 12;
                    ptag[41*4+1] = (u_int)DMAADDR(pA + (6*col + 3) * 16);
                    ptag[41*4+2] = 0;
                    ptag[41*4+3] = 0;

                }
                else
                {
                    // stuff for non diag block

                    ptag[0*13*4+0] = DESTCHAIN_TAG_ID_CNT | 12;
                    ptag[0*13*4+1] = (u_int)(pAchol + 0           * 16);
                    ptag[1*13*4+0] = DESTCHAIN_TAG_ID_CNT | 12;
                    ptag[1*13*4+1] = (u_int)(pAchol + (3*col + 1) * 16);
                    ptag[2*13*4+0] = DESTCHAIN_TAG_ID_CNT | 12;
                    ptag[2*13*4+1] = (u_int)(pAchol + (6*col + 3) * 16);
                    ptag[3*13*4+0] = DESTCHAIN_TAG_ID_CNT | 12;
                    ptag[3*13*4+1] = (u_int)DMAADDR(pA    + 0           * 16);
                    ptag[4*13*4+0] = DESTCHAIN_TAG_ID_CNT | 12;
                    ptag[4*13*4+1] = (u_int)DMAADDR(pA    + (3*col + 1) * 16);
                    ptag[5*13*4+0] = DESTCHAIN_TAG_ID_END | 12;
                    ptag[5*13*4+1] = (u_int)DMAADDR(pA    + (6*col + 3) * 16);

                }
            }

            pA             = pA + 48;
            pAchol         = pAchol + 48;
            buffer         = 1 - buffer;
        }

        j        = j + num_jquads*4;
        pjlen    = pjlen + 3;
        Acholcol = Acholcol + (6+9*col) * 4*4;
        Acol     = Acol + (6+9*col) * 4*4;
    }

    //*** Epliogue 0

    if(num_12_blocks!=1)
    {
        //*** Save the buffer that was calculated in the main loop but not yet saved

        DPUT_D_STAT(D_STAT_CISfromSPR_M);
        DPUT_D_PCR(D_PCR_CPCfromSPR_M);
        CPUSYNC

        DPUT_CHFROMSPR_QWC(0);
        DPUT_CHFROMSPR_SADR(buffer*BUFFERLENGTH_BYTES);
        CPUSYNC
        DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_CHAIN);

        calc_block(SPR_MEREAL+(1-buffer)*BUFFERLENGTH_WORDS);

        BC0FWAIT;
    }
    else
    {
        calc_block(SPR_MEREAL+(1-buffer)*BUFFERLENGTH_WORDS);
    }

    //*** Epilogue 1
    {
    DPUT_D_STAT(D_STAT_CISfromSPR_M);
    DPUT_D_PCR(D_PCR_CPCfromSPR_M);
    CPUSYNC

    DPUT_CHFROMSPR_QWC(0);
    DPUT_CHFROMSPR_SADR((1-buffer)*BUFFERLENGTH_BYTES);
    CPUSYNC
    DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_CHAIN);

    BC0FWAIT;
    }

//    InvalidDCache((void *)matrix,(void *)(matrix + (num_12_blocks*(num_12_blocks+1)/2)*144 ));

#if PRINT_CALCJMINVJT_PS2SMALLDENSE_OUTPUT
    //printCalcJinvMJTps2smalldense_Output(A,rlist,rlist_len,num_12_blocks);
#endif

#if PROFILE_KEA
    MdtKeaProfileEnd("36 calcJinvMJT");
#endif
}
/*
 * B:=A*X
*/
void keaMatrix_ps2smalldense :: multiply(
         MeReal       B[], /* Output */
         const MeReal X[]) /* Input  */
{

    int i,j,k,l;
    const MeReal *sourceblock;
    const MeReal *sourcevector;
    MeReal *destvector;

    int c4n = MeMathCEIL4(m_numRows);

#if 0
    printf("sourcevector\n");
    for(i=0;i!=c4n;i++)
    {
         printf("%12.6f\n",X[i]);
    }
    printf("-------------\n");

    printf("first block of A\n");
    for(i=0;i!=4;i++)
    {
        for(j=0;j!=4;j++)
        {
            printf("%12.6f ",A[4*i+j]);
        }
        printf("\n");
    }
    printf("-------------\n");
#endif

    for(i=0;i!=c4n/4;i++)
    {
        destvector   = B + i*4;
        for(l=0;l!=4;l++)
        {
            destvector[l] = 0.0f;
        }

        for(j=0;j!=i+1;j++)
        {
            sourceblock  = matrix + (i*(i+1)/2+j)*16;
            sourcevector = X + j*4;

            //printf("block %d\n",(i*(i+1)/2+j));

            for(k=0;k!=4;k++)
            {
                for(l=0;l!=4;l++)
                {
                    destvector[l] = destvector[l] + sourceblock[k*4+l]*sourcevector[k];
                    //printf("%12.6f * %12.6f\n",sourceblock[k*4+l],sourcevector[k]);
                }
            }
        }

        for(j=i+1;j!=c4n/4;j++)
        {
            sourceblock  = matrix + (j*(j+1)/2+i)*16;
            sourcevector = X + j*4;

            for(k=0;k!=4;k++)
            {
                for(l=0;l!=4;l++)
                {
                    destvector[l] = destvector[l] + sourceblock[l*4+k]*sourcevector[k];
                }
            }
        }

    }

    /* Zero pad up to next 12 boundary */
    for(i=m_numRows;i!=num_12_blocks*12;i++)
    {
        B[i]=0.0f;
    }

}
/* solveUnits
 * ----------
 *
 * On Entry:
 *
 * Ainv       - the matrix to insert the solve results into
 * clamped    - 
 * cached     - 
 * numClamped - 
 *
 */
extern "C" int fr;

void keaMatrix_ps2smalldense :: solveUnits(
         MeReal       Ainv[],     /* Output */
         int          cached[],   /* Output/Input */
         const int    clamped[],  /* Input  */
         int          numClamped, /* Input  */
         int          AinvStride) /* Input  */
{
    int c;
    int index;

    MdtKeaProfileStart("36 solveUnits");

    //** Transfer the factoriser and solver microcode to vu0

    load_code();

    for(c=0;c!=numClamped;c++) 
    {
        index = clamped[c];
        if(!cached[index]) 
        {
            memset(Ainv + index*AinvStride,0,AinvStride*sizeof(MeReal));
            Ainv[index*AinvStride+index]=1;
            cached[index]=1;
            {
                int c4n  = MeMathCEIL4(m_numRows);                                
                int qwc=(c4n*c4n)/8+c4n/2;
                int offset;
                int bqwc=c4n/4;
                const int end=255;
                
                upload(offset=qwc+c4n,Ainv + index*AinvStride,c4n/4);

#if PRINT_SOLVE_UNIT_INPUT
                if(fr==51)
                {
                printf("solve unit input\n");
                printSolvePS2SmallDenseInput(
                    (MeReal *)VUMEM_BASE,
                    (MeReal *)(VUMEM_BASE+(qwc+c4n)*16),
                    c4n);
                }
#endif

                /* Tell the solver how big the matrix is etc */

                __asm__ __volatile__("
                ctc2     %0, $1    # n
                ctc2     %1, $2    # qwc
                ctc2     %2, $3    # b-qwc
                ctc2     %3, $4

                # the w field needs to be zero
                vmulx    vf01, vf00, vf00

                vmfir.x  vf01, vi01
                vmfir.y  vf01, vi02
                vmfir.z  vf01, vi03

                vsqi     vf01, (vi04++)

                " : : "r" (c4n), "r" (qwc), "r" (bqwc), "r" (end));
                
                /* Call the microcode solver */

                __asm__ __volatile__("
                    sync.p
                    sync.l

                    vcallms  solve_only
                ");
                    
                while(get_E());
                    
                download(offset,Ainv + index*AinvStride,c4n/4);                    

#if PRINT_SOLVE_UNIT_OUTPUT
                if(fr==51)
                {
                printf("solve unit output\n");
                printSolvePS2SmallDenseOutput((MeReal *)(VUMEM_BASE+(qwc+c4n)*16),c4n);
                }
#endif
            }
        }
    }

    MdtKeaProfileEnd("36 solveUnits");

}

/*
 * makeAfromPSM
 * ------------
 *
 * this->A := PSM(inMatrix,clamped,numClamped)
 *
 * Note that this->A is in wallpaper format, and this function takes the PSM
 * and converts to wallpaper format at the same time.
*/

void keaMatrix_ps2smalldense :: makeFromColMajorPSM(
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
    int i,j,k,l;
    MeReal *pA;

#if PRINT_PPT_MAKE_Q_INPUT
    if(numClamped<=4)
    {
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
    }
#endif

    MdtKeaProfileStart("36 Make Q");

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
    
    /* Make the Q matrix */

    pA = (MeReal *)VUMEM0;
    for(i=0;i!=MeMathFLOOR4(numClamped)/4;i++)
    {
        for(j=0;j!=i+1;j++)
        {
            for(k=0;k!=4;k++)
            {
                for(l=0;l!=4;l++)
                {
                    (*pA++) = 
                        Ainv[clamped[i*4+l] * AinvStride + clamped[j*4+k]];
                }
            }
        }
    }

    /* Do the rightmost column */

    if((numClamped-MeMathFLOOR4(numClamped))!=0)
    {
        /* Loop over the blocks in the column */
        for(j=0;j!=i;j++)
        {
            /* Loops over the rows in the block */
            for(k=0;k!=4;k++)
            {
                for(l=0;l!=numClamped-MeMathFLOOR4(numClamped);l++)
                {
                    (*pA++) = 
                        Ainv[clamped[i*4+l] * AinvStride + clamped[j*4+k]];
                }
                /* Add the padding at the end of the row */

                for(l=numClamped-MeMathFLOOR4(numClamped);l!=4;l++)
                {
                    (*pA++) = 0.0f;
                }
            }
        }

        /* Do the bottom block in the last column */ 
        for(k=0;k!=numClamped-MeMathFLOOR4(numClamped);k++)
        {
            for(l=0;l!=numClamped-MeMathFLOOR4(numClamped);l++)
            {
                (*pA++) = 
                        Ainv[clamped[i*4+l] * AinvStride + clamped[j*4+k]];
            }
            /* Add the padding at the end of the row */
            for(l=numClamped-MeMathFLOOR4(numClamped);l!=4;l++)
            {
                (*pA++) = 0.0f;
            }
        }
        
        /* Pad the remaining diagonal with 1s */
        for(k=numClamped-MeMathFLOOR4(numClamped);k!=4;k++)
        {
            for(l=0;l!=4;l++) pA[l]=0.0f;
            pA[k]=1.0f;
            pA+=4;
        }
    }

    MdtKeaProfileEnd("36 Make Q");

#if PRINT_PPT_MAKE_Q_OUTPUT
    printMakeFromColMajorPSMOutput_ps2smalldense(
        (MeReal *)VUMEM0, /* Input */
        m_padded);        /* Input */
#endif
}

/* 
 *  Copy the matrixChol in vumem back to its home in main memory
 *  This should be done to the A matrix immediately after factorisation
 *  so that we can pull it back for later solve units if vumem gets trashed by Q
 */
void keaMatrix_ps2smalldense :: writebackMatrixChol()
{
    MdtKeaProfileStart("36 writeback matrixChol");

    /* todo - use dma, and only transfer the used part of the matrix */

    int numBlocks = m_padded/4;

    int i;
    MeReal *psrc = (MeReal *)VUMEM0;

#if WRITE_CHOL_BACK_TO_SPR
    
#if 1
    for(i=0;i!=16*numBlocks*(numBlocks+1)/2;i++)
    {
        ((MeReal *)SPR)[i]=*(psrc++);
    }
#endif

#if 0
    __asm__ __volatile__("
    ctc2   %0, $1
    " : : "r" (0));

    for(int i=0;i<16*numBlocks*(numBlocks+1)/2;i+=16){
        __asm__ __volatile__("
        vlqi   vf01, ($vi01++)
        vlqi   vf02, ($vi01++)
        vlqi   vf03, ($vi01++)
        vlqi   vf04, ($vi01++)

        sqc2   vf01, 0x0(%0)
        sqc2   vf02, 0x10(%0)
        sqc2   vf03, 0x20(%0)
        sqc2   vf04, 0x30(%0)
        " : : "r" (((MeReal *)SPR)+i));
    }
#endif

#else
    /* writeback matrixChol */
    for(i=0;i!=16*numBlocks*(numBlocks+1)/2;i++)
    {
        matrixChol[i]=*(psrc++);
    }
#endif

    /* writeback recipSqrt array (1 quadword per element, only x is used)*/
    for(i=0;i!=numBlocks*16;i++)
    {
        recipSqrt[i]=*(psrc++);
    }

#if PRINT_WRITEBACKMATRIXCHOL_OUTPUT
    printWritebackMatrixCholOutput(
        (MeReal *)VUMEM0+16*numBlocks*(numBlocks+1)/2,
        numBlocks*4);
#endif
    MdtKeaProfileEnd("36 writeback matrixChol");
}

/* 
 *  Copy matrixChol from main memory to vumem, so that we can do unit solves on it later
 *  This should be done after any 36*36 Q solve, as the Q factor will have written over
 *  the copy of Achol in vumem
 */
void keaMatrix_ps2smalldense :: prefetchMatrixChol()
{
    MdtKeaProfileStart("36 prefetch matrixChol");

    int i;

    int numBlocks = m_padded/4;

    MeReal *psrc = (MeReal *)VUMEM0;

#if WRITE_CHOL_BACK_TO_SPR

#if 1    
    for(i=0;i!=16*numBlocks*(numBlocks+1)/2;i++)
    {
        *(psrc++)=((MeReal *)SPR)[i];
    }
#endif

#if 0
    __asm__ __volatile__("
    ctc2   %0, $1
    " : : "r" (0));

    for(int i=0;i<16*numBlocks*(numBlocks+1)/2;i+=16){
        __asm__ __volatile__("
        lqc2   vf01, 0x0(%0)
        lqc2   vf02, 0x10(%0)
        lqc2   vf03, 0x20(%0)
        lqc2   vf04, 0x30(%0)

        vsqi   vf01, ($vi01++)
        vsqi   vf02, ($vi01++)
        vsqi   vf03, ($vi01++)
        vsqi   vf04, ($vi01++)

        " : : "r" (((MeReal *)SPR)+i));
    }
#endif

#else    
    for(i=0;i!=16*numBlocks*(numBlocks+1)/2;i++)
    {
        *(psrc++)=matrixChol[i];
    }
#endif

    /* Prefetch recipsqrt from mem to vumem0, 
       1 quadword for each matrix elt 
    */
    for(i=0;i!=numBlocks*16;i++)
    {
        *(psrc++)=recipSqrt[i];
    }

    MdtKeaProfileEnd("36 prefetch matrixChol");

}
