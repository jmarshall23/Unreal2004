/* -*- mode: C; -*- */

/*
  Copyright (c) 1997-2002 MathEngine PLC

  $Name: t-stevet-RWSpre-030110 $

  Date: $Date: 2002/04/12 17:58:06 $ - Revision: $Revision: 1.18.2.3 $

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
 * IRIX implementation of platform specific timer functionality.
 *
 */

/*
  This is the first MeProfile backend to use PCL as a layer to access
  the hardware counters and timers ... and as such might work with
  little modification on other platforms supported by PCL
*/

#include <time.h>
#include <unistd.h>
#include <MeMessage.h>
#include <MePrecision.h>
#include <MeProfile.h>
#ifdef ME_WITH_PCL
#include "pcl.h"
#include "determine_mhz.h"
#else
#include  <invent.h>
#endif

extern MeU64 clockSpeed;
extern MeProfileTimerResult frameTime;

void MEAPI GetOutputInfo(void);

#ifdef ME_WITH_PCL
int counter_list[2];
int res;
PCL_DESCR_TYPE descr;
const unsigned int pcl_mode = PCL_MODE_USER;
PCL_CNT_TYPE i_result_list[2];
PCL_FP_CNT_TYPE fp_result_list[2];
#endif

int MEAPI MeProfileStartHardwareTimer()
{
    int success;
    timespec_t tp;

#ifndef ME_WITH_PCL
    clock_gettime(CLOCK_SGI_CYCLE, &tp);
    frameTime.cpuCycles = (MeU64)(
        ((double)tp.tv_nsec + ((double)1000000000 * (double)tp.tv_sec)) *
        ((double)clockSpeed / (double)1000000000)
    );
#else
    frameTime.cpuCycles = 0;
#endif
    frameTime.count0 = 0;
    frameTime.count1 = 0;

#ifdef ME_WITH_PCL
    success = PCLstart(descr, counter_list, 2, pcl_mode);
    if(success != PCL_SUCCESS)
        return success;
    else
#endif
        return 0;
}

int MEAPI MeProfileStopHardwareTimer()
{
#ifdef ME_WITH_PCL
    int success;
    success = PCLstop(descr, i_result_list, fp_result_list, 2);
    if(success != PCL_SUCCESS)
        return success;
    else
#endif
        return 0;
}

int MEAPI MeProfileCreateHardwareTimer(MeProfileTimerMode mode)
{
#ifdef ME_WITH_PCL
    int success;

    if(PCLinit(&descr) != PCL_SUCCESS)
        MeWarning(3, "Couldn't initialise PCL");

    /* Todo: Make this fill in some strings somewhere to identify what
       each number means */

    switch(mode.counterMode)
    {
    case kMeProfileCounterModeFlops:
#if 0
        counter_list[0] = PCL_INSTR;
#else
        /* More accurate than using clock_gettime */
        counter_list[0] = PCL_CYCLES;
#endif
        counter_list[1] = PCL_FP_INSTR;

        *(mode.count0Label) = "CYCLES";
        *(mode.count1Label) = "FPU";
        break;

    case kMeProfileCounterModeCache:
        counter_list[0] = PCL_L1ICACHE_MISS;
        counter_list[1] = PCL_L1DCACHE_MISS;

        *(mode.count0Label) = "L1IMISS";
        *(mode.count1Label) = "L1DMISS";
        break;
    }

    success = PCLquery(descr, counter_list, 2, pcl_mode);
    if( success != PCL_SUCCESS)
        return success;
    else
#endif
        GetOutputInfo();

#if 0
    MeDebug(0, "Clockspeed: %llu", clockSpeed);
#endif
    return 0;
}

int MEAPI MeProfileDestroyHardwareTimer()
{
#ifdef ME_WITH_PCL
    int success = PCLexit(descr);
    if(success != PCL_SUCCESS)
        return success;
    else
#endif
        return 0;
}

void MEAPI MeProfileGetTimerValue(MeProfileTimerResult *const result)
{
    timespec_t tp;

#ifdef ME_WITH_PCL
    PCLread(descr, i_result_list, fp_result_list, 2);
#else
    clock_gettime(CLOCK_SGI_CYCLE, &tp);
#if 0
    MeDebug(0, "%u, %u\n", tp.tv_sec, tp.tv_nsec);
#endif
#endif

#ifndef ME_WITH_PCL
    /* What we really wanted was CPU cycles ... what we get is seconds
       and nanoseconds! Could use performance counter for better, less
       overheady, results... */

    /* time in nanoseconds * cycles per second * seconds per nanosecond */

    result->cpuCycles = (MeU64)(
        ((double)tp.tv_nsec + ((double)1000000000 * (double)tp.tv_sec))
        * ((double)clockSpeed / (double)1000000000)
    );
    result->count0 = 0;
    result->count1 = 0;

#if 0
    MeDebug(0, "Returning a cpuCycles value of: %llu\n", result.cpuCycles);
#endif
#else
    result->cpuCycles = i_result_list[0];
    result->count0 = i_result_list[0];
    result->count1 = i_result_list[1];
#endif
}

/*
 * Extract useful data from operating system.
 */
void MEAPI GetOutputInfo(void)
{
    /* This is a bit coarse really */
#ifdef ME_WITH_PCL
    clockSpeed = 1000000 * PCL_determine_mhz_rate();
#else
    /* This is, in fact, lifted from PCL ... */
    inventory_t *iv;
    int  found;

    found =  0;
    setinvent();
    clockSpeed = -1;
    for(iv = getinvent(); !found && (iv != NULL); iv = getinvent())
    {
        if((iv->inv_class == INV_PROCESSOR)
            && (iv->inv_type == INV_CPUBOARD))
        {
            found =  1;
            clockSpeed = (MeU64)iv->inv_controller * 1000000;
        }
    }
#if 0
    if(!found)
        return -1;
    else
        return 0;
#endif
#endif
}
