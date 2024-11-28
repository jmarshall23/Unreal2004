/***********************************************************************************************
*
*	$Id: skyextra.c,v 1.1.2.2 2002/03/10 19:19:16 piercarl Exp $
*
************************************************************************************************/
#include <sifdev.h>
#include <stdio.h>
#include <libpc.h>
#include "rwcore.h"

#include "platxtra.h"

#include "skeleton.h"

extern RwInt32 ReadPsTimer(void);

//int joystick_present = 0; //Well actually a joypad
static float last_time;

#if 0
__inline__ int scePcGetCounter0(void) {
	register int ctr0;
	asm __volatile__("mfpc %0, 0": "=r" (ctr0));
	return ctr0;
};
#endif

//Dummy functions for the moment
void
InitialisePerformanceTimer(void)
{
}

void
ResetPerformanceTimer(void)
{
#ifdef PERFORMANCE_METRICS
    static int control=(SCE_PC0_CPU_CYCLE|SCE_PC_U0
                    |SCE_PC1_CPU_CYCLE|SCE_PC_U1
                    |SCE_PC_CTE);

    scePcStart(control,0,0);

#if 0
#ifdef SCE_11
  last_time = (float)ReadPsTimer()/9216.0f;
#else
  last_time = (float)ReadPsTimer()/6250.0f;
#endif
#endif
#endif //PERFORMANCE_METRICS
}

float
ReadPerformanceTimer(void)
{
    float ret=0;

#ifdef PERFORMANCE_METRICS
    ret=(float)scePcGetCounter0();
#ifdef SCE_11
    ret/=150000.0f; //cpu clock cycles to milliseconds
#else
    ret/=300000.0f; //cpu clock cycles to milliseconds
#endif

#endif //PERFORMANCE_METRICS
    return(ret);

#if 0
  float new_time, delta_t;
#ifdef SCE_11
  new_time = (float)ReadPsTimer()/9216.0f;
#else
  new_time = (float)ReadPsTimer()/6250.0f;
#endif

  delta_t = (new_time - last_time);

  if(delta_t < 0) delta_t = 0.0f;

  return(delta_t);
#endif
}



