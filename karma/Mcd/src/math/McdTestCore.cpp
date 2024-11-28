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

#include <stdio.h>
#include <McdCheck.h>
#include <McdTestCore.h>

McdTestCore::McdTestCore(McdCore *ref, McdCore *dev)
{
    refCore = ref;
    devCore = dev;
}

static void checkContacts(lsVec3 *refbase, lsVec3 *reftop, lsVec3 *devbase, lsVec3 *devtop)
{
    if(reftop-refbase!=devtop-devbase)
    {
        puts("Number of contacts is different!");
        return;
    }

    int i;
    for(i=0;i<reftop-refbase;i++)
    {
        if(!refbase[i].checkAlmostEqual(devbase[i],1e-3f))
        {
            puts("Contact difference!");
            printf("%f  %f  %f\n", refbase[i][0], refbase[i][1], refbase[i][2]);
            printf("%f  %f  %f\n", devbase[i][0], devbase[i][1], devbase[i][2]);
        }
    }
}

bool McdTestCore::OverlapOBBTri(MeReal &outSep, lsVec3 &outN, MeReal &outPN, lsVec3* &outPos,
                   MeI16 &outDims, MeReal inEps, const lsVec3 &inR,
                   const CxTriangleNE &inTri) {

    lsVec3 *refbase = outPos;
    MeReal newOutSep, newOutPN;
    lsVec3 newOutN,newArray[100], *newOutPos = newArray;
    MeI16 newOutDims;
    bool refres, devres;

    refres = refCore->OverlapOBBTri(outSep, outN, outPN, outPos, outDims, inEps, inR, inTri);
    devres = devCore->OverlapOBBTri(newOutSep, newOutN, newOutPN, newOutPos, newOutDims, inEps, inR, inTri);

    if(refres || devres)
    {
        if(refres && !devres || devres && !refres)
        {
            puts("Different intersection results!");
            return refres;
        }
    
        if(fabs(outSep - newOutSep)>1e-6  || fabs (outPN-newOutPN)>1e-6 || outDims != newOutDims
            || (outN-newOutN).square_norm() > 1e-6)
            puts("Different float results");
    }

    checkContacts(refbase,outPos,newArray,newOutPos);
    return refres;
}

void McdTestCore::BoxEdgeBoxIntersect(lsVec3* &outList, const lsTransform &tAB,
        const lsVec3 &inRA, const lsVec3 &inRB)
{
    lsVec3 *refBase = outList;
    lsVec3 newOutBase[100], *newOutList = newOutBase;
    refCore->BoxEdgeBoxIntersect(outList, tAB, inRA, inRB);
    devCore->BoxEdgeBoxIntersect(newOutList, tAB, inRA, inRB);

    checkContacts(refBase,outList, newOutBase, newOutList);
}

bool McdTestCore::OverlapOBBs(MeReal &outSep, lsVec3 &outN, MeReal &outPN, MeI16 &outDims,
                 const MeReal inEps, const lsVec3 &inR1, const lsVec3 &inR2,
                 const lsTransform &inT12)
{
    MeReal newOutSep, newOutPN;
    lsVec3 newOutN;
    MeI16 newOutDims;
    bool refres, devres;

    refres = refCore->OverlapOBBs(outSep, outN, outPN, outDims, inEps, inR1, inR2, inT12);
    devres = devCore->OverlapOBBs(newOutSep, newOutN, newOutPN, newOutDims, inEps, inR1, inR2, inT12);

    if(refres || devres)
    {
        if(refres && !devres || devres && !refres)
        {
            puts("Different intersection results!");
            return refres;
        }
    
        if(fabs(outSep - newOutSep)>1e-6  || fabs (outPN-newOutPN)>1e-6 || outDims != newOutDims
            || (outN-newOutN).square_norm() > 1e-12)
        {
            puts("Different float results:");
            printf("%f, %f, %d, (%f, %f, %f)\n", outSep, outPN, outDims, outN[0], outN[1], outN[2]);
            printf("%f, %f, %d, (%f, %f, %f)\n", newOutSep, newOutPN, newOutDims, newOutN[0], newOutN[1], newOutN[2]);
        }
    }
    return refres;
}