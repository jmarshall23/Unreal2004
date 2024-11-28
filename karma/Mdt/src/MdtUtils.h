#ifndef _MDTUTILS_H
#define _MDTUTILS_H
/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.18.6.1 $

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

#include <MeInline.h>
#include <MeAssert.h>
#include <MdtTypes.h>


int MdtDictCompare(const void *, const void *);


void                MEAPI UpdateCOMTransform(MdtBodyID b);
void                MEAPI UpdateBodyTransform(MdtBodyID b, const MeMatrix4 keaTM);
void                MEAPI GetCOMTransform(MdtBodyID b, MeMatrix4 keaTM);

void                MEAPI ConvertCOMPositionVector(const MdtBodyID from_body,
                        const MeVector3 f, const MdtBodyID to_body, MeVector3 t);
void                MEAPI ConvertCOMVector(const MdtBodyID from_body, const MeVector3 f,
                        const MdtBodyID to_body, MeVector3 t);

/* 'Private' base-class constraint functions. */
void                MEAPI BaseConstraintReset(const MdtConstraintID c);
MdtConstraintID     MEAPI BaseConstraintCreate(const MdtWorldID w);
void                MEAPI BaseConstraintSetBodies(const MdtConstraintID c,
                             const MdtBodyID b1, const MdtBodyID b2);
void                MEAPI BaseConstraintSetAxis(const MdtConstraintID c,
                             const MeReal px, const MeReal py, const MeReal pz);

#endif
