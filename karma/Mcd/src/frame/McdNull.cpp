/* -*- mode: C++; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:28:55 $ - Revision: $Revision: 1.7.2.3 $

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

#include <McdCheck.h>
#include <McdGeometry.h>
#include <McdGeometryTypes.h>
#include <McdInteractionTable.h>
#include <McdContact.h>
#include <McdNull.h>
#include <MeMessage.h>

MCD_IMPLEMENT_GEOMETRY_TYPE( McdNull, "Null" , Null);


/**
  Create a null geometry
*/
McdNullID MEAPI
McdNullCreate(McdFramework *frame)
{
    McdNull *n;

    n = (McdNull *)MeMemoryAPI.createAligned(sizeof(McdNull), 16);
    if (!n) return 0;
    McdGeometryInit((McdGeometryID)n, frame, kMcdGeometryTypeNull);
    return (McdNullID)n;
}



/*--- McdNull polymorphic functions ---*/

void MEAPI
McdNullDestroy( McdGeometry* g)
{
    
    MCD_CHECKGEOMETRY(g, "McdNullDestroy");
    McdGeometryDeinit(g);
}


void MEAPI
McdNullUpdateAABB(McdGeometryInstanceID ins, MeMatrix4Ptr finalTM, MeBool tight)
{
    MeFatalError(0,"Attempt to calculate the bounding box of a null geometry"
        " (perhaps you inserted it into an McdSpace?)");
}
        
// just put a bounding sphere around the bounding box...

void MEAPI
McdNullGetBSphere(McdGeometryID g, MeVector3 center, MeReal *radius )
{
    MeFatalError(0,"Attempt to calculate the bounding sphere of a null geometry");
}


void MEAPI
McdNullMaximumPoint(McdGeometryInstanceID ins,
                   MeReal * const inDir, MeReal * const outPoint)
{
    MeFatalError(0,"Attempt to calculate a maximum point on a null geometry");
}


/**
    Not yet implemented.
*/
MeI16 MEAPI
McdNullGetMassProperties(McdGeometry *g,
                        MeMatrix4 relTM,
                        MeMatrix3 m,
                        MeReal *volume)
{
    MeFatalError(0,"Attempt to calculate mass properties of a null geometry");
    return 0;
}



void MEAPI McdNullDebugDraw(const McdGeometryID geom, const MeMatrix4 tm, const MeReal colour[3])
{
    static int warn = 1;
    if(warn)
        MeWarning(0, "McdNullDebugDraw: Don't know how to draw a Null!");
    warn = 0;
}

