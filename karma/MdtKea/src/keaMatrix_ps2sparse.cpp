/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.2.2.6 $

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

#include <MeMemory.h>

#include "keaMatrix_ps2sparse.hpp"
#include "keaInternal.hpp"
#include "MeMath.h"
#include "MdtKeaProfile.h"
#include "keaMemory.hpp"
#include "keaEeDefs.hpp"
#include "calcA.hpp"

#include <stdio.h>
#include <MeAssert.h>
#include "eeregs.h"
#include "eestruct.h"

#include "libgraph.h"
#include "libpc.h"
#include "eekernel.h"
#include "devvu0.h"
#include "devvif0.h"
#include "libdma.h"

//*** Pointers to the microcode in main memory

extern unsigned int ssyrk_dma_start __attribute__((section(".vudata")));
extern unsigned int ssyrk_dma_end __attribute__((section(".vudata")));
extern unsigned int MicrocodeBkMat1212_ssyrk __attribute__((section(".vudata")));

extern unsigned int sgemm_strsm_dma_start __attribute__((section(".vudata")));
extern unsigned int sgemm_strsm_dma_end __attribute__((section(".vudata")));
extern unsigned int MicrocodeBkMat1212_sgemm __attribute__((section(".vudata")));
extern unsigned int MicrocodeBkMat1212_strsm __attribute__((section(".vudata")));

extern unsigned int chol_dma_start __attribute__((section(".vudata")));
extern unsigned int chol_dma_end __attribute__((section(".vudata")));
extern unsigned int MicrocodeBkMat1212_chol __attribute__((section(".vudata")));

extern unsigned int sgemv_dma_start __attribute__((section(".vudata")));
extern unsigned int sgemv_dma_end   __attribute__((section(".vudata")));
extern unsigned int sgemvSubNoTrans __attribute__((section(".vudata")));
extern unsigned int sgemvSubTrans   __attribute__((section(".vudata")));
extern unsigned int strsv           __attribute__((section(".vudata")));
extern unsigned int strsvTrans      __attribute__((section(".vudata")));

extern unsigned int trmuladd        __attribute__((section(".vudata")));


//**************************************************
// Location of microcode in scratchpad

    #define SPRADDR_SSYRK       0x1000
    #define SPRADDR_CHOL        0x2000
    #define SPRADDR_SGEMM_STRSM 0x3000

//**************************************************

//*** These macros wrap the sony dma macros to make them more readable

#define SPRBASE     0x70000000

//*** Function prototypes

void doFirstRow(float *a,
                float **nzPairs,                                      /* Input / Output  */
                int *llist,int *llist_len,int *rlist_len,             /* Input / Output  */
                int j,int n,int ld                                    /* Input           */
               );

void doOtherRows(float *a,          /* Input / Output  */
                 float zeroBlock[], /* Input / Output  */
                 float ** nzPairs,  /* Input / Output  */
                 int *llist,        /* Input / Output  */
                 int *rlist,        /* Input / Output  */
                 int *llist_len,    /* Input / Output  */
                 int *rlist_len,    /* Input / Output  */
                 int j,             /* Input */
                 int n,             /* Input */
                 int ld             /* Input */
                );
void saveTile(float *dest,const float *src);

void send_microcode_1212sgemv();

/* microcodeSpTileMatCholesky
 * --------------------------
 * A->rlist
 *
 * On entry, whole matrix is in rlist
 * On exit, whole matrix is in llist
*/

void keaMatrix_ps2sparse :: factorize()
{
    MdtKeaProfileStart("ps2sparse factorize");

    int j;

    float *a          = this->matrixChol;

    //*** Make a copy of rlist_len, because we will want to overwrite it

    for(j=0;j!=num_12_blocks;j++)
    {
        rlist_len_copy[j] = this->rlist_len[j];
    }

    //*** Dma all the microcode to the scratchpad so that it can be quickly dmad to vu0

    //sceDmaReset(1);
     //DPUT_D_STAT(0x0000ff01);
    //DPUT_D_STAT(DGET_D_STAT() & 0xff010000);

    send_microcode_ssyrk_chol_sgemm_strsm_to_scratchpad();

    //*** Loop over the tile columns of the matrix

    for(j=0; j!=num_12_blocks; ++j)
    {
        doFirstRow(a,                                /* Input / output  */
                   nzPairs,                          /* Input / output  */
                   llist,
                   llist_len,
                   rlist_len_copy,                   /* Input / output  */
                   j,num_12_blocks,ld                /* Input           */
                  );

        //*** Do all the tile rows beneath the first tile row

        doOtherRows(a,                                  /* Input / output  */
                    zeroBlock,                          /* Input / output  */
                    nzPairs,                            /* Input / output  */
                    llist,
                    rlist,
                    llist_len,
                    rlist_len_copy,                     /* Input / output  */
                    j,num_12_blocks,ld                  /* Input           */
                   );

        a = a + (j+1)*12*12;
    }

    //** The very next thing that we will do on vu0 is LCP, so send the microcode for solves and multiplies over
    send_microcode_1212sgemv();

    MdtKeaProfileEnd("ps2sparse factorize");

#if PRINT_FACTORISER_PS2SPARSE_OUTPUT
    printFactoriseps2sparse_Output(matrixChol,rlist,rlist_len,num_12_blocks);
#endif

}

/* doFirstRow
 * ----------
 * a             - points to the start of row j in the a matrix
 * llist         -
 * llist_len     -
 * rlist_len     -
 * j             -
 * num_12_blocks -
 * ld            -
*/

void doFirstRow(
         float *a,
         float **nzPairs,
         int *llist,
         int *llist_len,
         int *rlist_len,
         int j,
         int num_12_blocks,
         int ld)
{
    int num_pairs;

    //*** Make a list of the non-zero blocks on row j

    for(num_pairs=0; num_pairs!=llist_len[j]; ++num_pairs)
    {
        nzPairs[num_pairs] = a + num_pairs*12*12;
    }

    //*** Make a vif chain that processes the first row (j)

    //printf("first row (%d) elt %08x\n",j,a + (j+1-rlist_len[j])*12*12);

    make_first_row_vif_prog( a + (j+1-rlist_len[j])*12*12, /* Pointer to block j,j of the A matrix (part of source) */
                            num_pairs,                     /* Length of non-zero pairs list                      */
                            nzPairs                        /* The non-zero pairs list (pointers into A)          */
                           );

    //*** Execute the vif chain and wait for it to finish

    //*** Set vi1 to 0 to tell double bufferer to read Abuffer0 on the first iteration
    asm __volatile__ ("ctc2.i $0,$vi01");
    execute_vif_chain();

    //*** Insert diagonal tile back into matrix

    saveTile((float *)UNCACHED( a + llist_len[j]*12*12 ),(float *)0x11004b40);
    llist[j*ld + llist_len[j]] = j;
    // An element has been added to the left list, so increment its length
    llist_len[j]               = llist_len[j] + 1;
}

/* doOtherRows
 * -----------
 *
 * a             - pointer to start of row j
 * j             - the column of the destination we are doing
 * num_12_blocks - the size of the matrix
 * ld            -
 * llist[i*ld+j] - element j of left list i
 * rlist[i*ld+j] - element j of right list i
 * llist_len[i]  - length of left list i
 * rlist_len[i]  - length of right list i
*/

void doOtherRows(
         float a[],           /* Input / Output  */
         float zeroBlock[],   /* Input / Output  */
         float ** nzPairs,    /* Input / Output  */
         int   llist[],       /* Input / Output  */
         int   rlist[],       /* Input / Output  */
         int   llist_len[],   /* Input / Output  */
         int   rlist_len[],   /* Input / Output  */
         int   j,             /* Input */
         int   num_12_blocks, /* Input */
         int   ld)            /* Input */
{
    int i,k,l,num_matching_pairs;

    float **mynzpairs = nzPairs;

    float *arowj;
    int *li;
    int *lj;

    if (j+1!=num_12_blocks)
        send_microcode_1212sgemm_strsm();

    //*** Point a     to the start of row j+1
    //*** Point arowj to the start of row j

    arowj = a;
    a     = a + (j+1)*12*12;

    for (i=j+1; i!=num_12_blocks; ++i)
    {
        //*** Find list of nonzero pairs

        li                 = llist+i*ld;
        lj                 = llist+j*ld;
        k                  = 0;
        l                  = 0;
        num_matching_pairs = 0;

        while(k!=llist_len[i] && li[k]<j && l!=llist_len[j])
        {
            if      (li[k] == lj[l])
            {
                mynzpairs[num_matching_pairs+0] = arowj + l*12*12;
                mynzpairs[num_matching_pairs+1] = a     + k*12*12;
                ++k; ++l;
                num_matching_pairs+=2;
            }
            else if (li[k] < lj[l]) ++k;
            else                    ++l;
        }

        #if 0
        int ii,jj;
        printf("row %d\n",i);
        printf("getting S from %d\n",i+1 - rlist_len[i]);
        printf("subtract in pair products ");
        for(ii=0;ii!=num_matching_pairs/2;ii++)
        {
            printf("(%d,%d) ",((u_int)mynzpairs[ii+0]),
                ((u_int)mynzpairs[ii+1]));
        }
        printf("\n");
        #endif

        #if 0
        if(i==3 && num_12_blocks==5)
        {
            #if 0
            printf("num_matching_pairs=%d (%08x,%08x)\n",
                num_matching_pairs/2,mynzpairs[0],mynzpairs[1]);
            printf("lj=[%d %d %d] llist_len[j]=%d\n",
                lj[0],lj[1],lj[2],llist_len[j]);
            printf("li=[%d] llist_len[i]=%d\n",li[0],llist_len[i]);
            printf("---- Column %d\n",j);
            printf("rlist_len[i]=%d\n",rlist_len[i]);
            printf("row starts at %08x\n",a);
            #endif

            printf("mynzpairs[0]\n");
            int ii,jj;
            for(ii=0;ii!=12;ii++)
            {
                for(jj=0;jj!=12;jj++)
                {
                    printf("%12.6f ",*(mynzpairs[0]+ii%4+(ii/4)*4*12+jj*4));
                }
                printf("\n");
            }

            printf("mynzpairs[1]\n");

            for(ii=0;ii!=12;ii++)
            {
                for(jj=0;jj!=12;jj++)
                {
                    printf("%12.6f ",*(mynzpairs[1]+ii%4+(ii/4)*4*12+jj*4));
                }
                printf("\n");
            }

            printf("a + (i+1 - rlist_len[i])*12*12\n");

            for(ii=0;ii!=12;ii++)
            {
                for(jj=0;jj!=12;jj++)
                {
                    printf("%12.6f ",*((a + (i+1 - rlist_len[i])*12*12)+ii%4+(ii/4)*4*12+jj*4));
                }
                printf("\n");
            }

        }
        #endif

        /*
          The only reason we would not want to do the following is if:
          there are no pairs AND 'block (i,j)' zero because in this
          case, the answer is definitely zero
        */

        if(num_matching_pairs!=0||rlist[i*ld + rlist_len[i] - 1] == j)
        {
            /*
              if block(i,j) is non zero then subtract stuff from it

              if block(i,j) is zero then it wont be stored in the
              matrix, so subtract stuff from the zero block
            */

            if(rlist[i*ld + rlist_len[i] - 1] == j)
            {
                make_fold_1212sgemm_thenstrsm_vifchain(mynzpairs,
                    num_matching_pairs/2,
                    a + (i+1 - rlist_len[i])*12*12
                );
            }
            else
            {
                //if(num_12_blocks==5 && i==3) printf("subtracting from zero\n");
                for(k=0;k!=12*12;k++) zeroBlock[k]=0.0f;
                make_fold_1212sgemm_thenstrsm_vifchain(mynzpairs,
                    num_matching_pairs/2,
                    zeroBlock
                );
            }

            //*** Set vi1 to 0 to tell double bufferer to read Abuffer0
            //    on the first iteration
            asm __volatile__ ("ctc2.i $0,$vi01");

            execute_vif_chain();

            //*** Save the processed block back to the matrix
            //*** finished tile is at 0x11004900

            saveTile((float *)UNCACHED( a + llist_len[i]*12*12 ),(float *)0x11004900);
            llist[i*ld + llist_len[i]] = j;

            // An element has been added to left list, so increment its length
            llist_len[i] = llist_len[i] + 1;

            if(rlist[i*ld + rlist_len[i] - 1] == j)
            {
#if 0
                if(num_12_blocks==5 && i==3) printf("head of rlist removed\n");
#endif
                //Remove an element of the left list by decrementing its length
                rlist_len[i] = rlist_len[i] - 1;
            }
        }

        a+=(i+1)*12*12;
    }
}

void saveTile(float *dest,const float *src)
{
    __asm__ __volatile__("
    lqc2 $vf1,0x000(%1);
    lqc2 $vf2,0x010(%1);
    lqc2 $vf3,0x020(%1);
    lqc2 $vf4,0x030(%1);
                         sqc2 $vf1,0x000(%0)
    lqc2 $vf1,0x040(%1); sqc2 $vf2,0x010(%0)
    lqc2 $vf2,0x050(%1); sqc2 $vf3,0x020(%0)
    lqc2 $vf3,0x060(%1); sqc2 $vf4,0x030(%0)
    lqc2 $vf4,0x070(%1);
             sqc2 $vf1,0x040(%0)
    lqc2 $vf1,0x080(%1); sqc2 $vf2,0x050(%0)
    lqc2 $vf2,0x090(%1); sqc2 $vf3,0x060(%0)
    lqc2 $vf3,0x0a0(%1); sqc2 $vf4,0x070(%0)
    lqc2 $vf4,0x0b0(%1);
             sqc2 $vf1,0x080(%0)
    lqc2 $vf1,0x0c0(%1); sqc2 $vf2,0x090(%0)
    lqc2 $vf2,0x0d0(%1); sqc2 $vf3,0x0a0(%0)
    lqc2 $vf3,0x0e0(%1); sqc2 $vf4,0x0b0(%0)
    lqc2 $vf4,0x0f0(%1);
                         sqc2 $vf1,0x0c0(%0)
    lqc2 $vf1,0x100(%1); sqc2 $vf2,0x0d0(%0)
    lqc2 $vf2,0x110(%1); sqc2 $vf3,0x0e0(%0)
    lqc2 $vf3,0x120(%1); sqc2 $vf4,0x0f0(%0)
    lqc2 $vf4,0x130(%1);
                         sqc2 $vf1,0x100(%0)
    lqc2 $vf1,0x140(%1); sqc2 $vf2,0x110(%0)
    lqc2 $vf2,0x150(%1); sqc2 $vf3,0x120(%0)
    lqc2 $vf3,0x160(%1); sqc2 $vf4,0x130(%0)
    lqc2 $vf4,0x170(%1);
             sqc2 $vf1,0x140(%0)
    lqc2 $vf1,0x180(%1); sqc2 $vf2,0x150(%0)
    lqc2 $vf2,0x190(%1); sqc2 $vf3,0x160(%0)
    lqc2 $vf3,0x1a0(%1); sqc2 $vf4,0x170(%0)
    lqc2 $vf4,0x1b0(%1);
                         sqc2 $vf1,0x180(%0)
    lqc2 $vf1,0x1c0(%1); sqc2 $vf2,0x190(%0)
    lqc2 $vf2,0x1d0(%1); sqc2 $vf3,0x1a0(%0)
    lqc2 $vf3,0x1e0(%1); sqc2 $vf4,0x1b0(%0)
    lqc2 $vf4,0x1f0(%1);
             sqc2 $vf1,0x1c0(%0)
    lqc2 $vf1,0x200(%1); sqc2 $vf2,0x1d0(%0)
    lqc2 $vf2,0x210(%1); sqc2 $vf3,0x1e0(%0)
    lqc2 $vf3,0x220(%1); sqc2 $vf4,0x1f0(%0)
    lqc2 $vf4,0x230(%1);
             sqc2 $vf1,0x200(%0)
               sqc2 $vf2,0x210(%0)
                   sqc2 $vf3,0x220(%0)
                         sqc2 $vf4,0x230(%0)

    " : : "r" (dest), "r" (src) : "$8" );
}

void send_microcode_ssyrk_chol_sgemm_strsm_to_scratchpad()
{

    //printf("ssyrk size=%d\n",(&ssyrk_dma_end-&ssyrk_dma_start)/4);
    //printf("chol size=%d\n",(&chol_dma_end-&chol_dma_start)/4);
    //printf("sgemm size=%d\n",(&sgemm_strsm_dma_end-&sgemm_strsm_dma_start)/4);

    //*** Make sure dma actually finished

    dma_safety_net_tospr("before send microcode to scratchpad");

    //*** sceDmaSendN(chSpr,&ssyrk_dma_start,(&ssyrk_dma_end-&ssyrk_dma_start)/4);

    DPUT_D_PCR       (D_PCR_CPCtoSPR_M);
    DPUT_D_STAT      (D_STAT_CIStoSPR_M);
    CPUSYNC

    DPUT_CHTOSPR_SADR(SPRADDR_SSYRK);
    DPUT_CHTOSPR_MADR((u_int)(&ssyrk_dma_start));
    DPUT_CHTOSPR_QWC((&ssyrk_dma_end-&ssyrk_dma_start)/4);

    CPUSYNC
    DPUT_CHTOSPR_CHCR(D_CHCR_START | D_CHCR_MODE_NORMAL | D_CHCR_DIRECTION_FROMMEM);

    BC0FWAIT;

    //*** Make sure dma actually finished

    dma_safety_net_tospr("after send ssyrk to spr");

    //*** sceDmaSendN(chSpr,&chol_dma_start,(&chol_dma_end-&chol_dma_start)/4);

    DPUT_D_PCR       (D_PCR_CPCtoSPR_M);
    DPUT_D_STAT      (D_STAT_CIStoSPR_M);
    CPUSYNC

    DPUT_CHTOSPR_SADR(SPRADDR_CHOL);
    DPUT_CHTOSPR_MADR((u_int)(&chol_dma_start));
    DPUT_CHTOSPR_QWC((&chol_dma_end-&chol_dma_start)/4);

    CPUSYNC
    DPUT_CHTOSPR_CHCR(D_CHCR_START | D_CHCR_MODE_NORMAL | D_CHCR_DIRECTION_FROMMEM);

    BC0FWAIT;

    //*** Make sure dma actually finished

    dma_safety_net_tospr("after send chol to spr");

    //*** sceDmaSendN(chSpr,&sgemm_strsm_dma_start,(&sgemm_strsm_dma_end-&sgemm_strsm_dma_start)/4);

    DPUT_D_PCR       (D_PCR_CPCtoSPR_M);
    DPUT_D_STAT      (D_STAT_CIStoSPR_M);
    CPUSYNC

    DPUT_CHTOSPR_SADR(SPRADDR_SGEMM_STRSM);
    DPUT_CHTOSPR_MADR((u_int)(&sgemm_strsm_dma_start));
    DPUT_CHTOSPR_QWC((&sgemm_strsm_dma_end-&sgemm_strsm_dma_start)/4);

    CPUSYNC
    DPUT_CHTOSPR_CHCR(D_CHCR_START | D_CHCR_MODE_NORMAL | D_CHCR_DIRECTION_FROMMEM);

    BC0FWAIT;

    dma_safety_net_tospr("after send sgemm strsm to spr");

}
void send_microcode_1212sgemm_strsm()
{

    //*** Send the microcode program to the scratchpad

    //*** Make sure dma actually finished

    dma_safety_net_vif0("before send sgemm strsm to vif0");

    //*** sceDmaSendN(chSprToVU0, (void *)(0x80000000|SPRADDR_SGEMM_STRSM),(&sgemm_strsm_dma_end-&sgemm_strsm_dma_start)/4);

    DPUT_D_PCR        (D_PCR_CPCVIF0_M);
    DPUT_D_STAT       (D_STAT_CISVIF0_M);
    CPUSYNC

    DPUT_CHTOVIF0_MADR((u_int)(0x80000000|SPRADDR_SGEMM_STRSM));
    DPUT_CHTOVIF0_QWC((&sgemm_strsm_dma_end-&sgemm_strsm_dma_start)/4);

    CPUSYNC
    DPUT_CHTOVIF0_CHCR(D_CHCR_START | D_CHCR_TAG_TRANSFER_ENABLE | D_CHCR_MODE_NORMAL | D_CHCR_DIRECTION_FROMMEM);

    BC0FWAIT;

    dma_safety_net_vif0("after send sgemm strsm to vif0");

}

//****************************
// Instruction stream:
//
// load Ajj,144
// if len_xs>0
//   send ssyrk microcode (mpg)
//   load xs[0],ABUFFER0
//   for(i=1;i!=len_xs;i++)
//     load xs[i],current_buffer | run ssyrk
//   rof
//   run ssyrk | flushe
// fi
// send chol microcode (mpg)
// end | run chol (mscal) | flushe
//****************************
void make_first_row_vif_prog(float *Ajj,int len_xs,float **xs)
{
    sceDmaTag *vifcodeptr=(sceDmaTag *)(SPRBASE);
    sceDmaTag *tag_start;
    int i;


    #define VUMEMADDR_ABUFFER0 0
    #define VUMEMADDR_BBUFFER0 36
    #define VUMEMADDR_ABUFFER1 72
    #define VUMEMADDR_BBUFFER1 108
    #define VUMEMADDR_S        180

    //*** load Ajj,144
    tag_start = vifcodeptr;
    sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(Ajj));
    tag_start->p[0] = 0;                                                   // NOP
    tag_start->p[1] = SCE_VIF0_SET_UNPACK(VUMEMADDR_S, 36,0xc, 0);         // tell vif to unpack 36 quadwords to S
    tag_start->mark = (char)0;

    if(len_xs>0)
    {
        //*** send ssyrk microcode
        int num_quadwords=(&ssyrk_dma_end-&ssyrk_dma_start)/4;
        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, num_quadwords, (void *)(0x80000000|SPRADDR_SSYRK));
        tag_start->p[0] = 0;                                               // NOP
        tag_start->p[1] = 0;
        tag_start->mark = (char)0;

        int current_buffer=VUMEMADDR_ABUFFER0;
        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(xs[0]));
        tag_start->p[0] = 0;                                                   // NOP
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);      // tell vif to unpack 36 quadwords to ABUFFER0
        tag_start->mark = (char)0;
        current_buffer=VUMEMADDR_ABUFFER1-current_buffer;

        for(i=1;i!=len_xs;i++)
        {
            tag_start = vifcodeptr;
            sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(xs[i]));
            tag_start->p[0] = 0x14000000;                                      // mscal ssyrk
            tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);  // tell vif to unpack 36 quadwords to ABUFFER0
            tag_start->mark = (char)0;

            current_buffer=VUMEMADDR_ABUFFER1-current_buffer;
        }
#if 0
        tag_start = vifcodeptr;
        sceDmaAddCont( &vifcodeptr, 0);
        tag_start->p[0] = 0x14000000;                  // mscal ssyrk
        tag_start->p[1] = 0x10000000;                  // flushe (wait for it to finish)
        tag_start->mark = (char)0;
#endif
        num_quadwords=(&chol_dma_end-&chol_dma_start)/4;
        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, num_quadwords, (void *)(0x80000000|SPRADDR_CHOL));
        tag_start->p[0] = 0x14000000;                  // mscal ssyrk;
        tag_start->p[1] = 0x10000000;                  // flushe (wait for it to finish)
        tag_start->mark = (char)0;

    }
    else
    {
#if 0
        tag_start = vifcodeptr;
        sceDmaAddCont( &vifcodeptr, 0);
        tag_start->p[0] = 0x00000000;
        tag_start->p[1] = 0x00000000;
        tag_start->mark = (char)0;
#endif
        int num_quadwords=(&chol_dma_end-&chol_dma_start)/4;
        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, num_quadwords, (void *)(0x80000000|SPRADDR_CHOL));
        tag_start->p[0] = 0x00000000;                  
        tag_start->p[1] = 0x00000000;                  
        tag_start->mark = (char)0;

    }
#if 0
    int num_quadwords=(&chol_dma_end-&chol_dma_start)/4;
    tag_start = vifcodeptr;
    sceDmaAddRef( &vifcodeptr, num_quadwords, (void *)(0x80000000|SPRADDR_CHOL));
    tag_start->p[0] = 0;
    tag_start->p[1] = 0;
    tag_start->mark = (char)0;
#endif
    //**** Call chol then wait

    tag_start = vifcodeptr;
    sceDmaAddEnd( &vifcodeptr, 0,0);
    tag_start->p[0] = 0x14000000;                  // mscal ssyrk
    tag_start->p[1] = 0x10000000;                  // flushe (wait for it to finish)
    tag_start->mark = (char)0;

    //*** Set vi1 to 0 to tell double bufferer to read Abuffer0 on the first iteration
}

/* execute_vif_chain
 * -----------------
 *
 * Execute the vif chain starting at the base of the scratchpad and wait for it to finish
*/

void execute_vif_chain()
{

    //*** Make sure dma actually finished

    dma_safety_net_vif0("before execute vif");

    //*** sceDmaSend(chSprToVU0, (void *)0x80000000);

    DPUT_D_PCR        (D_PCR_CPCVIF0_M);
    DPUT_D_STAT       (D_STAT_CISVIF0_M);
    CPUSYNC

    DPUT_CHTOVIF0_TADR((u_int)0x80000000);
    DPUT_CHTOVIF0_QWC (0);

    CPUSYNC
    DPUT_CHTOVIF0_CHCR(D_CHCR_START | D_CHCR_TAG_TRANSFER_ENABLE | D_CHCR_MODE_CHAIN | D_CHCR_DIRECTION_FROMMEM);

    BC0FWAIT;

    //*** Make sure dma actually finished

    dma_safety_net_vif0("after execute vif");

    //*** Wait for vif0 to finish
    while(DGET_VIF0_STAT()& (VIF0_STAT_FIFO_NONEMPTY | VIF0_STAT_VIF_PIPELINE_ACTIVE));

    //*** Wait for vu0 to finish

    __asm__ __volatile__("vnop");

#if 0
    register u_int vpu_stat;
    __asm__ __volatile__("cfc2 %0, $vi29" :"=r"(vpu_stat):);
    while(vpu_stat & VPU_STAT_VU0_EXECUTING)
    {
        __asm__ __volatile__("cfc2 %0, $vi29" :"=r"(vpu_stat):);
    }
#endif

}
// make_fold_1212sgemm_thenstrsm_vifchain
// --------------------------------------
//
// sgemm :: (bkmat,bkmat,bkmat) -> mkmat
// (fold_1212sgemm xs S) implements (fold sgemm S xs)
// the concrete representation of xs is defined by the following abstraction function, af
// af xs 0 = []
// af (l:r:xs) n = (l,r):af xs (n-1)
//
// Note: On entry, the diagonal block we will strsm with is already in the vumem
//
// Vif program constructed is:
// Copy mem[S..S+36) to buffer S
// Copy mem[xs[1]..xs[1]+36) to buffer ABUFFER0
// Copy mem[xs[0]..xs[0]+36) to buffer BBUFFER0
//
// Copy mem[xs[2*1+1]..xs[2*1+1]+36) to buffer ABUFFER1 || call sgemm
// Copy mem[xs[2*1+0]..xs[2*1+0]+36) to buffer BBUFFER1
// Copy mem[xs[2*2+1]..xs[2*2+1]+36) to buffer ABUFFER0 || call sgemm
// Copy mem[xs[2*2+0]..xs[2*2+0]+36) to buffer BBUFFER0
//        :                  :               :
// Copy mem[xs[2*len_xs-1+1]..xs[2*len_xs-1+1]+36) to the correct buffer || call sgemm
// Copy mem[xs[2*len_xs-1+0]..xs[2*len_xs-1+0]+36) to the correct buffer
//
// Call sgemm
// Wait for it to finish
//
// Call strsm
// Wait for it to finish



void make_fold_1212sgemm_thenstrsm_vifchain(float** xs, int len_xs,float* S)
{
    int i;
    sceDmaTag *vifcodeptr=(sceDmaTag*)(SPRBASE);
    sceDmaTag *tag_start;

    MEASSERT((2+2*len_xs)*16<=16384); //** assert that dma packet will fit into scratchpad

    #define VUMEMADDR_ABUFFER0 0
    #define VUMEMADDR_BBUFFER0 36
    #define VUMEMADDR_ABUFFER1 72
    #define VUMEMADDR_BBUFFER1 108
    #define VUMEMADDR_SX       144

    tag_start = vifcodeptr;
    sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(S));
    tag_start->p[0] = 0;                                                   // NOP
    tag_start->p[1] = SCE_VIF0_SET_UNPACK(VUMEMADDR_SX, 36,0xc, 0);         // tell vif to unpack 36 quadwords to S
    tag_start->mark = (char)0;

    //MEASSERT(len_xs>0);

    if(len_xs>0)
    {
    int current_buffer=VUMEMADDR_ABUFFER0;
    tag_start = vifcodeptr;
    sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(xs[1]));
    tag_start->p[0] = 0;                                                   // NOP
    tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);      // tell vif to unpack 36 quadwords to ABUFFER0
    tag_start->mark = (char)0;

    tag_start = vifcodeptr;
    sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(xs[0]));
    tag_start->p[0] = 0;                                                   // NOP
    tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer+36, 36,0xc, 0);      // tell vif to unpack 36 quadwords to BBUFFER0
    tag_start->mark = (char)0;

    current_buffer=VUMEMADDR_ABUFFER1-current_buffer;

    for(i=1;i!=len_xs;i++)
    {
        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(xs[2*i+1]));
        tag_start->p[0] = 0x14000000;                                         // mscal sgemm
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);     // tell vif to unpack 36 quadwords to current ABUFFER
        tag_start->mark = (char)0;

        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(xs[2*i+0]));
        tag_start->p[0] = 0;                                                  // nop
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer+36, 36,0xc, 0);  // tell vif to unpack 36 quadwords to current BBUFFER
        tag_start->mark = (char)0;
        current_buffer=VUMEMADDR_ABUFFER1-current_buffer;
    }
    tag_start = vifcodeptr;
    sceDmaAddCont( &vifcodeptr, 0);
    tag_start->p[0] = 0x14000000;                  // mscal sgemm
    tag_start->p[1] = 0x10000000;                  // flushe (wait for it to finish)
    tag_start->mark = (char)0;

    tag_start = vifcodeptr;
    sceDmaAddEnd( &vifcodeptr, 0,0);
    tag_start->p[0] = 0x14000000|(int)(&MicrocodeBkMat1212_strsm)/8;     // mscal strsm
    tag_start->p[1] = 0x10000000;                  // flushe (wait for it to finish)
    tag_start->mark = (char)0;

    }
    else
    {
    tag_start = vifcodeptr;
    sceDmaAddEnd( &vifcodeptr, 0, 0);
    tag_start->p[0] = 0x14000000|(int)(&MicrocodeBkMat1212_strsm)/8;        // mscal strsm
    tag_start->p[1] = 0x10000000;                  // flushe (wait for it to finish)
    tag_start->mark = (char)0;
    }
}

/*
 * allocate
 * --------
 * OnEntry:
 * n        - the number of rows excluding padding (so is not necessarily a multiple of 4 or 12)
 *
 * On Exit:
 *
*/

void keaMatrix_ps2sparse :: allocate(int n)
{
    int i;

    int ceil12_n       = MeMathCEIL12(n);
    int num_12_blocks  = ceil12_n / 12;

    this->m_numRows      = n;
    this->num_12_blocks  = num_12_blocks; // current number of rows and cols in the array
    this->ld             = num_12_blocks; // leading dimension of the array
    this->m_padded       = MeMathCEIL4(n);

    /* This matrix is either a factorisable, solveable matrix or a 
     * factorisable, solvable, LCP solveable matrix
     * If it is the latter, then extra data is needed, so allocate it here
    */

#if PRINT_MATRIX_TYPE
    printf("sparse\n");
#endif

    matrix         = (MeReal *) keaPoolAlloc(MeMathCEIL64(ceil12_n*ceil12_n             * sizeof(MeReal))  + 64,"matrix"           );
    matrixChol     = (MeReal *) keaPoolAlloc(MeMathCEIL64(ceil12_n*ceil12_n             * sizeof(MeReal))  + 64,"matrixChol"       );
    rlist          = (int *)    keaPoolAlloc(MeMathCEIL64(num_12_blocks*num_12_blocks   * sizeof(int)   )  + 64,"rlist"            );
    llist          = (int *)    keaPoolAlloc(MeMathCEIL64(num_12_blocks*num_12_blocks   * sizeof(int)   )  + 64,"llist"            );
    llist_len      = (int *)    keaPoolAlloc(MeMathCEIL64(num_12_blocks                 * sizeof(int)   )  + 64,"llist_len"        );
    rlist_len      = (int *)    keaPoolAlloc(MeMathCEIL64(num_12_blocks                 * sizeof(int)   )  + 64,"rlist_len"        );
    zeroBlock      = (MeReal *) keaPoolAlloc(MeMathCEIL64(12*12                         * sizeof(MeReal))  + 64,"zeroblock"        );
    nzPairs        = (MeReal **)keaPoolAlloc(MeMathCEIL64(2*num_12_blocks               * sizeof(MeReal*)) + 64,"nzPairs"          );
    rlist_len_copy = (int *)    keaPoolAlloc(MeMathCEIL64(num_12_blocks                 * sizeof(int)   )  + 64,"llist_len_copy"   );

    /*
     * Align pointers to 64 byte boundaries to make dma quicker and ensure 
     * cache flush/invalidate works properly
     */
    
    matrix         = (MeReal *) MeMemory64ALIGN(matrix);
    matrixChol     = (MeReal *) MeMemory64ALIGN(matrixChol);
    rlist          = (int *)    MeMemory64ALIGN(rlist);
    llist          = (int *)    MeMemory64ALIGN(llist);
    rlist_len      = (int *)    MeMemory64ALIGN(rlist_len);
    llist_len      = (int *)    MeMemory64ALIGN(llist_len);
    zeroBlock      = (MeReal *) MeMemory64ALIGN(zeroBlock);
    nzPairs        = (MeReal **)MeMemory64ALIGN(nzPairs);
    rlist_len_copy = (int *)    MeMemory64ALIGN(rlist_len_copy);

    matrix         = (MeReal *) UNCACHED(matrix);
    matrixChol     = (MeReal *) UNCACHED(matrixChol);
    rlist          = (int *)    UNCACHED(rlist);
    llist          = (int *)    UNCACHED(llist);
    rlist_len      = (int *)    UNCACHED(rlist_len);
    llist_len      = (int *)    UNCACHED(llist_len);
    zeroBlock      = (MeReal *) UNCACHED(zeroBlock);
    nzPairs        = (MeReal **)UNCACHED(nzPairs);
    rlist_len_copy = (int *)    UNCACHED(rlist_len_copy);

    //** Left and right lists should initially be empty

    for(i=0; i!=num_12_blocks; i++)
    {
        llist_len[i] = 0;
        rlist_len[i] = 0;
    }
}

/*
 * blockMultiply
 * -------------
 *
 * Y := Y + A' * X
*/
void blockMultiply(const float *A, const float *X, float *Y)
{
    const int Aqwc=36;

    sceDmaTag* links=(sceDmaTag*)(SPRBASE);
    memset(links,0x0,4*sizeof(sceDmaTag));

    sceDmaTag* chain=links;

    // data
    chain->id=REF_ID;
    chain->next=(sceDmaTag*)(DMAADDR(A));
    chain->qwc=Aqwc;

    chain->p[0]=SCE_VIF0_SET_ITOP(0,0);
    chain->p[1]=SCE_VIF0_SET_UNPACK(0,Aqwc,0xc,0);

    chain++;

    // execute the code
    chain->id=END_ID;
    chain->qwc=0;
    chain->p[0]=SCE_VIF0_SET_MSCAL((int)(&trmuladd)/8,0);
    chain->p[1]=SCE_VIF0_SET_FLUSHE(0);

    chain++;

    // set register params
    __asm__ __volatile__("
    lqc2  vf1, 0x0(%0)
    lqc2  vf2, 0x10(%0)
    lqc2  vf3, 0x20(%0)

    lqc2  vf4, 0x0(%1)
    lqc2  vf5, 0x10(%1)
    lqc2  vf6, 0x20(%1)

    sync.l
    sync.p

    " : : "r" (&Y[0]), "r" (&X[0]));

    execute_vif_chain();

    // store return values
    __asm__ __volatile__("
    sqc2  vf1, 0x0(%0)
    sqc2  vf2, 0x10(%0)
    sqc2  vf3, 0x20(%0)
    " : : "r" (&Y[0]));
}

/*
 *
*/
void send_microcode_1212sgemv()
{
    dma_safety_net_vif0("before send sgemv to vif0");

    //*** sceDmaSendN(chSprToVU0, );

    DPUT_D_PCR        (D_PCR_CPCVIF0_M);
    DPUT_D_STAT       (D_STAT_CISVIF0_M);
    CPUSYNC

    DPUT_CHTOVIF0_MADR((u_int)(&sgemv_dma_start));
    DPUT_CHTOVIF0_QWC((&sgemv_dma_end-&sgemv_dma_start)/4);

    CPUSYNC
    DPUT_CHTOVIF0_CHCR(D_CHCR_START | D_CHCR_TAG_TRANSFER_ENABLE | D_CHCR_MODE_NORMAL | D_CHCR_DIRECTION_FROMMEM);

    BC0FWAIT;

    dma_safety_net_vif0("after send sgemm strsm to vif0");
}

/* foldsgemvNoTrans
 * ----------------
 * On Entry:
 *
 * ansvec
 * arow
 * srcvec
 * list
 * list_len
 *
 * On Exit:
 * ansvec
*/

#define MICROCODE_SGEMV

void foldsgemvNoTransThenSolve(
         MeReal       *ansvec,   /* Output / Input */
         const MeReal *arow,     /* Input */
         const MeReal *srcvec,   /* Input */
         const int    *list,     /* Input */
         int           list_len) /* Input */
{
#if 1
    #define VUMEMADDR_MAT0    0
    #define VUMEMADDR_VEC0   36
    #define VUMEMADDR_MAT1   39
    #define VUMEMADDR_VEC1   75
    #define VUMEMADDR_ANSVEC 78

    sceDmaTag *vifcodeptr = (sceDmaTag*)(SPRBASE);
    sceDmaTag *tag_start;
    int i;
    const MeReal *parow = arow;

    if(list_len>0)
    {
#if 1
        int current_buffer = VUMEMADDR_MAT0;

        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(parow));
        tag_start->p[0] = 0;                                                   // NOP
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);      // tell vif to unpack 36 quadwords to MAT0
        tag_start->mark = (char)0;

        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 3, (void*)DMAADDR(srcvec+list[0]*12) );
        tag_start->p[0] = 0;                                                   // NOP
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer + 36, 3,0xc, 0);  // tell vif to unpack 3 quadwords to VEC0
        tag_start->mark = (char)0;

        current_buffer = VUMEMADDR_MAT1 - current_buffer;
        parow          = parow + 144;

        for(i=1; i!=list_len; i++)
        {
            tag_start = vifcodeptr;
            sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(parow));
            tag_start->p[0] = 0x14000000|(int)(&sgemvSubNoTrans)/8;         // mscal sgemv
            tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);     // tell vif to unpack 36 quadwords to current MAT buffer
            tag_start->mark = (char)0;

            tag_start = vifcodeptr;
            sceDmaAddRef( &vifcodeptr, 3, (void*)DMAADDR(srcvec+list[i]*12));
            tag_start->p[0] = 0;                                                  // nop
            tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer+36, 3,0xc, 0);   // tell vif to unpack 3 quadwords to current VEC buffer
            tag_start->mark = (char)0;
        
            current_buffer = VUMEMADDR_MAT1-current_buffer;
            parow          = parow + 144;
        }
#endif

        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(parow));
        tag_start->p[0] = 0x14000000|(int)(&sgemvSubNoTrans)/8;      // mscal sgemv
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);    //
        tag_start->mark = (char)0;

        //**** Call solve then wait

        tag_start = vifcodeptr;
        sceDmaAddEnd( &vifcodeptr, 0,0);
        tag_start->p[0] = 0x14000000|(int)(&strsv)/8;              // mscal strsv
        tag_start->p[1] = 0x10000000;                  // flushe (wait for it to finish)
        tag_start->mark = (char)0;
    }
    else
    {
        //**** Send diagonal block to vumem

        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(parow));
        tag_start->p[0] = 0;                                       // nop
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(VUMEMADDR_MAT0, 36,0xc, 0);  //
        tag_start->mark = (char)0;

        //**** Call solve then wait

        tag_start = vifcodeptr;
        sceDmaAddEnd( &vifcodeptr, 0,0);
        tag_start->p[0] = 0x14000000|(int)(&strsv)/8;        // mscal strsv
        tag_start->p[1] = 0x10000000;                  // flushe (wait for it to finish)
        tag_start->mark = (char)0;
    }

    /* Initialise microcode's vector accumulator with ansvec */

    __asm__ __volatile__("
        lqc2 vf1,0x00(%0)
        lqc2 vf2,0x10(%0)
        lqc2 vf3,0x20(%0)
    " : : "r" (&ansvec[0]));

    //*** Set vi1 to 0 to tell double bufferer to read Abuffer0 on the first iteration
    asm ("ctc2.i $0,$vi01;sync.l;sync.p");

    execute_vif_chain();

    __asm__ __volatile__("
        sqc2 vf1,0x00(%0)
        sqc2 vf2,0x10(%0)
        sqc2 vf3,0x20(%0)
    " : : "r" (&ansvec[0]));

    #undef VUMEMADDR_MAT0
    #undef VUMEMADDR_VEC0
    #undef VUMEMADDR_MAT1
    #undef VUMEMADDR_VEC1
    #undef VUMEMADDR_ANSVEC

#endif
}

MeReal temp[12] __attribute__ ((aligned(16)));

void solveThenfoldsgemvTrans(MeReal       *ansvec,   /* Output / Input */
                             MeReal       *srcvec,   /* Output / Input */
                             const MeReal *arow,     /* Input */
                             const int    *list,     /* Input */
                             int           list_len) /* Input */
{
    int j;

    #define VUMEMADDR_SGEMVTRANS_MAT0     0
    #define VUMEMADDR_SGEMVTRANS_MAT1    36
    #define VUMEMADDR_SGEMVTRANS_ANSVECS 72

    sceDmaTag *vifcodeptr = (sceDmaTag*)(SPRBASE);
    sceDmaTag *tag_start;
    const MeReal *parow = arow + list_len*12*12;

    if(list_len!=0)
    {
        int current_buffer = VUMEMADDR_SGEMVTRANS_MAT0;

        /* Do initial solve */
        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(parow));
        tag_start->p[0] = 0;                                                              // NOP
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);      // tell vif to unpack 36 quadwords to MAT0
        tag_start->mark = (char)0;
        current_buffer = VUMEMADDR_SGEMVTRANS_MAT1 - current_buffer;
        parow          = parow - 144;

        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(parow));
        tag_start->p[0] = 0x14000000|(int)(&strsvTrans)/8;       // mscal sgemvTrans
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);      // tell vif to unpack 36 quadwords to MAT0
        tag_start->mark = (char)0;

        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 3, (void*)DMAADDR(ansvec+list[list_len-1]*12) );
        tag_start->p[0] = 0;                                                                             // NOP
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(VUMEMADDR_SGEMVTRANS_ANSVECS + (list_len-1)*3, 3,0xc, 0);  // tell vif to unpack 3 quadwords to VEC0
        tag_start->mark = (char)0;

        current_buffer = VUMEMADDR_SGEMVTRANS_MAT1 - current_buffer;
        parow          = parow - 144;
        j              = list_len - 1;

        while(j!=0)
        {
            j         = j - 1;
            tag_start = vifcodeptr;
            sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(parow));
            tag_start->p[0] = 0x14000000|(int)(&sgemvSubTrans)/8;        // mscal sgemvTrans
            tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);     // tell vif to unpack 36 quadwords to current MAT buffer
            tag_start->mark = (char)0;

            tag_start = vifcodeptr;
            sceDmaAddRef( &vifcodeptr, 3, (void*)DMAADDR(ansvec+list[j]*12));
            tag_start->p[0] = 0;                                                                   // nop
            tag_start->p[1] = SCE_VIF0_SET_UNPACK(VUMEMADDR_SGEMVTRANS_ANSVECS + j*3, 3,0xc, 0);   // tell vif to unpack 3 quadwords to current VEC buffer
            tag_start->mark = (char)0;

            current_buffer = VUMEMADDR_SGEMVTRANS_MAT1-current_buffer;
            parow          = parow - 144;
        }

        tag_start = vifcodeptr;
        sceDmaAddEnd( &vifcodeptr, 0, 0);
        tag_start->p[0] = 0x14000000|(int)(&sgemvSubTrans)/8;        // mscal sgemvTrans
        tag_start->p[1] = 0x10000000;                          // flushe (wait for it to finish)
        tag_start->mark = (char)0;
    }
    else
    {
        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(parow));
        tag_start->p[0] = 0;                                                              // NOP
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(VUMEMADDR_SGEMVTRANS_MAT0, 36,0xc, 0);      // tell vif to unpack 36 quadwords to MAT0
        tag_start->mark = (char)0;

        tag_start = vifcodeptr;
        sceDmaAddEnd( &vifcodeptr, 0, 0);
        tag_start->p[0] = 0x14000000|(int)(&strsvTrans)/8;       // mscal sgemvTrans
        tag_start->p[1] = 0x10000000;                  // flushe (wait for it to finish)
        tag_start->mark = (char)0;

    }
    /* Initialise microcode's copy of X */

    __asm__ __volatile__("
        lqc2 vf1,0x00(%0)
        lqc2 vf2,0x10(%0)
        lqc2 vf3,0x20(%0)
    " : : "r" (&srcvec[0]));

    //*** Set vi1 to 0 to tell double bufferer to read Abuffer0 on the first iteration
    //*** Point vi1 to the place to put the first answer vec
    __asm__ __volatile__ ("
        ctc2.i $0,$vi01
        ctc2.i %0,$vi02
        sync.l
        sync.p
    " : : "r" (VUMEMADDR_SGEMVTRANS_ANSVECS+(list_len-1)*3)); //

    execute_vif_chain();

    //*** Copy back the modified srcvec

    __asm__ __volatile__("
        sqc2 vf1,0x00(%0)
        sqc2 vf2,0x10(%0)
        sqc2 vf3,0x20(%0)
    " : : "r" (&srcvec[0]));

    /* Copy back the results */

    MeReal *pvumemans = ((MeReal *)VUMEM0)+VUMEMADDR_SGEMVTRANS_ANSVECS*4;
    for(j=0;j!=list_len;j++)
    {
        __asm__ __volatile__("
            lqc2 vf1,0x00(%1)
            lqc2 vf2,0x10(%1)
            lqc2 vf3,0x20(%1)

            sqc2 vf1,0x00(%0)
            sqc2 vf2,0x10(%0)
            sqc2 vf3,0x20(%0)
        " : : "r" (&ansvec[list[j]*12]), "r" (&pvumemans[0]));

        pvumemans = pvumemans + 12;
    }
}

/*
 *  solve
 *  -----
 *
 *  x:[A.x=rhs]
 *
 *  n is ceil12(num_rows)
 *
 *  On Entry:
 *
 *  x             - empty array to put the answer in
 *  rhs           - right hand side vector
 *  llist_len     - 
 *  llist         - 
 *  num_12_blocks - 
 *  matrixChol    - 
 *  ld            - 
 *
 *  For row i, the non-zero blocks are stored in a left aligned list
 *  The number of blocks in the list is given by llist_len[i]
 *  llist
 *
 *  Cache status:
 *
 *  b is 64 byte aligned, a multiple of 64 bytes long and has the uncached bit set
 *
*/
void keaMatrix_ps2sparse :: solve(
         MeReal       x[],
         const MeReal rhs[])
{
    int i;

#if PRINT_SOLVE_INPUT
    printSolveInput(rhs,m_padded/4);
#endif

    /* rhs may not be 64 aligned if this partition is not the first,
       but as data is copied to x, all that matters is that x is aligned */
    CHECKALIGNEDANDUNCACHED(x);

    MdtKeaProfileStart("solve");

    /* 
       If dest and source dont point to the same place, 
       then copy source to dest, so we can solve in-place
    */

    if(x!=rhs)
    {
        for(i=0;i!=num_12_blocks*12;i++)
        {
            x[i]=rhs[i];
        }
    }

    

    //**** First, solve for G*y = b and overwrite b

    float *arow = matrixChol;
    for(i=0; i!=num_12_blocks; ++i)
    {
        MEASSERT(llist_len[i]>0);
        MEASSERT(llist[ld*i + llist_len[i]-1]==i);

        foldsgemvNoTransThenSolve(
            x + i * 12,             /* Output / input */
            arow,                   /* Input */
            x,                      /* Input */
            llist+ld*i,             /* Input */
            llist_len[i]-1);        /* Input */

        arow = arow + (i+1) * 12*12;
    }

    //**** Next, solve G'*x = y and overwrite the result.

    arow = matrixChol + (num_12_blocks*(num_12_blocks-1)/2) * 12*12;
    i    = num_12_blocks;
    while(i!=0)
    {
        i = i - 1;
        solveThenfoldsgemvTrans(
            x , 
            x + i*12, 
            arow, 
            llist+ld*i, 
            llist_len[i]-1);

        arow = arow - i * 12*12;
    }

    MdtKeaProfileEnd("solve");

    //DisableCache(DATA_CACHE);
    //EnableCache(DATA_CACHE);

    /* Zero pad up to next 12 boundary */
    for(i=m_numRows;i!=num_12_blocks*12;i++)
    {
        x[i]=0.0f;
    }

#if PRINT_SOLVE_OUTPUT
    printSolvePCSparseSSEOutput(x,
                                m_padded);
#endif

}

/* keaLCPMatrix_ps2sparse :: solveUnit
 * -----------------------------------
 *
 * find b st A*b=makeunit(posn)
 * where:
 *       makeunit(posn) is a vector of length n which is zero execpt for element posn, which is 1
 *
 * On Entry:
 *
 * Cache status:
 * b is a pointer that lies within an array which is 64 byte aligned and a multiple of 64 bytes long
 * however, b itself may not be 64 byte aligned
 * it is 16 byte aligned though, so we can dma it
 * The array b lies within is this->Ainv, which is uncached
*/
void keaMatrix_ps2sparse :: solveUnits(
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

/* foldsgemvNoTrans
 * ----------------
 *
 * On Entry
*/
void foldsgemvNoTrans(MeReal       *ansvec,    /* Output */
                      const MeReal *srcvec,     /* Input */
                      const MeReal *arow,       /* Input */
                      const int    *list,       /* Input */
                      int          list_len     /* Input */
                     )
{
    #define VUMEMADDR_FOLDSGEMV_MAT0    0
    #define VUMEMADDR_FOLDSGEMV_VEC0   36
    #define VUMEMADDR_FOLDSGEMV_MAT1   39
    #define VUMEMADDR_FOLDSGEMV_VEC1   75

    sceDmaTag *vifcodeptr = (sceDmaTag*)(SPRBASE);
    sceDmaTag *tag_start;
    int i;
    const MeReal *parow = arow;

    if(list_len>0)
    {
        int current_buffer = VUMEMADDR_FOLDSGEMV_MAT0;

        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(parow));
        tag_start->p[0] = 0;                                                   // NOP
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);      // tell vif to unpack 36 quadwords to MAT0
        tag_start->mark = (char)0;

        tag_start = vifcodeptr;
        sceDmaAddRef( &vifcodeptr, 3, (void*)DMAADDR(srcvec+list[0]*12) );
        tag_start->p[0] = 0;                                                   // NOP
        tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer + 36, 3,0xc, 0);  // tell vif to unpack 3 quadwords to VEC0
        tag_start->mark = (char)0;

        current_buffer = VUMEMADDR_FOLDSGEMV_MAT1 - current_buffer;
        parow          = parow - 144;

        for(i=1; i!=list_len; i++)
        {
            tag_start = vifcodeptr;
            sceDmaAddRef( &vifcodeptr, 36, (void*)DMAADDR(parow));
            tag_start->p[0] = 0x14000000|(int)(&sgemvSubNoTrans)/8;         // mscal sgemv
            tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer, 36,0xc, 0);     // tell vif to unpack 36 quadwords to current MAT buffer
            tag_start->mark = (char)0;

            tag_start = vifcodeptr;
            sceDmaAddRef( &vifcodeptr, 3, (void*)DMAADDR(srcvec+list[i]*12));
            tag_start->p[0] = 0;                                                  // nop
            tag_start->p[1] = SCE_VIF0_SET_UNPACK(current_buffer+36, 3,0xc, 0);   // tell vif to unpack 3 quadwords to current VEC buffer
            tag_start->mark = (char)0;

            current_buffer = VUMEMADDR_FOLDSGEMV_MAT1-current_buffer;
            parow          = parow - 144;
        }

        tag_start = vifcodeptr;
        sceDmaAddEnd( &vifcodeptr, 0,0);
        tag_start->p[0] = 0x14000000|(int)(&sgemvSubNoTrans)/8;      // mscal sgemv
        tag_start->p[1] = 0x10000000;
        tag_start->mark = (char)0;

        /* Initialise microcode's vector accumulator with ansvec */

        __asm__ __volatile__("
            vsub.xyzw vf1,vf0,vf0
            vsub.xyzw vf2,vf0,vf0
            vsub.xyzw vf3,vf0,vf0
        ");

        //*** Set vi1 to 0 to tell double bufferer to read Abuffer0 on the first iteration
        asm ("ctc2.i $0,$vi01;sync.l;sync.p");

        execute_vif_chain();

        __asm__ __volatile__("
            vsub.xyzw vf4,vf0,vf0
            vsub.xyzw vf1,vf4,vf1
            vsub.xyzw vf2,vf4,vf2
            vsub.xyzw vf3,vf4,vf3
            sqc2 vf1,0x00(%0)
            sqc2 vf2,0x10(%0)
            sqc2 vf3,0x20(%0)
        " : : "r" (&ansvec[0]));
    }

    #undef VUMEMADDR_FOLDSGEMV_MAT0
    #undef VUMEMADDR_FOLDSGEMV_VEC0
    #undef VUMEMADDR_FOLDSGEMV_MAT1
    #undef VUMEMADDR_FOLDSGEMV_VEC1
}

/*
 * keaLCPMatrix_ps2sparse :: multiply
 * ----------------------------------
 *
 * B := this->A*X
*/
void keaMatrix_ps2sparse :: multiply(
         MeReal       b[],      /* Output */
         const MeReal x[])      /* Input  */
{
    int srcvector, destvector, matrixrow, matrixelt, i, j;
    MeReal *arow;
    MeReal *arow2;

    for(i=0;i!=num_12_blocks;i++)
    {
        rlist_len_copy[i] = this->rlist_len[i];
    }

    arow = this->matrix;

    for(i=0;i!=num_12_blocks;i++)
    {
        //** Do stuff before diagonal by going across the matrix
        
        foldsgemvNoTrans(b+i*12, x, arow + i*12*12, rlist + i*ld, rlist_len[i]);

        //** Do stuff after the diagonal by going down the matrix

        arow2 = arow + (i+1)*12*12;
        for(j=i+1;j!=num_12_blocks;j++)
        {
             if(rlist[j*ld + rlist_len_copy[j] - 1]==i)
             {
                 srcvector  = j;
                 destvector = i;
                 matrixrow  = j;
                 matrixelt  = j+1-rlist_len_copy[j];

                
                 blockMultiply(arow2 + matrixelt*12*12, x + srcvector*12, b + destvector*12);

                 rlist_len_copy[j] = rlist_len_copy[j] - 1;
             }
             arow2 = arow2 + (j+1)*12*12;
        }

        arow = arow + (i+1)*12*12;
    }

    /* Zero pad up to next 12 boundary */
    for(i=m_numRows;i!=num_12_blocks*12;i++)
    {
        b[i]=0.0f;
    }
}

static sceDmaTag dmachain[4] __attribute__ ((aligned(64))) = {
    // {short qwc,char padding,char id,void *addr,int pad0,int pad1}
    // (each tag is 1 quadword (16bytes))
    {6,0,0x30,0,{0,0}},
    {6,0,0x30,0,{0,0}},
    {0,0,0x30,0,{0,0}},
    {0,0,0x00,0,{0,0}}
    };
#define UNCACHED_SCEDMATAG(p) ((sceDmaTag *)((unsigned int)p | 0x30000000))

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


/* calcJinvMJT
 * -----------
 *
 * On Entry:
 *
 * JM         -
 * j          -
 * jlen       -
 * jbltobody  -
 * slipfactor -
 * parameters -
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
 * Cache status
 *
 * this->matrix
 * this->matrixChol
 * this->rlist
 * this->rlist_len
 * JM
 * J
 * jlen
 * jbltobody
 * slipfactor
*/
void keaMatrix_ps2sparse :: makeFromJMJT(
         const MeReal *  JM,          /* Input */
         const MeReal *  j,           /* Input */
         const int *     jlen,        /* Input */
         const int *     jbltobody,   /* Input */
         const MeReal *  slipfactor,  /* Input */
         const MeReal    epsilon,     /* Input */
         const MeReal    hinv)        /* Input */
{
#ifndef _NOTIC
    MdtKeaProfileStart("ps2sparse calcJinvMJT");
#endif

    CHECKALIGNEDANDUNCACHED(this -> matrix);
    CHECKALIGNEDANDUNCACHED(this -> matrixChol);
    CHECKALIGNEDANDUNCACHED(this -> rlist);
    CHECKALIGNEDANDUNCACHED(this -> rlist_len);

    int    num_12_blocks = this -> num_12_blocks;
    MeReal *A            = this -> matrix;
    MeReal *matrixChol   = this -> matrixChol;
    int    *rlist        = this -> rlist;
    int    *rlist_len    = this -> rlist_len;

    MeReal *Abase = A;
    MeReal *Apipeline0;
    MeReal *Apipeline1;
    int *rlistpipeline0;
    int *rlistpipeline1;
    int *rlist_save = rlist;
    int *rlistlen_save = rlist_len;
    int rowpipeline0;
    int rowpipeline1;
    int *prlist     = rlist;
    int buffer;
    int col;
    int i;

    for(i=0;i!=num_12_blocks;i++)
    {
        rlist_len[i]=0;
    }

#if PRINT_CALCJMINVJT_INPUT
    printCalcJinvMJTInput(ji,jmi,j,JM,num_12_blocks);
#endif

    buffer=0;
    const int *pjlen = jlen;

    dma_safety_net_tospr("");
    dma_safety_net_fromspr("");

    for(col=0;col!=num_12_blocks;col++)
    {
        MeReal *sparseAp                 = A + (col + 1) * 144;            // sparseAp initially points to end of current col.s
        int num_jquads                   = (pjlen[0]+pjlen[1]+pjlen[2])*6;
        int row;

        const MeReal *pjm                = JM;

        //*** Point jm to end of column

        for(row=0;row!=col+1;row++)
        {
            pjm     = pjm + (jlen[row*3+0]+jlen[row*3+1]+jlen[row*3+2])* 6 * 4;
        }

        for(row=col; row!=-1; --row)
        {
            //*** Setup tospr dma chain
            {
            pjm     = pjm - (jlen[row*3+0]+jlen[row*3+1]+jlen[row*3+2])* 6 * 4;

            Calc_block_buf *loadbuf = (Calc_block_buf *)( ((u_int)SPR) + buffer*BUFFERLENGTH_BYTES );
            loadbuf->shape          = SQUARE;
            loadbuf->gap_for_tag    = 0;

            loadbuf->jm_len[0]   = jlen[row*3+0];
            loadbuf->jm_len[1]   = jlen[row*3+1];
            loadbuf->jm_len[2]   = jlen[row*3+2];
            loadbuf->j_len[0]    = pjlen[0];
            loadbuf->j_len[1]    = pjlen[1];
            loadbuf->j_len[2]    = pjlen[2];
            loadbuf->modify_diag = 0;

            int num_jmquads = (jlen[row*3+0]+jlen[row*3+1]+jlen[row*3+2])*6;

            UNCACHED_SCEDMATAG(dmachain)[0].next=(sceDmaTag *)DMAADDR(jbltobody+row*24);
            UNCACHED_SCEDMATAG(dmachain)[1].next=(sceDmaTag *)DMAADDR(jbltobody+col*24);
            UNCACHED_SCEDMATAG(dmachain)[2].next=(sceDmaTag *)DMAADDR(pjm);
            UNCACHED_SCEDMATAG(dmachain)[2].qwc =             num_jmquads;
            UNCACHED_SCEDMATAG(dmachain)[3].next=(sceDmaTag *)DMAADDR(j);
            UNCACHED_SCEDMATAG(dmachain)[3].qwc =             num_jquads;

            }

            //tic("Dma kick");

            if( col!=0 && (col!=1 || row!=1) )
            {
                //printf("%d ",((Calc_block_buf *)(SPR_MEREAL+buffer*BUFFERLENGTH_WORDS))->isnonzero);
                if(((Calc_block_buf *)(SPR_MEREAL+buffer*BUFFERLENGTH_WORDS))->isnonzero==1)
                {
                    //*** Save processed data in scratchpad back to main memory
                    //*** -----------------------------------------------------
                    //*** Apipeline1 points to area of main memory to dma to.
                    //***
                    //*** Apipeline1     - pointer to end of the row we are going to dma to
                    //*** buffer         -
                    //*** rlist_save     - pointer to rlist
                    //*** rlistlen_save  - pointer to rlistlen
                    //*** rowpipeline1   - the row number of the block to be saved
                    //*** rlistpipeline1 - rlistpipeline1==0 when the block is not the last block in the row (I think)

                    //** Set up dma controller so that executing asm("bc0f") will wait for fromspr and tospr channels to finish
                    
                    DPUT_D_STAT(D_STAT_CISfromSPR_M|D_STAT_CIStoSPR_M);
                    DPUT_D_PCR(D_PCR_CPCfromSPR_M|D_PCR_CPCtoSPR_M);
                    CPUSYNC

                    DPUT_CHFROMSPR_SADR((u_int)(buffer*BUFFERLENGTH_BYTES+16));
                    DPUT_CHFROMSPR_MADR(DMAADDR((u_int)(Apipeline1 - ((*rlistlen_save) + 1) * 144)));
                    DPUT_CHFROMSPR_QWC (36);
                    CPUSYNC
                    DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_NORMAL | D_CHCR_DIRECTION_FROMMEM);

                    rlist_save[*rlistlen_save] = rowpipeline1;
                    *rlistlen_save             = *rlistlen_save + 1;
                }

                //** If we have reached the end of the row, point rlist and rlistlen to the next row

                if(rlistpipeline1!=0)
                {
                    rlist_save=rlistpipeline1;
                    rlistlen_save++;
                    //printf("\n");
                }

                //*rlistpipeline0 = 1;
            }
            else
            {
                //*** Set up dma controller so that executing asm("bc0f") will wait for just tospr channel to finish
                DPUT_D_STAT(D_STAT_CIStoSPR_M);
                DPUT_D_PCR(D_PCR_CPCtoSPR_M);
                CPUSYNC
            }

            //*** sceDmaSend(chToSpr,(void *)dmachain);

            DPUT_CHTOSPR_SADR(buffer*BUFFERLENGTH_BYTES+BUFFERADDR_INPUT);
            DPUT_CHTOSPR_TADR((u_int)dmachain);
            DPUT_CHTOSPR_QWC (0);
            CPUSYNC
            DPUT_CHTOSPR_CHCR(0x00000105);

            Apipeline1        = Apipeline0;
            Apipeline0        = sparseAp;
            //sparseAp          = sparseAp - 144;
            rlistpipeline1    = rlistpipeline0;
            rlistpipeline0    = (int *)0;
            rowpipeline1      = rowpipeline0;
            rowpipeline0      = row;

            //tic("calc block");

            if((row+col)!=0) calc_block(SPR_MEREAL+(1-buffer)*BUFFERLENGTH_WORDS);

            //tic("dma wait");

            BC0FWAIT;

            dma_safety_net_tospr("");
            dma_safety_net_fromspr("");

            buffer         = 1 - buffer;
        }

        j              = j + num_jquads*4;
        pjlen          = pjlen + 3;
        A              = A + (col+1) * 144;
        prlist         = prlist + num_12_blocks;
        rlistpipeline0 = prlist;
    }

    //*** Epliogue 0

    DPUT_D_STAT(D_STAT_CISfromSPR_M);
    DPUT_D_PCR(D_PCR_CPCfromSPR_M);
    CPUSYNC

    if(num_12_blocks!=1)
    {
        if(((Calc_block_buf *)(SPR_MEREAL+buffer*BUFFERLENGTH_WORDS))->isnonzero==1)
        {
            //*** Save the buffer that was calculated in the main loop but not yet saved

            DPUT_CHFROMSPR_SADR((u_int)(buffer*BUFFERLENGTH_BYTES+16));
            DPUT_CHFROMSPR_MADR(DMAADDR((u_int)(Apipeline1 - ((*rlistlen_save) + 1) * 144)));
            DPUT_CHFROMSPR_QWC (36);
            CPUSYNC
            DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_NORMAL | D_CHCR_DIRECTION_FROMMEM);

            rlist_save[*rlistlen_save] = rowpipeline1;
            *rlistlen_save             = *rlistlen_save + 1;
        }

        if(rlistpipeline1!=0)
        {
            rlist_save=rlistpipeline1;
            rlistlen_save++;
        }

        calc_block(SPR_MEREAL+(1-buffer)*BUFFERLENGTH_WORDS);

        if(((Calc_block_buf *)(SPR_MEREAL+buffer*BUFFERLENGTH_WORDS))->isnonzero==1)
        {
            BC0FWAIT;

            dma_safety_net_fromspr("");
        }
    }
    else
    {
        calc_block(SPR_MEREAL+(1-buffer)*BUFFERLENGTH_WORDS);
    }


    //*** Epilogue 1

    if(((Calc_block_buf *)(SPR_MEREAL+(1-buffer)*BUFFERLENGTH_WORDS))->isnonzero==1)
    {
        //printf("epilogue 1 %08x\n",(u_int)(Apipeline0 - ((*rlistlen_save) + 1) * 144));
        DPUT_D_STAT(D_STAT_CISfromSPR_M);
        DPUT_D_PCR(D_PCR_CPCfromSPR_M);
        CPUSYNC

        DPUT_CHFROMSPR_SADR((u_int)((1-buffer)*BUFFERLENGTH_BYTES+16));
        DPUT_CHFROMSPR_MADR(DMAADDR((u_int)(Apipeline0 - ((*rlistlen_save) + 1) * 144)));
        DPUT_CHFROMSPR_QWC (36);
        CPUSYNC
        DPUT_CHFROMSPR_CHCR(D_CHCR_START | D_CHCR_MODE_NORMAL | D_CHCR_DIRECTION_FROMMEM);

        BC0FWAIT;

        dma_safety_net_fromspr("");

        rlist_save[*rlistlen_save] = rowpipeline0;
        *rlistlen_save             = *rlistlen_save + 1;
    }

    //*** Modify the diagonal (this should go into the scratchpad double buffered code eventually)

    int ii;
    int jj;
    int kk;
    MeReal *pblock = Abase;
    int row2 =0;

    //printf("m_epsilon=%12.6f\n",m_epsilon);

    for(ii=0;ii!=num_12_blocks;ii++)
    {
        for(jj=0;jj!=3;jj++)
        {
            MeReal *p4block = pblock + jj*48 + jj*16;
            for(kk=0;kk!=4;kk++)
            {
                if(row2<MeMathCEIL4(this->m_numRows))
                {
                    p4block[5*kk] += hinv*slipfactor[ii*12+jj*4+kk]+epsilon;
                }
                else
                {
                    p4block[5*kk]=1.0f;
                }
                row2++;
            }
        }
        pblock = pblock + (ii+2) * 12*12;
    }

    /*
      Copy A to matrixChol for use by factoriser. Both are in the sparse format
    */

    MeReal *dest = matrixChol;
    MeReal *src  = Abase;

    for (ii = 0; ii != (num_12_blocks*(num_12_blocks+1)/2)*36; ii++, dest += 4, src += 4)
    {
        __asm__ __volatile__(
            "    lqc2   vf1,0x00(%1)\n"
            "    sqc2   vf1,0x00(%0)\n"
            : : "r" (dest),"r" (src));
    }

#if PRINT_CALCJMINVJT_PS2SPARSE_OUTPUT
    printCalcJinvMJTps2sparse_Output(Abase,rlist,rlist_len,num_12_blocks);
#endif

#ifndef _NOTIC
    MdtKeaProfileEnd("ps2sparse calcJinvMJT");
#endif

}
void keaMatrix_ps2sparse :: makeFromColMajorPSM(
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

    MdtKeaProfileStart("Make Q");

    int rowblock;
    int colblock;
    int row;
    int col;
    int i;
    int j;

    MeReal *dest;

    /* Make the sparsity information
       We assume Q is dense, so rlist should look like (in memory)
       [0,x,x]
       [1,0,x]
       [2,1,0]
        :   :
    */
              
    for(rowblock=0;rowblock!=num_12_blocks;rowblock++)
    {
        for(colblock=0;colblock!=rowblock+1;colblock++)
        {
            rlist[rowblock*num_12_blocks+colblock]=rowblock-colblock;
        }
        rlist_len[rowblock]=rowblock+1;
    }

    /* Make the actual matrix
       The blocks are in the order you would expect in a dense matrix
       ie matrix A
                 B C
                 D E F
       is laid out in memory as A,B,C,D,E,F
    */

    dest = matrixChol;
    for(rowblock=0;rowblock!=num_12_blocks;rowblock++)
    {
        for(colblock=0;colblock!=rowblock+1;colblock++)
        {
            for(row=0;row!=3;row++)
            {
                for(col=0;col!=12;col++)
                {
                    for(i=0;i!=4;i++)
                    {
                        if((rowblock*12+row*4+i)<numClamped && (colblock*12+col)<numClamped)
                        {
                            *(dest++)=Ainv[clamped[rowblock*12+row*4+i]*AinvStride+clamped[colblock*12+col]];
                        }
                        else
                        {
                            *(dest++)=0.0f;
                        }
                    }
                }
            }
        }
    }

    /* Make Qrhs */

    for(i=0; i!=numClamped; i++) 
    {
        j=clamped[i];
        Qrhs[i]=clampedValues[j]-initialSolve[j];
    }
    for(i=numClamped; i!=MeMathCEIL4(numClamped); i++) 
    {
        Qrhs[i]=0.0f;
    }
    
    MdtKeaProfileEnd("Make Q");

#if PRINT_PPT_MAKE_Q_OUTPUT
    printMakeFromColMajorPSMOutput_ps2sparse(
         matrixChol,
         rlist,
         rlist_len,
         num_12_blocks);
#endif

}
