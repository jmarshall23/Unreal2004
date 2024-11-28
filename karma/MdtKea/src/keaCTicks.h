#ifndef _MDTCTICKS_H
#define _MDTCTICKS_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:01 $ - Revision: $Revision: 1.8.2.1 $

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

unsigned __int64 ReadTsc(void);
#define _CTP "%16s: %12I64d\n"

clock_t Start, Clocks, AllClocks, OverClocks, tmpClocks;
void ReadTscOverhead(void)
{
    tmpClocks = ReadTsc();
    OverClocks = ReadTsc() - tmpClocks;
}

#ifdef _WIN64
/* EPIC CHANGE  12/29/2003
 * !!! FIXME: This ain't right.  --ryan.
 */
unsigned __int64 ReadTsc(void) { return(0); }
#else
clock_t __declspec(naked)
ReadTsc(void)
{
  __asm
  {
    pushad
    cpuid
    popad
    rdtsc
    pushad
    cpuid
    popad
    ret
  }
}
#endif

