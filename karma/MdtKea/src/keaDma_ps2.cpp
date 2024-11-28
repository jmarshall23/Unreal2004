/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.9.2.1 $

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

#include "keaEeDefs.hpp"
#include <stdio.h>
#include <keaFunctions.hpp>

sceDmaChan *chToSpr;
sceDmaChan *chFromSpr;

void initDma()
{
    //sceDmaReset(1);
    chToSpr   = sceDmaGetChan(SCE_DMA_toSPR);
    chFromSpr = sceDmaGetChan(SCE_DMA_fromSPR);
    chToSpr  ->chcr.TTE = 0;
    chFromSpr->chcr.TTE = 0;
}

void dma_safety_net_vif0(const char *desc)
{
    while(DGET_CHTOVIF0_CHCR()&D_CHCR_START)
    {
#ifdef _DEBUG
        printf("vif0 dma not finished %s\n",desc);
#endif
    }
}

void dma_safety_net_tospr(const char *desc)
{
    while(DGET_CHTOSPR_CHCR()&D_CHCR_START)
    {
#ifdef _DEBUG
        printf("tospr dma not finished %s\n",desc);
#endif
    }
}

void dma_safety_net_fromspr(const char *desc)
{
    while(DGET_CHFROMSPR_CHCR()&D_CHCR_START)
    {
#ifdef _DEBUG
        printf("fromspr dma not finished %s\n",desc);
#endif
    }
}