/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/12 17:58:06 $ - Revision: $Revision: 1.6.2.2 $

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
 *
 */

#include <time.h>
#include <unistd.h>
#include <MeProfile.h>
#include <MePrecision.h>

extern MeU64 clockSpeed;
extern MeProfileTimerResult frameTime;

void MEAPI MeProfileGetTimerValue(MeProfileTimerResult result)
{
    result->cpuCycles = 0;
    result->count0 = 0;
    result->count1 = 0;
}

int MEAPI MeProfileStartHardwareTimer(MeProfileTimerMode* mode)
{
    MeProfileGetTimerValue(&frameTime);
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

int MEAPI MeProfileDestroyHardwareTimer() {
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
    sleep(5);
    MeProfileGetTimerValue(&finish);
    clockSpeed = (finish.cpuCycles - start.cpuCycles) / 5;
}
