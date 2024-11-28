/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/15 12:00:10 $ - Revision: $Revision: 1.13.2.4 $

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

/* Null renderer */

#ifdef WITH_NULL

#include "Render_null.h"

void Null_SetWindowTitle(RRender *rc, const char* title)
{

  if (!(title && *title)) {
      MeWarning(0, "Null_SetWindowTitle: You need to pass a valid char* in title.");
      return;
  }

  strncpy(rc->m_AppName, title, sizeof(rc->m_AppName));
  rc->m_AppName[sizeof(rc->m_AppName) -1] = '\0';
}

int Null_CreateRenderer(RRender *rc)
{
    MeProfileTimerMode tmode;
    tmode.counterMode = kMeProfileCounterModeCache;
#ifndef PS2
    tmode.granularity = 0;
#else
    tmode.granularity = 2;
#endif
    tmode.count0Label = 0;
    tmode.count1Label = 0;

    MeProfileStartTiming(tmode,rc->m_bProfiling);
    return 0;
}

void Null_RunApp(RRender *rc, RMainLoopCallBack func, void *userdata)
{
    if( !rc->m_nTimeout )
        ME_REPORT(MeWarning(0, "Running Null renderer without -timeout or -profile."));

    while( !rc->m_bQuitNextFrame )
    {
        MeProfileStartFrame();

        if( func )
            func(rc,userdata);

        MeProfileStartSection("Rendering", 0);

        /* update matrices */
        RRenderUpdateGraphicMatrices(rc);

        MeProfileEndSection("Rendering");
        MeProfileStopTimers();
        MeProfileEndFrame();
    }
    if(rc->m_bProfiling)
        MeProfileOutputResults();
    MeProfileStopTiming();
}

#endif
