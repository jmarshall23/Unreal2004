/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/15 18:13:15 $ - Revision: $Revision: 1.15.2.3 $

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
 * Linux implementation of platform specific timer functionality.
 */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <MePrecision.h>
#include <MeProfile.h>

extern MeU64 clockSpeed;
extern MeProfileTimerResult frameTime;

void MEAPI MeProfileGetTimerValue(MeProfileTimerResult *const result)
{
#if __i386__
    register MeU32 eax asm("ax");
    register MeU32 edx asm("dx");

    __asm__ __volatile__("rdtsc" : "=a" (eax), "=d" (edx));

    result->cpuCycles = ((MeU64) edx << 32) | (MeU64) eax;
#else
    result->cpuCycles = 0;
#endif
    result->count0 = 0;
    result->count1 = 0;
}

int MEAPI MeProfileStartHardwareTimer(MeProfileTimerMode* mode)
{
    MeProfileGetTimerValue(&frameTime);
    return 0;
}

int MEAPI MeProfileStopHardwareTimer()
{
    return 0;
}

int MEAPI MeProfileCreateHardwareTimer(MeProfileTimerMode mode)
{
    *(mode.count0Label) = "N.A.";
    *(mode.count1Label) = "N.A.";

    return 0;
}

int MEAPI MeProfileDestroyHardwareTimer()
{
    return 0;
}

/*
 * Extract useful data from operating system.
 */
void MEAPI GetOutputInfo()
{
    /* get approximation of clock speed */
    MeProfileTimerResult start,finish;
    struct timeval t;

    t.tv_sec = 1;
    t.tv_usec = 0;

    MeProfileGetTimerValue(&start);
    select(0, 0,0,0, &t);
    MeProfileGetTimerValue(&finish);

    clockSpeed = (finish.cpuCycles - start.cpuCycles);
}
