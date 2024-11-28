/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:42 $ - Revision: $Revision: 1.3.2.1 $

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

#ifdef _MSC_VER
#pragma warning( disable : 4305 4244 )
#endif

#include "utils.h"

#include "RGeometryUtils.h"

/*
   Debug line drawing
   (Very useful!)
*/
#define MAXDEBUGLINES (100)
int numDebugLines = 0;
RGraphic *debugLine[MAXDEBUGLINES];
MeVector3 zero = {0, 0, 0};

void DebugLine(MeVector3 start, MeVector3 end, int r, int g, int b)
{
    float color[4];

    color[0] = (float)r/255;
    color[1] = (float)g/255;
    color[2] = (float)b/255;
    color[3] = 1;

    if(numDebugLines >= MAXDEBUGLINES)
        return;

    RGraphicSetColor(debugLine[numDebugLines], color);
    RGraphicLineMoveEnds(debugLine[numDebugLines], start, end);

    numDebugLines++;
}

void CreateDebugLines(RRender* rc, MeMatrix4Ptr transform)
{
    int i;

    for(i=0; i<MAXDEBUGLINES; i++)
    {
        debugLine[i] = RGraphicLineCreate(rc, zero, zero, red, transform);
        debugLine[i]->m_pObject->m_bIsWireFrame = 1;
    }

    numDebugLines = 0;
}

void ClearDebugLines()
{
    int i;

    for(i=numDebugLines-1; i>=0; i--)
    {
        RGraphicLineMoveEnds(debugLine[i], zero, zero);
    }

    numDebugLines = 0;
}
/*****************************************************************/

