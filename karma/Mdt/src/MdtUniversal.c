/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.29.2.1 $

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

#include <string.h>

#include <MePrecision.h>
#include <MeMath.h>
#include "MdtUtils.h"
#include <MdtCheckMacros.h>
#include <MdtConstraint.h>
#include <MdtUniversal.h>

/**
 * Creates a new universal joint with default parameters.
 *
 * @see MdtUniversalReset
 */
MdtUniversalID MEAPI MdtUniversalCreate(const MdtWorldID w)
{
    MdtUniversal *j;
    MdtCHECKWORLD(w,"MdtUniversalCreate");

    j = (MdtUniversal*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtUniversalCreate: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclUNIVERSAL;
    j->head.bclFunction = MdtBclAddUniversal;
    j->head.maxRows = MdtBclGETMAXROWSUNIVERSAL;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtUniversalReset(j);

    return j;
}

/**
 * Initialises a universal joint and sets default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 *
 * @arg body1 = body2 = 0
 * @arg axis 1 = {1,0,0}
 * @arg axis 2 = {0,1,0}
 *
 * @param j universal joint ID
 */
void MEAPI MdtUniversalReset(const MdtUniversalID j)
{
    MdtCHECKUNIVERSAL(j,"MdtUniversalReset");

    if(MdtUniversalIsEnabled(j))
        MdtUniversalDisable(j);

    BaseConstraintReset(MdtUniversalQuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtUniversalQuaConstraint(const MdtUniversalID j)
{
    MdtCHECKUNIVERSAL(j,"MdtUniversalQuaConstraint");
    return (MdtBaseConstraint*)j;
}

/**
 * Gets an MdtUniversalID from an MdtConstraintID.
 * If this constraint is not a Universal, returns 0.
 */
MdtUniversalID MEAPI MdtConstraintDCastUniversal(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastUniversal");
    if(c->head.tag == MdtBclUNIVERSAL)
        return (MdtUniversal *) c;
    else
        return 0;
}

/*
  Accessors
*/



/*
  Mutators
*/

