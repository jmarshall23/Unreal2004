/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/15 12:03:22 $ - Revision: $Revision: 1.10.2.6 $

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

/* Benchmark renderer */

#ifdef WITH_BENCHMARK

#include <stdio.h>

#include "Render_benchmark.h"

#define N_TESTS (2)
#define N_TICKS_PER_TEST (100)

/*
  The following figures relate to simple automated benchmark runs.
*/

/*
  If the tick function takes a 10th (0.1) of a second (100 ms):

  .1 * 1000 (ticks per test) * 10 (tests) * 10 (runs of program) = 10000
  seconds = 2 hours 47 minutes

  .1 * 100 (ticks per test) * 2 (tests) * 5 (runs of program) = 100 seconds =
  1.7 minutes (Which is a much reduced benchmark run time)
*/

/*
  For the purpose of this general benchmarking we are only taking the first
  200 iterations of the world evolve function since for any example the
  simulation could settle to a static state that would be missleading for an
  overall average of the results.
*/

#ifdef _WIN32
/*
  Thread priority
*/
#include <windows.h>
#include <winbase.h>

#endif /* _WIN32 */

void BoostPriority()
{

#ifdef _WIN32
#ifndef _DEBUG
    Sleep(2000);                /* Quieten down the system */
    Sleep(2000);                /* Quieten down the system */
    SetThreadPriority(GetCurrentThread, THREAD_PRIORITY_HIGHEST);
#endif /*  DEBUG */
#endif /*  WIN32 */
}

void NormalPriority()
{

#ifdef _WIN32
#ifndef _DEBUG
    SetThreadPriority(GetCurrentThread, THREAD_PRIORITY_NORMAL);
#endif /*  DEBUG */
#endif /*  WIN32 */

}

void Bench_SetWindowTitle(RRender *rc, const char* title)
{
  if (!(title && *title)) {
      MeWarning(0, "Bench_SetWindowTitle: You need to pass a valid char* in title.");
      return;
  }

  strncpy(rc->m_AppName, title, sizeof(rc->m_AppName));
  rc->m_AppName[sizeof(rc->m_AppName) -1] = '\0';
}

int Bench_CreateRenderer(RRender *rc)
{
    MeProfileTimerMode tmode;
    tmode.counterMode = kMeProfileCounterModeCache;
    tmode.granularity = 0;
    tmode.count0Label = 0;
    tmode.count1Label = 0;
    MeProfileStartTiming(tmode,rc->m_bProfiling);
    return 0;
}

void Bench_RunApp(RRender*rc, RMainLoopCallBack func, void *userdata)
{
    int i;
    int j;
    double time[N_TESTS];
    double average = 0;

    BoostPriority();

    {
        for (i = 0; i < N_TESTS; i++)
        {

            MeProfileStartFrame();
            MeProfileStartSection("Benchmark", 0);

            for (j = 0; j < N_TICKS_PER_TEST; j++)
                func(rc,userdata);

            MeProfileEndSection("Benchmark");
            MeProfileStopTimers();
            MeProfileEndFrame();

            time[i] = MeProfileGetSectionTime("Benchmark");

            MeInfo(0, "test %d (ms) = %4.6f", i,
                time[i] / N_TICKS_PER_TEST);

            average += time[i];
        }

    }

    NormalPriority();

    average /= N_TESTS * N_TICKS_PER_TEST;

    MeInfo(0, "\nEach test returns the average time over %d ticks.",
        N_TICKS_PER_TEST);
    MeInfo(0, "The number of tests and the number of ticks can be adjusted"
         " by changing\nN_TESTS and N_TICKS_PER_TEST in benchmark.c\n");
    MeInfo(0,"");
    MeInfo(0, "frames per second = %4.1f",
        (average ? (1 / average) : 0));
    MeInfo(0, "%% of a 60Hz frame = %4.1f", 6 * average);
    MeInfo(0, "average time per frame (ms) = %4.6f", average);

    if(rc->m_bProfiling)
        MeProfileOutputResults();
    MeProfileStopTiming();
}


#endif


