#ifndef __KEAEEDEFS_HPP /* -*- mode: C++; -*- */
#define __KEAEEDEFS_HPP

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.14.2.4 $

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

#include <libdma.h>
#include <devvif0.h>
#include <devvu0.h>
#include <eestruct.h>
#include <MeAssert.h>
#include <eeregs.h>

#define SPR (0x70000000)
#define VUMEM0 (0x11004000)

//*** Some macros for DMA control that have more descriptive names than the sony ones
typedef struct {
  u_int chcr; u_int p0[3];  // channel control
  void  *madr;  u_int p1[3];  // memory address
  u_int qwc;  u_int p2[3];  // transfer count
  void  *tadr;  u_int p3[3];  // tag address
  void  *as0; u_int p4[3];  // address stack
  void  *as1; u_int p5[3];  // address stack
  u_int p6[4];      // pad
  u_int p7[4];      // pad
  u_int sadr; u_int p8[3];  // spr address
} DmaChan;

#define DPUT_CHFROMSPR_SADR DPUT_D8_SADR
#define DPUT_CHFROMSPR_MADR DPUT_D8_MADR
#define DPUT_CHFROMSPR_QWC  DPUT_D8_QWC
#define DPUT_CHFROMSPR_CHCR DPUT_D8_CHCR
#define DGET_CHFROMSPR_CHCR DGET_D8_CHCR

#define DPUT_CHTOSPR_SADR   DPUT_D9_SADR
#define DPUT_CHTOSPR_MADR   DPUT_D9_MADR
#define DPUT_CHTOSPR_TADR   DPUT_D9_TADR
#define DPUT_CHTOSPR_QWC    DPUT_D9_QWC
#define DPUT_CHTOSPR_CHCR   DPUT_D9_CHCR
#define DGET_CHTOSPR_CHCR   DGET_D9_CHCR

#define CHFROMSPR ((DmaChan *)D8_CHCR)
#define CHTOSPR   ((DmaChan *)D9_CHCR)

#define GETCHANFROMSPR ((sceDmaChan *)D8_CHCR)
#define GETCHANTOSPR   ((sceDmaChan *)D9_CHCR)

extern sceDmaChan* chToSpr;
extern sceDmaChan* chFromSpr;

void initDma();

void dma_safety_net_vif0(const char *desc);
void dma_safety_net_tospr(const char *desc);
void dma_safety_net_fromspr(const char *desc);

#define ASSERT_ALIGNED64(n) MEASSERT((((unsigned int)(n))%0x40)==0)
#define ASSERT_ALIGNED16(n) MEASSERT((((unsigned int)(n))%0x10)==0)

#define REF_ID (0x30)
#define REFE_ID (0x00)
#define CNT_ID (0x10)
#define END_ID (0x70)
#define D_STAT_CISVIF0_M                        D_STAT_CIS0_M
#define D_STAT_CISVIF1_M                        D_STAT_CIS1_M
#define D_STAT_CISGIF_M                         D_STAT_CIS2_M
#define D_STAT_CISfromIPU_M                     D_STAT_CIS3_M
#define D_STAT_CIStoIPU_M                       D_STAT_CIS4_M
#define D_STAT_CISSIF0_M                        D_STAT_CIS5_M
#define D_STAT_CISSIF1_M                        D_STAT_CIS6_M
#define D_STAT_CISSIF2_M                        D_STAT_CIS7_M
#define D_STAT_CISfromSPR_M                     D_STAT_CIS8_M
#define D_STAT_CIStoSPR_M                       D_STAT_CIS9_M
#define D_STAT_StallInterruptStatus_M   D_STAT_SIS_M
#define D_STAT_MFIFOEmptyInterruptStatus_M  D_STAT_MEIS_M
#define D_STAT_BUSERRInterruptStatus_M          D_STAT_BEIS_M
#define D_STAT_CIMVIF0_M                  D_STAT_CIM0_M
#define D_STAT_CIMVIF1_M                  D_STAT_CIM1_M
#define D_STAT_CIMGIF_M                   D_STAT_CIM2_M
#define D_STAT_CIMfromIPU_M                 D_STAT_CIM3_M
#define D_STAT_CIMtoIPU_M                 D_STAT_CIM4_M
#define D_STAT_CIMSIF0_M                  D_STAT_CIM5_M
#define D_STAT_CIMSIF1_M                  D_STAT_CIM6_M
#define D_STAT_CIMSIF2_M                  D_STAT_CIM7_M
#define D_STAT_CIMfromSPR_M                 D_STAT_CIM8_M
#define D_STAT_CIMtoSPR_M                 D_STAT_CIM9_M
#define D_STAT_StallInterruptMask_M             D_STAT_SIM_M
#define D_STAT_MFIFOEmtpyInterruptMask_M        D_STAT_MEIM_M

#define D_PCR_CPCVIF0_M      D_PCR_CPC0_M
#define D_PCR_CPCVIF1_M      D_PCR_CPC1_M
#define D_PCR_CPCGIF_M       D_PCR_CPC2_M
#define D_PCR_CPCfromIPU_M   D_PCR_CPC3_M
#define D_PCR_CPCtoIPU_M     D_PCR_CPC4_M
#define D_PCR_CPCSIF0_M      D_PCR_CPC5_M
#define D_PCR_CPCSIF1_M      D_PCR_CPC6_M
#define D_PCR_CPCSIF2_M      D_PCR_CPC7_M
#define D_PCR_CPCfromSPR_M   D_PCR_CPC8_M
#define D_PCR_CPCtoSPR_M     D_PCR_CPC9_M


#define D_PCR_CDEALL_M   (D_PCR_CDE9_M | \
                          D_PCR_CDE8_M | \
                          D_PCR_CDE7_M | \
                          D_PCR_CDE6_M | \
                          D_PCR_CDE5_M | \
                          D_PCR_CDE4_M | \
                          D_PCR_CDE3_M | \
                          D_PCR_CDE2_M | \
                          D_PCR_CDE1_M | \
                          D_PCR_CDE0_M)

#define DPUT_CHTOVIF0_SADR   DPUT_D0_SADR
#define DPUT_CHTOVIF0_MADR   DPUT_D0_MADR
#define DPUT_CHTOVIF0_TADR   DPUT_D0_TADR
#define DPUT_CHTOVIF0_QWC    DPUT_D0_QWC
#define DPUT_CHTOVIF0_CHCR   DPUT_D0_CHCR

#define DGET_CHTOVIF0_CHCR   DGET_D0_CHCR

#define DPUT_CHTOSPR_SADR    DPUT_D9_SADR
#define DPUT_CHTOSPR_MADR    DPUT_D9_MADR
#define DPUT_CHTOSPR_TADR    DPUT_D9_TADR
#define DPUT_CHTOSPR_QWC     DPUT_D9_QWC
#define DPUT_CHTOSPR_CHCR    DPUT_D9_CHCR

#define DGET_CHTOSPR_CHCR    DGET_D9_CHCR

#define D_CHCR_START               D_CHCR_STR_M
#define D_CHCR_DIRECTION_FROMMEM   D_CHCR_DIR_M
#define D_CHCR_MODE_CHAIN          (0x01 << D_CHCR_MOD_O)
#define D_CHCR_MODE_NORMAL         (0x00 << D_CHCR_MOD_O)
#define D_CHCR_TAG_TRANSFER_ENABLE D_CHCR_TTE_M

#define VIF0_STAT_VIF_PIPELINE_ACTIVE VIF0_STAT_VPS_M
#define VIF0_STAT_FIFO_NONEMPTY       VIF0_STAT_FQC_M

#define VPU_STAT_VU0_EXECUTING        (0x01 << 0)

#define UNCACHED_SCEDMATAG(p) ((sceDmaTag *)((unsigned int)p | 0x30000000))
#define UNCACHED(p) ((unsigned int)(p)|0x30000000)
#define DMAADDR(x) ( ((u_int)(x))&0x0fffffff )

#define CHECKALIGNEDANDUNCACHED(x) MEASSERT((((u_int)(x))&0x3f)==0 && ((u_int)(x))&&0x30000000!=0)

#define DESTCHAIN_TAG_PCE_DISABLED (2 << 26)
#define DESTCHAIN_TAG_PCE_ENABLED  (3 << 26)
#define DESTCHAIN_TAG_ID_CNTS      (0 << 28)
#define DESTCHAIN_TAG_ID_CNT       (1 << 28)
#define DESTCHAIN_TAG_ID_END       (7 << 28)
#define DESTCHAIN_TAG_IRQ_ENABLE   (1 << 31)
#define DESTCHAIN_TAG_SPR          (1 << 63)

#define CPUSYNC  asm __volatile__("sync.l;sync.p");
#define BC0FWAIT asm __volatile__(".set noreorder;sync.l;sync.p;.align 3;0:;bc0t 0f;nop;bc0t 0f;nop;bc0t 0f;nop;bc0f 0b;nop;0:.set reorder");


#endif //!__KEAEEDEFS_HPP
