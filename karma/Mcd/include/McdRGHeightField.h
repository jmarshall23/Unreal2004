#error this file is deprecated
#ifndef _MCDRGHEIGHTFIELD_H
#define _MCDRGHEIGHTFIELD_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:53 $ - Revision: $Revision: 1.23.2.3 $

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

/**
  @file
  The regular-grid height field geometry type
*/

#include <McdGeometry.h>
#include <McdGeometryTypes.h>
#include <McdInteractionTable.h>

#ifdef __cplusplus
extern "C" {
#endif

  /** McdRGHeightField is a concrete McdGeometry type. */


MCD_DECLARE_GEOMETRY_TYPE( McdRGHeightField );

MEPUBLIC
McdRGHeightFieldID MEAPI McdRGHeightFieldCreate(McdFramework *frame,
                                                MeReal* heightArray,
                                                int xVertexCount, int yVertexCount,
                                                MeReal xIncrement, MeReal yIncrement,
                                                MeReal x0, MeReal y0 );

MEPUBLIC
void MEAPI McdRGHeightFieldGetParameters(McdRGHeightFieldID g,
                                         MeReal** heightArray,
                                         int *xVertexCount, int* yVertexCount,
                                         MeReal *xIncrement, MeReal *yIncrement,
                                         MeReal *x0, MeReal *y0);


  /* individual interactions */
MCD_DECLARE_INTERSECT_INTERACTION(Box,RGHeightField);
MCD_DECLARE_INTERSECT_INTERACTION(Cylinder,RGHeightField);
MCD_DECLARE_INTERSECT_INTERACTION(Sphere,RGHeightField);

MCD_DECLARE_LINESEG_INTERACTION(RGHeightField);

MEPUBLIC
void MEAPI McdRGHeightFieldPrimitivesRegisterInteractions(McdFramework *frame);


#ifdef __cplusplus
}
#endif

#endif /* _MCDRGHEIGHTFIELD_H */
