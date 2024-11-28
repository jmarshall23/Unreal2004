#ifndef _KEAPERFORMANCETIMER_HPP
#define _KEAPERFORMANCETIMER_HPP
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:02 $ - Revision: $Revision: 1.8.2.1 $

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
  Purpose: create accurate wall-clock timing on Pentium machines (typically
  80ns resolution).

  Description:

  PerformanceTimer *pc = CreatePerformanceTimer();

  startPerformanceTimer(pc);
  { ...  do your action in here ... }
  StopPerformanceTimer(pc);
  {  ... record pc->deltaTime or pc->m_avgTime ... }
  DestroyPerformanceTimer(pc);

  Note, "startPerformanceTimer(pc); StopPerformanceTimer(pc);" will not record
  a zero time difference.

  Example:
*/

#if 0
  #include <stdio.h>
  #include "keaPerformanceTimer.h"

  void main()
  {
      PerformanceTimer *pc = CreatePerformanceTimer();
      int loop = 5;

      while (loop--)
      {
          printf("Please press the return key");
          startPerformanceTimer(pc);
          getchar();
          StopPerformanceTimer(pc);
          printf("%4.6f seconds (rolling average:%4.6f)\n",
              pc->m_deltaTime, pc->m_avgTime);
      }
      DestroyPerformanceTimer(pc);
  }
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include <malloc.h>

#if 1
#if _XBOX
# include <winbase.h>
#else
  /* The correct way  */
# include <windows.h>
#endif
#else
  /*
    The fast, hack that may or may not work on your system
  */

  typedef _int32 LONG;
  typedef _int64 LONGLONG;
  typedef unsigned _int32 DWORD;

  typedef union _LARGE_INTEGER
  {
      struct
      {
          DWORD LowPart;
          LONG HighPart;
      };
      LONGLONG QuadPart;
  }
  LARGE_INTEGER;

# if 0
  __declspec(dllimport)
# endif

  int __stdcall QueryPerformanceCounter(LARGE_INTEGER *
      lpPerformanceCount);

  int __stdcall QueryPerformanceFrequency(LARGE_INTEGER *
      lpFrequency);

  /*
    QueryPerformanceFrequency is in kernel32.lib; one of standard
    include libraries.
  */
#endif

typedef struct
{
    /*
      Time in seconds between startPerformanceTimer() &
      stopPerformanceTimer().
    */
    double m_deltaTime;

    /*
      A rolling average of deltaTimes, weighted by "performanceDecay".
    */
    double m_avgTime;

    /*
      The counter used to hold the initial start time, then the difference in
      system ticks.
    */
    __int64 m_counter;
}
PerformanceTimer;

#define PerformanceDecay 0.98f

static __inline __int64 PerformanceSystemCounter()
{
    LARGE_INTEGER i;
    QueryPerformanceCounter(&i);
    /*
      Returns zero if there isn't a performance counter on the
      system.
    */
    return i.QuadPart;
}

static __inline __int64 PerformanceSystemTicksPerSecond()
{
    static LARGE_INTEGER i;
    static int notFirstTime;

    if (!notFirstTime)
    {
        notFirstTime++;
        QueryPerformanceFrequency(&i);
        i.QuadPart ? 0 : i.QuadPart++;
    };

    /*
      Returns zero if there isnt a performance counter on the system.

      Typical resolution on a PPro 200 Mhz  is 80 ns.
    */

    return i.QuadPart;
}

static __inline PerformanceTimer *CreatePerformanceTimer()
{
    PerformanceTimer *p =
        (PerformanceTimer *) malloc(sizeof (PerformanceTimer));

    p->m_deltaTime = p->m_avgTime = 0;

    return p;
};

static __inline void DestroyPerformanceTimer(PerformanceTimer *p)
{
    free(p);
}

static __inline void startPerformanceTimer(PerformanceTimer *p)
{
    p->m_counter = -PerformanceSystemCounter();
}

static __inline void StopPerformanceTimer(PerformanceTimer *p)
{
    p->m_counter += PerformanceSystemCounter();
    p->m_deltaTime = ((double) p->m_counter)
        / PerformanceSystemTicksPerSecond();
    p->m_avgTime = (p->m_avgTime == 0)
        ?  p->m_deltaTime
        : PerformanceDecay *p->m_avgTime
          + (1 - PerformanceDecay) *p->m_deltaTime;
}

#ifdef __cplusplus
}
#endif

#endif
