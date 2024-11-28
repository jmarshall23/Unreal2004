/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:13 $ - Revision: $Revision: 1.2.2.1 $

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


#ifndef _UTILS_H
#define _UTILS_H


#include "Mst.h"
#include "MeViewer.h"

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif


static float red[4]         = { 0.9f, 0.0f,   0.05f,  1.0f};
static float orange[4]      = { 1.0f, 0.4f,   0.0f,   1.0f};
static float blue[4]        = { 0.0f, 0.598f, 0.797f, 1.0f};
static float green[4]       = { 0.4f, 0.5f,   0.2f,   1.0f};
static float sandColor[4]   = { 1.0f, 0.8f,   0.4f,   1.0f};
static float grassColor[4]  = { 0.0f, 0.8f,   0.0f,   1.0f};
static float tyreColor[4]   = { 0.1f, 0.1f,   0.1f,   1.0f};
static float waterColor[4]  = { 0.3f, 0.3f,   0.8f,   1.0f};
static float white[4]       = { 1.0f, 1.0f, 1.0f, 1.0f };
static float black[4]       = { 0.0f, 0.0f, 0.0f, 1.0f };

void DebugLine(MeVector3 start, MeVector3 end, int r, int g, int b);
void CreateDebugLines(RRender* rc, MeMatrix4Ptr transform);
void ClearDebugLines();

#endif
