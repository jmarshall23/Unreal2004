/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/15 18:05:12 $ - Revision: $Revision: 1.54.2.12 $

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MeProfile.h>
#include <MeMessage.h>
#include <MeSimpleFile.h>
#include <MeMemory.h>

#define MeProfileCOUNTCREATED 0

#if (defined _MSC_VER)
#   define MeI64F                   "I64d"
#elif (defined IRIX || defined __GNUC__ || defined __MWERKS__)
#   define MeI64F                   "lld"
#else
#   define MeI64F                   "ld"
#endif

/*
 * Each section of code to be timed has a timer. At the end of each
 * frame a MeProfileFrameData struct is created and added to the list.
 *
 * Each MeProfileFrameData struct contains a list of
 * MeProfileSectionInfo structs.
 */

typedef struct MeProfileTimer  MeProfileTimer;
typedef struct MeProfileFrameData MeProfileFrameData;
typedef struct MeProfileFrameSectionInfo MeProfileFrameSectionInfo;

/*
    This is where profiling info gets collected while the program
    is running, inside a frame.
*/

struct MeProfileTimer
{
    MeProfileTimer              *next;
    const char                  *codeSection;

    unsigned                    timesCalled;
    /* Total number of cycles spent in this section since timing began */
    MeI64                       cpuCycles;
    MeI64                       count0;
    MeI64                       count1;

    /* Timer stops automatically at start of next one when != 0 */
    char unsigned               autoStop;
    char unsigned               isRunning;

    MeI64                       cpuCycleStartValue;
    MeI64                       count0StartValue;
    MeI64                       count1StartValue;
};

/*
    At the end of each frame, a new instance of this 'struct' is
    appended to the list; each instance containts a list of detailed
    profile information for each section that was run in that particular
    frame.

    This is appropriate for output to a GNUplot file, where one needs
    the data for each frame to do a graph across all frames.

    But (as observed by RichardT) usually all one wants is a summary at
    the end of the per section stats, and for that logging each frame is
    pointless; we should keep just the running averages.
*/

struct MeProfileFrameData
{
    MeProfileFrameData          *next;
    unsigned                    frameNumber;

    MeProfileFrameSectionInfo   *firstfsi;
    MeProfileTimerResult        timings;
};

struct MeProfileFrameSectionInfo
{
    MeProfileFrameSectionInfo   *next;
    const char                  *codeSection;

    unsigned                    timesCalled;
    MeI64                       cpuCycles;
    MeI64                       count0;
    MeI64                       count1;
};

static void doGnuplot();

/* The following functions are defined in platform specific .c files */

extern int      MEAPI MeProfileCreateHardwareTimer(MeProfileTimerMode mode);
extern int      MEAPI MeProfileDestroyHardwareTimer();
extern int      MEAPI MeProfileStartHardwareTimer();
extern int      MEAPI MeProfileStopHardwareTimer();
extern void     MEAPI GetOutputInfo();


static char unsigned            timingRunning = 0;
static char unsigned            insideFrame = 0;

static MeProfileTimerMode       *HWTMode;

static MeProfileTimer           *firstt;
static MeProfileFrameData       *firstfd;

static MeProfileOutput          output;
static MeProfileLogModes_enum   logging;

MeProfileTimerResult            frameTime;
MeI64                           clockSpeed = 0;

#if MeProfileCOUNTCREATED
long                            nTimer = 0;
long                            nTimerMode = 0;
long                            nFrameData = 0;
long                            nFrameSectionInfo = 0;
#endif

static const char               *count0Label;
static const char               *count1Label;

void MEAPI MeProfileStartTiming(MeProfileTimerMode mode,
    MeProfileLogModes_enum log)
{
    /* Set logging */
    logging = log;

    /* Indicate to StartSection that there are no Timers yet */
    firstt = 0;

    /* Indicate to EndFrame that there are no frames yet */
    firstfd = 0;

    /* set up default output information */
    output.jaggedness = 0.2f;
    output.settletime = 10;
    output.style = kMeProfileOutputEvent;

    /* Store the Hardware Timer Setup Information */
    HWTMode = (MeProfileTimerMode*)
        MeMemoryAPI.create(sizeof (MeProfileTimerMode));

    MEASSERT (HWTMode != 0);
#if MeProfileCOUNTCREATED
    nTimerMode++;
#endif

    *HWTMode = mode;

    if (mode.count0Label == 0)
        HWTMode->count0Label = &count0Label;
    if (mode.count1Label == 0)
        HWTMode->count1Label = &count1Label;

    MeProfileCreateHardwareTimer(*HWTMode);
    GetOutputInfo();
    /* The Timing is now running */
    timingRunning = 1;
}

void MEAPI MeProfileStopTiming()
{
    MeProfileTimer *itemt, *nextt;
    MeProfileFrameData *itemfd, *nextfd;
    MeProfileFrameSectionInfo *itemfsi, *nextfsi;

    MeMemoryAPI.destroy(HWTMode);
#if MeProfileCOUNTCREATED
    --nTimerMode;
#endif

    /* Free all the timer memory */

    for (itemt = firstt; itemt != 0; itemt = nextt)
    {
        nextt = itemt->next;
        MeMemoryAPI.destroy(itemt);
#if MeProfileCOUNTCREATED
        --nTimer;
#endif
    }

    /* Now we can free all the stored data memory too */

    for (itemfd = firstfd; itemfd != 0; itemfd = nextfd)
    {
        nextfd = itemfd->next;

        for (itemfsi = itemfd->firstfsi; itemfsi != 0; itemfsi = nextfsi)
        {
            nextfsi = itemfsi->next;
            MeMemoryAPI.destroy(itemfsi);
#if MeProfileCOUNTCREATED
            --nFrameSectionInfo;
#endif
        }

        MeMemoryAPI.destroy(itemfd);
#if MeProfileCOUNTCREATED
        --nFrameData;
#endif
    }

    MeProfileDestroyHardwareTimer();

    timingRunning = 0;
}

void MEAPI MeProfileStartFrame()
{
    MeProfileTimer  *itemt;

    /* Reset all the timers */
    if (firstfd != 0)
        for (itemt = firstt; itemt != 0; itemt = itemt->next)
        {
            itemt->timesCalled = 0;
            itemt->cpuCycles = 0;
            itemt->count0 = 0;
            itemt->count1 = 0;
        }

    /* Warn if there is already a frame running */

    if(insideFrame)
        MeWarning(3,"%s",
            "MeProfileStartFrame: You started a frame when there\n"
            "was already one running...\n"
            "Did you forget to call 'MeProfileEndFrame' somewhere?");

    insideFrame = 1;

    /* And start the hardware timer */
    MeProfileStartHardwareTimer();
}

void MEAPI MeProfileEndFrame()
{
    static unsigned frame = 0;
    static MeProfileFrameData *tailfd = 0;
    static MeProfileFrameSectionInfo *tailfsi = 0;

    MeProfileTimer *thistt, *itemt;
    MeProfileFrameData *thisfd;
    MeProfileFrameSectionInfo *thisfsi, *itemfsi, *nextfsi;
    MeProfileTimerResult thist;

    MeProfileGetTimerValue(&thist);

    /* If any Timers are running, stop them and issue a warning - did you
       really mean to let this timer run until the end of the frame? */

    for(itemt = firstt; itemt != 0; itemt = itemt->next)
        if (itemt->isRunning)
        {
            MeProfileEndSection(itemt->codeSection);
            MeWarning(3,"%s",
                "MeProfileEndFrame: You left the timer '%s' running\n"
                "until the end of the frame - did you mean to?",
                itemt->codeSection);
        }

    /* Save the data in this frame's FrameInfo */

    switch (logging)
    {
    case kMeProfileDontLog:
        if (firstfd != 0)
            thisfd = firstfd;
        else
        {
            thisfd = (MeProfileFrameData *)
                MeMemoryAPI.create(sizeof (MeProfileFrameData));

            MEASSERT (thisfd != 0);
#if MeProfileCOUNTCREATED
            nFrameData++;
#endif

            thisfd->next = 0;
            thisfd->firstfsi = 0;
        
            firstfd = thisfd;
        }
            
        thisfd->timings.cpuCycles = thist.cpuCycles - frameTime.cpuCycles;
        thisfd->timings.count0 = thist.count0 - frameTime.count0;
        thisfd->timings.count1 = thist.count1 - frameTime.count1;
        break;

    case kMeProfileLogTotals:
        if (firstfd != 0)
        {
            thisfd = firstfd;
            
            thisfd->timings.cpuCycles += thist.cpuCycles - frameTime.cpuCycles;
            thisfd->timings.count0 += thist.count0 - frameTime.count0;
            thisfd->timings.count1 += thist.count1 - frameTime.count1;
        }
        else
        {
            thisfd = (MeProfileFrameData *)
                MeMemoryAPI.create(sizeof (MeProfileFrameData));

            MEASSERT (thisfd != 0);
#if MeProfileCOUNTCREATED
            nFrameData++;
#endif

            thisfd->next = 0;
            thisfd->firstfsi = 0;
            thisfd->timings.cpuCycles = 0;
            thisfd->timings.count0 = 0;
            thisfd->timings.count1 = 0;
        
            firstfd = thisfd;
        }
        break;

    case kMeProfileLogAll:
        thisfd = (MeProfileFrameData *)
            MeMemoryAPI.create(sizeof (MeProfileFrameData));

        MEASSERT (thisfd != 0);
#if MeProfileCOUNTCREATED
        nFrameData++;
#endif

        if (firstfd == 0)
            firstfd = thisfd, frame = 0;
        else
            tailfd->next = thisfd;
        break;

        thisfd->next = 0;
        thisfd->firstfsi = 0;
        thisfd->timings.cpuCycles = thist.cpuCycles - frameTime.cpuCycles;
        thisfd->timings.count0 = thist.count0 - frameTime.count0;
        thisfd->timings.count1 = thist.count1 - frameTime.count1;
    }

    thisfd->frameNumber = frame++;

    switch (logging)
    {
    case kMeProfileDontLog:
        /* Clear the FrameSectionInfos from last time first */
        for (thisfsi = thisfd->firstfsi; thisfsi != 0; thisfsi = nextfsi)
        {
            nextfsi = thisfsi->next;
            MeMemoryAPI.destroy(thisfsi);
        }

        thisfd->firstfsi = 0;

        for (thistt = firstt; thistt != 0; thistt = thistt->next)
        {
            thisfsi = (MeProfileFrameSectionInfo *)
                MeMemoryAPI.create(sizeof (MeProfileFrameSectionInfo));

            MEASSERT (thisfsi != 0);
#if MeProfileCOUNTCREATED
            nFrameSectionInfo++;
#endif

            thisfsi->next = 0;
            thisfsi->codeSection = thistt->codeSection;        
            thisfsi->timesCalled = thistt->timesCalled;
            thisfsi->cpuCycles = thistt->cpuCycles;
            thisfsi->count0 = thistt->count0;
            thisfsi->count1 = thistt->count1;

            if (thisfd->firstfsi == 0)
                thisfd->firstfsi = thisfsi;
            else
                tailfsi->next = thisfsi;

            tailfsi = thisfsi;
        }
        break;

    case kMeProfileLogTotals:
        for (thistt = firstt; thistt != 0; thistt = thistt->next)
        {
            MeBool foundIt = 0;

            for (itemfsi = thisfd->firstfsi;
                 itemfsi != 0 && !foundIt;
                 itemfsi = itemfsi->next
            )
                if (strcmp(thistt->codeSection,itemfsi->codeSection) == 0)
                {
                    thisfsi = itemfsi;
                    foundIt = 1;
                }

            if (foundIt)
            {
                thisfsi->timesCalled += thistt->timesCalled;
                thisfsi->cpuCycles += thistt->cpuCycles;
                thisfsi->count0 += thistt->count0;
                thisfsi->count1 += thistt->count1;
            }
            else
            {
                thisfsi = (MeProfileFrameSectionInfo *)
                    MeMemoryAPI.create(sizeof (MeProfileFrameSectionInfo));

                MEASSERT (thisfsi != 0);
#if MeProfileCOUNTCREATED
                nFrameSectionInfo++;
#endif

                thisfsi->next = 0;
                thisfsi->codeSection = thistt->codeSection;        
                thisfsi->timesCalled = thistt->timesCalled;
                thisfsi->cpuCycles = thistt->cpuCycles;
                thisfsi->count0 = thistt->count0;
                thisfsi->count1 = thistt->count1;

                if (thisfd->firstfsi == 0)
                    thisfd->firstfsi = thisfsi;
                else
                    tailfsi->next = thisfsi;

                tailfsi = thisfsi;
            }
        }
        break;

    case kMeProfileLogAll:
        for (thistt = firstt; thistt != 0; thistt = thistt->next)
        {
            thisfsi = (MeProfileFrameSectionInfo *)
                MeMemoryAPI.create(sizeof (MeProfileFrameSectionInfo));

            MEASSERT (thisfsi != 0);
#if MeProfileCOUNTCREATED
            nFrameSectionInfo++;
#endif

            thisfsi->next = 0;
            thisfsi->codeSection = thistt->codeSection;        
            thisfsi->timesCalled = thistt->timesCalled;
            thisfsi->cpuCycles = thistt->cpuCycles;
            thisfsi->count0 = thistt->count0;
            thisfsi->count1 = thistt->count1;

            if (thisfd->firstfsi == 0)
                thisfd->firstfsi = thisfsi;
            else
                tailfsi->next = thisfsi;

            tailfsi = thisfsi;
        }
        break;
    }

    /* Now, this one is the last on the list */
    tailfd = thisfd;

    /* Stop the hardware timer so it doesn't overflow */
    MeProfileStopHardwareTimer();

    /* Frame is over */
    insideFrame = 0;
}

#ifndef _ME_NOPROFILING
void MEAPI MeProfileStartSectionFn(const char* codeSection,
    char unsigned autoStop)
{
    MeProfileTimer *thistt, *itemt, *prevt;
    MeBool foundIt = 0;

    if (!timingRunning)
        return;

    /* Go through list of running timers, stopping the one that is running
       and that has the autoStop flag set, if it exists */

    /* Also, have we seen this tag before? */
    for (itemt = firstt; itemt != 0; itemt = itemt->next)
    {
        if (itemt->autoStop  && itemt->isRunning)
            MeProfileEndSection(itemt->codeSection);

        if (strcmp(itemt->codeSection, codeSection) == 0)
        {
            foundIt = 1;
            thistt = itemt;
        }

        /* In case we need to make a new one and therefore update this
           one's next pointer: */
        prevt = itemt;
    }

    /* If we didn't find it, make a new one and put it at the end of the
       list */

    if (!foundIt)
    {
        thistt = (MeProfileTimer *)
            MeMemoryAPI.create(sizeof (MeProfileTimer));

        MEASSERT (thistt != 0);
#if MeProfileCOUNTCREATED
        nTimer++;
#endif

        thistt->next = 0;
        thistt->codeSection = 0;
        thistt->timesCalled = 0;
        thistt->cpuCycles = 0;
        thistt->count0 = 0;
        thistt->count1 = 0;
        thistt->autoStop = 0;
        thistt->isRunning = 0;
        thistt->cpuCycleStartValue = 0;
        thistt->count0StartValue = 0;
        thistt->count1StartValue = 0;

        if (firstt == 0)
            firstt = thistt;
        else
            prevt->next = thistt;

        thistt->codeSection = codeSection;
    }

    /* Now we have a timer to work with, set it up and note the
       appropriate start value */

    thistt->autoStop = autoStop;
    thistt->isRunning = 1;
    thistt->timesCalled++;

    /* Get cycle count from timer thingy */

    {
        MeProfileTimerResult result;

        MeProfileGetTimerValue(&result);

#if 0
        if (result.cpuCycles > 18440000000000000000llu)
            MeDebug(0,"MeProfileStartSectionFn: cpuCycles %" MeI64F
                " (0x%08x)",
                result.cpuCycles,&result.cpuCycles);
#endif

        thistt->cpuCycleStartValue = result.cpuCycles;
        thistt->count0StartValue = result.count0;
        thistt->count1StartValue = result.count1;
    }

    /* TODO: Subtract time taken in this function
       from count of all other running timers? */
}
#endif

#ifndef _ME_NOPROFILING
void MEAPI MeProfileEndSectionFn(const char* cs)
{
    MeProfileTimer *thistt, *itemt;
    MeProfileTimerResult result;
    MeBool foundIt;

    if (!timingRunning)
        return;

    /* First, get the value of the timer as soon as possible */
    MeProfileGetTimerValue(&result);

#if 0
    if (result.cpuCycles > 18440000000000000000llu)
        MeDebug(0,"MeProfileEndSectionFn: result.cpuCycles %" MeI64F
            " (0x%08x)",
            result.cpuCycles,&result.cpuCycles);
#endif

    /* TODO: Stop all running/autoStop timers ?*/

    /* If we can't find the appropriate section, bail out - with
       warning */

    foundIt = 0;

    for(itemt = firstt; itemt != 0; itemt = itemt->next)
        if (itemt->isRunning && strcmp(itemt->codeSection, cs) == 0)
        {
            foundIt = 1;
            thistt = itemt;
            break;
        }

    if (!foundIt)
        MeWarning(3,
            "MeProfileEndSectionFn: You tried to stop '%s'\n"
            "which you hadn't started, which is a bit silly.",
            cs
        );
    else
    {
        thistt->isRunning = 0;

        /* If we can, though, add the value to the
           total count for this section. Timer counts */

        thistt->cpuCycles += (result.cpuCycles - thistt->cpuCycleStartValue);
        thistt->count0 += (result.count0 - thistt->count0StartValue);
        thistt->count1 += (result.count1 - thistt->count1StartValue);
    }

    /* TODO: Subtract time taken in this function from count
       of all other running timers? */

    /* In other words, improve the accuracy of the timings */
}
#endif

MeReal MEAPI MeProfileGetSectionTime(const char *codeSection)
{
    MeProfileFrameData *thisfd;
    MeProfileFrameSectionInfo *thisfsi;

    if (firstfd == 0)
        return 0.0f;

    if(insideFrame) MeWarning(3,
        "MeProfileGetSectionTime: You called\n"
        "MeProfileGetSectionTime(\"%s\") while a frame was under way.\n"
        "This time is therefore for the frame before last!", codeSection);

    switch (logging)
    {
    case kMeProfileDontLog:
        /* We only keep data from the last frame */
        thisfd = firstfd;
        break;

    case kMeProfileLogTotals:
        /* We only keep cumulative data in the last frame */
        thisfd = firstfd;
        break;

    case kMeProfileLogAll:
        /* Get most recent frame */
        for (thisfd = firstfd; thisfd->next != 0; thisfd = thisfd->next)
          continue;
        break;
    }

    for (thisfsi = thisfd->firstfsi;
         thisfsi != 0 && strcmp(thisfsi->codeSection, codeSection) != 0;
         thisfsi = thisfsi->next)
        continue;

    if (thisfsi == 0)
        return 0.0f;

#ifdef WIN32
    return (MeReal)((MeI64)thisfsi->cpuCycles
        / ((float)(MeI64)clockSpeed)) * 1000.0f;
#else
#   if 1
    return (((MeReal)thisfsi->cpuCycles) / (MeReal)(clockSpeed) ) * 1000.0f;
#else
    printf("getting: %llu\n", thisfsi->cpuCycles);
    printf("%x\n", thisfsi);
    printf("profile gettime: %f\n",(((MeReal)thisfsi->cpuCycles)
               / (MeReal)(clockSpeed) ) * 1000.0f);
    return (((MeReal)thisfsi->cpuCycles));
#endif

#endif
}

MeU64 MEAPI MeProfileGetClockSpeed()
{
    return clockSpeed;
}

MeReal MEAPI MeProfileGetAllSectionTime()
{
    MeProfileFrameData *thisfd;
    MeProfileFrameSectionInfo *thisfsi;

    unsigned     nFrameDatas = 0;
#if 0
    unsigned     nSections = 0;
#endif

    MeU64 sectionsTotal = 0; /* total cycles of all sections put together */
    /* ^-- This has no profiling overheaded added,
       but is only valid for full section coverage */

    MeU64 avSections; /* average cycles per frame (all sections) */
    MeReal avSectionsTimeInSecs;

    if( !firstfd )
        return 0.0f;

    /* count the number of frames */
    for (thisfd = firstfd; thisfd; thisfd = thisfd->next)
        nFrameDatas++;

    /* go through each section of each frame and add up totals */
    for(thisfd = firstfd; thisfd; thisfd = thisfd->next)
        for(thisfsi = thisfd->firstfsi; thisfsi; thisfsi = thisfsi->next)
            sectionsTotal += thisfsi->cpuCycles;

    avSections = sectionsTotal/nFrameDatas;
#ifdef WIN32
    avSectionsTimeInSecs = (MeI64)avSections / (MeReal)(MeI64)clockSpeed;
#else
    avSectionsTimeInSecs = (MeReal)avSections / (MeReal)clockSpeed;
#endif

    /* Convert to ms */
    return avSectionsTimeInSecs * (MeReal)1000.0f;
}

void MEAPI MeProfileStopTimers()
{
    /*
     * Bit of a hack really - Stop all running timers to catch any
     * before EndFrame warns you about them :)
     */
    MeProfileTimer *itemt;

    for(itemt = firstt; itemt != 0; itemt = itemt->next)
        if (itemt->autoStop && itemt->isRunning)
            MeProfileEndSection(itemt->codeSection);
}

void MEAPI MeProfileOutputResults()
{
    MeProfileFrameData *thisfd;
    MeProfileFrameSectionInfo *thisfsi;
    MeProfileTimer *timer;

    unsigned nFrames;
    unsigned nFrameDatas = 0;
    unsigned nSections = 0;

    if (logging == kMeProfileDontLog)
        MeWarning(1, "%s",
            "MeProfileOutputResults: You didn't turn logging on,\n"
            "so this information isn't likely to be very useful!\n"
            "Call MeProfileStartTiming with 1 as the second argument.");

    /* gets useful platform specific data such as clock speed */
    GetOutputInfo();

    /* count the number of frames */
    for (thisfd = firstfd; thisfd != 0; thisfd = thisfd->next)
    {
        nFrameDatas++;
        if (thisfd->next == 0)
            nFrames = thisfd->frameNumber + 1;
    }

    if (logging == kMeProfileDontLog)
        nFrames = 1;

    /*
     * Count the number of sections. Here we just count the number of
     * sections for the first frame, but they should all be the same.
     */
    for (timer = firstt; timer != 0; timer = timer->next)
        nSections++;

    switch (output.style)
    {
    case kMeProfileOutputNormal:

        for(thisfd = firstfd; thisfd != 0; thisfd = thisfd->next )
        {
            MeInfo(0,"Frame %d.", thisfd->frameNumber);

            for(thisfsi = thisfd->firstfsi; thisfsi; thisfsi = thisfsi->next)
            {
                MeInfo(0, "%s: %" MeI64F " cycles",
                    thisfsi->codeSection,thisfsi->cpuCycles);

                MeInfo(0, "%s: %" MeI64F " cycles",
                    thisfsi->codeSection,thisfsi->cpuCycles);
            }
        }
        break;

    case kMeProfileOutputAverage:
    {
        /* sectionTotals are total cycles for a code section
           over all frames */
        MeU64 *sectionTotal = (MeU64*)
            MeMemoryAPI.createAligned(nSections*sizeof(MeU64),sizeof(MeU64));
        /* total cycles of all frames put together */
        MeU64 total = 0;
        /* total cycles of all sections put together */
        MeU64 sectionsTotal = 0;
        /* ^-- This has no profiling overheaded added,
           but is only valid for full section coverage */
        MeU64 avCpu; /* average cycles per frame */
        MeU64 avSections; /* average cycles per frame (all sections) */
        float avTimeInSecs;
        float avSectionsTimeInSecs;
        unsigned     i;

        MEASSERT (sectionTotal != 0);

        for (i = 0; i < nSections; i++)
            sectionTotal[i] = 0;

        /* go through each section of each frame and add up totals */
        for (thisfd = firstfd; thisfd != 0; thisfd = thisfd->next)
        {
            i = 0;

            total += thisfd->timings.cpuCycles;

            for (thisfsi = thisfd->firstfsi;
                 thisfsi != 0;
                 thisfsi = thisfsi->next)
            {
                sectionTotal[i++] += thisfsi->cpuCycles;
                sectionsTotal += thisfsi->cpuCycles;
            }
        }

        MeInfo(0, "Averages over %u frames."
            " NB: Sections can be concurrent.",nFrames);
        MeInfo(0,"");

        MeInfo(0, "SECTION PROFILED                 AV CYCLES   AV %%  AV MS"
            "");

        for (timer = firstt, i=0; timer != 0; timer = timer->next,i++)
        {
#ifdef WIN32
            MeInfo(0, "%-30s: %10I64d %6.3f %6.3f",
                timer->codeSection,sectionTotal[i]/nFrames,
                (100.0f * (float)(MeI64)sectionTotal[i]/(float)(MeI64)total),
                ((float)(MeI64)(sectionTotal[i]/nFrames)
                    /((float)(MeI64)clockSpeed)) * 1000.0f);
#else
            MeInfo(0, "%-30s: %10u %6.3f %6.3f",
                timer->codeSection,(MeU32)(sectionTotal[i]/nFrames),
                ((100.0f * (float)sectionTotal[i])/(float)total),
                ((1000.0f * (float)sectionTotal[i]/(float)nFrames)
                    /((float) clockSpeed )));
#endif
        }

        avSections = sectionsTotal/nFrames;
        avCpu = total/nFrames;

        MeInfo(0, "SECTIONS TOTAL                  %10" MeI64F "",
            avSections);
        MeInfo(0, "TOTAL                           %10" MeI64F "",avCpu);

#ifdef WIN32
        avSectionsTimeInSecs = (MeI64)avSections / (float)(MeI64)clockSpeed;
        avTimeInSecs = (MeI64)avCpu / (float)(MeI64)clockSpeed;
#else
        avSectionsTimeInSecs = (float)avSections / (float)clockSpeed;
        avTimeInSecs = (float)avCpu / (float)clockSpeed;
#endif

        MeInfo(0, "Clock speed (approx): %" MeI64F "MHz",
            (clockSpeed+100000)/1000000);

        MeInfo(0, "Average time %6.2f milliseconds"
            ", %5.2f%% of a 60hz frame (sections)",
            avSectionsTimeInSecs*1000.0f,
            avSectionsTimeInSecs*60.0f*100.0f);

        MeInfo(0, "Average time %6.2f milliseconds"
            ", %5.2f%% of a 60hz frame (total)",
            avTimeInSecs*1000.0f,
            avTimeInSecs*60.0f*100.0f);

        MeMemoryAPI.destroyAligned(sectionTotal);
        break;
    }

    case kMeProfileOutputEvent:
    {
        /*
           For each frame we have both:

           - a per frame overall cycle, count0 and count1 totals;

           - for each frame, a list of sections each of which has:

             + per section number of times entered the section;
             + per section cycle, count0 and count1.

           We also have a total number of sections and a total number of
           frames.

           We want to print:

           - overall average cycle, count0, count1 numbers over all
             frames, irrespective of section;

           - average cycle, count0, count1 numbers per section over all
             frames;

           - average cycle, count0, count1 numbers per section over all
             executions of a section.

           So we want to collect cycle, count0 and count1 total for the
           whole execution, overall and by section, and then divide the
           overall numbers by the number of frames, and then the per
           section numbers by frame and by number of invocations by
           section.
        */

        static const size_t size64 = sizeof (MeI64);

        MeI64 secondsTotal = 0;

        register unsigned i;

        /*
            Overalls totals, for each whole frame, not just the inside
            sections. 'eventsAllFrames' is a bit dodgy and probably
            useless, but we have it for simmetry.

            Note that the totals do not correspond to the sum of the
            numbers for all sections, they may be larger or smaller than
            that sum:

            - If not all of a frame is covered by sections, the sum may
              be smaller than the overall per frame number.

            - If sections are nested, the sum may be greater.
        */
        MeI64 eventsAllFrames = 0;
        MeI64 cyclesAllFrames = 0;
        MeI64 count0AllFrames = 0;
        MeI64 count1AllFrames = 0;

        float secondsAllFrames;
        float secondsAvgPerFrame;

        MeI64 *const eventsPerSection = (MeI64 *)
            MeMemoryAPI.createAligned(nSections*size64,size64);
        MeI64 *const cyclesPerSection = (MeI64 *)
            MeMemoryAPI.createAligned(nSections*size64,size64);
        MeI64 *const count0PerSection = (MeI64 *)
            MeMemoryAPI.createAligned(nSections*size64,size64);
        MeI64 *const count1PerSection = (MeI64 *)
            MeMemoryAPI.createAligned(nSections*size64,size64);

        MEASSERT (eventsPerSection != 0);
        MEASSERT (cyclesPerSection != 0);
        MEASSERT (count0PerSection != 0);
        MEASSERT (count1PerSection != 0);

        for (i = 0; i < nSections; i++)
        {
          eventsPerSection[i] = 0;
          cyclesPerSection[i] = 0;
          count0PerSection[i] = 0;
          count1PerSection[i] = 0;
        }

        /* go through each section of each frame and add up totals */

        for (thisfd = firstfd;
             thisfd != 0;
             thisfd = thisfd->next)
        {
            cyclesAllFrames += thisfd->timings.cpuCycles;
            count0AllFrames += thisfd->timings.count0;
            count1AllFrames += thisfd->timings.count1;

            for ((thisfsi = thisfd->firstfsi), i = 0;
                 thisfsi != 0;
                 (thisfsi = thisfsi->next), i++)
            {
                eventsAllFrames += thisfsi->timesCalled;

                eventsPerSection[i] += thisfsi->timesCalled;
                cyclesPerSection[i] += thisfsi->cpuCycles;
                count0PerSection[i] += thisfsi->count0;
                count1PerSection[i] += thisfsi->count1;
            }
        }

#ifdef WIN32
        secondsAllFrames = (float) (MeI64) cyclesAllFrames
            / (float) (MeI64) clockSpeed;
#else
        secondsAllFrames = (float) cyclesAllFrames
            / (float) clockSpeed;
#endif
        secondsAvgPerFrame = secondsAllFrames/nFrames;

        MeInfo(0,"CLOCK SPEED (approx): %" MeI64F "MHz",
            ((clockSpeed+100000)/1000000));

        MeInfo(0,"");
        MeInfo(0,"OVERALL TOTALS:");

        MeInfo(0,"  %6.1f milliseconds, %u frames",
               secondsAllFrames*1000.0f,nFrames);
        MeInfo(0,"  cycles: %11" MeI64F "; calls:  %11" MeI64F,
               cyclesAllFrames,eventsAllFrames);
        MeInfo(0,"  count0: %11" MeI64F "; count1: %11" MeI64F,
               count0AllFrames,count1AllFrames);
        MeInfo(0,"");

#if MeProfileCOUNTCREATED
        MeInfo(0," nTimer %u, nTimerMode %u",nTimer,nTimerMode);
        MeInfo(0," nFrameData %u, nFrameSectionInfo %u",
            nFrameData,nFrameSectionInfo);
        MeInfo("");
#endif

        /*
            Now we print the overall numbers divided by the number
            of frames.
        */
        {
            const MeI64 eventsAvgPerFrame = eventsAllFrames/nFrames;
            const MeI64 cyclesAvgPerFrame = cyclesAllFrames/nFrames;
            const MeI64 count0AvgPerFrame = count0AllFrames/nFrames;
            const MeI64 count1AvgPerFrame = count1AllFrames/nFrames;

            MeInfo(0,"OVERALL PER FRAME AVERAGES:");
            MeInfo(0,"  %6.1f milliseconds, %-4.1f%% of 60Hz",
                secondsAvgPerFrame*1000.0f,
                secondsAvgPerFrame*60.0f*100.0f);
            MeInfo(0,"  cycles: %11" MeI64F "; calls:  %11" MeI64F,
                cyclesAvgPerFrame,eventsAvgPerFrame);
            MeInfo(0,"  count0: %11" MeI64F "; count1: %11" MeI64F,
                count0AvgPerFrame,count1AvgPerFrame);
            MeInfo(0,"");
        }

        MeInfo(0,"    %s",
            "IN THE FOLLOWING NOTE THAT SECTIONS CAN OVERLAP/NEST");

#if 1
        MeInfo(0,"");
        MeInfo(0,
            "SECTION TOTALS                  CYCLES   CALLS"
             " %10.10s %10.10s"
#if (defined PS2)
            " STALLS"
#endif
            ,*(HWTMode->count0Label),*(HWTMode->count1Label)
        );

        for (timer = firstt, i = 0;
             timer != 0;
             timer = timer->next, i++)
        {
            MeInfo(0, "%-26.26s:%11" MeI64F " %7" MeI64F
                " %10" MeI64F " %10" MeI64F
#if (defined PS2)
                " %4.1f%%"
#endif
                ,timer->codeSection,
                cyclesPerSection[i],eventsPerSection[i],
                count0PerSection[i],count1PerSection[i]
#if (defined PS2)
                ,100.0f
                * (1.0f - (float)
                    (count0PerSection[i]
                        + count1PerSection[i])
                    / (float) cyclesPerSection[i])
#endif
            );
        }
#endif

#if 1
        MeInfo(0,"");
        MeInfo(0,
            "SECTION AVG. PER CALL           CYCLES     MS."
            " %10.10s %10.10s"
            ,*(HWTMode->count0Label),*(HWTMode->count1Label)
        );

        for (timer = firstt, i = 0;
             timer != 0;
             timer = timer->next, i++)
        {
            const MeU32 nEvents = (MeU32)eventsPerSection[i];
            const MeI64 cyclesSectAvgEvent
              = (nEvents == 0) ? 0 : cyclesPerSection[i]/nEvents;
            const MeU32 count0SectAvgEvent
              = (nEvents == 0) ? 0 : (MeU32)count0PerSection[i]/nEvents;
            const MeU32 count1SectAvgEvent
              = (nEvents == 0) ? 0 : (MeU32)count1PerSection[i]/nEvents;
#ifdef WIN32
            const float secondsSectAvgEvent
                = (float) (MeI64) cyclesSectAvgEvent
                / (float) (MeI64) clockSpeed;
#else
            const float secondsSectAvgEvent
                = (float) cyclesSectAvgEvent
                / (float) clockSpeed;
#endif

            MeInfo(0, "%-26.26s:%11" MeI64F " %7.3f" " %10lu %10lu"
                ,timer->codeSection,
                cyclesSectAvgEvent,secondsSectAvgEvent*1000.0f,
                count0SectAvgEvent,count1SectAvgEvent
            );
        }
#endif

#if 0
        /*
            This is disabled by default as per-frame average make almost
            no sense for most sections, except those that get called
            only once per frame anyhow.
        */

        MeInfo(0,"");
        MeInfo(0,
            "SECTION AVG. PER FRAME          CYCLES     MS.    COUNT0    COUNT1"
        );

        for (timer = firstt, i = 0;
             timer != 0;
             timer = timer->next, i++)
        {
            const MeU32 eventsSectAvgFrame = eventsPerSection[i]/nFrames;
            const MeI64 cyclesSectAvgFrame = cyclesPerSection[i]/nFrames;
            const MeU32 count0SectAvgFrame = count0PerSection[i]/nFrames;
            const MeU32 count1SectAvgFrame = count1PerSection[i]/nFrames;

#ifdef WIN32
            const float secondsSectAvgFrame
                = (float) (MeI64) cyclesSectAvgFrame
                / (float) (MeI64) clockSpeed;
#else
            const float secondsSectAvgFrame
                = (float) cyclesSectAvgFrame
                / (float) clockSpeed;
#endif

            MeInfo(0, "%-26.26s:%11" MeI64F " %7.3f %10lu %10lu",
                timer->codeSection,
                cyclesSectAvgFrame,
                secondsSectAvgFrame*1000.0f,
                count0SectAvgFrame,
                count1SectAvgFrame
            );
        }
#endif

        MeMemoryAPI.destroyAligned(eventsPerSection);
        MeMemoryAPI.destroyAligned(cyclesPerSection);
        MeMemoryAPI.destroyAligned(count0PerSection);
        MeMemoryAPI.destroyAligned(count1PerSection);

        break;
    }

    case kMeProfileOutputGnuplot:
        doGnuplot();
        break;

    default:
        MeWarning(12,"MeProfileOutputResults:"
            " MeProfile output style not recognized");
    }
}

void MEAPI MeProfileSetOutputParameters(MeProfileOutput p)
{
    output.jaggedness = p.jaggedness;
    output.settletime = p.settletime;
    output.style = p.style;
}

static void doGnuplot()
{
    int fh, counter;
    char buffer[1000];

    MeU64 cpuCycleAcc = 0, count0Acc = 0, count1Acc = 0;

    typedef struct _weightingData
    {
        struct _weightingData   *next;
        MeU64                   timesCalled;

        MeU64                   cpuCycles;
        MeU64                   count0;
        MeU64                   count1;
    }
    weightingData;

    weightingData *thisWD, *tailWD, *firstWD=0;

    short unsigned frames;

    char unsigned twiddler = 0;

    MeProfileTimer *thist;
    MeProfileFrameData *thisfd;
    MeProfileFrameSectionInfo *thisfsi;

    /* Construct the weighting data lookup list */

    for (thist = firstt; thist != 0; thist = thist->next)
    {
        thisWD = (weightingData *)
            MeMemoryAPI.create(sizeof(weightingData));

        MEASSERT (thisWD != 0);

        
        thisWD->next = 0;
        thisWD->timesCalled = 0;
        thisWD->cpuCycles = 0;
        thisWD->count0 = 0;
        thisWD->count1 = 0;

        if (firstWD == 0)
            firstWD = thisWD;
        else
            tailWD->next = thisWD;

        tailWD = thisWD;
    };

    /* Make one for the whole-frame data */
    thisWD = (weightingData *) MeMemoryAPI.createZeroed(sizeof(weightingData));
    MEASSERT (thisWD != 0);

    tailWD->next = thisWD;
    tailWD = thisWD;

    /* And populate the list with initial values for those
       that exist in the first frame we plot */

    thisfd = firstfd;

    MeInfo(0, "twiddler=%d output.settletime=%d thisfd=%08x",
        twiddler,output.settletime,thisfd);

    while(twiddler++ != output.settletime)
        thisfd = thisfd->next;

    for(thisfsi = thisfd->firstfsi, thisWD = firstWD;
        thisfsi != 0;
        thisfsi = thisfsi->next, thisWD=thisWD->next)
    {
        thisWD->cpuCycles = thisfsi->cpuCycles;
        thisWD->timesCalled = thisfsi->timesCalled;
        thisWD->count0 = thisfsi->count0;
        thisWD->count1 = thisfsi->count1;
    }

    /* The whole frame one */

    tailWD->cpuCycles = thisfd->timings.cpuCycles;
    tailWD->count0 = thisfd->timings.count0;
    tailWD->count1 = thisfd->timings.count1;

    /* The rest can just start at 0 */

    switch (output.style)
    {
    case kMeProfileOutputGnuplot:
        /* Make four command files */

        /* CPU cycles */

        fh = MeOpen("cyclePlot.gpt", kMeOpenModeWRONLY);
        sprintf(0, buffer, "plot ");
        MeWrite(fh,buffer, strlen(buffer));

        for(counter=2, thist=firstt; thist; thist=thist->next, counter+=4)
        {
            sprintf(buffer, "\"MeProfile.dat\" using 1:%d title "
                "\"%s - Cycles\" with lines",
                counter, thist->codeSection);
            sprintf(buffer + strlen(buffer), ", ");
            MeWrite(fh, buffer, strlen(buffer));
        }

        /* Add overall Frame data */
        sprintf(buffer, "\"MeProfile.dat\" using 1:%d title "
            "\"%s\" with lines", counter, "Whole Frame");
        sprintf(buffer + strlen(buffer), "");
        MeWrite(fh, buffer, strlen(buffer));
        MeClose(fh);

        /* Times Called */
        fh = MeOpen("timesCalledPlot.gpt",kMeOpenModeWRONLY);
        sprintf(buffer, "plot ");
        MeWrite(fh, buffer, strlen(buffer));

        for (counter = 3, thist = firstt;
             thist != 0;
             thist = thist->next, counter += 4)
        {
            sprintf(buffer, "\"MeProfile.dat\" using 1:%d title "
                "\"%s - Times Called\" with lines%s",
              counter, thist->codeSection,
              (thist->next != 0) ? ", " : "");

            MeWrite(fh, buffer, strlen(buffer));
        }

        /* (No data for whole frame - 'Times Called'
           doesn't make any sense here!) */

        MeClose(fh);

        /* Count 0 */

        fh = MeOpen("count0Plot.gpt",kMeOpenModeWRONLY);
        sprintf(buffer, "plot ");
        MeWrite(fh, buffer, strlen(buffer));

        for (counter = 4, thist = firstt;
             thist != 0;
             thist = thist->next, counter += 4)
        {
            switch(HWTMode->counterMode)
            {
            case kMeProfileCounterModeFlops:
                sprintf(buffer, "\"MeProfile.dat\" using 1:%d title "
                    "\"%s - VeOps\" with lines, ",
                    counter, thist->codeSection);
                break;

            case kMeProfileCounterModeCache:
                sprintf(buffer, "\"MeProfile.dat\" using 1:%d title "
                    "\"%s - ICache Misses\" with lines, ",
                    counter, thist->codeSection);
                break;
            }

            MeWrite(fh, buffer, strlen(buffer));
        }

        /* Add overall Frame data */
        sprintf(buffer, "\"MeProfile.dat\" using 1:%d title "
            "\"%s\" with lines", counter-1, "Whole Frame");
        MeWrite(fh, buffer, strlen(buffer));

        MeClose(fh);

        /* Count 1 */

        fh = MeOpen("count1Plot.gpt", kMeOpenModeWRONLY);
        sprintf(buffer, "plot ");
        MeWrite(fh, buffer, strlen(buffer));

        for(counter=5, thist=firstt; thist; thist=thist->next, counter+=4)
        {
            switch(HWTMode->counterMode)
            {
            case kMeProfileCounterModeFlops:
                sprintf(buffer, "\"MeProfile.dat\" using 1:%d title "
                    "\"%s - FlOps\" with lines",
                    counter, thist->codeSection);
                break;
            case kMeProfileCounterModeCache:
                sprintf(buffer, "\"MeProfile.dat\" using 1:%d title "
                    "\"%s - DCache Misses\" with lines",
                    counter, thist->codeSection);
                break;
            }
            sprintf(buffer + strlen(buffer), ", ");
            MeWrite(fh, buffer, strlen(buffer));
        }

        /* Add overall Frame data */
        sprintf(buffer, "\"MeProfile.dat\" using 1:%d "
            "title \"%s\" with lines", counter-1, "Whole Frame");
        sprintf(buffer + strlen(buffer), "");
        MeWrite(fh, buffer, strlen(buffer));

        MeClose(fh);

        fh = MeOpen("MeProfile.dat", kMeOpenModeWRONLY);

        /*
            Column Headings
            TODO: FixMe: make match columns of data again (?)
        */

#if 0
        sprintf(buffer, "# \"Frame no.\"");
        MeWrite(fh, buffer, strlen(buffer));
        for(thist = firstt; thist; thist = thist->next )
        {
            sprintf(buffer, ",\"%s\"", thist->codeSection );
            MeWrite(fh, buffer, strlen(buffer));
        }
        sprintf(buffer, "");
        MeWrite(fh, buffer, strlen(buffer));
#endif

        for(thisfd = firstfd; thisfd; thisfd = thisfd->next )
        {
            /* Comment out first few frames to let things settle down */

            if(thisfd->frameNumber<=output.settletime)
            {
                sprintf(buffer, "#");
                MeWrite(fh, buffer, strlen(buffer));
            }

            /* Frame Number */

            sprintf(buffer, "%d", thisfd->frameNumber);
            MeWrite(fh, buffer, strlen(buffer));

            /* Data for each section*/

            for(thisfsi = thisfd->firstfsi, thisWD = firstWD;
                thisfsi != 0;
                thisfsi = thisfsi->next, thisWD = thisWD->next)
            {

                thisWD->cpuCycles = (MeU64)(output.jaggedness) *
                    thisfsi->cpuCycles + (MeU64)(1.0f - output.jaggedness) *
                    thisWD->cpuCycles;

                sprintf(buffer, " %llu", thisWD->cpuCycles);
                MeWrite(fh, buffer, strlen(buffer));

                thisWD->timesCalled = (MeU64)(output.jaggedness) *
                    thisfsi->timesCalled + (MeU64)(1.0f - output.jaggedness) *
                    thisWD->timesCalled;

                sprintf(buffer, " %llu", thisWD->timesCalled);
                MeWrite(fh, buffer, strlen(buffer));

                thisWD->count0 = (MeU64)(output.jaggedness) *
                    thisfsi->count0 + (MeU64)(1.0f - output.jaggedness) *
                    thisWD->count0;

                sprintf(buffer, " %llu", thisWD->count0);
                MeWrite(fh, buffer, strlen(buffer));

                thisWD->count1 = (MeU64)(output.jaggedness) *
                    thisfsi->count1 + (MeU64)(1.0f - output.jaggedness) *
                    thisWD->count1;

                sprintf(buffer, " %llu", thisWD->count1);
                MeWrite(fh, buffer, strlen(buffer));

            }

            /* Overall data for frame */

            tailWD->cpuCycles = (MeU64)(output.jaggedness) *
                thisfd->timings.cpuCycles + (MeU64)(1.0f - output.jaggedness) *
                thisWD->cpuCycles;

            sprintf(buffer, " %llu", tailWD->cpuCycles);
            MeWrite(fh, buffer, strlen(buffer));

            tailWD->count0 = (MeU64)(output.jaggedness) *
                thisfd->timings.count0 + (MeU64)(1.0f - output.jaggedness) *
                thisWD->count0;

            sprintf(buffer, " %llu", tailWD->count0);
            MeWrite(fh, buffer, strlen(buffer));

            tailWD->count1 = (MeU64)(output.jaggedness) *
                thisfd->timings.count1 + (MeU64)(1.0f - output.jaggedness) *
                thisWD->count1;

            sprintf(buffer, " %llu", tailWD->count1);
            MeWrite(fh, buffer, strlen(buffer));

            sprintf(buffer, "" );
            MeWrite(fh, buffer, strlen(buffer));

            cpuCycleAcc += thisfd->timings.cpuCycles;
            count0Acc += thisfd->timings.count0;
            count1Acc += thisfd->timings.count1;
            frames = thisfd->frameNumber;

        }

        /* TODO: keaProfile style analysis of data for each section! */

        /* For the time being, here are some quick averages */

        sprintf(buffer, "#Averages:\n#CPU Cycles: "
            "%llu\n#Count 0: %llu\n#Count 1: %llu\n",
            cpuCycleAcc/frames, count0Acc/frames, count1Acc/frames);

        MeWrite(fh, buffer, strlen(buffer));

        MeClose(fh);
        break;

    default:
        MeWarning(12,"MeProfile: Unkown output style");
    }
}
