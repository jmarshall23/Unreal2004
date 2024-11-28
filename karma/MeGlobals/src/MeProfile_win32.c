/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/12 17:58:06 $ - Revision: $Revision: 1.15.2.2 $

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
 * Win32 implementation of platform specific timer functionality.
 *
 */


#include <time.h>
/* #include <windows.h> */
#include <MePrecision.h>
#include <MeProfile.h>

extern MeU64 clockSpeed;
extern MeProfileTimerResult frameTime;

/* Queries clock counter */

/* EPIC CHANGE 12/29/2003
 *  No inline asm in VS.NET/amd64...use intrinsic function instead.  --ryan.
 *  !!! FIXME: Use __rdtsc() on win32, too?  --ryan.
 */
#ifdef _WIN64
MeU64
ReadTsc(void)
{
    return(__rdtsc());
}
#else
MeU64 __declspec(naked)
ReadTsc(void)
{
    __asm
    {
        pushad
            cpuid
            popad
            rdtsc
            ret
    }
}
#endif

/* Pauses for a specified number of milliseconds. */
/* Todo: Surely this is in a system header somewhere? */
void sleep( clock_t wait )
{
   clock_t goal;
   goal = wait + clock();
   while( goal > clock() )
      ;
}

void MEAPI MeProfileGetTimerValue(MeProfileTimerResult *const result)
{
    result->cpuCycles = ReadTsc();
    result->count0 = 0;
    result->count1 = 0;
}


int MEAPI MeProfileStartHardwareTimer()
{
    MeProfileGetTimerValue(&frameTime);
    return 0;
}

int MEAPI MeProfileStopHardwareTimer()
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

    MeProfileGetTimerValue(&start);
    sleep( (clock_t) 2 * CLOCKS_PER_SEC );
    MeProfileGetTimerValue(&finish);

    clockSpeed = (finish.cpuCycles - start.cpuCycles)/2;
}

int MEAPI MeProfileCreateHardwareTimer(MeProfileTimerMode mode)
{
    *(mode.count0Label) = "N.A.";
    *(mode.count1Label) = "N.A.";

    return 0;
}

int MEAPI MeProfileDestroyHardwareTimer() {
    return 0;
}
