#error this file should not be in the build
/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.3.4.2 $

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
#ifndef _MCDTESTCORE_H
#define _MCDTESTCORE_H

#include <McdCore.h>
#include <McdVanillaCore.h>

class McdTestCore: public McdVanillaCore
{
private:
    McdCore *refCore; // reference core: test core returns these results
    McdCore *devCore; // development core: results generated, tested against, thrown away.

public:

    McdTestCore(McdCore *referenceCore, McdCore *devCore);

    bool OverlapOBBTri(MeReal &outSep, lsVec3 &outN, MeReal &outPN, lsVec3* &outPos,
                   MeI16 &outDims, const MeReal inEps, const lsVec3 &inR,
                   const CxTriangleNE &inTri);

    void BoxEdgeBoxIntersect(lsVec3* &outList, const lsTransform &tAB,
        const lsVec3 &inRA, const lsVec3 &inRB);

    bool OverlapOBBs(MeReal &outSep, lsVec3 &outN, MeReal &outPN, MeI16 &outDims,
                     const MeReal inEps, const lsVec3 &inR1, const lsVec3 &inR2,
                     const lsTransform &inT12);
};

#endif // _GEOMETRY_H
