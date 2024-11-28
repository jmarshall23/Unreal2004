/* -*-c++-*-
 *===============================================================
 * File:        IxTriangleMeshLineSegment.h
 *
 * Copyright (c) 1997-2002 MathEngine PLC
 *
 * Author: Zhaoheng Liu
 *
 * $Revision: 1.10.2.1 $
 * $Date: 2002/04/04 15:28:56 $
 *
 *================================================================
 */

#include <MePrecision.h>
#include <McdModel.h>

#ifndef _IxTriangleMeshLineSegment_H_
#define _IxTriangleMeshLineSegment_H_

void
          McdTriangleMeshLineSegmentRegisterInteraction(McdFrameworkID frame);

int       MEAPI
IxTriangleMeshLineSegment(const McdModelID inModel,
              MeReal *const inOrig, MeReal *const inDest,
              McdLineSegIntersectResult * resl);

#endif              // _IxTriangleMeshLineSegment_H_
