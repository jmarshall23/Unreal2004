/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/22 18:11:29 $ - Revision: $Revision: 1.8.2.5 $

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

#include <new>
#include <stdio.h>
#include <keaInternal.hpp>
#include <keaDebug.h>
#include <keaFunctions.hpp>

#ifdef PS2
#   include <keaEeDefs.hpp>
#endif

/*
 * The PC doesnt have crazy interrupt problems like the PS2
 * So the platform init does nothing
 *
 */
void keaFunctions_Vanilla :: platformInit()
{
}

#ifndef PS2
#ifndef _BUILD_VANILLA
void keaFunctions_SSE :: platformInit()
{
}
#endif
#endif

/* keaFunctions_PS2 :: platformInit
 * --------------------------------
 * Kill toSPR, fromSPR and vif0 interrupts to make it work with renderware
 */
#ifdef PS2

void keaFunctions_PS2 :: platformInit()
{

    DPUT_D_STAT(D_STAT_BUSERRInterruptStatus_M |
                D_STAT_MFIFOEmptyInterruptStatus_M |
                D_STAT_StallInterruptStatus_M |
                D_STAT_CIStoSPR_M |
                D_STAT_CISfromSPR_M |
                D_STAT_CISVIF0_M);

    DPUT_D_STAT(DGET_D_STAT() & (D_STAT_MFIFOEmtpyInterruptMask_M |
                                 D_STAT_StallInterruptMask_M |
                                 D_STAT_CIMtoSPR_M |
                                 D_STAT_CIMfromSPR_M |
                                 D_STAT_CIMVIF0_M));
}
#endif
