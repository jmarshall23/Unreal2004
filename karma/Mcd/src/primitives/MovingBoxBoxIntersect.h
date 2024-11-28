#ifndef _MOVINGBOXBOXINTERSECT_H
#define _MOVINGBOXBOXINTERSECT_H
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:58 $ - Revision: $Revision: 1.8.4.1 $

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

#include "lsVec3.h"
#include "lsTransform.h"
#include <McdModelPair.h>

// Return value is 1 for intersection, zero otherwise.

// boxes are moving, each with constant velocity
unsigned int
MovingBoxBoxIntersect(  const MeReal *ExtBox0, const lsTransform* tm0, const lsVec3& V0,
                        const MeReal *ExtBox1, const lsTransform* tm1, const lsVec3& V1,
                        MeReal dt, MeReal& T, lsVec3& P );

#endif // _MOVINGBOXBOXINTERSECT_H
