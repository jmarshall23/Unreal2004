/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:07 $ - Revision: $Revision: 1.8.2.1 $

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

/*
 * Elate implementation of platform specific timer functionality.
 *
 */

#include <MeProfile.h>
#include <MePrecision.h>

extern MeU64 clockSpeed;

/*static inline MeU64 ReadTsc(void)
{
    register MeU32 eax asm("ax");
    register MeU32 edx asm("dx");
    MeU64 result;
    asm("rdtsc" : "=a" (eax), "=d" (edx));
    result = ((MeU64)edx << 32) | eax;
    return result;
}
*/

void MeProfileGetTimerValue(MeProfileTimerResult *const result)
{
    result.cpuCycles = 0; /* ReadTsc(); */
    result.count0 = 0;
    result.count1 = 0;
}

void MeProfileStartHardwareTimer(MeProfileTimerMode* mode)
{
    extern MeProfileTimerResult frameTime;
    MeProfileGetTimerValue(&frameTime);
}

void MeProfileStopHardwareTimer()
{
}


/*
 * Extract useful data from operating system.
 */
void GetOutputInfo()
{
    /* get approximation of clock speed */
    MeProfileTimerResult start,finish;
    MeProfileGetTimerValue(&start);
    sleep(5);
    MeProfileGetTimerValue(&finish);
    clockSpeed = (finish.cpuCycles - start.cpuCycles) / 5;
}
