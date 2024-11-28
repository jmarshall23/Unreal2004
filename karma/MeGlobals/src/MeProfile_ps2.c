/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/12 17:58:06 $ - Revision: $Revision: 1.26.2.7 $

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
 * PS2 implementation of platform specific timer functionality.
 */

/* Todo: More meaningful return codes */

#include <sys/types.h>
#include <eetypes.h>
#include <eeregs.h>
#include <libpc.h>

#include <MeProfile.h>
#include <MePrecision.h>
#include <MeSimpleFile.h>
#include <MeMessage.h>
#include <MeMemory.h>

#ifdef WIN32
#       define MeI64F                   "I64d"
#elif (defined IRIX || defined LINUX)
#       define MeI64F                   "llu"
#else /* Todo: PS2, ELATE ???*/
#       define MeI64F                   "lu"
#endif

#define PS2_CLOCKSPEED (294912000)

extern MeI64 clockSpeed;
extern MeProfileTimerResult frameTime;

static MeProfileTimerMode settings;

static int control;

void MEAPI GetOutputInfo()
{
    /* get approximation of clock speed */
    clockSpeed = PS2_CLOCKSPEED;
}

int MEAPI MeProfileStartHardwareTimer()
{
    MeProfileGetTimerValue(&frameTime);
    frameTime.count0 = 0;
    frameTime.count1 = 0;

    scePcStart(control,0,0);

    return 0;
}

void MEAPI MeProfileGetTimerValue(MeProfileTimerResult *const result)
{
    register int cycles, ctr0, ctr1;

    __asm__ __volatile__("  mfc0 %0,$9; nop; nop" : "=r" (cycles));
    __asm__ __volatile__("  mfpc %0,0;  nop; nop" : "=r" (ctr0));
    __asm__ __volatile__("  mfpc %0,1;  nop; nop" : "=r" (ctr1));

    /*
        for some peculiar reasons the casts as '(unsigned)' below are
        essential, _even if_ 'cycles' etc. are themselves declared
        'unsigned'
    */

    result->cpuCycles = (unsigned) cycles;
    result->count0 = (unsigned) ctr0;
    result->count1 = (unsigned) ctr1;

#if 0
    if (result->cpuCycles > 18440000000000000000llu)
    {
        MeDebug(0,"MeProfileGetTimerValue: cycles %u"
            ", result->cpuCycles %" MeI64F  " (0x%08x)\n",
            cycles,result->cpuCycles,&result->cpuCycles);
    }
#endif
}

int MEAPI MeProfileStopHardwareTimer()
{
    DPUT_T0_MODE(0);
    scePcStop();

    return 0;
}

int MEAPI MeProfileCreateHardwareTimer(MeProfileTimerMode mode)
{
    clockSpeed = PS2_CLOCKSPEED;

    settings=mode;

    /* 16 bit Bus Cycle Timer Config */
#if 0
    DPUT_T0_MODE(T_MODE_CUE_M |mode.granularity);
#endif

    /* Performance Counters Config */
    /* Todo: Make this fill in some strings somewhere to identify what
       each number means */

    switch(mode.counterMode)
    {
    case kMeProfileCounterModeFlops:
        control =
            SCE_PC0_COP2_COMP | SCE_PC_U0 | /* Vector Unit 0 Macro-Ops */
            SCE_PC1_COP1_COMP | SCE_PC_U1 | /* Floating Point Unit Ops */
            SCE_PC_CTE;

        *(mode.count0Label) = "VU0INS";
        *(mode.count1Label) = "FPUINS";
        break;

    /* To get the time wasted due to cache misses, we need the number
       of instructions issued and the number of cpu cycles */
    case kMeProfileCounterModeCache:
        control =
            SCE_PC0_SINGLE_ISSUE | SCE_PC_U0 |
            SCE_PC1_DUAL_ISSUE   | SCE_PC_U1 |
            SCE_PC_CTE;

        *(mode.count0Label) = "1ISSUE";
        *(mode.count1Label) = "2ISSUE";
        break;
    }

    return 0;
}

int MeProfileDestroyHardwareTimer()
{
    return MeProfileStopHardwareTimer();
}

/*
        Just for PS2 profiling
        Simple KeaTic style timer
*/
 
#define MAX_EVENTS 100000
 
 typedef struct {
        int           cycles;
        int           singleIssue;
        int           doubleIssue;
        const char *  name;
 } MeTicEvent;

int timer_enabled = 0;
int num_events = 0;

MeTicEvent *  ticEvents;

 
void MEAPI ticInit()
{
    int control;
    
    ticEvents = (MeTicEvent *)MeMemoryAPI.create(MAX_EVENTS*sizeof(MeTicEvent));
        
    num_events = 0;
    timer_enabled = 1;
 
    control  = (SCE_PC0_SINGLE_ISSUE | SCE_PC_U0);
    control |= (SCE_PC1_DUAL_ISSUE   | SCE_PC_U1);
    control |= SCE_PC_CTE;
    
    scePcStart(control, 0, 0);

}
void MEAPI ticCleanup()
{
    MeMemoryAPI.destroy(ticEvents);
}

void MEAPI tic(const char *name)
{       
    int cycles, singleIssue, doubleIssue;

    __asm__ __volatile__("mfc0 %0,$9" : "=r" (cycles));
    __asm__ __volatile__("mfpc %0,0": "=r" (singleIssue));
    __asm__ __volatile__("mfpc %0,1": "=r" (doubleIssue));
 
    if(MAX_EVENTS==num_events)
        MeFatalError(0,"simple timer buffer overflowed");
 
    ticEvents[num_events].cycles=cycles;
    ticEvents[num_events].singleIssue=singleIssue;
    ticEvents[num_events].doubleIssue=doubleIssue;
    ticEvents[num_events].name=name;
    num_events++;
}
 
void MEAPI ticOutputResults(const char *filename)
{
    int i;
    int total;
    int file;
    int bytes;
    char buf[256];
    
    file = MeOpen(filename,kMeOpenModeWRONLY);
    
    if(num_events!=0)
    {
        total = ticEvents[num_events-1].cycles-ticEvents[0].cycles;
        
        for(i=0;i!=num_events-1;i++)
        {
            int cycles = ticEvents[i+1].cycles-ticEvents[i].cycles;
            int singleIssue = ticEvents[i+1].singleIssue-ticEvents[i].singleIssue;
            int doubleIssue = ticEvents[i+1].doubleIssue-ticEvents[i].doubleIssue;
            
            bytes = sprintf(buf,"%50s %10d %5.1f%% %10d %10d\n",
                ticEvents[i].name,
                cycles,
                ((MeReal)cycles/(MeReal)total)*100.0f,
                singleIssue,
                doubleIssue);
            MeWrite(file,buf,bytes);
        }       
        bytes = sprintf(buf,"total                %10d\n",total);
        MeWrite(file,buf,bytes);
    }
    else
    {
        const int cycles = ticEvents[i+1].cycles-ticEvents[i].cycles;
        bytes = sprintf(buf,"%50s %10d %5.1f%%\n",
            ticEvents[i].name,cycles,
            ((MeReal)cycles/(MeReal)total)*100.0f);
        MeWrite(file,buf,bytes);
    }       
    
    bytes = sprintf(buf,"total                %10d\n",total);
    MeWrite(file,buf,bytes);

    MeClose(file);
}
