/* -*-c++-*-
 *===============================================================
 * File:        McdTriangleMeshRegisterInteractions.c
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 *
 * $Revision: 1.9.18.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#include <McdTriangleMesh.h>
#include "IxTriangleMeshLineSegment.h"

/*----------------------------------------------------------------
 * McdTriangleMeshRegisterInteractions implementation
 *----------------------------------------------------------------
 */

extern    "C" MeBool MEAPI
McdTriangleMeshRegisterInteractions(McdFrameworkID frame)
{
    McdTriangleMeshTriangleMeshRegisterInteraction(frame);
    McdTriangleMeshLineSegmentRegisterInteraction(frame);
    return true;
}
