/* -*- mode: C; -*- */

/*
   Copyright (c) 1997-2002 MathEngine PLC

   $Name: t-stevet-RWSpre-030110 $

   Date: $Date: 2002/04/04 15:29:00 $ - Revision: $Revision: 1.29.6.2 $

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
#include <MdtLimit.h>
#include <MdtConstraint.h>
#include <MdtHinge.h>

/**
 * Creates a new hinge with default parameters.
 *
 * @see MdtHingeReset
 */
MdtHingeID MEAPI MdtHingeCreate(const MdtWorldID w)
{
    MdtHinge *j;
    MdtCHECKWORLD(w,"MdtHingeCreate");

    j = (MdtHinge*)BaseConstraintCreate(w);

    if(j == 0)
    {
#ifdef _MECHECK
        MeWarning(0, "MdtHingeCreate: Constraint Pool Too Small.");
#endif
        return j;
    }

    j->head.tag = MdtBclHINGE;
    j->head.bclFunction = MdtBclAddHinge;
    j->head.maxRows = MdtBclGETMAXROWSHINGE;

    j->head.setBodyFunc = BaseConstraintSetBodies;
    j->head.setAxisFunc = BaseConstraintSetAxis;

    MdtHingeReset(j);

    return j;
}

/**
 * Sets a hinge to its default values.
 * Note that the joint's bodies are not reset.
 * This will disable the constraint.
 *
 * Default values:
 *
 * @arg position = {0,0,0}
 * @arg axis = {1,0,0}
 *
 * @param j hinge joint ID
 */
void MEAPI MdtHingeReset(const MdtHingeID j)
{
    MdtCHECKHINGE(j,"MdtHingeReset");

    if(MdtHingeIsEnabled(j))
        MdtHingeDisable(j);

    /* initialize the joint limit */
    MdtLimitReset( &j->limit );

    BaseConstraintReset(MdtHingeQuaConstraint(j));
}

/**
 * Gets an MdtConstraintID from a specific constraint for
 * performing functions common to all constraints.
 */
MdtConstraintID MEAPI MdtHingeQuaConstraint(const MdtHingeID j)
{
    MdtCHECKHINGE(j,"MdtHingeQuaConstraint");
    return (MdtBaseConstraint *) j;
}

/**
 * Gets an MdtHingeID from an MdtConstraintID.
 * If this constraint is not a Hinge, returns 0.
 */
MdtHingeID MEAPI MdtConstraintDCastHinge(const MdtConstraintID c)
{
    MdtCHECKCONSTRAINT(c, "MdtConstraintDCastHinge");
    if(c->head.tag == MdtBclHINGE)
        return (MdtHinge *) c;
    else
        return 0;
}

/*
  Accessors
*/

/**
 * Provides read/write access to the constraint limit.
 */
MdtLimitID MEAPI MdtHingeGetLimit(const MdtHingeID j)
{
    MdtCHECKHINGE(j,"MdtHingeGetLimit");
    return &(j->limit);
}

/*
  Mutators
*/


/**
 * Resets the joint limit and then copies the public attributes of
 * NewLimit.
 */
void MEAPI MdtHingeSetLimit(const MdtHingeID j, const MdtLimitID NewLimit)
{
    MdtLimitID Limit;
    MdtCHECKHINGE(j,"MdtHingeSetLimit");
    MdtCHECKCONSTRAINTBODIES_NULL_OK(j,"MdtHingeSetLimit");

#ifdef _MECHECK
    if (NewLimit == 0)
        MeFatalError(0, "Invalid NewLimit passed to MdtHingeSetLimit()");
#endif

    Limit = MdtHingeGetLimit(j);

    MdtLimitReset(Limit);
    MdtLimitSetLowerLimit(Limit, MdtLimitGetLowerLimit(NewLimit));
    MdtLimitSetUpperLimit(Limit, MdtLimitGetUpperLimit(NewLimit));
    MdtLimitActivateLimits(Limit, MdtLimitIsActive(NewLimit));
    MdtLimitSetLimitedForceMotor(Limit,
        MdtLimitGetMotorDesiredVelocity(NewLimit),
        MdtLimitGetMotorMaxForce(NewLimit));
    MdtLimitActivateMotor(Limit, MdtLimitIsMotorized(NewLimit));
}

