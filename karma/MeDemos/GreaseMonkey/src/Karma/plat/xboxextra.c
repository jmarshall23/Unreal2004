/***********************************************************************************************
*
*	$Id: xboxextra.c,v 1.1.2.1 2002/03/13 10:31:05 richardm Exp $
*
************************************************************************************************/
#define DIRECTINPUT_VERSION 0x0700

#include <xtl.h>

#ifndef _XBOX
#include <dinput.h>
#endif
#include <stdio.h>

#include "rwcore.h"
#include "rpworld.h"

#include "skeleton.h"
#include "events.h"
#include "MePrecision.h"
#include "control.hpp"
#include "xboxextra.h"
#include "platxtra.h"
#include "MePrecision.h"

extern void SetPlayerSteering(MeReal);
extern void SetPlayerThrottle(MeReal);
extern void wUpshift(void);
extern void wDownshift(void);

#define RANGE_MIN (-1000)
#define RANGE_MAX (1000)
#define DEADZONE (1000)
#define FF_GAIN (10000)

/*--- Functions ---*/

/*
 * Platform Specific Functionality
 */
static LARGE_INTEGER perf_timer_freq;
static LARGE_INTEGER perf_start_time;
static BOOL perf_timer = 0;


void
InitialisePerformanceTimer(void)
{
#ifdef PERFORMANCE_METRICS
	perf_timer = 0;

	if(QueryPerformanceFrequency(&perf_timer_freq)) perf_timer = 1;
#endif
}

void
ResetPerformanceTimer(void)
{
#ifdef PERFORMANCE_METRICS
	QueryPerformanceCounter(&perf_start_time);
#endif
}

float
ReadPerformanceTimer(void)
{
#ifdef PERFORMANCE_METRICS
	float res = 0.0f;
	LARGE_INTEGER perf_timer_count;

	if(perf_timer && perf_timer_freq.QuadPart)
	{
		QueryPerformanceCounter(&perf_timer_count);
 		return (float)(1000.0f * (double)(perf_timer_count.QuadPart - perf_start_time.QuadPart)/(double)perf_timer_freq.QuadPart); //time in ms
	}
#endif
	return 0.0f;
}





